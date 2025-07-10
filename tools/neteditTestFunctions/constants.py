# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    constants.py
# @author  Pablo Alvarez Lopez
# @date    28-05-25

# imports
import os

# files
NETEDIT_APP = os.environ.get("NETEDIT_BINARY", "netedit")
TEXTTEST_SANDBOX = os.environ.get("TEXTTEST_SANDBOX", os.getcwd())
REFERENCE_PNG = os.path.join(os.path.dirname(__file__), "reference.png")

# Delays between operations
DELAY_KEY = 0.2
DELAY_DRAGDROP = 3
DELAY_KEY_TAB = 0.2
DELAY_MOUSE_MOVE = 0.5
DELAY_MOUSE_CLICK = 1
DELAY_QUESTION = 2
DELAY_SAVING = 1
DELAY_RELOAD = 3
DELAY_QUIT_NETEDIT = 5
DELAY_UNDOREDO = 1
DELAY_SELECT = 1
DELAY_RECOMPUTE = 3
DELAY_RECOMPUTE_VOLATILE = 5
DELAY_REMOVESELECTION = 2
DELAY_REFERENCE = 15
