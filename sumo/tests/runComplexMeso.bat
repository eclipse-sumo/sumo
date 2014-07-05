call testEnv.bat %1
set SUMO_BINARY=%CD%\..\bin\meso%1.exe
texttest.py -a complex.meso -gx
