# GD Foxus

GD Foxus is a [Godot Engine](https://godotengine.org) plugin to decode video images
from the [Foxus](https://foxus.com/) stereo camera and handle camera properties on
the Meta Quest 2. The images can be decompressed to either RGB or YUV color format.
The result is stored in a PoolByteArray and it is emitted with the `frame_ready`
signal.

## Quick Start

1. Download Godot Engine 3.5 from [here](https://godotengine.org/download/).
2. Download the latest `gd_foxus.zip` from [here](https://github.com/cryptovoxels/gd_foxus/releases).
3. Extract the downloaded `gd_foxus.zip` to the `sample` folder.
4. Open the sample app in the Godot editor.
5. Connect the Foxus Camera to the PC.
5. Press `Play` in the top right corner or `F5` to start the application on PC.
### Run the application on Android
6. Download the export template from `Editor` > `Manage Export Templates...` > `Download and Install`
7. Install Android build template with `Project` > `Install Android Build Template ...` > `Install`
8. The application can be exported to an APK file with 
`Project` > `Export`˙> `Android (Runnable)` > `Export Project...`.

Alternatively the app may also be installed to the attached device with the Android icon in the top right corner of the editor.

```Note:``` Make sure that the Oculus Quest 2 is in Developer Mode, and it is connected over TCP to your development machine. This way the Foxus camera can be connected to the Quest 2, while the ADB connection is available to install apps, check the logs and otherwise control the device.

```Note 2:``` Currently the binary release only includes Linux and Android libraries, running on Mac requires building from source code.

## How to build

Building GD Foxus is supported on Linux and MacOS.

1. Clone this repository and initialize its submodules:
    ```
    git clone --recursive https://github.com/cryptovoxels/gd_foxus
    ```
2. Install prerequisites that are required to compile Godot Engine.
   The list of tools along with the recommended commands for each platform may
   be found in the [Godot Engine Documentation](https://docs.godotengine.org/en/stable/development/compiling/). You will also need to install the Android SDK and Android NDK.
3. Build the Godot C++ bindings:
    ```
    cd gd_foxus
    ./build_godot_bindings.sh
    ```
4. Build the GD Foxus plugin
    ```
    ./build.sh
    ```

## Testing the built plugin

The build process copies the addon to the sample project.
The sample app may be opened, exported and executed, as described in the
Quick Start section, except for the downloading / extracting of the GD Foxus release.

## Support

GD Foxus is brought to you by the [Voxels Team](https://voxels.com).

If you have any questions, don't hesitate to [Contact Us](https://www.foxus.com/pages/contact).




