LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -DDEVILUTION_ENGINE -DDEVILUTION_STUB -DFASTER -D_DEBUG \
 				-fms-extensions -fms-compatibility -fms-compatibility-version=19.00 \
 				-Wno-narrowing -Wno-parentheses -Wno-logical-op-parentheses -Wno-bitwise-op-parentheses \
 				-Wno-ignored-attributes -fno-omit-frame-pointer -Wno-unknown-pragmas \
 				-fpermissive -Wno-write-strings -Wno-multichar -w -fno-strict-aliasing -fsigned-char

LOCAL_CPP_FEATURES := rtti exceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../SourceS Source

LOCAL_MODULE    := devilution
LOCAL_SRC_FILES :=\
		$(LOCAL_PATH)/appfat.cpp \
		$(LOCAL_PATH)/automap.cpp \
		$(LOCAL_PATH)/capture.cpp \
		$(LOCAL_PATH)/codec.cpp \
		$(LOCAL_PATH)/control.cpp \
		$(LOCAL_PATH)/cursor.cpp \
		$(LOCAL_PATH)/dead.cpp \
		$(LOCAL_PATH)/debug.cpp \
		$(LOCAL_PATH)/diablo.cpp \
		$(LOCAL_PATH)/doom.cpp \
		$(LOCAL_PATH)/drlg_l1.cpp \
		$(LOCAL_PATH)/drlg_l2.cpp \
		$(LOCAL_PATH)/drlg_l3.cpp \
		$(LOCAL_PATH)/drlg_l4.cpp \
		$(LOCAL_PATH)/dthread.cpp \
		$(LOCAL_PATH)/effects.cpp \
		$(LOCAL_PATH)/encrypt.cpp \
		$(LOCAL_PATH)/engine.cpp \
		$(LOCAL_PATH)/error.cpp \
		$(LOCAL_PATH)/fault.cpp \
		$(LOCAL_PATH)/gamemenu.cpp \
		$(LOCAL_PATH)/gendung.cpp \
		$(LOCAL_PATH)/gmenu.cpp \
		$(LOCAL_PATH)/help.cpp \
		$(LOCAL_PATH)/init.cpp \
		$(LOCAL_PATH)/interfac.cpp \
		$(LOCAL_PATH)/inv.cpp \
		$(LOCAL_PATH)/itemdat.cpp \
		$(LOCAL_PATH)/items.cpp \
		$(LOCAL_PATH)/lighting.cpp \
		$(LOCAL_PATH)/loadsave.cpp \
		$(LOCAL_PATH)/logging.cpp \
		$(LOCAL_PATH)/mainmenu.cpp \
		$(LOCAL_PATH)/minitext.cpp \
		$(LOCAL_PATH)/misdat.cpp \
		$(LOCAL_PATH)/missiles.cpp \
		$(LOCAL_PATH)/monstdat.cpp \
		$(LOCAL_PATH)/monster.cpp \
		$(LOCAL_PATH)/movie.cpp \
		$(LOCAL_PATH)/mpqapi.cpp \
		$(LOCAL_PATH)/msgcmd.cpp \
		$(LOCAL_PATH)/msg.cpp \
		$(LOCAL_PATH)/multi.cpp \
		$(LOCAL_PATH)/nthread.cpp \
		$(LOCAL_PATH)/objdat.cpp \
		$(LOCAL_PATH)/objects.cpp \
		$(LOCAL_PATH)/pack.cpp \
		$(LOCAL_PATH)/palette.cpp \
		$(LOCAL_PATH)/path.cpp \
		$(LOCAL_PATH)/pfile.cpp \
		$(LOCAL_PATH)/player.cpp \
		$(LOCAL_PATH)/plrmsg.cpp \
		$(LOCAL_PATH)/portal.cpp \
		$(LOCAL_PATH)/spelldat.cpp \
		$(LOCAL_PATH)/quests.cpp \
		$(LOCAL_PATH)/render.cpp \
		$(LOCAL_PATH)/restrict.cpp \
		$(LOCAL_PATH)/scrollrt.cpp \
		$(LOCAL_PATH)/setmaps.cpp \
		$(LOCAL_PATH)/sha.cpp \
		$(LOCAL_PATH)/spells.cpp \
		$(LOCAL_PATH)/stores.cpp \
		$(LOCAL_PATH)/sync.cpp \
		$(LOCAL_PATH)/textdat.cpp \
		$(LOCAL_PATH)/themes.cpp \
		$(LOCAL_PATH)/tmsg.cpp \
		$(LOCAL_PATH)/town.cpp \
		$(LOCAL_PATH)/towners.cpp \
		$(LOCAL_PATH)/track.cpp \
		$(LOCAL_PATH)/trigs.cpp \
		$(LOCAL_PATH)/wave.cpp

include $(BUILD_STATIC_LIBRARY)
