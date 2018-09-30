cmake_minimum_required(VERSION 3.5)

include(CMake/out_of_tree.cmake)

# This must be included before project()
include(CMake/32bit.cmake)

set(CMAKE_CXX_STANDARD 11)


project(devil-miniwin
	VERSION 0.0.1
	LANGUAGES CXX
)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(WARNING "sizeof(void*) == ${CMAKE_SIZEOF_VOID_P}.")
	message(FATAL_ERROR [[This project can only be compiled in 32-bit mode.]])
endif()

set(SOURCES
	Source/automap.cpp
	Source/codec.cpp
	Source/control.cpp
	Source/cursor.cpp
	Source/dead.cpp
	Source/debug.cpp
	Source/diablo.cpp
	Source/doom.cpp
	Source/drlg_l1.cpp
	Source/drlg_l2.cpp
	Source/drlg_l3.cpp
	Source/drlg_l4.cpp
#	Source/effects.cpp
	Source/encrypt.cpp
	Source/engine.cpp
	Source/error.cpp
	Source/gamemenu.cpp
	Source/gendung.cpp
	Source/gmenu.cpp
	Source/help.cpp
	Source/interfac.cpp
	Source/inv.cpp
	Source/items.cpp
	Source/lighting.cpp
	Source/loadsave.cpp
	Source/mainmenu.cpp
	Source/minitext.cpp
	Source/missiles.cpp
	Source/monster.cpp
	Source/mpqapi.cpp
	Source/msgcmd.cpp
	Source/msg.cpp
	Source/multi.cpp
	Source/objects.cpp
	Source/pack.cpp
	Source/palette.cpp
	Source/path.cpp
	Source/pfile.cpp
	Source/player.cpp
	Source/plrmsg.cpp
	Source/portal.cpp
	Source/quests.cpp
	Source/render.cpp
	Source/scrollrt.cpp
	Source/setmaps.cpp
	Source/sha.cpp
	Source/spells.cpp
	Source/stores.cpp
	Source/sync.cpp
	Source/textdat.cpp
	Source/themes.cpp
	Source/tmsg.cpp
	Source/town.cpp
	Source/towners.cpp
	Source/track.cpp
	Source/trigs.cpp
	Source/wave.cpp

	3rdParty/PKWare/explode.cpp
	3rdParty/PKWare/implode.cpp
)

# Completely stubbed out sources, for reference
set(REPLACED_SOURCES
	Source/capture.cpp
	Source/appfat.cpp
	Source/dthread.cpp
	Source/dx.cpp
	Source/fault.cpp
	Source/init.cpp
	Source/logging.cpp
	Source/movie.cpp
	Source/nthread.cpp
	Source/restrict.cpp
	Source/sound.cpp
	Source/effects.cpp # There is a function here that references Dx3d and lead to crash.
)

set(STUB_SOURCES
	Stub/miniwin.cpp
	Stub/miniwin_rand.cpp
	Stub/appfat.cpp
	Stub/capture.cpp
	Stub/dthread.cpp
	Stub/dx.cpp
	Stub/effects.cpp
	Stub/fault.cpp
	Stub/init.cpp
	Stub/movie.cpp
	Stub/nthread.cpp
	Stub/restrict.cpp
	Stub/sound.cpp
	Stub/storm.cpp
	Stub/diabloui.cpp
    Stub/miniwin_io.cpp
    Stub/miniwin_sdl.cpp
	Stub/storm_net.cpp
	Stub/sdlrender.cpp
	Stub/SDL_FontCache.cpp

	3rdParty/StormLib/src/FileStream.cpp
	3rdParty/StormLib/src/SBaseCommon.cpp
	3rdParty/StormLib/src/SBaseFileTable.cpp
	3rdParty/StormLib/src/SBaseSubTypes.cpp
	3rdParty/StormLib/src/SCompression.cpp
	3rdParty/StormLib/src/SFileExtractFile.cpp
	3rdParty/StormLib/src/SFileFindFile.cpp
	3rdParty/StormLib/src/SFileGetFileInfo.cpp
	3rdParty/StormLib/src/SFileOpenArchive.cpp
	3rdParty/StormLib/src/SFileOpenFileEx.cpp
	3rdParty/StormLib/src/SFileReadFile.cpp
)

include(CMake/SDL2_fixed.cmake)
include(CMake/sanitize.cmake)

include_directories(${SDL2_INCLUDE_DIRS})

include_directories(. Stub)
# FUTURE: use add_compile_definitions()
add_definitions(-DMINIWIN -D_DEBUG -DFASTER)
add_compile_options(-g -fpermissive -Wno-write-strings -Wno-multichar)

add_executable(devil-test ${SOURCES} ${STUB_SOURCES} Stub/main_test.cpp)
target_compile_options(devil-test PRIVATE ${SANITIZE_OPTIONS})
target_link_libraries(devil-test PUBLIC m ${SDL2_LIBRARIES} ${SANITIZE_OPTIONS})
