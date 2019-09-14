LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS :=

LOCAL_MODULE    := stormlib
LOCAL_SRC_FILES :=\
		  $(LOCAL_PATH)/src/FileStream.cpp \
		  $(LOCAL_PATH)/src/SBaseCommon.cpp \
		  $(LOCAL_PATH)/src/SBaseFileTable.cpp \
		  $(LOCAL_PATH)/src/SBaseSubTypes.cpp \
		  $(LOCAL_PATH)/src/SCompression.cpp \
		  $(LOCAL_PATH)/src/SFileExtractFile.cpp \
		  $(LOCAL_PATH)/src/SFileFindFile.cpp \
		  $(LOCAL_PATH)/src/SFileGetFileInfo.cpp \
		  $(LOCAL_PATH)/src/SFileOpenArchive.cpp \
		  $(LOCAL_PATH)/src/SFileOpenFileEx.cpp \
		  $(LOCAL_PATH)/src/SFileReadFile.cpp \

include $(BUILD_STATIC_LIBRARY)
