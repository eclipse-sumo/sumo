# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    taxi.py
# @author  Joerg Schweizer
# @date   2012

"""
taxi service module

In order to use taxis, the at least one vtype needs to have the taxi device
"""
import os
import sys
import numpy as np


from agilepy.lib_base.processes import Process
#from xml.sax import saxutils, parse, handler

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from coremodules.demand.demandbase import DemandobjMixin


class TaxiService(DemandobjMixin, cm.BaseObjman):
    def __init__(self,  simulation,
                 name='Taxi service',
                 info='Taxi service configuration. In order to use vehicles as taxis, they must have the taxi device.',
                 version=0.1,
                 **kwargs):
        # print 'TaxiService.__init__'

        self._init_objman(ident='taxiservice', parent=simulation,
                          name=name, info=info,
                          version=0.1,
                          **kwargs)

        attrsman = self.set_attrsman(cm.Attrsman(self))
        self._init_attributes()
        self._init_constants()

        # make TaxiService a demand object as link
        self.get_scenario().demand.add_demandobject(obj=self)

    def get_scenario(self):
        return self.parent.parent

    def _init_constants(self):
        self._typemap = {}
        self.get_attrsman().do_not_save_attrs(['_typemap', ])

    def _init_attributes(self):
        # print 'TaxiService._init_attributes',hasattr(self,'prttransit')
        attrsman = self.get_attrsman()
        scenario = self.get_scenario()

        self.is_enabled = attrsman.add(cm.AttrConf('is_enabled', False,
                                                   groupnames=['options'],
                                                   name='Enabled',
                                                   info="""Enable taxi services.""",
                                                   ))

        self.dispatchalgorithm = attrsman.add(cm.AttrConf('dispatchalgorithm', 'greedy',
                                                          groupnames=['options'],
                                                          name='Dispatch Algorithm',
                                                          choices={'Assigns taxis to customers in the order of reservations': 'greedy',
                                                                   'For each taxi assign closest customer': 'greedyClosest',
                                                                   'Assigns taxis to customers in the order of reservations and pick up others on the way': 'greedyShared',
                                                                   'Assigns taxis to customers in the order of reservations and make detours to pick up others': 'routeExtension',
                                                                   'Algorithms managed by Traci': 'traci'},
                                                          info="Used dispatch Algorithm, see SUMO documentation.",
                                                          tag='device.taxi.dispatch-algorithm',
                                                          ))

        self.period_dispatch = attrsman.add(cm.AttrConf('period_dispatch', 1.0,
                                                        groupnames=['options'],
                                                        name='Dispatch period',
                                                        info="The period between successive calls to the dispatcher.",
                                                        unit='s',
                                                        tag='device.taxi.dispatch-period',
                                                        ))

        self.loss_abs = attrsman.add(cm.AttrConf('loss_abs', -1.0,
                                                 groupnames=['options'],
                                                 name='Absolute loss',
                                                 info="Absolute  acceptable loss for detoures. Applies only to Dispatch Algorithm greedyShared.",
                                                 unit='s',
                                                 ))

        self.loss_rel = attrsman.add(cm.AttrConf('loss_rel', -1.0,
                                                 groupnames=['options'],
                                                 name='Relative loss',
                                                 info="Relative  acceptable loss for detoures. Applies only to Dispatch Algorithm greedyShared.",
                                                 ))

        self.idlealgorithm = attrsman.add(cm.AttrConf('idlealgorithm', 'stop',
                                                      groupnames=['options'],
                                                      name='Idle Algorithm',
                                                      choices=['stop', 'randomCircling'],
                                                      info="Used Idle algorithm that determines the begavior when taxi has no passengers.",
                                                      tag='device.taxi.idle-algorithm',
                                                      ))

        # self.is_generate_taxi = attrsman.add(cm.AttrConf( 'is_generate_taxi',kwargs.get('is_generate_taxi',False),
        #                groupnames = ['options'],
        #                perm='rw',
        #                name = 'Generate taxi',
        #                info = """Generate a certain amount of taxis on all or specific zones. Mode is taxi and all taxi vtypes should have a taxi device.""",
        #                ))

        # self.set_version(0.2)

    def prepare_sim(self):
        pass

    def finish_sim(self):
        pass

    def write_config(self, fd, ident=0):
        """
        Write taxi configuration to sumo configuration file feed.
        """
        print 'TaxiService.write_config is_enabled', self.is_enabled
        if self.is_enabled:
            attrsman = self.get_attrsman()

            fd.write(ident*' '+'<%s value="%s"/>\n' %
                     (attrsman.dispatchalgorithm.tag, str(attrsman.dispatchalgorithm.get_value())))
            fd.write(ident*' '+'<%s value="%s"/>\n' %
                     (attrsman.period_dispatch.tag, str(attrsman.period_dispatch.get_value())))

            if self.dispatchalgorithm == 'greedyShared':
                fd.write(ident*' '+'<%s value="%s"/>\n' % ('relLossThreshold', str(attrsman.loss_rel.get_value())))
                fd.write(ident*' '+'<%s value="%s"/>\n' % ('absLossThreshold', str(attrsman.loss_abs.get_value())))

            fd.write(ident*' '+'<%s value="%s"/>\n' %
                     (attrsman.idlealgorithm.tag, str(attrsman.idlealgorithm.get_value())))
