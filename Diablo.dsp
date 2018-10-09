# Microsoft Developer Studio Project File - Name="Diablo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Diablo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "Diablo.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Diablo.mak" CFG="Diablo - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Diablo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Diablo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Diablo - Win32 Release with PDB" (based on "Win32 (x86) Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Diablo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Source/WinRel"
# PROP BASE Intermediate_Dir "Source/WinRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bld/WinRel"
# PROP Intermediate_Dir "Source/WinRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 DiabloUI/WinRel/DiabloUI.lib 3rdParty/Storm/Source/WinRel/Storm.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Diablo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Diablo__"
# PROP BASE Intermediate_Dir "Diablo__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bld/WinDebug"
# PROP Intermediate_Dir "Source/WinDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /O1 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib user32.lib gdi32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /machine:I386
 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "Diablo - Win32 Release with PDB"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Diablo___Win32_Release_with_PDB"
# PROP BASE Intermediate_Dir "Diablo___Win32_Release_with_PDB"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bld/WinRel"
# PROP Intermediate_Dir "Source/WinRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT BASE CPP /WX
# ADD CPP /nologo /MT /W3 /GX /Zi /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 DiabloUI/WinRel/DiabloUI.lib 3rdParty/Storm/Source/WinRel/Storm.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 DiabloUI/WinRel/DiabloUI.lib 3rdParty/Storm/Source/WinRel/Storm.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib version.lib /nologo /subsystem:windows /debug /machine:I386

!ENDIF

# Begin Target

# Name "Diablo - Win32 Release"
# Name "Diablo - Win32 Debug"
# Name "Diablo - Win32 Release with PDB"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\appfat.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\automap.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\capture.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\codec.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\control.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\dead.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\diablo.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\doom.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l1.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l2.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l3.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l4.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\dthread.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\dx.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\encrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\error.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\fault.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\gamemenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\gendung.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\gmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\help.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\init.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\interfac.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\inv.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\items.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\lighting.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\loadsave.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\logging.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\mainmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\minitext.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\missiles.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\monster.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\movie.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\mpqapi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\msg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\msgcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\multi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\nthread.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\objects.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\pack.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\palette.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\path.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\pfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\player.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\plrmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\portal.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\quests.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\restrict.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\scrollrt.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\setmaps.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\sha.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\spells.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\stores.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\sync.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\textdat.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\themes.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\tmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\town.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\towners.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\track.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\trigs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\wave.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\render.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Diablo.ico
# End Source File
# Begin Source File

SOURCE=.\Diablo.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"

# Begin Source File

SOURCE=.\defs.h
# End Source File
# Begin Source File

SOURCE=.\enums.h
# End Source File
# Begin Source File

SOURCE=.\structs.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\Source\appfat.h
# End Source File
# Begin Source File

SOURCE=.\Source\automap.h
# End Source File
# Begin Source File

SOURCE=.\Source\capture.h
# End Source File
# Begin Source File

SOURCE=.\Source\codec.h
# End Source File
# Begin Source File

SOURCE=.\Source\control.h
# End Source File
# Begin Source File

SOURCE=.\Source\cursor.h
# End Source File
# Begin Source File

SOURCE=.\Source\dead.h
# End Source File
# Begin Source File

SOURCE=.\Source\debug.h
# End Source File
# Begin Source File

SOURCE=.\Source\diablo.h
# End Source File
# Begin Source File

SOURCE=.\Source\doom.h
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l1.h
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l2.h
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l3.h
# End Source File
# Begin Source File

SOURCE=.\Source\drlg_l4.h
# End Source File
# Begin Source File

SOURCE=.\Source\dthread.h
# End Source File
# Begin Source File

SOURCE=.\Source\dx.h
# End Source File
# Begin Source File

SOURCE=.\Source\effects.h
# End Source File
# Begin Source File

SOURCE=.\Source\encrypt.h
# End Source File
# Begin Source File

SOURCE=.\Source\engine.h
# End Source File
# Begin Source File

SOURCE=.\Source\error.h
# End Source File
# Begin Source File

SOURCE=.\Source\fault.h
# End Source File
# Begin Source File

SOURCE=.\Source\gamemenu.h
# End Source File
# Begin Source File

SOURCE=.\Source\gendung.h
# End Source File
# Begin Source File

SOURCE=.\Source\gmenu.h
# End Source File
# Begin Source File

SOURCE=.\Source\help.h
# End Source File
# Begin Source File

SOURCE=.\Source\init.h
# End Source File
# Begin Source File

SOURCE=.\Source\interfac.h
# End Source File
# Begin Source File

SOURCE=.\Source\inv.h
# End Source File
# Begin Source File

SOURCE=.\Source\items.h
# End Source File
# Begin Source File

SOURCE=.\Source\lighting.h
# End Source File
# Begin Source File

SOURCE=.\Source\loadsave.h
# End Source File
# Begin Source File

SOURCE=.\Source\logging.h
# End Source File
# Begin Source File

SOURCE=.\Source\mainmenu.h
# End Source File
# Begin Source File

SOURCE=.\Source\minitext.h
# End Source File
# Begin Source File

SOURCE=.\Source\missiles.h
# End Source File
# Begin Source File

SOURCE=.\Source\monster.h
# End Source File
# Begin Source File

SOURCE=.\Source\movie.h
# End Source File
# Begin Source File

SOURCE=.\Source\mpqapi.h
# End Source File
# Begin Source File

SOURCE=.\Source\msg.h
# End Source File
# Begin Source File

SOURCE=.\Source\msgcmd.h
# End Source File
# Begin Source File

SOURCE=.\Source\multi.h
# End Source File
# Begin Source File

SOURCE=.\Source\nthread.h
# End Source File
# Begin Source File

SOURCE=.\Source\objects.h
# End Source File
# Begin Source File

SOURCE=.\Source\pack.h
# End Source File
# Begin Source File

SOURCE=.\Source\palette.h
# End Source File
# Begin Source File

SOURCE=.\Source\path.h
# End Source File
# Begin Source File

SOURCE=.\Source\pfile.h
# End Source File
# Begin Source File

SOURCE=.\Source\player.h
# End Source File
# Begin Source File

SOURCE=.\Source\plrmsg.h
# End Source File
# Begin Source File

SOURCE=.\Source\portal.h
# End Source File
# Begin Source File

SOURCE=.\Source\quests.h
# End Source File
# Begin Source File

SOURCE=.\Source\render.h
# End Source File
# Begin Source File

SOURCE=.\Source\restrict.h
# End Source File
# Begin Source File

SOURCE=.\Source\scrollrt.h
# End Source File
# Begin Source File

SOURCE=.\Source\setmaps.h
# End Source File
# Begin Source File

SOURCE=.\Source\sha.h
# End Source File
# Begin Source File

SOURCE=.\Source\sound.h
# End Source File
# Begin Source File

SOURCE=.\Source\spells.h
# End Source File
# Begin Source File

SOURCE=.\Source\stores.h
# End Source File
# Begin Source File

SOURCE=.\Source\sync.h
# End Source File
# Begin Source File

SOURCE=.\Source\textdat.h
# End Source File
# Begin Source File

SOURCE=.\Source\themes.h
# End Source File
# Begin Source File

SOURCE=.\Source\tmsg.h
# End Source File
# Begin Source File

SOURCE=.\Source\town.h
# End Source File
# Begin Source File

SOURCE=.\Source\towners.h
# End Source File
# Begin Source File

SOURCE=.\Source\track.h
# End Source File
# Begin Source File

SOURCE=.\Source\trigs.h
# End Source File
# Begin Source File

SOURCE=.\Source\wave.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "PKWare"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\3rdParty\PKWare\explode.cpp
# End Source File
# Begin Source File

SOURCE=.\3rdParty\PKWare\implode.cpp
# End Source File
# End Group
# End Target
# End Project