# Microsoft Developer Studio Project File - Name="giant" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=giant - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "giant.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "giant.mak" CFG="giant - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "giant - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "giant - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "giant - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(FOX)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "NDEBUG" /D "FOXDLL" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 opengl32.lib $(XERCES)\lib\xerces-c_2.lib $(FOX)\lib\FOXDLL-1.2.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "giant - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(FOX)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "_DEBUG" /D "ABS_DEBUG" /D "FOXDLL" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib $(XERCES)\lib\xerces-c_2D.lib $(FOX)\lib\FOXDLLD-1.2.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "giant - Win32 Release"
# Name "giant - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\netedit\ColorSelector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\ConfigDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Edge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\giant_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\GNEApplicationWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\GNEViewParent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\GNEViewTraffic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Graph.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Point.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\helpers\PreStartInitialised.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_version.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_only\SUMOFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Vertex.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\netedit\ConfigDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Edge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\giant_help.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\GNEApplicationWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\GNEViewTraffic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Graph.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Image.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Point.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sumo_only\SUMOFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netedit\Vertex.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
