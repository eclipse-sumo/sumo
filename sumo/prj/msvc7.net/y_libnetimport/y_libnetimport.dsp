# Microsoft Developer Studio Project File - Name="y_libnetimport" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=y_libnetimport - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "y_libnetimport.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "y_libnetimport.mak" CFG="y_libnetimport - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "y_libnetimport - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "y_libnetimport - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "y_libnetimport - Win32 Release"

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

!ELSEIF  "$(CFG)" == "y_libnetimport - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "$(XERCES)\include" /I "$(XERCES)\include\xercesc" /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
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

# Name "y_libnetimport - Win32 Release"
# Name "y_libnetimport - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\dbfopen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\NIArcView_Loader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\NIArcView_ShapeReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_HVdests.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Lanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Links.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Nodes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Segments.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_SignalGroups.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_SignalPhases.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Signals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisTempEdgeLanes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisTempSegments.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisTempSignal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\cell\NICellEdgesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\cell\NICellNodesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\NILoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\NIOptionsIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\sumo\NISUMOHandlerDepth.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\sumo\NISUMOHandlerEdges.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\sumo\NISUMOHandlerNodes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimAbstractEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimBoundedClusterObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimClosedLaneDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimClosures.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimConnectionCluster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimDistrictConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimDisturbance.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimEdgePosMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimExtendedEdgePoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\NIVissimLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeCluster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Edges.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Poly.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeParticipatingEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser__XKurvedefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser__XVerteilungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Auswertungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Detektordefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_DynUml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Einheitendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Emission.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrtverlaufdateien.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrverhaltendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugklassendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugtypdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fensterdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Gefahrwarnungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Gelbverhaltendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Haltestellendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kantensperrung.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kennungszeile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Knotendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Laengenverteilungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Langsamfahrbereichdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Lichtsignalanlagendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Liniendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Linksverkehr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_LSAKopplungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Messungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Netzobjektdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Parkplatzdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querschnittsmessungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Reisezeitmessungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungsentscheidungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungspfeildefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Routenentscheidungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgeberdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgruppendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Simdauer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_SimRate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Startuhrzeit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Startzufallszahl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stauparameterdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stauzaehlerdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stopschilddefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckendefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckentypdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_TEAPACDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verbindungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_VWunschentscheidungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zeitenverteilungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zeitschrittfaktor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zuflussdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zusammensetzungsdefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimTL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimTrafficDescription.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimVehicleClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimVehicleType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimVehTypeClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Connectors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Districts.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Edges.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Nodes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_NodesToTrafficLights.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Phases.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_SignalGroups.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_SignalGroupsToPhases.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_TrafficLights.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Turns.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_TurnsToSignalGroups.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Types.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_VSysTypes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumTL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLConnectionsHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLEdgesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLNodesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLTypesHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\shapereader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\shpopen.c
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\NIArcView_Loader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\NIArcView_ShapeReader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_HVdests.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Lanes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Links.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Nodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Segments.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_SignalGroups.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_SignalPhases.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisParser_Signals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisTempEdgeLanes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisTempSegments.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\artemis\NIArtemisTempSignal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\cell\NICellEdgesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\cell\NICellNodesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\NILoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\NIOptionsIO.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\sumo\NISUMOHandlerDepth.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\sumo\NISUMOHandlerEdges.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\sumo\NISUMOHandlerNodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimAbstractEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimBoundedClusterObject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimClosedLaneDef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimClosedLanesVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimClosures.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimConnectionCluster.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimDistrictConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimDisturbance.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimEdgePosMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\NIVissimElements.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimExtendedEdgePoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimExtendedEdgePointVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\NIVissimLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeCluster.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Edges.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeDef_Poly.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeParticipatingEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimNodeParticipatingEdgeVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser__XKurvedefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser__XVerteilungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Auswertungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Detektordefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_DynUml.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Einheitendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Emission.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrtverlaufdateien.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrverhaltendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugklassendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fahrzeugtypdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Fensterdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Gefahrwarnungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Gelbverhaltendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Haltestellendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kantensperrung.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Kennungszeile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Knotendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Laengenverteilungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Langsamfahrbereichdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Lichtsignalanlagendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Liniendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Linksverkehr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_LSAKopplungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Messungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Netzobjektdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Parkplatzdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querschnittsmessungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Reisezeitmessungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungsentscheidungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Richtungspfeildefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Routenentscheidungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgeberdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Signalgruppendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Simdauer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_SimRate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Startuhrzeit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Startzufallszahl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stauparameterdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stauzaehlerdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Stopschilddefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckendefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Streckentypdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_TEAPACDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verbindungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_VWunschentscheidungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zeitenverteilungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zeitschrittfaktor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zuflussdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\typeloader\NIVissimSingleTypeParser_Zusammensetzungsdefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimSource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimTL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimTrafficDescription.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimVehicleClass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimVehicleClassVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimVehicleType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\vissim\tempstructs\NIVissimVehTypeClass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumLoader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Connectors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Districts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Edges.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Nodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_NodesToTrafficLights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Phases.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_SignalGroups.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_SignalGroupsToPhases.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_TrafficLights.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Turns.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_TurnsToSignalGroups.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_Types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumParser_VSysTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\visum\NIVisumTL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLConnectionsHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLEdgesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLNodesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\xml\NIXMLTypesHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\shapefil.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netimport\arcview\shapereader.h
# End Source File
# End Group
# End Target
# End Project
