# Microsoft Developer Studio Project File - Name="netconvert" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=netconvert - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "netconvert.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "netconvert.mak" CFG="netconvert - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "netconvert - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "netconvert - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "netconvert - Win32 Release"

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
# ADD CPP /nologo /w /W0 /GX /O2 /I "d:\libs\xerces\include" /I "d:\libs\xerces\include\xercesc" /I "..\..\src" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "netconvert - Win32 Debug"

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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "d:\libs\xerces\include" /I "d:\libs\xerces\include\xercesc" /I "..\..\src" /D "_DEBUG" /D "_AFXDLL" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "netconvert - Win32 Release"
# Name "netconvert - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=..\..\src\utils\distribution\Distribution_MeanDev.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\distribution\Distribution_Points.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\distribution\DistributionCont.cpp
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

SOURCE=..\..\src\utils\geom\GeomHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\IntVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\importio\LineReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\dev\MemDiff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\importio\NamedColumnsParser.cpp
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

SOURCE=..\..\src\netbuild\NBLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBLogicKeyBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBNodeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBOptionsIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequestEdgeLinkIterator.cpp
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

SOURCE=..\..\src\netconvert_main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\arcview\NIArcView_Loader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\arcview\NIArcView_ShapeReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_HVdests.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Lanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Links.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Nodes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Segments.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_SignalGroups.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_SignalPhases.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Signals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisTempEdgeLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisTempSegments.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisTempSignal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\cell\NICellEdgesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\cell\NICellNodesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\sumo\NISUMOHandlerDepth.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\sumo\NISUMOHandlerEdges.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\sumo\NISUMOHandlerNodes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimAbstractEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimBoundedClusterObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimClosedLaneDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimClosures.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimConnectionCluster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimDistrictConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimDisturbance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimEdgePosMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimExtendedEdgePoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\NIVissimLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeCluster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Edges.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Poly.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeParticipatingEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Detektordefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_DynUml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrverhaltendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugklassendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugtypdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Haltestellendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kantensperrung.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kennungszeile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Knotendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Laengenverteilungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Langsamfahrbereichdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Lichtsignalanlagendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Liniendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Messungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Parkplatzdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querschnittsmessungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Reisezeitmessungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungsentscheidungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungspfeildefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Routenentscheidungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgeberdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgruppendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Simdauer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Startuhrzeit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stauzaehlerdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stopschilddefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckentypdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verbindungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_VWunschentscheidungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zeitenverteilungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zuflussdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimTL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimTrafficDescription.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimVehicleClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimVehicleType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimVehTypeClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Connectors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Districts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Edges.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Nodes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Turns.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Types.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_VSysTypes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLConnectionsHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLEdgesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLNodesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLTypesHandler.cpp
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

SOURCE=..\..\src\utils\geom\Position2DVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\dev\profile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\gfx\RGBColor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\SErrorHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\SLogging.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\STRConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringTokenizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\importio\StringUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOSAXHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\sumoxml\SUMOXMLDefinitions.cpp
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

SOURCE=..\..\src\utils\geom\AbstractPoly.h
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

SOURCE=..\..\src\utils\distribution\Distribution.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\distribution\Distribution_MeanDev.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\distribution\Distribution_Points.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\distribution\DistributionCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\distribution\DistributionDefinition_Points.h
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

SOURCE=..\..\src\utils\xml\GenericSAX2Handler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\GeomHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\HelpPrinter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\IntVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\importio\LineHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\importio\LineReader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\Named.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\importio\NamedColumnsParser.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\NamedObjectCont.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBCapacity2Lanes.h
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

SOURCE=..\..\src\netbuild\NBLoader.h
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

SOURCE=..\..\src\netbuild\NBOptionsIO.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequest.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netbuild\NBRequestEdgeLinkIterator.h
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

SOURCE=..\..\src\netconvert_help.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\arcview\NIArcView_Loader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\arcview\NIArcView_ShapeReader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_HVdests.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Lanes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Links.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Nodes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Segments.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_SignalGroups.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_SignalPhases.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisParser_Signals.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisTempEdgeLanes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisTempSegments.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\artemis\NIArtemisTempSignal.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\cell\NICellEdgesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\cell\NICellNodesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\sumo\NISUMOHandlerDepth.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\sumo\NISUMOHandlerEdges.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\sumo\NISUMOHandlerNodes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimAbstractEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimBoundedClusterObject.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimClosedLaneDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimClosedLanesVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimClosures.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimConnectionCluster.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimDistrictConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimDisturbance.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimEdgePosMap.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\NIVissimElements.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimExtendedEdgePoint.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimExtendedEdgePointVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\NIVissimLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeCluster.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Edges.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Poly.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeParticipatingEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimNodeParticipatingEdgeVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Detektordefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_DynUml.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrverhaltendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugklassendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugtypdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Haltestellendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kantensperrung.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kennungszeile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Knotendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Laengenverteilungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Langsamfahrbereichdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Lichtsignalanlagendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Liniendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Messungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Parkplatzdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querschnittsmessungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Reisezeitmessungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungsentscheidungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungspfeildefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Routenentscheidungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgeberdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgruppendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Simdauer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Startuhrzeit.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stauzaehlerdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stopschilddefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckentypdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verbindungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_VWunschentscheidungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zeitenverteilungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zuflussdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimSource.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimTL.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimTrafficDescription.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimVehicleClass.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimVehicleClassVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimVehicleType.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\vissim\tempstructs\NIVissimVehTypeClass.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Connectors.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Districts.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Edges.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Nodes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Turns.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_Types.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\visum\NIVisumParser_VSysTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLConnectionsHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLEdgesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLNodesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\netimport\xml\NIXMLTypesHandler.h
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

SOURCE=..\..\src\utils\geom\Position2D.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\geom\Position2DVector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\dev\profile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\SErrorHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\SLogging.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\convert\STRConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\common\StringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\importio\StringUtils.h
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

SOURCE=..\..\..\..\..\libs\xerces\lib\xerces.lib
# End Source File
# End Target
# End Project
