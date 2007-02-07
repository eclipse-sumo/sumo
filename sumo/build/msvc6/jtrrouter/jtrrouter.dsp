# Microsoft Developer Studio Project File - Name="jtrrouter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=jtrrouter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jtrrouter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jtrrouter.mak" CFG="jtrrouter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jtrrouter - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "jtrrouter - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jtrrouter - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(XERCES)\include" /I "..\..\..\src" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 $(XERCES)\lib\xerces-c_2.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\..\bin	copy Release\jtrrouter.exe ..\..\..\bin\jtrrouter.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "jtrrouter - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(XERCES)\include" /I "..\..\..\src" /D "ACE_HAS_DLL" /D "_DEBUG" /D "_AFXDLL" /D "_CONSOLE" /D "WIN32" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 $(XERCES)\lib\xerces-c_2D.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\..\bin	copy Debug\jtrrouter.exe ..\..\..\bin\jtrrouterD.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "jtrrouter - Win32 Release"
# Name "jtrrouter - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\utils\router\IDSupplier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\jtrrouter_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTREdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTREdgeBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRRouter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRTurnDefLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\router\TextHelpers.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\utils\router\FloatValueTimeLine.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\router\IDSupplier.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTREdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTREdgeBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRRouter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRTurnDefLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\router\TextHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\router\ValueTimeLine.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
