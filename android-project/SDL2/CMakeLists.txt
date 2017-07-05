# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
project(SDL2)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

find_library( # Sets the name of the path variable.
        GLESv1_CM
        GLESv2
        android
        cpufeatures
        log
        OpenSLES
)

include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()

include_directories(include)

add_definitions(-DGL_GLEXT_PROTOTYPES)

add_library( # Sets the name of the library.
        SDL2

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/render/opengles/SDL_render_gles.c
        src/render/opengles2/SDL_shaders_gles2.c
        src/render/opengles2/SDL_render_gles2.c
        src/render/opengl/SDL_shaders_gl.c
        src/render/opengl/SDL_render_gl.c
        src/render/SDL_render.c
        src/render/software/SDL_blendline.c
        src/render/software/SDL_drawline.c
        src/render/software/SDL_blendfillrect.c
        src/render/software/SDL_blendpoint.c
        src/render/software/SDL_rotate.c
        src/render/software/SDL_drawpoint.c
        src/render/software/SDL_render_sw.c
        src/video/SDL_yuv.c
        src/render/SDL_yuv_sw.c
        src/render/SDL_d3dmath.c
        src/render/psp/SDL_render_psp.c
        src/audio/SDL_mixer.c
        src/audio/SDL_audiocvt.c
        src/audio/SDL_wave.c
        src/audio/dummy/SDL_dummyaudio.c
        src/audio/SDL_audiodev.c
        src/audio/android/SDL_androidaudio.c
        src/audio/paudio/SDL_paudio.c
        src/audio/SDL_audiotypecvt.c
        src/audio/SDL_audio.c
        src/audio/openslES/SDL_openslES.c
        src/core/android/SDL_android.c
        src/haptic/android/SDL_syshaptic.c
        src/cpuinfo/SDL_cpuinfo.c
        src/file/SDL_rwops.c
        src/timer/unix/SDL_systimer.c
        src/timer/SDL_timer.c
        src/test/SDL_test_assert.c
        src/test/SDL_test_crc32.c
        src/test/SDL_test_harness.c
        src/test/SDL_test_md5.c
        src/test/SDL_test_fuzzer.c
        src/test/SDL_test_random.c
        src/test/SDL_test_log.c
        src/test/SDL_test_imagePrimitives.c
        src/test/SDL_test_imagePrimitivesBlend.c
        src/test/SDL_test_compare.c
        src/test/SDL_test_imageFace.c
        src/test/SDL_test_imageBlit.c
        src/test/SDL_test_common.c
        src/test/SDL_test_font.c
        src/test/SDL_test_imageBlitBlend.c
        src/test/SDL_test_memory.c
        src/events/SDL_dropevents.c
        src/events/SDL_mouse.c
        src/events/SDL_clipboardevents.c
        src/events/SDL_windowevents.c
        src/events/SDL_keyboard.c
        src/events/SDL_gesture.c
        src/events/SDL_quit.c
        src/events/SDL_events.c
        src/events/SDL_touch.c
        src/events/SDL_displayevents.c
        src/thread/SDL_thread.c
        src/thread/pthread/SDL_syscond.c
        src/thread/pthread/SDL_systhread.c
        src/thread/pthread/SDL_sysmutex.c
        src/thread/pthread/SDL_syssem.c
        src/thread/pthread/SDL_systls.c
        src/haptic/SDL_haptic.c
        src/haptic/dummy/SDL_syshaptic.c
        src/video/SDL_video.c
        src/video/SDL_blit_0.c
        src/video/SDL_bmp.c
        src/video/SDL_blit_slow.c
        src/video/SDL_egl.c
        src/video/SDL_blit_1.c
        src/video/SDL_blit.c
        src/video/SDL_RLEaccel.c
        src/video/SDL_stretch.c
        src/video/SDL_blit_A.c
        src/video/SDL_pixels.c
        src/video/SDL_blit_N.c
        src/video/SDL_fillrect.c
        src/video/android/SDL_androidevents.c
        src/video/android/SDL_androidtouch.c
        src/video/android/SDL_androidwindow.c
        src/video/android/SDL_androidmessagebox.c
        src/video/android/SDL_androidclipboard.c
        src/video/android/SDL_androidkeyboard.c
        src/video/android/SDL_androidvideo.c
        src/video/android/SDL_androidmouse.c
        src/video/android/SDL_androidgl.c
        src/video/android/SDL_androidvulkan.c
        src/video/SDL_blit_copy.c
        src/video/SDL_clipboard.c
        src/video/SDL_blit_auto.c
        src/video/SDL_rect.c
        src/video/SDL_shape.c
        src/video/SDL_surface.c
        src/video/yuv2rgb/yuv_rgb.c
        src/loadso/dlopen/SDL_sysloadso.c
        src/dynapi/SDL_dynapi.c
        src/SDL_error.c
        src/SDL_assert.c
        src/stdlib/SDL_stdlib.c
        src/stdlib/SDL_malloc.c
        src/stdlib/SDL_string.c
        src/stdlib/SDL_getenv.c
        src/stdlib/SDL_iconv.c
        src/stdlib/SDL_qsort.c
        src/SDL.c
        src/power/android/SDL_syspower.c
        src/power/SDL_power.c
        src/atomic/SDL_spinlock.c
        src/atomic/SDL_atomic.c
        src/joystick/SDL_gamecontroller.c
        src/joystick/SDL_joystick.c
        src/joystick/android/SDL_sysjoystick.c
        src/joystick/hidapi/SDL_hidapijoystick.c
        src/joystick/hidapi/SDL_hidapi_ps4.c
        src/joystick/hidapi/SDL_hidapi_switch.c
        src/joystick/hidapi/SDL_hidapi_xbox360.c
        src/joystick/hidapi/SDL_hidapi_xboxone.c
        src/filesystem/android/SDL_sysfilesystem.c
        src/SDL_log.c
        src/SDL_hints.c
        src/SDL_dataqueue.c
        src/sensor/SDL_sensor.c
        src/video/SDL_vulkan_utils.c
        src/hidapi/android/hid.cpp
        src/sensor/android/SDL_androidsensor.c
       )


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.



# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        SDL2
        GLESv1_CM
        GLESv2
        cpufeatures
        OpenSLES
        android
        log
        # Links the target library to the log library
        # included in the NDK.
        #${log-lib}
        )