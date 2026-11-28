#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Mark Colley
# @date    2026-11-23


"""Check how TraCI reports an unavailable automatically assigned port."""

import os
import sys
from unittest.mock import patch

if "SUMO_HOME" in os.environ:
    sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from traci import main  # noqa
from traci.exceptions import FatalTraCIError  # noqa


with patch("sumolib.miscutils.socket.socket", side_effect=OSError("socket unavailable")), \
        patch("traci.main.subprocess.Popen") as launch:
    try:
        main.start(["sumo"], numRetries=0)
    except FatalTraCIError as error:
        assert str(error) == "Could not find a free TraCI port.", error
    else:
        raise AssertionError("FatalTraCIError was not raised")
    launch.assert_not_called()

print("Automatic port selection failed before SUMO started.")
