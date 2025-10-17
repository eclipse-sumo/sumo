@echo off
REM Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
REM Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
REM This program and the accompanying materials are made available under the
REM terms of the Eclipse Public License 2.0 which is available at
REM https://www.eclipse.org/legal/epl-2.0/
REM This Source Code may also be made available under the following Secondary
REM Licenses when the conditions for such availability set forth in the Eclipse
REM Public License 2.0 are satisfied: GNU General Public License, version 2
REM or later which is available at
REM https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
REM SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

REM Batch launcher for Visum XML export
REM Tries Visum Python, fallback to any Python with pywin32


REM Scan Program Files for VISUM Python
for /d %%d in ("C:\Program Files\PTV Vision\PTV Visum*" "C:\Program Files (x86)\PTV Vision\PTV Visum*") do (
    if exist %%d\Exe\Python\python.exe (
        set PYTHON_EXE=%%d\Exe\Python\python.exe
        goto :found_python
    )
    if exist %%d\Exe\PythonModules\Scripts\python.exe (
        set PYTHON_EXE=%%d\Exe\PythonModules\Scripts\python.exe
        goto :found_python
    )
)

REM Fallback to any Python in PATH
echo Visum Python not found, trying Python from PATH...
for %%p in (py,python) do (
    call %%p -c "import win32com.client" 2> NUL
    if %errorlevel%==0 (
        set PYTHON_EXE=%%p
        goto :found_python
    )
)

REM Scan Program Files for any Python
for /d %%d in ("C:\Program Files\Python*" "C:\Program Files (x86)\Python*") do (
    call %%d\python.exe -c "import win32com.client" 2> NUL
    if %errorlevel%==0 (
        set PYTHON_EXE=%%d\python.exe
        goto :found_python
    )
)

REM Fail if no suitable Python found
echo Could not find a Python with pywin32 installed!
pause
exit /b 1


:found_python
"%PYTHON_EXE%" "%~dp0\visum_export.py" %*
