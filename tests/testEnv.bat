REM Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
REM Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
REM This program and the accompanying materials
REM are made available under the terms of the Eclipse Public License v2.0
REM which accompanies this distribution, and is available at
REM http://www.eclipse.org/legal/epl-v20.html
REM SPDX-License-Identifier: EPL-2.0
set TEXTTEST_HOME=%~dp0
IF "%SUMO_HOME%"=="" SET SUMO_HOME=%~dp0\..
set ACTIVITYGEN_BINARY=%~dp0\..\bin\activitygen%1.exe
set DFROUTER_BINARY=%~dp0\..\bin\dfrouter%1.exe
set DUAROUTER_BINARY=%~dp0\..\bin\duarouter%1.exe
set JTRROUTER_BINARY=%~dp0\..\bin\jtrrouter%1.exe
set NETCONVERT_BINARY=%~dp0\..\bin\netconvert%1.exe
set NETEDIT_BINARY=%~dp0\..\bin\netedit%1.exe
set NETGENERATE_BINARY=%~dp0\..\bin\netgenerate%1.exe
set OD2TRIPS_BINARY=%~dp0\..\bin\od2trips%1.exe
set SUMO_BINARY=%~dp0\..\bin\sumo%1.exe
set POLYCONVERT_BINARY=%~dp0\..\bin\polyconvert%1.exe
set GUISIM_BINARY=%~dp0\..\bin\sumo-gui%1.exe
set MAROUTER_BINARY=%~dp0\..\bin\marouter%1.exe
set EMISSIONSDRIVINGCYCLE_BINARY=%~dp0\..\bin\emissionsDrivingCycle%1.exe
set EMISSIONSMAP_BINARY=%~dp0\..\bin\emissionsMap%1.exe
set PYTHON=python

SET TEXTTESTPY=texttest.py
python -c "import texttestlib"
IF NOT ERRORLEVEL 1 SET TEXTTESTPY=texttest.pyw
where.exe texttest.exe > NUL 2> NUL
IF NOT ERRORLEVEL 1 SET TEXTTESTPY=texttest.exe
