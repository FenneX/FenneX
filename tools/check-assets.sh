#!/bin/sh

# The structure of the asset directory must contains the following subfolders :
# - ipadhd
# - ipad
# - iphone
#
# You can create a file named .assetignore to ignore some asset.
# The rules are : one file per line, the file to ignore has to be in the ipadhd directory and the file must end with a empty line
#

TOIGNORE=("*")
ASSETPATH=""
DELTASENSIBILITY="5"

function showHelp()
{
	echo "\nUsage : ./check-assets.sh /Path/To/Asset/Root/Folder\n"
}

if [ ! `identify --version | grep "not found"` ]; then
	if [ $1 ]; then
		array=('hello' 'world' 'my' 'name' 'is' 'perseus')
		word="machin"
		if echo "${array[@]}" | fgrep --word-regexp "$word"; then
		    echo "FOUND"
		fi

		ASSETPATH=$1
		if [ "${ASSETPATH:${#ASSETPATH}-1}" != "/" ]; then
			ASSETPATH="$ASSETPATH/"
		fi

		if [ -f ".assetignore" ]; then
			while read line
			do
				TOIGNORE+=($line)
			done < ".assetignore"
		fi


		for f in ${ASSETPATH}ipadhd/*
		do
			echo "${f}\n"
			filename=$(basename "$f")
			
			if echo "${TOIGNORE[@]}" | fgrep --word-regexp "$filename"; then
				#ignore this file
				echo "[INFO]Ignoring ${filename}"
			else
				echo "[INFO]Analysing ${filename}"
				if [ -f "${ASSETPATH}ipad/${filename}" ] && [ -f "${ASSETPATH}iphone/${filename}" ]; then					
					WIDTH=`identify -format "%w" $f`
					HEIGHT=`identify -format "%h" $f`

					WIDTH50=`identify -format "%w" ${ASSETPATH}ipad/${filename}`
					HEIGHT50=`identify -format "%h" ${ASSETPATH}ipad/${filename}`

					WIDTH21=`identify -format "%w" ${ASSETPATH}iphone/${filename}`
					HEIGHT21=`identify -format "%h" ${ASSETPATH}iphone/${filename}`

					if [ $WIDTH50 -le $((($WIDTH / 2)+$DELTASENSIBILITY)) ] && [ $WIDTH50 -ge $((($WIDTH / 2)-$DELTASENSIBILITY)) ] && [ $HEIGHT50 -le $((($HEIGHT / 2)+$DELTASENSIBILITY)) ] && [ $HEIGHT50 -ge $((($HEIGHT / 2)-$DELTASENSIBILITY)) ]; then
						if [ $WIDTH21 -le $((($WIDTH * 21 / 100)+$DELTASENSIBILITY)) ] && [ $WIDTH21 -ge $((($WIDTH * 21 / 100)-$DELTASENSIBILITY)) ] && [ $HEIGHT21 -le $((($HEIGHT * 21 / 100)+$DELTASENSIBILITY)) ] && [ $HEIGHT21 -ge $((($HEIGHT * 21 / 100)-$DELTASENSIBILITY)) ]; then
							echo "[OK]${f} is correct"
						else
							echo "[ERROR]The resize of ${filename} is not correct\n------>iPhone version - ${WIDTH}x${HEIGHT} to ${WIDTH21}x${HEIGHT21} instead of $((($WIDTH * 21 / 100)-$DELTASENSIBILITY))x$((($HEIGHT * 21 / 100)-$DELTASENSIBILITY))"
							exit 1
						fi
					else
						echo "[ERROR]The resize of ${filename} is not correct\n------>iPad version - ${WIDTH}x${HEIGHT} to ${WIDTH50}x${HEIGHT50} instead of $((WIDTH / 2))x$((HEIGHT / 2))"
						exit 1
					fi
				else
					echo "[ERROR]The asset ${filename} is not present in ipad or iphone folder"
					exit 1
				fi
			fi
		done
	else
		showHelp
	fi
else
	echo "[ERROR]ImageMagick is not installed"
	exit 1
fi