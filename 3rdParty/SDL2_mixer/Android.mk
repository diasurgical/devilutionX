# Save the local path
SDL_MIXER_LOCAL_PATH := $(call my-dir)


# Enable this if you want to support loading FLAC music with libFLAC
SUPPORT_FLAC ?= true
FLAC_LIBRARY_PATH := external/flac-1.3.2

# Enable this if you want to support loading OGG Vorbis music via Tremor
SUPPORT_OGG ?= true
OGG_LIBRARY_PATH := external/libogg-1.3.2
VORBIS_LIBRARY_PATH := external/libvorbisidec-1.2.1

# Enable this if you want to support loading MP3 music via MPG123
SUPPORT_MP3_MPG123 ?= true
MPG123_LIBRARY_PATH := external/mpg123-1.25.6

# Enable this if you want to support loading MOD music via modplug
SUPPORT_MOD_MODPLUG ?= true
MODPLUG_LIBRARY_PATH := external/libmodplug-0.8.9.0

# Enable this if you want to support TiMidity
SUPPORT_MID_TIMIDITY ?= true
TIMIDITY_LIBRARY_PATH := timidity


# Build the library
ifeq ($(SUPPORT_FLAC),true)
    include $(SDL_MIXER_LOCAL_PATH)/$(FLAC_LIBRARY_PATH)/Android.mk
endif

# Build the library
ifeq ($(SUPPORT_OGG),true)
    include $(SDL_MIXER_LOCAL_PATH)/$(OGG_LIBRARY_PATH)/Android.mk
    include $(SDL_MIXER_LOCAL_PATH)/$(VORBIS_LIBRARY_PATH)/Android.mk
endif

# Build the library
ifeq ($(SUPPORT_MP3_MPG123),true)
    include $(SDL_MIXER_LOCAL_PATH)/$(MPG123_LIBRARY_PATH)/Android.mk
endif

# Build the library
ifeq ($(SUPPORT_MOD_MODPLUG),true)
    include $(SDL_MIXER_LOCAL_PATH)/$(MODPLUG_LIBRARY_PATH)/Android.mk
endif

# Build the library
ifeq ($(SUPPORT_MID_TIMIDITY),true)
    include $(SDL_MIXER_LOCAL_PATH)/$(TIMIDITY_LIBRARY_PATH)/Android.mk
endif


# Restore local path
LOCAL_PATH := $(SDL_MIXER_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_mixer

LOCAL_SRC_FILES := $(notdir $(filter-out %/playmus.c %/playwave.c, $(wildcard $(LOCAL_PATH)/*.c))) \

LOCAL_CFLAGS :=
LOCAL_LDLIBS :=
LOCAL_STATIC_LIBRARIES :=
LOCAL_SHARED_LIBRARIES := SDL2

ifeq ($(SUPPORT_FLAC),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(FLAC_LIBRARY_PATH)/include
    LOCAL_CFLAGS += -DMUSIC_FLAC
    LOCAL_STATIC_LIBRARIES += libFLAC
endif

ifeq ($(SUPPORT_OGG),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(OGG_LIBRARY_PATH)/include
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(VORBIS_LIBRARY_PATH)
    LOCAL_CFLAGS += -DMUSIC_OGG -DOGG_USE_TREMOR -DOGG_HEADER="<ivorbisfile.h>"
    LOCAL_STATIC_LIBRARIES += ogg vorbisidec
endif

# This needs to be a shared library to comply with the LGPL license
ifeq ($(SUPPORT_MP3_MPG123),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(MPG123_LIBRARY_PATH)
    LOCAL_CFLAGS += -DMUSIC_MP3_MPG123
    LOCAL_SHARED_LIBRARIES += mpg123
endif

ifeq ($(SUPPORT_MOD_MODPLUG),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(MODPLUG_LIBRARY_PATH)/src
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(MODPLUG_LIBRARY_PATH)/src/libmodplug
    LOCAL_CFLAGS += -DMUSIC_MOD_MODPLUG -DMODPLUG_HEADER="<modplug.h>"
    LOCAL_STATIC_LIBRARIES += modplug
endif

ifeq ($(SUPPORT_MID_TIMIDITY),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(TIMIDITY_LIBRARY_PATH)
    LOCAL_CFLAGS += -DMUSIC_MID_TIMIDITY
    LOCAL_STATIC_LIBRARIES += timidity
endif

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)

include $(BUILD_SHARED_LIBRARY)
