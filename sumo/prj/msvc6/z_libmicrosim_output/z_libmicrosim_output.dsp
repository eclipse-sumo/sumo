# Microsoft Developer Studio Project File - Name="z_libmicrosim_output" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=z_libmicrosim_output - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "z_libmicrosim_output.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "z_libmicrosim_output.mak" CFG="z_libmicrosim_output - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "z_libmicrosim_output - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "z_libmicrosim_output - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "z_libmicrosim_output - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\src" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /D "HAVE_CONFIG_H" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "HAVE_MESOSIM" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "z_libmicrosim_output - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\src" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /D "HAVE_CONFIG_H" /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "HAVE_MESOSIM" /YX /FD /GZ /c
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

# Name "z_libmicrosim_output - Win32 Release"
# Name "z_libmicrosim_output - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e2_detectors\MS_E2_ZS_CollectorOverLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDensity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetector2File.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e1_detectors\MSE1Collector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e1_detectors\MSE1MeanSpeed.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e2_detectors\MSE2Collector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e3_detectors\MSE3Collector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSInductLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\meandata\MSMeanData_Net.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\meandata\MSMeanData_Net_Utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSNVehicles.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSXMLRawOut.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e2_detectors\MS_E2_ZS_CollectorOverLanes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSCrossSection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSCurrentHaltingDurationSumPerVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDensity.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetector2File.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorContainerWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorContainerWrapperBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorCounterContainerWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorFileOutput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorHaltingContainerWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorInterfaceCommon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorOccupancyCorrection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSDetectorTypedefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e1_detectors\MSE1.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e1_detectors\MSE1Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e1_detectors\MSE1MeanSpeed.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e1_detectors\MSE1MoveReminder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e2_detectors\MSE2Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e3_detectors\MSE3Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e3_detectors\MSE3MeanNHaltings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e3_detectors\MSE3MoveReminder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e3_detectors\MSE3NVehicles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\e3_detectors\MSE3Traveltime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSEDDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSEDDetectorInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSHaltDuration.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSHaltingDurationSum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSInductLoop.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSJamLengthAheadOfTrafficLights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSJamLengthSum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\meandata\MSLaneMeanDataValues.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSLDDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSLDDetectorInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSMaxJamLength.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\meandata\MSMeanData_Net.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\meandata\MSMeanData_Net_Cont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\meandata\MSMeanData_Net_Utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSMeanDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSNStartedHalts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSNVehicles.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSObserver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSOccupancyDegree.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSPredicates.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSQueueLengthAheadOfTrafficLights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSSpaceMeanSpeed.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSSubject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSSumDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSTDDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSTDDetectorInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\microsim\output\MSXMLRawOut.h
# End Source File
# End Group
# End Target
# End Project
