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

# @file    mcube.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

"""
This is a dummy plugin preserved only to preserve downward compatibility with old scenarios.
"""

import agilepy.lib_base.classman as cm

from coremodules.demand.demandbase import DemandobjMixin


class MCube(DemandobjMixin, cm.BaseObjman):
    def __init__(self, ident='mcube', demand=None,
                 workdirpath=None, **kwargs):

        self._init_objman(ident=ident, parent=demand,
                          name='MCube Integration', info='MCube Integration',
                          version=0.0,
                          **kwargs)
