# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    __init__.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09

from __future__ import absolute_import
import os
import warnings

# the pure python version needs to be the first variant to help IDEs finding the docstrings
if 'LIBSUMO_AS_TRACI' not in os.environ and 'LIBTRACI_AS_TRACI' not in os.environ:
    from .main import *  # noqa
else:
    try:
        if 'LIBSUMO_AS_TRACI' in os.environ:
            from libsumo import *  # noqa
            if os.environ['LIBSUMO_AS_TRACI'] != "quiet":
                print("Using libsumo as traci as requested by environment variable.")
        else:
            from libtraci import *  # noqa
            if os.environ['LIBTRACI_AS_TRACI'] != "quiet":
                print("Using libtraci as traci as requested by environment variable.")
    except ImportError:
        if 'LIBSUMO_AS_TRACI' in os.environ:
            warnings.warn("Could not import libsumo, falling back to pure python traci.")
        else:
            warnings.warn("Could not import libtraci, falling back to pure python traci.")
        from .main import *  # noqa
