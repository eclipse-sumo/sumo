@echo off

SET FILEPREFIX=msvc8
SET MAKELOG=%FILEPREFIX%buildRelease.log
SET STATUSLOG=%FILEPREFIX%status.log
SET MAKEALLLOG=%FILEPREFIX%buildDebug.log
SET SUMO_BATCH_RESULT=%FILEPREFIX%batch_result
SET SUMO_REPORT=%FILEPREFIX%report
SET SMTP_SERVER=129.247.218.247

cd ..\..
del %MAKELOG% %MAKEALLLOG%
svn.exe up > %MAKELOG% 2>&1

"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\devenv.exe" /rebuild "Release|Win32" build\msvc8\prj.sln /out %MAKELOG%
"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\devenv.exe" /rebuild "Debug|Win32" build\msvc8\prj.sln /out %MAKEALLLOG%

REM run tests
cd tests
SET TEXTTEST_TMP=%CD%\texttesttmp
rmdir /S /Q %TEXTTEST_TMP%
rmdir /S /Q %SUMO_REPORT%
mkdir %SUMO_REPORT%
call testEnv.bat
texttest.py -b %SUMO_REPORT%
texttest.py -b %SUMO_REPORT% -coll
cd ..
tools\build\status.py %MAKELOG% %MAKEALLLOG% %TEXTTEST_TMP% %SMTP_SERVER% > %STATUSLOG%

WinSCP3.com behrisch,sumo@web.sourceforge.net /privatekey=..\..\key.ppk /command "option batch on" "option confirm off" "put tests\%SUMO_REPORT% %MAKELOG% %MAKEALLLOG% %STATUSLOG% /home/groups/s/su/sumo/htdocs/daily/" "exit"
