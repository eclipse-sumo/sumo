# Microsoft Developer Studio Project File - Name="y_libmicrosim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=y_libmicrosim - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "y_libmicrosim.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "y_libmicrosim.mak" CFG="y_libmicrosim - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "y_libmicrosim - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "y_libmicrosim - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "y_libmicrosim - Win32 Release"

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
# ADD CPP /nologo /MD /w /W0 /GX /O2 /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "y_libmicrosim - Win32 Debug"

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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "ABS_DEBUG" /YX /FD /GZ /c
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

# Name "y_libmicrosim - Win32 Release"
# Name "y_libmicrosim - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\Command_SaveTLCoupledDet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\Command_SaveTLCoupledLaneDet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\Command_SaveTLSState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MS_E2_ZS_CollectorOverLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSActuatedTrafficLightLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSAgentbasedTrafficLightLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSApproachingVehiclesStates.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSCORN.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSCrossSection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSCurrentHaltingDurationSumPerVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDensity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetector2File.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorCounterContainerWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorFileOutput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorInterfaceCommon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorOccupancyCorrection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorSubSys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\MSDiscreteEventControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE2Collector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3Collector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3MeanNHaltings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3NVehicles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3Traveltime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEDDetectorInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEdgeControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEmitControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEventControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSGlobals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSHaltDuration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSHaltingDurationSum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSInductLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSInternalLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJamLengthAheadOfTrafficLights.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJamLengthSum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJunctionControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJunctionLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLaneChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLaneSpeedTrigger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLaneState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLDDetectorInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLinkCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLogicJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSMaxJamLength.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNoLogicJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNStartedHalts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNVehicles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSOccupancyDegree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSQueueLengthAheadOfTrafficLights.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRightOfWayJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRoute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRouteHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRouteLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRouteLoaderControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSimpleTrafficLightLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSourceLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSpaceMeanSpeed.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTDDetectorInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTLLogicControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTrafficLightLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredSourceXMLHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredXMLReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSUnit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleTransfer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleType.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\Action.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\logging\CastingFunctionBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\Command_SaveTLCoupledDet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\Command_SaveTLCoupledLaneDet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\Command_SaveTLSState.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\logging\FuncBinding_UIntParam.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\logging\FunctionBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\logging\LoggedValue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\logging\LoggedValue_Single.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\logging\LoggedValue_TimeFixed.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\logging\LoggedValue_TimeFloating.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MS_E2_ZS_CollectorOverLanes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSActuatedPhaseDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSActuatedTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSAgentbasedTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSApproachingVehiclesStates.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSBitSetLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSCORN.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSCrossSection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSCurrentHaltingDurationSumPerVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDensity.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetector2File.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorContainerWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorContainerWrapperBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorCounterContainerWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorFileOutput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorHaltingContainerWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorInterfaceCommon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorOccupancyCorrection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorSubSys.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSDetectorTypedefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\MSDiscreteEventControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE2Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3MeanNHaltings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3MoveReminder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3NVehicles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSE3Traveltime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEDDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEDDetectorInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEdgeControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEmitControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSEventControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSGlobals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSHaltDuration.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSHaltingDurationSum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSInductLoop.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSInternalLane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJamLengthAheadOfTrafficLights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJamLengthSum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJunctionControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSJunctionLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLaneChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLaneSpeedTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLaneState.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLDDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLDDetectorInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLinkCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSMaxJamLength.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSMeanDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSMoveReminder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNoLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNStartedHalts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNVehicles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSObserver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSOccupancyDegree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSPhaseDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSPredicates.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSQueueLengthAheadOfTrafficLights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRightOfWayJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRoute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRouteHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRouteLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSRouteLoaderControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSimpleTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSourceLane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSpaceMeanSpeed.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSubject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSumDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTDDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTDDetectorInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTLLogicControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTravelcostDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredReader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredSource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredSourceXMLHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTriggeredXMLReader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSUnit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSUpdateEachTimestep.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSUpdateEachTimestepContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleTransfer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSVehicleType.h
# End Source File
# End Group
# End Target
# End Project
