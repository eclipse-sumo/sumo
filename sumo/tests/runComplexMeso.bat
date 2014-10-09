call testEnv.bat %1
set SUMO_BINARY=%CD%\..\bin\meso%1.exe
%TEXTTESTPY% -a complex.meso
