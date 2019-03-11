LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmpg123

LOCAL_C_INCLUDES := $(LOCAL_PATH)/android \
                    $(LOCAL_PATH)/src \
                    $(LOCAL_PATH)/src/compat \
                    $(LOCAL_PATH)/src/libmpg123 \

DECODER_CFLAGS_NEON := -DOPT_NEON -DREAL_IS_FLOAT

DECODER_SRC_NEON := \
    src/libmpg123/stringbuf.c \
    src/libmpg123/icy.c \
    src/libmpg123/icy2utf8.c \
    src/libmpg123/ntom.c \
    src/libmpg123/synth.c \
    src/libmpg123/synth_8bit.c \
    src/libmpg123/layer1.c \
    src/libmpg123/layer2.c \
    src/libmpg123/layer3.c \
    src/libmpg123/dct36_neon.S \
    src/libmpg123/dct64_neon_float.S \
    src/libmpg123/synth_neon_float.S \
    src/libmpg123/synth_neon_s32.S \
    src/libmpg123/synth_stereo_neon_float.S \
    src/libmpg123/synth_stereo_neon_s32.S \
    src/libmpg123/dct64_neon.S \
    src/libmpg123/synth_neon.S \
    src/libmpg123/synth_stereo_neon.S \
    src/libmpg123/synth_s32.c \
    src/libmpg123/synth_real.c \
    src/libmpg123/feature.c \

DECODER_CFLAGS_NEON64 := -DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_NEON64 -DREAL_IS_FLOAT

DECODER_SRC_NEON64 := \
    src/libmpg123/stringbuf.c \
    src/libmpg123/icy.c \
    src/libmpg123/icy2utf8.c \
    src/libmpg123/ntom.c \
    src/libmpg123/synth.c \
    src/libmpg123/synth_8bit.c \
    src/libmpg123/layer1.c \
    src/libmpg123/layer2.c \
    src/libmpg123/layer3.c \
    src/libmpg123/dct36_neon64.S \
    src/libmpg123/dct64_neon64_float.S \
    src/libmpg123/synth_neon64_float.S \
    src/libmpg123/synth_neon64_s32.S \
    src/libmpg123/synth_stereo_neon64_float.S \
    src/libmpg123/synth_stereo_neon64_s32.S \
    src/libmpg123/dct64_neon64.S \
    src/libmpg123/synth_neon64.S \
    src/libmpg123/synth_stereo_neon64.S \
    src/libmpg123/synth_s32.c \
    src/libmpg123/synth_real.c \
    src/libmpg123/dither.c \
    src/libmpg123/getcpuflags_arm.c \
    src/libmpg123/check_neon.S \
    src/libmpg123/feature.c \

# Unfortunately the assembly isn't relocatable so doesn't work on modern
# Android devices
DECODER_CFLAGS_X86 := -DOPT_GENERIC -DREAL_IS_FLOAT
DECODER_CFLAGS_X86_ASM := -DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_I386 -DOPT_I586 -DOPT_I586_DITHER -DOPT_MMX -DOPT_3DNOW -DOPT_3DNOW_VINTAGE -DOPT_3DNOWEXT -DOPT_3DNOWEXT_VINTAGE -DOPT_SSE -DOPT_SSE_VINTAGE -DREAL_IS_FLOAT

DECODER_SRC_X86 := \
    src/libmpg123/feature.c \
    src/libmpg123/icy2utf8.c \
    src/libmpg123/icy.c \
    src/libmpg123/layer1.c \
    src/libmpg123/layer2.c \
    src/libmpg123/layer3.c \
    src/libmpg123/ntom.c \
    src/libmpg123/stringbuf.c \
    src/libmpg123/synth_8bit.c \
    src/libmpg123/synth.c \
    src/libmpg123/synth_real.c \
    src/libmpg123/synth_s32.c \
    src/libmpg123/dither.c \

DECODER_SRC_X86_ASM := \
    src/libmpg123/stringbuf.c \
    src/libmpg123/icy.c \
    src/libmpg123/icy2utf8.c \
    src/libmpg123/ntom.c \
    src/libmpg123/synth.c \
    src/libmpg123/synth_8bit.c \
    src/libmpg123/layer1.c \
    src/libmpg123/layer2.c \
    src/libmpg123/layer3.c \
    src/libmpg123/synth_s32.c \
    src/libmpg123/synth_real.c \
    src/libmpg123/dct64_i386.c \
    src/libmpg123/synth_i586.S \
    src/libmpg123/synth_i586_dither.S \
    src/libmpg123/dct64_mmx.S \
    src/libmpg123/tabinit_mmx.S \
    src/libmpg123/synth_mmx.S \
    src/libmpg123/synth_3dnow.S \
    src/libmpg123/dct64_3dnow.S \
    src/libmpg123/equalizer_3dnow.S \
    src/libmpg123/dct36_3dnow.S \
    src/libmpg123/dct64_3dnowext.S \
    src/libmpg123/synth_3dnowext.S \
    src/libmpg123/dct36_3dnowext.S \
    src/libmpg123/dct64_sse_float.S \
    src/libmpg123/synth_sse_float.S \
    src/libmpg123/synth_stereo_sse_float.S \
    src/libmpg123/synth_sse_s32.S \
    src/libmpg123/synth_stereo_sse_s32.S \
    src/libmpg123/dct36_sse.S \
    src/libmpg123/dct64_sse.S \
    src/libmpg123/synth_sse.S \
    src/libmpg123/getcpuflags.S \
    src/libmpg123/dither.c \
    src/libmpg123/feature.c \

DECODER_CFLAGS_X64 := -DOPT_MULTI -DOPT_X86_64 -DOPT_GENERIC -DOPT_GENERIC_DITHER -DREAL_IS_FLOAT -DOPT_AVX

DECODER_SRC_X64 := \
    src/libmpg123/stringbuf.c \
    src/libmpg123/icy.c \
    src/libmpg123/icy.h \
    src/libmpg123/icy2utf8.c \
    src/libmpg123/icy2utf8.h \
    src/libmpg123/ntom.c \
    src/libmpg123/synth.c \
    src/libmpg123/synth.h \
    src/libmpg123/synth_8bit.c \
    src/libmpg123/synth_8bit.h \
    src/libmpg123/layer1.c \
    src/libmpg123/layer2.c \
    src/libmpg123/layer3.c \
    src/libmpg123/synth_s32.c \
    src/libmpg123/synth_real.c \
    src/libmpg123/dct36_x86_64.S \
    src/libmpg123/dct64_x86_64_float.S \
    src/libmpg123/synth_x86_64_float.S \
    src/libmpg123/synth_x86_64_s32.S \
    src/libmpg123/synth_stereo_x86_64_float.S \
    src/libmpg123/synth_stereo_x86_64_s32.S \
    src/libmpg123/synth_x86_64.S \
    src/libmpg123/dct64_x86_64.S \
    src/libmpg123/synth_stereo_x86_64.S \
    src/libmpg123/dither.c \
    src/libmpg123/dither.h \
    src/libmpg123/getcpuflags_x86_64.S \
    src/libmpg123/dct36_avx.S \
    src/libmpg123/dct64_avx_float.S \
    src/libmpg123/synth_stereo_avx_float.S \
    src/libmpg123/synth_stereo_avx_s32.S \
    src/libmpg123/dct64_avx.S \
    src/libmpg123/synth_stereo_avx.S \
    src/libmpg123/feature.c

DECODER_CFLAGS_MIPS := -DOPT_GENERIC -DREAL_IS_FLOAT

DECODER_SRC_MIPS := \
    src/libmpg123/stringbuf.c \
    src/libmpg123/icy.c \
    src/libmpg123/icy2utf8.c \
    src/libmpg123/ntom.c \
    src/libmpg123/synth.c \
    src/libmpg123/synth_8bit.c \
    src/libmpg123/layer1.c \
    src/libmpg123/layer2.c \
    src/libmpg123/layer3.c \
    src/libmpg123/synth_s32.c \
    src/libmpg123/synth_real.c \
    src/libmpg123/feature.c

ifeq ($(TARGET_ARCH_ABI),armeabi)
DECODER_CFLAGS := $(DECODER_CFLAGS_NEON)
DECODER_SRC := $(DECODER_SRC_NEON)
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
DECODER_CFLAGS := $(DECODER_CFLAGS_NEON)
DECODER_SRC := $(DECODER_SRC_NEON)
endif
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
DECODER_CFLAGS := $(DECODER_CFLAGS_NEON64)
DECODER_SRC := $(DECODER_SRC_NEON64)
endif
ifeq ($(TARGET_ARCH_ABI),x86)
DECODER_CFLAGS := $(DECODER_CFLAGS_X86)
DECODER_SRC := $(DECODER_SRC_X86)
endif
ifeq ($(TARGET_ARCH_ABI),x86_64)
DECODER_CFLAGS := $(DECODER_CFLAGS_X64)
DECODER_SRC := $(DECODER_SRC_X64)
endif
ifeq ($(TARGET_ARCH_ABI),mips)
DECODER_CFLAGS := $(DECODER_CFLAGS_MIPS)
DECODER_SRC := $(DECODER_SRC_MIPS)
endif
ifeq ($(TARGET_ARCH_ABI),mips64)
DECODER_CFLAGS := $(DECODER_CFLAGS_MIPS)
DECODER_SRC := $(DECODER_SRC_MIPS)
endif

LOCAL_CFLAGS := $(DECODER_CFLAGS)

LOCAL_SRC_FILES := \
    src/libmpg123/parse.c \
    src/libmpg123/frame.c \
    src/libmpg123/format.c \
    src/libmpg123/dct64.c \
    src/libmpg123/equalizer.c \
    src/libmpg123/id3.c \
    src/libmpg123/optimize.c \
    src/libmpg123/readers.c \
    src/libmpg123/tabinit.c \
    src/libmpg123/libmpg123.c \
    src/libmpg123/index.c \
    src/compat/compat_str.c \
    src/compat/compat.c \
    $(DECODER_SRC)


LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)

include $(BUILD_SHARED_LIBRARY)
