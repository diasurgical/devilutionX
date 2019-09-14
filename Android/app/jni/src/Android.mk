LOCAL_PATH := $(call my-dir)

##########################
## Sodium
##########################

LIB_FOLDER := lib
# Bugfix for arm, which should refer to the armv6 folder
# Bugfix for x86, which should refer to the i686 folder
MY_ARCH_FOLDER := $(TARGET_ARCH)
ifeq ($(MY_ARCH_FOLDER),arm)
    MY_ARCH_FOLDER = armv6
endif
ifeq ($(MY_ARCH_FOLDER),arm64)
    MY_ARCH_FOLDER = armv8-a
endif
ifeq ($(MY_ARCH_FOLDER),x86)
    MY_ARCH_FOLDER = i686
endif
ifeq ($(MY_ARCH_FOLDER),x86_64)
    MY_ARCH_FOLDER = westmere
endif

include $(CLEAR_VARS)
LOCAL_MODULE := sodium
LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH))/../../../../libs/libsodium-1.0.17/libsodium-android-$(MY_ARCH_FOLDER)/$(LIB_FOLDER)/libsodium.a #/installs/libsodium/libsodium-android-(x86|arm)/lib/libsodium.a
LOCAL_LDFLAGS  += -fPIC
#LOCAL_LDLIBS   += -Wl,--no-warn-shared-textrel
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true
include $(PREBUILT_STATIC_LIBRARY)


##########################
## DevilutionX
##########################

include $(CLEAR_VARS)

LOCAL_CFLAGS := -DDEVILUTION_STUB -DASIO_STANDALONE \
 				-fms-extensions -fms-compatibility -fms-compatibility-version=19.00 \
 				-Wno-narrowing -Wno-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses \
 				-Wno-ignored-attributes -Wall -Wextra -Wno-write-strings -Wno-multichar -Wno-unused-parameter \
 				-static-libgcc -static-libstdc++ -fsigned-char -fno-strict-aliasing -fpermissive -Wno-unknown-pragmas \
 				-D_DEBUG

LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../SourceS $(LOCAL_PATH)/../3rdParty/Radon/Radon/include \
					$(LOCAL_PATH)/../3rdParty/libsmacker $(LOCAL_PATH)/$(SDL_PATH)/include \
					$(LOCAL_PATH)/../SDLMixer \
					$(LOCAL_PATH)/../SourceX $(LOCAL_PATH)/../Source \
					$(LOCAL_PATH)/../../../../3rdParty/asio/include \
					$(abspath $(LOCAL_PATH))/../../../../libs/libsodium-1.0.17/libsodium-android-$(MY_ARCH_FOLDER)/include

LOCAL_MODULE := main
LOCAL_SRC_FILES :=\
	$(LOCAL_PATH)/../SourceX/dx.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/misc.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/misc_io.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/misc_msg.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/misc_dx.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/rand.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/thread.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/dsound.cpp \
	$(LOCAL_PATH)/../SourceX/miniwin/ddraw.cpp \
	$(LOCAL_PATH)/../SourceX/sound.cpp \
	$(LOCAL_PATH)/../SourceX/storm/storm.cpp \
	$(LOCAL_PATH)/../SourceX/storm/storm_net.cpp \
	$(LOCAL_PATH)/../SourceX/storm/storm_dx.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/abstract_net.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/loopback.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/packet.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/base.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/frame_queue.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/tcp_client.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/tcp_server.cpp \
	$(LOCAL_PATH)/../SourceX/dvlnet/udp_p2p.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/credits.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/diabloui.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/dialogs.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/mainmenu.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/progress.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/selconn.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/selgame.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/selhero.cpp \
	$(LOCAL_PATH)/../SourceX/DiabloUI/title.cpp \
	$(LOCAL_PATH)/../SourceX/main.cpp

LOCAL_STATIC_LIBRARIES := devilution pkware stormlib libsmacker \
						 libradon SDL2 SDL2_ttf SDL2_mixer sodium


LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -pthread -lz

include $(BUILD_SHARED_LIBRARY)
