# Microsoft Developer Studio Project File - Name="sumo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=sumo - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "sumo.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "sumo.mak" CFG="sumo - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "sumo - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "sumo - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sumo - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /w /W0 /GX /O2 /I "d:\libs\xerces-c_2_3_0-win32\include" /I "d:\libs\xerces-c_2_3_0-win32\include\xercesc" /I "..\..\src" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\sumo.exe ..\..\bin\sumo.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "sumo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "d:\libs\xerces-c_2_3_0-win32\include" /I "d:\libs\xerces-c_2_3_0-win32\include\xercesc" /I "..\..\src" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "ABS_DEBUG" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\sumo.exe ..\..\bin\sumoD.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "sumo - Win32 Release"
# Name "sumo - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\utils\xml\AttributesHandler.cpp
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

SOURCE=..\..\src\utils\xml\GenericSAX2Handler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\gfx\GfxConvHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\dev\MemDiff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MS_E2_ZS_CollectorOverLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDensity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetector2File.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorFileOutput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorSubSys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\actions\MSDiscreteEventControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSE2Collector.cpp
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

SOURCE=..\..\src\microsim\MSJamLengthAheadOfTrafficLights.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJamLengthSum.cpp
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

SOURCE=..\..\src\microsim\MSMaxJamLength.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSNoLogicJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSNVehicles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSQueueLengthAheadOfTrafficLights.cpp
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

SOURCE=..\..\src\microsim\MSSimpleTrafficLightLogic.cpp
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

SOURCE=..\..\src\microsim\MSUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleTransfer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleType.cpp
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

SOURCE=..\..\src\utils\options\OptionsSubSys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\helpers\PreStartInitialised.cpp
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

SOURCE=..\..\src\sumo_main.cpp
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

SOURCE=..\..\src\utils\xml\AttributesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\AttributesReadingGenericSAX2Handler.h
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

SOURCE=..\..\src\utils\common\FileErrorReporter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\FileHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\GenericSAX2Handler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\HelpPrinter.h
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

SOURCE=..\..\src\microsim\MS_E2_ZS_Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSActuatedPhaseDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSActuatedTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSAgentbasedTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSBitSetLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDensity.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetector2File.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorContainerBase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorFileOutput.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorSubSys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSDetectorTypedefs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\actions\MSDiscreteEventControl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSE2Detector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSE2DetectorInterface.h
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

SOURCE=..\..\src\microsim\MSHaltingDetectorContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSInductLoop.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSInternalLane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJamLengthAheadOfTrafficLights.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSJamLengthSum.h
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

SOURCE=..\..\src\microsim\MSLink.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLinkCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSMaxJamLength.h
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

SOURCE=..\..\src\microsim\MSOccupancyCorrection.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSPhaseDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSPredicates.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSQueueLengthAheadOfTrafficLights.h
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

SOURCE=..\..\src\microsim\MSSumDetector.h
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

SOURCE=..\..\src\microsim\MSUnit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleTransfer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\microsim\MSVehicleType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\Named.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\NamedObjectCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netload\NLDetectorBuilder.h
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

SOURCE=..\..\src\utils\options\OptionsSubSys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\helpers\PreStartInitialised.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\RetrievableValue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\STRConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\src\sumo_help.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOSAXHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOXMLDefinitions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\TplConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\TplConvertSec.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\UIntFunction2DoubleBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\logging\UIntParametrisedDblFuncBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\UtilExceptions.h
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

SOURCE="..\..\..\..\..\libs\xerces-c_2_3_0-win32\lib\xerces-c_2.lib"
# End Source File
# End Target
# End Project
