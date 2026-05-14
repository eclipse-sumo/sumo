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

# @file    matsim.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012


import agilepy.lib_base.classman as cm

from coremodules.demand.demandbase import DemandobjMixin
from . import matsim_network
from . import matsim_config
from . import matsim_pt
from . import matsim_demand
from . import matsim_mapmatch


class Matsim(DemandobjMixin, cm.BaseObjman):
    def __init__(self, ident='matsim', demand=None,
                 workdirpath=None, **kwargs):

        self._init_objman(ident=ident, parent=demand,
                          name='MATSim Scenario', info='Main MATSim scenario instance.',
                          version=0.0,
                          **kwargs)

        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.config = attrsman.add(cm.ObjConf(matsim_config.Configuration(self)))
        self.net = attrsman.add(cm.ObjConf(matsim_network.Network(self)))
        self.pt = attrsman.add(cm.ObjConf(matsim_pt.PublicTransport(self)))
        self.mapmatch = attrsman.add(cm.ObjConf(matsim_mapmatch.mapmatch(self)))
        # self.demand = attrsman.add(   cm.ObjConf( matsim_demand.Demand(self) ) )

    def get_scenario(self):
        return self.parent.parent
