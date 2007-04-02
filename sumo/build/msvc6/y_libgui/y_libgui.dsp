# Microsoft Developer Studio Project File - Name="y_libgui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=y_libgui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "y_libgui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "y_libgui.mak" CFG="y_libgui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "y_libgui - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "y_libgui - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "y_libgui - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(FOX14)\include" /I "$(XERCES)\include" /I "..\..\..\src" /I "..\..\..\src\foreign" /D "NDEBUG" /D "FOXDLL" /D "_LIB" /D "WIN32" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(FOX14)\include" /I "$(XERCES)\include" /I "..\..\..\src" /D "_DEBUG" /D "FOXDLL" /D "_LIB" /D "WIN32" /D "_MBCS" /FD /GZ /c
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

# Name "y_libgui - Win32 Release"
# Name "y_libgui - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\gui\GUIApplicationWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_AboutSUMO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_AppSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_Breakpoints.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_EditAddWeights.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_GLObjChooser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_SimSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIJunctionDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUILoadThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIManipulator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIRunThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOViewParent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIThreadFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIViewTraffic.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\gui\GUIAddWeightsStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIApplicationWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColorer_ColorRetrival.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_AboutSUMO.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_AppSettings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_Breakpoints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_EditAddWeights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_GLObjChooser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_SimSettings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIJunctionDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUILoadThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIManipulator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIRunThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOViewParent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIThreadFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIViewTraffic.h
# End Source File
# End Group
# End Target
# End Project
