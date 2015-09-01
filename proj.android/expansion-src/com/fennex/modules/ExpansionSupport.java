package com.fennex.modules;

import java.io.File;

import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Environment;
import android.os.Messenger;
import android.util.Log;

import com.google.android.vending.expansion.downloader.DownloadProgressInfo;
import com.google.android.vending.expansion.downloader.DownloaderClientMarshaller;
import com.google.android.vending.expansion.downloader.DownloaderServiceMarshaller;
import com.google.android.vending.expansion.downloader.Helpers;
import com.google.android.vending.expansion.downloader.IDownloaderClient;
import com.google.android.vending.expansion.downloader.IDownloaderService;
import com.google.android.vending.expansion.downloader.IStub;
import com.google.android.vending.expansion.downloader.impl.DownloaderService;
/** Used to download Expansions from Google Play
 * requires permissions:
    <!-- Required to access Google Play Licensing -->
    <uses-permission android:name="com.android.vending.CHECK_LICENSE" />

    <!-- Required to download files from Google Play -->
    <uses-permission android:name="android.permission.INTERNET" />

    <!-- Required to keep CPU alive while downloading files (NOT to keep screen awake) -->
    <uses-permission android:name="android.permission.WAKE_LOCK" />

    <!-- Required to poll the state of the network connection and respond to changes -->
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />

    <!-- Required to check whether Wi-Fi is enabled -->
    <uses-permission android:name="android.permission.ACCESS_WIFI_STATE"/>

    <!-- Required to read and write the expansion files on shared storage -->
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />

 * The service must be added to the application:
    <service android:name="com.fennex.modules.ExpansionSupport" android:enabled="true" />
    <receiver android:name="com.fennex.modules.ExpansionAlarmReceiver" android:enabled="true" />

 * Libraries dependencies must be added to your build.gradle (in dependencies {}):
    compile project(':downloader_library')
    compile project(':play_licensing_library')

 * Libraries dependencies must be referenced from settings.gradle:
    include ':downloader_library'
    include ':play_licensing_library'

    project(':downloader_library').projectDir = new File(rootProject.projectDir, '/libraries/downloader_library')
    project(':play_licensing_library').projectDir = new File(rootProject.projectDir, '/libraries/play_licensing_library')

 * Those sources must be added in android/sourceSets/main/java.srcDirs (after 'src'):
    'expansion-src'


 * The main Activity must have the public key set to Google Play key

 * The main Activity must set ExpansionSupport.xAPKS during onCreate, example:
   ExpansionSupport.xAPKS = new ExpansionSupport.XAPKFile[] {
                                new ExpansionSupport.XAPKFile(
                                    true, // true signifies a main file
                                    2, // the version of the APK that the file was uploaded against
                                    51960989L // the length of the file in bytes
                                    ) };
 */
public class ExpansionSupport extends DownloaderService implements ActivityObserver, IDownloaderClient 
{
    // Salt generated with random.org
    public static final byte[] SALT = new byte[] { -65, -70, -79, 85, 3, -38,
            84, 15, -86, -39, 63, 96, 55, -50, -96, -86, 64, -48, -1, 5
    };
    private static IStub mDownloaderClientStub = null;
    private IDownloaderService mRemoteService = null;
    
    //try to get the publicKey and store it for when the activity is NULL
    private static String publicKey;
    public static XAPKFile[] xAPKS = null;
    
    private static final String TAG = "ExpansionSupport";
    //This just means you can now pause/resume the download (not implemented yet)
	private native void notifyServiceConnected(); 
	
	/*The interface should update with the status for the end-users. The codes are (at the end, the codes from IDownloadClient) :
	 * -1 => Un-affected codes (that shouldn't happen or are irrelevant)
	 * 1 => Connecting to the server (2, 3)
	 * 2 => Downloading the content (4)
	 * 3 => Connection problem (6, 8 ,9, 10, 11, 12)
	 * 4 => External storage problem (14, 17)
	 * 5 => Other error (13, 15, 16, 18, 19)
	 * 
	 * Un-affacted codes : 
	 * - 1 (Idle)
	 * - 5 => notifyDownloadCompleted
	 * - 7 => pausedByRequest => not implemented yet
	 */
	private native void notifyDownloadStateChanged(String status, int code, String translationKey);
	
	private native void notifyDownloadCompleted();
	
	//Update the % visible to the user and the total size (in bytes)
	private native void notifyDownloadProgress(float percent, long totalSize);


    private static volatile ExpansionSupport instance = null;
    public ExpansionSupport() 
    { 
    	if(instance != null && instance != this)
    	{
    		Log.w(TAG, "Warning, an instance of ExpansionSupport already exists");
    	}
    	else if(instance == this)
    	{
    		return;
    	}
    	instance = this;
    	if(NativeUtility.getMainActivity() != null)
    	{
    		NativeUtility.getMainActivity().addObserver(instance);
    		publicKey = NativeUtility.getMainActivity().getPublicKey();
    	}
    }

    public static ExpansionSupport getInstance() 
    {
        if (instance == null) 
        {
            synchronized (ExpansionSupport.class)
            {
                if (instance == null) 
                {
                	instance = new ExpansionSupport();
                }
            }
        }
        return instance;
    }
    
    /** Allow to know if the expansion files was downloaded
     * Always return true in debug mode
     * It's the caller responsibility to setup the download UI if it returns false
     * 
     * @return true if the expansion was delivered, false if it's starting to download
     */
    public static boolean checkExpansionFiles() 
    {
		boolean isDebuggable =  ( 0 != ( NativeUtility.getMainActivity().getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE ) );
		if(isDebuggable)
		{
			//return true;
		}
		if(xAPKS == null)
		{
			return true;
		}
        for (XAPKFile xf : xAPKS) 
        {
            String fileName = Helpers.getExpansionAPKFileName(NativeUtility.getMainActivity(), xf.mIsMain, xf.mFileVersion);
            if (!Helpers.doesFileExist(NativeUtility.getMainActivity(), fileName, xf.mFileSize, false))
            {  
            	// Build an Intent to start this activity from the Notification
                Intent notifierIntent = new Intent(NativeUtility.getMainActivity(), NativeUtility.getMainActivity().getClass());
                notifierIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK |
                                        Intent.FLAG_ACTIVITY_CLEAR_TOP);
                
                PendingIntent pendingIntent = PendingIntent.getActivity(NativeUtility.getMainActivity(), 0,
                        notifierIntent, PendingIntent.FLAG_UPDATE_CURRENT);

                // Start the download service (if required)
                int startResult = -1;
				try 
				{
					startResult = DownloaderClientMarshaller.startDownloadServiceIfRequired(NativeUtility.getMainActivity(),
					                pendingIntent, ExpansionSupport.class);
				} 
				catch (NameNotFoundException e) 
				{
					e.printStackTrace();
				}
                // If download has started, initialize this activity to show download progress
                if (startResult != DownloaderClientMarshaller.NO_DOWNLOAD_REQUIRED) 
                {
                	//The caller will setup the download UI
                	Log.i(TAG, "creating stub for download ...");
                	NativeUtility.getMainActivity().runOnUiThread(new Runnable() 
                	{
                		public void run() 
                		{ //For some reasons, that needs to be runned on UI Thread because it requires a Looper ....
                			try
                			{
		                	mDownloaderClientStub = DownloaderClientMarshaller.CreateStub(ExpansionSupport.getInstance(),
		                            ExpansionSupport.class);
		                	if(NativeUtility.getMainActivity().isActive())
		                	{
		                        mDownloaderClientStub.connect(NativeUtility.getMainActivity());
		                	}
                			}
                			catch(Exception e)
                			{
                				e.printStackTrace();
                			}
                		}
                	});
                	Log.i(TAG, "Stub created! Returning false");

                    return false;
                }
            }
        }
    	Log.i(TAG, "Expansion file exists");
        return true;
    }
    
    /* return the absolute path of the uncompressed file from the expansion if everything works well
     * return null if there is no information about that expansion
     * return "NOTDOWNLOADED" if the expansion file doesn't exist
     */
    public static String getExpansionFileFullPath(boolean main)
    {
    	//TODO : add a check beforehand : if the file already exists, directly return it instead of trying to read from expansion
        String fileName = null;
        int version = 0;
        for (XAPKFile xf : xAPKS) 
        {
        	if(xf.mIsMain)
        	{
        		fileName = Helpers.getExpansionAPKFileName(NativeUtility.getMainActivity(), xf.mIsMain, xf.mFileVersion);
                if (!Helpers.doesFileExist(NativeUtility.getMainActivity(), fileName, xf.mFileSize, false))
                {  
                	fileName = "NOTDOWNLOADED";
                }
                else
                {
                	version = xf.mFileVersion;
                }
        	}
        }
        if(fileName == null || version == 0 || fileName.equals(new String("NOTDOWNLAODED")))
        { //early return because there was an error
        	return fileName;
        }
        String packageName = NativeUtility.getMainActivity().getPackageName();
        //That's the full path, according to the documentation. Pretty complicated if you ask me.
        String absolutePath = Environment.getExternalStorageDirectory() + 
        		"/Android/obb/" + 
        		packageName + 
        		File.separator + 
        		(main ? "main." : "patch.") + 
        		version + 
        		"." + 
        		packageName + 
        		".obb";
        
        return absolutePath;
    }

    /**
     * This is a little helper class that demonstrates simple testing of an
     * Expansion APK file delivered by Market.
     */
    public static class XAPKFile 
    {
        public final boolean mIsMain;
        public final int mFileVersion;
        public final long mFileSize;

        public XAPKFile(boolean isMain, int fileVersion, long fileSize) 
        {
            mIsMain = isMain;
            mFileVersion = fileVersion;
            mFileSize = fileSize;
        }
    }

    @Override
    public String getPublicKey() 
    {
        return publicKey;
    }

    @Override
    public byte[] getSALT() 
    {
        return SALT;
    }

    @Override
    public String getAlarmReceiverClassName() 
    {
        return ExpansionAlarmReceiver.class.getName();
    }
    
    //TODO :  since we call checkExpansionFiles() from native init and not from onCreate, the order may not be correct ... 
    //Try to see if we can call that from native instead ?
    @Override
    public void onStateChanged(int state)
    {
    	switch(state)
    	{
    	case ActivityObserver.RESUME:
            if (mDownloaderClientStub != null) 
            {
                mDownloaderClientStub.connect(NativeUtility.getMainActivity());
            }    
            break;
    	case ActivityObserver.STOP:
            destroy();   
            break;
    	}
    }
    
    @Override
    public void destroy()
    {
        if (mDownloaderClientStub != null) 
        {
            mDownloaderClientStub.disconnect(NativeUtility.getMainActivity());
        }
    }

	@Override
	public void onServiceConnected(Messenger m) 
	{
	    mRemoteService = DownloaderServiceMarshaller.CreateProxy(m);
	    mRemoteService.onClientUpdated(mDownloaderClientStub.getMessenger());
	    notifyServiceConnected();
	}

	@Override
	public void onDownloadStateChanged(int newState) 
	{
		Log.i(TAG, "on download state changed : " + newState + ", as string : " + Helpers.getDownloaderStringResourceIDFromState(newState));
		switch (newState)
		{
		case IDownloaderClient.STATE_IDLE:
			notifyDownloadStateChanged("", -1, "");
			break;
	    case IDownloaderClient.STATE_FETCHING_URL:
	    case IDownloaderClient.STATE_CONNECTING:
			notifyDownloadStateChanged("Récupération des informations de téléchargement en cours ...", 1, "AC_Expansion_Infos");
			break;
	    case IDownloaderClient.STATE_DOWNLOADING:
			notifyDownloadStateChanged("Téléchargement du contenu additionnel de l'application ...", 2, "AC_Expansion_Download");
			break;
	    case IDownloaderClient.STATE_COMPLETED:
	    	notifyDownloadCompleted();
			break;
	    case IDownloaderClient.STATE_PAUSED_BY_REQUEST:
			notifyDownloadStateChanged("", -1, "");
			//We don't want to go into the details of those states yet, just draw a generic "no connexion"
	    case IDownloaderClient.STATE_PAUSED_NETWORK_UNAVAILABLE:
	    case IDownloaderClient.STATE_PAUSED_WIFI_DISABLED_NEED_CELLULAR_PERMISSION:
	    case IDownloaderClient.STATE_PAUSED_NEED_CELLULAR_PERMISSION:
	    case IDownloaderClient.STATE_PAUSED_WIFI_DISABLED:
	    case IDownloaderClient.STATE_PAUSED_NEED_WIFI:
	    case IDownloaderClient.STATE_PAUSED_ROAMING:
			notifyDownloadStateChanged("Aucune connexion réseau disponible, merci de réessayer après vous être connecté.", 3, "AC_Expansion_NoConnexion");
			break;
	    case IDownloaderClient.STATE_PAUSED_NETWORK_SETUP_FAILURE:
	    case IDownloaderClient.STATE_FAILED_FETCHING_URL:
			notifyDownloadStateChanged("Un problème s'est produit pendant la récupération des informations de téléchargement, merci de réessayer plus tard", 5, "AC_Expansion_ProblemInfos");
			break;
	    case IDownloaderClient.STATE_PAUSED_SDCARD_UNAVAILABLE:
			notifyDownloadStateChanged("L'espace de stockage externe n'est pas disponible.", 4, "AC_Expansion_StorageNotFound");
			break;
	    case IDownloaderClient.STATE_FAILED_UNLICENSED:
			notifyDownloadStateChanged("Un problème s'est produit. Merci de ré-installer l'application depuis Google Play.", 5, "AC_Expansion_Reinstall");
			break;
	    case IDownloaderClient.STATE_FAILED_SDCARD_FULL:
			notifyDownloadStateChanged("L'espace de stockage externe est plein. Merci de libérer de l'espace et réessayer.", 4, "AC_Expansion_StorageFull");
			break;
	    case IDownloaderClient.STATE_FAILED_CANCELED:
	    case IDownloaderClient.STATE_FAILED:
			notifyDownloadStateChanged("Un problème s'est produit. Merci de réessayer ultérieurement.", 5, "AC_Expansion_Problem");
			break;
		}	
	}

	@Override
	public void onDownloadProgress(DownloadProgressInfo progress) 
	{
		Log.i(TAG, "on download progress : " + Helpers.getDownloadProgressPercent(progress.mOverallProgress, progress.mOverallTotal));
		notifyDownloadProgress((float)progress.mOverallProgress / (float)progress.mOverallTotal, progress.mOverallTotal);
	}
}
