ifeq ($(strip $(DEVKITPRO)),)
	$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

contents := $(shell mkdir -p obj ; mkdir -p release ; mkdir -p RomFS)

TOPDIR ?= $(CURDIR)

export BUILD_EXEFS_SRC := build/exefs

include $(DEVKITPRO)/libnx/switch_rules

APP_TITLE 	:= DevilutionX
APP_VERSION	:= 0.5.0
APP_AUTHOR	:= Devilution Team
ICON 		:= switch/icon.jpg
BUILD		:= build
DATA		:= data
INCLUDES	:= include
EXEFS_SRC	:= exefs_src
ROMFS		:= RomFS


BINDIR		= release
OUTPUT    	= devilutionx

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif


ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif


ifeq ($(TARGET),)
TARGET = diablo
endif

# compiler, linker and utilities
AR 			= aarch64-none-elf-gcc-ar
CC 			= aarch64-none-elf-gcc
CPP 		= aarch64-none-elf-g++
LINK 		= aarch64-none-elf-g++
ASM 		= @nasm
ASMFLAGS 	= -f coff
MD 			= -mkdir
RM 			= @rm -f

SMACKEROBJ 		= obj/smk_bitstream.o obj/smk_hufftree.o obj/smacker.o
RADONOBJ		= obj/File.o obj/Key.o obj/Named.o obj/Section.o
STORMLIBOBJ 	= obj/FileStream.o obj/SBaseCommon.o obj/SBaseFileTable.o obj/SBaseSubTypes.o obj/SCompression.o obj/SFileExtractFile.o obj/SFileFindFile.o obj/SFileGetFileInfo.o obj/SFileOpenArchive.o obj/SFileOpenFileEx.o obj/SFileReadFile.o
PKWAREOBJ		= obj/explode.o obj/implode.o
DEVILUTIONOBJ 	= obj/appfat.o obj/automap.o obj/capture.o obj/codec.o obj/control.o obj/cursor.o obj/dead.o obj/debug.o obj/diablo.o obj/doom.o obj/drlg_l1.o obj/drlg_l2.o obj/drlg_l3.o obj/drlg_l4.o obj/dthread.o obj/effects.o obj/encrypt.o obj/engine.o obj/error.o obj/gamemenu.o obj/gendung.o obj/gmenu.o obj/help.o obj/init.o obj/interfac.o obj/inv.o obj/itemdat.o obj/items.o obj/lighting.o obj/loadsave.o obj/logging.o obj/mmainmenu.o obj/minitext.o obj/misdat.o obj/missiles.o obj/monstdat.o obj/monster.o obj/movie.o obj/mpqapi.o obj/msgcmd.o obj/msg.o obj/multi.o obj/nthread.o obj/objdat.o obj/objects.o obj/pack.o obj/palette.o obj/path.o obj/pfile.o obj/player.o obj/plrctrls.o obj/plrmsg.o obj/portal.o obj/spelldat.o obj/quests.o obj/render.o obj/restrict.o obj/scrollrt.o obj/setmaps.o obj/sha.o obj/spells.o obj/stores.o obj/sync.o obj/textdat.o obj/themes.o obj/tmsg.o obj/town.o obj/towners.o obj/track.o obj/trigs.o obj/wave.o
MAINOBJ			= obj/dx.o obj/misc.o obj/misc_io.o obj/misc_msg.o obj/misc_dx.o obj/rand.o obj/thread.o obj/dsound.o obj/sound.o obj/storm.o obj/storm_net.o obj/storm_dx.o obj/abstract_net.o obj/loopback.o obj/packet.o obj/base.o obj/frame_queue.o obj/credits.o obj/diabloui.o obj/dialogs.o obj/mainmenu.o obj/progress.o obj/selconn.o obj/selgame.o obj/selhero.o obj/selyesno.o obj/title.o obj/main.o obj/switch_keyboard.o

LIBS      	= -specs=$(DEVKITPRO)/libnx/switch.specs -g -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -L$(DEVKITPRO)/libnx/lib -L$(DEVKITPRO)/portlibs/switch/lib -lSDL2_mixer -lSDL2_ttf -lfreetype -lvorbisfile -lvorbis -logg -lmodplug -lmikmod -lmpg123 -lSDL2 -lopusfile -lopus -lEGL -lglapi -ldrm_nouveau -lpng -lbz2 -lz -lnx
INCS      	= -I$(DEVKITPRO)/portlibs/switch/include/SDL2 -I"Source" -I"SourceS" -I"SourceX" -I"3rdParty/asio/include" -I"3rdParty/Radon/Radon/include" -I"3rdParty/libsmacker" -I$(DEVKITPRO)/libnx/include -I$(DEVKITPRO)/portlibs/switch/include
CXXINCS   	= -I$(DEVKITPRO)/portlibs/switch/include/SDL2 -I"Source" -I"SourceS" -I"SourceX" -I"3rdParty/asio/include" -I"3rdParty/Radon/Radon/include" -I"3rdParty/libsmacker" -I$(DEVKITPRO)/libnx/include -I$(DEVKITPRO)/portlibs/switch/include
BIN       	= release/diablo-nx.elf
BUILD	  	= build
BINDIR	  	= release
DEFINES   	= -DSWITCH -DPLATFORM_NX -DSDL2 -DDEVILUTION_STUB -DDEVILUTION_ENGINE -DASIO_STANDALONE -DASIO_HEADER_ONLY -DNONET
CXXFLAGS  	= $(CXXINCS) $(DEFINES) -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -fsigned-char -Wall -Wextra -Wno-write-strings -fpermissive -Wno-write-strings -Wno-multichar -w -O2
CFLAGS    	= $(INCS) $(DEFINES)    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -fsigned-char -Wall -Wextra -Wno-write-strings -fpermissive -Wno-write-strings -Wno-multichar -w -O2
GPROF     	= gprof.exe
RM        	= rm -f
OUTPUT    	= diablo-nx

.PHONY: all all-before all-after clean clean-custom
all: all-before $(BIN) all-after

clean: clean-custom
	$(RM) $(MAINOBJ) $(SMACKEROBJ) $(RADONOBJ) $(STORMLIBOBJ) $(PKWAREOBJ) $(DEVILUTIONOBJ) $(BIN)


$(BIN): $(MAINOBJ) $(SMACKEROBJ) $(RADONOBJ) $(STORMLIBOBJ) $(PKWAREOBJ) $(DEVILUTIONOBJ)
	$(LINK) $(MAINOBJ) $(SMACKEROBJ) $(RADONOBJ) $(STORMLIBOBJ) $(PKWAREOBJ) $(DEVILUTIONOBJ) -o "release/diablo-nx.elf" $(LIBS)

#Smacker

obj/smk_bitstream.o: $(GLOBALDEPS) 3rdParty/libsmacker/smk_bitstream.c
	$(CC) -c 3rdParty/libsmacker/smk_bitstream.c -o obj/smk_bitstream.o $(CFLAGS)
obj/smk_hufftree.o: $(GLOBALDEPS) 3rdParty/libsmacker/smk_hufftree.c
	$(CC) -c 3rdParty/libsmacker/smk_hufftree.c -o obj/smk_hufftree.o $(CFLAGS)
obj/smacker.o: $(GLOBALDEPS) 3rdParty/libsmacker/smacker.c
	$(CC) -c 3rdParty/libsmacker/smacker.c -o obj/smacker.o $(CFLAGS)

#Radon

obj/File.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/File.cpp
	$(CPP) -c 3rdParty/Radon/Radon/source/File.cpp -o obj/File.o $(CXXFLAGS)
obj/Key.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/Key.cpp
	$(CPP) -c 3rdParty/Radon/Radon/source/Key.cpp -o obj/Key.o $(CXXFLAGS)
obj/Named.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/Named.cpp
	$(CPP) -c 3rdParty/Radon/Radon/source/Named.cpp -o obj/Named.o $(CXXFLAGS)
obj/Section.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/Section.cpp
	$(CPP) -c 3rdParty/Radon/Radon/source/Section.cpp -o obj/Section.o $(CXXFLAGS)

#StormLib

obj/FileStream.o: $(GLOBALDEPS) 3rdParty/StormLib/src/FileStream.cpp
	$(CPP) -c 3rdParty/StormLib/src/FileStream.cpp -o obj/FileStream.o $(CXXFLAGS)
obj/SBaseCommon.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SBaseCommon.cpp
	$(CPP) -c 3rdParty/StormLib/src/SBaseCommon.cpp -o obj/SBaseCommon.o $(CXXFLAGS)
obj/SBaseFileTable.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SBaseFileTable.cpp
	$(CPP) -c 3rdParty/StormLib/src/SBaseFileTable.cpp -o obj/SBaseFileTable.o $(CXXFLAGS)
obj/SBaseSubTypes.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SBaseSubTypes.cpp
	$(CPP) -c 3rdParty/StormLib/src/SBaseSubTypes.cpp -o obj/SBaseSubTypes.o $(CXXFLAGS)
obj/SCompression.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SCompression.cpp
	$(CPP) -c 3rdParty/StormLib/src/SCompression.cpp -o obj/SCompression.o $(CXXFLAGS)
obj/SFileExtractFile.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileExtractFile.cpp
	$(CPP) -c 3rdParty/StormLib/src/SFileExtractFile.cpp -o obj/SFileExtractFile.o $(CXXFLAGS)
obj/SFileFindFile.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileFindFile.cpp
	$(CPP) -c 3rdParty/StormLib/src/SFileFindFile.cpp -o obj/SFileFindFile.o $(CXXFLAGS)
obj/SFileGetFileInfo.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileGetFileInfo.cpp
	$(CPP) -c 3rdParty/StormLib/src/SFileGetFileInfo.cpp -o obj/SFileGetFileInfo.o $(CXXFLAGS)
obj/SFileOpenArchive.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileOpenArchive.cpp
	$(CPP) -c 3rdParty/StormLib/src/SFileOpenArchive.cpp -o obj/SFileOpenArchive.o $(CXXFLAGS)
obj/SFileOpenFileEx.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileOpenFileEx.cpp
	$(CPP) -c 3rdParty/StormLib/src/SFileOpenFileEx.cpp -o obj/SFileOpenFileEx.o $(CXXFLAGS)
obj/SFileReadFile.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileReadFile.cpp
	$(CPP) -c 3rdParty/StormLib/src/SFileReadFile.cpp -o obj/SFileReadFile.o $(CXXFLAGS)

#PKWare

obj/explode.o: $(GLOBALDEPS) 3rdParty/PKWare/explode.cpp
	$(CPP) -c 3rdParty/PKWare/explode.cpp -o obj/explode.o $(CXXFLAGS)
obj/implode.o: $(GLOBALDEPS) 3rdParty/PKWare/implode.cpp
	$(CPP) -c 3rdParty/PKWare/implode.cpp -o obj/implode.o $(CXXFLAGS)

#Devilution
obj/appfat.o: $(GLOBALDEPS) Source/appfat.cpp
	$(CPP) -c Source/appfat.cpp -o obj/appfat.o $(CXXFLAGS)
obj/automap.o: $(GLOBALDEPS) Source/automap.cpp
	$(CPP) -c Source/automap.cpp -o obj/automap.o $(CXXFLAGS)
obj/capture.o: $(GLOBALDEPS)  Source/capture.cpp
	$(CPP) -c  Source/capture.cpp -o obj/capture.o $(CXXFLAGS)
obj/codec.o: $(GLOBALDEPS) Source/codec.cpp
	$(CPP) -c Source/codec.cpp -o obj/codec.o $(CXXFLAGS)
obj/control.o: $(GLOBALDEPS) Source/control.cpp
	$(CPP) -c Source/control.cpp -o obj/control.o $(CXXFLAGS)
obj/cursor.o: $(GLOBALDEPS) Source/cursor.cpp
	$(CPP) -c Source/cursor.cpp -o obj/cursor.o $(CXXFLAGS)
obj/dead.o: $(GLOBALDEPS) Source/dead.cpp
	$(CPP) -c Source/dead.cpp -o obj/dead.o $(CXXFLAGS)
obj/debug.o: $(GLOBALDEPS) Source/debug.cpp
	$(CPP) -c Source/debug.cpp -o obj/debug.o $(CXXFLAGS)
obj/diablo.o: $(GLOBALDEPS) Source/diablo.cpp
	$(CPP) -c Source/diablo.cpp -o obj/diablo.o $(CXXFLAGS)
obj/doom.o: $(GLOBALDEPS) Source/doom.cpp
	$(CPP) -c Source/doom.cpp -o obj/doom.o $(CXXFLAGS)
obj/drlg_l1.o: $(GLOBALDEPS)  Source/drlg_l1.cpp
	$(CPP) -c  Source/drlg_l1.cpp -o obj/drlg_l1.o $(CXXFLAGS)
obj/drlg_l2.o: $(GLOBALDEPS) Source/drlg_l2.cpp
	$(CPP) -c Source/drlg_l2.cpp -o obj/drlg_l2.o $(CXXFLAGS)
obj/drlg_l3.o: $(GLOBALDEPS) Source/drlg_l3.cpp
	$(CPP) -c Source/drlg_l3.cpp -o obj/drlg_l3.o $(CXXFLAGS)
obj/drlg_l4.o: $(GLOBALDEPS) Source/drlg_l4.cpp
	$(CPP) -c Source/drlg_l4.cpp -o obj/drlg_l4.o $(CXXFLAGS)
obj/dthread.o: $(GLOBALDEPS) Source/dthread.cpp
	$(CPP) -c Source/dthread.cpp -o obj/dthread.o $(CXXFLAGS)
obj/effects.o: $(GLOBALDEPS) Source/effects.cpp
	$(CPP) -c Source/effects.cpp -o obj/effects.o $(CXXFLAGS)
obj/encrypt.o: $(GLOBALDEPS)  Source/encrypt.cpp
	$(CPP) -c  Source/encrypt.cpp -o obj/encrypt.o $(CXXFLAGS)
obj/engine.o: $(GLOBALDEPS) Source/engine.cpp
	$(CPP) -c Source/engine.cpp -o obj/engine.o $(CXXFLAGS)
obj/error.o: $(GLOBALDEPS) Source/error.cpp
	$(CPP) -c Source/error.cpp -o obj/error.o $(CXXFLAGS)
obj/gamemenu.o: $(GLOBALDEPS)  Source/gamemenu.cpp
	$(CPP) -c  Source/gamemenu.cpp -o obj/gamemenu.o $(CXXFLAGS)
obj/gendung.o: $(GLOBALDEPS)  Source/gendung.cpp
	$(CPP) -c  Source/gendung.cpp -o obj/gendung.o $(CXXFLAGS)
obj/gmenu.o: $(GLOBALDEPS)  Source/gmenu.cpp
	$(CPP) -c  Source/gmenu.cpp -o obj/gmenu.o $(CXXFLAGS)
obj/help.o: $(GLOBALDEPS)  Source/help.cpp
	$(CPP) -c  Source/help.cpp -o obj/help.o $(CXXFLAGS)
obj/init.o: $(GLOBALDEPS)  Source/init.cpp
	$(CPP) -c  Source/init.cpp -o obj/init.o $(CXXFLAGS)
obj/interfac.o: $(GLOBALDEPS)  Source/interfac.cpp
	$(CPP) -c  Source/interfac.cpp -o obj/interfac.o $(CXXFLAGS)
obj/inv.o: $(GLOBALDEPS)  Source/inv.cpp
	$(CPP) -c  Source/inv.cpp -o obj/inv.o $(CXXFLAGS)
obj/itemdat.o: $(GLOBALDEPS)  Source/itemdat.cpp
	$(CPP) -c  Source/itemdat.cpp -o obj/itemdat.o $(CXXFLAGS)
obj/items.o: $(GLOBALDEPS)  Source/items.cpp
	$(CPP) -c  Source/items.cpp -o obj/items.o $(CXXFLAGS)
obj/lighting.o: $(GLOBALDEPS)  Source/lighting.cpp
	$(CPP) -c  Source/lighting.cpp -o obj/lighting.o $(CXXFLAGS)
obj/loadsave.o: $(GLOBALDEPS)  Source/loadsave.cpp
	$(CPP) -c  Source/loadsave.cpp -o obj/loadsave.o $(CXXFLAGS)
obj/mmainmenu.o: $(GLOBALDEPS)  Source/mainmenu.cpp
	$(CPP) -c  Source/mainmenu.cpp -o obj/mmainmenu.o $(CXXFLAGS)
obj/minitext.o: $(GLOBALDEPS)  Source/minitext.cpp
	$(CPP) -c  Source/minitext.cpp -o obj/minitext.o $(CXXFLAGS)
obj/misdat.o: $(GLOBALDEPS)  Source/misdat.cpp
	$(CPP) -c  Source/misdat.cpp -o obj/misdat.o $(CXXFLAGS)
obj/missiles.o: $(GLOBALDEPS)  Source/missiles.cpp
	$(CPP) -c  Source/missiles.cpp -o obj/missiles.o $(CXXFLAGS)
obj/monstdat.o: $(GLOBALDEPS)  Source/monstdat.cpp
	$(CPP) -c  Source/monstdat.cpp -o obj/monstdat.o $(CXXFLAGS)
obj/monster.o: $(GLOBALDEPS)  Source/monster.cpp
	$(CPP) -c  Source/monster.cpp -o obj/monster.o $(CXXFLAGS)
obj/movie.o: $(GLOBALDEPS)  Source/movie.cpp
	$(CPP) -c  Source/movie.cpp -o obj/movie.o $(CXXFLAGS)
obj/mpqapi.o: $(GLOBALDEPS)  Source/mpqapi.cpp
	$(CPP) -c  Source/mpqapi.cpp -o obj/mpqapi.o $(CXXFLAGS)
obj/msg.o: $(GLOBALDEPS)  Source/msg.cpp
	$(CPP) -c  Source/msg.cpp -o obj/msg.o $(CXXFLAGS)
obj/multi.o: $(GLOBALDEPS)  Source/multi.cpp
	$(CPP) -c  Source/multi.cpp -o obj/multi.o $(CXXFLAGS)
obj/nthread.o: $(GLOBALDEPS)  Source/nthread.cpp
	$(CPP) -c  Source/nthread.cpp -o obj/nthread.o $(CXXFLAGS)
obj/objdat.o: $(GLOBALDEPS)  Source/objdat.cpp
	$(CPP) -c  Source/objdat.cpp -o obj/objdat.o $(CXXFLAGS)
obj/objects.o: $(GLOBALDEPS)  Source/objects.cpp
	$(CPP) -c  Source/objects.cpp -o obj/objects.o $(CXXFLAGS)
obj/pack.o: $(GLOBALDEPS)  Source/pack.cpp
	$(CPP) -c  Source/pack.cpp -o obj/pack.o $(CXXFLAGS)
obj/palette.o: $(GLOBALDEPS)  Source/palette.cpp
	$(CPP) -c  Source/palette.cpp -o obj/palette.o $(CXXFLAGS)
obj/path.o: $(GLOBALDEPS)  Source/path.cpp
	$(CPP) -c  Source/path.cpp -o obj/path.o $(CXXFLAGS)
obj/pfile.o: $(GLOBALDEPS)  Source/pfile.cpp
	$(CPP) -c  Source/pfile.cpp -o obj/pfile.o $(CXXFLAGS)
obj/player.o: $(GLOBALDEPS)  Source/player.cpp
	$(CPP) -c  Source/player.cpp -o obj/player.o $(CXXFLAGS)
obj/plrmsg.o: $(GLOBALDEPS)  Source/plrmsg.cpp
	$(CPP) -c  Source/plrmsg.cpp -o obj/plrmsg.o $(CXXFLAGS)
obj/plrctrls.o: $(GLOBALDEPS) Source/plrctrls.cpp
	$(CPP) -c Source/plrctrls.cpp -o obj/plrctrls.o $(CXXFLAGS)
obj/portal.o: $(GLOBALDEPS)  Source/portal.cpp
	$(CPP) -c  Source/portal.cpp -o obj/portal.o $(CXXFLAGS)
obj/spelldat.o: $(GLOBALDEPS)  Source/spelldat.cpp
	$(CPP) -c  Source/spelldat.cpp -o obj/spelldat.o $(CXXFLAGS)
obj/quests.o: $(GLOBALDEPS)  Source/quests.cpp
	$(CPP) -c  Source/quests.cpp -o obj/quests.o $(CXXFLAGS)
obj/render.o: $(GLOBALDEPS)  Source/render.cpp
	$(CPP) -c  Source/render.cpp -o obj/render.o $(CXXFLAGS)
obj/restrict.o: $(GLOBALDEPS)  Source/restrict.cpp
	$(CPP) -c  Source/restrict.cpp -o obj/restrict.o $(CXXFLAGS)
obj/scrollrt.o: $(GLOBALDEPS)  Source/scrollrt.cpp
	$(CPP) -c  Source/scrollrt.cpp -o obj/scrollrt.o $(CXXFLAGS)
obj/setmaps.o: $(GLOBALDEPS)  Source/setmaps.cpp
	$(CPP) -c  Source/setmaps.cpp -o obj/setmaps.o $(CXXFLAGS)
obj/sha.o: $(GLOBALDEPS)  Source/sha.cpp
	$(CPP) -c  Source/sha.cpp -o obj/sha.o $(CXXFLAGS)
obj/spells.o: $(GLOBALDEPS)  Source/spells.cpp
	$(CPP) -c  Source/spells.cpp -o obj/spells.o $(CXXFLAGS)
obj/stores.o: $(GLOBALDEPS)  Source/stores.cpp
	$(CPP) -c  Source/stores.cpp -o obj/stores.o $(CXXFLAGS)
obj/sync.o: $(GLOBALDEPS)  Source/sync.cpp
	$(CPP) -c  Source/sync.cpp -o obj/sync.o $(CXXFLAGS)
obj/textdat.o: $(GLOBALDEPS)  Source/textdat.cpp
	$(CPP) -c  Source/textdat.cpp -o obj/textdat.o $(CXXFLAGS)
obj/themes.o: $(GLOBALDEPS)  Source/themes.cpp
	$(CPP) -c  Source/themes.cpp -o obj/themes.o $(CXXFLAGS)
obj/tmsg.o: $(GLOBALDEPS)  Source/tmsg.cpp
	$(CPP) -c  Source/tmsg.cpp -o obj/tmsg.o $(CXXFLAGS)
obj/town.o: $(GLOBALDEPS)  Source/town.cpp
	$(CPP) -c  Source/town.cpp -o obj/town.o $(CXXFLAGS)
obj/towners.o: $(GLOBALDEPS)  Source/towners.cpp
	$(CPP) -c  Source/towners.cpp -o obj/towners.o $(CXXFLAGS)
obj/track.o: $(GLOBALDEPS)  Source/track.cpp
	$(CPP) -c  Source/track.cpp -o obj/track.o $(CXXFLAGS)
obj/trigs.o: $(GLOBALDEPS)  Source/trigs.cpp
	$(CPP) -c  Source/trigs.cpp -o obj/trigs.o $(CXXFLAGS)
obj/wave.o: $(GLOBALDEPS)  Source/wave.cpp
	$(CPP) -c  Source/wave.cpp -o obj/wave.o $(CXXFLAGS)

#Main
obj/dx.o: $(GLOBALDEPS) SourceX/dx.cpp
	$(CPP) -c SourceX/dx.cpp -o obj/dx.o $(CXXFLAGS)
obj/misc.o: $(GLOBALDEPS) SourceX/miniwin/misc.cpp
	$(CPP) -c SourceX/miniwin/misc.cpp -o obj/misc.o $(CXXFLAGS)
obj/misc_io.o: $(GLOBALDEPS) SourceX/miniwin/misc_io.cpp
	$(CPP) -c SourceX/miniwin/misc_io.cpp -o obj/misc_io.o $(CXXFLAGS)
obj/misc_msg.o: $(GLOBALDEPS) SourceX/miniwin/misc_msg.cpp
	$(CPP) -c SourceX/miniwin/misc_msg.cpp -o obj/misc_msg.o $(CXXFLAGS)
obj/misc_dx.o: $(GLOBALDEPS) SourceX/miniwin/misc_dx.cpp
	$(CPP) -c SourceX/miniwin/misc_dx.cpp -o obj/misc_dx.o $(CXXFLAGS)
obj/rand.o: $(GLOBALDEPS) SourceX/miniwin/rand.cpp
	$(CPP) -c SourceX/miniwin/rand.cpp -o obj/rand.o $(CXXFLAGS)
obj/thread.o: $(GLOBALDEPS) SourceX/miniwin/thread.cpp
	$(CPP) -c SourceX/miniwin/thread.cpp -o obj/thread.o $(CXXFLAGS)
obj/dsound.o: $(GLOBALDEPS) SourceX/miniwin/dsound.cpp
	$(CPP) -c SourceX/miniwin/dsound.cpp -o obj/dsound.o $(CXXFLAGS)
obj/sound.o: $(GLOBALDEPS) SourceX/sound.cpp
	$(CPP) -c SourceX/sound.cpp -o obj/sound.o $(CXXFLAGS)
obj/storm.o: $(GLOBALDEPS) SourceX/storm/storm.cpp
	$(CPP) -c SourceX/storm/storm.cpp -o obj/storm.o $(CXXFLAGS)
obj/storm_net.o: $(GLOBALDEPS) SourceX/storm/storm_net.cpp
	$(CPP) -c SourceX/storm/storm_net.cpp -o obj/storm_net.o $(CXXFLAGS)
obj/storm_dx.o: $(GLOBALDEPS) SourceX/storm/storm_dx.cpp
	$(CPP) -c SourceX/storm/storm_dx.cpp -o obj/storm_dx.o $(CXXFLAGS)
obj/abstract_net.o: $(GLOBALDEPS) SourceX/dvlnet/abstract_net.cpp
	$(CPP) -c SourceX/dvlnet/abstract_net.cpp -o obj/abstract_net.o $(CXXFLAGS)
obj/loopback.o: $(GLOBALDEPS) SourceX/dvlnet/loopback.cpp
	$(CPP) -c SourceX/dvlnet/loopback.cpp -o obj/loopback.o $(CXXFLAGS)
obj/packet.o: $(GLOBALDEPS) SourceX/dvlnet/packet.cpp
	$(CPP) -c SourceX/dvlnet/packet.cpp -o obj/packet.o $(CXXFLAGS)
obj/base.o: $(GLOBALDEPS) SourceX/dvlnet/base.cpp
	$(CPP) -c SourceX/dvlnet/base.cpp -o obj/base.o $(CXXFLAGS)
obj/frame_queue.o: $(GLOBALDEPS) SourceX/dvlnet/frame_queue.cpp
	$(CPP) -c SourceX/dvlnet/frame_queue.cpp -o obj/frame_queue.o $(CXXFLAGS)
obj/credits.o: $(GLOBALDEPS) SourceX/DiabloUI/credits.cpp
	$(CPP) -c SourceX/DiabloUI/credits.cpp -o obj/credits.o $(CXXFLAGS)
obj/diabloui.o: $(GLOBALDEPS) SourceX/DiabloUI/diabloui.cpp
	$(CPP) -c SourceX/DiabloUI/diabloui.cpp -o obj/diabloui.o $(CXXFLAGS)
obj/dialogs.o: $(GLOBALDEPS) SourceX/DiabloUI/dialogs.cpp
	$(CPP) -c SourceX/DiabloUI/dialogs.cpp -o obj/dialogs.o $(CXXFLAGS)
obj/mainmenu.o: $(GLOBALDEPS) SourceX/DiabloUI/mainmenu.cpp
	$(CPP) -c SourceX/DiabloUI/mainmenu.cpp -o obj/mainmenu.o $(CXXFLAGS)
obj/progress.o: $(GLOBALDEPS) SourceX/DiabloUI/progress.cpp
	$(CPP) -c SourceX/DiabloUI/progress.cpp -o obj/progress.o $(CXXFLAGS)
obj/selconn.o: $(GLOBALDEPS) SourceX/DiabloUI/selconn.cpp
	$(CPP) -c SourceX/DiabloUI/selconn.cpp -o obj/selconn.o $(CXXFLAGS)
obj/selgame.o: $(GLOBALDEPS) SourceX/DiabloUI/selgame.cpp
	$(CPP) -c SourceX/DiabloUI/selgame.cpp -o obj/selgame.o $(CXXFLAGS)
obj/selhero.o: $(GLOBALDEPS) SourceX/DiabloUI/selhero.cpp
	$(CPP) -c SourceX/DiabloUI/selhero.cpp -o obj/selhero.o $(CXXFLAGS)
obj/selyesno.o: $(GLOBALDEPS) SourceX/DiabloUI/selyesno.cpp
	$(CXX) -c SourceX/DiabloUI/selyesno.cpp -o obj/selyesno.o $(CXXFLAGS)
obj/title.o: $(GLOBALDEPS) SourceX/DiabloUI/title.cpp
	$(CPP) -c SourceX/DiabloUI/title.cpp -o obj/title.o $(CXXFLAGS)
obj/main.o: $(GLOBALDEPS) SourceX/main.cpp
	$(CPP) -c SourceX/main.cpp -o obj/main.o $(CXXFLAGS)
obj/switch_keyboard.o: $(GLOBALDEPS) switch/switch_keyboard.cpp
	$(CPP) -c switch/switch_keyboard.cpp -o obj/switch_keyboard.o $(CXXFLAGS)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all	:	$(BINDIR)/$(OUTPUT).pfs0 $(BINDIR)/$(OUTPUT).nro

$(BINDIR)/$(OUTPUT).pfs0	:	$(BINDIR)/$(OUTPUT).nso

$(BINDIR)/$(OUTPUT).nso	:	$(BINDIR)/$(OUTPUT).elf

ifeq ($(strip $(NO_NACP)),)
$(BINDIR)/$(OUTPUT).nro	:	$(BINDIR)/$(OUTPUT).elf $(BINDIR)/$(OUTPUT).nacp
else
$(BINDIR)/$(OUTPUT).nro	:	$(BINDIR)/$(OUTPUT).elf
endif

$(BINDIR)/$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

# end of Makefile ...
