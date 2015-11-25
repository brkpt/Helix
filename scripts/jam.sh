#!/bin/bash
#set -x

BUILD="D:/projects/DeferredShader/build/jam.bat"
BUILDDIR="D:/projects/DeferredShader/build/"
declare -A CONFIGMAP
CONFIGMAP=( [debug]="win32/debug" [release]="win32/release" )

# Default to debug
CONFIG=debug

JAMOPTS=()
while getopts "c:fgh" opt; do
	case $opt in 
		# Set configuration
		c)
			if [ "${OPTARG,,}" = "debug" ]; then
				CONFIG=debug
			elif [ "${OPTARG,,}" = "release" ]; then
				CONFIG=release
			else
				echo "Unknown configuration: ${OPTVAR}"
			fi
			;;

		# Force rebuild
		f)
			JAMOPTS+=( "-a" )
			;;		

		# Generate dependency graph
		g)
			JAMOPTS+=( "-dg" )
			;;

		h)
			echo "Usage: jam.sh [option]"
			echo ""
			echo "Options:"
			echo "--------"
			echo "-c [configuration]"
			echo "  where [configuration] is debug|release"
			;;

		/?)
			echo "Invalid option: -${OPTARG}"
			exit 1
			;;
	esac
done

JAMOPTSTR=""
if [ ${#JAMOPTS[@]} -gt 0 ]; then
	for (( i=0; i<${#JAMOPTS[@]}; i++ )); do
		JAMOPTSTR+=${JAMOPTS[${i}]}
		JAMOPTSTR+=" "
	done
fi

echo "Config: ${CONFIG}"
echo "Jam options: ${JAMOPTSTR}"
#echo "ConfigMap: ${CONFIGMAP[${CONFIG}]}"
${BUILD} -C${BUILDDIR} C.TOOLCHAIN=${CONFIGMAP[${CONFIG}]} ${JAMOPTSTR}

