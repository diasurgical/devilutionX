# mingw32 and mingw64 have different executables
ifdef MINGW32
	CXX=mingw32-g++
	DLLTOOL=dlltool
	WINDRES=windres
else
	CXX=i686-w64-mingw32-g++
	DLLTOOL=i686-w64-mingw32-dlltool
	WINDRES=i686-w64-mingw32-windres
endif

# Clang doesn't understand permissive compilation, we need to "fix" invalid
# casts from a pointer type there using
#     static_cast<NEW_TYPE>(reinterpret_cast<uintptr_t>(ptr))
# instead of
#     (NEW_TYPE)(ptr)
CXXFLAGS=-fpermissive -Wno-write-strings
CPPFLAGS=-MMD -MF $*.d
LDLIBS=-lgdi32 -lversion -ldiabloui -lstorm
LDFLAGS=-L./ -static-libgcc -mwindows

all: devilution.exe

debug: CXXFLAGS += -D_DEBUG
debug: CPPFLAGS += -D_DEBUG
debug: devilution.exe

DIABLO_SRC=$(wildcard Source/*.cpp)
OBJS=$(DIABLO_SRC:.cpp=.o)

PKWARE_SRC=$(wildcard 3rdParty/PKWare/*.cpp)
PKWARE_OBJS=$(PKWARE_SRC:.cpp=.o)

devilution.exe: $(OBJS) $(PKWARE_OBJS) diabres.o diabloui.lib storm.lib
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

diabres.o: Diablo.rc
	$(WINDRES) $< $@

diabloui.lib: diabloui.dll DiabloUI/diabloui_gcc.def
	$(DLLTOOL) -d DiabloUI/diabloui_gcc.def -D $< -l $@

diabloui.dll:
#	$(error Please copy diabloui.dll (version 1.09[b]) here)

storm.lib: storm.dll 3rdParty/Storm/Source/storm_gcc.def
	$(DLLTOOL) -d 3rdParty/Storm/Source/storm_gcc.def -D $< -l $@

storm.dll:
#	$(error Please copy storm.dll (version 1.09[b]) here)

clean:
	@$(RM) -v $(OBJS) $(OBJS:.o=.d) $(PKWARE_OBJS) $(PKWARE_OBJS:.o=d) diabres.o storm.lib diabloui.lib devilution.exe

.PHONY: clean all
