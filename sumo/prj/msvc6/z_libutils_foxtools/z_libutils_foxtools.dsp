# Microsoft Developer Studio Project File - Name="z_libutils_foxtools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=z_libutils_foxtools - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "z_libutils_foxtools.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "z_libutils_foxtools.mak" CFG="z_libutils_foxtools - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "z_libutils_foxtools - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "z_libutils_foxtools - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "z_libutils_foxtools - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(FOX14)\include" /I "..\..\..\src" /I "..\..\..\src\foreign" /D "HAVE_CONFIG_H" /D "NDEBUG" /D "FOXDLL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "BOOST_HAS_THREADS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "z_libutils_foxtools - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(FOX14)\include" /I "..\..\..\src" /I "..\..\..\src\foreign" /D "HAVE_CONFIG_H" /D "_DEBUG" /D "FOXDLL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "BOOST_HAS_THREADS" /FD /GZ /c
# SUBTRACT CPP /YX
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

# Name "z_libutils_foxtools - Win32 Release"
# Name "z_libutils_foxtools - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXBaseObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXLCDLabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXLockable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXMutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXRealSpinDial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXSevenSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXSingleEventThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXThreadEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXAddEditTypedTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXCheckableButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXEditableTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXEventQue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXImageHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXMenuHeader.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXBaseObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\fxexdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXLCDLabel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXLockable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXMutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXRealSpinDial.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXSevenSegment.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXSingleEventThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXThreadEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\FXThreadMessageRetriever.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXAddEditTypedTable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXCheckableButton.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXEditableTable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXEventQue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXImageHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXInterThreadEventClient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXMenuHeader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\foxtools\MFXThreadDefs.h
# End Source File
# End Group
# End Target
# End Project
