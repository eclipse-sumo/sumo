@echo off
rem ===========================================================================
rem  SUMO per-user removal (no administrator rights required)
rem
rem  Reverses everything written by sumo-setup-user.bat:
rem    * SUMO_HOME removed, SUMO entries filtered out of PATH / PYTHONPATH
rem    * .sumocfg / .netecfg associations deleted
rem    * .xml / .gz context menu entries deleted
rem
rem  Place this file in the SUMO bin\ directory and run it by double-clicking;
rem  SUMO_HOME is derived as the parent of that directory. Other entries in
rem  PATH / PYTHONPATH are kept intact.
rem ===========================================================================

setlocal EnableExtensions EnableDelayedExpansion

rem --- SUMO_HOME = parent of this script's directory (script lives in bin\) ---
for %%I in ("%~dp0..") do set "SUMO_HOME=%%~fI"

echo Removing SUMO settings for the current user.
echo SUMO_HOME = %SUMO_HOME%
echo.

rem ===========================================================================
rem  Environment variables (HKCU\Environment)
rem ===========================================================================

rem --- SUMO_HOME: only delete if it still points at this installation --------
set "REG_SUMO_HOME="
for /f "tokens=2,*" %%A in ('reg query "HKCU\Environment" /v SUMO_HOME 2^>nul') do set "REG_SUMO_HOME=%%B"
if /i "!REG_SUMO_HOME!"=="%SUMO_HOME%" (
    reg delete "HKCU\Environment" /v SUMO_HOME /f >nul 2>nul
    echo SUMO_HOME removed.
) else (
    echo SUMO_HOME does not match this installation - left unchanged.
)

rem --- PATH: filter out the bin and tools entries ----------------------------
set "USERPATH="
for /f "tokens=2,*" %%A in ('reg query "HKCU\Environment" /v Path 2^>nul') do set "USERPATH=%%B"

set "NEWPATH=%USERPATH%"
set "NEWPATH=!NEWPATH:;%SUMO_HOME%\bin=!"
set "NEWPATH=!NEWPATH:%SUMO_HOME%\bin;=!"
set "NEWPATH=!NEWPATH:%SUMO_HOME%\bin=!"
set "NEWPATH=!NEWPATH:;%SUMO_HOME%\tools=!"
set "NEWPATH=!NEWPATH:%SUMO_HOME%\tools;=!"
set "NEWPATH=!NEWPATH:%SUMO_HOME%\tools=!"

rem tidy up a stray leading or trailing semicolon
if "!NEWPATH:~0,1!"==";" set "NEWPATH=!NEWPATH:~1!"
if "!NEWPATH:~-1!"==";" set "NEWPATH=!NEWPATH:~0,-1!"

if "!NEWPATH!"=="" (
    reg delete "HKCU\Environment" /v Path /f >nul 2>nul
) else (
    reg add "HKCU\Environment" /v Path /t REG_EXPAND_SZ /d "!NEWPATH!" /f >nul
)

rem --- PYTHONPATH: filter out the tools entry --------------------------------
set "USERPYPATH="
for /f "tokens=2,*" %%A in ('reg query "HKCU\Environment" /v PYTHONPATH 2^>nul') do set "USERPYPATH=%%B"

set "NEWPYPATH=%USERPYPATH%"
set "NEWPYPATH=!NEWPYPATH:;%SUMO_HOME%\tools=!"
set "NEWPYPATH=!NEWPYPATH:%SUMO_HOME%\tools;=!"
set "NEWPYPATH=!NEWPYPATH:%SUMO_HOME%\tools=!"

if "!NEWPYPATH:~0,1!"==";" set "NEWPYPATH=!NEWPYPATH:~1!"
if "!NEWPYPATH:~-1!"==";" set "NEWPYPATH=!NEWPYPATH:~0,-1!"

if "!NEWPYPATH!"=="" (
    reg delete "HKCU\Environment" /v PYTHONPATH /f >nul 2>nul
) else (
    reg add "HKCU\Environment" /v PYTHONPATH /t REG_EXPAND_SZ /d "!NEWPYPATH!" /f >nul
)

echo Environment variables cleaned up.

rem ===========================================================================
rem  File associations (HKCU\Software\Classes)
rem ===========================================================================
reg delete "HKCU\Software\Classes\.sumocfg" /f >nul 2>nul
reg delete "HKCU\Software\Classes\Sumo.Document" /f >nul 2>nul
reg delete "HKCU\Software\Classes\.netecfg" /f >nul 2>nul
reg delete "HKCU\Software\Classes\SumoNetedit.Document" /f >nul 2>nul

echo File associations removed.

rem ===========================================================================
rem  Context menu entries (HKCU\Software\Classes\SystemFileAssociations)
rem ===========================================================================
reg delete "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoGui" /f >nul 2>nul
reg delete "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoNetedit" /f >nul 2>nul
reg delete "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoGui" /f >nul 2>nul
reg delete "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoNetedit" /f >nul 2>nul

echo Context menu entries removed.
echo.

rem ===========================================================================
rem  Broadcast WM_SETTINGCHANGE so the environment changes take effect without
rem  a logoff. (setx is not usable here because it would re-create a value.)
rem ===========================================================================
powershell -NoProfile -Command "Add-Type -Namespace Win32 -Name NM -MemberDefinition '[DllImport(\"user32.dll\",SetLastError=true)]public static extern IntPtr SendMessageTimeout(IntPtr hWnd,uint Msg,UIntPtr wParam,string lParam,uint fuFlags,uint uTimeout,out UIntPtr lpdwResult);'; $r=[UIntPtr]::Zero; [void][Win32.NM]::SendMessageTimeout([IntPtr]0xffff,0x1A,[UIntPtr]::Zero,'Environment',2,5000,[ref]$r)" >nul 2>nul

echo Done. Newly started programs will no longer see the SUMO settings.
echo Already running programs may need a restart.
echo.
echo Note: this does not uninstall any Python packages installed via pip.
echo       Remove them manually if desired, e.g. pip uninstall libsumo
echo.
pause
endlocal
