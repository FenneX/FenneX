#!/bin/sh

# All SplashScreens must me in the root of the specified directory in -i
# The names of SplashScreens must be : splash-screen-WIDTH-HEIGHT.png
# The folder AppName/projects/Ressources must exist and contains splashscreens 
#
# The destination folder must respect the following structure :
# - iOS -> AppName/projects/project.ios/Splashscreens or AppName/projects/project.ios
# - Android -> AppName/projects/Ressources
#
#

# Initialize variables:
PROJECTPATH=""
SPLASHPATH=""
SPLASHDIR=""
SPLASHCOMMITDIR=""
COMMITCMD="git commit -am 'Update SplashScreens'"
TOPROCESS=()
TOCOMMIT=()

function showHelp()
{
	echo "\nUsage : ./splashscreen-script.sh /Path/To/Project/Root/Folder/ /Path/To/SplashScreens/Folder/\n"
}

if [ ! `convert --version | grep "not found"` ]; then
	#if there is the four arguments needed
	if [ $2 ]; then
		PROJECTPATH=$1
		SPLASHPATH=$2

		if [ "${PROJECTPATH:${#PROJECTPATH}-1}" != "/" ]; then
			PROCESSPLASHPATH="$PROJECTPATH/"
		fi

		if [ "${SPLASHPATH:${#SPLASHPATH}-1}" != "/" ]; then
			PROCESSPLASHPATH="$SPLASHPATH/"
		fi

		if [ ! -d $PROJECTPATH ]; then
			echo "[ERROR] The folder '$PROJECTPATH' doesn't exist"
			exit 1
		fi

		if [ ! -d $SPLASHPATH ]; then
			echo "[ERROR] The folder '$SPLASHPATH' doesn't exist"
			exit 1
		fi

		if [ ! -d "${PROJECTPATH}Resources" ]; then
			echo "[ERROR] The folder '${PROJECTPATH}Resources' doesn't exist"
			exit 1
		fi

		#iOS
		if [ -d "${PROJECTPATH}proj.ios_mac/ios/Splashscreens/" ]; then
			SPLASHDIR="${PROJECTPATH}proj.ios_mac/ios/Splashscreens/"
			SPLASHCOMMITDIR="proj.ios_mac/ios/Splashscreens/"
		else
			SPLASHDIR="${PROJECTPATH}proj.ios_mac/ios/"
			SPLASHCOMMITDIR="proj.ios_mac/ios/"
		fi

		PROCESSSPLASHPATH="$SPLASHPATH*"
		#Check icons size
		echo "[INFO]Browsing ${SPLASHPATH}"
		for f in $PROCESSSPLASHPATH
		do
			filename=$(basename "$f")
			filename="${filename%.*}"
			
			TYPE=`echo $filename | cut -d'-' -f1`
			TYPE=`echo $TYPE | awk '{print tolower($0)}'`
			WIDTH=`echo $filename | cut -d'-' -f3`
			HEIGHT=`echo $filename | cut -d'-' -f4`
		  	
		  	echo "[INFO]Checking if it's a SplashScreen"
		  	if [ $TYPE == "splash" ]; then
		  		echo "[OK]SplashScreen detected"
		  		RESULT=`identify -format "%wx%h" $f`
		  		echo "[INFO]Checking ${f} size"
		  		if [ $RESULT == "${WIDTH}x${HEIGHT}" ]; then
		  			echo "[OK]Size checked"
		  			
		  			if [ $RESULT == "2560x1600" ] || [ $RESULT == "2048x1536" ] || 
		  				[ $RESULT == "1920x1080" ]|| [ $RESULT == "1280x800" ]|| 
		  				[ $RESULT == "1280x720" ]|| [ $RESULT == "1024x768" ]|| 
		  				[ $RESULT == "1024x600" ]|| [ $RESULT == "960x640" ]|| 
		  				[ $RESULT == "800x480" ]|| [ $RESULT == "480x320" ]|| 
		  				[ $RESULT == "1136x640" ]; then
		  				TOPROCESS+=("${f}")
		  			fi
		  		else
		  			echo "[ERROR]The SplashScreen is not well named or has not the correct size ($RESULT instead of ${WIDTH}x${HEIGHT})"
					exit 1
		  		fi
		  	fi
		done

		count=${#TOPROCESS[@]}
		if [ ${count} == 11 ]; then
			#iOS
			cp "${SPLASHPATH}splash-screen-1024-768.png" "${SPLASHDIR}Default-Landscape~ipad.png"
			cp "${SPLASHPATH}splash-screen-1024-768.png" "${SPLASHDIR}Default-Landscape.png"

		  	convert "${SPLASHDIR}Default-Landscape~ipad.png" -rotate 90 "${SPLASHDIR}Default-Portrait~ipad.png"
			cp "${SPLASHDIR}Default-Portrait~ipad.png" "${SPLASHDIR}Default~ipad.png"
			cp "${SPLASHDIR}Default-Portrait~ipad.png" "${SPLASHDIR}Default-Portrait.png"

			cp "${SPLASHPATH}splash-screen-2048-1536.png" "${SPLASHDIR}Default-Landscape@2x~ipad.png"
			cp "${SPLASHPATH}splash-screen-2048-1536.png" "${SPLASHDIR}Default-Landscape@2x.png"
		  	convert "${SPLASHDIR}Default-Landscape@2x~ipad.png" -rotate 90 "${SPLASHDIR}Default-Portrait@2x~ipad.png"
			cp "${SPLASHDIR}Default-Portrait@2x~ipad.png" "${SPLASHDIR}Default-Portrait@2x.png"

		  	convert "${SPLASHPATH}splash-screen-480-320.png" -rotate 90 "${SPLASHDIR}Default.png"

		  	convert "${SPLASHPATH}splash-screen-960-640.png" -rotate 90 "${SPLASHDIR}Default~iphone.png"
			cp "${SPLASHDIR}Default~iphone.png" "${SPLASHDIR}Default@2x.png"

		  	convert "${SPLASHPATH}splash-screen-1136-640.png" -rotate 90 "${SPLASHDIR}Default-568h@2x.png"

		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Landscape~ipad.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Landscape.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Portrait~ipad.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Portrait.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default~ipad.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Landscape@2x~ipad.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Landscape@2x.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Portrait@2x~ipad.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-Portrait@2x.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default@2x.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default~iphone.png")
		  	TOCOMMIT+=("${SPLASHCOMMITDIR}Default-568h@2x.png")
		else
			echo "[ERROR]Missing SplashScreen"
			exit 1
		fi

		cd ${PROJECTPATH}
		count=${#TOCOMMIT[@]}
		if [ ${count} -gt 0 ]; then
			index=0
			while [ "$index" -lt "$count" ]; do
			    echo "git add ${TOCOMMIT[$index]}"
			    git add ${TOCOMMIT[$index]}
	  			let "index++"
			done
			echo "${COMMITCMD}"
			eval $COMMITCMD
		fi
		rm -rf 0
		cd -
	else
		showHelp
	fi
else
	echo "[ERROR]ImageMagick is not installed"
	exit 1
fi
# End of file