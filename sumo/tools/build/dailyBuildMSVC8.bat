@echo off

SET FILEPREFIX=msvc8
SET MAKELOG=%FILEPREFIX%buildRelease.log
REM SET RUNLOG=%FILEPREFIX%examplelogs.tar.gz
SET MAKEALLLOG=%FILEPREFIX%buildDebug.log
SET SUMO_BATCH_RESULT=%FILEPREFIX%batch_result
SET SUMO_REPORT=%FILEPREFIX%report
SET REMOTE_DIR=D:\Sumo\copytest

cd ..\..
D:\Programme\svn-win32-1.4.0\bin\svn.exe up > %MAKELOG%

"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\VCExpress.exe" /rebuild Release build\msvc8\prj.sln /out %MAKELOG%
move /Y %MAKELOG% %REMOTE_DIR%

"D:\Programme\Microsoft Visual Studio 8\Common7\IDE\VCExpress.exe" /rebuild Debug build\msvc8\prj.sln /out %MAKEALLLOG%
move /Y %MAKEALLLOG% %REMOTE_DIR%

REM run tests
SET PATH=D:\Programme\TextTest3.8\;%PATH%
cd tests
call testEnv.bat
texttest.py -b
texttest.py -b -coll
xcopy %SUMO_REPORT% %REMOTE_DIR% /Y /E
del %SUMO_REPORT% /Q
