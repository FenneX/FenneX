#Tools scripts description and usage

/ ! \ Don't forget to set rights permissions on scripts files

##Icon generator

- Script name : icons-script.sh
- Required tools : ImageMagick command tools (convert and identify), grep and git
- Usage : ./icon-script.sh /Path/To/Project/Root/Folder /Path/To/Icons/Folder

All icons must be in the root of the specified directory in /Path/To/Icons/Folder
The names of iOS and Android icons must be : AppName-logo-android.png and AppName-logo-ios.png
 
The destination folder must respect the following structure :
- iOS -> AppName/projects/project.ios/icons
- Android -> AppName/projects/project.android/res/drawable-...

The Project directory has to be a git repository


##SplashScreens extractor

- Script name : splashscreen-script.sh
- Required tools : ImageMagick command tools (convert and identify), grep and git
- Usage : 
    ./splashscreen-script.sh /Path/To/Project/Root/Folder /Path/To/SplashScreens/Folder

All SplashScreens must me in the root of the specified directory in /Path/To/SplashScreens/Folder
The names of SplashScreens must be : splash-screen-WIDTH-HEIGHT.png
The folder AppName/projects/Resources must exist and contains splashscreens 

The destination folder must respect the following structure :
- iOS -> AppName/projects/project.ios/Splashscreens or AppName/projects/project.ios
- Android -> AppName/projects/Ressources


##Asset checker

- Script name : check-assets.sh
- Required tools : ImageMagick command tools (identify) and grep
- Usage : 
    ./check-assets.sh /Path/To/Project/Asset/Folder

The structure of the asset directory must contains the following subfolders :
- ipadhd
- ipad
- iphone

You can create a file named .assetignore to ignore some asset.
The rules are of this file are :
- One file per line
- The file to ignore has to be in the ipadhd directory
- Don't put the path of the file but the name with the extension
- The file must end with a empty line
