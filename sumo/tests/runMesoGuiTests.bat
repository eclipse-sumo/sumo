set TEXTTEST_HOME=%CD%
set SUMO_BINARY=%CD%\..\bin\meso%1.exe
set GUISIM_BINARY=%CD%\..\bin\meso-gui%1.exe
texttest.py -a sumo.meso.gui -gx
