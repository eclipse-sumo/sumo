# Microsoft Developer Studio Project File - Name="netgen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=netgen - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "netgen.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "netgen.mak" CFG="netgen - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "netgen - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "netgen - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "netgen - Win32 Release"

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
# ADD CPP /nologo /w /W0 /GX /O2 /I "d:\libs\xerces-c_2_3_0-win32\include" /I "d:\libs\xerces-c_2_3_0-win32\include\xercesc" /I "..\..\src" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
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
PostBuild_Cmds=copy Release\netgen.exe ..\..\bin\netgen.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "netgen - Win32 Debug"

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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "d:\libs\xerces-c_2_3_0-win32\include" /I "d:\libs\xerces-c_2_3_0-win32\include\xercesc" /I "..\..\src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
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
PostBuild_Cmds=copy Debug\netgen.exe ..\..\bin\netgenD.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "netgen - Win32 Release"
# Name "netgen - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\utils\xml\AttributesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\bezier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Boundery.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Bresenham.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\DoubleVector.cpp
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

SOURCE=..\..\src\utils\geom\GeomConvHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\GeomHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\IntVector.cpp
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

SOURCE=..\..\src\utils\common\MsgHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBContHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBDistribution.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBDistrict.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBDistrictCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBEdgeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBJunctionLogicCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBJunctionTypesMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLinkCliqueContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLoadedTLDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLogicKeyBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBNetBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBNodeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBOwnTLDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequestEdgeLinkIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightLogicCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightLogicVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightPhases.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTypeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen\NGNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen\NGNetElements.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen\NGRandomNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\NIOptionsIO.cpp
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

SOURCE=..\..\src\utils\geom\Position2DVector.cpp
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

SOURCE=..\..\src\utils\common\BoolVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\DoubleVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\FileErrorReporter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\FileHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\GeomConvHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\HaveBoundery.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\HelpPrinter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\IntVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\LogFile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\MsgHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\MsgRetriever.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\Named.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\NamedObjectCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBCapacity2Lanes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBConnectionDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBContHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBDistribution.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBDistrict.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBDistrictCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBEdgeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBJunctionLogicCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBJunctionTypesMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLinkCliqueContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLinkPossibilityMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLoadedTLDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLogicKeyBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBNode.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBNodeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBOwnTLDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequest.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequestEdgeLinkIterator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightLogicCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightLogicVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTrafficLightPhases.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBTypeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen_help.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen\NGNet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen\NGNetElements.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netgen\NGRandomNet.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\NIOptionsIO.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NLLoadFilter.h
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

SOURCE=..\..\src\utils\convert\STRConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\SystemFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\ToString.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\TplConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\TplConvertSec.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\UtilExceptions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\xml\XMLBuildingExceptions.h
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
