# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    __init__.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09
# @version $Id$

from __future__ import absolute_import
import os
import warnings

if 'LIBSUMO_AS_TRACI' not in os.environ:
    from .main import *  # noqa
else:
    try:
        from libsumo import *  # noqa
    except ImportError:
        warnings.warn("Could not import libsumo, falling back to standard traci.")
        from .main import *  # noqa
