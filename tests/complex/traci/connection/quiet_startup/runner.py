#!/usr/bin/env python
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
# @author  Josh
# @date    2026-11-26

from contextlib import redirect_stdout
import io
import os
import subprocess
import sys
import warnings
from unittest.mock import Mock, patch

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
import traci  # noqa

established = Mock()
established.getVersion.return_value = (1, "test")

for quiet in (False, True):
    messages = io.StringIO()
    with redirect_stdout(messages), patch("traci.main.time.sleep") as sleep:
        with patch("traci.main.connection.Connection", side_effect=[OSError("delayed"), established]):
            assert traci.init(numRetries=1, doSwitch=False, quiet=quiet) == (1, "test")
        sleep.assert_called_once_with(1)
    assert ("Retrying in 1 seconds" in messages.getvalue()) != quiet

for quiet, stdout, expected in ((False, None, None), (True, None, subprocess.DEVNULL),
                                (True, subprocess.PIPE, subprocess.PIPE)):
    messages = io.StringIO()
    with redirect_stdout(messages), warnings.catch_warnings(record=True) as alerts:
        with patch("traci.main.subprocess.Popen") as popen, patch("traci.main.getFreeSocketPort", return_value=8813):
            with patch("traci.main.connect", side_effect=[traci.TraCIException("retry"), established]) as connect:
                assert traci.start(["sumo"], numRetries=1, verbose=True, stdout=stdout, doSwitch=False,
                                   quiet=quiet) == (1, "test")
            assert connect.call_args[1] == {"quiet": quiet}
        assert popen.call_count == 2
        popen.assert_called_with(["sumo", "--remote-port", "8813"], stdout=expected)
    assert ("Calling sumo" in messages.getvalue()) != quiet
    assert bool(alerts) != quiet

with redirect_stdout(io.StringIO()) as messages:
    with patch("traci.main.connection.Connection", side_effect=OSError("unavailable")):
        try:
            traci.connect(numRetries=1, waitBetweenRetries=0, quiet=True)
        except traci.FatalTraCIError as error:
            assert str(error) == "Could not connect in 2 tries"
        else:
            raise AssertionError("Expected connection failure")
    assert not messages.getvalue()
