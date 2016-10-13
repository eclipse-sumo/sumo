call %~dp0\testEnv.bat %1
start %SUMO_HOME%\tests\runSikulixServer.pyw
start %TEXTTESTPY% -a netedit.gui
