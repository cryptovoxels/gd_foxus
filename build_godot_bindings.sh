#!/bin/bash

set -eu

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

cd godot-cpp
scons platform=${host_platform} -j${cpus} generate_bindings=yes target="release"
scons platform=android -j$cpus android_arch=arm64v8 generate_bindings=yes target="release"