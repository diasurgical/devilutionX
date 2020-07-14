VC5_DIR ?= $(HOME)/DevStudio_5.10/VC

# The $(VS6_DIR) directory is a copy of the "Microsoft Visual Studio" directory.
#
# To get a working setup on Linux or other "portable" copies of VS,
# the following DLLs have to be copied to the
# $(VS6_DIR)/VC98/Bin directory.
#
#     - $(VS6_DIR)/Common/MSDev98/Bin/MSPDB60.DLL
#
# And to the $(VC5_DIR)/bin directory.
#
#     - $(VC5_DIR)/SharedIDE/bin/MSDIS100.DLL
#     - $(VC5_DIR)/SharedIDE/bin/MSPDB50.DLL
VS6_DIR ?= $(HOME)/VS6

VC6_DIR     = $(VS6_DIR)/VC98

VC6_BIN_DIR = $(VC6_DIR)/Bin
VC6_INC_DIR = $(VC6_DIR)/Include
VC6_LIB_DIR = $(VC6_DIR)/Lib

VC5_LIB_DIR = $(VC5_DIR)/lib

IDE_DIR ?= $(VS6_DIR)/Common/MSDev98
IDE_BIN_DIR = $(IDE_DIR)/bin
ifeq ($(OS),Windows_NT)
	CL = $(VC6_BIN_DIR)/CL.EXE
	RC = $(IDE_BIN_DIR)/RC.EXE
	VC5_LINK = $(VC5_DIR)/bin/link.exe
	VC6_LINK = $(VC6_BIN_DIR)/link.exe
else
	CL = wine $(VC6_BIN_DIR)/CL.EXE
	RC = wine $(IDE_BIN_DIR)/RC.EXE
	VC5_LINK = wine $(VC5_DIR)/bin/link.exe
	VC6_LINK = wine $(VC6_BIN_DIR)/link.exe
endif

CFLAGS=/nologo /c /GX /W3 /O1 /I $(VC6_INC_DIR) /FD /MT /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /Gm /Zi
LINKFLAGS=/nologo /subsystem:windows /machine:I386 /incremental:no

VC_LINK=$(VC5_LINK)
LINKFLAGS+= /LIBPATH:$(VC5_LIB_DIR)

all: pkware.lib

PKWARE_SRC=$(sort $(wildcard *.cpp))
PKWARE_OBJS=$(PKWARE_SRC:.cpp=.obj)

pkware.lib: $(PKWARE_OBJS)
	$(VC_LINK) -lib /OUT:$@ $^ /nologo

%.obj: %.cpp
	$(CL) $(CFLAGS) /Fo$@ $<

clean:
	@$(RM) -v $(PKWARE_OBJS) pkware.lib vc60.{idb,pch,pdb}

.PHONY: clean all
