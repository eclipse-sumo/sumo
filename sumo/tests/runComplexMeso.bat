call %~dp0\testEnv.bat %1
set SUMO_BINARY=%~dp0\..\bin\meso%1.exe
%TEXTTESTPY% -a complex.meso
