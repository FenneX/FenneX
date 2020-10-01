package com.fennex.modules;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.hardware.Camera;
import android.hardware.Camera.Size;
import android.media.CamcorderProfile;
import android.media.MediaMetadataRetriever;
import android.media.MediaRecorder;
import android.os.Environment;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.Toast;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.util.Date;
import java.util.List;

@SuppressLint("Registered")
public class CameraHandler extends Activity implements SurfaceHolder.Callback, MediaRecorder.OnInfoListener {
	public static String TAG = "VideoRecorder";
	@SuppressLint("StaticFieldLeak")
	private static volatile CameraHandler instance = null;
	private CameraHandler() { }

	public static CameraHandler getInstance() {
		if (instance == null) {
			synchronized (ImagePicker .class) {
				if (instance == null) {
					instance = new CameraHandler();
				}
			}
		}
		return instance;
	}

	private static Camera camera;
	private static int cameraID = 0;
	@SuppressLint("StaticFieldLeak")
	private static SurfaceView cameraView;
	private static MediaRecorder recorder;
	private static boolean recorderStopped; //used because the maximum duration/filesize stop the recorder
	private static String videoPath;
	private static boolean previewRunning = false;
	private static boolean recording = false;

	//Convenience variables
	public static int widthScreen = NativeUtility.getMainActivity().getMainLayout().getWidth();
	public static int heightScreen = NativeUtility.getMainActivity().getMainLayout().getHeight();

	//Default values : centered and full screen
	private static float centerX = widthScreen/2;
	private static float centerY = heightScreen/2;
	private static float localWidth = widthScreen;
	private static float localHeight = heightScreen;

	public native static void notifyRecordingCancelled();
	protected native static void notifyPictureTaken(String fullpath);

	//The init goal is to create the cameraView (which will be valid during any preview/recording session)
	@SuppressWarnings("deprecation")
	private static void init(boolean front) {
		if(cameraView == null) {
			NativeUtility.getMainActivity().runOnUiThread(() -> {
				FrameLayout mainFrame = NativeUtility.getMainActivity().getMainLayout();

				cameraView = new SurfaceView(NativeUtility.getMainActivity());
				SurfaceHolder holder = cameraView.getHolder();
				holder.addCallback(getInstance());
				holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

				cameraView.setClickable(false);

				cameraView.setZOrderMediaOverlay(front);
				cameraView.invalidate();
				mainFrame.addView(cameraView);
			});
		}
	}

	//When a preview/recording is finished, release the cameraView
	private static void releaseView() {
		if(cameraView != null) {
			NativeUtility.getMainActivity().runOnUiThread(() -> {
				FrameLayout mainFrame = NativeUtility.getMainActivity().getMainLayout();
				mainFrame.removeView(cameraView);
				cameraView = null;
			});
		}
	}

	//All record method start by checking if there is a camera, and exiting if there is none
	//Also check that the current mode is not already running

	@SuppressWarnings("unused")
	public static void startCameraPreview(float x, float y, float width, float height, boolean front) {
		if(!ImagePicker.isCameraAvailable() || previewRunning) {
			cameraView.setZOrderMediaOverlay(front);
			cameraView.invalidate();
			return;
		}
		//Assign local before the init because init() will use them
		previewRunning = true;
		centerX = x;
		centerY = y;
		localWidth = width;
		localHeight = height;
		init(front);
	}

	public static void stopCameraPreview() {
		if(!ImagePicker.isCameraAvailable() || !previewRunning) {
			return;
		}
		releaseView();
		previewRunning = false;
	}

	@SuppressWarnings("unused")
	public static boolean canSwitchCamera() {
		return Camera.getNumberOfCameras() > 1;
	}

	@SuppressWarnings("unused")
	public static void startRecording() {
		if(!ImagePicker.isCameraAvailable() || recording) {
			return;
		}
		//Do the init in case the recording is started directly, without preview. Front = false is the legacy behavior
		init(false);
		//If there is a preview, re-use the camera
		if(previewRunning && camera != null) {
			camera.unlock();
		}
		getInstance().prepareRecorder();
		recording = true;
		previewRunning = false;
		recorder.start();
	}

	public static void stopRecording() {
		if(!ImagePicker.isCameraAvailable() || !recording) {
			return;
		}
		getInstance().stopRecorder();

		String externalPath;
		//On some devices, copying the video in the Movies directory doesn't work. If it fails, show a message to the user
		try {
			File videoFolder = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_MOVIES).getAbsoluteFile();
			externalPath = videoFolder.getAbsolutePath() + "/" + videoPath.substring(videoPath.lastIndexOf('/')+1);

			//Copy the file using streams and channels
			FileInputStream inStream = new FileInputStream(videoPath);
			FileOutputStream outStream = new FileOutputStream(externalPath);
			FileChannel inChannel = inStream.getChannel();
			FileChannel outChannel = outStream.getChannel();
			inChannel.transferTo(0, inChannel.size(), outChannel);
			inStream.close();
			outStream.close();

			//Once the file is copied, delete the local file
			File localFile = new File(videoPath);
			boolean result = localFile.delete();
			Log.i(TAG, "Local file at path " + videoPath + (result ? " deleted" : " not deleted"));
		} catch(Exception e) {
			Log.e(TAG, e.getMessage() != null ? e.getMessage() : "exception message is null");
			e.printStackTrace();
			NativeUtility.getMainActivity().runOnUiThread(() -> NativeUtility.showToast("CantCopyVideo", Toast.LENGTH_LONG));
			externalPath = null;
		}

		final String path = externalPath != null ? externalPath : videoPath;

		NativeUtility.getMainActivity().runOnGLThread(() ->
				VideoPicker.notifyVideoPickedWrap(path, FileUtility.FileLocation.Absolute.getValue()));
		//Execute this part in another thread since MediaMetadataRetriever can take some time
		Thread thread = new Thread() {
			@Override
			public void run() {
				MediaMetadataRetriever retriever = new MediaMetadataRetriever();
				retriever.setDataSource(path);
				String title = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_TITLE);

				NativeUtility.getMainActivity().runOnGLThread(() ->
						VideoPicker.notifyVideoName(
								path,
								title != null ? title : path.substring(path.lastIndexOf('/') + 1, path.lastIndexOf('.'))));
			}
		};

		thread.start();
	}

	@SuppressWarnings("unused")
	public static boolean cancelRecording(boolean notify) {
		boolean stopped = false;
		if(recording) {
			getInstance().stopRecorder();

			//delete local file, since the video won't be used
			File localFile = new File(videoPath);
			boolean result = localFile.delete();
			Log.i(TAG, "Cancelling recording, Local file at path " + videoPath + (result ? " deleted" : " not deleted"));
			stopped = true;
		}
		else if(previewRunning) {
			stopCameraPreview();
			Log.i(TAG, "Record preview stopped by a cancel");
			stopped = true;
		}
		if(stopped && notify) {
			NativeUtility.getMainActivity().runOnGLThread(() -> {
				//A little sleep otherwise there are some problems with other views which intercepts touches
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}

				notifyRecordingCancelled();
			});
		}
		return stopped;
	}

	@SuppressWarnings("unused")
	public static void capturePicture() {
		try {
			camera.setPreviewDisplay(cameraView.getHolder());
		} catch (IOException e) {
			e.printStackTrace();
		}
		camera.startPreview();
		camera.takePicture(() -> Log.d(TAG, "TRACKING -- shutter called"),
				null, //Raw data is not properly supported by Android. Never has been, never will be. https://www.mail-archive.com/android-developers@googlegroups.com/msg23515.html
				(jpegData, camera) -> {
					if (jpegData == null) {
						// No jpeg data => the recording can be considered a failure
						notifyRecordingCancelled();
						return;
					}
					File file;
					try {
						//Save data directly to a temporary jpeg file
						file = File.createTempFile("PickedImage", ".jpg");

						if(cameraID == Camera.CameraInfo.CAMERA_FACING_FRONT) {
							//Front-facing camera requires flipping the image horizontally
							//as the preview show the image in the expected rotation, but the capture takes it horizontally flipped
							ByteArrayInputStream jpegInput = new ByteArrayInputStream(jpegData);
							Bitmap original = BitmapFactory.decodeStream(jpegInput);
							jpegInput.close();

							Matrix matrix = new Matrix();
							matrix.setScale(-1, 1);
							Bitmap flipped = Bitmap.createBitmap(original, 0, 0, original.getWidth(), original.getHeight(), matrix, true);

							FileOutputStream fileOutputStream = new FileOutputStream(file);
							flipped.compress(Bitmap.CompressFormat.JPEG, 100, fileOutputStream);
							fileOutputStream.close();
							flipped.recycle();
						}
						else { //For back-facing camera, we can just save jpeg data as-is
							FileOutputStream fileOutputStream = new FileOutputStream(file);
							fileOutputStream.write(jpegData);
							fileOutputStream.close();
						}

						final String filePath = file.getAbsolutePath(); //Make lambda happy
						//Notify that the picture is taken before finishing inserting the picture to Camera Roll, to make UI more responsive
						NativeUtility.getMainActivity().runOnGLThread(() -> notifyPictureTaken(filePath));
					} catch (IOException e) {
						e.printStackTrace();
						// Could not save jpeg   => the recording can be considered a failure
						notifyRecordingCancelled();
						file = null;
					}
					//If file was properly saved, insert it into gallery
					if(file != null) {
						try {
							CapturePhotoUtils.insertImage(file.getAbsolutePath(), null, null);
						} catch (FileNotFoundException e) {
							e.printStackTrace();
						}
					}
				});
	}

	@SuppressWarnings("unused")
	public static void switchCamera() {
		if(canSwitchCamera() && previewRunning) {
			NativeUtility.getMainActivity().runOnUiThread(() -> {
				//Loop through cameras. There will generally be only 2 cameras : Back (0) and Front (1), but we are prepared to use more
				//if there is an external camera.
				cameraID++;
				if (cameraID >= Camera.getNumberOfCameras()) cameraID = 0;
				camera.stopPreview();
				camera.release();
				camera = Camera.open(cameraID);
				updatePreviewSize();
				try {
					camera.setPreviewDisplay(cameraView.getHolder());
				} catch (IOException e) {
					e.printStackTrace();
				}
				camera.startPreview();
			});
		}
	}

	private void prepareRecorder() {
		recorder = new MediaRecorder();
		if(camera != null) {
			recorder.setCamera(camera);
		}
		recorder.setOnInfoListener(this);
		recorder.setAudioSource(MediaRecorder.AudioSource.DEFAULT);
		recorder.setVideoSource(MediaRecorder.VideoSource.CAMERA);
		recorder.setProfile(CamcorderProfile.get(CamcorderProfile.QUALITY_HIGH));

		String currentDate = DateFormat.format("dd:MM:yyyy-kk:mm:ss", new Date().getTime()).toString();
		//There are some problems with ':" in file path, use "_" instead
		currentDate = currentDate.replace(':', '-');

		videoPath = NativeUtility.getMainActivity().getFilesDir().getPath() + "/" + NativeUtility.getAppName() + " " + currentDate + ".mp4";
		Log.i(TAG, "Saving video at path : " + videoPath);
		recorder.setOutputFile(videoPath);
		recorder.setMaxDuration(3600000); // 1 hour
		recorder.setMaxFileSize(1000000000); // Approximately 1000 megabytes
		recorder.setPreviewDisplay(cameraView.getHolder().getSurface());
		recorderStopped = false;

		try {
			recorder.prepare();
		} catch (Exception e) {
			e.printStackTrace();
			finish();
		}
	}

	private void stopRecorder() {
		if(!recorderStopped) {
			try{
				recorder.stop();
			} catch(RuntimeException e){
				Log.i(TAG, "recorder.stop() failed, not a real issue since the release take care of everything after");
			}
		}
		recorder.release();
		recorder = null;
		recording = false;
		releaseView();
		recorderStopped = true;
	}

	public void onInfo(MediaRecorder mr, int what, int extra) {
		if(what == MediaRecorder.MEDIA_RECORDER_ERROR_UNKNOWN
				|| what == MediaRecorder.MEDIA_RECORDER_INFO_MAX_DURATION_REACHED
				|| what == MediaRecorder.MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED) {
			final String toastText = what == MediaRecorder.MEDIA_RECORDER_INFO_MAX_DURATION_REACHED ?
					"Maximum recording duration reached" :
					what == MediaRecorder.MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED ? "Maximum file sized reached" :
							"An error occured during video recording";
			Log.v(TAG, toastText);

			recorderStopped = true;
			stopRecording();
			NativeUtility.getMainActivity().runOnUiThread(() -> {
				if(what == MediaRecorder.MEDIA_RECORDER_INFO_MAX_DURATION_REACHED) {
					NativeUtility.showToast("VideoRecordingMaxDuration", Toast.LENGTH_LONG);
				}
				else if(what == MediaRecorder.MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED) {
					NativeUtility.showToast("VideoRecordingMaxFilesize", Toast.LENGTH_LONG);
				}
				else {
					NativeUtility.showToast("VideoRecordingError", Toast.LENGTH_LONG);
				}
			});
		}
	}

	public void surfaceCreated(SurfaceHolder holder) {
		if(previewRunning) {
			//If the app hang here, there was a problem with the Camera. Restart the device, it's an Android OS bug
			camera = Camera.open(cameraID);
			if (camera != null) {
				Camera.Parameters params = camera.getParameters();
				camera.setParameters(params);
			}
			else { //if the camera fail to open, it means another app is using it
				stopCameraPreview();
				NativeUtility.getMainActivity().runOnUiThread(() -> NativeUtility.showToast("CantUseCamera", Toast.LENGTH_LONG));
			}
		}
	}

	private static Camera.Size getPreviewSize(Camera camera) {
		Camera.Parameters p = camera.getParameters();

		//Find the best preview size in the supported ones
		Size previewSize = null;
		List<Size> list = p.getSupportedPreviewSizes();
		for(int i = 0; i < list.size(); i++) {
			if(list.get(i).height <= localHeight
					&& list.get(i).width <= localWidth) {
				if(previewSize == null
						|| (list.get(i).height > previewSize.height
						&& list.get(i).width > previewSize.width)) {
					previewSize = list.get(i);
				}
			}
		}
		return previewSize;
	}

	private static void updatePreviewSize() {
		Camera.Parameters p = camera.getParameters();
		Size previewSize = getPreviewSize(camera);

		Log.i(TAG, "Setting preview size : " + previewSize.width + ", " + previewSize.height);
		p.setPreviewSize(previewSize.width, previewSize.height);
		if(p.getSupportedPreviewFormats().contains(ImageFormat.JPEG)) { //Some devices don't support it, they will have the default preview format
			p.setPreviewFormat(ImageFormat.JPEG);
		}
		else if(p.getSupportedPreviewFormats().contains(ImageFormat.NV21)) { //This should be the default format. Set it anyway ....
			p.setPreviewFormat(ImageFormat.NV21);
		}
		camera.setParameters(p);

		//Use the same previewSize for the layout, and center it around centerX/Y
		FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(previewSize.width, previewSize.height);
		lp.leftMargin = (int)(widthScreen - centerX - (previewSize.width / 2));
		lp.topMargin = (int)(heightScreen - centerY - (previewSize.height / 2));
		cameraView.setLayoutParams(lp);

	}

	public void surfaceChanged(SurfaceHolder holder, int format, int width,
							   int height) {
		if (previewRunning) {
			camera.stopPreview();
			updatePreviewSize();
			try {
				camera.setPreviewDisplay(holder);
				camera.startPreview();
				previewRunning = true;
			} catch (IOException e) {
				Log.e(TAG, e.getMessage() != null ? e.getMessage() : "exception message is null");
				e.printStackTrace();
			}
		}
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		if(camera != null) {
			camera.stopPreview();
			camera.release();
			camera = null;
		}
		previewRunning = false;
		recording = false;
	}
}
