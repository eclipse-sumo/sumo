call %~dp0\testEnv.bat %1
start %SUMO_HOME%\tools\build\runSikulixServer.pyw
start %TEXTTESTPY% -gui -a netedit.server
