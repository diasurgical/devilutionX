LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libFLAC

FLAC_OGG_LIBRARY_PATH := ../libogg-1.3.2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/src/libFLAC/include \
                    $(LOCAL_PATH)/$(FLAC_OGG_LIBRARY_PATH)/include \
                    $(LOCAL_PATH)/$(FLAC_OGG_LIBRARY_PATH)/android
LOCAL_CFLAGS := -include $(LOCAL_PATH)/android/config.h

LOCAL_SRC_FILES := \
    src/libFLAC/bitmath.c \
    src/libFLAC/bitreader.c \
    src/libFLAC/bitwriter.c \
    src/libFLAC/cpu.c \
    src/libFLAC/crc.c \
    src/libFLAC/fixed.c \
    src/libFLAC/fixed_intrin_sse2.c \
    src/libFLAC/fixed_intrin_ssse3.c \
    src/libFLAC/float.c \
    src/libFLAC/format.c \
    src/libFLAC/lpc.c \
    src/libFLAC/lpc_intrin_sse.c \
    src/libFLAC/lpc_intrin_sse2.c \
    src/libFLAC/lpc_intrin_sse41.c \
    src/libFLAC/lpc_intrin_avx2.c \
    src/libFLAC/md5.c \
    src/libFLAC/memory.c \
    src/libFLAC/metadata_iterators.c \
    src/libFLAC/metadata_object.c \
    src/libFLAC/stream_decoder.c \
    src/libFLAC/stream_encoder.c \
    src/libFLAC/stream_encoder_intrin_sse2.c \
    src/libFLAC/stream_encoder_intrin_ssse3.c \
    src/libFLAC/stream_encoder_intrin_avx2.c \
    src/libFLAC/stream_encoder_framing.c \
    src/libFLAC/window.c \
    src/libFLAC/ogg_decoder_aspect.c \
    src/libFLAC/ogg_encoder_aspect.c \
    src/libFLAC/ogg_helper.c \
    src/libFLAC/ogg_mapping.c

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)
