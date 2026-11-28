#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
# @date    2026-11-22

import os
import sys
import types
from unittest.mock import MagicMock, patch

TOOLS = os.path.join(os.environ["SUMO_HOME"], "tools")
sys.path.insert(0, TOOLS)

try:
    import tkinter  # noqa
except ImportError:
    tk_mod = types.ModuleType("tkinter")
    for attr in ("BooleanVar", "Canvas", "Menu", "StringVar", "Tk", "Toplevel"):
        setattr(tk_mod, attr, MagicMock)
    tk_mod.LEFT = "left"
    ttk_mod = types.ModuleType("tkinter.ttk")
    for attr in ("Button", "Checkbutton", "Frame", "LabelFrame", "Menubutton", "Label", "Scrollbar", "Entry"):
        setattr(ttk_mod, attr, MagicMock)
    fd_mod = types.ModuleType("tkinter.filedialog")
    fd_mod.askopenfilename = MagicMock(return_value="")
    tk_mod.ttk = ttk_mod
    tk_mod.filedialog = fd_mod
    sys.modules["tkinter"] = tk_mod
    sys.modules["tkinter.ttk"] = ttk_mod
    sys.modules["tkinter.filedialog"] = fd_mod

import sumolib  # noqa
import purgatory.launcher as launcher  # noqa

HELP_SAMPLE = """Input Options:
  -n, --net-file FILE       Load road network description from FILE
                              multiline detail
Processing Options:
  --no-warnings             Disables output of warnings
  --step-length TIME        Defines the step duration
"""
opts = launcher.parse_help_text(HELP_SAMPLE)
assert len(opts) == 3
assert (opts[0].name, opts[0].type, opts[0].category) == ("net-file", "FILE", "Input Options")
assert "multiline detail" in opts[0].help
assert (opts[1].name, opts[1].type, opts[1].category) == ("no-warnings", "BOOL", "Processing Options")
assert (opts[2].name, opts[2].type) == ("step-length", "TIME")

with patch.object(launcher, "BooleanVar", MagicMock()), patch.object(launcher, "Checkbutton", MagicMock()) as cb:
    _w, _v = launcher.buildValueWidget(MagicMock(), "BOOL")
    assert cb.called

with patch.object(launcher, "StringVar", MagicMock()), patch.object(launcher, "Frame", MagicMock()), \
        patch.object(launcher, "Entry", MagicMock()), patch.object(launcher, "Button", MagicMock()) as btn:
    _w2, _v2 = launcher.buildValueWidget(MagicMock(), "FILE")
    assert btn.called

sumo_bin = sumolib.checkBinary("sumo")
launcher.BINDIR = os.path.dirname(sumo_bin)
real_opts = launcher.parse_help(os.path.basename(sumo_bin))
assert any(o.name == "net-file" and "Input" in o.category for o in real_opts)
print("OK parsed=%d cats=%s,%s" % (len(opts), opts[0].category, opts[1].category))
