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

# @file    turnflows.py
# @author  Joerg Schweizer
# @date   2012

import os
import string
import numpy as np
from numpy import random
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.geometry import *
#from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from agilepy.lib_base.processes import Process, P, call, CmlMixin
import demandbase as db


class Flows(am.ArrayObjman):
    def __init__(self, ident, parent, edges, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='Flows',
                          info='Contains the number of vehicles which start on the given edge during a certain time interval.',
                          version=0.2,
                          xmltag=('flows', 'flow', None), **kwargs)

        self._init_attributes(edges)

    def _init_attributes(self, edges=None):
        if edges is None:
            # recover edges from already initialized
            edges = self.ids_edge.get_linktab()

        if self.get_version() < 0.1:
            # update attrs from previous
            # IdsArrayConf not yet modifiable interactively, despite perm = 'rw',!!!
            self.ids_edge.set_perm('rw')
        if hasattr(self, 'func_delete_row'):
            self.func_make_row._is_returnval = False
            self.func_delete_row._is_returnval = False

        self.add_col(am.IdsArrayConf('ids_edge', edges,
                                     groupnames=['state'],
                                     name='Edge ID',
                                     perm='rw',
                                     info='Edge ID of flow.',
                                     xmltag='from',
                                     ))

        self.add_col(am.ArrayConf('flows', 0,
                                  dtype=np.int32,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Flow',
                                  info='Absolute number of vehicles which start on the given edge during a certain time interval.',
                                  xmltag='number',
                                  ))

        self.add(cm.FuncConf('func_make_row', 'on_add_row', None,
                             groupnames=['rowfunctions', '_private'],
                             name='New flow.',
                             info='Add a new flow.',
                             is_returnval=False,
                             ))

        self.add(cm.FuncConf('func_delete_row', 'on_del_row', None,
                             groupnames=['rowfunctions', '_private'],
                             name='Del flow',
                             info='Delete flow.',
                             is_returnval=False,
                             ))

    def on_del_row(self, id_row=None):
        if id_row is not None:
            # print 'on_del_row', id_row
            self.del_row(id_row)

    def on_add_row(self, id_row=None):
        if len(self) > 0:
            # copy previous
            flow_last = self.get_row(self.get_ids()[-1])
            self.add_row(**flow_last)
        else:
            self.add_row(self.suggest_id())

    def _init_constants(self):
        #self.edgeweights_orig = None
        #self.edgeweights_dest = None
        pass

    def add_flow(self, id_edge, flow):
        # print 'Flows.add_flows'
        return self.add_row(ids_edge=id_edge,
                            flows=flow)

    def get_edges(self):
        return self.ids_edge.get_linktab()

    def get_demand(self):
        return self.parent.parent.parent

    def count_left(self, counter):
        """
        Counts the number of vehicles leaving the edge.
        The counter is an array where the index equals the edge ID
        and the value represens the number of entered vehicles per edge.
        """
        ids_flow = self.get_ids()
        counter[self.ids_edge[ids_flow]] += self.flows[ids_flow]

    def export_xml(self, fd, vtype, id_flow, share=1.0, indent=2):
        """
        Generates a line for each edge with a flow.

        id_flow is the flow count and is used to generate a unique flow id


        """
        # TODO: better handling of mode and vtypes, distributions
        # DONE with share
        # <flow id="0" from="edge0" to="edge1" type= "vType" number="100"/>
        ids_eges = []
        ids_sumoeges = self.get_edges().ids_sumo
        i = 0
        # print 'Flows.export_xml vtypes,id_flow,len(self)',vtype,id_flow,len(self)
        for id_edge, flow in zip(self.ids_edge.get_value(), share*self.flows.get_value()):
            # print '    id_edge,flow',id_edge,flow
            ids_eges.append(id_edge)
            id_flow += 1
            fd.write(xm.start('flow'+xm.num('id', id_flow), indent))
            fd.write(xm.num('from', ids_sumoeges[id_edge]))
            fd.write(xm.num('type', vtype))
            fd.write(xm.num('number', int(flow)))
            fd.write(xm.stopit())
            i += 1
        # print '  return ids_eges, id_flow',ids_eges, id_flow
        return ids_eges, id_flow


class Turns(am.ArrayObjman):
    def __init__(self, ident, parent, edges, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='Turn flows',
                          info='The table contains turn probabilities between two edges during a given time interval.',
                          version=0.1,
                          xmltag=('odtrips', 'odtrip', None), **kwargs)

        self._init_attributes(edges)

    def _init_attributes(self, edges=None):
        if edges is None:
            # recover edges from already initialized
            edges = self.ids_fromedge.get_linktab()

        if self.get_version() < 0.1:
            # update attrs from previous
            # IdsArrayConf not yet modifiable interactively, despite perm = 'rw',!!!
            self.ids_fromedge.set_perm('rw')
            self.ids_toedge.set_perm('rw')
        if hasattr(self, 'func_delete_row'):
            self.func_make_row._is_returnval = False
            self.func_delete_row._is_returnval = False

        self.add_col(am.IdsArrayConf('ids_fromedge', edges,
                                     groupnames=['state'],
                                     name='Edge ID from',
                                     info='Edge ID where turn starts.',
                                     xmltag='fromEdge',
                                     ))

        self.add_col(am.IdsArrayConf('ids_toedge', edges,
                                     groupnames=['state'],
                                     name='Edge ID to',
                                     info='Edge ID where turn ends.',
                                     xmltag='toEdge',
                                     ))

        self.add_col(am.ArrayConf('flows', 0,
                                  dtype=np.int32,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Flow',
                                  info='Absolute number of vehicles which pass from "fromedge" to "toedge" during a certain time interval.',
                                  xmltag='number',
                                  ))

        self.add_col(am.ArrayConf('probabilities', 0.0,
                                  dtype=np.float32,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Probab.',
                                  info='Probability to make a turn between "Edge ID from" and "Edge ID to" and .',
                                  xmltag='probability',
                                  ))

        self.add(cm.FuncConf('func_make_row', 'on_add_row', None,
                             groupnames=['rowfunctions', '_private'],
                             name='New turns',
                             info='Add a new turnflow.',
                             is_returnval=False,
                             ))

        self.add(cm.FuncConf('func_delete_row', 'on_del_row', None,
                             groupnames=['rowfunctions', '_private'],
                             name='Del turns',
                             info='Delete turns.',
                             is_returnval=False,
                             ))

    def on_del_row(self, id_row=None):
        if id_row is not None:
            # print 'on_del_row', id_row
            self.del_row(id_row)

    def on_add_row(self, id_row=None):
        if len(self) > 0:
            # copy previous
            flow_last = self.get_row(self.get_ids()[-1])
            self.add_row(**flow_last)
        else:
            self.add_row(self.suggest_id())

    def _init_constants(self):
        #self.edgeweights_orig = None
        #self.edgeweights_dest = None
        pass

    def normalize_turnprobabilities(self):
        """
        Makes sure that sum of turn probabilities from an edge equals 1.
        """
        # TODO: there is a quicker way with picking a set and then select
        # and sum with vectors
        #ids_source = set(self.ids_fromedge.get_values())

        flows_total = {}
        for _id in self.get_ids():
            id_fromedge = self.ids_fromedge[_id]
            if not flows_total.has_key(id_fromedge):
                flows_total[id_fromedge] = 0.0
            flows_total[id_fromedge] += self.flows[_id]

        for _id in self.get_ids():
            if flows_total[self.ids_fromedge[_id]] > 0:
                self.probabilities[_id] = self.flows[_id] / flows_total[self.ids_fromedge[_id]]

    def add_turn(self, id_fromedge, id_toedge, flow):
        print 'Turns.add_turn'
        return self.add_row(ids_fromedge=id_fromedge,
                            ids_toedge=id_toedge,
                            flows=flow,
                            )

    def get_edges(self):
        return self.ids_fromedge.get_linktab()

    def count_entered(self, counter):
        """
        Counts the number of vehicles entered in an edge.
        The counter is an array where the index equals the edge ID
        and the value represens the number of entered vehicles per edge.
        """
        ids_turns = self.get_ids()
        counter[self.ids_toedge[ids_turns]] += self.flows[ids_turns]

    def export_xml(self, fd, indent=0):
        # <edgeRelations>
        # <interval begin="0" end="3600">
        # <edgeRelation from="myEdge0" to="myEdge1" probability="0.2"/>
        # <edgeRelation from="myEdge0" to="myEdge2" probability="0.7"/>
        # <edgeRelation from="myEdge0" to="myEdge3" probability="0.1"/>

        # ... any other edges ...

        # </interval>

        # ... some further intervals ...

        # </edgeRelations>

        fromedge_to_turnprobs = {}
        for _id in self.get_ids():
            id_fromedge = self.ids_fromedge[_id]
            if not fromedge_to_turnprobs.has_key(id_fromedge):
                fromedge_to_turnprobs[id_fromedge] = []
            fromedge_to_turnprobs[id_fromedge].append((self.ids_toedge[_id], self.probabilities[_id]))

        ids_sumoeges = self.get_edges().ids_sumo

        fd.write(xm.begin('edgeRelations', indent))
        for id_fromedge in fromedge_to_turnprobs.keys():

            for id_toedge, turnprob in fromedge_to_turnprobs[id_fromedge]:
                fd.write(xm.start('edgeRelation', indent+2))
                fd.write(xm.num('from', ids_sumoeges[id_fromedge]))
                fd.write(xm.num('to', ids_sumoeges[id_toedge]))
                fd.write(xm.num('probability', turnprob))
                fd.write(xm.stopit())

        fd.write(xm.end('edgeRelations', indent))


class TurnflowModes(am.ArrayObjman):
    def __init__(self, ident, parent, modes, edges, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='Mode OD tables',
                          info='Contains for each transport mode an OD trip table.',
                          xmltag=('modesods', 'modeods', 'ids_mode'), **kwargs)

        print 'TurnflowModes.__init__', modes
        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['state'],
                                     choices=MODES,
                                     name='ID mode',
                                     is_index=True,
                                     #xmltag = 'vClass',
                                     info='ID of transport mode.',
                                     ))
        print '  self.ids_mode.is_index', self.ids_mode.is_index()

        self.add_col(cm.ObjsConf('flowtables',
                                 groupnames=['state'],
                                 name='Flows',
                                 info='Flow generation per edge for a specific mode.',
                                 ))

        self.add_col(cm.ObjsConf('turntables',
                                 groupnames=['state'],
                                 name='Turns',
                                 info='Turn probabilities between edges for a specific mode.',
                                 ))

        self.add(cm.ObjConf(edges, is_child=False, groups=['_private']))

    # def generate_trips(self, demand, time_start, time_end,**kwargs):
    #    for id_od_mode in self.get_ids():
    #        self.odtrips[id_od_mode].generate_trips( demand, time_start, time_end, self.ids_mode[id_od_mode],**kwargs)

    # def generate_odflows(self, odflowtab, time_start, time_end,**kwargs):
    #    for id_od_mode in self.get_ids():
    #        self.odtrips[id_od_mode].generate_odflows( odflowtab, time_start, time_end, self.ids_mode[id_od_mode],**kwargs)

    def get_demand(self):
        return self.parent.parent

    def normalize_turnprobabilities(self):
        """
        Makes sure that sum of turn probabilities from an edge equals 1.
        """
        for _id in self.get_ids():
            self.turntables[_id].normalize_turnprobabilities()

    def add_mode(self, id_mode):
        id_tf_modes = self.add_row(ids_mode=id_mode)
        print '  add_mode', id_mode, id_tf_modes

        flows = Flows((self.flowtables.attrname, id_tf_modes), self, self.edges.get_value())
        self.flowtables[id_tf_modes] = flows

        turns = Turns((self.turntables.attrname, id_tf_modes), self, self.edges.get_value())
        self.turntables[id_tf_modes] = turns

        return id_tf_modes

    def add_flow(self, id_mode, id_edge, flow):
        """
        Sets a demand flows between from-Edge and toEdge pairs for mode where flows is a dictionary
        with (fromEdgeID,toEdgeID) pair as key and number of trips as values.
        """
        print 'TurnflowModes.add_turnflows', id_mode  # ,flows,kwargs
        print '  self.ids_mode.is_index()', self.ids_mode.is_index()
        if self.ids_mode.has_index(id_mode):
            id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
        else:
            id_tf_modes = self.add_mode(id_mode)
        self.flowtables[id_tf_modes].add_flow(id_edge, flow)
        return self.flowtables[id_tf_modes]

    def add_turn(self, id_mode, id_fromedge, id_toedge, turnflow):
        """
        Sets turn probability between from-edge and to-edge.
        """
        print 'TurnflowModes.add_turn', id_mode  # ,turnflow,kwargs
        # if scale!=1.0:
        #    for od in odm.iterkeys():
        #        odm[od] *= scale
        # if not self.contains_key(mode):
        #    self._initTurnflowsMode(mode)
        if self.ids_mode.has_index(id_mode):
            id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
        else:
            id_tf_modes = self.add_mode(id_mode)

        self.turntables[id_tf_modes].add_turn(id_fromedge, id_toedge, turnflow)
        return self.turntables[id_tf_modes]

    def export_flows_xml(self, fd, id_mode, id_flow=0, indent=0):
        """
        Export flow data of desired mode to xml file.
        Returns list with edge IDs with non zero flows and a flow ID counter.
        """
        print 'TurnflowModes.export_flows_xml id_mode, id_flow', id_mode, id_flow, self.ids_mode.has_index(id_mode)
        ids_sourceedge = []

        if not self.ids_mode.has_index(id_mode):
            return ids_sourceedge, id_flow

        # get vtypes for specified mode
        vtypes, shares = self.get_demand().vtypes.select_by_mode(
            id_mode, is_sumoid=True, is_share=True)

        # for vtype in vtypes:
        #    print '  test vtype',vtype
        #    print '            _index_to_id', self.get_demand().vtypes.ids_sumo._index_to_id
        #    print '       id_vtype',self.get_demand().vtypes.ids_sumo.get_id_from_index(vtype)

        # if len(vtypes) > 0:# any vehicles found for this mode?
        #    # TODO: can we put some distributen here?
        #    vtype = vtypes[0]
        for vtype, share in zip(vtypes, shares):
            print '  write flows for vtype', vtype, share
            if self.ids_mode.has_index(id_mode):
                id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
                ids_sourceedge, id_flow = self.flowtables[id_tf_modes].export_xml(
                    fd, vtype, id_flow, share=share, indent=indent)

        # print '  return ids_sourceedge, id_flow',ids_sourceedge, id_flow
        return ids_sourceedge, id_flow

    def export_turns_xml(self, fd, id_mode, indent=0):
        """
        Export flow data of desired mode to xml file.
        Returns list with edge IDs with non zero flows and a flow ID counter.
        """
        print 'TurnflowModes.export_turns_xml'

        if self.ids_mode.has_index(id_mode):
            id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
            self.turntables[id_tf_modes].export_xml(fd, indent=indent)

    def count_entered(self, counter):
        """
        Counts the number of vehicles entered in an edge.
        The counter is an array where the index equals the edge ID
        and the value represens the number of entered vehicles per edge.
        """
        for id_tf_modes in self.get_ids():
            self.turntables[id_tf_modes].count_entered(counter)


class Turnflows(am.ArrayObjman):
    def __init__(self, ident, demand, net, **kwargs):
        self._init_objman(ident, parent=demand,  # = demand
                          name='Turnflow Demand',
                          info='Contains flows and turn probailities for different modes and time intervals. Here demand data is ordered by time intervals.',
                          xmltag=('turnflows', 'interval', None), **kwargs)

        self.add_col(am.ArrayConf('times_start', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Start time',
                                  unit='s',
                                  info='Start time of interval in seconds (no fractional seconds).',
                                  xmltag='t_start',
                                  ))

        self.add_col(am.ArrayConf('times_end', 3600,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='End time',
                                  unit='s',
                                  info='End time of interval in seconds (no fractional seconds).',
                                  xmltag='t_end',
                                  ))

        self.add_col(cm.ObjsConf('turnflowmodes',
                                 groupnames=['state'],
                                 is_save=True,
                                 name='Turnflows by modes',
                                 info='Turnflow transport demand for all transport modes within the respective time interval.',
                                 ))
        #self.add( cm.ObjConf( Sinkzones('sinkzones', self, demand.get_scenario().net.edges) ))

    def get_demand(self):
        return self.parent

    def normalize_turnprobabilities(self):
        """
        Makes sure that sum of turn probabilities from an edge equals 1.
        """
        print 'Turnflows.normalize_turnprobabilities'
        # for turnflowmode in self.turnflowmodes.get_value():
        #    turnflowmode.normalize_turnprobabilities() # no! it's a dict!!
        # print '  ',self.turnflowmodes.get_value()
        for _id in self.get_ids():
            self.turnflowmodes[_id].normalize_turnprobabilities()

    def clear_turnflows(self):
        self.clear()

    def add_flow(self, t_start, t_end, id_mode, id_edge, flow):

        # print 'turnflows.add_flow', t_start, t_end, id_mode, id_edge, flow
        ids_inter = self.select_ids((self.times_start.get_value() == t_start) & (self.times_end.get_value() == t_end))
        if len(ids_inter) == 0:

            id_inter = self.add_row(times_start=t_start, times_end=t_end,)
            # print '  create new',id_inter
            tfmodes = TurnflowModes((self.turnflowmodes.attrname, id_inter),
                                    self, self.get_net().modes, self.get_net().edges)

            # NO!! odmodes = OdModes( ('ODMs for modes', id_inter), parent = self, modes = self.get_net().modes, zones = self.get_zones())
            self.turnflowmodes[id_inter] = tfmodes

            flows = tfmodes.add_flow(id_mode, id_edge, flow)

        else:

            # there should be only one demand table found for a certain interval
            id_inter = ids_inter[0]
            # print '  use',id_inter
            flows = self.turnflowmodes[id_inter].add_flow(id_mode, id_edge, flow)
        return flows

    def add_turn(self, t_start, t_end, id_mode, id_fromedge, id_toedge, turnflow):

        # print 'turnflows.add_turnflows',t_start, t_end,id_mode,id_fromedge, id_toedge, turnprob
        ids_inter = self.select_ids((self.times_start.get_value() == t_start) & (self.times_end.get_value() == t_end))
        if len(ids_inter) == 0:

            id_inter = self.add_row(times_start=t_start, times_end=t_end,)
            # print '  create new',id_inter
            tfmodes = TurnflowModes((self.turnflowmodes.attrname, id_inter),
                                    self, self.get_net().modes, self.get_net().edges)

            # NO!! odmodes = OdModes( ('ODMs for modes', id_inter), parent = self, modes = self.get_net().modes, zones = self.get_zones())
            self.turnflowmodes[id_inter] = tfmodes

            turns = tfmodes.add_turn(id_mode, id_fromedge, id_toedge, turnflow)

        else:

            # there should be only one demand table found for a certain interval
            id_inter = ids_inter[0]
            # print '  use',id_inter
            turns = self.turnflowmodes[id_inter].add_turn(id_mode, id_fromedge, id_toedge, turnflow)
        return turns

    def get_net(self):
        return self.parent.get_scenario().net

    def get_edges(self):
        return self.get_net().edges

    def get_modes(self):
        ids_mode = set()
        for id_inter in self.get_ids():
            ids_mode.update(self.turnflowmodes[id_inter].ids_mode.value)
        return list(ids_mode)  # self.get_net().modes

    def get_sinkedges(self):
        zones = self.parent.get_scenario().landuse.zones
        ids_sinkedges = set()
        ids_sinkzone = zones.select_ids(zones.ids_landusetype.get_value() == 7)
        for ids_edge in zones.ids_edges_inside[ids_sinkzone]:
            ids_sinkedges.update(ids_edge)
        #sinkedges = zones.ids_edges_orig.get_value().tolist()
        # print 'get_sinkedges',sinkedges
        # print '  sinkedges',np.array(sinkedges,np.object)
        return ids_sinkedges

    def export_flows_and_turns(self, flowfilepath, turnsfilepath, id_mode, indent=0):
        """
        Create the flow file and turn ratios file for a specific mode.
        In the SUMOpy tunflow data structure, each mode has its own 
        flow and turnratio data.
        """
        print '\n\n'+79*'_'
        print 'export_flows_and_turns id_mode=', id_mode, 'ids_vtype=', self.parent.vtypes.select_by_mode(id_mode)
        print '  write flows', flowfilepath
        fd = open(flowfilepath, 'w')
        fd.write(xm.begin('flows', indent))

        # write all possible vtypes for this mode
        self.parent.vtypes.write_xml(fd, indent=indent,
                                     ids=self.parent.vtypes.select_by_mode(id_mode),
                                     is_print_begin_end=False)

        id_flow = 0
        ids_allsourceedges = []
        time_start_min = +np.inf
        time_end_max = -np.inf
        for id_inter in self.get_ids():
            time_start = self.times_start[id_inter]
            time_end = self.times_end[id_inter]
            fd.write(xm.begin('interval'+xm.num('begin', time_start)+xm.num('end', time_end), indent+2))
            ids_sourceedge, id_flow = self.turnflowmodes[id_inter].export_flows_xml(fd, id_mode, id_flow,  indent+4)
            # print '  got ids_sourceedge, id_flow',ids_sourceedge, id_flow
            ids_allsourceedges += ids_sourceedge

            if len(ids_sourceedge) > 0:
                # print '  extend total time interval only for intervals with flow'
                if time_start < time_start_min:
                    time_start_min = time_start

                if time_end > time_end_max:
                    time_end_max = time_end

            fd.write(xm.end('interval', indent+2))

        fd.write(xm.end('flows', indent))
        fd.close()

        # print '  write turndefs', turnsfilepath
        fd = open(turnsfilepath, 'w')
        fd.write(xm.begin('turns', indent))

        for id_inter in self.get_ids():
            time_start = self.times_start[id_inter]
            time_end = self.times_end[id_inter]
            fd.write(xm.begin('interval'+xm.num('begin', time_start)+xm.num('end', time_end), indent+2))
            self.turnflowmodes[id_inter].export_turns_xml(fd, id_mode, indent+4)
            fd.write(xm.end('interval', indent+2))

        #  take sink edges from sink zones
        ids_sinkedge = self.get_sinkedges()  # it's a set
        # ...and remove source edges, otherwise vehicle will be inserted and
        # immediately removed
        # print '  ids_sinkedge',ids_sinkedge
        # print '  ids_allsourceedges',ids_allsourceedges
        ids_sinkedge = ids_sinkedge.difference(ids_allsourceedges)

        ids_sumoedge = self.get_edges().ids_sumo
        # print '  determined sink edges',list(ids_sinkedge)
        if len(ids_sinkedge) > 0:
            fd.write(xm.start('sink'))
            fd.write(xm.arr('edges', ids_sumoedge[list(ids_sinkedge)]))
            fd.write(xm.stopit())

        fd.write(xm.end('turns', indent))
        fd.close()
        if len(ids_allsourceedges) == 0:
            time_start_min = 0
            time_end_max = 0

        return time_start_min, time_end_max

    def estimate_entered(self):
        """
        Estimates the entered number of vehicles for each edge
        generated by turnflow definitions. Bases are the only the 
        turnflows, not the generated flows (which are not entering an edge).

        returns ids_edge and entered_vec
        """

        counter = np.zeros(np.max(self.get_edges().get_ids())+1, int)

        for id_inter in self.get_ids():
            self.turnflowmodes[id_inter].count_entered(counter)

        ids_edge = np.flatnonzero(counter)
        entered_vec = counter[ids_edge].copy()
        return ids_edge, entered_vec

    def turnflows_to_routes(self, is_clear_trips=True, is_export_network=True,
                            is_make_probabilities=True, cmloptions=None,):
        #  jtrrouter --flow-files=<FLOW_DEFS>
        # --turn-ratio-files=<TURN_DEFINITIONS> --net-file=<SUMO_NET> \
        # --output-file=MySUMORoutes.rou.xml --begin <UINT> --end <UINT>

        if is_make_probabilities:
            self.normalize_turnprobabilities()

        scenario = self.parent.get_scenario()
        if cmloptions is None:
            cmloptions = '-v --max-edges-factor 1  --seed 23423 --repair --ignore-vclasses false --ignore-errors --turn-defaults 5,90,5'

        trips = scenario.demand.trips
        if is_clear_trips:
            # clear all current trips = routes
            trips.clear_trips()

        rootfilepath = scenario.get_rootfilepath()
        netfilepath = scenario.net.get_filepath()
        flowfilepath = rootfilepath+'.flow.xml'
        turnfilepath = rootfilepath+'.turn.xml'

        routefilepath = trips.get_routefilepath()

        # first generate xml for net
        if is_export_network:
            scenario.net.export_netxml()

        ids_mode = self.get_modes()
        print 'turnflows_to_routes', ids_mode  # scenario.net.modes.get_ids()
        print '  cmloptions', cmloptions

        # route for all modes and read in routes
        for id_mode in ids_mode:
            # write flow and turns xml file for this mode
            time_start, time_end = self.export_flows_and_turns(flowfilepath, turnfilepath, id_mode)
            print '  time_start, time_end =', time_start, time_end

            if time_end > time_start:  # means there exist some flows for this mode
                cmd = 'jtrrouter --route-files=%s --turn-ratio-files=%s --net-file=%s --output-file=%s --begin %s --end %s %s'\
                    % (P+flowfilepath+P,
                       P+turnfilepath+P,
                       P+netfilepath+P,
                       P+routefilepath+P,
                       time_start,
                       time_end,
                       cmloptions,
                       )
                # print '\n Starting command:',cmd
                if call(cmd):
                    if os.path.isfile(routefilepath):
                        trips.import_routes_xml(routefilepath, is_generate_ids=True)
                        os.remove(routefilepath)

            else:
                print 'jtrroute: no flows generated for id_mode', id_mode

        # self.simfiles.set_modified_data('rou',True)
        # self.simfiles.set_modified_data('trip',True)
        # trips and routes are not yet saved!!


class TurnflowRouter(db.TripoptionMixin, CmlMixin, Process):
    def __init__(self, turnflows, logger=None, **kwargs):

        self._init_common('turnflowrouter', name='Turnflow Router',
                          parent=turnflows,
                          logger=logger,
                          info='Generates routes from turnflow database using the JTR router.',
                          )

        self.init_cml('')  # pass  no commad to generate options only

        attrsman = self.get_attrsman()

        self.add_option('turnratio_defaults', '30,50,20',
                        groupnames=['options'],
                        cml='--turn-defaults',
                        name='Default turn ratios',
                        info='Default turn definition. Comma separated string means: "percent right, percent straight, percent left".',
                        )

        self.add_option('max-edges-factor', 2.0,
                        groupnames=['options'],
                        cml='--max-edges-factor',
                        name='Maximum edge factor',
                        info='Routes are cut off when the route edges to net edges ratio is larger than this factor.',
                        )

        self.add_option('is_internal_links', False,
                        groupnames=['options'],
                        cml='--no-internal-links',
                        name='Disable internal links',
                        info='Disable (junction) internal links.',
                        )

        self.add_option('is_randomize_flows', True,
                        groupnames=['options'],
                        cml='--randomize-flows',
                        name='Randomize flows',
                        info='generate random departure times for flow input.',
                        )

        self.add_option('is_ignore_vclasses', False,
                        groupnames=['options'],
                        cml='--ignore-vclasses',
                        name='Ignore mode restrictions',
                        info='Ignore mode restrictions of network edges.',
                        )

        self.add_option('is_remove_loops', True,
                        groupnames=['options'],
                        cml='--remove-loops',
                        name='Remove loops',
                        info='emove loops within the route; Remove turnarounds at start and end of the route.',
                        )

        self.add_option('is_remove_loops', True,
                        groupnames=['options'],
                        cml='--remove-loops',
                        name='Remove loops',
                        info='Remove loops within the route; Remove turnarounds at start and end of the route.',
                        )

        self.add_option('is_weights_interpolate', True,
                        groupnames=['options'],
                        cml='--weights.interpolate',
                        name='Interpolate edge weights',
                        info='Interpolate edge weights at interval boundaries.',
                        )

        self.add_option('weights_minor_penalty', 1.5,
                        groupnames=['options'],
                        cml='--weights.minor-penalty',
                        name='Minor link panelty',
                        info='Apply the given time penalty when computing routing costs for minor-link internal lanes.',
                        )

        self.add_option('n_routing_threads', 0,
                        groupnames=['options'],
                        cml='--routing-threads',
                        name='Number of parallel threads',
                        info='ATTENTION: Numbers greater than 0 may cause errors!. The number of parallel execution threads used for routing.',
                        )
        self.add_option('seed', 1,
                        groupnames=['options'],
                        cml='--seed',
                        name='Random seed',
                        info='Initialises the random number generator with the given value.',
                        )

        self.add_option('is_repair', True,
                        groupnames=['options'],
                        cml='--repair',
                        name='Repair routes',
                        info='Tries to correct a false route.',
                        )

        self.add_option('is_ignore_errors', True,
                        groupnames=['options'],
                        cml='--ignore-errors',
                        name='Ignore errors',
                        info="""Continue routing, even if errors occur. 
                        This option is recommended to avoid abortion if no sink zones are.""",
                        )

        self.is_export_network = attrsman.add(am.AttrConf('is_export_network', True,
                                                          groupnames=['options'],
                                                          perm='rw',
                                                          name='Export network',
                                                          info='Export network before routing.',
                                                          ))

        self.is_clear_trips = attrsman.add(am.AttrConf('is_clear_trips', True,
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Clear trips',
                                                       info='Clear all trips in current trips database before routing.',
                                                       ))

        self.add_posoptions()
        self.add_laneoptions()
        self.add_speedoptions()
        # self.add_option('turnratiofilepath', turnratiofilepath,
        #                groupnames = ['_private'],#
        #                cml = '--turn-ratio-files',
        #                perm='r',
        #                name = 'Net file',
        #                wildcards = 'Net XML files (*.net.xml)|*.net.xml',
        #                metatype = 'filepath',
        #                info = 'SUMO Net file in XML format.',
        #                )

    def do(self):
        print 'do'
        cml = self.get_cml(is_without_command=True)  # only options, not the command #
        print '  cml=', cml
        self.parent.turnflows_to_routes(is_clear_trips=self.is_clear_trips,
                                        is_export_network=self.is_export_network,
                                        is_make_probabilities=True,
                                        cmloptions=cml)
        return True


class TurnflowImporter(Process):
    def __init__(self, turnflows, rootname=None, rootdirpath=None, tffilepath=None,
                 logger=None, **kwargs):

        self._init_common('turnflowimporter', name='Turnflow Importer',
                          parent=turnflows,
                          logger=logger,
                          info='Reads and imports turnflow data from different file formates.',
                          )

        self._edges = turnflows.get_edges()
        self._net = self._edges.get_parent()

        if rootname is None:
            rootname = self._net.parent.get_rootfilename()

        if rootdirpath is None:
            if self._net.parent is not None:
                rootdirpath = self._net.parent.get_workdirpath()
            else:
                rootdirpath = os.getcwd()

        if tffilepath is None:
            tffilepath = os.path.join(rootdirpath, rootname+'.net.xml')

        attrsman = self.get_attrsman()

        self.t_start = attrsman.add(am.AttrConf('t_start', 0,
                                                groupnames=['options'],
                                                perm='rw',
                                                name='Start time',
                                                unit='s',
                                                info='Start time of interval',
                                                ))

        self.t_end = attrsman.add(am.AttrConf('t_end', 3600,
                                              groupnames=['options'],
                                              perm='rw',
                                              name='End time',
                                              unit='s',
                                              info='End time of interval',
                                              ))

        # here we get currently available vehicle classes not vehicle type
        # specific vehicle type within a class will be generated later
        self.id_mode = attrsman.add(am.AttrConf('id_mode', MODES['passenger'],
                                                groupnames=['options'],
                                                choices=turnflows.parent.vtypes.get_modechoices(),
                                                name='ID mode',
                                                info='ID of transport mode.',
                                                ))

        self.tffilepath = attrsman.add(am.AttrConf('tffilepath', tffilepath,
                                                   groupnames=['options'],  # this will make it show up in the dialog
                                                   perm='rw',
                                                   name='Turnflow file',
                                                   wildcards="Turnflows CSV files (*.csv)|*.csv|CSV files (*.txt)|*.txt|All files (*.*)|*.*",
                                                   metatype='filepath',
                                                   info='CSV file with turnflow information for the specific mode and time interval.',
                                                   ))

    def update_params(self):
        """
        Make all parameters consistent.
        example: used by import OSM to calculate/update number of tiles
        from process dialog
        """
        pass
        #self.workdirpath = os.path.dirname(self.netfilepath)
        #bn =  os.path.basename(self.netfilepath).split('.')
        # if len(bn)>0:
        #    self.rootname = bn[0]

    def do(self):
        # self.update_params()
        if os.path.isfile(self.tffilepath):
            ids_sumoedge_notexist, pairs_sumoedge_unconnected = self.import_pat_csv()
        return (len(ids_sumoedge_notexist) == 0) & (len(pairs_sumoedge_unconnected) == 0)

    def import_pat_csv(self, sep=","):

        f = open(self.tffilepath, 'r')
        # self.attrs.print_attrs()
        turnflows = self.parent
        edges = turnflows.get_edges()
        ids_edge_sumo = edges.ids_sumo

        ids_sumoedge_notexist = []
        pairs_sumoedge_unconnected = []

        print 'import_pat_csv', self.tffilepath
        i_line = 1
        for line in f.readlines():
            cols = line.split(sep)
            # print '    cols=',cols
            if len(cols) >= 2:
                id_fromedge_sumo = cols[0].strip()
                if not ids_edge_sumo.has_index(id_fromedge_sumo):
                    ids_sumoedge_notexist.append(id_fromedge_sumo)
                else:
                    id_fromedge = ids_edge_sumo.get_id_from_index(id_fromedge_sumo)

                    if cols[1].strip() != '':
                        flow = int(string.atof(cols[1].strip()))
                        # print '   id_fromedge,flow',id_fromedge,flow
                        if flow > 0:
                            turnflows.add_flow(self.t_start, self.t_end, self.id_mode, id_fromedge, flow)

                        if len(cols) >= 4:
                            for i in range(2, len(cols), 2):
                                id_toedge_sumo = cols[i].strip()
                                if not ids_edge_sumo.has_index(id_toedge_sumo):
                                    ids_sumoedge_notexist.append(id_toedge_sumo)
                                else:
                                    id_toedge = ids_edge_sumo.get_id_from_index(id_toedge_sumo)
                                    if not (id_toedge in edges.get_outgoing(id_fromedge)):
                                        pairs_sumoedge_unconnected.append((id_fromedge_sumo, id_toedge_sumo))
                                    else:
                                        if cols[i+1].strip() != '':
                                            turnflow = int(string.atof(cols[i+1].strip()))
                                            turnflows.add_turn(self.t_start, self.t_end, self.id_mode,
                                                               id_fromedge, id_toedge, turnflow)

            else:
                print 'WARNING: inconsistent row in line %d, file %s' % (i_line, self.tffilepath)
            i_line += 1
        f.close()
        if len(ids_sumoedge_notexist) > 0:
            print 'WARNING: inexistant edge IDs:', ids_sumoedge_notexist
        if len(pairs_sumoedge_unconnected) > 0:
            print 'WARNING: unconnected edge pairs:', pairs_sumoedge_unconnected

        return ids_sumoedge_notexist, pairs_sumoedge_unconnected
