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
# ADD CPP /nologo /MD /w /W0 /GX /O2 /I "$(QTDIR)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /GZ /c
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

SOURCE=..\..\..\src\gui\drawerimpl\GUIAggregatedLaneDrawer.cpp
# End Source File
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

SOURCE=..\..\..\src\gui\GUIChooser.cpp
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

SOURCE=..\..\..\src\gui\GUIGlObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObjectStorage.cpp
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

SOURCE=..\..\..\src\gui\GUIMessageWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTable.cpp
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

SOURCE=..\..\..\src\gui\GUISUMOAbstractView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOViewParent.cpp
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

SOURCE=..\..\..\src\gui\moc_GUIApplicationWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_GUIChooser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_GUIMessageWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\moc_GUIParameterTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_GUISUMOAbstractView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_GUISUMOViewParent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\tlstracker\moc_GUITLLogicPhasesTrackerWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_GUIViewAggregatedLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_GUIViewTraffic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_QGUIImageField.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\moc_QGUIToggleButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\moc_QParamPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QAboutSUMO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QApplicationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\popup\QGLObjectPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\popup\QGLObjectPopupMenuItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QGLObjectToolTip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QGUIImageField.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QGUIToggleButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QMicroscopicViewSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\QParamPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QSimulationEndedEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QSimulationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\TrackerValueDesc.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\gui\drawerimpl\GUIAggregatedLaneDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIApplicationWindow.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIApplicationWindow.h...
InputPath=..\..\..\src\gui\GUIApplicationWindow.h

"..\..\..\src\gui\moc_GUIApplicationWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIApplicationWindow.h -o ..\..\..\src\gui\moc_GUIApplicationWindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIApplicationWindow.h...
InputPath=..\..\..\src\gui\GUIApplicationWindow.h

"..\..\..\src\gui\moc_GUIApplicationWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIApplicationWindow.h -o ..\..\..\src\gui\moc_GUIApplicationWindow.cpp

# End Custom Build

!ENDIF 

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

SOURCE=..\..\..\src\gui\GUIChooser.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIChooser.h...
InputPath=..\..\..\src\gui\GUIChooser.h

"..\..\..\src\gui\moc_GUIChooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIChooser.h -o ..\..\..\src\gui\moc_GUIChooser.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIChooser.h...
InputPath=..\..\..\src\gui\GUIChooser.h

"..\..\..\src\gui\moc_GUIChooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIChooser.h -o ..\..\..\src\gui\moc_GUIChooser.cpp

# End Custom Build

!ENDIF 

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

SOURCE=..\..\..\src\gui\GUIEvents.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObjectStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIGlObjectTypes.h
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

SOURCE=..\..\..\src\gui\GUIMessageWindow.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIMessageWindow.h...
InputPath=..\..\..\src\gui\GUIMessageWindow.h

"..\..\..\src\gui\moc_GUIMessageWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIMessageWindow.h -o ..\..\..\src\gui\moc_GUIMessageWindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIMessageWindow.h...
InputPath=..\..\..\src\gui\GUIMessageWindow.h

"..\..\..\src\gui\moc_GUIMessageWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIMessageWindow.h -o ..\..\..\src\gui\moc_GUIMessageWindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTable.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIParameterTable.h...
InputPath=..\..\..\src\gui\partable\GUIParameterTable.h

"..\..\..\src\gui\partable\moc_GUIParameterTable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\partable\GUIParameterTable.h -o ..\..\..\src\gui\partable\moc_GUIParameterTable.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIParameterTable.h...
InputPath=..\..\..\src\gui\partable\GUIParameterTable.h

"..\..\..\src\gui\partable\moc_GUIParameterTable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\partable\GUIParameterTable.h -o ..\..\..\src\gui\partable\moc_GUIParameterTable.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTableItem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\GUIParameterTableWindow.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIParameterTableWindow.h...
InputPath=..\..\..\src\gui\partable\GUIParameterTableWindow.h

"..\..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\partable\GUIParameterTableWindow.h -o ..\..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIParameterTableWindow.h...
InputPath=..\..\..\src\gui\partable\GUIParameterTableWindow.h

"..\..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\partable\GUIParameterTableWindow.h -o ..\..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\GUIParameterTracker.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIParameterTracker.h...
InputPath=..\..\..\src\gui\vartracker\GUIParameterTracker.h

"..\..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\vartracker\GUIParameterTracker.h -o ..\..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIParameterTracker.h...
InputPath=..\..\..\src\gui\vartracker\GUIParameterTracker.h

"..\..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\vartracker\GUIParameterTracker.h -o ..\..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp

# End Custom Build

!ENDIF 

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

SOURCE=..\..\..\src\gui\GUISUMOAbstractView.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUISUMOAbstractView.h...
InputPath=..\..\..\src\gui\GUISUMOAbstractView.h

"..\..\..\src\gui\moc_GUISUMOAbstractView.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUISUMOAbstractView.h -o ..\..\..\src\gui\moc_GUISUMOAbstractView.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUISUMOAbstractView.h...
InputPath=..\..\..\src\gui\GUISUMOAbstractView.h

"..\..\..\src\gui\moc_GUISUMOAbstractView.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUISUMOAbstractView.h -o ..\..\..\src\gui\moc_GUISUMOAbstractView.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUISUMOViewParent.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUISUMOViewParent.h...
InputPath=..\..\..\src\gui\GUISUMOViewParent.h

"..\..\..\src\gui\moc_GUISUMOViewParent.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUISUMOViewParent.h -o ..\..\..\src\gui\moc_GUISUMOViewParent.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUISUMOViewParent.h...
InputPath=..\..\..\src\gui\GUISUMOViewParent.h

"..\..\..\src\gui\moc_GUISUMOViewParent.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUISUMOViewParent.h -o ..\..\..\src\gui\moc_GUISUMOViewParent.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\tlstracker\GUITLLogicPhasesTrackerWindow.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUITLLogicPhasesTrackerWindow.h...
InputPath=..\..\..\src\gui\tlstracker\GUITLLogicPhasesTrackerWindow.h

"..\..\..\src\gui\tlstracker\moc_GUITLLogicPhasesTrackerWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\tlstracker\GUITLLogicPhasesTrackerWindow.h -o ..\..\..\src\gui\tlstracker\moc_GUITLLogicPhasesTrackerWindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUITLLogicPhasesTrackerWindow.h...
InputPath=..\..\..\src\gui\tlstracker\GUITLLogicPhasesTrackerWindow.h

"..\..\..\src\gui\tlstracker\moc_GUITLLogicPhasesTrackerWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\tlstracker\GUITLLogicPhasesTrackerWindow.h -o ..\..\..\src\gui\tlstracker\moc_GUITLLogicPhasesTrackerWindow.cpp

# End Custom Build

!ENDIF 

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

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIViewAggregatedLanes.h...
InputPath=..\..\..\src\gui\GUIViewAggregatedLanes.h

"..\..\..\src\gui\moc_GUIViewAggregatedLanes.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIViewAggregatedLanes.h -o ..\..\..\src\gui\moc_GUIViewAggregatedLanes.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIViewAggregatedLanes.h...
InputPath=..\..\..\src\gui\GUIViewAggregatedLanes.h

"..\..\..\src\gui\moc_GUIViewAggregatedLanes.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIViewAggregatedLanes.h -o ..\..\..\src\gui\moc_GUIViewAggregatedLanes.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\GUIViewTraffic.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing GUIViewTraffic.h...
InputPath=..\..\..\src\gui\GUIViewTraffic.h

"..\..\..\src\gui\moc_GUIViewTraffic.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIViewTraffic.h -o ..\..\..\src\gui\moc_GUIViewTraffic.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIViewTraffic.h...
InputPath=..\..\..\src\gui\GUIViewTraffic.h

"..\..\..\src\gui\moc_GUIViewTraffic.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\GUIViewTraffic.h -o ..\..\..\src\gui\moc_GUIViewTraffic.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QAboutSUMO.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QAboutSUMO.h ...
InputPath=..\..\..\src\gui\qdialogs\QAboutSUMO.h

"..\..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QAboutSUMO.h -o ..\..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QAboutSUMO.h ...
InputPath=..\..\..\src\gui\qdialogs\QAboutSUMO.h

"..\..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QAboutSUMO.h -o ..\..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QApplicationSettings.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QApplicationSettings.h ...
InputPath=..\..\..\src\gui\qdialogs\QApplicationSettings.h

"..\..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QApplicationSettings.h -o ..\..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QApplicationSettings.h ...
InputPath=..\..\..\src\gui\qdialogs\QApplicationSettings.h

"..\..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QApplicationSettings.h -o ..\..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\popup\QGLObjectPopupMenu.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QGLObjectPopupMenu.h ...
InputPath=..\..\..\src\gui\popup\QGLObjectPopupMenu.h

"..\..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\popup\QGLObjectPopupMenu.h -o ..\..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QGLObjectPopupMenu.h ...
InputPath=..\..\..\src\gui\popup\QGLObjectPopupMenu.h

"..\..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\popup\QGLObjectPopupMenu.h -o ..\..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\popup\QGLObjectPopupMenuItem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QGLObjectToolTip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QGUIImageField.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QGUIImageField.h...
InputPath=..\..\..\src\gui\QGUIImageField.h

"..\..\..\src\gui\moc_QGUIImageField.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\QGUIImageField.h -o ..\..\..\src\gui\moc_QGUIImageField.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QGUIImageField.h...
InputPath=..\..\..\src\gui\QGUIImageField.h

"..\..\..\src\gui\moc_QGUIImageField.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\QGUIImageField.h -o ..\..\..\src\gui\moc_QGUIImageField.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QGUIToggleButton.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QGUIToggleButton.h...
InputPath=..\..\..\src\gui\QGUIToggleButton.h

"..\..\..\src\gui\moc_QGUIToggleButton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\QGUIToggleButton.h -o ..\..\..\src\gui\moc_QGUIToggleButton.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QGUIToggleButton.h...
InputPath=..\..\..\src\gui\QGUIToggleButton.h

"..\..\..\src\gui\moc_QGUIToggleButton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\QGUIToggleButton.h -o ..\..\..\src\gui\moc_QGUIToggleButton.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QMessageEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QMicroscopicViewSettings.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QMicroscopicViewSettings.h...
InputPath=..\..\..\src\gui\qdialogs\QMicroscopicViewSettings.h

"..\..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QMicroscopicViewSettings.h -o ..\..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QMicroscopicViewSettings.h...
InputPath=..\..\..\src\gui\qdialogs\QMicroscopicViewSettings.h

"..\..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QMicroscopicViewSettings.h -o ..\..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\partable\QParamPopupMenu.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QParamPopupMenu.h...
InputPath=..\..\..\src\gui\partable\QParamPopupMenu.h

"..\..\..\src\gui\partable\moc_QParamPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\partable\QParamPopupMenu.h -o ..\..\..\src\gui\partable\moc_QParamPopupMenu.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QParamPopupMenu.h...
InputPath=..\..\..\src\gui\partable\QParamPopupMenu.h

"..\..\..\src\gui\partable\moc_QParamPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\partable\QParamPopupMenu.h -o ..\..\..\src\gui\partable\moc_QParamPopupMenu.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QSimulationEndedEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QSimulationLoadedEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\qdialogs\QSimulationSettings.h

!IF  "$(CFG)" == "y_libgui - Win32 Release"

# Begin Custom Build - Moc'ing QSimulationSettings.h ...
InputPath=..\..\..\src\gui\qdialogs\QSimulationSettings.h

"..\..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QSimulationSettings.h -o ..\..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "y_libgui - Win32 Debug"

# Begin Custom Build - Moc'ing QSimulationSettings.h ...
InputPath=..\..\..\src\gui\qdialogs\QSimulationSettings.h

"..\..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\..\src\gui\qdialogs\QSimulationSettings.h -o ..\..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QSimulationStepEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\QSUMOEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\gui\vartracker\TrackerValueDesc.h
# End Source File
# End Group
# End Target
# End Project
