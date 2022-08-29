
import os, sys
from xml.etree.ElementPath import ops

opts = Variables([], ARGUMENTS)

# Try to detect the host platform automatically.
# This is used if no `platform` argument is passed
if sys.platform.startswith("linux"):
    tool_prefix = "linux"
elif sys.platform == "darwin":
    tool_prefix = "darwin"
else:
    raise ValueError("Only Linux and OSX is supported")

# Gets the standard flags CC, CCX, etc.
build_dir = 'build/' + ARGUMENTS['platform']
VariantDir(build_dir, 'src', duplicate=0)

env = Environment()

target_path = 'addons/gd_foxus/'

# Define our options
opts.Add(EnumVariable('target', "Compilation target", 'debug', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", '', ['osx','android', 'linux']))
opts.Add('android_ndk_root', 'Path to your Android NDK installation.',os.environ.get("ANDROID_NDK_ROOT", None))
opts.Add(EnumVariable('p', "Compilation target, alias for 'platform'", '', ['','linux', 'osx']))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", 'yes')) #default to yes, for os x
opts.Add(PathVariable('target_path', 'The path where the lib is installed.', target_path))
opts.Add(PathVariable('target_name', 'The library name.', 'libgd_foxus', PathVariable.PathAccept))
opts.Add(PathVariable('godot_cpp', 'path to godot-cpp', './godot-cpp', PathVariable.PathAccept))
opts.Add(PathVariable('godot_headers', 'path to godot-headers', './godot-cpp/godot-headers', PathVariable.PathAccept))
opts.Add(PathVariable('prebuilts_dir', 'path to prebuilts', 'prebuilts', PathVariable.PathAccept))
opts.Add(EnumVariable('arch', '', 'x86_64', ['x86_64', 'arm64', '']))

cpp_library = "libgodot-cpp"

# only support 64 at this time..
bits = 64

if not os.path.exists(target_path):
    os.makedirs(target_path)

# Updates the environment with the option variables.
opts.Update(env)

# Process some arguments
if env['use_llvm']:
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'

if env['p'] != '':
    env['platform'] = env['p']

if env['platform'] == '':
    print("No valid target platform selected.")
    quit();

platform_string = env['platform'] + "-" + env['arch']

godot_cpp_arch = ''

# Check our platform specifics
if env['platform'] == "osx":
    arch = env['arch']
    godot_cpp_arch = '64'
    env['target_path'] += 'osx/'
    cpp_library += '.osx'
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-g','-O0', '-arch', arch, '-std=c++11'])
    else:
        env.Append(CCFLAGS = ['-O3', '-arch', arch, '-std=c++11'])

    env.Append(LINKFLAGS = ['-arch', arch,'-Wl,-framework,CoreFoundation,-framework,IOKit,-framework,Security'])

    #libuvc stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libuvc/' + platform_string + '/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libuvc/' + platform_string + '/lib/libuvc.a')])
    #libusb stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libusb/' + platform_string + '/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libusb/' + platform_string + '/lib/libusb-1.0.a')])
    #jpegturbo stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libjpeg-turbo/' + platform_string + '/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libjpeg-turbo/' + platform_string + '/lib/libturbojpeg.a')])

    env.Append(LIBS=['-lz'])

    # OpenCV
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_core.a')])

    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_photo.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_imgproc.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_ximgproc.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_videoio.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_calib3d.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_features2d.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_dnn.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_imgcodecs.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_highgui.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_video.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_gapi.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_ml.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_objdetect.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_stitching.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_flann.a')])

    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libade.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libIlmImf.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libippicv.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libippiw.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libittnotify.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibjpeg-turbo.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibopenjp2.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibpng.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibprotobuf.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibtiff.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibwebp.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libquirc.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libzlib.a')])

    env.Append(FRAMEWORKS=['OpenCL', 'Accelerate'])
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/opencv/' + platform_string + '/include/opencv4'])

elif env['platform'] == "linux":
    arch = env['arch']
    godot_cpp_arch = ''

    env['target_path'] += 'linux/'
    cpp_library += '.linux'
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-g','-O0', '-std=c++11'])
    else:
        env.Append(CCFLAGS = ['-O3', '-std=c++11'])

    env.Append(LINKFLAGS = [ "-Wl,--no-undefined", "-Wl,--start-group"])

    env.Append(LIBS=['-ludev'])
    env.Append(LIBS=['-lpthread'])
    env.Append(LIBS=['-ldl'])
    env.Append(LIBS=['-lz'])

    #libuvc stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libuvc/' + platform_string + '/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libuvc/' + platform_string + '/lib/libuvc.a')])
    #libusb stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libusb/' + platform_string + '/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libusb/' + platform_string + '/lib/libusb-1.0.a')])
    #jpegturbo stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libjpeg-turbo/' + platform_string + '/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libjpeg-turbo/' + platform_string + '/lib64/libturbojpeg.a')])

    # OpenCV
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_core.a')])

    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_photo.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_imgproc.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_ximgproc.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_videoio.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_calib3d.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_features2d.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_dnn.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_imgcodecs.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_highgui.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_video.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_gapi.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_ml.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_objdetect.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_stitching.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/libopencv_flann.a')])

    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libade.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libIlmImf.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libippicv.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libippiw.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libittnotify.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibjpeg-turbo.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibopenjp2.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibpng.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibprotobuf.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibtiff.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/liblibwebp.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libquirc.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/' + platform_string + '/lib/opencv4/3rdparty/libzlib.a')])

    env.Append(CPPPATH=[env['prebuilts_dir'] + '/opencv/' + platform_string + '/include/opencv4'])
elif env['platform'] == "android":
    env['target_path'] += 'android/'
    cpp_library += '.android'
    # Verify NDK root
    if not 'android_ndk_root' in env:
        raise ValueError("To build for Android, ANDROID_NDK_ROOT must be defined. Please set ANDROID_NDK_ROOT to the root folder of your Android NDK installation.")

    # Validate API level
    api_level='29'
    arch = 'arm64v8'
    godot_cpp_arch = arch

    arch_info = {"arch_path":"arm64-v8a","march":"armv8-a", "target":"aarch64-linux-android", "tool_path":"aarch64-linux-android", "compiler_path":"aarch64-linux-android", "ccflags" : []}
    # Setup toolchain

    env['arch_path'] = "/" + arch_info['arch_path']+"/"
    toolchain = env['android_ndk_root'] + "/toolchains/llvm/prebuilt/" + tool_prefix + "-x86_64/"
    env.PrependENVPath('PATH', toolchain + "/bin") # This does nothing half of the time, but we'll put it here anyways

    env['CC'] = toolchain + "/bin/" + arch_info['compiler_path'] + api_level + "-clang"
    env['CXX'] = toolchain + "/bin/" + arch_info['compiler_path'] + api_level + "-clang++"
    env['AR'] = toolchain + "/bin/" + arch_info['tool_path'] + "-ar"

    env.Append(CCFLAGS=['--target=' + arch_info['target'] + api_level, '-march=' + arch_info['march'], '-fPIC', '-Wno-c++11-narrowing'])

    # if env['target'] in ('release', 'r'):
    env.Append(CCFLAGS = ['-O3']) # , '-DENABLE_NEON=ON',"-DENABLE_VFPV3=ON"])

    env.Append(CCFLAGS=arch_info['ccflags'])
    env.Append(LINKFLAGS=['-shared', '-static-libstdc++', "-Wl,--no-undefined", "-Wl,--start-group"])

    # libuvc stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libuvc/android/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libuvc/android/lib/arm64-v8a/libuvc.a')])
    # libusb stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libusb/android/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libusb/android/lib/arm64-v8a/libusb1.0.a')])
    # jpegturbo stuff
    env.Append(CPPPATH=[env['prebuilts_dir'] + '/libjpeg-turbo/android/include'])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/libjpeg-turbo/android/lib/libturbojpeg.a')])
    env.Append(LIBS=['-llog'])
    env.Append(LIBS=['-lz'])

    # OpenCV
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_core.a')])

    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_photo.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_imgproc.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_ximgproc.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_videoio.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_calib3d.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_features2d.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_dnn.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_imgcodecs.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_highgui.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_video.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_gapi.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_ml.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_objdetect.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_stitching.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/staticlibs/arm64-v8a/libopencv_flann.a')])

    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/liblibtiff.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/liblibpng.a')])
    #env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/libtbb.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/liblibwebp.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/libquirc.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/liblibopenjp2.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/liblibprotobuf.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/libIlmImf.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/libade.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/libcpufeatures.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/libittnotify.a')])
    env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/libtegra_hal.a')])
    # env.Append(LIBS=[File(env['prebuilts_dir'] + '/opencv/android/sdk/native/3rdparty/libs/arm64-v8a/liblibjpeg-turbo.a')])

    env.Append(CPPPATH=[env['prebuilts_dir'] + '/opencv/android/sdk/native/jni/include'])
    # AddPostAction(copyIntoPlace, postBuildFunction)

if env['target'] in ('debug', 'd'):
    cpp_library += '.debug'
else:
    cpp_library += '.release'

if godot_cpp_arch != '' and env['platform'] == "osx" and env['arch'] == "arm64":
    cpp_library += '.arm' + godot_cpp_arch
elif godot_cpp_arch != '':
    cpp_library += '.' + godot_cpp_arch

if env['platform'] == 'linux':
    cpp_library += '.' + str(bits)

# make sure our binding library is properly includes
env.Append(CPPPATH=[
    '.',
    env['godot_headers'],
    env['godot_cpp'] + '/include/',
    env['godot_cpp'] + '/include/core/',
    env['godot_cpp'] + '/include/gen/'
])

env.Append(LIBPATH=[
    env['godot_cpp'] + '/bin/',
])

env.Append(LIBS=[cpp_library])

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=[build_dir])
sources = [Glob(build_dir + '/*.cpp')]

library = env.SharedLibrary(target=env['target_path'] + env['target_name'] , source=sources)

if env['platform'] == "android":
    library= env.SharedLibrary(target=env['target_path'] + env['arch_path'] + env['target_name'] + ".so" , source=sources)

Default(library)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))
