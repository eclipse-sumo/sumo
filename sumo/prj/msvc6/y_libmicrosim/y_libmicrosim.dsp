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
# ADD CPP /nologo /MD /w /W0 /GX /O2 /I "$(FOX)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(FOX)\include" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "ABS_DEBUG" /YX /FD /GZ /c
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

SOURCE=..\..\..\src\microsim\MSCORN.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\MSDiscreteEventControl.cpp
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

SOURCE=..\..\..\src\microsim\MSInternalLane.cpp
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

SOURCE=..\..\..\src\microsim\lanechanging\MSLCM_DK2002.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\lanechanging\MSLCM_DK2004.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\lanechanging\MSLCM_Krauss.cpp
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

SOURCE=..\..\..\src\microsim\MSNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNoLogicJunction.cpp
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

SOURCE=..\..\..\src\microsim\MSSlowLaneChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSSourceLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSTrigger.cpp
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

SOURCE=..\..\..\src\microsim\MSAbstractLaneChangeModel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSBitSetLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSCORN.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\actions\MSDiscreteEventControl.h
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

SOURCE=..\..\..\src\microsim\MSInterface_NetRun.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSInternalLane.h
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

SOURCE=..\..\..\src\microsim\lanechanging\MSLCM_DK2002.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\lanechanging\MSLCM_DK2004.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\lanechanging\MSLCM_Krauss.h
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

SOURCE=..\..\..\src\microsim\MSMoveReminder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSNoLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSPhaseDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\MSPredicates.h
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

SOURCE=..\..\..\src\microsim\MSSlowLaneChanger.h
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
