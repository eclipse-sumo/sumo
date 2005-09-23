# Microsoft Developer Studio Project File - Name="z_libutils_gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=z_libutils_gui - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "z_libutils_gui.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "z_libutils_gui.mak" CFG="z_libutils_gui - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "z_libutils_gui - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "z_libutils_gui - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "z_libutils_gui - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(FOX14)\include" /I "..\..\..\src" /D "HAVE_CONFIG_H" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FOXDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "z_libutils_gui - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(FOX14)\include" /I "..\..\..\src" /D "HAVE_CONFIG_H" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "FOXDLL" /FD /GZ /c
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

# Name "z_libutils_gui - Win32 Release"
# Name "z_libutils_gui - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIAppGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIDanielPerspectiveChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIDialog_EditViewport.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIDialog_GLChosenEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\events\GUIEvent_SimulationEnded.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIGlChildWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIGlobalSelection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObject_AbstractAdd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObjectGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGLObjectPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObjectStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGLObjectToolTip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIGradients.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIGradientStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIGrid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUIIconSubSys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUIImageGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIIOGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIMainWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIMessageWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIParam_PopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIParameterTableItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIParameterTableWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\tracker\GUIParameterTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIPerspectiveChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIPointOfInterest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIPolygon2D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUISelectedStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUISUMOAbstractView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUITexturesHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\tracker\GUITLLogicPhasesTrackerWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\tracker\TrackerValueDesc.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIAppEnum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIAppGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIBaseColorer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIBaseLaneDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColorer_GradientByFunctionValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColorer_GradientByTimedFV.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColorer_LaneBySelection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColorer_ShadeByFunctionValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColorer_ShadeByFunctionValue1.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColorer_SingleColor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIColoringSchemesMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIColoringSchemesMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIDanielPerspectiveChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIDialog_EditViewport.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIDialog_GLChosenEditor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\events\GUIEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\events\GUIEvent_Message.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\events\GUIEvent_SimulationEnded.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\events\GUIEvent_SimulationStep.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\events\GUIEvents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIExcp_VehicleIsInvisible.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIGlChildWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIGlobalSelection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObject_AbstractAdd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObjectGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGLObjectPopupMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObjectStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGLObjectToolTip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIGlObjectTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIGradients.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUIGradientStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIGrid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUIIcons.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUIIconSubSys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUIImageGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIInterface_GridSpeedUp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIIOGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUILaneDrawer_FGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUILaneDrawer_FGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUILaneDrawer_SGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUILaneDrawer_SGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\drawer\GUILaneRepresentation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIMainWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIMessageWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIParam_PopupMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIParameterTableItem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUIParameterTableWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\tracker\GUIParameterTracker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUIPerspectiveChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIPointOfInterest.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\globjects\GUIPolygon2D.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\div\GUISelectedStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\windows\GUISUMOAbstractView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUITextures.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\images\GUITexturesHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\tracker\GUITLLogicPhasesTrackerWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\utils\gui\tracker\TrackerValueDesc.h
# End Source File
# End Group
# End Target
# End Project
