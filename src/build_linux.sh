#!/usr/bin/env bash
# This file use to build native executable on linux
# You also need run "build_dotnet.sh"

CEF_SDK_DIR="../cef_sdk/cef_binary_3.2623.1399.g64e2fe1_linux64"

if [ ! -d "$CEF_SDK_DIR" ]; then
	echo "cef sdk directory does not exist, please download it first"
	exit
fi

if [ -d "$CEF_SDK_DIR/cefclient" -o \
	-d "$CEF_SDK_DIR/cefsimple" -o \
	-d "$CEF_SDK_DIR/libcef_dll" ]; then
	echo "please remove 'cefclient', 'cefsimple', 'libcef_dll' under cef sdk directory"
	exit
fi

build() {
	oldDir="$(pwd)"
	mkdir -p ../build
	cd ../build
	cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$1" ../src/native
	make -j4 tchclient
	make -j4 tchmain
	make -j4 tchsubprocess
	cd "$oldDir"
}

build "Release"
