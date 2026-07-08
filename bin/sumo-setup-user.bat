@echo off
rem ===========================================================================
rem  SUMO per-user setup (no administrator rights required)
rem
rem  Mirrors the per-machine MSI behaviour, but writes everything into the
rem  current user's hive instead of HKLM:
rem    * SUMO_HOME, PATH and PYTHONPATH  -> HKCU\Environment
rem    * .sumocfg / .netecfg associations -> HKCU\Software\Classes
rem
rem  Place this file in the SUMO bin\ directory and run it by double-clicking;
rem  SUMO_HOME is derived as the parent of that directory.
rem
rem  Per-user environment:   https://learn.microsoft.com/en-us/windows/win32/procthread/environment-variables
rem  Per-user file types:    https://learn.microsoft.com/en-us/windows/win32/shell/fa-file-types
rem ===========================================================================

setlocal EnableExtensions EnableDelayedExpansion

rem --- SUMO_HOME = parent of this script's directory (script lives in bin\) ---
for %%I in ("%~dp0..") do set "SUMO_HOME=%%~fI"

echo Setting up SUMO for the current user.
echo SUMO_HOME = %SUMO_HOME%
echo.

rem ===========================================================================
rem  Environment variables (HKCU\Environment)
rem ===========================================================================

rem --- SUMO_HOME -------------------------------------------------------------
reg add "HKCU\Environment" /v SUMO_HOME /t REG_SZ /d "%SUMO_HOME%" /f >nul

rem --- PATH: append bin and tools, only if not already present ---------------
set "USERPATH="
for /f "tokens=2,*" %%A in ('reg query "HKCU\Environment" /v Path 2^>nul') do set "USERPATH=%%B"

set "NEWPATH=%USERPATH%"
echo !NEWPATH! | find /i "%SUMO_HOME%\bin" >nul || set "NEWPATH=!NEWPATH!;%SUMO_HOME%\bin"
echo !NEWPATH! | find /i "%SUMO_HOME%\tools" >nul || set "NEWPATH=!NEWPATH!;%SUMO_HOME%\tools"

rem strip a leading semicolon in case the user PATH was empty before
if "!NEWPATH:~0,1!"==";" set "NEWPATH=!NEWPATH:~1!"
reg add "HKCU\Environment" /v Path /t REG_EXPAND_SZ /d "!NEWPATH!" /f >nul

rem --- PYTHONPATH: append tools, only if not already present ------------------
set "USERPYPATH="
for /f "tokens=2,*" %%A in ('reg query "HKCU\Environment" /v PYTHONPATH 2^>nul') do set "USERPYPATH=%%B"

set "NEWPYPATH=%USERPYPATH%"
echo !NEWPYPATH! | find /i "%SUMO_HOME%\tools" >nul || set "NEWPYPATH=!NEWPYPATH!;%SUMO_HOME%\tools"
if "!NEWPYPATH:~0,1!"==";" set "NEWPYPATH=!NEWPYPATH:~1!"
reg add "HKCU\Environment" /v PYTHONPATH /t REG_EXPAND_SZ /d "!NEWPYPATH!" /f >nul

echo Environment variables written.

rem ===========================================================================
rem  File associations (HKCU\Software\Classes)
rem
rem  Note: %%1 is written so that the registry receives a literal %1, which
rem  Windows replaces with the opened file path at launch time.
rem ===========================================================================

rem --- .sumocfg -> Sumo.Document ---------------------------------------------
reg add "HKCU\Software\Classes\.sumocfg" /ve /d "Sumo.Document" /f >nul
reg add "HKCU\Software\Classes\Sumo.Document" /ve /d "SUMO Configuration File" /f >nul
reg add "HKCU\Software\Classes\Sumo.Document\DefaultIcon" /ve /d "%SUMO_HOME%\bin\sumo-gui.exe,0" /f >nul
reg add "HKCU\Software\Classes\Sumo.Document\shell\open\command" /ve /d "\"%SUMO_HOME%\bin\sumo-gui.exe\" -c \"%%1\"" /f >nul

rem --- .netecfg -> SumoNetedit.Document --------------------------------------
reg add "HKCU\Software\Classes\.netecfg" /ve /d "SumoNetedit.Document" /f >nul
reg add "HKCU\Software\Classes\SumoNetedit.Document" /ve /d "Sumo Netedit Configuration File" /f >nul
reg add "HKCU\Software\Classes\SumoNetedit.Document\DefaultIcon" /ve /d "%SUMO_HOME%\bin\netedit.exe,0" /f >nul
reg add "HKCU\Software\Classes\SumoNetedit.Document\shell\open\command" /ve /d "\"%SUMO_HOME%\bin\netedit.exe\" -c \"%%1\"" /f >nul

echo File associations written.

rem ===========================================================================
rem  Explorer context menu entries (HKCU\Software\Classes\SystemFileAssociations)
rem
rem  Scoped to .xml via SystemFileAssociations, which adds verbs to the
rem  extension without taking over its default handler. The key's default
rem  value is the menu text; Icon adds the program icon.
rem
rem  SystemFileAssociations: https://learn.microsoft.com/en-us/windows/win32/shell/fa-progids
rem  Shell verbs:            https://learn.microsoft.com/en-us/windows/win32/shell/launch
rem ===========================================================================

rem --- Open with SUMO GUI ----------------------------------------------------
reg add "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoGui" /ve /d "Open with SUMO GUI" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoGui" /v Icon /d "%SUMO_HOME%\bin\sumo-gui.exe,0" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoGui\command" /ve /d "\"%SUMO_HOME%\bin\sumo-gui.exe\" -c \"%%1\"" /f >nul

rem --- Open with SUMO netedit ------------------------------------------------
reg add "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoNetedit" /ve /d "Open with SUMO netedit" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoNetedit" /v Icon /d "%SUMO_HOME%\bin\netedit.exe,0" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.xml\shell\SumoNetedit\command" /ve /d "\"%SUMO_HOME%\bin\netedit.exe\" -c \"%%1\"" /f >nul

rem --- Same verbs for .gz (Windows keys only off the last extension, so
rem     .xml.gz cannot be targeted specifically; this applies to all .gz). ----
reg add "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoGui" /ve /d "Open with SUMO GUI" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoGui" /v Icon /d "%SUMO_HOME%\bin\sumo-gui.exe,0" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoGui\command" /ve /d "\"%SUMO_HOME%\bin\sumo-gui.exe\" -c \"%%1\"" /f >nul

reg add "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoNetedit" /ve /d "Open with SUMO netedit" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoNetedit" /v Icon /d "%SUMO_HOME%\bin\netedit.exe,0" /f >nul
reg add "HKCU\Software\Classes\SystemFileAssociations\.gz\shell\SumoNetedit\command" /ve /d "\"%SUMO_HOME%\bin\netedit.exe\" -c \"%%1\"" /f >nul

echo Context menu entries written.
echo.

rem ===========================================================================
rem  Broadcast the change so Explorer and new processes pick up the new
rem  environment without a logoff. setx sends WM_SETTINGCHANGE as a side
rem  effect; the value it writes (SUMO_HOME) is identical to the one above.
rem ===========================================================================
setx SUMO_HOME "%SUMO_HOME%" >nul

echo Done. Newly started programs will see the updated environment.
echo Already running programs (including this console's parent) may need a restart.
echo.

rem ===========================================================================
rem  Python / package check (informational only)
rem
rem  Uses the python found on the current PATH, so a virtual environment that
rem  is active in this console is honoured. Each package is import-tested.
rem ===========================================================================
echo Checking Python and optional packages...
where python >nul 2>nul
if errorlevel 1 (
    echo   [MISSING] python not found on PATH. Some SUMO tools ^(e.g. osmWebWizard^) need a Python installation.
) else (
    for %%P in (jupedsim lxml matplotlib pyproj rtree shapely libsumo) do (
        python -c "import %%P" >nul 2>nul && (echo   [ok]      %%P) || (echo   [MISSING] %%P ^(install with: pip install %%P^))
    )
)
echo.

pause
endlocal
