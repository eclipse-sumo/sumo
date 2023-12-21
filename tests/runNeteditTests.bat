@echo off
REM Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
REM Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
REM This program and the accompanying materials are made available under the
REM terms of the Eclipse Public License 2.0 which is available at
REM https://www.eclipse.org/legal/epl-2.0/
REM This Source Code may also be made available under the following Secondary
REM Licenses when the conditions for such availability set forth in the Eclipse
REM Public License 2.0 are satisfied: GNU General Public License, version 2
REM or later which is available at
REM https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
REM SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

call %~dp0\testEnv.bat %1
start %TEXTTESTPY% -a netedit.gui -gui -l
