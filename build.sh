#!/bin/bash

set -eu

build_mode="release"

basedir="$PWD"

trap "exit 1" 10
PROC="$$"

function fatal() {
    echo "$@" >&2
    kill -10 $PROC
}

ndk_version_prefix='23\.2\.'

# Find the Android SDK
function find_sdk() {
    for f in "$HOME/Android/Sdk" "$HOME/Library/Android/sdk"
    do
        if [ -d "$f" ]
        then
            echo "$f"
            return
        fi
    done
}

if [ "${ANDROID_SDK_ROOT:-}" = "" ]
then
    export ANDROID_SDK_ROOT="$(find_sdk)"
fi

# Find the latest NDK installed
function find_ndk() {
    ndk_version=$(ls -1 "$ANDROID_SDK_ROOT/ndk" | grep "^$ndk_version_prefix" | sort | tail -n 1)
    ndk_path="$ANDROID_SDK_ROOT/ndk/$ndk_version"
    if [ ! -d "$ndk_path" ]
    then
        ndk_path="/opt/android-ndk/"
        if [ ! -d "$ndk_path" ]
        then
            fatal "Unable to find Android NDK, please install it"
        fi
        echo $ndk_path
    else
        echo "$ANDROID_SDK_ROOT/ndk/$ndk_version"
    fi
}

if [ "${ANDROID_NDK_ROOT:-}" = "" ]
then
    export ANDROID_NDK_ROOT="$(find_ndk)"
fi

echo "Android SDK: $ANDROID_SDK_ROOT"
echo "Android NDK: $ANDROID_NDK_ROOT"

host_system="$(uname -s)"

case "$host_system" in
    Linux)
        host_platform="linux"
        cpus="$(nproc)"
    ;;
    Darwin)
        host_platform="osx"
        cpus="$(sysctl -n hw.logicalcpu)"
    ;;
    *)
        echo "System $host_system is unsupported"
        exit 1
    ;;
esac

host_arch="$(uname -m)"

while [ "${1:-}" != "" ]
do
    case "$1" in
    --debug)
        build_mode="debug"
        ;;
    *)
        echo "Usage: $0 [--debug]"
        exit 1
        ;;
    esac
    shift
done

scons -j$cpus platform=android target=${build_mode}

scons -j$cpus platform=${host_platform} target=${build_mode} arch=${host_arch}

if [ ! -f godot-lib.3.5.stable.release.aar ]
then
    wget -c https://downloads.tuxfamily.org/godotengine/3.5/godot-lib.3.5.stable.release.aar
fi

cd android/plugins/EiffelCamera
./gradlew assembleRelease
./gradlew assembleDebug

cd "$basedir"
mkdir -p install
cp -r addons install/
mkdir -p install/android/plugins
cp android/plugins/EiffelCamera.gdap install/android/plugins/
cp android/plugins/EiffelCamera/eiffelcamera/build/outputs/aar/*.aar "$basedir/install/android/plugins/"

cd "$basedir/install"
zip $basedir/gd_foxus.zip -r addons/ android/

cd "$basedir/sample" && unzip $basedir/gd_foxus.zip
