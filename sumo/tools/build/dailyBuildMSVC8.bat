@echo off

SET FILEPREFIX=msvc8
SET MAKELOG=%FILEPREFIX%buildRelease.log
SET STATUSLOG=%FILEPREFIX%status.log
SET MAKEALLLOG=%FILEPREFIX%buildDebug.log
SET SUMO_BATCH_RESULT=%FILEPREFIX%batch_result
SET SUMO_REPORT=%FILEPREFIX%report
SET SMTP_SERVER=129.247.218.247
SET REMOTE_DIR=D:\Sumo\copytest

cd ..\..
svn.exe up > %MAKELOG%

"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\VCExpress.exe" /rebuild Release build\msvc8\prj.sln /out %MAKELOG%
move /Y %MAKELOG% %REMOTE_DIR%

"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\VCExpress.exe" /rebuild Debug build\msvc8\prj.sln /out %MAKEALLLOG%
move /Y %MAKEALLLOG% %REMOTE_DIR%

REM run tests
SET PATH=D:\Programme\TextTest3.8\;%PATH%
cd tests
SET TEXTTEST_TMP=%CD%\texttesttmp
call testEnv.bat
texttest.py -b %SUMO_REPORT%
texttest.py -b %SUMO_REPORT% -coll
mkdir %REMOTE_DIR%\%SUMO_REPORT%
xcopy %SUMO_REPORT% %REMOTE_DIR%\%SUMO_REPORT% /Y /E
cd ..\tools\build
status.py %REMOTE_DIR%\%MAKELOG% %REMOTE_DIR%\%MAKEALLLOG% %TEXTTEST_TMP% > %REMOTE_DIR%\%STATUSLOG%

WinSCP3.com behrisch@shell.sf.net /privatekey=%REMOTE_DIR%\..\key.ppk /command "option batch on" "option confirm off" "cd sumo-www" "cd daily" "put %REMOTE_DIR%\*.*" "exit"
