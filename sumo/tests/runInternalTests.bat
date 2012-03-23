set TEXTTEST_HOME=%CD%
set ACTIVITYGEN_BINARY=%CD%\..\bin\activitygen%1.exe
set DFROUTER_BINARY=%CD%\..\bin\dfrouter%1.exe
set DUAROUTER_BINARY=%CD%\..\bin\duarouterInt%1.exe
set JTRROUTER_BINARY=%CD%\..\bin\jtrrouterInt%1.exe
set NETCONVERT_BINARY=%CD%\..\bin\netconvertInt%1.exe
set NETGEN_BINARY=%CD%\..\bin\netgen%1.exe
set OD2TRIPS_BINARY=%CD%\..\bin\od2trips%1.exe
set SUMO_BINARY=%CD%\..\bin\meso%1.exe
set POLYCONVERT_BINARY=%CD%\..\bin\polyconvert%1.exe
set GUISIM_BINARY=%CD%\..\bin\meso-gui%1.exe
texttest.py -a sumo.internal,sumo.meso,complex.meso,duarouter.astar,duarouter.chrouter,netconvert.internal
