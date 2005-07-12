# Microsoft Developer Studio Project File - Name="jtrrouter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=jtrrouter - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "jtrrouter.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "jtrrouter.mak" CFG="jtrrouter - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "jtrrouter - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "jtrrouter - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "BOOST_HAS_THREADS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(XERCES)\lib\xerces-c_2.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\..\bin	copy Release\jtrrouter.exe ..\..\..\bin\jtrrouter.exe	..\..\add-ins\incBuildNumber.pl jtrrouter
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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "BOOST_HAS_THREADS" /YX /FD /GZ /c
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

SOURCE=..\..\..\src\utils\router\FloatValueTimeLine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\router\IDSupplier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\jtrrouter_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPEdgeBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPRouter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPTurnDefLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_version.cpp
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

SOURCE=..\..\..\src\jtrrouter_build.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\jtrrouter_help.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPEdgeBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPRouter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJPTurnDefLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\routing_jtr\ROJTRFrame.h
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
