# -*- coding: Latin-1 -*-
"""
@file    Path.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-17

Contains paths which are needed frequently

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""
#main="F:/DLR/Projekte/Diplom/Daten/"
main="D:/Krieg/Projekte/Diplom/Daten/" 

fcd=main+"originalFCD/Proz-fcd_nuremberg_2007-07-18.dat"
simFcd=main+"simProzessiertFCD/nuremberg_sim_on_edges3Mit_t2Complete.dat"
vls=main+"originalFCD/Proz-fcd_nuremberg_VLS_2007-07-18.dat"
rawFcd=main+"originalFCD/Roh-nuremberg_2007-07-18.dat"
rawFcdVehIdList=main+"simRawFCD/rawFcdVehIdList.pickle"
edgeLengthDict=main+"taxiRouten/edgeLengthDict.pickle"
simRawFcd=main+"simRawFCD/simulatedRawFCD3Mit_t2Complete.out.dat" #used input
taxiRoutes=main+"taxiRouten/taxiRoutes.rou.xml"
taxiRoutesComplete=main+"taxiRouten/t2Complete.rou.xml"
net=main+"sumoNetzFilesNurnbergIIProjektion/nuernberg_vls_new.net.xml"
vtypeprobe=main+"simRawFCD/vtypeprobe3Mit_t2Complete.out.xml"
analysis=main+"auswertung/taxiAnalysisInformation.xml"
analysisWEE=main+"auswertung/taxiAnalysisInformationWithoutEmptyEdges.xml"
simulatedRawFCD=main+"simRawFCD/simulatedRawFCD.out.dat" #generated output
taxisPerEdge=main+"mpl_dump_onNet__Files/taxisPerEdge.out.xml"
fcdVsCompleteRoute=main+"mpl_dump_onNet__Files/FCD_vs_completeRoute"
taxiVsFCDSpeed=main+"auswertung/geschwindigkeitsvergleich/taxiVsFCD.csv"
normalTrafficRoutes=main+"sumoNetzFilesNurnbergIV/joined_a5f.rou.xml"
drivenEdges=main+"sumoNetzFilesNurnbergIV/drivenEdgesSet.pickle"
vOverTimeDir=main+"auswertung/Geschwindigkeitsganglinie/ZeitproKante/"
vOverRouteDir=main+"auswertung/GeschwindigkeitUeberRoute/"