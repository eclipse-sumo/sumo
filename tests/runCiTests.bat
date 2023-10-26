@echo off
REM Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
REM Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
REM This program and the accompanying materials
REM are made available under the terms of the Eclipse Public License v2.0
REM which accompanies this distribution, and is available at
REM http://www.eclipse.org/legal/epl-v20.html
REM SPDX-License-Identifier: EPL-2.0

SET SUFFIX=
IF "%2" == "Debug" SET SUFFIX=D
call %~dp0\testEnv.bat %SUFFIX%
SET TEXTTEST_INTERPRETER=
IF "%~x1" == ".py" SET TEXTTEST_INTERPRETER=python 
%TEXTTEST_INTERPRETER% %1 -b ci -v ci %TEXTTEST_CI_APPS%
