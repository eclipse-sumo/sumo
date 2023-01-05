# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
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

# @file    simplaconfig.py
# @author  Joerg Schweizer
# @date   2012

"""
simpla is a configurable, platooning plugin for the TraCI Python client. It manages the spontaneous formation of vehicle platoons and allows you to define specific behavior for vehicles that travel inside a platoon.

This is achieved by defining additional vehicle types which represent platooning modes, and by providing information, which type (mode) to use for normal, solitary travel, and which one to use when platooning is activated. Further, 'catch-up' modes may be specified, which are used when a potential platooning partner is farther ahead downstream.

For platooning up to four operational modes may be specified apart from the normal traveling mode (see section Configuration for the details):

  *  Platoon leader mode (parametrization of a vehicle driving at the front of a platoon)
  *  Platoon follower mode (parametrization of a vehicle driving behind another vehicle within a platoon)
  *  Platoon catch-up mode (parametrization of a vehicle in range of a platoon, which is feasible for joining)
  *  Platoon catch-up follower mode (parametrization of a vehicle traveling in a platoon, with a platoon leader in catchup mode)
"""
import os
import sys
import numpy as np
# print 'module simplaconfig SUMO_HOME',os.environ['SUMO_HOME'],'SUMO_HOME' in os.environ
try:
    if 'SUMO_HOME' in os.environ:
        tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
        sys.path.append(tools)
    else:
        print("please declare environment variable 'SUMO_HOME'")

    import simpla

except:
    print 'WARNING: No module simpla in syspath. Please provide SUMO_HOME.'

    simpla = None

# local simpla
#import simpla

from agilepy.lib_base.processes import Process
#from xml.sax import saxutils, parse, handler

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from coremodules.demand.demandbase import DemandobjMixin


class SimplaConfig(DemandobjMixin, cm.BaseObjman):
    def __init__(self,  simulation,
                 name='Simple Platooning configuration',
                 info='Configuration of simple platooning service. Simpla is a service that allows to crate platoons in any simulation. The platooning option must be enabled in the Traci micro simulation.',
                 xmltag='configuration',
                 version=0.2,
                 **kwargs):
        # print 'SimplaConfig.__init__',name

        self._init_objman(ident='simplaconfig', parent=simulation,
                          name=name, info=info, xmltag=xmltag,
                          version=0.3,
                          **kwargs)

        attrsman = self.set_attrsman(cm.Attrsman(self))
        self._init_attributes()
        self._init_constants()

        # make PRTservice a demand object as link
        self.get_scenario().demand.add_demandobject(obj=self)

    def get_scenario(self):
        return self.parent.parent

    def _init_constants(self):
        self._typemap = {}
        self.get_attrsman().do_not_save_attrs(['_typemap', ])

    def _init_attributes(self):
        # print 'PrtService._init_attributes',hasattr(self,'prttransit')
        attrsman = self.get_attrsman()
        scenario = self.get_scenario()

        is_update = False
        if not hasattr(self, '_version'):
            # crazy simpla has no version???
            is_update = True
            self._version = 0.0

        elif self.get_version() < 0.2:
            is_update = True

        if is_update:
            # there were config errors in the 0.0 version
            attrsman.delete('tau_follower')
            if hasattr(self, 'dist_min_follower'):
                attrsman.delete('dist_min_follower')
            if hasattr(self, 'sigma_follower'):
                attrsman.delete('sigma_follower')

        if self.get_version() < 0.3:
            # this would remove _private entirely
            # self.is_enabled.del_groupname('_private')
            attrsman.delete('is_enabled')

        self.is_enabled = attrsman.add(cm.AttrConf('is_enabled', False,
                                                   groupnames=['options'],
                                                   name='Enabled',
                                                   info="""Enable platooning""",
                                                   ), is_prepend=True)

        # here we ged classes not vehicle type
        # specific vehicle type within a class will be generated later
        # these are the mode choices which have a defined vehicle type
        modechoices = scenario.demand.vtypes.get_modechoices()

        # print '  modechoices',modechoices

        self.ids_platoonmodes = attrsman.add(cm.ListConf('ids_platoonmodes', [scenario.net.modes.get_id_mode('passenger')],
                                                         groupnames=['options'],
                                                         choices=modechoices,
                                                         name='Platooned modes',
                                                         info="""Transport modes to platoon.
                                            A vehicle is controlled by simpla if its vehicle type ID is part of the given mode IDs.
                                            """,
                                                         #xmltag = 'vehicleSelectors',
                                                         ))

        # self.testlist = attrsman.add(cm.ListConf(  'testlist', ['1','dd','cvc'],
        #                            groupnames = ['state'],
        #                            choices = ['1','dd','cvc','dddd','eeeee'],
        #                            perm='rw',
        #                            is_save = True,
        #                            name = 'Test list',
        #                            info = 'This is a test list',
        #                            xmltag = 'testList',
        #                            ))

        self.time_update = attrsman.add(cm.AttrConf('time_update', 1.0,
                                                    groupnames=['options'],
                                                    name='Man. update time',
                                                    info="Update time for vehicle management.",
                                                    unit='s',
                                                    xmltag='controlRate',
                                                    ))

        self.platoongap_max = attrsman.add(cm.AttrConf('platoongap_max', 15.0,
                                                       groupnames=['options'],
                                                       name='Max. platoon gap',
                                                       info="""Gap (in m.) below which vehicles are considered as a platoon (if their vType parameters allow safe traveling for the actual situation).""",
                                                       unit='m',
                                                       xmltag='maxPlatoonGap',
                                                       ))

        self.time_platoon_splitup = attrsman.add(cm.AttrConf('time_platoon_splitup', 3.0,
                                                             groupnames=['options'],
                                                             name='Platoon splitup time',
                                                             info="Time until a vehicle which maintains a distance larger than max Platoon Gap from its leader within the platoon (or travels on a different lane or behind a vehicle not belonging to its platoon) is split off. Defaults to 3.0 secs.",
                                                             unit='s',
                                                             xmltag='platoonSplitTime',
                                                             ))

        self.dist_catchup = attrsman.add(cm.AttrConf('dist_catchup', 100.0,
                                                     groupnames=['options'],
                                                     name='catchup distance',
                                                     info="""If a catch-up mode was defined, vehicles switch their type to the corresponding catch-up vehicle type as soon as a platoon is ahead closer than the given value (in m.).""",
                                                     unit='m',
                                                     xmltag='catchupDist',
                                                     ))

        self.factor_switch_impatience = attrsman.add(cm.AttrConf('factor_switch_impatience', -1.0,
                                                                 groupnames=['options'],
                                                                 name='switch impatience factor ',
                                                                 info="""The switch impatience factor determines the magnitude of the effect that an increasing waiting time for a mode switch (due to safety constraints) has on the active speed factor of a vehicle. The active speed factor is calculated as activeSpeedFactor = modeSpecificSpeedFactor/(1+impatienceFactor*waitingTime). The default value for the switch impatience factor is 0.1. Providing values <= 0 will deactivate the impatience mechanism.""",
                                                                 xmltag='switchImpatienceFactor',
                                                                 ))

        # self.lanechangemode_original  = attrsman.add(cm.AttrConf(  'lanechangemode_original', 594,
        #                        groupnames = ['options','lanechange-modes'],
        #                        name = 'Lane-change mode original',
        #                        info = """Specifies the binary lane-change mode to be used as original.""",
        #                        ))

        self.lanechangemode_leader = attrsman.add(cm.AttrConf('lanechangemode_leader', 594,
                                                              groupnames=['options', 'lanechange-modes'],
                                                              name='Lane-change mode leader',
                                                              info="""Specifies the binary lane-change mode to be used as leader.""",
                                                              ))
        self.lanechangemode_follower = attrsman.add(cm.AttrConf('lanechangemode_follower', 514,
                                                                groupnames=['options', 'lanechange-modes'],
                                                                name='Lane-change mode follower',
                                                                info="""Specifies the binary lane-change mode to be used as follower.""",
                                                                ))
        self.lanechangemode_catchup = attrsman.add(cm.AttrConf('lanechangemode_catchup', 514,
                                                               groupnames=['options', 'lanechange-modes'],
                                                               name='Lane-change mode catchup',
                                                               info="""Specifies the binary lane-change mode to be used as catchup.""",
                                                               ))

        self.lanechangemode_catchup_follower = attrsman.add(cm.AttrConf('lanechangemode_catchup_follower', 514,
                                                                        groupnames=['options', 'lanechange-modes'],
                                                                        name='Lane-change mode catchup-follower',
                                                                        info="""Specifies the binary lane-change mode to be used as catchup-follower.""",
                                                                        ))

        self.speedfactor_original = attrsman.add(cm.AttrConf('speedfactor_original', 1.0,
                                                             groupnames=['options', 'speedfactors'],
                                                             name='Speed factor original',
                                                             info="""Specifies the speed factor to be used as original.""",
                                                             ))
        self.speedfactor_leader = attrsman.add(cm.AttrConf('speedfactor_leader', 1.0,
                                                           groupnames=['options', 'speedfactors'],
                                                           name='Speed factor leader',
                                                           info="""Specifies the speed factor to be used as leader.""",
                                                           ))
        self.speedfactor_follower = attrsman.add(cm.AttrConf('speedfactor_follower', 2.0,
                                                             groupnames=['options', 'speedfactors'],
                                                             name='Speed factor follower',
                                                             info="""Specifies the speed factor to be used as follower. Must be greater than one.""",
                                                             ))
        self.speedfactor_catchup = attrsman.add(cm.AttrConf('speedfactor_catchup', 2.5,
                                                            groupnames=['options', 'speedfactors'],
                                                            name='Speed factor catchup',
                                                            info="""Specifies the speed factor to be used as catchup. Must be greater than one.""",
                                                            ))
        self.speedfactor_catchup_follower = attrsman.add(cm.AttrConf('speedfactor_catchup_follower', 3.0,
                                                                     groupnames=['options', 'speedfactors'],
                                                                     name='Speed factor catchup follower',
                                                                     info="""Specifies the speed factor to be used as follower. Must be greater than one.""",
                                                                     ))

        self.accelfactor_follower = attrsman.add(cm.AttrConf('accelfactor_follower', 1.0,
                                                             groupnames=['options', 'dynamics'],
                                                             name='Accel. factor follower',
                                                             info="""Specifies the acceleration factor to be used as follower.""",
                                                             ))

        self.decelfactor_follower = attrsman.add(cm.AttrConf('decelfactor_follower', 1.0,
                                                             groupnames=['options', 'dynamics'],
                                                             name='Decel. factor follower',
                                                             info="""Specifies the deceleration factor to be used as follower.""",
                                                             ))

        self.tau_follower = attrsman.add(cm.AttrConf('tau_follower', 0.2,
                                                     groupnames=['options', 'dynamics'],
                                                     name='Reaction time follower',
                                                     info="Follower's reaction time.",
                                                     unit='s',
                                                     ))

        self.dist_min_follower = attrsman.add(cm.AttrConf('dist_min_follower', 0.3,
                                                          groupnames=['options', 'dynamics'],
                                                          name='Min. gap follower',
                                                          info="Follower's reaction time.",
                                                          unit='m',
                                                          ))

        self.sigma_follower = attrsman.add(cm.AttrConf('sigma_follower', 0.0,
                                                       groupnames=['options', 'dynamics'],
                                                       name='Driver follower',
                                                       info="Follower's driver imperfection in driving (between 0 and 1). Used only in follower models  SUMOKrauss, SKOrig.",
                                                       unit='s',
                                                       ))

        self.is_keep_vtypes = attrsman.add(cm.AttrConf('is_keep_vtypes', False,
                                                       groupnames=['options', 'misc'],
                                                       name='Keep platoon vtypes',
                                                       info="Keep platoon specific vtypes after simulation in the vehicle type database. This for reviewing purpose and not required.",
                                                       ))
        self.set_version(0.3)

    def enable(self, is_enabled):
        """Enables simulation with platoons. Must be called before preparing simulation."""
        self.is_enabled = is_enabled

    def prepare_sim(self):
        if self.is_enabled:
            # self.add_vtypes()# done in get_writexmlinfo means in get_vtypes()
            self.export_config()
            print 'Simplaconfig.prepare_sim', self.configfilepath, self.is_enabled
            simpla.load(self.configfilepath)

    def finish_sim(self):
        if self.is_enabled:
            if not self.is_keep_vtypes:
                self.del_vtypes()

    def export_config(self):
        self.configfilepath = self.parent.get_scenario().get_rootfilepath()+'.simpla.xml'
        xm.write_obj_to_xml(self, self.configfilepath)

    def write_xml(self, fd, indent=0):
        """
        Write simpla xml config file
        """
        print 'Simplaconfig.write_xml'
        fd.write(xm.begin(self.xmltag, indent))
        attrsman = self.get_attrsman()
        vtypes = self.parent.get_scenario().demand.vtypes
        ids_sumo_vtypes = vtypes.ids_sumo
        # <controlRate value="0.5" />
        # <maxPlatoonGap value="15.0" />
        # <catchupDist value="100.0" />
        # <switchImpatienceFactor value="0.1" />
        # <platoonSplitTime value="3.0" />
        # <lcMode original="597" leader="597" follower="514" catchup="514" catchupFollower="514" />
        # <speedFactor original="1.0" leader="1.0" follower="2.0" catchup="2.5" catchupFollower="3.0" />
        # <verbosity value="1" />
        # <vTypeMap original="passenger1" leader="leaderVTypeID" follower="followerVTypeID" catchup="catchupVTypeID" catchupFollower="catchupFollowerVTypeID" />

        #ids_plattypes = ['catchup','follower','original']
        #ids_plattypes = self._typemap.keys()
        #ids_vtypes_plat_sumo = []
        # for id_mode in self.ids_platoonmodes:
        #    ids_vtype_sumo= ids_sumo_vtypes[vtypes.select_by_mode(id_mode=id_mode)]
        #    #print '  id_mode',id_mode,'ids_vtype',ids_vtype.tolist()
        #    for id_vtype_sumo in ids_vtype_sumo:
        #        if id_vtype in ids_plattypes
        #            #if id_vtype_sumo.split('_')[-1] not in plattypes:
        #            ids_vtypes_plat_sumo.append(id_vtype_sumo)

        ids_vtypes_plat_sumo = ids_sumo_vtypes[self._typemap.keys()]
        fd.write(xm.start('vehicleSelectors', indent+2))
        fd.write(xm.arr('value', ids_vtypes_plat_sumo, sep=','))
        fd.write(xm.stopit())

        fd.write(xm.start('lcMode', indent+2))
        fd.write(xm.num('leader', self.lanechangemode_leader))
        fd.write(xm.num('follower', self.lanechangemode_follower))
        fd.write(xm.num('catchup', self.lanechangemode_catchup))
        fd.write(xm.num('catchupFollower', self.lanechangemode_catchup_follower))
        fd.write(xm.stopit())

        fd.write(xm.start('speedFactor', indent+2))
        fd.write(xm.num('original', 1.0))
        fd.write(xm.num('leader', self.speedfactor_leader))
        fd.write(xm.num('follower', self.speedfactor_follower))
        fd.write(xm.num('catchup', self.speedfactor_catchup))
        fd.write(xm.num('catchupFollower', self.speedfactor_catchup_follower))
        fd.write(xm.stopit())

        for plattypes in self._typemap.values():
            fd.write(xm.start('vTypeMap', indent+2))
            for plattype, id_vtype in plattypes.iteritems():
                fd.write(xm.num(plattype, ids_sumo_vtypes[id_vtype]))
            fd.write(xm.stopit())

        for attrconfig in attrsman.get_configs():
            if attrconfig.xmltag is not None:
                if (attrconfig.xmltag == 'switchImpatienceFactor') & (attrconfig.get_value() < 0):
                    pass
                else:
                    fd.write(xm.start(attrconfig.xmltag, indent+2))
                    fd.write(xm.num('value', attrconfig.format_value()))
                    fd.write(xm.stopit())

        fd.write(xm.end(self.xmltag, indent))

    def add_vtypes(self):
        """
        Add necessary vtypes.
        This function is called before writing vtypes.
        These vtypes should be deleted after the export.
        """
        print 'Simplaconfig.add_vtypes'
        self._typemap = {}
        vtypes = self.get_scenario().demand.vtypes
        for id_mode in self.ids_platoonmodes:
            ids_vtype = vtypes.select_by_mode(id_mode=id_mode)

            # original is not created but points to the
            #self._add_vtypes(vtypes, ids_vtype, 'original')
            plattype_original = 'original'
            for _id in ids_vtype:
                self._typemap[_id] = {plattype_original: _id}

            co = 0.1  # default coloroffset
            cf = 1.2  # default colorfactor
            self._add_vtypes(vtypes, ids_vtype, 'leader',
                             coloroffset=np.array([co, 0, 0, 1], dtype=np.float32),
                             colorfactor=np.array([cf, 0, 0, 1], dtype=np.float32),
                             carfollowermodel='ACC',
                             )

            self._add_vtypes(vtypes, ids_vtype, 'follower',
                             accelfactor=self.accelfactor_follower,
                             decelfactor=self.decelfactor_follower,
                             tau=self.tau_follower,
                             sigma=self.sigma_follower,
                             dist_min=self.dist_min_follower,
                             coloroffset=np.array([0, co, 0, 1], dtype=np.float32),
                             colorfactor=np.array([0, cf, 0, 1], dtype=np.float32),
                             carfollowermodel='CACC',
                             )

            self._add_vtypes(vtypes, ids_vtype, 'catchup',
                             accelfactor=self.accelfactor_follower,
                             decelfactor=self.decelfactor_follower,
                             tau=self.tau_follower,
                             sigma=self.sigma_follower,
                             dist_min=self.dist_min_follower,
                             coloroffset=np.array([co, co, co, 1], dtype=np.float32),
                             colorfactor=np.array([0, 0, cf, 1], dtype=np.float32),
                             carfollowermodel='ACC',
                             )

            self._add_vtypes(vtypes, ids_vtype, 'catchupFollower',
                             accelfactor=self.accelfactor_follower,
                             decelfactor=self.decelfactor_follower,
                             tau=self.tau_follower,
                             sigma=self.sigma_follower,
                             dist_min=self.dist_min_follower,
                             coloroffset=np.array([0, co, 0, 1], dtype=np.float32),
                             colorfactor=np.array([0, cf, cf, 1], dtype=np.float32),
                             carfollowermodel='CACC',
                             )

    def get_vtypes(self):
        """
        Returns used vtypes for export
        """
        # ATTENTION: here the platoon vtypes are actually added
        if not self.is_enabled:
            return []

        print 'Simpla.get_vtypes'
        plattype_original = 'original'
        # add vtypes for platooning here
        self.add_vtypes()
        # here we return only the additional vtypes
        ids_vtypes_plat = []
        for plattypes in self._typemap.values():
            for plattype, id_vtype in plattypes.iteritems():
                if plattype != plattype_original:
                    ids_vtypes_plat.append(id_vtype)
        # print '  ids_vtypes_plat',ids_vtypes_plat
        return ids_vtypes_plat

    def del_vtypes(self):
        """Delete all necessary vtypes for platooning from vtype database."""

        vtypes = self.get_scenario().demand.vtypes
        plattype_original = 'original'
        ids_vtypes_plat = []
        for plattypes in self._typemap.values():
            for plattype, id_vtype in plattypes.iteritems():
                if plattype != plattype_original:
                    ids_vtypes_plat.append(id_vtype)

        print 'del_vtypes', ids_vtypes_plat
        vtypes.del_rows(ids_vtypes_plat)
        self._typemap = {}

    def _add_vtypes(self, vtypes, ids, plattype,
                    accelfactor=1.0, decelfactor=1.0,
                    tau=np.nan, sigma=np.nan, dist_min=np.nan,
                    coloroffset=np.zeros(4, np.float32),
                    colorfactor=np.ones(4, np.float32),
                    carfollowermodel='Krauss'):
        print '_add_vtypes', ids, plattype
        n = len(ids)
        ids_new = vtypes.add_rows(n=len(ids))
        for colconfig in vtypes.get_attrsman()._colconfigs:
            values = colconfig[ids].copy()

            attrname = colconfig.attrname
            if attrname == 'ids_sumo':
                values += '_'+plattype
            elif attrname == 'accels':
                values *= accelfactor
            elif attrname == 'decels':
                values *= decelfactor

            elif attrname == 'colors':
                values = np.clip((values+coloroffset)*colorfactor, 0, 1)

            elif (not np.isnan(tau)) & (attrname == 'taus'):
                values = np.ones(len(values), dtype=np.float32)*tau

            elif (not np.isnan(sigma)) & (attrname == 'sigmas'):
                values = np.ones(len(values), dtype=np.float32)*sigma

            colconfig.set(ids_new, values=values)

        # adjust emergency breake deceleration to
        # comfort deceleration
        # this reduces headway
        vtypes.decels_apparent[ids_new] = vtypes.decels[ids_new]+0.1
        vtypes.decels_emergency[ids_new] = vtypes.decels[ids_new]+0.1
        # update typemap database
        for _id, _id_new in zip(ids, ids_new):
            if self._typemap.has_key(_id):
                self._typemap[_id][plattype] = _id_new
            else:
                self._typemap[_id] = {plattype: _id_new}

        return ids_new
