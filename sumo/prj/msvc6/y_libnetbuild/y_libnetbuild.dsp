# Microsoft Developer Studio Project File - Name="y_libnetbuild" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=y_libnetbuild - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "y_libnetbuild.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "y_libnetbuild.mak" CFG="y_libnetbuild - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "y_libnetbuild - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "y_libnetbuild - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "y_libnetbuild - Win32 Release"

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
# ADD CPP /nologo /MD /w /W0 /GX /O2 /I "..\..\..\src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "y_libnetbuild - Win32 Debug"

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
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I "..\..\..\src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "y_libnetbuild - Win32 Release"
# Name "y_libnetbuild - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBContHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBDistribution.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBDistrict.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBDistrictCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBEdgeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBHelpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBJoinedEdgesMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBJunctionLogicCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBJunctionTypesMatrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBLinkCliqueContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBLoadedTLDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBLogicKeyBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBNetBuilder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\nodes\NBNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\nodes\NBNodeCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\nodes\NBNodeShapeComputer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBOwnTLDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBRequest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBRequestEdgeLinkIterator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightDefinition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightLogic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightLogicCont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightLogicVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightPhases.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTypeCont.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBCapacity2Lanes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBConnectionDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBContHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBDistribution.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBDistrict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBDistrictCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBEdgeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBHelpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBJoinedEdgesMap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBJunctionLogicCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBJunctionTypesMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBLinkCliqueContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBLinkPossibilityMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBLoadedTLDef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBLogicKeyBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBMMLDirections.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBNetBuilder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\nodes\NBNode.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\nodes\NBNodeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\nodes\NBNodeShapeComputer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBOwnTLDef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBRequest.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBRequestEdgeLinkIterator.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightDefinition.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightLogic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightLogicCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightLogicVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTrafficLightPhases.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NBTypeCont.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\netbuild\NLLoadFilter.h
# End Source File
# End Group
# End Target
# End Project
