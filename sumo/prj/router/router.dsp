# Microsoft Developer Studio Project File - Name="router" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=router - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "router.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "router.mak" CFG="router - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "router - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "router - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "router - Win32 Release"

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
# ADD CPP /nologo /MD /w /W0 /GX /O2 /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\src" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(XERCES)\lib\xerces-c_2.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\router.exe ..\..\bin\router.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "router - Win32 Debug"

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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 $(XERCES)\lib\xerces-c_2D.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\router.exe ..\..\bin\routerD.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "router - Win32 Release"
# Name "router - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\router\CellDriverInfoParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\router\IDSupplier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\gfx\RGBColor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROArtemisRouteDefHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROCellRouteDefHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROCompleteRouteDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROEdgeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROEdgeVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROLane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RONet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RONetHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RONode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROOrigDestRouteDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORandomTripGenerator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORoute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteAlternativesDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteDefCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteSnippletCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORunningVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROSUMOAltRoutesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROSUMOHandlerBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROSUMORoutesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROTripHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROTypedRoutesLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROTypedXMLRoutesLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicleCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicleType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicleType_Krauss.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehTypeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROWeightsHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\router\TextHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\router\ValueTimeLine.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\router\CellDriverInfoParser.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\HelpPrinter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\router\IDSupplier.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\Named.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\NamedObjectCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ReferencedItem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROArtemisRouteDefHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROCellRouteDefHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROCompleteRouteDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROEdgeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROEdgeVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROLane.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RONet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RONetHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RONode.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RONodeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROOrigDestRouteDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORandomTripGenerator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORoute.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteAlternativesDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteDefCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteDefList.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORouteSnippletCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\RORunningVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROSUMOAltRoutesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROSUMOHandlerBase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROSUMORoutesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROTripHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROTypedRoutesLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROTypedXMLRoutesLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router_help.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicleCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicleType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehicleType_Krauss.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROVehTypeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\router\ROWeightsHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\router\TextHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\router\ValueTimeLine.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
