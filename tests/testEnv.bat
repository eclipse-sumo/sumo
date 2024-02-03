REM Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
REM Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
REM This program and the accompanying materials are made available under the
REM terms of the Eclipse Public License 2.0 which is available at
REM https://www.eclipse.org/legal/epl-2.0/
REM This Source Code may also be made available under the following Secondary
REM Licenses when the conditions for such availability set forth in the Eclipse
REM Public License 2.0 are satisfied: GNU General Public License, version 2
REM or later which is available at
REM https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
REM SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
SET TEXTTEST_HOME=%~dp0
IF "%SUMO_HOME%"=="" SET SUMO_HOME=%~dp0\..
SET ACTIVITYGEN_BINARY=%SUMO_HOME%\bin\activitygen%1.exe
SET DFROUTER_BINARY=%SUMO_HOME%\bin\dfrouter%1.exe
SET DUAROUTER_BINARY=%SUMO_HOME%\bin\duarouter%1.exe
SET JTRROUTER_BINARY=%SUMO_HOME%\bin\jtrrouter%1.exe
SET NETCONVERT_BINARY=%SUMO_HOME%\bin\netconvert%1.exe
SET NETEDIT_BINARY=%SUMO_HOME%\bin\netedit%1.exe
SET NETGENERATE_BINARY=%SUMO_HOME%\bin\netgenerate%1.exe
SET OD2TRIPS_BINARY=%SUMO_HOME%\bin\od2trips%1.exe
SET SUMO_BINARY=%SUMO_HOME%\bin\sumo%1.exe
SET POLYCONVERT_BINARY=%SUMO_HOME%\bin\polyconvert%1.exe
SET GUISIM_BINARY=%SUMO_HOME%\bin\sumo-gui%1.exe
SET MAROUTER_BINARY=%SUMO_HOME%\bin\marouter%1.exe
SET EMISSIONSDRIVINGCYCLE_BINARY=%SUMO_HOME%\bin\emissionsDrivingCycle%1.exe
SET EMISSIONSMAP_BINARY=%SUMO_HOME%\bin\emissionsMap%1.exe
SET PYTHON=python
SET LANG=C
SET TEXTTESTPY=texttest.exe
