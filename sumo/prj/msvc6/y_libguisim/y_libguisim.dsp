# Microsoft Developer Studio Project File - Name="y_libguisim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=y_libguisim - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "y_libguisim.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "y_libguisim.mak" CFG="y_libguisim - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "y_libguisim - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "y_libguisim - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "y_libguisim - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(FOX14)\include" /I "..\..\..\src" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /D "HAVE_CONFIG_H" /D "NDEBUG" /D "FOXDLL" /D "_LIB" /D "WIN32" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "y_libguisim - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "$(FOX14)\include" /I "..\..\..\src" /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /D "HAVE_CONFIG_H" /D "_DEBUG" /D "FOXDLL" /D "_LIB" /D "WIN32" /D "_MBCS" /FD /GZ /c
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

# Name "y_libguisim - Win32 Release"
# Name "y_libguisim - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\guisim\GUI_E2_ZS_Collector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUI_E2_ZS_CollectorOverLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIDetectorWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIE3Collector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIEmitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIGridBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIHelpingJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIInductLoop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIInternalLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIJunctionWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneChanger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneSpeedTrigger.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneStateReporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUINet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUINetWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUINoLogicJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIRightOfWayJunction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIRoute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIRouteHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUISelectionLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUISourceLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUITrafficLightLogicWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUITriggeredRerouter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIVehicleControl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIVehicleType.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\guisim\GUI_E2_ZS_Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUI_E2_ZS_CollectorOverLanes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIDetectorWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIE3Collector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIEdgeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIGridBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIHelpingJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIInductLoop.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIInternalLane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIJunctionWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneChanger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneSpeedTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneStateReporter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUILaneWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUINet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUINetWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUINoLogicJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIRightOfWayJunction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIRoute.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIRouteHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUISelectionLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUISourceLane.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUITrafficLightLogicWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUITriggeredRerouter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIVehicleControl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\guisim\GUIVehicleType.h
# End Source File
# End Group
# End Target
# End Project
