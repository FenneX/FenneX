#!/bin/sh

# All icons must be in the root of the specified directory in -i
# The names of iOS and Android icons must be : AppName-logo-android.png and AppName-logo-ios.png
# 
# The destination folder must respect the following structure :
# - iOS -> AppName/proj.ios/icons
# - Android -> AppName/proj.android/res/drawable-...
#
#

# Initialize variables:
PROJECTPATH=""
ICONPATH=""
ICONDIR=""
COMMITCMD="git commit -am 'Update icons'"
TOCOMMIT=()

function showHelp()
{
	echo "Usage : ./icon-script.sh /Path/To/Project/Root/Folder /Path/To/Icons/Folder"
}

if [ ! `convert --version | grep "not found"` ]; then
	#if there is the four arguments needed
	if [ $2 ]; then
		PROJECTPATH=$1
		ICONPATH=$2

		if [ "${PROJECTPATH:${#PROJECTPATH}-1}" != "/" ]; then
			PROCESSICONPATH="$PROJECTPATH/"
		fi

		if [ "${ICONPATH:${#ICONPATH}-1}" != "/" ]; then
			PROCESSICONPATH="$ICONPATH/"
		fi

		if [ ! -d $PROJECTPATH ]; then
			echo "[ERROR] The folder '$PROJECTPATH' doesn't exist"
			exit 1
		fi

		if [ ! -d $ICONPATH ]; then
			echo "[ERROR] The folder '$ICONPATH' doesn't exist"
			exit 1
		fi

		if [ -d "${PROJECTPATH}proj.ios/icons/" ]; then
			ICONDIR="Icons/"
		fi

		if [ ! -d "${ICONPATH}Store/" ]; then
			mkdir "${ICONPATH}Store/"
			echo "[INFO]Creating ${ICONPATH}Store/"
		else
			echo "[INFO]${ICONPATH}Store/ already exist"
		fi

		PROCESSICONPATH="$ICONPATH*"
		#Check icons size
		echo "[INFO]Browsing ${ICONPATH}"
		for f in $PROCESSICONPATH
		do
			filename=$(basename "$f")
			filename="${filename%.*}"
			
			OS=`echo $filename | cut -d'-' -f3`
			APP=`echo $filename | cut -d'-' -f1`
			
			echo "OS: $OS App: $APP";
		  	
		  	echo "[INFO]Checking operating system"
		  	if [ $OS == "android" ]; then
		  		echo "******************** ANDROID ********************"
		  		RESULT=`identify -format "%wx%h" $f`
		  		echo "[INFO]Checking ${f} size"
		  		if [ $RESULT == "1024x1024" ]; then
		  			echo "[OK]Size checked"
		  			convert ${f} -resize 72x72 "${PROJECTPATH}proj.android/res/drawable-hdpi/icon.png"
		  			convert ${f} -resize 32x32 "${PROJECTPATH}proj.android/res/drawable-ldpi/icon.png"
		  			convert ${f} -resize 48x48 "${PROJECTPATH}proj.android/res/drawable-mdpi/icon.png"
		  			convert ${f} -resize 96x96 "${PROJECTPATH}proj.android/res/drawable-xhdpi/icon.png"
		  			convert ${f} -resize 144x144 "${PROJECTPATH}proj.android/res/drawable-xxhdpi/icon.png"
		  			convert ${f} -resize 192x192 "${PROJECTPATH}proj.android/res/drawable-xxxhdpi/icon.png"

		  			TOCOMMIT+=("proj.android/res/drawable-hdpi/icon.png")
		  			TOCOMMIT+=("proj.android/res/drawable-ldpi/icon.png")
		  			TOCOMMIT+=("proj.android/res/drawable-mdpi/icon.png")
		  			TOCOMMIT+=("proj.android/res/drawable-xhdpi/icon.png")
		  			TOCOMMIT+=("proj.android/res/drawable-xxhdpi/icon.png")
		  			TOCOMMIT+=("proj.android/res/drawable-xxxhdpi/icon.png")

		  			convert ${f} -resize 512x512 "${ICONPATH}Store/${APP}-PlayStore.png"
		  		else
		  			echo "[ERROR]The Android icon is not correctly sized ($RESULT)"
					exit 1
		  		fi
		  	elif [ $OS == "ios" ]; then
		  		echo "********************   iOS   ********************"
		  		RESULT=`identify -format "%wx%h" $f`
		  		CHANNEL=`identify -format "%[channels]" $f`
		  		echo "[INFO]Checking ${f} size"
		  		if [ $RESULT == "1024x1024" ]; then
		  			echo "[OK]Size checked"
		  			echo "[INFO]Checking ${f} alpha channel"
		  			if [ $CHANNEL == "srgb" ]; then
		  				echo "[OK]Alpha channel checked"
			  			convert ${f} -resize 29x29 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-29.png"
			  			convert ${f} -resize 32x32 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-32.png"
			  			convert ${f} -resize 48x48 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-48.png"
			  			convert ${f} -resize 57x57 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-57.png"
			  			convert ${f} -resize 58x58 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-58.png"
			  			convert ${f} -resize 72x72 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-72.png"
			  			convert ${f} -resize 76x76 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-76.png"
			  			convert ${f} -resize 96x96 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-96.png"
			  			convert ${f} -resize 100x100 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-100.png"
			  			convert ${f} -resize 114x114 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-114.png"
			  			convert ${f} -resize 120x120 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-120.png"
			  			convert ${f} -resize 144x144 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-144.png"
			  			convert ${f} -resize 152x152 "${PROJECTPATH}proj.ios/${ICONDIR}Icon-152.png"
			  			
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-29.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-32.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-48.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-57.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-58.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-72.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-76.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-96.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-100.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-114.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-120.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-144.png")
			  			TOCOMMIT+=("proj.ios/${ICONDIR}Icon-152.png")

		  				convert ${f} -resize 1024x1024 "${ICONPATH}Store/${APP}-AppleStore.png"
			  		else
			  			echo "[ERROR]The iOS icon has an alpha channel ($CHANNEL)"
						exit 1
			  		fi
		  		else
		  			echo "[ERROR]The iOS icon is not correctly sized ($RESULT)"
					exit 1
		  		fi
		  	fi
		done

		cd ${PROJECTPATH}
		count=${#TOCOMMIT[@]}
		if [ ${count} -gt 0 ]; then
			index=0
			while [ "$index" -lt "$count" ]; do
	  			if [ -f ${TOCOMMIT[$index]} ]; then
				    echo "git add ${TOCOMMIT[$index]}"
				    git add ${TOCOMMIT[$index]}
	  			fi
	  			let "index++"
			done
			echo "$COMMITCMD"
			eval $COMMITCMD
		fi
		cd -
	else
		showHelp
	fi
else
	echo "[ERROR]ImageMagick is not installed"
	exit 1
fi
# End of file