#!/bin/sh
if [[ $1 == *android-ndk-* ]]; then
	echo "----------------- NDK Path is : $1 ----------------"
	Input=$1;
else
	echo "Please enter your android ndk path:"
	echo "For example:/home/astro/android-ndk-r8e"
	read Input
	echo "You entered:$Input"

	echo "----------------- Exporting the android-ndk path ----------------"
fi

#Set path
export PATH=$PATH:$Input:$Input/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin

#create install directories
mkdir -p ./../../../build
mkdir -p ./../../../build/android

#udns module build
echo "------ Building udns v0.2 for ANDROID platform ------"
pushd `pwd`
mkdir -p ./../../../build/android/udns

rm -rf ./obj/*
export NDK_PROJECT_PATH=`pwd`
ndk-build APP_PLATFORM=android-9
popd

echo "-------- Installing udns libs -----"
cp -r ./obj/local/armeabi/lib* ./../../../build/android/udns/

#clean
rm -rf ./obj