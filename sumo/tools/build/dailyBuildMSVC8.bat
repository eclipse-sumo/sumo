@echo off

IF "%1" == "" GOTO loop

SET FILEPREFIX=msvc8%1
SET MAKELOG=%FILEPREFIX%buildRelease.log
SET STATUSLOG=%FILEPREFIX%status.log
SET MAKEALLLOG=%FILEPREFIX%buildDebug.log
SET SUMO_BATCH_RESULT=%FILEPREFIX%batch_result
SET SUMO_REPORT=%FILEPREFIX%report
SET SMTP_SERVER=129.247.218.247
SET BINARY_ZIP=N:\Daten\Sumo\Nightly\sumo-%FILEPREFIX%-bin.zip

cd ..\..
del %MAKELOG% %MAKEALLLOG%
del bin\*.exe
svn.exe up > %MAKELOG% 2>&1

"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\devenv.exe" /rebuild "Release|%1" build\msvc8\prj.sln /out %MAKELOG%
IF "%1" == "Win32" zip -j %BINARY_ZIP% %PROJ_GDAL%\bin\proj.dll %PROJ_GDAL%\bin\gdal16.dll %XERCES%\bin\xerces-c_3_0.dll %FOX14%\lib\FOXDLL-1.4.dll bin\*.exe
IF "%1" == "x64" zip -j %BINARY_ZIP% %PROJ_GDAL_64%\bin\proj.dll %PROJ_GDAL_64%\bin\gdal16.dll %XERCES_64%\bin\xerces-c_3_0.dll %FOX14%\lib\FOXDLL-1.4.dll bin\*.exe
"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\devenv.exe" /rebuild "Debug|%1" build\msvc8\prj.sln /out %MAKEALLLOG%

REM run tests
cd tests
SET TEXTTEST_TMP=%CD%\texttesttmp
rmdir /S /Q %TEXTTEST_TMP%
rmdir /S /Q %SUMO_REPORT%
mkdir %SUMO_REPORT%
IF "%1" == "Win32" call testEnv.bat
IF "%1" == "x64" call testEnv.bat 64
runGuisimTests.py -b
texttest.py -b %SUMO_REPORT%
texttest.py -b %SUMO_REPORT% -coll
cd ..
tools\build\status.py %MAKELOG% %MAKEALLLOG% %TEXTTEST_TMP% %SMTP_SERVER% > %STATUSLOG%

WinSCP3.com behrisch,sumo@web.sourceforge.net /privatekey=..\..\key.ppk /command "option batch on" "option confirm off" "put tests\%SUMO_REPORT% %MAKELOG% %MAKEALLLOG% %STATUSLOG% %BINARY_ZIP% /home/groups/s/su/sumo/htdocs/daily/" "exit"

cd tools\build
GOTO exit

:loop
FOR %%i IN (Win32 x64) DO CALL %0 %%i

:exit
