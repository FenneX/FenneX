package com.fennex.modules;

import android.annotation.SuppressLint;
import android.content.ContentResolver;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.RectF;
import android.media.MediaMetadataRetriever;
import android.media.MediaPlayer;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.Environment;
import android.os.StrictMode;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.VideoView;

import org.videolan.libvlc.IVLCVout;
import org.videolan.libvlc.LibVLC;
import org.videolan.libvlc.Media;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Objects;

public class VideoPlayer implements IVLCVout.Callback, Runnable {
    /*
     * VideoPlayer implements two ways of displaying a video :
     * - using a native Android VideoView (default)
     * - using LibVLC : it allows to set playback rate (not supported by VideoView) but requires a long build process and specific c files not included in the Stack
     *
     * both ways use the view hierarchy as follow :
     * attach itself to NativeUtility.getMainActivity().getMainLayout(), assuming it's full screen
     * - baseView : a FrameLayout added just for the video
     *   - videoView : the SurfaceView on which the video is played. It's either a VideoView or a SurfaceView (for LibVLC)
     *
     * Note: to center the video, the position and size of baseView is modified
     * and videoView use match parent size with gravity: center
     */
    public static String TAG = "VideoPlayer";
    private static Integer baseId = null;
    private static Integer videoViewId = null;
    public static String path;

    //The position when embedded. Allow to easily switch between full screen and embedded
    private static RectF embeddedPos = null;
    private static boolean toFront;
    private static boolean isFullScreen = false;
    //Used by VLC implementation only to keep the video size for setSurfaceSize (avoid flickering)
    private static int currentVideoWidth;
    private static int currentVideoHeight;

    private static boolean useVLC = false;

    //TODO make this instance redundant by separating callbacks from VideoPlayer
    @SuppressLint("StaticFieldLeak")
    private static VideoPlayer instance = null;

    private static boolean isPrepared = false;
    private static boolean shouldLoop;
    private static boolean hideOnPause;
    private static boolean videoEnded; //Video ended is there because restart is different than play for LibVLC
    private static float lastPlaybackRate; //Playback rate must be kept between sessions (when restarting video)
    private static boolean muted = false;
    private static org.videolan.libvlc.MediaPlayer vlcMediaPlayer;
    private static org.videolan.libvlc.LibVLC libVLC;

    private static org.videolan.libvlc.MediaPlayer.EventListener mPlayerListener = new MyPlayerListener(getInstance());

    private static MediaPlayer videoViewMediaPlayer = null;

    @SuppressWarnings("unused")
    public static void setUseVLC(boolean use) {
        if(videoViewId != null) {
            Log.e(TAG, "Can't change VideoPlayer VLC/VideoView mode after starting it");
            return;
        }
        if(use && !useVLC) { //If the app crash here, check that LibVLC is properly compiled using compile.sh
            try {
                Log.i(TAG, "LibVLC loaded");
            }
            catch(Exception e) {
                Log.e(TAG, "Exception while loading vlcjni, it's probably not compiled for the current architecture. Falling back to VideoView display");
                e.printStackTrace();
                useVLC = false;
            }
        }
        useVLC = use;
    }

    private static VideoPlayer getInstance() {
        if(instance == null) {
            instance = new VideoPlayer();
        }
        return instance;
    }

    //Is used by VideoPicker when doing getAllVideos
    public native static void notifyVideoDurationAvailable(String path, float duration);
    public native static void notifyVideoEnded(String path);
    public native static void notifyVideoError(String path);

    private static void adjustBaseView() {
        FrameLayout mainFrame = NativeUtility.getMainActivity().getMainLayout();
        if(mainFrame == null || baseId == null) return;
        FrameLayout base = mainFrame.findViewById(baseId);
        if(base == null) return;

        //Size is API 21, since we still support API 19, abuse Point
        Point screen = new Point(mainFrame.getWidth(), mainFrame.getHeight());
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                VideoPlayer.isFullScreen ? screen.x : (int) embeddedPos.width(),
                VideoPlayer.isFullScreen ? screen.y : (int) embeddedPos.height(),
                Gravity.TOP | Gravity.START); //Position is according to bottom left of the screen (TOP for Android because it's y-inverted)
        lp.leftMargin = VideoPlayer.isFullScreen ? 0 : (int)(embeddedPos.left - ( embeddedPos.width() / 2) + 0.5);
        lp.topMargin = VideoPlayer.isFullScreen ? 0 : (int)(screen.y - embeddedPos.top - (embeddedPos.height() / 2) + 0.5);
        base.setLayoutParams(lp);
        base.invalidate();
    }

    @SuppressWarnings("unused")
    public static void initVideoPlayer(String file, float x, float y, float height, float width, boolean front, boolean loop) {
        lastPlaybackRate = 1.0f;
        path = file;
        toFront = front;
        //Warning: Rect assumes left <= right and top <= bottom (inverted y)
        embeddedPos = new RectF(x, y, x + width, y + height);
        isPrepared = false;
        shouldLoop = loop;
        hideOnPause = false;
        videoViewMediaPlayer = null;

        final File videoFile = getFile(path, FileUtility.FileLocation.Unknown);
        if(videoFile == null || (!videoFile.exists() && NativeUtility.getMainActivity().getUriFromFileName(path) == null)) {
            return;
        }
        Log.i(TAG, "initVideoPlayer: path = " + path + ", x = " +
                x + ", y = " + y + " height = " + height + ", width = " + width + " front = " + toFront);

        NativeUtility.getMainActivity().runOnUiThread(() -> {
            videoEnded = false;

            //Prepare base view
            FrameLayout mainFrame = NativeUtility.getMainActivity().getMainLayout();
            if(mainFrame == null) return;
            FrameLayout base = new FrameLayout(NativeUtility.getMainActivity());
            baseId = View.generateViewId();
            base.setId(baseId);
            mainFrame.addView(base);
            adjustBaseView();

            SurfaceView videoView;
            if(VideoPlayer.useVLC) {
                videoView = new SurfaceView(NativeUtility.getMainActivity());
                SurfaceHolder mSurfaceHolder = videoView.getHolder();
                mSurfaceHolder.setFormat(PixelFormat.RGBX_8888);
                getInstance().releasePlayer();
                try {
                    // Create LibVLC
                    // TODO: make this more robust, and sync with audio demo
                    ArrayList<String> options = new ArrayList<>();
                    //noinspection SpellCheckingInspection
                    options.add("--aout=opensles");
                    options.add("--audio-time-stretch"); // time stretching
                    libVLC = new LibVLC(NativeUtility.getMainActivity(), options);
                    mSurfaceHolder.setKeepScreenOn(true);

                    // Create media player
                    vlcMediaPlayer = new org.videolan.libvlc.MediaPlayer(libVLC);
                    vlcMediaPlayer.setEventListener(mPlayerListener);
                    Media m = new Media(libVLC, path);
                    vlcMediaPlayer.setMedia(m);
                    // Set up video output
                    final IVLCVout vout = vlcMediaPlayer.getVLCVout();
                    vout.setVideoView(videoView);
                    vout.addCallback(getInstance());
                    vout.attachViews();
                } catch (Exception e) {
                    e.printStackTrace();
                }
                isPrepared = true;
            }
            else {

                Uri uri;
                if(videoFile.exists()) {
                    // TODO : Change to use a FileProvider
                    StrictMode.VmPolicy.Builder builder = new StrictMode.VmPolicy.Builder();
                    StrictMode.setVmPolicy(builder.build());
                    uri = Uri.fromFile(videoFile);
                }
                else {
                    Log.i(TAG, "File " + videoFile.getAbsolutePath() + " doesn't exist, trying to find URI from main activity");
                    uri = NativeUtility.getMainActivity().getUriFromFileName(path);
                }

                if(uri == null) {
                    Log.e(TAG, "URI is null for file: " + videoFile.getAbsolutePath() + ", cannot start Video player");
                    mainFrame.removeView(base);
                    baseId = null;
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyVideoError(path));
                    return;
                }

                VideoView video = new VideoView(NativeUtility.getMainActivity());
                video.setOnPreparedListener(mp -> {
                    Log.i(TAG, "Video is prepared, playing it ...");
                    videoViewMediaPlayer = mp;
                    isPrepared = true;
                    play();
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyVideoDurationAvailable(path, getDuration()));
                });
                video.setOnCompletionListener(mp -> {
                    if(shouldLoop) {
                        Log.i(TAG, "Video ended, replaying it ...");
                        play();
                    }
                    else {
                        videoEnded = true;
                    }
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyVideoEnded(path));
                });
                video.setOnErrorListener((mp, what, extra) -> {
                    Log.e(TAG, "VideoView error : " + what + ", " + extra);
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyVideoError(path));
                    return false;
                });
                Log.i(TAG, "Using URI : " + uri.toString() + ", path : " + uri.getPath());
                video.setVideoURI(uri);
                videoView = video;
            }

            videoView.setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT, Gravity.CENTER));
            videoViewId = View.generateViewId();
            videoView.setId(videoViewId);
            videoView.setZOrderMediaOverlay(VideoPlayer.toFront);
            base.addView(videoView);

            if(toFront)
                base.bringToFront();
            base.setClickable(false);
            videoView.setClickable(false);
            videoView.invalidate();
            mainFrame.invalidate();
        });
    }

    private static SurfaceView getVideoView() {
        if(baseId == null || videoViewId == null) return null;
        try
        {
            return NativeUtility.getMainActivity().getMainLayout().findViewById(baseId).findViewById(videoViewId);
        }
        catch (NullPointerException e)
        {
            return null;
        }
    }

    @SuppressWarnings("unused")
    public static void setPlayerPosition(float x, float y, float height, float width, boolean animated) {
        //Warning: Rect assumes left <= right and top <= bottom (inverted y)
        embeddedPos = new RectF(x, y, x + width, y + height);
        NativeUtility.getMainActivity().runOnUiThread(VideoPlayer::adjustBaseView);
    }

    public static void play() {
        Log.i(TAG, "Play.");
        if(useVLC) {
            if(vlcMediaPlayer != null) {
                if(videoEnded) {
                    Log.i(TAG, "videoEnded, restarting");
                    vlcMediaPlayer.stop();
                }
                vlcMediaPlayer.play();
                videoEnded = false;
            }
            else {
                Log.e(TAG, "stop vlcMediaPlayer is Null");
            }
        }
        else if(isPrepared) {
            SurfaceView videoView = getVideoView();
            if(videoView != null) {
                if (videoEnded) {
                    ((VideoView) videoView).seekTo(0);
                }
                ((VideoView) videoView).start();
            }
        }
        if(hideOnPause) {
            NativeUtility.getMainActivity().runOnUiThread(() -> {
                SurfaceView videoView = getVideoView();
                if(videoView != null) {
                    videoView.setVisibility(View.VISIBLE);
                    videoView.invalidate();
                }
            });
        }
        videoEnded = false;
        setMuted(muted);
    }


    public static void pause() {
        Log.i(TAG, "Pause.");
        SurfaceView videoView = getVideoView();
        if(useVLC) {
            if(vlcMediaPlayer != null && vlcMediaPlayer.isPlaying()) {
                vlcMediaPlayer.pause();
            }
            else {
                Log.e(TAG, "pause vlcMediaPlayer is Null or not playing");
            }
        }
        else if(isPrepared) {
            assert videoView != null;
            ((VideoView)videoView).pause();
        }
        if(hideOnPause) {
            NativeUtility.getMainActivity().runOnUiThread(() -> {
                if(videoView != null) {
                    videoView.setVisibility(View.GONE);
                    videoView.invalidate();
                }
            });
        }
    }

    public static void stop() {
        Log.i(TAG, "Stop.");
        NativeUtility.getMainActivity().runOnUiThread(() -> {
            SurfaceView videoView = getVideoView();
            if (videoView != null) {
                if (useVLC) {
                    if(vlcMediaPlayer != null) {
                        vlcMediaPlayer.pause();
                    }
                    else {
                        Log.e(TAG, "stop vlcMediaPlayer is Null");
                    }
                } else {
                    ((VideoView) videoView).stopPlayback();
                }
                FrameLayout mainFrame = NativeUtility.getMainActivity().getMainLayout();
                if(mainFrame == null) return;
                FrameLayout base = mainFrame.findViewById(baseId);
                if(base == null) return;
                base.removeAllViews();
                mainFrame.removeView(base);
                baseId = null;
                videoViewId = null;
                isPrepared = false;
            }
        });
    }

    @SuppressWarnings("unused")
    public static float getPlaybackRate() {
        if(useVLC) {
            if(videoEnded) {
                return lastPlaybackRate;
            }
            if(vlcMediaPlayer != null) {
                return vlcMediaPlayer.getRate();
            }
            else {
                Log.e(TAG, "getPlaybackRate vlcMediaPlayer is Null");
            }
            return lastPlaybackRate;
        }
        Log.e(TAG, "getPlaybackRate is only implemented for LibVLC");
        return 1;
    }

    @SuppressWarnings("unused")
    public static void setPlaybackRate(float rate) {
        if(useVLC) {
            if(vlcMediaPlayer != null) {
                vlcMediaPlayer.setRate(rate);
            }
            else {
                Log.e(TAG, "setPlaybackRate vlcMediaPlayer is Null");
            }
            lastPlaybackRate = rate;
            return;
        }
        Log.e(TAG, "setPlaybackRate is only implemented for LibVLC");
    }

    @SuppressWarnings("unused")
    public static void setHideOnPause(boolean hide) {
        hideOnPause = hide;
    }

    @SuppressWarnings("unused")
    public static void setFullscreen(boolean fullscreen, boolean animated) {
        //TODO : implement animated
        isFullScreen = fullscreen;
        if(useVLC) {
            NativeUtility.getMainActivity().runOnUiThread(() -> {
                adjustBaseView();
                //Run later, so that BaseView have its new size and we can adjust accordingly
                NativeUtility.getMainActivity().runOnGLThread(() -> {
                    //This force LibVLC to recalculate the surface size to a correct value. Put 1,1 to not have a visual glitch
                    getInstance().setSurfaceSize(currentVideoWidth, currentVideoHeight);
                });
            });
        }
        else if (baseId != null) {
            NativeUtility.getMainActivity().runOnUiThread(() -> {
                FrameLayout mainFrame = NativeUtility.getMainActivity().getMainLayout();
                if(mainFrame == null) return;
                FrameLayout base = mainFrame.findViewById(baseId);
                if(base != null) {
                    if(isFullScreen) {
                        base.setBackgroundColor(Color.BLACK);
                        base.setAlpha((float) 0.6);
                    }
                    else {
                        base.setAlpha((float) 0);
                    }
                    adjustBaseView();
                }
            });
        }
    }

    @SuppressWarnings("unused")
    public static boolean isFullscreen() {
        return isFullScreen;
    }

    @SuppressWarnings("WeakerAccess")
    public static float getDuration() {
        float duration = 0;
        if(useVLC) {
            if(vlcMediaPlayer != null) {
                duration = vlcMediaPlayer.getLength();
            }
            else {
                Log.e(TAG, "getDuration vlcMediaPlayer is Null");
            }
        }
        else if (isPrepared) {
            try {
                duration = ((VideoView) Objects.requireNonNull(getVideoView())).getDuration();
            } catch (Exception e) {
                //If the video view isn't properly prepared, handle it
                e.printStackTrace();
                duration = 0;
            }
        }
        //convert in seconds or to 0 for empty
        duration = duration == -1 ? 0 : duration / 1000;
        return duration;
    }

    @SuppressWarnings("unused")
    public static float getPosition() {
        float position = 0;
        if(useVLC) {
            if(vlcMediaPlayer != null) {
                position = vlcMediaPlayer.getTime();
            }
            else {
                Log.e(TAG, "getPosition getPosition is Null");
            }
        }
        else {
            position = ((VideoView) Objects.requireNonNull(getVideoView())).getCurrentPosition();
        }
        //convert in seconds or to 0 for empty
        position = position == -1 ? 0 : position / 1000;
        return position;
    }

    @SuppressWarnings("unused")
    public static void setPosition(float position) {
        if(useVLC) {
            if(vlcMediaPlayer != null) {
                vlcMediaPlayer.setTime((long)(position * 1000));
            }
            else {
                Log.e(TAG, "setPosition getPosition is Null");
            }
        }
        else {
            VideoView targetView = (VideoView) getVideoView();
            if(targetView != null) {
                targetView.seekTo((int) (position * 1000));
            }
            else {
                Log.e(TAG, "No VideoView during setPosition call");
            }
        }
    }

    @SuppressWarnings("WeakerAccess")
    public static void setMuted(boolean _muted) {
        if(useVLC) {
            if(vlcMediaPlayer != null) {
                vlcMediaPlayer.setVolume(_muted ? 0 : 100);
            }
            else {
                Log.e(TAG, "setMuted vlcMediaPlayer is Null");
            }
        }
        else {
            muted = _muted;
            try {
                if(videoViewMediaPlayer != null && videoViewMediaPlayer.isPlaying()) {
                    if (muted)
                        videoViewMediaPlayer.setVolume(0, 0);
                    else
                        videoViewMediaPlayer.setVolume(1.0f, 1.0f);
                }
            }
            catch(Exception e) {
                Log.e(TAG, "setMuted Exception");
                e.printStackTrace();
            }
        }
    }

    @SuppressWarnings("unused")
    public static String getScreenshot(String path, int videoLocation, String screenshotPath, int screenshotLocation)
    {
        File videoFile = getFile(path, FileUtility.FileLocation.valueOf(videoLocation));
        if(videoFile == null) {
            return null;
        }
        String thumbName = screenshotPath;
        if(thumbName.isEmpty())
        {
            //Get the video full path, without extension
            String fileName = path.lastIndexOf('.') > -1 ? path.substring(0, path.lastIndexOf('.')) : path;
            if(FileUtility.FileLocation.valueOf(videoLocation) == FileUtility.FileLocation.Absolute
                    && FileUtility.FileLocation.valueOf(screenshotLocation) != FileUtility.FileLocation.Absolute
                    && fileName.lastIndexOf('/') > -1) {
                // If we are not using absolute for screenshot but we use it for video, that mean we have a path to parse
                fileName = fileName.substring(fileName.lastIndexOf('/') + 1);
            }
            //Add -thumbnail. That's the path used by cocos2dx
            thumbName = fileName + "-thumbnail";
        }

        String thumbPath = FileUtility.getFullPath(thumbName + ".png", screenshotLocation);
        //Don't redo it if it already exists
        if(new File(thumbPath).exists()) {
            Log.d(TAG, "Video screenshot already created at path: " + thumbName);
            return thumbPath;
        }
        try {
            Log.d(TAG, "saving video screenshot at path: " + thumbName + ", video path: " + videoFile.getAbsolutePath());
            //Save the screenshot in a PNG compressed format, and close everything. If something fails, return null
            FileOutputStream streamScreenshot = new FileOutputStream(thumbPath);

            MediaMetadataRetriever retriever = new MediaMetadataRetriever();
            try {
                Uri appUri = NativeUtility.getMainActivity().getUriFromFileName(path);
                if(appUri != null && appUri.toString().startsWith("android.resource://")) {
                    //Raw resources cannot be loaded with absolute path
                    retriever.setDataSource(NativeUtility.getMainActivity(), appUri);
                }
                else {
                    retriever.setDataSource(videoFile.getAbsolutePath());
                }
                int timeInSeconds = 1;
                Bitmap thumb = retriever.getFrameAtTime(timeInSeconds * 1000000,
                        MediaMetadataRetriever.OPTION_CLOSEST_SYNC);
                if(thumb == null) {
                    // Since the first method didn't work, let's try an other one
                    // The problem is that it doesn't allow to get at a specific time
                    thumb = ThumbnailUtils.createVideoThumbnail(videoFile.getAbsolutePath(), MediaStore.Images.Thumbnails.MINI_KIND);
                }
                if(thumb != null) {
                    thumb.compress(CompressFormat.PNG, 80, streamScreenshot);
                    thumb.recycle(); //ensure the image is freed;
                }
                else {
                    thumbPath = null;
                }
            } catch (Exception ex) {
                Log.i(TAG, "MediaMetadataRetriever in getScreenshot got exception:" + ex);
                thumbPath = null;
            }
            retriever.release();
            streamScreenshot.close();
            Log.d(TAG, "screenshot saved successfully");
        } catch (FileNotFoundException e) {
            Log.d(TAG, "File Not Found Exception : check directory path");
            e.printStackTrace();
            thumbPath = null;
        } catch (IOException e) {
            Log.d(TAG, "IOException while closing the stream");
            e.printStackTrace();
            thumbPath = null;
        }
        return thumbPath;
    }

    @SuppressWarnings("unused")
    public static float[] getVideoSize(String path, int location) {
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        File videoFile = getFile(path, FileUtility.FileLocation.valueOf(location));
        float[] size = new float[2];
        size[0] = 0;
        size[1] = 0;
        try {
            Uri appUri = NativeUtility.getMainActivity().getUriFromFileName(path);
            if(appUri != null && appUri.toString().startsWith("android.resource://")) {
                //Raw resources cannot be loaded with absolute path
                retriever.setDataSource(NativeUtility.getMainActivity(), appUri);
            }
            else {
                assert videoFile != null;
                retriever.setDataSource(videoFile.getAbsolutePath());
            }
            size[0] = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH));
            size[1] = Integer.valueOf(retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT));
        } catch (Exception ex) {
            Log.i(TAG, "MediaMetadataRetriever in getVideoSize got exception:" + ex);
        }
        try {
            retriever.release();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return size;
    }

    @SuppressWarnings("unused")
    public static boolean videoExists(String path) {
        int dotIndex = path.lastIndexOf(".");
        if (dotIndex != -1) {
            String fileExt = path.substring(dotIndex);
            File videoFile = getFile(path, FileUtility.FileLocation.Unknown);
            return org.videolan.libvlc.util.Extensions.VIDEO.contains(fileExt)
                    && videoFile != null
                    && (NativeUtility.getMainActivity().getUriFromFileName(path) != null // check for assets files
                        || videoFile.exists() && videoFile.canRead());
        }
        return false;
    }

    // location support Unknown which trigger legacy case
    @SuppressLint("SetWorldReadable")
    private static File getFile(String path, FileUtility.FileLocation location) {
        File foundFile = null;
        if(location != FileUtility.FileLocation.Unknown) {
            if (location == FileUtility.FileLocation.Resources) {// Search first in expansion
                Uri expansionUri = NativeUtility.getMainActivity().getUriFromFileName(path);
                if (expansionUri != null) {
                    foundFile = new File(Objects.requireNonNull(expansionUri.getPath()));
                }
                // Search inside the assets if it's not in the expansion
                if (foundFile == null || !foundFile.exists() || !foundFile.canRead()) {
                    // TODO: code copy/pasted from below, refactor me
                    ContentResolver cr = NativeUtility.getMainActivity().getContentResolver();
                    String[] projection = {MediaStore.MediaColumns.DATA};
                    Cursor cur = cr.query(Uri.parse(path), projection, null, null, null);
                    if (cur != null && cur.moveToFirst()) {
                        String filePath = cur.getString(0);
                        cur.close();
                        foundFile = new File(filePath);
                        if (foundFile.exists()) {
                            boolean result = foundFile.setReadable(true, false);
                            Log.i(TAG, "File found, path : " + filePath + " could set readable ? " + (result ? "true" : "false"));
                            if (!foundFile.canRead()) {
                                Log.e(TAG, "Error, cannot read file");
                                return null;
                            }
                        } else {
                            Log.e(TAG, "File not found for path : " + filePath);
                            return null;
                        }
                    } else {
                        Log.e(TAG, "Invalid URI or other problem with path : " + path);
                        return null;
                    }
                }
            } else {
                String fullPath = FileUtility.getFullPath(path, location);
                foundFile = new File(fullPath);
                if (!foundFile.exists() || !foundFile.canRead()) {
                    return null;
                }
            }
        }
        else {
            // Legacy case, left untouched for now for compatibility
            // TODO: refactor me
            Uri localURI = NativeUtility.getMainActivity().getUriFromFileName(path);
            final String[] storageDict = VideoPicker.getStorageDirectories();
            //First check the most common path, then check all storage directories
            boolean startWithStorageDict = path.startsWith(Environment.getExternalStorageDirectory().toString());
            for (int i = 0; i < storageDict.length && !startWithStorageDict; i++) {
                if (path.startsWith(storageDict[i])) {
                    startWithStorageDict = true;
                }
            }
            if (localURI != null) {
                foundFile = new File(Objects.requireNonNull(localURI.getPath()));
            } else if (path.startsWith(FileUtility.getLocalPath())
                    || startWithStorageDict) {
                foundFile = new File(path);
            } else {
                ContentResolver cr = NativeUtility.getMainActivity().getContentResolver();
                String[] projection = {MediaStore.MediaColumns.DATA};
                Cursor cur = cr.query(Uri.parse(path), projection, null, null, null);
                if (cur != null && cur.moveToFirst()) {
                    String filePath = cur.getString(0);
                    cur.close();
                    foundFile = new File(filePath);
                    if (foundFile.exists()) {
                        boolean result = foundFile.setReadable(true, false);
                        Log.i(TAG, "File found, path : " + filePath + " could set readable ? " + (result ? "true" : "false"));
                        if (!foundFile.canRead()) {
                            Log.e(TAG, "Error, cannot read file");
                            return null;
                        }
                    } else {
                        Log.e(TAG, "File not found for path : " + filePath);
                        return null;
                    }
                } else {
                    Log.e(TAG, "Invalid URI or other problem with path : " + path);
                    return null;
                }
            }
        }
        return foundFile;
    }

    private void setSurfaceSize(final int width, final int height) {
        if (width * height == 0)
            return;
        currentVideoWidth = width;
        currentVideoHeight = height;
        // force surface buffer size
        NativeUtility.getMainActivity().runOnUiThread(() -> {
            FrameLayout mainFrame = NativeUtility.getMainActivity().getMainLayout();
            if(mainFrame == null) return;
            FrameLayout base = mainFrame.findViewById(baseId);
            SurfaceView videoView = getVideoView();
            if(videoView != null && base != null) {
                SurfaceHolder mSurfaceHolder = videoView.getHolder();
                mSurfaceHolder.setFixedSize(width, height);

                // set display size as libVLC otherwise doesn't keep aspect ratio.
                //VideoPlayer.localWidth/Height is the frame maximum size, width/height is the video size
                //visible_width/height should be ignored, it's how VLC think it should fit
                PointF targetSize = new PointF(base.getWidth(), base.getHeight());
                float ratioWidth = targetSize.x / (float)width;
                float ratioHeight = targetSize.y / (float)height;
                if(ratioWidth > ratioHeight) {
                    targetSize.x = targetSize.x / ratioWidth * ratioHeight;
                }
                else {
                    targetSize.y = targetSize.y / ratioHeight * ratioWidth;
                }

                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                        (int)targetSize.x,
                        (int)targetSize.y,
                        Gravity.TOP | Gravity.START);
                lp.leftMargin = (int) (base.getWidth() - targetSize.x) / 2;
                lp.topMargin = (int)(base.getHeight() - targetSize.y) / 2;
                videoView.setLayoutParams(lp);
                videoView.invalidate();
            }
        });
    }

    @Override
    @SuppressWarnings("unused")
    public void onNewLayout(IVLCVout vlcVout,final int width,final int height, int visibleWidth, int visibleHeight, int sarNum, int sarDen) {
        if (width * height == 0)
            return;

        currentVideoWidth = width;
        currentVideoHeight = height;
        if(vlcVout != vlcMediaPlayer.getVLCVout()) {
            try {
                vlcVout.setVideoView(getVideoView());
                vlcVout.attachViews();
            }
            catch(IllegalStateException e) {
                e.printStackTrace();
            }
        }
        setSurfaceSize(width, height);
    }

    @Override
    @SuppressWarnings("unused")
    public void onSurfacesCreated(IVLCVout vlcVout) {
    }

    @Override
    @SuppressWarnings("unused")
    public void onSurfacesDestroyed(IVLCVout vlcVout) {
        this.releasePlayer();
    }

    @Override
    public void onHardwareAccelerationError(IVLCVout vlcVout) {
        Log.e(TAG, "Error with hardware acceleration");
        this.releasePlayer();
    }

    @Override
    public void run() {
        play();
    }

    private static class MyPlayerListener implements org.videolan.libvlc.MediaPlayer.EventListener {
        private WeakReference<VideoPlayer> mOwner;

        MyPlayerListener(VideoPlayer owner) {
            mOwner = new WeakReference<>(owner);
        }

        @Override
        @SuppressWarnings("unused")
        public void onEvent(org.videolan.libvlc.MediaPlayer.Event event) {
            VideoPlayer player = mOwner.get();

            switch(event.type) {
                case org.videolan.libvlc.MediaPlayer.Event.EndReached:
                    Log.d(TAG, "MediaPlayerEndReached");
                    videoEnded = true;
                    NativeUtility.getMainActivity().runOnGLThread(() -> {
                        notifyVideoEnded(path);
                        if(shouldLoop) {
                            play();
                        }
                    });
                    break;
                case org.videolan.libvlc.MediaPlayer.Event.Playing:
                case org.videolan.libvlc.MediaPlayer.Event.Paused:
                case org.videolan.libvlc.MediaPlayer.Event.Stopped:
                default:
                    break;
            }
        }
    }

    private void releasePlayer() {
        if (libVLC == null)
            return;
        vlcMediaPlayer.stop();
        final IVLCVout vout = vlcMediaPlayer.getVLCVout();
        vout.removeCallback(this);
        vout.detachViews();
        libVLC.release();
        libVLC = null;
    }
}
