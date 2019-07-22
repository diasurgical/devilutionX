ifeq ($(strip $(DEVKITPRO)),)
	$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif
 
include $(DEVKITPRO)/libnx/switch_rules

contents := $(shell mkdir -p obj ; mkdir -p release)

# these are used in the included switch_rules
APP_TITLE 	:= diablo-nx
APP_VERSION	:= 1.0.0
APP_AUTHOR	:= Devilution Team
ICON 		:= switch/icon.jpg

BINDIR		= release
OUTPUT    	= diablo-nx

LIBS      	= -specs=$(DEVKITPRO)/libnx/switch.specs -g -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -L$(DEVKITPRO)/libnx/lib -L$(DEVKITPRO)/portlibs/switch/lib -lSDL2_mixer -lSDL2_ttf -lfreetype -lvorbisfile -lvorbis -logg -lmodplug -lmikmod -lmpg123 -lSDL2 -lopusfile -lopus -lEGL -lglapi -ldrm_nouveau -lpng -lbz2 -lz -lnx 
INCS      	= -I$(DEVKITPRO)/portlibs/switch/include/SDL2 -I"Source" -I"SourceS" -I"SourceX" -I"3rdParty/asio/include" -I"3rdParty/Radon/Radon/include" -I"3rdParty/libsmacker" -I$(DEVKITPRO)/libnx/include -I$(DEVKITPRO)/portlibs/switch/include
CXXINCS   	= -I$(DEVKITPRO)/portlibs/switch/include/SDL2 -I"Source" -I"SourceS" -I"SourceX" -I"3rdParty/asio/include" -I"3rdParty/Radon/Radon/include" -I"3rdParty/libsmacker" -I$(DEVKITPRO)/libnx/include -I$(DEVKITPRO)/portlibs/switch/include
DEFINES   	= -DSWITCH -DPLATFORM_NX -DSDL2 -DDEVILUTION_STUB -DDEVILUTION_ENGINE -DASIO_STANDALONE -DASIO_HEADER_ONLY
CXXFLAGS  	= $(CXXINCS) $(DEFINES) -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -fsigned-char -Wall -Wextra -Wno-write-strings -fpermissive -Wno-write-strings -Wno-multichar -w -O2
CFLAGS    	= $(INCS) $(DEFINES)    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE -fsigned-char -Wall -Wextra -Wno-write-strings -fpermissive -Wno-write-strings -Wno-multichar -w -O2

export NROFLAGS += --icon=$(ICON)
export NROFLAGS += --nacp=$(BINDIR)/$(OUTPUT).nacp
 
SMACKEROBJ 		= obj/smk_bitstream.o obj/smk_hufftree.o obj/smacker.o 
RADONOBJ		= obj/File.o obj/Key.o obj/Named.o obj/Section.o
STORMLIBOBJ 	= obj/FileStream.o obj/SBaseCommon.o obj/SBaseFileTable.o obj/SBaseSubTypes.o obj/SCompression.o obj/SFileExtractFile.o obj/SFileFindFile.o obj/SFileGetFileInfo.o obj/SFileOpenArchive.o obj/SFileOpenFileEx.o obj/SFileReadFile.o
PKWAREOBJ		= obj/explode.o obj/implode.o
DEVILUTIONOBJ 	= \
	obj/appfat.o \
	obj/automap.o \
	obj/capture.o \
	obj/codec.o \
	obj/control.o \
	obj/cursor.o \
	obj/dead.o \
	obj/debug.o \
	obj/diablo.o \
	obj/doom.o \
	obj/drlg_l1.o \
	obj/drlg_l2.o \
	obj/drlg_l3.o \
	obj/drlg_l4.o \
	obj/dthread.o \
	obj/effects.o \
	obj/encrypt.o \
	obj/engine.o \
	obj/error.o \
	obj/fault.o \
	obj/gamemenu.o \
	obj/gendung.o \
	obj/gmenu.o \
	obj/help.o \
	obj/init.o \
	obj/interfac.o \
	obj/inv.o \
	obj/itemdat.o \
	obj/items.o \
	obj/lighting.o \
	obj/loadsave.o \
	obj/logging.o \
	obj/mmainmenu.o \
	obj/minitext.o \
	obj/misdat.o \
	obj/missiles.o \
	obj/monstdat.o \
	obj/monster.o \
	obj/movie.o \
	obj/mpqapi.o \
	obj/msgcmd.o \
	obj/msg.o \
	obj/multi.o \
	obj/nthread.o \
	obj/objdat.o \
	obj/objects.o \
	obj/pack.o \
	obj/palette.o \
	obj/path.o \
	obj/pfile.o \
	obj/player.o \
	obj/plrctrls.o \
	obj/plrmsg.o \
	obj/portal.o \
	obj/spelldat.o \
	obj/quests.o \
	obj/render.o \
	obj/restrict.o \
	obj/scrollrt.o \
	obj/setmaps.o \
	obj/sha.o \
	obj/spells.o \
	obj/stores.o \
	obj/sync.o \
	obj/textdat.o \
	obj/themes.o \
	obj/tmsg.o \
	obj/town.o \
	obj/towners.o \
	obj/track.o \
	obj/trigs.o \
	obj/wave.o

MAINOBJ = \
	obj/dx.o \
	obj/misc.o \
	obj/misc_io.o \
	obj/misc_msg.o \
	obj/misc_dx.o \
	obj/rand.o \
	obj/thread.o \
	obj/dsound.o \
	obj/ddraw.o \
	obj/sound.o \
	obj/storm.o \
	obj/storm_net.o \
	obj/storm_dx.o \
	obj/abstract_net.o \
	obj/loopback.o \
	obj/packet.o \
	obj/base.o \
	obj/frame_queue.o \
	obj/credits.o \
	obj/diabloui.o \
	obj/dialogs.o \
	obj/mainmenu.o \
	obj/progress.o \
	obj/selconn.o \
	obj/selgame.o \
	obj/selhero.o \
	obj/selyesno.o \
	obj/title.o \
	obj/main.o \
	obj/touch.o

# touch keyboard on Switch
MAINOBJ += obj/switch_keyboard.o

.PHONY: all clean
all: $(BINDIR)/$(OUTPUT).nro
clean:
	@rm -f $(MAINOBJ) $(SMACKEROBJ) $(RADONOBJ) $(STORMLIBOBJ) $(PKWAREOBJ) $(DEVILUTIONOBJ) $(BINDIR)/$(OUTPUT).elf

$(BINDIR)/$(OUTPUT).elf: $(MAINOBJ) $(SMACKEROBJ) $(RADONOBJ) $(STORMLIBOBJ) $(PKWAREOBJ) $(DEVILUTIONOBJ)
	$(CXX) $(MAINOBJ) $(SMACKEROBJ) $(RADONOBJ) $(STORMLIBOBJ) $(PKWAREOBJ) $(DEVILUTIONOBJ) -o $(BINDIR)/$(OUTPUT).elf $(LIBS)

# *.nacp/*.nro build rules are defined in the switch_rules file, part of devkitA64
$(BINDIR)/$(OUTPUT).nro	:	$(BINDIR)/$(OUTPUT).elf $(BINDIR)/$(OUTPUT).nacp

#Smacker

obj/smk_bitstream.o: $(GLOBALDEPS) 3rdParty/libsmacker/smk_bitstream.c 
	$(CC) -c 3rdParty/libsmacker/smk_bitstream.c -o obj/smk_bitstream.o $(CFLAGS)
obj/smk_hufftree.o: $(GLOBALDEPS) 3rdParty/libsmacker/smk_hufftree.c
	$(CC) -c 3rdParty/libsmacker/smk_hufftree.c -o obj/smk_hufftree.o $(CFLAGS)
obj/smacker.o: $(GLOBALDEPS) 3rdParty/libsmacker/smacker.c 
	$(CC) -c 3rdParty/libsmacker/smacker.c -o obj/smacker.o $(CFLAGS)

#Radon

obj/File.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/File.cpp 
	$(CXX) -c 3rdParty/Radon/Radon/source/File.cpp -o obj/File.o $(CXXFLAGS)
obj/Key.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/Key.cpp
	$(CXX) -c 3rdParty/Radon/Radon/source/Key.cpp -o obj/Key.o $(CXXFLAGS)
obj/Named.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/Named.cpp
	$(CXX) -c 3rdParty/Radon/Radon/source/Named.cpp -o obj/Named.o $(CXXFLAGS)
obj/Section.o: $(GLOBALDEPS) 3rdParty/Radon/Radon/source/Section.cpp
	$(CXX) -c 3rdParty/Radon/Radon/source/Section.cpp -o obj/Section.o $(CXXFLAGS)

#StormLib

obj/FileStream.o: $(GLOBALDEPS) 3rdParty/StormLib/src/FileStream.cpp
	$(CXX) -c 3rdParty/StormLib/src/FileStream.cpp -o obj/FileStream.o $(CXXFLAGS)
obj/SBaseCommon.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SBaseCommon.cpp
	$(CXX) -c 3rdParty/StormLib/src/SBaseCommon.cpp -o obj/SBaseCommon.o $(CXXFLAGS)
obj/SBaseFileTable.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SBaseFileTable.cpp
	$(CXX) -c 3rdParty/StormLib/src/SBaseFileTable.cpp -o obj/SBaseFileTable.o $(CXXFLAGS)
obj/SBaseSubTypes.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SBaseSubTypes.cpp
	$(CXX) -c 3rdParty/StormLib/src/SBaseSubTypes.cpp -o obj/SBaseSubTypes.o $(CXXFLAGS)
obj/SCompression.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SCompression.cpp
	$(CXX) -c 3rdParty/StormLib/src/SCompression.cpp -o obj/SCompression.o $(CXXFLAGS)
obj/SFileExtractFile.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileExtractFile.cpp
	$(CXX) -c 3rdParty/StormLib/src/SFileExtractFile.cpp -o obj/SFileExtractFile.o $(CXXFLAGS)
obj/SFileFindFile.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileFindFile.cpp
	$(CXX) -c 3rdParty/StormLib/src/SFileFindFile.cpp -o obj/SFileFindFile.o $(CXXFLAGS)
obj/SFileGetFileInfo.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileGetFileInfo.cpp
	$(CXX) -c 3rdParty/StormLib/src/SFileGetFileInfo.cpp -o obj/SFileGetFileInfo.o $(CXXFLAGS)
obj/SFileOpenArchive.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileOpenArchive.cpp
	$(CXX) -c 3rdParty/StormLib/src/SFileOpenArchive.cpp -o obj/SFileOpenArchive.o $(CXXFLAGS)
obj/SFileOpenFileEx.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileOpenFileEx.cpp
	$(CXX) -c 3rdParty/StormLib/src/SFileOpenFileEx.cpp -o obj/SFileOpenFileEx.o $(CXXFLAGS)
obj/SFileReadFile.o: $(GLOBALDEPS) 3rdParty/StormLib/src/SFileReadFile.cpp
	$(CXX) -c 3rdParty/StormLib/src/SFileReadFile.cpp -o obj/SFileReadFile.o $(CXXFLAGS)

#PKWare

obj/explode.o: $(GLOBALDEPS) 3rdParty/PKWare/explode.cpp
	$(CXX) -c 3rdParty/PKWare/explode.cpp -o obj/explode.o $(CXXFLAGS)
obj/implode.o: $(GLOBALDEPS) 3rdParty/PKWare/implode.cpp
	$(CXX) -c 3rdParty/PKWare/implode.cpp -o obj/implode.o $(CXXFLAGS)

#Devilution
obj/appfat.o: $(GLOBALDEPS) Source/appfat.cpp
	$(CXX) -c Source/appfat.cpp -o obj/appfat.o $(CXXFLAGS)
obj/automap.o: $(GLOBALDEPS) Source/automap.cpp 
	$(CXX) -c Source/automap.cpp -o obj/automap.o $(CXXFLAGS)
obj/capture.o: $(GLOBALDEPS)  Source/capture.cpp
	$(CXX) -c  Source/capture.cpp -o obj/capture.o $(CXXFLAGS)
obj/codec.o: $(GLOBALDEPS) Source/codec.cpp 
	$(CXX) -c Source/codec.cpp -o obj/codec.o $(CXXFLAGS)
obj/control.o: $(GLOBALDEPS) Source/control.cpp 
	$(CXX) -c Source/control.cpp -o obj/control.o $(CXXFLAGS)
obj/cursor.o: $(GLOBALDEPS) Source/cursor.cpp
	$(CXX) -c Source/cursor.cpp -o obj/cursor.o $(CXXFLAGS)
obj/dead.o: $(GLOBALDEPS) Source/dead.cpp
	$(CXX) -c Source/dead.cpp -o obj/dead.o $(CXXFLAGS)
obj/debug.o: $(GLOBALDEPS) Source/debug.cpp
	$(CXX) -c Source/debug.cpp -o obj/debug.o $(CXXFLAGS)
obj/diablo.o: $(GLOBALDEPS) Source/diablo.cpp
	$(CXX) -c Source/diablo.cpp -o obj/diablo.o $(CXXFLAGS)
obj/doom.o: $(GLOBALDEPS) Source/doom.cpp
	$(CXX) -c Source/doom.cpp -o obj/doom.o $(CXXFLAGS)
obj/drlg_l1.o: $(GLOBALDEPS)  Source/drlg_l1.cpp
	$(CXX) -c  Source/drlg_l1.cpp -o obj/drlg_l1.o $(CXXFLAGS)
obj/drlg_l2.o: $(GLOBALDEPS) Source/drlg_l2.cpp 
	$(CXX) -c Source/drlg_l2.cpp -o obj/drlg_l2.o $(CXXFLAGS)
obj/drlg_l3.o: $(GLOBALDEPS) Source/drlg_l3.cpp
	$(CXX) -c Source/drlg_l3.cpp -o obj/drlg_l3.o $(CXXFLAGS)
obj/drlg_l4.o: $(GLOBALDEPS) Source/drlg_l4.cpp
	$(CXX) -c Source/drlg_l4.cpp -o obj/drlg_l4.o $(CXXFLAGS)
obj/dthread.o: $(GLOBALDEPS) Source/dthread.cpp
	$(CXX) -c Source/dthread.cpp -o obj/dthread.o $(CXXFLAGS)
obj/effects.o: $(GLOBALDEPS) Source/effects.cpp
	$(CXX) -c Source/effects.cpp -o obj/effects.o $(CXXFLAGS)
obj/encrypt.o: $(GLOBALDEPS)  Source/encrypt.cpp
	$(CXX) -c  Source/encrypt.cpp -o obj/encrypt.o $(CXXFLAGS)
obj/engine.o: $(GLOBALDEPS) Source/engine.cpp
	$(CXX) -c Source/engine.cpp -o obj/engine.o $(CXXFLAGS)
obj/error.o: $(GLOBALDEPS) Source/error.cpp
	$(CXX) -c Source/error.cpp -o obj/error.o $(CXXFLAGS)
obj/fault.o: $(GLOBALDEPS) Source/fault.cpp 
	$(CXX) -c Source/fault.cpp -o obj/fault.o $(CXXFLAGS)
obj/gamemenu.o: $(GLOBALDEPS)  Source/gamemenu.cpp
	$(CXX) -c  Source/gamemenu.cpp -o obj/gamemenu.o $(CXXFLAGS)
obj/gendung.o: $(GLOBALDEPS)  Source/gendung.cpp
	$(CXX) -c  Source/gendung.cpp -o obj/gendung.o $(CXXFLAGS)
obj/gmenu.o: $(GLOBALDEPS)  Source/gmenu.cpp
	$(CXX) -c  Source/gmenu.cpp -o obj/gmenu.o $(CXXFLAGS)
obj/help.o: $(GLOBALDEPS)  Source/help.cpp
	$(CXX) -c  Source/help.cpp -o obj/help.o $(CXXFLAGS)
obj/init.o: $(GLOBALDEPS)  Source/init.cpp
	$(CXX) -c  Source/init.cpp -o obj/init.o $(CXXFLAGS)
obj/interfac.o: $(GLOBALDEPS)  Source/interfac.cpp
	$(CXX) -c  Source/interfac.cpp -o obj/interfac.o $(CXXFLAGS)
obj/inv.o: $(GLOBALDEPS)  Source/inv.cpp
	$(CXX) -c  Source/inv.cpp -o obj/inv.o $(CXXFLAGS)
obj/itemdat.o: $(GLOBALDEPS)  Source/itemdat.cpp
	$(CXX) -c  Source/itemdat.cpp -o obj/itemdat.o $(CXXFLAGS)
obj/items.o: $(GLOBALDEPS)  Source/items.cpp
	$(CXX) -c  Source/items.cpp -o obj/items.o $(CXXFLAGS)
obj/lighting.o: $(GLOBALDEPS)  Source/lighting.cpp
	$(CXX) -c  Source/lighting.cpp -o obj/lighting.o $(CXXFLAGS)
obj/loadsave.o: $(GLOBALDEPS)  Source/loadsave.cpp
	$(CXX) -c  Source/loadsave.cpp -o obj/loadsave.o $(CXXFLAGS)
obj/logging.o: $(GLOBALDEPS)  Source/logging.cpp
	$(CXX) -c  Source/logging.cpp -o obj/logging.o $(CXXFLAGS)
obj/mmainmenu.o: $(GLOBALDEPS)  Source/mainmenu.cpp
	$(CXX) -c  Source/mainmenu.cpp -o obj/mmainmenu.o $(CXXFLAGS)
obj/minitext.o: $(GLOBALDEPS)  Source/minitext.cpp
	$(CXX) -c  Source/minitext.cpp -o obj/minitext.o $(CXXFLAGS)
obj/misdat.o: $(GLOBALDEPS)  Source/misdat.cpp
	$(CXX) -c  Source/misdat.cpp -o obj/misdat.o $(CXXFLAGS)
obj/missiles.o: $(GLOBALDEPS)  Source/missiles.cpp
	$(CXX) -c  Source/missiles.cpp -o obj/missiles.o $(CXXFLAGS)
obj/monstdat.o: $(GLOBALDEPS)  Source/monstdat.cpp
	$(CXX) -c  Source/monstdat.cpp -o obj/monstdat.o $(CXXFLAGS)
obj/monster.o: $(GLOBALDEPS)  Source/monster.cpp
	$(CXX) -c  Source/monster.cpp -o obj/monster.o $(CXXFLAGS)
obj/movie.o: $(GLOBALDEPS)  Source/movie.cpp
	$(CXX) -c  Source/movie.cpp -o obj/movie.o $(CXXFLAGS)
obj/mpqapi.o: $(GLOBALDEPS)  Source/mpqapi.cpp
	$(CXX) -c  Source/mpqapi.cpp -o obj/mpqapi.o $(CXXFLAGS)
obj/msgcmd.o: $(GLOBALDEPS)  Source/msgcmd.cpp
	$(CXX) -c  Source/msgcmd.cpp -o obj/msgcmd.o $(CXXFLAGS)
obj/msg.o: $(GLOBALDEPS)  Source/msg.cpp
	$(CXX) -c  Source/msg.cpp -o obj/msg.o $(CXXFLAGS)
obj/multi.o: $(GLOBALDEPS)  Source/multi.cpp
	$(CXX) -c  Source/multi.cpp -o obj/multi.o $(CXXFLAGS)
obj/nthread.o: $(GLOBALDEPS)  Source/nthread.cpp
	$(CXX) -c  Source/nthread.cpp -o obj/nthread.o $(CXXFLAGS)
obj/objdat.o: $(GLOBALDEPS)  Source/objdat.cpp
	$(CXX) -c  Source/objdat.cpp -o obj/objdat.o $(CXXFLAGS)
obj/objects.o: $(GLOBALDEPS)  Source/objects.cpp
	$(CXX) -c  Source/objects.cpp -o obj/objects.o $(CXXFLAGS)
obj/pack.o: $(GLOBALDEPS)  Source/pack.cpp
	$(CXX) -c  Source/pack.cpp -o obj/pack.o $(CXXFLAGS)
obj/palette.o: $(GLOBALDEPS)  Source/palette.cpp
	$(CXX) -c  Source/palette.cpp -o obj/palette.o $(CXXFLAGS)
obj/path.o: $(GLOBALDEPS)  Source/path.cpp
	$(CXX) -c  Source/path.cpp -o obj/path.o $(CXXFLAGS)
obj/pfile.o: $(GLOBALDEPS)  Source/pfile.cpp
	$(CXX) -c  Source/pfile.cpp -o obj/pfile.o $(CXXFLAGS)
obj/player.o: $(GLOBALDEPS)  Source/player.cpp
	$(CXX) -c  Source/player.cpp -o obj/player.o $(CXXFLAGS)
obj/plrmsg.o: $(GLOBALDEPS)  Source/plrmsg.cpp
	$(CXX) -c  Source/plrmsg.cpp -o obj/plrmsg.o $(CXXFLAGS)
obj/plrctrls.o: $(GLOBALDEPS) Source/plrctrls.cpp
	$(CXX) -c Source/plrctrls.cpp -o obj/plrctrls.o $(CXXFLAGS)
obj/portal.o: $(GLOBALDEPS)  Source/portal.cpp
	$(CXX) -c  Source/portal.cpp -o obj/portal.o $(CXXFLAGS)
obj/spelldat.o: $(GLOBALDEPS)  Source/spelldat.cpp
	$(CXX) -c  Source/spelldat.cpp -o obj/spelldat.o $(CXXFLAGS)
obj/quests.o: $(GLOBALDEPS)  Source/quests.cpp
	$(CXX) -c  Source/quests.cpp -o obj/quests.o $(CXXFLAGS)
obj/render.o: $(GLOBALDEPS)  Source/render.cpp
	$(CXX) -c  Source/render.cpp -o obj/render.o $(CXXFLAGS)
obj/restrict.o: $(GLOBALDEPS)  Source/restrict.cpp
	$(CXX) -c  Source/restrict.cpp -o obj/restrict.o $(CXXFLAGS)
obj/scrollrt.o: $(GLOBALDEPS)  Source/scrollrt.cpp
	$(CXX) -c  Source/scrollrt.cpp -o obj/scrollrt.o $(CXXFLAGS)
obj/setmaps.o: $(GLOBALDEPS)  Source/setmaps.cpp
	$(CXX) -c  Source/setmaps.cpp -o obj/setmaps.o $(CXXFLAGS)
obj/sha.o: $(GLOBALDEPS)  Source/sha.cpp
	$(CXX) -c  Source/sha.cpp -o obj/sha.o $(CXXFLAGS)
obj/spells.o: $(GLOBALDEPS)  Source/spells.cpp
	$(CXX) -c  Source/spells.cpp -o obj/spells.o $(CXXFLAGS)
obj/stores.o: $(GLOBALDEPS)  Source/stores.cpp
	$(CXX) -c  Source/stores.cpp -o obj/stores.o $(CXXFLAGS)
obj/sync.o: $(GLOBALDEPS)  Source/sync.cpp
	$(CXX) -c  Source/sync.cpp -o obj/sync.o $(CXXFLAGS)
obj/textdat.o: $(GLOBALDEPS)  Source/textdat.cpp
	$(CXX) -c  Source/textdat.cpp -o obj/textdat.o $(CXXFLAGS)
obj/themes.o: $(GLOBALDEPS)  Source/themes.cpp
	$(CXX) -c  Source/themes.cpp -o obj/themes.o $(CXXFLAGS)
obj/tmsg.o: $(GLOBALDEPS)  Source/tmsg.cpp
	$(CXX) -c  Source/tmsg.cpp -o obj/tmsg.o $(CXXFLAGS)
obj/town.o: $(GLOBALDEPS)  Source/town.cpp
	$(CXX) -c  Source/town.cpp -o obj/town.o $(CXXFLAGS)
obj/towners.o: $(GLOBALDEPS)  Source/towners.cpp
	$(CXX) -c  Source/towners.cpp -o obj/towners.o $(CXXFLAGS)
obj/track.o: $(GLOBALDEPS)  Source/track.cpp
	$(CXX) -c  Source/track.cpp -o obj/track.o $(CXXFLAGS)
obj/trigs.o: $(GLOBALDEPS)  Source/trigs.cpp
	$(CXX) -c  Source/trigs.cpp -o obj/trigs.o $(CXXFLAGS)
obj/wave.o: $(GLOBALDEPS)  Source/wave.cpp
	$(CXX) -c  Source/wave.cpp -o obj/wave.o $(CXXFLAGS)
 
#Main	
obj/dx.o: $(GLOBALDEPS) SourceX/dx.cpp 
	$(CXX) -c SourceX/dx.cpp -o obj/dx.o $(CXXFLAGS)
obj/misc.o: $(GLOBALDEPS) SourceX/miniwin/misc.cpp
	$(CXX) -c SourceX/miniwin/misc.cpp -o obj/misc.o $(CXXFLAGS)
obj/misc_io.o: $(GLOBALDEPS) SourceX/miniwin/misc_io.cpp
	$(CXX) -c SourceX/miniwin/misc_io.cpp -o obj/misc_io.o $(CXXFLAGS)
obj/misc_msg.o: $(GLOBALDEPS) SourceX/miniwin/misc_msg.cpp
	$(CXX) -c SourceX/miniwin/misc_msg.cpp -o obj/misc_msg.o $(CXXFLAGS)
obj/misc_dx.o: $(GLOBALDEPS) SourceX/miniwin/misc_dx.cpp
	$(CXX) -c SourceX/miniwin/misc_dx.cpp -o obj/misc_dx.o $(CXXFLAGS)
obj/rand.o: $(GLOBALDEPS) SourceX/miniwin/rand.cpp
	$(CXX) -c SourceX/miniwin/rand.cpp -o obj/rand.o $(CXXFLAGS)
obj/thread.o: $(GLOBALDEPS) SourceX/miniwin/thread.cpp
	$(CXX) -c SourceX/miniwin/thread.cpp -o obj/thread.o $(CXXFLAGS)
obj/dsound.o: $(GLOBALDEPS) SourceX/miniwin/dsound.cpp
	$(CXX) -c SourceX/miniwin/dsound.cpp -o obj/dsound.o $(CXXFLAGS)
obj/ddraw.o: $(GLOBALDEPS) SourceX/miniwin/ddraw.cpp
	$(CXX) -c SourceX/miniwin/ddraw.cpp -o obj/ddraw.o $(CXXFLAGS)
obj/sound.o: $(GLOBALDEPS) SourceX/sound.cpp
	$(CXX) -c SourceX/sound.cpp -o obj/sound.o $(CXXFLAGS)
obj/storm.o: $(GLOBALDEPS) SourceX/storm/storm.cpp
	$(CXX) -c SourceX/storm/storm.cpp -o obj/storm.o $(CXXFLAGS)
obj/storm_net.o: $(GLOBALDEPS) SourceX/storm/storm_net.cpp
	$(CXX) -c SourceX/storm/storm_net.cpp -o obj/storm_net.o $(CXXFLAGS)
obj/storm_dx.o: $(GLOBALDEPS) SourceX/storm/storm_dx.cpp
	$(CXX) -c SourceX/storm/storm_dx.cpp -o obj/storm_dx.o $(CXXFLAGS)
obj/abstract_net.o: $(GLOBALDEPS) SourceX/dvlnet/abstract_net.cpp
	$(CXX) -c SourceX/dvlnet/abstract_net.cpp -o obj/abstract_net.o $(CXXFLAGS)
obj/loopback.o: $(GLOBALDEPS) SourceX/dvlnet/loopback.cpp
	$(CXX) -c SourceX/dvlnet/loopback.cpp -o obj/loopback.o $(CXXFLAGS)
obj/packet.o: $(GLOBALDEPS) SourceX/dvlnet/packet.cpp
	$(CXX) -c SourceX/dvlnet/packet.cpp -o obj/packet.o $(CXXFLAGS)
obj/base.o: $(GLOBALDEPS) SourceX/dvlnet/base.cpp
	$(CXX) -c SourceX/dvlnet/base.cpp -o obj/base.o $(CXXFLAGS)
obj/frame_queue.o: $(GLOBALDEPS) SourceX/dvlnet/frame_queue.cpp
	$(CXX) -c SourceX/dvlnet/frame_queue.cpp -o obj/frame_queue.o $(CXXFLAGS)
obj/credits.o: $(GLOBALDEPS) SourceX/DiabloUI/credits.cpp
	$(CXX) -c SourceX/DiabloUI/credits.cpp -o obj/credits.o $(CXXFLAGS)
obj/diabloui.o: $(GLOBALDEPS) SourceX/DiabloUI/diabloui.cpp
	$(CXX) -c SourceX/DiabloUI/diabloui.cpp -o obj/diabloui.o $(CXXFLAGS)
obj/dialogs.o: $(GLOBALDEPS) SourceX/DiabloUI/dialogs.cpp
	$(CXX) -c SourceX/DiabloUI/dialogs.cpp -o obj/dialogs.o $(CXXFLAGS)
obj/mainmenu.o: $(GLOBALDEPS) SourceX/DiabloUI/mainmenu.cpp
	$(CXX) -c SourceX/DiabloUI/mainmenu.cpp -o obj/mainmenu.o $(CXXFLAGS)
obj/progress.o: $(GLOBALDEPS) SourceX/DiabloUI/progress.cpp
	$(CXX) -c SourceX/DiabloUI/progress.cpp -o obj/progress.o $(CXXFLAGS)
obj/selconn.o: $(GLOBALDEPS) SourceX/DiabloUI/selconn.cpp
	$(CXX) -c SourceX/DiabloUI/selconn.cpp -o obj/selconn.o $(CXXFLAGS)
obj/selgame.o: $(GLOBALDEPS) SourceX/DiabloUI/selgame.cpp
	$(CXX) -c SourceX/DiabloUI/selgame.cpp -o obj/selgame.o $(CXXFLAGS)
obj/selhero.o: $(GLOBALDEPS) SourceX/DiabloUI/selhero.cpp
	$(CXX) -c SourceX/DiabloUI/selhero.cpp -o obj/selhero.o $(CXXFLAGS)
obj/selyesno.o: $(GLOBALDEPS) SourceX/DiabloUI/selyesno.cpp
	$(CXX) -c SourceX/DiabloUI/selyesno.cpp -o obj/selyesno.o $(CXXFLAGS)
obj/title.o: $(GLOBALDEPS) SourceX/DiabloUI/title.cpp
	$(CXX) -c SourceX/DiabloUI/title.cpp -o obj/title.o $(CXXFLAGS)
obj/main.o: $(GLOBALDEPS) SourceX/main.cpp
	$(CXX) -c SourceX/main.cpp -o obj/main.o $(CXXFLAGS)
obj/touch.o: $(GLOBALDEPS) touch/touch.cpp
	$(CXX) -c touch/touch.cpp -o obj/touch.o $(CXXFLAGS)
obj/switch_keyboard.o: $(GLOBALDEPS) switch/switch_keyboard.cpp
	$(CXX) -c switch/switch_keyboard.cpp -o obj/switch_keyboard.o $(CXXFLAGS)

