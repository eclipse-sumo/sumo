# Microsoft Developer Studio Project File - Name="z_libutils_common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=z_libutils_common - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "z_libutils_common.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "z_libutils_common.mak" CFG="z_libutils_common - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "z_libutils_common - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "z_libutils_common - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "z_libutils_common - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "z_libutils_common - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "z_libutils_common - Win32 Release"
# Name "z_libutils_common - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\utils\common\DevHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\DoubleVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\FileErrorReporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\FileHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\IntVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\MsgHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\RandHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\StringTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\SystemFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\SysUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\UtilExceptions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\XMLHelpers.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\utils\common\AbstractMutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\BoolVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\DevHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\DoubleVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\FileErrorReporter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\FileHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\HelpPrinter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\InstancePool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\IntVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\LogFile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\MsgHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\MsgRetriever.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\MsgRetrievingFunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\Named.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\NamedObjectCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\RandHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\StdDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\StringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\StringUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\SUMOTime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\SystemFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\SysUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\UtilExceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\common\XMLHelpers.h
# End Source File
# End Group
# End Target
# End Project
