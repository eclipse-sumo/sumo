# Microsoft Developer Studio Project File - Name="gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=gui - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE
!MESSAGE NMAKE /f "gui.mak".
!MESSAGE
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "gui.mak" CFG="gui - Win32 Debug"
!MESSAGE
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "gui - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "gui - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gui - Win32 Release"

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
# ADD CPP /nologo /MD /w /W0 /GR /GX /O2 /I "d:\libs\xerces-c_2_3_0-win32\include" /I "d:\libs\xerces-c_2_3_0-win32\include\xercesc" /I "d:\libs\glut\\" /I "d:\libs\qt\include" /I "C:\Benutzer\daniel\own\xerces-c_2_3_0-win32\include" /I "C:\Benutzer\daniel\own\xerces-c_2_3_0-win32\include\xercesc" /I "c:\qt\include" /I "$(QTDIR)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(XERCES)\lib\xerces-c_2.lib $(QTDIR)\lib\qt-mt230nc.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:console /incremental:yes /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\gui.exe ..\..\..\bin\gui.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GR /GX /ZI /Od /I "$(QTDIR)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "ABS_DEBUG" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib $(XERCES)\lib\xerces-c_2D.lib $(QTDIR)\lib\qt-mt230nc.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\gui.exe ..\..\..\bin\guiD.exe
# End Special Build Tool

!ENDIF

# Begin Target

# Name "gui - Win32 Release"
# Name "gui - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\gui_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\PreStartInitialised.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\SimpleCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_version.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_only\SUMOFrame.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\helpers\Command.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\Counter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\Dictionary.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\gcc_NullType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\gcc_Typelist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\gcc_TypeManip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\gcc_TypeTraits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui_help.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\HelpPrinter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSExtendedTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\msvc6_NullType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\msvc6_static_check.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\msvc6_Typelist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\msvc6_TypeManip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\msvc6_TypeTraits.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\OneArgumentCommand.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\PreStartInitialised.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\SimpleCommand.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\SingletonDictionary.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_version.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_only\SUMOFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\ValueRetriever.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\ValueSource.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
