# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    definitions.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012


import os
APPNAME = "hybridPY"
EXECNAME = "hybridpy"
VERSION = "0.1"
INFO = f"""{APPNAME} is intended to expand the user-base of the traffic micro-simulator SUMO (Simulation of Urban MObility)
an the mesoscopic simulator MATSIM by providing a user-friendly, yet flexible simulation suite.

A further scope of hybridPY is to manage the huge amount of data necessary to run complex multi-modal simulations.
This includes different demand generation models as well as a large range of modes, such as public transport, shared autonomous vehicle (SAV) sevice, private taxis,
bicycles and personal rapid transit (PRT). {APPNAME} consists of a GUI interface, network editor as well as a simple to use scripting language which facilitates the use of SUMO.
"""
LICENSE = f"""{APPNAME} is licensed under Eclipse Public License - v 2.0."""
COPYRIGHT = "(c) 2012-2026 University of Bologna - DICAM, Technical University of Munich"
DEVELOPERS = {
    "Joerg Schweizer": {},
    "Fabian Schuhmann": {},
    "Ngoc An Nguyen": {},
    "Cristian Poliziani": {},
}

URL = ('https://sumo.dlr.de/docs/Contributed/hybridPY/index.html', 'SUMO homepage')

USAGE = f"""USAGE:
from command line:
    Open with empty scenario:
        python {EXECNAME}

    Open new scenario and  import all SUMO xml files with scenariobasename:
        python {EXECNAME} <scenariobasename> <scenariodir>

    Open binary scenario file:
        python {EXECNAME} <path/scenarioname.obj>


use for debugging:
    python {EXECNAME} --debug > debug.txt 2>&1
"""

IMAGEDIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'images')


def get_logopath(kind='icon'):
    if kind == 'icon':
        return os.path.join(IMAGEDIR, 'icon_hybridpy.png')
    elif kind == 'image':
        return os.path.join(IMAGEDIR, 'icon_hybridpy_big.png')
