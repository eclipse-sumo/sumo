# Microsoft Developer Studio Project File - Name="y_libnetload" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=y_libnetload - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "y_libnetload.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "y_libnetload.mak" CFG="y_libnetload - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "y_libnetload - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "y_libnetload - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "y_libnetload - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /w /W0 /GX /O2 /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "y_libnetload - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "y_libnetload - Win32 Release"
# Name "y_libnetload - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\netload\NLContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLDetectorBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLDiscreteEventBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLEdgeControlBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLJunctionControlBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLNetBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLNetHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLSourceBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLSucceedingLaneBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLTriggerBuilder.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\netload\NLContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLDetectorBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLDiscreteEventBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLEdgeControlBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLJunctionControlBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLLoadFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLNetBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLNetHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLSourceBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLSucceedingLaneBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLTriggerBuilder.h
# End Source File
# End Group
# End Target
# End Project
