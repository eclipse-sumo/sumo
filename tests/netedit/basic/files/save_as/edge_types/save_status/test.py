#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    test.py
# @author  Josh
# @date    2026-11-25

import os
import sys

sys.path.append(os.path.join(os.environ.get("SUMO_HOME", "."), "tools"))
import neteditTestFunctions as netedit  # noqa

neteditProcess, referencePosition = netedit.setupAndStart()

# Neither configured output file should be created until its contents change.
netedit.saveExistentFile("edgeTypes")

netedit.loadFile(referencePosition, "edgeTypes", "edgetypes_B", "typ.xml", 0)
netedit.undo(referencePosition, 1)
netedit.saveExistentFile("edgeTypes")
netedit.quit(neteditProcess)
