REM Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
REM Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
REM This program and the accompanying materials
REM are made available under the terms of the Eclipse Public License v2.0
REM which accompanies this distribution, and is available at
REM http://www.eclipse.org/legal/epl-v20.html
REM SPDX-License-Identifier: EPL-2.0
call %~dp0\testEnv.bat
%1 -b ci -v ci -keeptmp 0 %TEXTTEST_CI_APPS%
