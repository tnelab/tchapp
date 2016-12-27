#!/usr/bin/env bash
# This file use to build dotnet libraries
# You also need run "build_linux.sh" before this

PUBLISH_DIR_RELEASE="../publish/linux/$(uname -m)/Release"
PUBLISH_DIR_DEBUG="../publish/linux/$(uname -m)/Debug"

mkdir -p "$PUBLISH_DIR_RELEASE"
mkdir -p "$PUBLISH_DIR_DEBUG"

build() {
	oldDir="$(pwd)"
	cd "$1"
	dotnet restore
	dotnet build
	dotnet build -c "Release"
	cd "$oldDir"
	find "$1/bin/Debug/$2/" -type f -exec cp -f {} "$PUBLISH_DIR_DEBUG/" \;
	find "$1/bin/Release/$2/" -type f -exec cp -f {} "$PUBLISH_DIR_RELEASE/" \;
}
publish(){
	oldDir="$(pwd)"
	cd "$1"
	dotnet restore
	dotnet publish -r $2 -o ../../$PUBLISH_DIR_RELEASE/$1
	cd "$oldDir"
}
build "tools/TchApp.Razor" "netcoreapp1.0"
publish "tools/RazorCompiler" "ubuntu.16.04-x64"
$PUBLISH_DIR_RELEASE/tools/RazorCompiler/RazorCompiler test/tchclient

build "CLS/TchApp.TchClient" "netstandard1.6"
build "CLS/TchApp.HttpSimulator" "netstandard1.6"
build "test/tchclient" "netcoreapp1.0"
