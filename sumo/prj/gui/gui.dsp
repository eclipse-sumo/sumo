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
# ADD CPP /nologo /MD /w /W0 /GR /GX /O2 /I "d:\libs\xerces-c_2_3_0-win32\include" /I "d:\libs\xerces-c_2_3_0-win32\include\xercesc" /I "..\..\src" /I "d:\libs\glut\\" /I "d:\libs\qt\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\gui.exe ..\..\bin\gui.exe
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
# ADD CPP /nologo /MDd /w /W0 /Gm /GR /GX /ZI /Od /I "d:\libs\xerces-c_2_3_0-win32\include" /I "d:\libs\xerces-c_2_3_0-win32\include\xercesc" /I "..\..\src" /I "d:\libs\glut\\" /I "d:\libs\qt\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "ABS_DEBUG" /D "_AFXDLL" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\gui.exe ..\..\bin\guiD.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "gui - Win32 Release"
# Name "gui - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial10.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial11.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial6.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial7.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial8.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial9.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\AttributesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Boundery.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Bresenham.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\actions\Command_SaveTLSState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\FileErrorReporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\FileHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\glutils\FontStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\GenericSAX2Handler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\GeomConvHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\GeomHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\glutils\GLHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIAggregatedLaneDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIApplicationWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseJunctionDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseLaneDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseROWDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseVehicleDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIChooser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIDanielPerspectiveChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIDetectorBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIDetectorDrawer_nT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIDetectorDrawer_wT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIDetectorWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIEdgeControlBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIEmitterWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIGlObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIGlObjectStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIGrid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIHelpingJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIInductLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIInternalLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIJunctionControlBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIJunctionDrawer_nT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIJunctionDrawer_wT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIJunctionWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUILane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUILaneChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_FGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_FGwT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_SGnT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_SGwT.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUILaneStateReporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUILaneWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUILoadThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUINet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUINetBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUINetHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUINetWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUINoLogicJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\GUIParameterTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\GUIParameterTableItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\GUIParameterTableWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\vartracker\GUIParameterTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIPerspectiveChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIRightOfWayJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIROWDrawer_FG.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIROWDrawer_SG.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIRunThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUISourceLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUISUMOAbstractView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUISUMOViewParent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGnTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGwTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGnTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGwTasTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIViewAggregatedLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIViewTraffic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\glutils\lfontrenderer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Line2D.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\dev\MemDiff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_GUIApplicationWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_GUIChooser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\moc_GUIParameterTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_GUISUMOAbstractView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_GUISUMOViewParent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_GUIViewAggregatedLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_GUIViewTraffic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_QGUIImageField.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\moc_QGUIToggleButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\moc_QParamPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetector2File.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorSubSys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\actions\MSDiscreteEventControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEdgeControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEmitControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEventControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\MsgHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSInductLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSInternalLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJunctionControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJunctionLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLaneChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLaneSpeedTrigger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLaneState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLogicJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSNoLogicJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRightOfWayJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRoute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRouteHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRouteLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRouteLoaderControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSSourceLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTLLogicControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTrafficLightLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredSourceXMLHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredXMLReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\qutils\NewQMutex.cpp
# End Source File
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
# Begin Source File

SOURCE=..\..\src\utils\options\Option.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsSaveStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsSubSys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Position2DVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\helpers\PreStartInitialised.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QAboutSUMO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QApplicationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\popup\QGLObjectPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\popup\QGLObjectPopupMenuItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QGLObjectToolTip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QGUIImageField.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QGUIToggleButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QMicroscopicViewSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\QParamPopupMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QSimulationEndedEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QSimulationSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\gfx\RGBColor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\STRConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\sumo_version.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\sumo_only\SUMOFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOSAXHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOXMLDefinitions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\SystemFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\vartracker\TrackerValueDesc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\UtilExceptions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\XMLBuildingExceptions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\XMLSubSys.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\microsim\actions\Action.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial10.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial11.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial5.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial6.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial7.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial8.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\fonts\arial9.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\AttributesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\AttributesReadingGenericSAX2Handler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Boundery.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Bresenham.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\actions\Command_SaveTLSState.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\DoubleFunctionBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\DoubleValueSource.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\glutils\FontStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\FunctionBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\GenericSAX2Handler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\GeomConvHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\GeomHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\glutils\GLHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui_help.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIAggregatedLaneDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIApplicationWindow.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUIApplicationWindow.h...
InputPath=..\..\src\gui\GUIApplicationWindow.h

"..\..\src\gui\moc_GUIApplicationWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIApplicationWindow.h -o ..\..\src\gui\moc_GUIApplicationWindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIApplicationWindow.h...
InputPath=..\..\src\gui\GUIApplicationWindow.h

"..\..\src\gui\moc_GUIApplicationWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIApplicationWindow.h -o ..\..\src\gui\moc_GUIApplicationWindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIBarROWRulesDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseJunctionDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseLaneDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseROWDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIBaseVehicleDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIChooser.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUIChooser.h...
InputPath=..\..\src\gui\GUIChooser.h

"..\..\src\gui\moc_GUIChooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIChooser.h -o ..\..\src\gui\moc_GUIChooser.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIChooser.h...
InputPath=..\..\src\gui\GUIChooser.h

"..\..\src\gui\moc_GUIChooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIChooser.h -o ..\..\src\gui\moc_GUIChooser.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIDanielPerspectiveChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIDetectorBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIDetectorDrawer_nT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIDetectorDrawer_wT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIDetectorWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIEdgeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIEdgeControlBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIEmitterWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIEvents.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIGlObject.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIGlObjectStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIGlObjectTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIGrid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIHelpingJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIInductLoop.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIInternalLane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUIJunctionControlBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIJunctionDrawer_nT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIJunctionDrawer_wT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIJunctionWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUILane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUILaneChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_FGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_FGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_SGnT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUILaneDrawer_SGwT.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUILaneStateReporter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUILaneWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUILoadThread.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUINet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUINetBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guinetload\GUINetHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUINetWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUINoLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\GUIParameterTable.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUIParameterTable.h...
InputPath=..\..\src\gui\partable\GUIParameterTable.h

"..\..\src\gui\partable\moc_GUIParameterTable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\partable\GUIParameterTable.h -o ..\..\src\gui\partable\moc_GUIParameterTable.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIParameterTable.h...
InputPath=..\..\src\gui\partable\GUIParameterTable.h

"..\..\src\gui\partable\moc_GUIParameterTable.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\partable\GUIParameterTable.h -o ..\..\src\gui\partable\moc_GUIParameterTable.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\GUIParameterTableItem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\GUIParameterTableWindow.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUIParameterTableWindow.h...
InputPath=..\..\src\gui\partable\GUIParameterTableWindow.h

"..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\partable\GUIParameterTableWindow.h -o ..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIParameterTableWindow.h...
InputPath=..\..\src\gui\partable\GUIParameterTableWindow.h

"..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\partable\GUIParameterTableWindow.h -o ..\..\src\gui\partable\moc_GUIParameterTableWindow.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\vartracker\GUIParameterTracker.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUIParameterTracker.h...
InputPath=..\..\src\gui\vartracker\GUIParameterTracker.h

"..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\vartracker\GUIParameterTracker.h -o ..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIParameterTracker.h...
InputPath=..\..\src\gui\vartracker\GUIParameterTracker.h

"..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\vartracker\GUIParameterTracker.h -o ..\..\src\gui\vartracker\moc_GUIParameterTracker.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIPerspectiveChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIRightOfWayJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIROWDrawer_FG.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIROWDrawer_SG.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIRunThread.h
# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUISourceLane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUISUMOAbstractView.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUISUMOAbstractView.h...
InputPath=..\..\src\gui\GUISUMOAbstractView.h

"..\..\src\gui\moc_GUISUMOAbstractView.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUISUMOAbstractView.h -o ..\..\src\gui\moc_GUISUMOAbstractView.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUISUMOAbstractView.h...
InputPath=..\..\src\gui\GUISUMOAbstractView.h

"..\..\src\gui\moc_GUISUMOAbstractView.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUISUMOAbstractView.h -o ..\..\src\gui\moc_GUISUMOAbstractView.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUISUMOViewParent.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUISUMOViewParent.h...
InputPath=..\..\src\gui\GUISUMOViewParent.h

"..\..\src\gui\moc_GUISUMOViewParent.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUISUMOViewParent.h -o ..\..\src\gui\moc_GUISUMOViewParent.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUISUMOViewParent.h...
InputPath=..\..\src\gui\GUISUMOViewParent.h

"..\..\src\gui\moc_GUISUMOViewParent.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUISUMOViewParent.h -o ..\..\src\gui\moc_GUISUMOViewParent.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\guisim\GUIVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGnTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_FGwTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGnTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\drawerimpl\GUIVehicleDrawer_SGwTasTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIViewAggregatedLanes.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUIViewAggregatedLanes.h...
InputPath=..\..\src\gui\GUIViewAggregatedLanes.h

"..\..\src\gui\moc_GUIViewAggregatedLanes.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIViewAggregatedLanes.h -o ..\..\src\gui\moc_GUIViewAggregatedLanes.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIViewAggregatedLanes.h...
InputPath=..\..\src\gui\GUIViewAggregatedLanes.h

"..\..\src\gui\moc_GUIViewAggregatedLanes.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIViewAggregatedLanes.h -o ..\..\src\gui\moc_GUIViewAggregatedLanes.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\GUIViewTraffic.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing GUIViewTraffic.h...
InputPath=..\..\src\gui\GUIViewTraffic.h

"..\..\src\gui\moc_GUIViewTraffic.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIViewTraffic.h -o ..\..\src\gui\moc_GUIViewTraffic.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing GUIViewTraffic.h...
InputPath=..\..\src\gui\GUIViewTraffic.h

"..\..\src\gui\moc_GUIViewTraffic.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\GUIViewTraffic.h -o ..\..\src\gui\moc_GUIViewTraffic.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\HelpPrinter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\glutils\lfontrenderer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\LogFile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\LoggedValue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\LoggedValue_Single.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\LoggedValue_TimeFixed.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\LoggedValue_TimeFloating.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\dev\MemDiff.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSActuatedTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSBitSetLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetector2File.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorFileOutput.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorSubSys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\actions\MSDiscreteEventControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEdgeControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEmitControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSEventControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\MsgHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\MsgRetriever.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSInductLoop.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSInternalLane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJunctionControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJunctionLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLaneChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLaneSpeedTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLaneState.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLaneStateReminder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLink.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLinkCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSMoveReminder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSNet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSNoLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRightOfWayJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRoute.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRouteHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRouteLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSRouteLoaderControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSSimpleTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSSource.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSSourceLane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTLLogicControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTravelcostDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredReader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredSource.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredSourceXMLHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSTriggeredXMLReader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\NamedObjectCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\qutils\NewQMutex.h
# End Source File
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
# Begin Source File

SOURCE=..\..\src\utils\options\Option.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsIO.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsParser.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsSaveStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\options\OptionsSubSys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Position2D.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QAboutSUMO.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QAboutSUMO.h ...
InputPath=..\..\src\gui\qdialogs\QAboutSUMO.h

"..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QAboutSUMO.h -o ..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QAboutSUMO.h ...
InputPath=..\..\src\gui\qdialogs\QAboutSUMO.h

"..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QAboutSUMO.h -o ..\..\src\gui\qdialogs\moc_QAboutSUMO.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QApplicationSettings.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QApplicationSettings.h ...
InputPath=..\..\src\gui\qdialogs\QApplicationSettings.h

"..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QApplicationSettings.h -o ..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QApplicationSettings.h ...
InputPath=..\..\src\gui\qdialogs\QApplicationSettings.h

"..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QApplicationSettings.h -o ..\..\src\gui\qdialogs\moc_QApplicationSettings.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\popup\QGLObjectPopupMenu.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QGLObjectPopupMenu.h ...
InputPath=..\..\src\gui\popup\QGLObjectPopupMenu.h

"..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\popup\QGLObjectPopupMenu.h -o ..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QGLObjectPopupMenu.h ...
InputPath=..\..\src\gui\popup\QGLObjectPopupMenu.h

"..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\popup\QGLObjectPopupMenu.h -o ..\..\src\gui\popup\moc_QGLObjectPopupMenu.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\popup\QGLObjectPopupMenuItem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QGLObjectToolTip.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QGUIImageField.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QGUIImageField.h...
InputPath=..\..\src\gui\QGUIImageField.h

"..\..\src\gui\moc_QGUIImageField.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\QGUIImageField.h -o ..\..\src\gui\moc_QGUIImageField.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QGUIImageField.h...
InputPath=..\..\src\gui\QGUIImageField.h

"..\..\src\gui\moc_QGUIImageField.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\QGUIImageField.h -o ..\..\src\gui\moc_QGUIImageField.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QGUIToggleButton.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QGUIToggleButton.h...
InputPath=..\..\src\gui\QGUIToggleButton.h

"..\..\src\gui\moc_QGUIToggleButton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\QGUIToggleButton.h -o ..\..\src\gui\moc_QGUIToggleButton.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QGUIToggleButton.h...
InputPath=..\..\src\gui\QGUIToggleButton.h

"..\..\src\gui\moc_QGUIToggleButton.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\QGUIToggleButton.h -o ..\..\src\gui\moc_QGUIToggleButton.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QMessageEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QMicroscopicViewSettings.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QMicroscopicViewSettings.h...
InputPath=..\..\src\gui\qdialogs\QMicroscopicViewSettings.h

"..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QMicroscopicViewSettings.h -o ..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QMicroscopicViewSettings.h...
InputPath=..\..\src\gui\qdialogs\QMicroscopicViewSettings.h

"..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QMicroscopicViewSettings.h -o ..\..\src\gui\qdialogs\moc_QMicroscopicViewSettings.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\partable\QParamPopupMenu.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QParamPopupMenu.h...
InputPath=..\..\src\gui\partable\QParamPopupMenu.h

"..\..\src\gui\partable\moc_QParamPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\partable\QParamPopupMenu.h -o ..\..\src\gui\partable\moc_QParamPopupMenu.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QParamPopupMenu.h...
InputPath=..\..\src\gui\partable\QParamPopupMenu.h

"..\..\src\gui\partable\moc_QParamPopupMenu.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\partable\QParamPopupMenu.h -o ..\..\src\gui\partable\moc_QParamPopupMenu.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QSimulationEndedEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QSimulationLoadedEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\qdialogs\QSimulationSettings.h

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Moc'ing QSimulationSettings.h ...
InputPath=..\..\src\gui\qdialogs\QSimulationSettings.h

"..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QSimulationSettings.h -o ..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Moc'ing QSimulationSettings.h ...
InputPath=..\..\src\gui\qdialogs\QSimulationSettings.h

"..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe ..\..\src\gui\qdialogs\QSimulationSettings.h -o ..\..\src\gui\qdialogs\moc_QSimulationSettings.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QSimulationStepEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\QSUMOEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\RetrievableValue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\gfx\RGBColor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\STRConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\src\sumo_only\SUMOFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOSAXHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOXMLDefinitions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\SystemFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\TplConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\TplConvertSec.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gui\vartracker\TrackerValueDesc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\UIntFunction2DoubleBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\UIntParametrisedDblFuncBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\XMLBuildingExceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\XMLSubSys.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE="..\..\..\..\..\libs\qt\lib\qt-mt230nc.lib"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\libs\qt\lib\qutil.lib
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\libs\xerces-c_2_3_0-win32\lib\xerces-c_2.lib"
# End Source File
# End Target
# End Project
