# Microsoft Developer Studio Project File - Name="y_libgui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=y_libgui - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE
!MESSAGE NMAKE /f "y_libgui.mak".
!MESSAGE
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE
!MESSAGE NMAKE /f "y_libgui.mak" CFG="y_libgui - Win32 Debug"
!MESSAGE
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE
!MESSAGE "y_libgui - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "y_libgui - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
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
# ADD CPP /nologo /MD /w /W0 /GX /O2 /I "$(FOX)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /I "..\..\..\src\foreign" /D "NDEBUG" /D "FOXDLL" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(FOX)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /I "..\..\..\src\foreign" /D "_DEBUG" /D "FOXDLL" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseDetectorDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseJunctionDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseLaneDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseROWDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseVehicleDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIDanielPerspectiveChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_FGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_FGwT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_SGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_SGwT.cpp
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

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_GLChosenEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_GLObjChooser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_MicroViewSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_SimSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIEvent_SimulationEnded.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObject_AbstractAdd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\popup\GUIGLObjectPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObjectStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGLObjectToolTip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGradientStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\icons\GUIIconSubSys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIJunctionDrawer_nT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIJunctionDrawer_wT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_FGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_FGwT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_SGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_SGwT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUILoadThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\manipulators\GUIManip_LaneSpeedTrigger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\manipulators\GUIManipulator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIMessageWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParam_PopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTableItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTableWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\GUIParameterTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIPerspectiveChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_FGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_FGwT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_SGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_SGwT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIRunThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISelectedStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOAbstractView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOViewParent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISupplementaryWeightsHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\textures\GUITexturesHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIThreadFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\tlstracker\GUITLLogicPhasesTrackerWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGnTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGwTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGnTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGwTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIViewAggregatedLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIViewTraffic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\TrackerValueDesc.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\gui\GUIAddWeightsStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIAppEnum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIApplicationWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseDetectorDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseJunctionDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseLaneDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseROWDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIBaseVehicleDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIColoringSchemesMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIDanielPerspectiveChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_FGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_FGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_SGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIDetectorDrawer_SGwT.h
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

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_GLChosenEditor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_GLObjChooser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_MicroViewSettings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\dialogs\GUIDialog_SimSettings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIEvent_Message.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIEvent_SimulationEnded.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIEvent_SimulationLoaded.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIEvent_SimulationStep.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIEvents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIExcp_VehicleIsInvisible.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlobalSelection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObject_AAManipulatable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObject_AbstractAdd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\popup\GUIGLObjectPopupMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObjectStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGLObjectToolTip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObjectTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGradientStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\icons\GUIIcons.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\icons\GUIIconSubSys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIJunctionDrawer_nT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIJunctionDrawer_wT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_FGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_FGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_SGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUILaneDrawer_SGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUILoadThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\manipulators\GUIManip_LaneSpeedTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\manipulators\GUIManipulator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIMessageWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParam_PopupMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTableItem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTableWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\GUIParameterTracker.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIPerspectiveChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_FGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_FGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_SGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIROWDrawer_SGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIRunThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISelectedStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOAbstractView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOViewParent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISupplementaryWeightsHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\textures\GUITextures.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\textures\GUITexturesHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIThreadFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\tlstracker\GUITLLogicPhasesTrackerWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGnTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGwTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGnTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGwTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIViewAggregatedLanes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIViewTraffic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\TrackerValueDesc.h
# End Source File
# End Group
# End Target
# End Project
