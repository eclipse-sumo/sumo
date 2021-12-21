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

# @file    prt.py
# @author  Joerg Schweizer
# @date   2012

"""
This plugin provides methods to run and analyze PRT networks.

   
"""
import os
import sys
import numpy as np
import random
from agilepy.lib_base.processes import Process
#from xml.sax import saxutils, parse, handler
from collections import OrderedDict


from coremodules.modules_common import *
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
#from agilepy.lib_base.misc import get_inversemap
#from agilepy.lib_base.geometry import find_area
#from agilepy.lib_base.processes import Process,CmlMixin,ff,call
from coremodules.network.network import SumoIdsConf
from coremodules.network.routing import edgedijkstra, get_mincostroute_edge2edge
from coremodules.simulation import sumo
from coremodules.simulation.sumo import traci
#from coremodules.network import routing
from coremodules.demand.demandbase import DemandobjMixin
from coremodules.simulation.simulationbase import SimobjMixin

from coremodules.demand.virtualpop import StageTypeMixin, StrategyMixin
from coremodules.simulation import results as res

#from demo import TraciDemo

BERTHSTATES = {'free': 0, 'waiting': 1, 'boarding': 2, 'alighting': 3}
VEHICLESTATES = {'init': 0, 'waiting': 1, 'boarding': 2, 'alighting': 3,
                 'emptytrip': 4, 'occupiedtrip': 5, 'forewarding': 6, 'await_forwarding': 7}
LEADVEHICLESTATES = [VEHICLESTATES['boarding'], VEHICLESTATES['waiting'],
                     VEHICLESTATES['emptytrip'], VEHICLESTATES['occupiedtrip']]
STOPTYPES = {'person': 0, 'freight': 1, 'depot': 2, 'group': 3, 'mixed': 4}

get_traci_odo = traci.vehicle.getDistance
get_traci_velocity = traci.vehicle.getSpeed
get_traci_route_all = traci.vehicle.getRoute
get_traci_routeindex = traci.vehicle.getRouteIndex
get_traci_distance = traci.vehicle.getDrivingDistance


def get_traci_route(id_veh_sumo):
    return get_traci_route_all(id_veh_sumo)[get_traci_routeindex(id_veh_sumo):]


def get_entered_vehs(ids_veh_sumo_current, ids_veh_sumo_before):
    ids_veh_entered_sumo = np.array(list(ids_veh_sumo_current.difference(ids_veh_sumo_before)), dtype=np.object)
    n_entered = len(ids_veh_entered_sumo)
    positions = np.zeros(n_entered, dtype=np.float32)
    for i, id_veh_sumo in zip(xrange(n_entered), ids_veh_entered_sumo):
        positions[i] = traci.vehicle.getLanePosition(id_veh_sumo)

    return list(ids_veh_entered_sumo[positions.argsort()])[::-1]


INF = 10.0**10
INFINT = 10**10
# def detect_entered_left(x,y):
#    """
#    returns the enter and left elemets of list x (before)
#    and list y (after)
#    """
#    if len(x) == 0:
#        if len(y) == 0:
#            return


class Compressors(am.ArrayObjman):
    def __init__(self, ident, prtservices, **kwargs):
        self._init_objman(ident=ident,
                          parent=prtservices,
                          name='Compressors',
                          info='Comressors try to create platoons by shunting vehicles with common destination in a common shunt edge. Contains information and methods for compressors to create platoons.',
                          version=0.1,
                          **kwargs)
        self._init_attributes()

    def _init_attributes(self):
        #self.add(cm.ObjConf(PrtBerths('berths',self))   )

        self.add(cm.AttrConf('time_update', 1.0,
                             groupnames=['parameters'],
                             name='Update time',
                             info="Simulation update time.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('time_accumulation_max', 29.0,
                             groupnames=['parameters'],
                             name='Maximum accumulation time',
                             info="Maximum accumulation time in a buffer. After this time the buffer is trying to release the vehicles.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('time_release_min', 40.0,
                             groupnames=['parameters'],
                             name='Minimum realease time',
                             info="Minimum time that must pass between successive releases.",
                             unit='s',
                             ))

        net = self.get_scenario().net

        self.add_col(am.IdsArrayConf('ids_detectoredge', net.edges,
                                     name='Detector edge ID',
                                     info='Detector edge ID is the edge from where vehicles are deviated into the compressor.',
                                     is_index=True,
                                     ))

        self.add_col(am.IdlistsArrayConf('ids_shuntedges', net.edges,
                                         name='Shunt edge IDs',
                                         info='Shunt edge IDs where PRT vehicles with common destination are sorted.',
                                         ))

        self.set_version(0.1)

    def _init_constants(self):
        self.do_not_save_attrs([
            'decel', 'ids_comp_to_ids_detectedge_sumo', 'queuess',
            'queuess_alloc', 'capacitiess', 'ids_edge_sumo_targets',
            'are_queues_avail', 'releasetimes', 'releasetimes_queues',
            'length_veh',
        ])

    def make(self, id_detectoredge=None, **kwargs):
        print 'make', kwargs
        # print '  ids_shuntedge',kwargs.get('ids_shuntedge',None),type(kwargs.get('ids_shuntedge',None)[0])
        id_comp = self.add_row(ids_shuntedges=kwargs.get('ids_shuntedge', None),
                               ids_detectoredge=id_detectoredge
                               )
        if id_detectoredge is None:
            self.update(id_comp)
        return id_comp

    def update(self, id_comp):
        print 'update id_comp', id_comp
        edges = self.get_scenario().net.edges

        self.ids_detectoredge[id_comp] = edges.get_incoming(self.ids_shuntedges[id_comp][0])[0]

    def update_all(self):
        print 'update_all'
        for id_comp in self.get_ids():
            self.update(id_comp)

    def get_scenario(self):
        return self.parent.get_scenario()

    def prepare_sim(self, process):
        print 'Compressors.prepare_sim'
        net = self.get_scenario().net
        nodes = net.nodes
        edges = net.edges
        lanes = net.lanes
        ids_edge_sumo = edges.ids_sumo

        id_prtmode = self.parent.id_prtmode
        self.decel = self.parent.prtvehicles.decel_emergency

        ids = self.get_ids()

        if len(ids) == 0:
            return []

        n_id_max = np.max(ids)+1

        self.ids_comp_to_ids_detectedge_sumo = np.zeros(n_id_max, dtype=np.object)
        self.ids_comp_to_ids_detectedge_sumo[ids] = edges.ids_sumo[self.ids_detectoredge[ids]]

        # this is list of queues for all shunt lines for each compressor
        self.queuess = np.zeros(n_id_max, dtype=np.object)

        # thses queus are the same as queuess but contains the allocated, but not yet arrived vehicles
        self.queuess_alloc = np.zeros(n_id_max, dtype=np.object)

        # this is list of maximum queue length for all shunt lines for each compressor
        self.capacitiess = np.zeros(n_id_max, dtype=np.object)

        # list with target edges for each queue for each compressor
        self.ids_edge_sumo_targets = np.zeros(n_id_max, dtype=np.object)

        # list with availabilities for each queue for each compressor
        self.are_queues_avail = np.zeros(n_id_max, dtype=np.object)

        # releasetimes for each  compressor
        self.releasetimes = np.zeros(n_id_max, dtype=np.int32)

        # releasetimes for each queue for each compressor
        self.releasetimes_queues = np.zeros(n_id_max, dtype=np.object)

        self.length_veh = self.parent.prtvehicles.get_length()

        # this queue contains all vehicles on the detect edge
        # it is used to identify whether a new vehicle has left the merge
        self.ids_vehs_detect_sumo = np.zeros(n_id_max, dtype=np.object)

        for id_comp, ids_shuntedge in zip(ids, self.ids_shuntedges[ids]):
            # print '  id_comp',id_comp
            n_shunts = len(ids_shuntedge)  # first edge is not a real shunt!!

            queues = n_shunts*[None]
            queues_alloc = n_shunts*[None]
            capacities = n_shunts*[None]

            for i, length_edge in zip(range(n_shunts), edges.lengths[ids_shuntedge]):
                capacities[i] = max(2, int((length_edge-30.0)/(self.length_veh+0.5)))
                queues[i] = []
                queues_alloc[i] = []

            self.queuess[id_comp] = queues
            self.queuess_alloc[id_comp] = queues_alloc
            self.capacitiess[id_comp] = capacities
            self.ids_edge_sumo_targets[id_comp] = n_shunts*['']
            self.are_queues_avail[id_comp] = n_shunts*[True]
            self.releasetimes_queues[id_comp] = n_shunts*[0]

            self.ids_vehs_detect_sumo[id_comp] = set()
            # print '  capacities',self.capacitiess[id_comp]
            # print '  queues',self.queuess[id_comp]

        return [(self.time_update.get_value(), self.process_step),
                ]

    def process_step(self, process):
        simtime = process.simtime
        print 79*'_'
        print 'Compressors.process_step at', simtime
        net = self.get_scenario().net
        edges = net.edges
        vehicles = self.parent.prtvehicles
        ids = self.get_ids()
        for id_comp,\
            id_detectedge_sumo,\
            ids_veh_detect_sumo,\
            ids_shuntedge,\
            ids_edge_sumo_target,\
            queues,\
            queues_alloc,\
            capacities,\
            are_queue_avail,\
            releasetime_queues\
            in zip(ids,
                   self.ids_comp_to_ids_detectedge_sumo[ids],
                   self.ids_vehs_detect_sumo[ids],
                   self.ids_shuntedges[ids],
                   self.ids_edge_sumo_targets[ids],
                   self.queuess[ids],
                   self.queuess_alloc[ids],
                   self.capacitiess[ids],
                   self.are_queues_avail[ids],
                   self.releasetimes_queues[ids],
                   ):
            n_queues = len(queues)
            # print '  '+30*'><'
            # print '  Compressor id_comp', id_comp,'n_queues',n_queues
            # check for new vehicle arrivals/departures

            ids_veh_entered_sumo, ids_veh_left_sumo,  ids_veh_new_sumo = vehicles.get_entered_left(
                id_detectedge_sumo, ids_veh_detect_sumo)

            if 0:
                print '  id_detectedge_sumo', id_detectedge_sumo
                print '  ids_veh_detect_sumo', ids_veh_detect_sumo
                print '  ids_veh_new_sumo', ids_veh_new_sumo
                print '  ids_veh_entered_sumo=', ids_veh_entered_sumo
                print '  ids_veh_left_sumo=', ids_veh_left_sumo

            if len(ids_veh_entered_sumo) > 0:

                ids_veh_entered = vehicles.get_ids_from_ids_sumo(ids_veh_entered_sumo)

                for id_veh_entered, id_veh_entered_sumo, id_leader, length_plat\
                    in zip(ids_veh_entered, ids_veh_entered_sumo,
                           vehicles.ids_leader[ids_veh_entered],
                           vehicles.lengths_plat[ids_veh_entered]):

                    # we cannot deconcatenate here, vehicles are in movement
                    # instead, measure if there is enough room for the platoon
                    # otherwise send platoon through bypass
                    if (length_plat < 0.001) & (id_leader == -1):
                        # no platoon, it is a single vehicle
                        id_targetedge_sumo = traci.vehicle.getRoute(id_veh_entered_sumo)[-1]
                        ids_shuntedge_sumo = edges.ids_sumo[ids_shuntedge]
                        # print '    dispatch id_veh_entered_sumo %s'%id_veh_entered_sumo,id_targetedge_sumo,ids_edge_sumo_target

                        is_found_queue = False
                        inds_queue = np.flatnonzero((np.array(ids_edge_sumo_target, object)
                                                     == id_targetedge_sumo) & np.array(are_queue_avail, dtype=np.bool))
                        # print '    inds_queue',inds_queue
                        if len(inds_queue) > 0:
                            #ind_queue = ids_edge_sumo_target.index(id_targetedge_sumo)
                            ind_queue = inds_queue[0]
                            queue = queues[ind_queue]
                            capa_queue = capacities[ind_queue]
                            len_queue = len(queue)
                            # print '      found ind_queue,len_queue,capa_queue',ind_queue,len_queue,capa_queue
                            # if len_queue < capa_queue:
                            self._alloc_queue(queue, queues_alloc[ind_queue],
                                              capa_queue,
                                              id_veh_entered,
                                              id_veh_entered_sumo,
                                              ids_shuntedge[ind_queue],
                                              ids_shuntedge_sumo[ind_queue],
                                              vehicles, edges)

                            # queue is full now, remove target
                            # no, otherwise th target is lost to program the vehicles
                            if len_queue+1 == capa_queue:
                                are_queue_avail[ind_queue] = False

                            is_found_queue = True

                        if not is_found_queue:
                            # print '      no queue with target or specific queue is full, search for a new queue'

                            is_found_queue = False
                            for ind_queue, queue in zip(range(1, n_queues), queues[1:]):
                                if len(queue) == 0:
                                    is_found_queue = True
                                    break
                            # ind_queue = 1 # start with 1 because 0 is bypass
                            #is_queueend = False
                            # while len(queues[ind_queue])>0 & (not is_queueend):
                            #    is_queueend = ind_queue == n_queues-1
                            #    if not
                            #    ind_queue += 1

                            if not is_found_queue:
                                # print '      all queues are busy, let vehicle %s go ahead'%id_veh_entered_sumo
                                pass
                            else:
                                # print '      configure target of new queue',ind_queue,id_targetedge_sumo
                                ids_edge_sumo_target[ind_queue] = id_targetedge_sumo
                                self._alloc_queue(queues[ind_queue],
                                                  queues_alloc[ind_queue],
                                                  capacities[ind_queue],
                                                  id_veh_entered,
                                                  id_veh_entered_sumo,
                                                  ids_shuntedge[ind_queue],
                                                  ids_shuntedge_sumo[ind_queue],
                                                  vehicles, edges)

                    else:
                        # let platoon pass
                        pass
            else:
                # nothing happened
                pass

            # update veh on detector edge
            self.ids_vehs_detect_sumo[id_comp] = ids_veh_new_sumo

            if True:  # do this later ...simtime - self.releasetimes[id_comp]> 10:
                # now check if any of the vehicle in any of the queues has arrived
                # and if a queue is complete, create a platoon, send it off and
                # reset the queue
                # print '  Deal with queues...'
                for ind_queue, queue, queue_alloc,\
                    capacity, id_edge_sumo_target,\
                    id_shuntedge_sumo, releasetime_queue\
                        in zip(
                        range(n_queues), queues, queues_alloc,
                        capacities, ids_edge_sumo_target,
                        edges.ids_sumo[ids_shuntedge], releasetime_queues):
                    if 0:
                        print '    OOOO shunt %s --> target %s' % (id_shuntedge_sumo, id_edge_sumo_target)
                        print '    simtime-releasetime_queue', simtime-releasetime_queue
                        print '    queue', queue, id(queue)
                        print '    queue_alloc', queue_alloc

                    # check if allocated arrived
                    ids_veh_arrived = []
                    for id_veh, id_veh_sumo in zip(queue_alloc, vehicles.ids_sumo[queue_alloc]):
                        if vehicles.reached_stop_sumo(id_veh_sumo):
                            ids_veh_arrived.append(id_veh)

                    for id_veh in ids_veh_arrived:
                        queue_alloc.remove(id_veh)

                    # check if queue reached capacity limit
                    len_queue = len(queue)
                    if (len(queue_alloc) == 0) & (len_queue > 0):
                        # no allocations for this queue
                        len_queue = len(queue)
                        # print '  rtq,rtc',releasetime_queue,self.releasetimes[id_comp], len_queue == capacity,simtime - releasetime_queue,simtime - self.releasetimes[id_comp]
                        # here we coukd also test timeout conditions
                        if ((len_queue == capacity) | ((simtime - releasetime_queue) > self.time_accumulation_max.get_value())) & (simtime - self.releasetimes[id_comp] > self.time_release_min.get_value()):
                            # queue is full
                            # print '  platoon and send vehicles:',queue
                            # action 1
                            # platoon all vehicles in queue, starting from last in queue
                            for i in range(len_queue-1, 0, -1):
                                vehicles.concatenate(queue[i], queue[i-1])

                            # print '  check platooned vehicles:'
                            vehicles.get_platoon(queue[0])
                            # reprogram destination and start
                            for id_veh, id_veh_sumo in zip(queue, vehicles.ids_sumo[queue]):
                                vehicles.reschedule_trip_sumo(id_veh_sumo, id_edge_sumo_target)
                                #vehicles.control_slow_down( id_veh, speed = 6.0/3.6)
                                traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)

                            # reset queue
                            del queue[:]
                            ids_edge_sumo_target[ind_queue] = ""
                            are_queue_avail[ind_queue] = True
                            releasetime_queues[ind_queue] = int(simtime)
                            self.releasetimes[id_comp] = int(simtime)

    def _alloc_queue(self, queue, queue_alloc, capa_queue,
                     id_veh, id_veh_sumo,
                     id_shuntedge, id_shuntedge_sumo, vehicles, edges):

        queue.append(id_veh)
        queue_alloc.append(id_veh)
        len_queue = len(queue)
        #pos_stop = 1.0+(capa_queue-len_queue)*(self.length_veh+0.5)
        pos_stop = edges.lengths[id_shuntedge]-10-len_queue*(self.length_veh+0.5)
        # print '_alloc_queue',id_veh_sumo,id_shuntedge_sumo,capa_queue-len_queue,pos_stop
        vehicles.reschedule_trip_sumo(id_veh_sumo, id_shuntedge_sumo)

        vehicles.set_stop(id_veh, id_shuntedge_sumo, pos_stop, laneindex=0)
        # print '  get_traci_route',get_traci_route(id_veh_sumo)
        # print '  get_traci_route_all',get_traci_route_all(id_veh_sumo)
        # print '  get_traci_routeindex',get_traci_routeindex(id_veh_sumo)
        #get_traci_route_all = traci.vehicle.getRoute
        #get_traci_routeindex = traci.vehicle.getRouteIndex
        # def get_traci_route


class Decompressors(Compressors):
    def __init__(self, ident, prtservices, **kwargs):
        self._init_objman(ident=ident,
                          parent=prtservices,
                          name='Decompressors',
                          info='Decomressors dissolve platoons by stopping and decatenating vehicles in parallel shunt edges. Contains information and methods for decompressors to create platoons.',
                          version=0.1,
                          **kwargs)
        self._init_attributes()

    def _init_constants(self):
        self.do_not_save_attrs([
            'decel', 'ids_comp_to_ids_detectedge_sumo', 'queuess',
            'queuess_alloc', 'capacitiess',  # 'ids_edge_sumo_targets',
            'are_queues_avail', 'releasetimes', 'releasetimes_queues',
            'length_veh', 'ids_vehs_detect_sumo',
        ])

    def prepare_sim(self, process):
        print 'Decompressors.prepare_sim'
        net = self.get_scenario().net
        nodes = net.nodes
        edges = net.edges
        lanes = net.lanes
        ids_edge_sumo = edges.ids_sumo

        id_prtmode = self.parent.id_prtmode
        self.decel = self.parent.prtvehicles.decel_emergency

        ids = self.get_ids()
        if len(ids) == 0:
            return []
        n_id_max = np.max(ids)+1

        self.ids_comp_to_ids_detectedge_sumo = np.zeros(n_id_max, dtype=np.object)
        self.ids_comp_to_ids_detectedge_sumo[ids] = edges.ids_sumo[self.ids_detectoredge[ids]]

        # this is list of queues for all shunt lines for each compressor
        self.queuess = np.zeros(n_id_max, dtype=np.object)

        # this is list of target edges for all shunt lines for each compressor
        self.ids_targetedgess_sumo = np.zeros(n_id_max, dtype=np.object)

        # thses queus are the same as queuess but contains the allocated, but not yet arrived vehicles
        self.queuess_alloc = np.zeros(n_id_max, dtype=np.object)

        # this is list of maximum queue length for all shunt lines for each compressor
        self.capacitiess = np.zeros(n_id_max, dtype=np.object)

        # list with target edges for each queue for each compressor
        #self.ids_edge_sumo_targets = np.zeros(n_id_max,dtype = np.object)

        # list with availabilities for each queue for each compressor
        self.are_queues_avail = np.zeros(n_id_max, dtype=np.object)

        # releasetimes for each  compressor
        self.releasetimes = np.zeros(n_id_max, dtype=np.int32)

        # releasetimes for each queue for each compressor
        self.releasetimes_queues = np.zeros(n_id_max, dtype=np.object)

        self.length_veh = self.parent.prtvehicles.get_length()

        # this queue contains all vehicles on the detect edge
        # it is used to identify whether a new vehicle has left the merge
        self.ids_vehs_detect_sumo = np.zeros(n_id_max, dtype=np.object)

        for id_comp, ids_shuntedge in zip(ids, self.ids_shuntedges[ids]):
            # print '  id_comp',id_comp
            n_shunts = len(ids_shuntedge)  # first edge is not a real shunt!!

            queues = n_shunts*[None]
            queues_alloc = n_shunts*[None]
            capacities = n_shunts*[None]
            ids_targetedges_sumo = n_shunts*[None]
            for i, length_edge in zip(range(n_shunts), edges.lengths[ids_shuntedge]):
                capacities[i] = int((length_edge-25.0)/(self.length_veh+0.5))
                queues[i] = []
                queues_alloc[i] = []
                ids_targetedges_sumo[i] = {}

            self.queuess[id_comp] = queues
            self.queuess_alloc[id_comp] = queues_alloc
            self.capacitiess[id_comp] = capacities
            self.ids_targetedgess_sumo[id_comp] = ids_targetedges_sumo
            #self.ids_edge_sumo_targets [id_comp] = n_shunts*['']
            self.are_queues_avail[id_comp] = n_shunts*[True]
            self.releasetimes_queues[id_comp] = np.arange(n_shunts)*int(self.time_accumulation_max.get_value()/n_shunts)

            self.ids_vehs_detect_sumo[id_comp] = set()
            # print '  capacities',self.capacitiess[id_comp]
            # print '  queues',self.queuess[id_comp]

        return [(self.time_update.get_value(), self.process_step),
                ]

    def process_step(self, process):
        simtime = process.simtime
        print 79*'_'
        print 'Deompressors.process_step at', simtime
        net = self.get_scenario().net
        edges = net.edges
        vehicles = self.parent.prtvehicles
        ids = self.get_ids()
        for id_comp,\
            id_detectedge_sumo,\
            ids_veh_detect_sumo,\
            ids_shuntedge,\
            queues,\
            queues_alloc,\
            capacities,\
            are_queue_avail,\
            releasetime_queues,\
            ids_targetedges_sumo\
            in zip(ids,
                   self.ids_comp_to_ids_detectedge_sumo[ids],
                   self.ids_vehs_detect_sumo[ids],
                   self.ids_shuntedges[ids],
                   self.queuess[ids],
                   self.queuess_alloc[ids],
                   self.capacitiess[ids],
                   self.are_queues_avail[ids],
                   self.releasetimes_queues[ids],
                   self.ids_targetedgess_sumo[ids]
                   ):
            n_queues = len(queues)
            ids_shuntedge_sumo = edges.ids_sumo[ids_shuntedge]
            # print '  '+60*'|'
            # print '  Decompressor id_comp', id_comp,'n_queues',n_queues
            # check for new vehicle arrivals/departures

            ids_veh_sumo = set(traci.edge.getLastStepVehicleIDs(id_detectedge_sumo))
            if 0:
                print '  id_detectedge_sumo', id_detectedge_sumo
                print '  ids_veh_detect_sumo', ids_veh_detect_sumo, ids_veh_detect_sumo != ids_veh_sumo
                print '  ids_veh_sumo=', ids_veh_sumo
                ids_veh_sumo_raw = traci.edge.getLastStepVehicleIDs(id_detectedge_sumo)
                print '  ids_veh_sumo_raw=', ids_veh_sumo_raw

            if ids_veh_detect_sumo != ids_veh_sumo:
                # there are new vehicles

                #ids_veh_entered_sumo, poss = vehicles.get_ids_sumo_sorted(ids_veh_sumo.difference(ids_veh_detect_sumo))
                # ids_veh_entered_sumo = list(ids_veh_sumo.difference(ids_veh_detect_sumo))
                ids_veh_entered_sumo = get_entered_vehs(ids_veh_sumo, ids_veh_detect_sumo)
                ids_veh_entered = vehicles.get_ids_from_ids_sumo(ids_veh_entered_sumo)

                if 0:
                    print '  ids_veh_entered', ids_veh_entered, type(ids_veh_entered)
                    # print '  poss',poss
                    print '  ids_veh_entered_sumo', ids_veh_entered_sumo
                    print '  ids_leader', vehicles.ids_leader[ids_veh_entered]
                    print '  ids_follower', vehicles.ids_follower[ids_veh_entered]
                    print '  lengths_plat', vehicles.lengths_plat[ids_veh_entered]

                for id_veh_entered, id_veh_entered_sumo, id_leader, length_plat\
                    in zip(ids_veh_entered, ids_veh_entered_sumo,
                           vehicles.ids_leader[ids_veh_entered],
                           vehicles.lengths_plat[ids_veh_entered]):

                    # we cannot deconcatenate here, vehicles are in movement
                    # instead, measure if there is enough room for the platoon
                    # otherwise send platoon through bypass
                    if (length_plat > 0.001) & (id_leader == -1):
                        # vehicle is a platoon leader:
                        # find a queue that fits the platoon
                        id_targetedge_sumo = traci.vehicle.getRoute(id_veh_entered_sumo)[-1]
                        # print '    dispatch id_veh_entered_sumo %s with target %s'%(id_veh_entered_sumo,id_targetedge_sumo)

                        is_found_queue = False
                        costs = np.zeros(n_queues, dtype=np.float32)
                        for ind_queue, queue, is_avail, capa in zip(range(n_queues), queues, are_queue_avail, capacities):

                            dl = (capa - len(queue)) * self.length_veh - 2*length_plat
                            # print '      ceck queue %d with capa %d, len = %d, dl=%d'%(ind_queue,capa,len(queue),dl)
                            if (dl > 0) & is_avail:
                                costs[ind_queue] = dl
                            else:
                                costs[ind_queue] = np.inf

                        ind_queue = np.argmin(costs)
                        # print '    ind_queue,costs',ind_queue,costs
                        # if costs[ind_queue] <10000:# OK queue has been found
                        #ind_queue = ids_edge_sumo_target.index(id_targetedge_sumo)
                        #ind_queue = inds_queue[0]
                        queue = queues[ind_queue]
                        capa_queue = capacities[ind_queue]
                        len_queue = len(queue)
                        # print '      found ind_queue,len_queue,capa_queue',ind_queue,len_queue,capa_queue
                        # if len_queue < capa_queue:
                        self._alloc_queue(queue, queues_alloc[ind_queue],
                                          ids_targetedges_sumo[ind_queue],
                                          capa_queue,
                                          id_veh_entered,
                                          id_veh_entered_sumo,
                                          id_targetedge_sumo,
                                          ids_shuntedge[ind_queue],
                                          ids_shuntedge_sumo[ind_queue],
                                          vehicles, edges)

                    elif (id_leader > -1):
                        # this is a follower:
                        # shunt it to the same edge as its leader
                        id_targetedge_sumo = traci.vehicle.getRoute(id_veh_entered_sumo)[-1]
                        id_platoonleader = vehicles.get_platoonleader(id_leader)
                        for ind_queue, queue, capa_queue, in zip(range(n_queues), queues, capacities):
                            if id_platoonleader in queue:
                                # print '      found id_platoonleader prt.%d of prt.%d at queue %d'%(id_platoonleader,id_veh_entered,ind_queue)
                                self._alloc_queue_follower(
                                    queue, queues_alloc[ind_queue],
                                    ids_targetedges_sumo[ind_queue],
                                    capa_queue,
                                    id_veh_entered,
                                    id_veh_entered_sumo,
                                    id_targetedge_sumo,
                                    ids_shuntedge_sumo[ind_queue],
                                    vehicles)
                                break
                    else:
                        # individual vehicle
                        pass

            # update veh on detector edge
            self.ids_vehs_detect_sumo[id_comp] = ids_veh_sumo

            # (simtime - self.releasetimes[id_comp]> self.time_release_min.get_value()): # do this later ...simtime - self.releasetimes[id_comp]> 10:
            if 1:
                # now check if any of the vehicle in any of the queues has arrived
                # and if a queue is complete, create a platoon, send it off and
                # reset the queue
                # print '  Deal with queues...'
                is_released = False
                for ind_queue, queue, queue_alloc,\
                    ids_targetedge_sumo, capacity,\
                    id_shuntedge_sumo, releasetime_queue\
                        in zip(
                        range(n_queues), queues, queues_alloc,
                        ids_targetedges_sumo, capacities,
                        edges.ids_sumo[ids_shuntedge], releasetime_queues):
                    if 0:
                        print '    QQQQQQQQQQQ shunt %s ' % (id_shuntedge_sumo)
                        print '      simtime-releasetime_queue', simtime-releasetime_queue, (simtime - releasetime_queue > self.time_accumulation_max.get_value()), (simtime - self.releasetimes[id_comp] > 10)
                        print '      queue', queue, id(queue)
                        print '      ids_targetedge_sumo', ids_targetedge_sumo
                        print '      queue_alloc', queue_alloc
                        print '      releasetime_queue', releasetime_queue, simtime - releasetime_queue, simtime - releasetime_queue > self.time_accumulation_max.get_value()
                    # here we could also test timeout conditions
                    if (simtime - self.releasetimes[id_comp] > self.time_release_min.get_value()) & (simtime - releasetime_queue > self.time_accumulation_max.get_value()):
                        # if (simtime - self.releasetimes[id_comp]> self.time_release_min.get_value()):

                        # check if allocated arrived
                        id_veh_arrived = -1
                        # for id_vehinfo,id_veh_sumo, length_plat  in zip(range(),queue_alloc, vehicles.ids_sumo[queue_alloc],vehicles.lengths_plat[queue_alloc]):

                        for id_veh in queue_alloc:
                            if vehicles.reached_stop_sumo(vehicles.ids_sumo[id_veh]):
                                id_veh_arrived = id_veh
                                break

                        # print '      allocated prt.%d reached stop (-1 means no vehicle)'%(id_veh_arrived)

                        if id_veh_arrived > -1:
                            # a platoon leader  id_veh_arrived has arrived

                            # print '      check if entire platoon arrived at the queue'
                            length_plat = vehicles.lengths_plat[id_veh_arrived]
                            ind_pl = queue.index(id_veh_arrived)
                            len_queue = len(queue)
                            ids_platoon = []
                            #ids_targetedge_plat_sumo= []

                            if len_queue > ind_pl+1:

                                for i, id_veh in zip(range(ind_pl+1, len_queue), queue[ind_pl+1:]):
                                    # print '        check prt.%d with leader prt.%d'%(id_veh,vehicles.ids_leader[id_veh])
                                    if vehicles.ids_leader[id_veh] != -1:
                                        # print '          real lead prt.%d qlead prt.%d | plat lead prt.%d plat qlead prt.%d '%(vehicles.ids_leader[id_veh],queue[i-1],vehicles.get_platoonleader(id_veh),id_veh_arrived)
                                        # if vehicles.ids_leader[id_veh] == queue[i-1]:
                                        #    print '          add'
                                        #    ids_platoon.append(id_veh)
                                        #
                                        if vehicles.get_platoonleader(id_veh) == id_veh_arrived:
                                            # TODO: this is very inefficient, by happens
                                            # when platoon has ben broken up during a merge operation
                                            # idea: use a pointer to follower
                                            ids_platoon.append(id_veh)
                                            # ids_targetedge_plat_sumo.append(id_targetedge_sumo)
                                            # print '          add'
                                    else:
                                        # is not a follower
                                        pass
                            length_plat_arrived = len(ids_platoon)*self.length_veh + 0.5
                            # print '    arrived id_veh_arrived prt.%d at queue pos ind_pl=%d (should be 0)'%(id_veh_arrived,ind_pl)
                            # print '    with platoon length %.2fm > %.2fm?'%(length_plat_arrived,length_plat),(length_plat_arrived >= length_plat)& (not is_released),'is_released',is_released

                            if (length_plat_arrived >= length_plat) & (not is_released):
                                # print '    entire platoon length in queue, now release...',ids_platoon
                                is_released = True
                                # reschedule leader

                                vehicles.reschedule_trip(id_veh_arrived, ids_targetedge_sumo[id_veh_arrived])
                                id_veh_sumo = vehicles.get_id_sumo(id_veh_arrived)
                                #traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)
                                #vehicles.control_slow_down( id_veh_arrived, speed = 6.0/3.6)
                                # print '      queue_alloc',queue_alloc
                                # print '      queue',queue
                                # deconcatenate and reschedule followers
                                for id_veh, id_veh_sumo in zip(ids_platoon[::-1], vehicles.ids_sumo[ids_platoon[::-1]]):
                                    vehicles.decatenate(id_veh)
                                    vehicles.reschedule_trip_sumo(id_veh_sumo, ids_targetedge_sumo[id_veh])
                                    #vehicles.control_slow_down( id_veh, speed = 6.0/3.6)
                                    #traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)
                                    queue.remove(id_veh)
                                    del ids_targetedge_sumo[id_veh]

                                releasetime_queues[ind_queue] = int(simtime)
                                self.releasetimes[id_comp] = int(simtime)

                                # remove platoon leader
                                queue_alloc.remove(id_veh_arrived)
                                queue.remove(id_veh_arrived)
                                del ids_targetedge_sumo[id_veh_arrived]

                            else:
                                # print '    platoon incomplete'
                                pass
                        else:
                            # print '      no leaders arrived in this quque'
                            pass

    def _alloc_queue(self, queue, queue_alloc, ids_targetedge_sumo, capa_queue,
                     id_veh, id_veh_sumo, id_targetedge_sumo,
                     id_shuntedge, id_shuntedge_sumo, vehicles, edges):
        """Queue allocation for platoon leader only."""
        queue.append(id_veh)

        # store here also the target necessary to reprogram
        queue_alloc.append(id_veh)
        # TODO: idea, the original target should be stored in the vehicles db
        ids_targetedge_sumo[id_veh] = id_targetedge_sumo
        len_queue = len(queue)

        # make platoon leder always stop at the foremost position
        pos_stop = 1.0 + capa_queue*(self.length_veh)
        # print '_alloc_queue',id_veh_sumo,id_shuntedge_sumo,capa_queue-len_queue,pos_stop
        vehicles.reschedule_trip_sumo(id_veh_sumo, id_shuntedge_sumo)
        vehicles.set_stop(id_veh, id_shuntedge_sumo, pos_stop, laneindex=0)

        # Forced exit of vehicle from merge conrol
        self.parent.mergenodes.exit_veh_forced(id_veh, id_veh_sumo, vehicles)

    def _alloc_queue_follower(self, queue, queue_alloc, ids_targetedge_sumo,
                              capa_queue,
                              id_veh, id_veh_sumo, id_targetedge_sumo,
                              id_shuntedge_sumo, vehicles):
        """Queue allocation for follower only."""
        queue.append(id_veh)
        # TODO: idea, the original target should be stored in the vehicles db
        ids_targetedge_sumo[id_veh] = id_targetedge_sumo
        # queue_alloc.append(id_veh)
        #len_queue = len(queue)

        # make platoon leder always stop at the foremost position
        #pos_stop = 1.0 + capa_queue*(self.length_veh + 0.5)
        # print '_alloc_queue_follower',id_veh_sumo,id_shuntedge_sumo,id_targetedge_sumo
        vehicles.reschedule_trip_sumo(id_veh_sumo, id_shuntedge_sumo)
        #vehicles.set_stop(id_veh, id_shuntedge_sumo, pos_stop, laneindex = 0)


# class Shortmergenodes(Mergenodes):

class Mergenodes(am.ArrayObjman):
    def __init__(self, ident, prtservices, **kwargs):
        self._init_objman(ident=ident,
                          parent=prtservices,
                          name='Merge nodes',
                          info='Contains information and methods for merging the flow of two vehicle streams, including platoons.',
                          version=0.2,
                          **kwargs)
        self._init_attributes()

    def _init_attributes(self):
        #self.add(cm.ObjConf(PrtBerths('berths',self))   )

        self.add(cm.AttrConf('time_update', 0.5,
                             groupnames=['parameters'],
                             name='Update time',
                             info="Update time for merges.",
                             unit='s',
                             ))

        net = self.get_scenario().net

        self.add_col(am.IdsArrayConf('ids_node', net.nodes,
                                     name='Node ID',
                                     info='Network node ID.',
                                     is_index=True,
                                     ))

        # self.add_col(am.IdsArrayConf('ids_edge_in1', net.edges,
        #                            name = 'ID edge 1',
        #                            info = 'ID of edge at entrance 1.',
        #                            ))
        if self.get_version() < 0.2:
            self.delete('ids_mergenode_in1')
            self.delete('ids_mergenode_in2')
            self.delete('ids_mergenodes_out')
            self.delete('distances_mergenode_in1')
            self.delete('distances_mergenode_in2')

        self.add_col(am.IdsArrayConf('ids_node_in1', net.nodes,
                                     name='ID node in 1',
                                     info='ID of upstream network node at incoming line 1.',
                                     ))

        self.add_col(am.ArrayConf('distances_node_in1', 0.0,
                                  groupnames=['parameters'],
                                  name='Distance node in 1',
                                  info="Distance to network node at incoming line 1.",
                                  unit='m',
                                  ))

        # self.add_col(am.IdsArrayConf('ids_edge2', net.edges,
        #                            name = 'ID edge 2',
        #                            info = 'ID of edge at entrance 2.',
        #                            ))

        self.add_col(am.IdsArrayConf('ids_node_in2', net.nodes,
                                     name='ID node in 2',
                                     info='ID of upstream network node at incoming line 2.',
                                     ))

        self.add_col(am.ArrayConf('distances_node_in2', 0.0,
                                  groupnames=['parameters'],
                                  name='Distance mergenode in 2',
                                  info="Distance to upstream network node at incoming line 2.",
                                  unit='m',
                                  ))

        self.add_col(am.IdlistsArrayConf('ids_nodes_out', net.nodes,
                                         name='IDs nodes out',
                                         info='IDs of downstream network nodes.',
                                         ))

        self.add_col(am.ArrayConf('are_station',  default=False,
                                  groupnames=['parameters'],
                                  name='is station',
                                  info="Node is the exit node of a station, comressor or decompressor. The incoming edge of this are internal and are not considered.",
                                  ))

        self.add_col(am.ArrayConf('are_entry',  default=False,
                                  groupnames=['parameters'],
                                  name='is entry',
                                  info="Node is a merge wich receives an entry line from a station, comressor or decompressor. This shorter line is always input line 1 and expects that vehicles have a speed close to zero.",
                                  ))

        self.set_version(0.2)

    def _init_constants(self):
        self.do_not_save_attrs([
            'ids_merge_to_ids_node_sumo', 'ids_merge_to_ids_edge_out_sumo',
            'ids_node_to_ids_merge', 'ids_vehs_out_sumo,'
            'ids_vehs_in1', 'ids_vehs_in1_sumo',
            'ids_vehs_in2', 'ids_vehs_in2_sumo',
            'ids_vehs_merged', 'ids_vehs_merged_sumo',
            'lineinds_vehs_merged', 'vehicle_to_merge',
            'vehicles_entries', 'vehicles_mains',
        ])

    def make_from_net(self):
        """
        Make merge node database from network.
        """
        print 'Mergenodes.make_from_net'
        self.clear()
        id_prtmode = self.parent.id_prtmode

        net = self.get_scenario().net
        nodes = net.nodes
        edges = net.edges
        lanes = net.lanes
        id_zippertype = nodes.types.choices['zipper']

        compressors = self.parent.compressors
        decompressors = self.parent.decompressors
        ids_detectoredges = set(compressors.ids_detectoredge.get_value())
        ids_detectoredges.update(decompressors.ids_detectoredge.get_value())

        ids_shuntedges = set()
        ids_mainlinedges = set()

        ids = compressors.get_ids()
        for id_unit, ids_edge in zip(ids, compressors.ids_shuntedges[ids]):
            # put in all shunts except for the bypass
            print '    bypass of compressor', id_unit, '=', ids_edge[0]
            ids_mainlinedges.add(ids_edge[0])
            print '    update ids_shuntedges with compressors', ids_edge[1:]
            ids_shuntedges.update(ids_edge[1:])

        # collect decompressornodes, wich will not become entry merges
        ids = decompressors.get_ids()
        ids_node_decompressorout = []
        for id_unit, ids_edge in zip(ids, decompressors.ids_shuntedges[ids]):
            # put in all shunts except for the bypass
            print '    bypass of decompressor', id_unit, '=', ids_edge[0], 'id_tonode', edges.ids_tonode[ids_edge[0]]
            ids_mainlinedges.add(ids_edge[0])
            ids_node_decompressorout.append(edges.ids_tonode[ids_edge[0]])
            print '    update ids_shuntedges with decompressors', ids_edge[1:]
            ids_shuntedges.update(ids_edge[1:])
        print '  ids_node_decompressorout', ids_node_decompressorout
        ids_node_compressorout = edges.ids_tonode[list(ids_mainlinedges)]
        ids_mainlinefromnodes = edges.ids_fromnode[list(ids_mainlinedges)]

        # now put also all incoming edges of main lines as shunts
        # in order to cut of compressor entrance from downstrem search
        # for id_edge in ids_mainlinedges:
        #    print '  update ids_shuntedges with mainline incoming',edges.get_incoming(id_edge)
        #    ids_shuntedges.update(edges.get_incoming(id_edge))

        print '  ids_shuntedges',  ids_shuntedges
        #ids_ptstop = ptstops.get_ids()
        #id_mode_prt = self.parent.id_prtmode

        #ids_edges = net.lanes.ids_edge[ptstops.ids_lane[ids_ptstop]]
        #ids_lanes = net.edges.ids_lanes[ids_edges]
        #ids_lane = ptstops.ids_lane[ids_ptstop]
        #edgelengths = net.edges.lengths

        ids_node = nodes.get_ids()
        print '  ids_node', ids_node
        for id_node, ids_edge_from, ids_edge_to, id_type in zip(
            ids_node,
            nodes.ids_incoming[ids_node],
            nodes.ids_outgoing[ids_node],
            nodes.types[ids_node],
        ):

            if True:
                #

                print 60*'-'
                print '  check node', id_node, nodes.ids_sumo[id_node], id_type, id_zippertype == id_type

                # id merge to be created, for debugging
                id_merge = -1

                if (ids_edge_from is None):
                    print '    WARNING: isolate node: ids_edge_from,ids_edge_to', ids_edge_from, ids_edge_to
                    pass

                elif (len(ids_edge_from) == 2) & (len(ids_edge_to) == 1):

                    # ids_mainlinedges.add(ids_edge[0])
                    # ids_shuntedges.update(ids_edge[1:])

                    # check accesslevels
                    id_edge1, id_edge2 = ids_edge_from
                    ids_lane1, ids_lane2 = edges.ids_lanes[ids_edge_from]
                    print '    id_edge1', id_edge1, 'ids_lane1', ids_lane1, self.is_prt_only(ids_lane1, lanes)
                    print '    id_edge2', id_edge2, 'ids_lane2', ids_lane2, self.is_prt_only(ids_lane2, lanes)
                    if self.is_prt_only(ids_lane1, lanes) & self.is_prt_only(ids_lane2, lanes):
                        print '      +PRT merge with 2 PRT lines entering'

                        if id_type != id_zippertype:
                            print 'WARNING: PRT network node %d %s is NOT in zipper mode!' % (id_node, nodes.ids_sumo[id_node])

                        if not ids_shuntedges.isdisjoint(ids_edge_from):
                            print '    one incoming edge is  a shunt edge. Detect last shunt node.'

                            # TODO: this should be a function of the compressor class
                            id_tonode_out = edges.ids_tonode[ids_edge_to[0]]
                            if id_tonode_out in ids_node_compressorout:
                                print '    output node of compressor => station merge node with output node', id_tonode_out
                                # model this node as a platform end node of a station
                                id_merge = self.add_row(ids_node=id_node,
                                                        ids_nodes_out=[id_tonode_out],
                                                        are_station=True,
                                                        )
                            else:
                                print '    compressor internal node'
                                pass

                        # elif not ids_mainlinedges.isdisjoint(ids_edge_from):
                        #    print '    one incoming line is the bypass'
                        #    ids_node_out = self.search_downstream_merges(ids_edge_to[0], edges, lanes, id_prtmode)
                        #
                        #    # model this node as a station output
                        #    self.add_row(       ids_node = id_node,
                        #                        ids_nodes_out = ids_node_out,
                        #                        are_station = True,
                        #                        )

                        else:
                            print '    regular merge'
                            id_node_up1, dist1 = self.search_upstream_merge(id_edge1, edges, lanes, id_prtmode)
                            id_node_up2, dist2 = self.search_upstream_merge(id_edge2, edges, lanes, id_prtmode)

                            ids_node_out = self.search_downstream_merges(
                                ids_edge_to[0], edges, lanes, id_prtmode, ids_sinkedge=ids_shuntedges)
                            id_merge = self.add_row(ids_node=id_node,
                                                    ids_node_in1=id_node_up1,
                                                    distances_node_in1=dist1,
                                                    ids_node_in2=id_node_up2,
                                                    distances_node_in2=dist2,
                                                    ids_nodes_out=ids_node_out,
                                                    are_station=False,
                                                    )

                elif (len(ids_edge_from) == 1) & (len(ids_edge_to) == 2):

                    id_edge_from = ids_edge_from[0]
                    ids_lane_in = edges.ids_lanes[id_edge_from]
                    #al_in = lanes.get_accesslevel(edges.ids_lanes[id_edge_from], id_prtmode)
                    #is_prt_in = lanes.ids_modes_allow[edges.ids_lanes[id_edge_from]] == id_prtmode
                    is_prt_in = self.is_prt_only(ids_lane_in, lanes)
                    print '    one in, 2 out => diverge node, access is_prt_in', is_prt_in, 'is_platform_in', self.is_platform(ids_lane_in, lanes)

                    # if id_edge_from in ids_detectoredges:
                    #    print '    fromnode is a detectoredge of a compressor'
                    #    id_node_up, dist = self.search_upstream_merge(id_edge_from, edges, lanes, id_prtmode)
                    #    self.add_row(   ids_node = id_node,
                    #                        ids_node_in1 = id_node_up,
                    #                        distances_node_in1 = dist,
                    #                        #ids_nodes_out = ids_node_out,
                    #                        are_station = True,
                    #                        )

                    # check if node is outgoing node at a station
                    if 0:  # no longer considered
                        if is_prt_in & (id_node in ids_mainlinefromnodes):
                            print '    Diverge node in front of a compressor/decompressorr.'
                            id_node_up, dist = self.search_upstream_merge(id_edge_from, edges, lanes, id_prtmode)

                            id_edge1, id_edge2 = ids_edge_to
                            if id_edge1 in ids_mainlinedges:
                                id_edge = id_edge1
                            else:
                                id_edge = id_edge2

                            #ids_node_out = self.search_downstream_merges(id_edge, edges, lanes, id_prtmode, ids_sinkedge = ids_shuntedges)
                            ids_node_out = [edges.ids_tonode[id_edge]]
                            self.add_row(ids_node=id_node,
                                         ids_node_in1=id_node_up,
                                         distances_node_in1=dist,
                                         ids_nodes_out=ids_node_out,
                                         are_station=True,
                                         )

                    if self.is_platform(ids_lane_in, lanes):
                        print '    mixed access level of incoming edge, check for platform exit node'

                        id_edge1, id_edge2 = ids_edge_to
                        ids_lane1, ids_lane2 = edges.ids_lanes[ids_edge_to]

                        # here we could also decide on the number of lanes
                        # but this may not be robust in the future
                        print '    out id_edge1', id_edge1, 'ids_lane1', ids_lane1, 'is_prt_only', self.is_prt_only(ids_lane1, lanes)
                        print '    out id_edge2', id_edge2, 'ids_lane2', ids_lane2, 'is_prt_only', self.is_prt_only(ids_lane2, lanes)
                        # if self.is_prt_only(ids_lane1, lanes) & self.is_prt_only(ids_lane2, lanes):
                        if self.is_prt_only(ids_lane2, lanes):

                            print '      Ped exit on outedge 1'
                            #id_node_up, dist = self.search_upstream_merge(id_edge_from, edges, lanes, id_prtmode)
                            ids_node_out = self.search_downstream_merges(
                                id_edge2, edges, lanes, id_prtmode, ids_sinkedge=ids_shuntedges)
                            id_merge = self.add_row(ids_node=id_node,
                                                    #ids_node_in1 = id_node_up,
                                                    #distances_node_in1 = dist,
                                                    ids_nodes_out=ids_node_out,
                                                    are_station=True,
                                                    )

                        elif self.is_prt_only(ids_lane1, lanes):
                            print '      Ped exit on outedge 2'
                            #id_node_up, dist = self.search_upstream_merge(id_edge_from, edges, lanes, id_prtmode)
                            ids_node_out = self.search_downstream_merges(
                                id_edge1, edges, lanes, id_prtmode, ids_sinkedge=ids_shuntedges)
                            id_merge = self.add_row(ids_node=id_node,
                                                    #ids_node_in1 = id_node_up,
                                                    #distances_node_in1 = dist,
                                                    ids_nodes_out=ids_node_out,
                                                    are_station=True,
                                                    )

                if id_merge >= 0:
                    self.print_attrs(ids=[id_merge, ])

        # now check if merge has entry line form stations or compressors'#,self.ids_node._index_to_id
        ids = self.get_ids()
        i = 0
        while i < len(ids):
            id_merge = ids[i]

            id_node_in1 = self.ids_node_in1[id_merge]
            id_node_in2 = self.ids_node_in2[id_merge]
            print '  check entry of id_merge', id_merge, 'id_node', self.ids_node[id_merge], ' no decomp', self.ids_node[id_merge] not in ids_node_decompressorout
            if (id_node_in1 > -1) & (id_node_in2 > -1)\
                    & (self.ids_node[id_merge] not in ids_node_decompressorout):

                print '  id_node_in1', self.ids_node_in1[id_merge], nodes.ids_sumo[self.ids_node_in1[id_merge]], self.ids_node.has_index(self.ids_node_in1[id_merge])
                print '  id_node_in2', self.ids_node_in2[id_merge], nodes.ids_sumo[self.ids_node_in2[id_merge]], self.ids_node.has_index(self.ids_node_in2[id_merge])
                if 1:
                    id_merge_in1 = self.ids_node.get_id_from_index(self.ids_node_in1[id_merge])
                    id_merge_in2 = self.ids_node.get_id_from_index(self.ids_node_in2[id_merge])
                    print '    station check id_merge', id_merge, 'id_merge_in1', id_merge_in1, 'station', self.are_station[id_merge_in1], 'id_merge_in2', id_merge_in2, 'station', self.are_station[id_merge_in2]

                    if self.are_station[id_merge_in2]:
                        self.are_entry[id_merge] = True
                        # in line 2 is already entry line
                        pass

                    elif self.are_station[id_merge_in1]:
                        self.are_entry[id_merge] = True
                        # move entry line from inline 1 to inline 2

                        id_node_in2 = self.ids_node_in2[id_merge]
                        dist2 = self.distances_node_in2[id_merge]

                        self.ids_node_in2[id_merge] = self.ids_node_in1[id_merge]
                        self.distances_node_in2[id_merge] = self.distances_node_in1[id_merge]

                        self.ids_node_in1[id_merge] = id_node_in2
                        self.distances_node_in1[id_merge] = dist2

            i += 1

    def get_scenario(self):
        return self.parent.get_scenario()

    def prepare_sim(self, process):
        print 'Mergenodes.prepare_sim'
        net = self.get_scenario().net
        nodes = net.nodes
        edges = net.edges
        lanes = net.lanes
        ids_edge_sumo = edges.ids_sumo

        id_prtmode = self.parent.id_prtmode
        self.decel = self.parent.prtvehicles.decel_emergency

        ids = self.get_ids()
        if len(ids) == 0:
            return []
        n_id_max = np.max(ids)+1

        # database with vehicle id as key and controlling merge as value
        self.vehicle_to_merge = {}

        # vehicle database for merge operation
        self.vehicles_entries = {}
        self.vehicles_mains = {}

        #self.ids_merge_to_ids_node_sumo = np.zeros(n_id_max,dtype = np.object)
        #self.ids_merge_to_ids_node_sumo[ids] = nodes.ids_sumo[self.ids_node[ids]]

        self.ids_merge_to_ids_edge_out_sumo = np.zeros(n_id_max, dtype=np.object)
        #self.ids_node_to_ids_edge_out_sumo = np.zeros(n_id_max,dtype = np.object)
        self.ids_node_to_ids_merge = np.zeros(np.max(self.ids_node[ids])+1, dtype=np.int32)
        self.ids_node_to_ids_merge[self.ids_node[ids]] = ids

        # this queue contains all vehicles on the outgoing edge
        # it is used to identify whether a new vehicle has left the merge
        self.ids_vehs_out_sumo = np.zeros(n_id_max, dtype=np.object)

        # this queue contains all vehicles on line 1 between
        # merge and id_node_in1
        self.ids_vehs_in1 = np.zeros(n_id_max, dtype=np.object)
        self.ids_vehs_in1_sumo = np.zeros(n_id_max, dtype=np.object)

        # this queue contains all vehicles on line 2 between
        # merge and id_node_in2
        self.ids_vehs_in2 = np.zeros(n_id_max, dtype=np.object)
        self.ids_vehs_in2_sumo = np.zeros(n_id_max, dtype=np.object)

        # this queue contains the merged vehicles for the controlled range
        self.ids_vehs_merged = np.zeros(n_id_max, dtype=np.object)
        self.ids_vehs_merged_sumo = np.zeros(n_id_max, dtype=np.object)

        # this queuse contains the line index (1 or 2, 0= not assigned)
        self.lineinds_vehs_merged = np.zeros(n_id_max, dtype=np.object)

        #self.ids_merge_to_ids_edge_out_sumo[ids] = edges.ids_sumo[self.ids_node[ids]]
        ids_node = self.ids_node[ids]
        for id_merge, id_node, ids_edge_out in zip(ids, ids_node, nodes.ids_outgoing[ids_node]):
            if len(ids_edge_out) == 1:
                # regular merge
                self.ids_merge_to_ids_edge_out_sumo[id_merge] = ids_edge_sumo[ids_edge_out[0]]

            elif len(ids_edge_out) == 2:
                # one edge is PRT, the other ped access
                # check accesslevels
                id_edge1, id_edge2 = ids_edge_out
                ids_lane1, ids_lane2 = edges.ids_lanes[ids_edge_out]
                al1 = lanes.get_accesslevel(ids_lane1, id_prtmode)
                al2 = lanes.get_accesslevel(ids_lane2, id_prtmode)
                if al1 == 2:
                    self.ids_merge_to_ids_edge_out_sumo[id_merge] = ids_edge_sumo[id_edge1]
                    #self.ids_node_to_ids_edge_out_sumo[id_node] = ids_edge_sumo[id_edge1]
                if al2 == 2:
                    self.ids_merge_to_ids_edge_out_sumo[id_merge] = ids_edge_sumo[id_edge2]
                    #self.ids_node_to_ids_edge_out_sumo[id_node] = ids_edge_sumo[id_edge2]

            self.ids_vehs_out_sumo[id_merge] = set()

            self.ids_vehs_in1[id_merge] = list()
            self.ids_vehs_in1_sumo[id_merge] = list()
            self.ids_vehs_in2[id_merge] = list()
            self.ids_vehs_in2_sumo[id_merge] = list()

            self.ids_vehs_merged[id_merge] = list()
            self.ids_vehs_merged_sumo[id_merge] = list()
            self.lineinds_vehs_merged[id_merge] = list()

            self.vehicles_entries[id_merge] = OrderedDict()
            self.vehicles_mains[id_merge] = OrderedDict()

        return [(self.time_update.get_value(), self.process_step),
                ]

    def process_step(self, process):
        simtime = process.simtime
        print 79*'_'
        print 'Mergenodes.process_step at', simtime
        net = self.get_scenario().net
        vehicles = self.parent.prtvehicles
        ids = self.get_ids()
        for id_merge, id_node, id_edge_out, ids_node_out, ids_veh_out_sumo, is_entry in\
            zip(ids,
                self.ids_node[ids],
                self.ids_merge_to_ids_edge_out_sumo[ids],
                self.ids_nodes_out[ids],
                self.ids_vehs_out_sumo[ids],
                self.are_entry[ids],
                ):
            print '  '+60*'.'
            print '  process id_merge', id_merge, ',id_node', id_node, net.nodes.ids_sumo[id_node]

            ####
            debug = 0  # id_merge in [3,4]
            # debug =  (id_merge==1)|('gneJ1' in net.nodes.ids_sumo[ids_node_out])#'gneJ29':#'gneJ1':
            #debug = debug |  (id_merge==17)|  ('gneJ29' in net.nodes.ids_sumo[ids_node_out])
            ###

            if debug > 0:
                print '  ids_vehs_merged_sumo', self.ids_vehs_merged_sumo[id_merge]

            # check for new vehicle arrivals/departures
            ids_veh_sumo = set(traci.edge.getLastStepVehicleIDs(id_edge_out))

            if debug:
                print '    ids_veh_sumo_prev=', ids_veh_out_sumo
                print '    ids_veh_sumo=', ids_veh_sumo

            if ids_veh_out_sumo != ids_veh_sumo:

                # attention, this is a list, not an array with numpy indexing
                #ids_veh_entered_sumo = list(ids_veh_sumo.difference(ids_veh_out_sumo))
                ids_veh_entered_sumo = get_entered_vehs(ids_veh_sumo, ids_veh_out_sumo)
                # this is an array
                ids_veh_entered = vehicles.get_ids_from_ids_sumo(ids_veh_entered_sumo)
                #ids_veh_left = vehicles.get_ids_from_ids_sumo(list(ids_veh_sumo_prev.difference(ids_veh_sumo)))
                if debug > 0:
                    print '  ids_veh_entered', ids_veh_entered, type(ids_veh_entered)
                # print '  ids_veh_entered_sumo',ids_veh_entered_sumo
                # print '  ids_leader',vehicles.ids_leader[ids_veh_entered]

                if ids_node_out is None:
                    # the present merge is an end node example at a station
                    ids_merge_out = []
                else:
                    # determine id merge according to given output node
                    ids_merge_out = self.ids_node_to_ids_merge[ids_node_out]
                # if debug>0:
                #    print '  ids_node_out',ids_node_out
                #    print '  ids_merge_out',ids_merge_out
                ids_edge_mergeout_sumo = self.ids_merge_to_ids_edge_out_sumo[ids_merge_out]

                # merge only vehicle without a leader (individual or first in a platoon)
                inds_entered = np.flatnonzero(vehicles.ids_leader[ids_veh_entered] == -1)

                if len(inds_entered) > 0:
                    for ind_entered, id_veh in zip(inds_entered, ids_veh_entered[inds_entered]):

                        id_veh_sumo = ids_veh_entered_sumo[ind_entered]
                        if debug > 0:
                            print '    >>exiting vehicle', id_veh_sumo, 'is_leader', vehicles.ids_leader[id_veh] == -1, 'ids_node_out', ids_node_out, 'ids_merge_out', ids_merge_out

                        # print '      route_sumo',route_sumo

                        if ids_node_out is not None:
                            if debug > 0:
                                print '  check which out mergenode are on the current route of the vehicle'

                            # exit from previous merge
                            # if debug:
                            #    print '    vehicle',id_veh_sumo,'exits id_merge',id_merge
                            self.exit_veh(id_veh, id_veh_sumo, id_merge, vehicles)

                            if len(ids_merge_out) > 0:

                                #  inform the respectine merges
                                # then communicate vehicle with dist to merge
                                # attention, we need the first hit in the route
                                # check which downstream merge to enter
                                route_sumo = get_traci_route(id_veh_sumo)

                                routeinds = [INFINT]*len(ids_merge_out)
                                # print '  ids_merge_out',ids_merge_out
                                i = 0
                                for id_edge_sumo in ids_edge_mergeout_sumo:

                                    if id_edge_sumo in route_sumo:
                                        routeinds[i] = route_sumo.index(id_edge_sumo)
                                    i += 1

                                mergeind = np.argmin(routeinds)

                                if debug > 0:
                                    print '      route_sumo', route_sumo
                                    print '      routeinds', routeinds, 'downstream merge', routeinds[mergeind] < INFINT
                                    print '      ids_edge_mergeout_sumo', ids_edge_mergeout_sumo
                                    print '      mergeind,routeinds', mergeind, routeinds
                                    print '      ids_merge_out[mergeind]', ids_merge_out[mergeind], ids_edge_mergeout_sumo[mergeind]

                                if routeinds[mergeind] < INFINT:
                                    if debug > 0:
                                        print '      merge %d is on the route is_entry' % (ids_merge_out[mergeind])
                                    if self.are_entry[ids_merge_out[mergeind]]:
                                        if debug > 0:
                                            print '      call enter_veh_entry'
                                        self.enter_veh_entry(
                                            id_veh, id_veh_sumo, id_merge, ids_merge_out[mergeind], ids_edge_mergeout_sumo[mergeind], vehicles, debug=debug)
                                    else:
                                        if debug > 0:
                                            print '      call enter_veh'
                                        self.enter_veh(
                                            id_veh, id_veh_sumo, id_merge, ids_merge_out[mergeind], ids_edge_mergeout_sumo[mergeind], vehicles, debug=debug)
                                else:
                                    # no downstream merge, vehicle goes into a station
                                    pass
                            else:
                                # print '  ids_node_out is  empty means that there is a station or compressor'
                                pass

                        else:
                            if debug:
                                print '  ids_node_out is  None means that there is a station or compressor'
                            # shunt edges behind.
                            # completely disconnect from all merge controlls
                            # including ghosts

                            # exit from previous merge
                            self.exit_veh(id_veh, id_veh_sumo, id_merge, vehicles)

                # update vehicles on detection edge
                self.ids_vehs_out_sumo[id_merge] = ids_veh_sumo

            # process special merge decisions and processes for entry merge types
            if is_entry:
                self.process_step_entry(id_merge, vehicles, debug)
        if 0:
            print '========check mergeprocess'
            for id_merge, id_node_sumo, ids_veh_merged_sumo, ids_veh_merged in\
                zip(ids,
                    net.nodes.ids_sumo[self.ids_node[ids]],
                    self.ids_vehs_merged_sumo[ids],
                    self.ids_vehs_merged[ids]
                    ):

                print '  ', id_merge, id_node_sumo, ids_veh_merged_sumo
                # print '    ids_veh_merged',ids_veh_merged

    def exit_veh(self, id_veh, id_veh_sumo, id_merge_from, vehicles, is_remove_from_control=False, debug=0):
        print 'exit_veh id_veh %s, id_merge_from %d ' % (id_veh_sumo, id_merge_from), 'entry', self.are_entry[id_merge_from]
        # print '    check for platooned vehicles:'
        # vehicles.get_platoon(id_veh)
        # in id_merge_from: take vehicle out of merged queue and input queue

        if id_veh in self.ids_vehs_merged[id_merge_from]:
            # enterd veh should be in pole position in merge queue of merge_from

            # pop if vehicles are properly merged
            # id_veh_from = self.ids_vehs_merged[id_merge_from].pop()
            # self.ids_vehs_merged_sumo.ids_vehs_merged[id_merge_from].pop()
            # if id_veh_from != id_veh:
            #    print 'WARNING in enter_veh: veh %d instead of veh %d in polepos of merge %d'%(id_veh_from,id_veh, id_merge_from)
            #    return False
            # lineind = self.lineinds_vehs_merged[id_merge_from].pop()
            # for testing: just remove, no matter where
            ind_pos = self.ids_vehs_merged[id_merge_from].index(id_veh)
            lineind = self.lineinds_vehs_merged[id_merge_from][ind_pos]

            if len(self.ids_vehs_merged[id_merge_from]) > ind_pos+1:
                # there is a vehicle behind=> remove ghost
                id_veh_behind = self.ids_vehs_merged[id_merge_from][ind_pos+1]
                id_veh_tail = vehicles.get_platoontail(id_veh)  # get last in platoon
                vehicles.del_ghost(id_veh_behind, id_veh_tail)
                if debug > 0:
                    print '  del ghost from veh', id_veh_behind, 'ghost', id_veh_tail
                    print '    check ghosts:', vehicles.ids_ghosts[id_veh_behind]
                # is there a vehicle in fron of the removed vehicle
                # this happens if a vehicle is interactively deviated
                if ind_pos > 0:

                    id_veh_infront = self.ids_vehs_merged[id_merge_from][ind_pos-1]
                    id_veh_tail = vehicles.get_platoontail(id_veh_infront)  # get last in platoon

                    id_edge_mergeout_sumo = self.ids_merge_to_ids_edge_out_sumo[id_merge_from]
                    dist_to_merge_behind = get_traci_distance(
                        vehicles.get_id_sumo(id_veh_behind), id_edge_mergeout_sumo, 3.0)
                    dist_to_merge_infront = get_traci_distance(
                        vehicles.get_id_sumo(id_veh_infront), id_edge_mergeout_sumo, 3.0)
                    # print '   now let the vehicle behind %d, d=%.2f'%(id_veh_behind,dist_to_merge_behind),'see the vehicle in front %d, d=%.2f'%(id_veh_infront,dist_to_merge_infront)

                    vehicles.add_ghost(id_veh_behind, id_veh_tail, dist_to_merge_behind, dist_to_merge_infront)

                # remove any possible ghosts from this vehicle to vehicles behind
                # this can happen if this vehicle passed by its ghost vehicle
                vehicles.del_ghosts(id_veh)

            self.ids_vehs_merged[id_merge_from].pop(ind_pos)  # remove(id_veh)
            self.ids_vehs_merged_sumo[id_merge_from].pop(ind_pos)  # remove(id_veh_sumo)
            self.lineinds_vehs_merged[id_merge_from].pop(ind_pos)
            if debug > 0:
                print '    vehicle has been on line index', lineind
            # remove vehicle from line buffers
            if lineind == 1:
                self.ids_vehs_in1[id_merge_from].remove(id_veh)  # pop()
                self.ids_vehs_in1_sumo[id_merge_from].remove(id_veh_sumo)  # .pop()
                if self.are_entry[id_merge_from]:
                    if debug > 0:
                        print '    vehicle is involved in entry merge processes?', self.vehicles_mains[id_merge_from].has_key(id_veh)
                    if self.vehicles_mains[id_merge_from].has_key(id_veh):
                        #
                        #id_veh_entry = self.vehicles_mains[id_merge_from][id_veh]

                        # TODO: this is still a lousy method, vehicles_mains neds to be improved
                        for id_veh_entry, state in zip(self.vehicles_entries[id_merge_from].keys(), self.vehicles_entries[id_merge_from].values()):
                            #state = self.vehicles_entries[id_merge_from][id_veh_entry]
                            # print '      state before',state
                            if state.has_key('id_veh_infront'):
                                if state['id_veh_infront'] == id_veh:
                                    del state['id_veh_infront']
                            if state.has_key('id_veh_behind'):
                                if state['id_veh_behind'] == id_veh:
                                    del state['id_veh_behind']
                            # print '      state after',state

                        del self.vehicles_mains[id_merge_from][id_veh]

            if lineind == 2:
                self.ids_vehs_in2[id_merge_from].remove(id_veh)  # .pop()
                self.ids_vehs_in2_sumo[id_merge_from].remove(id_veh_sumo)  # .pop()

                if self.are_entry[id_merge_from]:
                    if debug > 0:
                        print '    del veh prt.%s from vehicles_entries' % id_veh
                    del self.vehicles_entries[id_merge_from][id_veh]
            else:
                pass

            # remove from centralized database
            del self.vehicle_to_merge[id_veh]

        else:
            # the entered vehicle is not in a merge queue
            # probably a new vehicle at station

            # just be sure that the vehicle is not in any queue
            # but actually this cannot happen
            if id_veh in self.ids_vehs_in1[id_merge_from]:
                print 'WARNING in exit_veh: new veh %d should not be in inqueue 1' % id_veh
                self.ids_vehs_in1[id_merge_from].remove(id_veh)
                self.ids_vehs_in1_sumo[id_merge_from].remove(id_veh_sumo)

            if id_veh in self.ids_vehs_in2[id_merge_from]:
                print 'WARNING in exit_veh: new veh %d should not be in inqueue 2' % id_veh
                self.ids_vehs_in2[id_merge_from].remove(id_veh)
                self.ids_vehs_in2_sumo[id_merge_from].remove(id_veh_sumo)

            if self.vehicle_to_merge.has_key(id_veh):
                del self.vehicle_to_merge[id_veh]

        if is_remove_from_control:
            # remove any merge control operations
            vehicles.del_all_ghosts(id_veh)

    def exit_veh_forced(self, id_veh, id_veh_sumo, vehicles):
        if self.vehicle_to_merge.has_key(id_veh):
            # exit vehicle from respective merge
            self.exit_veh(id_veh, id_veh_sumo, self.vehicle_to_merge[id_veh], vehicles, is_remove_from_control=True)
        else:
            # id_veh not under any merge conrol
            pass

    def print_vehs(self, id_veh1, id_veh2, dist1_min, dist1_max, dist2_min, dist2_max,
                   lineind1, lineind2, pos_max=79):
        dist_max = max(dist1_min, dist1_max, dist2_min, dist2_max)+1
        #dist_min = min(dist1_min, dist1_max, dist2_min, dist2_max)
        #dist_diff = dist_max-dist_min+10.0
        if np.all([dist1_min, dist1_max, dist2_min, dist2_max]) > 0:
            f = float(pos_max)/dist_max
            print '________________________'
            print 'vehicle %s from line %d: %.f--%2f' % (id_veh1, lineind1, dist1_min, dist1_max)
            pos_min = int(dist1_min*f)
            pos_max = int(dist1_max*f)
            print max(pos_min-1, 0)*' '+'<'+(pos_max-pos_min)*'X'+'|'

            pos_min = int(dist2_min*f)
            pos_max = int(dist2_max*f)
            print max(pos_min-1, 0)*' '+'<'+(pos_max-pos_min)*'X'+'|'
            print 'vehicle %s from line %d: %.f--%2f' % (id_veh2, lineind2, dist2_min, dist2_max)
            print '________________________'
        else:
            print 'WARNING: some negative distances:'
            print 'vehicle %s from line %d: %.f--%2f' % (id_veh1, lineind1, dist1_min, dist1_max)
            print 'vehicle %s from line %d: %.f--%2f' % (id_veh2, lineind2, dist2_min, dist2_max)

    def enter_veh(self, id_veh, id_veh_sumo, id_merge_from, id_merge_to, id_edge_merge_sumo, vehicles, debug=0):
        print 'enter_veh id_veh %s, id_merge_from %d to id_merge_to %d' % (id_veh_sumo, id_merge_from, id_merge_to)

        # in id_merge_from: take vehicle out of merged queue and input queue

        # put vehicle in centralized database
        self.vehicle_to_merge[id_veh] = id_merge_to

        # on which input line of merge id_merge_to does the vehicle approach?
        if id_veh in self.ids_vehs_merged[id_merge_to]:
            # vehicle is in merge queue and input line can be retrived from it
            indpos = self.ids_vehs_merged[id_merge_to].index(id_veh)

            # on which input line is the vehicle at position index indpos
            lineind = self.lineinds_vehs_merged[id_merge_to][indpos]

        elif self.ids_node_to_ids_merge[self.ids_node_in1[id_merge_to]] == id_merge_from:
            # vehicle not in merge queue, detect input line 1 with nodes db
            indpos = -1
            lineind = 1
        else:
            # vehicle not in merge queue, detect input line 2 with nodes db
            indpos = -1
            lineind = 2

        if lineind == 1:
            # from line 1
            self.ids_vehs_in1[id_merge_to].append(id_veh)
            self.ids_vehs_in1_sumo[id_merge_to].append(id_veh_sumo)
            dist_tomerge_head_new = self.distances_node_in1[id_merge_to]  # correct with pos

        elif lineind == 2:
            self.ids_vehs_in2[id_merge_to].append(id_veh)
            self.ids_vehs_in2_sumo[id_merge_to].append(id_veh_sumo)
            dist_tomerge_head_new = self.distances_node_in2[id_merge_to]  # correct with pos

        if indpos == -1:
            # vehicle is new and must be merged into   ids_vehs_merged
            if debug > 0:
                print '  merge veh %d arriving from in %d at dist %.2fm' % (id_veh, lineind, dist_tomerge_head_new)

            ids_vehs_merged = self.ids_vehs_merged[id_merge_to]
            ids_vehs_merged_sumo = self.ids_vehs_merged_sumo[id_merge_to]
            lineinds_vehs_merged = self.lineinds_vehs_merged[id_merge_to]
            #id_edge_merge_sumo = self.ids_merge_to_ids_edge_out_sumo[id_merge_to]
            ind_insert = len(ids_vehs_merged)
            is_insert = False
            id_veh_merged = -1

            # print '  ids_vehs_merged_sumo[%d]'%id_merge_to,ids_vehs_merged_sumo
            # print '  lineinds_vehs_merged[%d]'%id_merge_to,lineinds_vehs_merged
            if (ind_insert == 0) | self.are_station[id_merge_to]:
                if debug > 0:
                    print '    new vehicle is the only vehicle or station', ind_insert, self.are_station[id_merge_to]

                # vehicles heading toward a station merge are not
                # really merged because only one incoming line
                ids_vehs_merged.append(id_veh)
                ids_vehs_merged_sumo.append(id_veh_sumo)
                lineinds_vehs_merged.append(lineind)

            elif ind_insert > 0:  # there is at least another vehicle
                # slower, but more precise than  self.distances_node_inX[id_merge_to]
                #dist_tomerge_head_new2 =  get_traci_distance(id_veh_sumo,id_edge_merge_sumo, 3.0)
                #dist_tomerge_tail_new = dist_tomerge_head_new-0.5/ get_traci_velocity(id_veh_sumo)**2+vehicles.lengths_plat[id_veh]
                #dist_tomerge_tail_new = dist_tomerge_head_new+vehicles.lengths_plat[id_veh]
                id_veh_tail_new = vehicles.get_platoontail(id_veh)
                id_veh_tail_new_sumo = vehicles.ids_sumo[id_veh_tail_new]
                dist_tomerge_tail_new = get_traci_distance(id_veh_tail_new_sumo, id_edge_merge_sumo, 3.0)
                if debug > 0:
                    # print '    new veh %d arriving from in %d at dist head %.2f /__| tail %.2fm'%(id_veh,lineind,dist_tomerge_head_new,dist_tomerge_tail_new)
                    print '       ids_vehs_merged_sumo', ids_vehs_merged_sumo
                for id_veh_merged, id_veh_merged_sumo in zip(ids_vehs_merged[::-1], ids_vehs_merged_sumo[::-1]):
                    dist_tomerge_head = get_traci_distance(id_veh_merged_sumo, id_edge_merge_sumo, 3.0)
                    #stoppeddist_tomerge = dist-0.5/self.decel*get_traci_velocity(id_veh_merged_sumo)**2+vehicles.lengths_plat[id_veh]
                    #stoppeddist_tomerge = dist+vehicles.lengths_plat[id_veh]
                    id_veh_tail = vehicles.get_platoontail(id_veh_merged)
                    id_veh_tail_sumo = vehicles.ids_sumo[id_veh_tail]
                    dist_tomerge_tail = get_traci_distance(id_veh_tail_sumo, id_edge_merge_sumo, 3.0)

                    if debug > 0:
                        self.print_vehs(id_veh, id_veh_merged,
                                        dist_tomerge_head_new, dist_tomerge_tail_new,
                                        dist_tomerge_head, dist_tomerge_tail,
                                        lineind, lineinds_vehs_merged[ind_insert-1])
                        # print '    check veh %d arriving from in %d at dist head %.2f /__| tail %.2fm'%(id_veh,lineind,dist_tomerge_head_new,dist_tomerge_tail_new)
                        # print '      check veh %d, d  %.2fm, d_new  %.2fm, ind_insert = %d'%(id_veh_merged,dist,dist_tomerge_head_new,ind_insert)
                        # print '                    ds %.2fm, ds_new %.2fm, v %.2f, v_new =%.2fm, '%(stoppeddist_tomerge,dist_tomerge_tail_new,get_traci_velocity(id_veh_merged_sumo),get_traci_velocity(id_veh_sumo),)
                        # print '       tail veh %d, ds %.2fm'%(id_veh_tail,dist_tomerge_tail)

                    if lineind == lineinds_vehs_merged[ind_insert-1]:
                        is_insert = True
                        break

                    if dist_tomerge_tail_new > dist_tomerge_tail:
                        # distance to merge of this vehicle in queue is greater
                        # than the new vehicle
                        # inseri on copies? No break

                        # isert in fron of currently checked vehicle
                        is_insert = True
                        break
                    dist_last = dist_tomerge_head
                    ind_insert -= 1

                if is_insert:
                    # at least one vehicle is in front
                    if debug > 0:
                        print '    insert veh %d behind veh %d, index %d' % (id_veh, id_veh_merged, ind_insert)
                    # send control info to involved vehicles
                    if ind_insert == len(ids_vehs_merged):
                        if debug > 0:
                            print '    appended vehicle after veh', id_veh_tail_sumo, 'with leader', ids_vehs_merged_sumo[ind_insert-1], 'dtm=%.2fm' % dist_tomerge_tail
                        # V
                        # |
                        # G ind_insert-1

                        # is vehicle and ghost in the same input line?
                        if lineinds_vehs_merged[ind_insert-1] != lineind:
                            # id_ghost = ids_vehs_merged[ind_insert-1]# last veh in queue
                            #vehicles.add_ghost(id_veh, id_ghost, dist_tomerge_head_new, dist)

                            vehicles.add_ghost(id_veh, id_veh_tail, dist_tomerge_head_new, dist_tomerge_tail)

                    elif ind_insert > 0:
                        # there is at least 1 other veh in front
                        if debug > 0:
                            print '    vehicle will be inserted in front of', ids_vehs_merged_sumo[ind_insert], 'and in behind', id_veh_tail_sumo, 'with leader', ids_vehs_merged_sumo[ind_insert-1], 'dtm=%.2fm' % dist_tomerge_tail
                        # G1
                        # |
                        # V
                        # |
                        # G2
                        id_ghost1 = ids_vehs_merged[ind_insert]
                        #id_ghost2 = ids_vehs_merged[ind_insert-1]
                        id_ghost2 = id_veh_tail

                        vehicles.del_ghost(id_ghost1, id_ghost2)
                        if lineinds_vehs_merged[ind_insert] != lineind:
                            vehicles.add_ghost(id_ghost1, id_veh_tail_new, dist_last, dist_tomerge_tail_new)

                        if lineinds_vehs_merged[ind_insert-1] != lineind:
                            #vehicles.add_ghost(id_veh, id_ghost2, dist_tomerge_head_new, dist)
                            vehicles.add_ghost(id_veh, id_ghost2, dist_tomerge_head_new, dist_tomerge_tail)

                else:
                    if debug > 0:
                        print '    prepend veh %d in front of veh %d, first index %d' % (id_veh, id_veh_merged, ind_insert)
                    # is vehicle and ghost in the same input line?
                    if lineinds_vehs_merged[ind_insert] != lineind:
                        id_veh_behind = ids_vehs_merged[ind_insert]  # last veh in queue
                        vehicles.add_ghost(id_veh_behind, id_veh_tail_new, dist_tomerge_head, dist_tomerge_tail_new)

                # finally insert vehicle in merge queue
                ids_vehs_merged.insert(ind_insert, id_veh)
                ids_vehs_merged_sumo.insert(ind_insert, id_veh_sumo)
                lineinds_vehs_merged.insert(ind_insert, lineind)

                # inform downstream merges

            # else:
            #    # new vehicle is the only vehicle
            #    ids_vehs_merged[0] = id_veh
            #    ids_vehs_merged_sumo[0] = id_veh_sumo
            #    lineinds_vehs_merged[0] = lineind

    def enter_veh_entry(self, id_veh, id_veh_sumo, id_merge_from, id_merge_to, id_edge_merge_sumo, vehicles, debug=0):
        print 'enter_veh_entry id_veh %s, id_merge_from %d to id_merge_to %d' % (id_veh_sumo, id_merge_from, id_merge_to)

        # in id_merge_from: take vehicle out of merged queue and input queue

        # put vehicle in centralized database
        self.vehicle_to_merge[id_veh] = id_merge_to

        # on which input line of merge id_merge_to does the vehicle approach?
        if id_veh in self.ids_vehs_merged[id_merge_to]:
            # vehicle is in merge queue and input line can be retrived from it
            indpos = self.ids_vehs_merged[id_merge_to].index(id_veh)

            # on which input line is the vehicle at position index indpos
            lineind = self.lineinds_vehs_merged[id_merge_to][indpos]

        elif self.ids_node_to_ids_merge[self.ids_node_in1[id_merge_to]] == id_merge_from:
            # vehicle not in merge queue, detect input line 1 with nodes db
            indpos = -1
            lineind = 1
        else:
            # vehicle not in merge queue, detect input line 2 with nodes db
            indpos = -1
            lineind = 2

        # put vehicle in respective line queue
        ids_vehs_merged = self.ids_vehs_merged[id_merge_to]
        ids_vehs_merged_sumo = self.ids_vehs_merged_sumo[id_merge_to]
        lineinds_vehs_merged = self.lineinds_vehs_merged[id_merge_to]

        if lineind == 1:
            # from line 1 (main line)
            if debug > 0:
                print '  Detected veh', id_veh_sumo, 'on mainline. Is new', indpos == -1
                print '    check for platooned vehicles:'
                vehicles.get_platoon(id_veh)

            id_edge_out_sumo = self.ids_merge_to_ids_edge_out_sumo[id_merge_to]
            dist_tomerge = get_traci_distance(id_veh_sumo, id_edge_out_sumo, 3.0)
            ids_vehs_in1 = self.ids_vehs_in1[id_merge_to]
            if len(ids_vehs_in1) > 0:
                id_veh1_last = self.ids_vehs_in1[id_merge_to][-1]
            else:
                id_veh1_last = -1

            id_veh2, state2 = self.get_mergeveh_entry(id_merge_to)
            if id_veh2 == -1:
                # no vehicle to merge
                if id_veh1_last != -1:
                    # make ghost to last vehicle
                    #id_veh_tail = vehicles.get_platoontail(id_veh1_last)
                    #id_veh_tail_sumo = vehicles.ids_sumo[id_veh_tail]
                    #dist_tomerge_tail = get_traci_distance(id_veh_tail_sumo, id_edge_out_sumo, 3.0)
                    #vehicles.add_ghost(id_veh, id_veh_tail, dist_tomerge, dist_tomerge_tail)

                    # let SUMO do the distances
                    pass

                else:
                    # merge is empty
                    pass

            elif state2['status'] != 'wait':
                # there is an approaching vehicle on entry line
                if state2['status'] == 'accelerate':
                    # vehicle in acceleration mode
                    if state2.has_key('id_veh_behind'):
                        # accelerating vehicle has already a vehicle
                        # in front of which to merge
                        # => look at last vehicle

                        # let SUMO do the distances
                        pass

                    else:
                        # let the entering vehicle see the new vehicle
                        state2['id_veh_behind'] = id_veh

                if state2['status'] == 'sync':
                    if state2.has_key('id_veh_behind'):
                        # accelerating vehicle has already a vehicle
                        # in front of which to merge
                        # => look at last vehicle

                        # let SUMO do the distances
                        pass

                    else:
                        # speed is already synchonized, but has not yet
                        # a vehicle behind
                        # let look the new vehicle look at
                        # the tail of the entering vehicle
                        id_veh_tail = vehicles.get_platoontail(id_veh2)
                        id_veh_tail_sumo = vehicles.ids_sumo[id_veh_tail]
                        dist_tomerge_tail = get_traci_distance(id_veh_tail_sumo, id_edge_out_sumo, 3.0)
                        vehicles.add_ghost(id_veh, id_veh_tail, dist_tomerge, dist_tomerge_tail)

            self.ids_vehs_in1[id_merge_to].append(id_veh)
            self.ids_vehs_in1_sumo[id_merge_to].append(id_veh_sumo)

            # append to main merge queue
            ids_vehs_merged.append(id_veh)
            ids_vehs_merged_sumo.append(id_veh_sumo)
            lineinds_vehs_merged.append(lineind)

        elif lineind == 2:
            # from line 2 (entry line)
            if debug > 0:
                print '  Detected veh', id_veh_sumo, 'on entry line. Is new', indpos == -1

            self.ids_vehs_in2[id_merge_to].append(id_veh)
            self.ids_vehs_in2_sumo[id_merge_to].append(id_veh_sumo)
            if debug > 0:
                print '    command vehicle to stop and wait further instructions'
            #vehicles.control_slow_down(id_veh, speed = 6.0/3.6)
            #traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)
            # print '    set speed to',traci.vehicle.getMaxSpeed(id_veh_sumo)
            #traci.vehicle.slowDown(id_veh_sumo, 13.0, 5.0)

            vehicles.control_stop(id_veh)

            # prevent SUMO from reaccelerating vehicle
            # vehicles.switch_off_control(id_veh)
            if debug > 0:
                print '    set veh id_veh prt.%d' % id_veh
            self.vehicles_entries[id_merge_to][id_veh] = {'status': 'wait'}
            if debug > 0:
                print '    vehicles_entries[', id_merge_to, ']=', self.vehicles_entries[id_merge_to]
            #
            # self.vehicles_mains[id_merge_to][id_veh] = {}# later when used

            # Attention: Vehicle on entry line not yet in queue.
            # this needs to be fixed later when it becomes clear where to insert
            # ids_vehs_merged.append(id_veh)
            # ids_vehs_merged_sumo.append(id_veh_sumo)
            # lineinds_vehs_merged.append(lineind)

    def get_mergeveh_entry(self, id_merge):
        """Returns ID and state of vehicle that is about to merege from
        the entry line. ID is -1 if no vehicle starts.
        """
        vehicles_entries = self.vehicles_entries[id_merge]
        if len(vehicles_entries) == 0:
            return -1, {'status': 'wait'}
        else:
            return vehicles_entries.keys()[0], vehicles_entries.values()[0]

    def process_step_entry(self, id_merge, vehicles, debug):
        print 'process_step_entry id_merge', id_merge
        if debug > 0:
            print '  vehicles_entries=', self.vehicles_entries[id_merge]
            print '  vehicles_mains', self.vehicles_mains[id_merge]
        # print '  self.vehicles_entries',self.vehicles_entries
        #self.vehicles_entries[id_merge]= OrderedDict()
        # self.vehicles_mains[id_merge] = OrderedDict()
        id_veh_del = -1

        # for id_veh, state in zip(self.vehicles_entries[id_merge].keys()[::-1],self.vehicles_entries[id_merge].values()[::-1]):
        # for id_veh, state in self.vehicles_entries.iteritems():
        # for id_veh, state in zip(self.vehicles_entries[id_merge].keys(),self.vehicles_entries[id_merge].values()):
        ids_veh_entry = self.vehicles_entries[id_merge].keys()
        states_veh_entry = self.vehicles_entries[id_merge].values()
        id_edge_out_sumo = self.ids_merge_to_ids_edge_out_sumo[id_merge]
        ids_veh_in1 = self.ids_vehs_in1[id_merge]
        time_update = self.time_update.get_value()
        i = 0
        #is_cont = len(ids_veh_entry)>0
        # while is_cont:
        if len(ids_veh_entry) > 0:
            id_veh = ids_veh_entry[0]
            id_veh_sumo = vehicles.get_id_sumo(id_veh)
            state = states_veh_entry[0]

            ids_vehs_merged = self.ids_vehs_merged[id_merge]
            ids_vehs_merged_sumo = self.ids_vehs_merged_sumo[id_merge]
            lineinds_vehs_merged = self.lineinds_vehs_merged[id_merge]
            if debug > 0:
                print '  check id_veh_sumo', id_veh_sumo, 'status', state['status'], 'n vehs on main', len(ids_veh_in1)
            if state['status'] == 'wait':
                if traci.vehicle.isStopped(id_veh_sumo):
                    # check potential conflict vehicle on main line

                    n1 = len(ids_veh_in1)
                    if n1 == 0:
                        if debug > 0:
                            print '    main line is empty => accelerate immediately'
                        for id_veh_plat in vehicles.get_platoon(id_veh):
                            vehicles.control_speedup(id_veh_plat)
                        state['status'] = 'accelerate'

                        ids_vehs_merged.append(id_veh)
                        ids_vehs_merged_sumo.append(id_veh_sumo)
                        lineinds_vehs_merged.append(2)

                    else:
                        ids_veh_in1_sumo = self.ids_vehs_in1_sumo[id_merge]
                        vehicles_main = self.vehicles_mains[id_merge]
                        dist_in1 = self.distances_node_in1[id_merge]
                        dist_in2 = self.distances_node_in2[id_merge]

                        i = 0  # n1-1
                        is_found = False
                        while (i < n1) & (not is_found):
                            id_veh1_sumo = ids_veh_in1_sumo[i]
                            id_veh1 = ids_veh_in1[i]
                            if debug > 0:
                                print '    check', id_veh1_sumo  # ,'free',id_veh1 not in vehicles_main

                            if True:  # id_veh1 not in vehicles_main:

                                # get tail position of id_veh_main
                                id_veh1_tail = vehicles.get_platoontail(id_veh1)
                                id_veh1_tail_sumo = vehicles.ids_sumo[id_veh1_tail]
                                pos1 = dist_in1-get_traci_distance(id_veh1_sumo, id_edge_out_sumo, 3.0)
                                pos1_tail = dist_in1-get_traci_distance(id_veh1_tail_sumo, id_edge_out_sumo, 3.0)
                                pos2 = dist_in2 - get_traci_distance(id_veh_sumo, id_edge_out_sumo, 3.0)
                                p_from, p_to = self.get_pos_crit_entry(pos2,
                                                                       vehicles.lengths_plat[id_veh],
                                                                       get_traci_velocity(id_veh1_sumo),
                                                                       dist_in1, dist_in2,
                                                                       vehicles)
                                if debug > 0:
                                    print '    potential veh %s (tail %s)' % (id_veh1_sumo, id_veh1_tail_sumo), 'at pos1_tail=%.1f>p_to=%.1f' % (pos1_tail, p_to), pos1_tail > p_to

                                    print '       i=%d, n1=%d' % (i, n1), 'last vehicle on main', i == n1-1, 'more', i < n1-1

                                    #self.print_vehs(id_veh1, id_veh2, dist1_min, dist1_max, dist2_min, dist2_max,lineind1, lineind2, pos_max = 79)

                                # here we check whether the tail of the vehicle on the main line
                                # has passed the critical point p_to

                                if pos1_tail > p_to:

                                    if i == n1-1:  # last vehicle on main
                                        if debug > 0:
                                            print '      insert id_veh', id_veh_sumo, 'behind id_veh1', id_veh1_sumo, 'the only veh on main'
                                        state['id_veh_infront'] = id_veh1
                                        vehicles_main[id_veh1] = id_veh  # no, it does not get a ghost
                                        # vehicles_main[id_veh1] = {  'id_veh':id_veh,
                                        #                            'delta':pos1_tail-p_to
                                        #                                }
                                        is_found = True
                                        ids_vehs_merged.append(id_veh)
                                        ids_vehs_merged_sumo.append(id_veh_sumo)
                                        lineinds_vehs_merged.append(2)

                                    elif i < n1-1:  # there are others behind on main
                                        # ensure that gap is big enough
                                        id_veh_behind = ids_veh_in1[i+1]
                                        id_veh_behind_sumo = vehicles.ids_sumo[id_veh_behind]
                                        pos1 = dist_in1 - get_traci_distance(id_veh_behind_sumo, id_edge_out_sumo, 3.0)
                                        if debug > 0:
                                            print '      vehicle behind', id_veh_behind_sumo, 'pos=%.1f, p_from=%.1f' % (pos1, p_from), 'ok', pos1 < p_from
                                        if pos1 < p_from:
                                            state['id_veh_infront'] = id_veh1
                                            state['id_veh_behind'] = id_veh_behind

                                            vehicles_main[id_veh_behind] = id_veh
                                            #vehicles_main[id_veh1] = {  'id_veh_infront':id_veh}
                                            is_found = True
                                            j = ids_vehs_merged.index(id_veh1)+1
                                            # print '        j=',j
                                            ids_vehs_merged.insert(j, id_veh)
                                            ids_vehs_merged_sumo.insert(j, id_veh_sumo)
                                            lineinds_vehs_merged.insert(j, 2)

                                # here we check whether the vehicle on the main line
                                # has not yet reached the critical point p_from
                                elif pos1 < p_from:
                                    if i == 0:  # first vehicle on main
                                        if debug > 0:
                                            print '      insert id_veh', id_veh_sumo, 'in front of id_veh1', id_veh1_sumo, 'the only veh on main'
                                        state['id_veh_behind'] = id_veh1
                                        vehicles_main[id_veh1] = id_veh
                                        is_found = True
                                        # determine insert position
                                        j = ids_vehs_merged.index(id_veh1)
                                        # print '        j=',j
                                        ids_vehs_merged.insert(j, id_veh)
                                        ids_vehs_merged_sumo.insert(j, id_veh_sumo)
                                        lineinds_vehs_merged.insert(j, 2)

                                    # elif i < n1-1: # there are others behind on main
                                    #    # ensure that gap is big enough
                                    #    id_veh_behind = ids_veh_in1[i+1]
                                    #    id_veh_behind_sumo = vehicles.ids_sumo[id_veh_behind]
                                    #    pos1 = dist_in1 - get_traci_distance(id_veh_behind_sumo, id_edge_out_sumo, 3.0)
                                    #    print '      vehicle behind',id_veh_behind_sumo,'pos=%.1f, p_from=%.1f'%(pos1,p_from),'ok',pos1<p_from
                                    #    if pos1<p_from:
                                    #        state['id_veh_infront'] = id_veh1
                                    #        state['id_veh_behind'] = id_veh_behind
                                    #        is_found = True

                            i += 1

                        if is_found:
                            if debug > 0:
                                print '    suitable vehicle after which entry vehicle can run has been found'
                            # Note: if no vehicle has been found then
                            # nothing will happen and the vehicle will
                            # wait until the righ moment has arrived
                            for id_veh_plat in vehicles.get_platoon(id_veh):
                                vehicles.control_speedup(id_veh_plat)
                            state['status'] = 'accelerate'

                        else:
                            if debug > 0:
                                print '    nothing will happen and the vehicle will  wait until the righ moment has arrived'

            elif state['status'] == 'accelerate':
                # test if speed reached
                if traci.vehicle.getSpeed(id_veh_sumo) > 0.9*vehicles.speed_max:
                    if debug > 0:
                        print '    synchronization reached for veh', id_veh_sumo
                    state['status'] = 'sync'
                    #id_veh_del = id_veh
                    # now create ghosts
                    # id_veh -> id_veh_infront_tail:
                    if state.has_key('id_veh_infront'):
                        id_veh_in_front = state['id_veh_infront']
                        id_veh_infront_tail = vehicles.get_platoontail(id_veh_in_front)
                        id_veh_infront_tail_sumo = vehicles.ids_sumo[id_veh_infront_tail]
                        dist_tomerge = get_traci_distance(id_veh_sumo, id_edge_out_sumo, 3.0)
                        dist_tomerge_tail = get_traci_distance(id_veh_infront_tail_sumo, id_edge_out_sumo, 3.0)
                        if debug > 0:
                            print '      add ghost to entering veh', id_veh_sumo, ' behind', id_veh_infront_tail_sumo, 'with leader', id_veh_in_front
                        vehicles.add_ghost(id_veh, id_veh_infront_tail, dist_tomerge,
                                           dist_tomerge_tail, is_substitute=True)

                    if state.has_key('id_veh_behind'):
                        id_veh_behind = state['id_veh_behind']
                        id_veh_behind_sumo = vehicles.ids_sumo[id_veh_behind]

                        dist_tomerge_behind = get_traci_distance(id_veh_behind_sumo, id_edge_out_sumo, 3.0)

                        id_veh_tail = vehicles.get_platoontail(id_veh)
                        id_veh_tail_sumo = vehicles.ids_sumo[id_veh_tail]
                        dist_tomerge_tail = get_traci_distance(id_veh_tail_sumo, id_edge_out_sumo, 3.0)
                        # print '      add ghost to mainline veh',id_veh_behind_sumo,' behind',id_veh_tail_sumo,'with leader',id_veh_sumo
                        vehicles.add_ghost(id_veh_behind, id_veh_tail, dist_tomerge_behind,
                                           dist_tomerge_tail, is_substitute=True)

                else:
                    if debug > 0:
                        speed = traci.vehicle.getSpeed(id_veh_sumo)
                        print '    sync of veh', id_veh_sumo, ',v=%.1f, not yet reached: %.2f' % (speed, speed/vehicles.speed_max)

                    if state.has_key('id_veh_behind'):
                        id_veh_behind_sumo = vehicles.ids_sumo[state['id_veh_behind']]
                        info = traci.vehicle.getLeader(id_veh_behind_sumo, dist=200.0)
                        if (info is not None):
                            # extract lead vehicle ID and distance from info
                            id_leader_sumo, dist_leader = info
                            #speed = vehicles.speed_max
                            speed = get_traci_velocity(id_veh_behind_sumo)
                            dist_safe = vehicles.tau*speed + 0.5*speed**2/vehicles.decel_emergency
                            dist_target = 2*dist_safe+vehicles.lengths_plat[id_veh]
                            if debug > 0:
                                print '      behind', id_veh_behind_sumo, 'with infront', id_leader_sumo, 'at dist=%.2f' % dist_leader, 'at ds=%.2f' % dist_safe, 'is_brake', dist_leader < dist_target
                            if dist_leader < dist_target:
                                dv = time_update*vehicles.decel
                                if (speed-dv) > 0:
                                    if debug > 0:
                                        print '      slowdown', id_veh_behind_sumo, 'from speed %.2f to %.2f' % (speed, speed-dv)
                                    traci.vehicle.slowDown(id_veh_behind_sumo, speed-dv, time_update)
                            else:
                                dv = time_update*vehicles.decel
                                if debug > 0:
                                    print '      accel', id_veh_behind_sumo, 'from speed %.2f to %.2f' % (speed, speed+dv)
                                traci.vehicle.slowDown(id_veh_behind_sumo, speed+dv, time_update)

            elif state['status'] == 'sync':
                pass

        # if id_veh_del > -1:
        #    del self.vehicles_entries[id_merge][id_veh_del]

    def get_pos_crit_entry(self, pos2, len_veh2, speed, dist1, dist2, vehicles):
        """
        Returns critical position interval of vehicle-platoon on main line
        with respect to the starting position of a vehicle at entry line.
        The returned critical position interval(pos_from, pos_to), which should
        ideally be free of vehicles.
        pos_from refers to the position of the nose of a vehicle on the main line
        pos_to refers to the position of the tail of the platoon on the main line.

        """
        # print 'get_pos_crit_entry pos2=%.1f, v=%.1f, d1=%.1f, d2=%.1f'%(pos2, speed, dist1, dist2)
        # see 191030
        decel_emergency = vehicles.decel_emergency
        decel_comfort = vehicles.decel
        time_emergency = vehicles.tau
        s2 = 0.5*speed**2/decel_comfort
        delta2 = dist2 - pos2 - s2

        s1 = speed**2/decel_comfort
        s_safe = time_emergency*speed + 0.5*speed**2/decel_emergency
        p_to = dist1 - delta2 - s1 + s_safe
        p_from = p_to-len_veh2-2*s_safe
        # print '  delta2=%.1f, s2=%.1f, s1=%.1f, ss=%.1f, CI=(%.1f,%.1f)'%(delta2,s2,s1,s_safe,p_from,p_to)
        return p_from, p_to

    def search_upstream_merge(self, id_edge_start, edges, lanes, id_prtmode, ids_mainline=set()):
        """
        Searches next upstream merge node.
        Returns id_node, length to node

        """
        # print 'search_upstream_merge id_edge_start',id_edge_start
        length = edges.lengths[id_edge_start]
        is_merge = False
        id_edge_platform = -1
        id_edge = id_edge_start
        while (not is_merge):
            ids_edge_incoming = edges.get_incoming(id_edge)

            # print '    search id_edge,ids_edge_incoming',id_edge,ids_edge_incoming
            if len(ids_edge_incoming) == 2:
                #id_edge1, id_edge2 =  ids_edge_to
                ids_lane1, ids_lane2 = edges.ids_lanes[ids_edge_incoming]
                #al1 = lanes.get_accesslevel(ids_lane1, id_prtmode)
                #al2 = lanes.get_accesslevel(ids_lane2, id_prtmode)
                # print '  check: '
                # print '    id_edge1',ids_edge_incoming[0],'ids_lane1',ids_lane1,self.is_prt_only(ids_lane1, lanes)
                # print '    id_edge2',ids_edge_incoming[1],'ids_lane2',ids_lane2,self.is_prt_only(ids_lane2, lanes)
                if self.is_prt_only(ids_lane1, lanes) & self.is_prt_only(ids_lane2, lanes):
                    # print '    2 in, one out => it is a merge node'
                    is_merge = True

            elif len(ids_edge_incoming) == 1:
                ids_lane = edges.ids_lanes[ids_edge_incoming[0]]
                #al = lanes.get_accesslevel(ids_lane, id_prtmode)
                if self.is_prt_only(ids_lane, lanes) & (id_edge in ids_mainline):
                    # print '    mainline input node
                    is_merge = True

                if self.is_platform(ids_lane, lanes):
                    # print '    1 in, with multiple access => it is a station output node'
                    is_merge = True

            if not is_merge:
                id_edge = ids_edge_incoming[0]
                length += edges.lengths[id_edge]

            # print '    id_edge,is_merge',id_edge,is_merge
        # print '  found node',edges.ids_fromnode[id_edge]
        return edges.ids_fromnode[id_edge], length

    def search_downstream_merges(self, id_edge_start, edges, lanes, id_prtmode, ids_sinkedge=set()):
        """
        Searches next downstream merge nodes.
        Returns array of downstream merge node IDs

        """

        ids_edge = set([id_edge_start])
        ids_mergenode = set()
        # print 'search_downstream_merges id_edge_start',id_edge_start,'is sinkedge', id_edge_start in ids_sinkedge

        if id_edge_start in ids_sinkedge:
            return None  # np.array([], dtype = np.int32)

        is_cont = True
        #n_cont = 20
        while (len(ids_edge) > 0) & is_cont:
            # print '  len(ids_edge)',len(ids_edge),ids_edge
            ids_edge_new = set()

            for id_edge in ids_edge:
                ids_edge_outgoing = edges.get_outgoing(id_edge)
                # print '  id_edge,ids_edge_outgoing',id_edge,ids_edge_outgoing
                for id_downedge in ids_edge_outgoing:
                    ids_lane = edges.ids_lanes[id_downedge]
                    if id_downedge in ids_sinkedge:
                        # print '    sinkedge id_downedge',id_downedge
                        pass

                    elif self.is_prt_only(ids_lane, lanes):
                        ids_downedge_incoming = edges.get_incoming(id_downedge)
                        # print '    id_downedge,ids_downedge_incoming',id_downedge,ids_downedge_incoming
                        is_merge = False

                        if len(ids_downedge_incoming) == 2:
                            ids_lane1, ids_lane2 = edges.ids_lanes[ids_downedge_incoming]
                            #al1 = lanes.get_accesslevel(ids_lane1, id_prtmode)
                            #al2 = lanes.get_accesslevel(ids_lane2, id_prtmode)
                            # print '      check al1,al2',al1,al2,(al1 == 2) & (al2 == 2)
                            # if (al1 == 2) & (al2 == 2):# real merges
                            if self.is_prt_only(ids_lane1, lanes) & self.is_prt_only(ids_lane2, lanes):
                                # print '      add mergenode',edges.ids_fromnode[id_downedge]
                                ids_mergenode.add(edges.ids_fromnode[id_downedge])
                                is_merge = True

                        if not is_merge:  # len(ids_downedge_incoming) == 1:
                            #ids_lane = edges.ids_lanes[ids_downedge_incoming]
                            # if lanes.get_accesslevel(ids_lane1, id_prtmode) == 2:
                            # print '    no merge detected, add edge',id_downedge
                            ids_edge_new.add(id_downedge)

                        # else:
                        #    print 'WARNING in search_downstream_merges: edge %d has %d incoming edges.'%(id_downedge, len(ids_downedge_incoming))

            ids_edge = ids_edge_new.copy()
            # print '    ids_edge_new',ids_edge_new,ids_edge,len(ids_edge)
            # is_cont = n_cont>0 #id_edge_start not in ids_edge
            #n_cont -= 1

        # if not is_cont:
        #    print '  endless!!id_edge_start,ids_edge',id_edge_start,ids_edge
        # print '  found ids_mergenode',ids_mergenode
        return np.array(list(ids_mergenode), dtype=np.int32)

    def is_prt_only(self, ids_lane, lanes):
        if len(ids_lane) > 1:
            return False
        else:
            if len(lanes.ids_modes_allow[ids_lane[0]]) > 0:
                return lanes.ids_modes_allow[ids_lane[0]][0] == self.parent.id_prtmode
            else:
                return False

    def is_platform(self, ids_lane, lanes):
        if len(ids_lane) > 1:
            if len(lanes.ids_modes_allow[ids_lane[1]]) > 0:
                # we could also chech ped on lane 0
                return lanes.ids_modes_allow[ids_lane[1]][0] == self.parent.id_prtmode
            else:
                return False
        else:
            return False


class PrtBerths(am.ArrayObjman):

    def __init__(self, ident, prtstops, **kwargs):
        # print 'PrtVehicles vtype id_default',vtypes.ids_sumo.get_id_from_index('passenger1')
        self._init_objman(ident=ident,
                          parent=prtstops,
                          name='PRT Berths',
                          info='PRT Berths.',
                          **kwargs)

        self._init_attributes()

    def _init_attributes(self):
        #vtypes = self.get_scenario().demand.vtypes
        net = self.get_scenario().net
        self.add(cm.AttrConf('length_default', 4.0,
                             groupnames=['parameters', 'options'],
                             name='Default length',
                             info='Default berth length.',
                             unit='m',
                             ))

        self.add_col(am.IdsArrayConf('ids_prtstop', self.parent,
                                     name='PRT stop ID',
                                     info='PRT stop ID',
                                     ))

        # states now dynamic, see prepare_sim
        # if hasattr(self,'states'):
        #    self.delete('states')
        # self.add_col(am.ArrayConf( 'states', default = BERTHSTATES['free'],
        #                            dtype = np.int32,
        #                            choices = BERTHSTATES,
        #                            name = 'state',
        #                            info = 'State of berth.',
        #                            ))

        self.add_col(am.ArrayConf('stoppositions', default=0.0,
                                  dtype=np.float32,
                                  name='Stop position',
                                  info='Position on edge where vehicle nose stops.',
                                  ))

    def _init_constants(self):
        self.do_not_save_attrs([
            'states', 'ids_veh',
        ])

    def prepare_sim(self, process):
        # print 'PrtBerths.prepare_sim'
        ids = self.get_ids()
        if len(ids) == 0:
            return []
        self.states = BERTHSTATES['free']*np.ones(np.max(ids)+1, dtype=np.int32)
        self.ids_veh = -1*np.ones(np.max(ids)+1, dtype=np.int32)
        return []  # berth has no update function

    def get_scenario(self):
        return self.parent.get_scenario()

    def get_prtvehicles(self):
        return self.parent.parent.prtvehicles

    def make(self, id_stop, position_from=None, position_to=None,
             n_berth=None,
             offset_firstberth=0.0, offset_stoppos=-0.0):
        stoplength = position_to-position_from
        # print 'Berths.make',id_stop,stoplength

        # TODO: let define berth number either explicitely or through stoplength

        length_berth = self.length_default.get_value()
        positions = position_from + offset_firstberth\
            + np.arange(length_berth-offset_firstberth, stoplength+length_berth, length_berth) + offset_stoppos
        n_berth = len(positions)

        # force number of berth to be pair
        if n_berth % 2 == 1:
            positions = positions[1:]
            n_berth -= 1

        ids_berth = self.add_rows(n=n_berth,
                                  stoppositions=positions,
                                  ids_prtstop=id_stop * np.ones(n_berth, dtype=np.int32),
                                  )
        return ids_berth

    def set_prtvehicles(self, prtvehicles):
        """
        Defines attributes which are linked with prtvehicles
        """
        self.add_col(am.IdsArrayConf('ids_veh_allocated', prtvehicles,
                                     name='Alloc. veh ID',
                                     info='ID of  vehicle which have allocated this berth. -1 means no allocation.',
                                     ))


class PrtPlatoonStops(am.ArrayObjman):
    """
    This class is only for backwardscompatibility with some scenarios.
    Is to be depricated
    """

    def __init__(self, ident, prtservices, **kwargs):
        pass

    def get_edges(self, ids_prtstop):
        net = self.get_scenario().net
        return net.lanes.ids_edge[net.ptstops.ids_lane[self.ids_ptstop[ids_prtstop]]]

    def get_berths(self):
        return self.berths.get_value()

    def get_scenario(self):
        return self.parent.get_scenario()

    def set_prtvehicles(self, prtvehicles):
        self.get_berths().set_prtvehicles(prtvehicles)

    def set_vehicleman(self, vehicleman):
        self.add(cm.ObjConf(vehicleman, is_child=False, groups=['_private']))

    def get_vehicleman(self):
        return self.vehicleman.get_value()


class PrtStops(am.ArrayObjman):
    def __init__(self, ident, prtservices, **kwargs):
        self._init_objman(ident=ident,
                          parent=prtservices,
                          name='PRT stops',
                          info='Contains information on PRT stops.',
                          #xmltag = ('additional','busStop','stopnames'),
                          version=0.1,
                          **kwargs)
        self._init_attributes()

    def _init_attributes(self):
        self.add(cm.ObjConf(PrtBerths('berths', self)))

        berths = self.get_berths()
        net = self.get_scenario().net

        self.add(cm.AttrConf('time_update', 0.5,
                             groupnames=['parameters'],
                             name='Update time',
                             info="Update time for station.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('time_kickout', 30.0,
                             groupnames=['parameters'],
                             name='Kickout time',
                             info="Time to kick out empty vehicles after vehicles behing have been occupied with passengers.",
                             unit='s',
                             ))
        self.add(cm.AttrConf('timeconst_flow', 0.98,
                             groupnames=['parameters'],
                             name='Flow time const',
                             info="Constant to update the moving average flow.",
                             ))

        self.add(cm.AttrConf('stoplinegap', 12.0,
                             groupnames=['parameters'],
                             name='Stopline gap',
                             unit='m',
                             info="Distance between stopline, where vehicles get started, and the end of the lane.",
                             ))

        if hasattr(self, 'time_update_man'):
            self.delete('time_update_man')
            self.delete('timehorizon')

        self.add_col(am.IdsArrayConf('ids_ptstop', net.ptstops,
                                     name='ID PT stop',
                                     info='ID of public transport stop. ',
                                     ))

        if hasattr(self, 'are_depot'):
            self.delete('are_depot')

        self.add_col(am.ArrayConf('types', default=STOPTYPES['person'],
                                  dtype=np.int32,
                                  perm='rw',
                                  choices=STOPTYPES,
                                  name='Type',
                                  info='Type of stop.',
                                  ))

        self.add_col(am.IdlistsArrayConf('ids_berth_alight', berths,
                                         #groupnames = ['_private'],
                                         name='Alight berth IDs',
                                         info="Alight berth IDs.",
                                         ))

        self.add_col(am.IdlistsArrayConf('ids_berth_board', berths,
                                         #groupnames = ['_private'],
                                         name='Board berth IDs',
                                         info="Board berth IDs.",
                                         ))

        # self.add_col(am.ArrayConf( 'inds_berth_alight_allocated', default = 0,
        #                            #groupnames = ['_private'],
        #                            dtype = np.int32,
        #                            perm = 'rw',
        #                            name = 'Ind aberth lastalloc',
        #                            info = 'Berth index of last allocated berth in alight zone.',
        #                            ))

        # self.add_col(am.ArrayConf( 'inds_berth_board_allocated', default = 0,
        #                            #groupnames = ['_private'],
        #                            dtype = np.int32,
        #                            perm = 'rw',
        #                            name = 'Ind bberth lastalloc',
        #                            info = 'Berth index of last allocated berth in boarding zone.',
        #                            ))

    def _init_constants(self):
        self.do_not_save_attrs([
            'ids_stop_to_ids_edge_sumo', 'ids_stop_to_ids_edge',
            'stoplines', 'ids_stop_to_ids_acceledge_sumo',
            'id_edge_sumo_to_id_stop', 'inds_berth_alight_allocated',
            'inds_berth_board_allocated', 'ids_vehs_alight_allocated',
            'ids_vehs_board_allocated', 'times_plat_accumulate',
            'ids_vehs_sumo_prev', 'ids_vehs',
            'ids_vehs_toallocate', 'times_lastboard',
            'numbers_veh', 'numbers_person_wait',
            'flows_person', 'ids_veh_lead',
            'ids_veh_lastdep', 'ids_vehs_prog',
            'ids_persons_sumo_prev', 'ids_persons_sumo_boarded',
            'waittimes_persons', 'waittimes_tot',
            'id_person_to_origs_dests',
            'ids_detectoredge_decompr'
        ])

    def get_edges(self, ids_prtstop):
        net = self.get_scenario().net
        return net.lanes.ids_edge[net.ptstops.ids_lane[self.ids_ptstop[ids_prtstop]]]

    def get_berths(self):
        return self.berths.get_value()

    def get_scenario(self):
        return self.parent.get_scenario()

    def set_prtvehicles(self, prtvehicles):
        self.get_berths().set_prtvehicles(prtvehicles)

    def set_vehicleman(self, vehicleman):
        self.add(cm.ObjConf(vehicleman, is_child=False, groups=['_private']))

    def get_vehicleman(self):
        return self.vehicleman.get_value()

    def get_closest(self, coords):
        """
        Returns the closest prt stop for each coord in coords vector.
        """
        net = self.get_scenario().net
        ptstops = net.ptstops
        lanes = net.lanes
        n = len(coords)
        # print 'get_closest',n

        #ids_stop = self.get_ids()

        ids_prtstop = self.get_ids()
        ids_ptstop = self.ids_ptstop[ids_prtstop]
        coords_stop = ptstops.centroids[ids_ptstop]
        ids_edge_stop = net.lanes.ids_edge[ptstops.ids_lane[ids_ptstop]]

        inds_closest = np.zeros(n, dtype=np.int32)

        i = 0
        for coord in coords:
            ind_closest = np.argmin(np.sum((coord-coords_stop)**2, 1))
            inds_closest[i] = ind_closest
            i += 1

        ids_prtstop_closest = ids_prtstop[inds_closest]
        ids_edge_closest = ids_edge_stop[inds_closest]

        return ids_prtstop_closest, ids_edge_closest

    def get_waitpositions(self, ids, is_alight=False, offset=-0.0):
        """
        Assign randomly a wait-position for each stop in ids

        offset is wait position relative to the vehicle nose.
        """
        # print 'get_waitpositions min(ids),max(ids)',min(ids),is_alight,max(ids),offset
        positions = np.zeros(len(ids), dtype=np.float32)
        randint = random.randint
        if is_alight:
            ids_berths = self.ids_berth_alight[ids]
        else:
            ids_berths = self.ids_berth_board[ids]

        stoppositions = self.get_berths().stoppositions
        # print '  ids_berths',ids_berths
        i = 0
        for id_stop, ids_berth in zip(ids, ids_berths):
            #ids_berth = ids_berths[id_stop]
            ind_berth = randint(0, len(ids_berth)-1)

            positions[i] = stoppositions[ids_berth[ind_berth]]
            # print '  id_stop,ids_berth,posiions',id_stop,ids_berth,stoppositions[ids_berth[ind_berth]]
            i += 1
            #positions[i] = stoppositions[ids_berth[randint(0,len(ids_berth))]]
        # for id_stop , pos in zip(ids, positions):
        #    print '  id_stop %d, is_alight = %s, pos %.2fm'%(id_stop, is_alight ,pos)

        return positions+offset

    def prepare_sim(self, process):
        print 'PrtStops.prepare_sim'
        net = self.get_scenario().net
        ptstops = net.ptstops
        ids_edge_sumo = net.edges.ids_sumo

        berths = self.get_berths()
        lanes = net.lanes
        ids_edge_sumo = net.edges.ids_sumo
        ids = self.get_ids()

        if len(ids) == 0:  # np PRT stops in network
            return []

        get_outgoing = net.edges.get_outgoing

        # station management
        self.ids_stop_to_ids_edge_sumo = np.zeros(np.max(ids)+1, dtype=np.object)
        self.ids_stop_to_ids_edge = np.zeros(np.max(ids)+1, dtype=np.int32)

        ids_stopedge = lanes.ids_edge[ptstops.ids_lane[self.ids_ptstop[ids]]]
        # print '  ids,self.stoplines[ids]',ids,self.stoplines[ids]
        self.ids_stop_to_ids_edge_sumo[ids] = ids_edge_sumo[ids_stopedge]
        self.ids_stop_to_ids_edge[ids] = ids_stopedge

        # Determine stopline position  where vehicles actually start
        # running off the station

        # final stop at one meter before end of stopedge
        self.stoplines = np.zeros(np.max(ids)+1, dtype=np.float32)
        stoplinegap = self.stoplinegap.get_value()
        #self.stoplines[ids] = net.edges.lengths[ids_stopedge]-12.0
        #stoplengths = net.edges.lengths[ids_stopedge]
        for id_stop, ids_berth_board, length_stopedge in zip(ids, self.ids_berth_board[ids], net.edges.lengths[ids_stopedge]):
            lastberthstoppos = berths.stoppositions[ids_berth_board][-1]
            if (length_stopedge-lastberthstoppos) > stoplinegap+1:
                self.stoplines[id_stop] = length_stopedge-stoplinegap
                print '  LI:id_stop', id_stop, 'length_stopedge', length_stopedge, 'stopline', self.stoplines[id_stop]

            elif length_stopedge > lastberthstoppos:
                self.stoplines[id_stop] = 0.5*(length_stopedge+lastberthstoppos)
                print '  AV:id_stop', id_stop, 'length_stopedge', length_stopedge, 'stopline', self.stoplines[id_stop]

        self.ids_stop_to_ids_acceledge_sumo = np.zeros(np.max(ids)+1, dtype=np.object)
        for id_stop, id_stopedge in zip(ids, ids_stopedge):
            self.ids_stop_to_ids_acceledge_sumo[id_stop] = ids_edge_sumo[get_outgoing(id_stopedge)[0]]

        self.id_edge_sumo_to_id_stop = {}
        for id_stop, id_edge_sumo in zip(ids, self.ids_stop_to_ids_edge_sumo[ids]):
            self.id_edge_sumo_to_id_stop[id_edge_sumo] = id_stop

        self.inds_berth_alight_allocated = -1*np.ones(np.max(ids)+1, dtype=np.int32)
        self.inds_berth_board_allocated = -1*np.ones(np.max(ids)+1, dtype=np.int32)
        self.ids_vehs_alight_allocated = np.zeros(np.max(ids)+1, dtype=np.object)
        self.ids_vehs_board_allocated = np.zeros(np.max(ids)+1, dtype=np.object)

        # time when last platoon vehicle accumulation started
        # -1 means no platoon accumulation takes place
        self.times_plat_accumulate = -1*np.ones(np.max(ids)+1, dtype=np.int32)

        self.ids_vehs_sumo_prev = np.zeros(np.max(ids)+1, dtype=np.object)
        self.ids_vehs = np.zeros(np.max(ids)+1, dtype=np.object)
        self.ids_vehs_toallocate = np.zeros(np.max(ids)+1, dtype=np.object)
        #
        self.times_lastboard = 10**4*np.ones(np.max(ids)+1, dtype=np.int32)

        # for vehicle management
        self.numbers_veh = np.zeros(np.max(ids)+1, dtype=np.int32)
        self.numbers_person_wait = np.zeros(np.max(ids)+1, dtype=np.int32)
        self.flows_person = np.zeros(np.max(ids)+1, dtype=np.float32)
        self.ids_veh_lead = -1*np.ones(np.max(ids)+1, dtype=np.int32)
        #self.ids_veh_lastdep = -1*np.ones(np.max(ids)+1,dtype = np.int32)
        self.ids_vehs_prog = np.zeros(np.max(ids)+1, dtype=np.object)

        # person management
        self.ids_persons_sumo_prev = np.zeros(np.max(ids)+1, dtype=np.object)
        #self.ids_persons_sumo_boarded = np.zeros(np.max(ids)+1,dtype = np.object)
        self.waittimes_persons = np.zeros(np.max(ids)+1, dtype=np.object)
        self.waittimes_tot = np.zeros(np.max(ids)+1, dtype=np.float32)

        virtualpop = self.get_scenario().demand.virtualpop

        ids_persons = virtualpop.get_ids()
        stagelists = virtualpop.get_plans().stagelists
        prttransits = virtualpop.get_plans().get_stagetable('prttransits')
        id_person_to_origs_dests = {}

        # create map from person id to various destination information
        # TODO: needs to be improved for trip chains, move to prtservices
        # idea: whu not scan prttransits?
        for id_person, stagelist in zip(ids_persons, stagelists[virtualpop.ids_plan[ids_persons]]):
            # print '  check dests of id_person',id_person
            for stages, id_stage in stagelist:
                if stages.ident == 'prttransits':
                    id_fromedge_sumo = ids_edge_sumo[stages.ids_fromedge[id_stage]]
                    id_toedge_sumo = ids_edge_sumo[stages.ids_toedge[id_stage]]
                    data_orig_dest = (self.id_edge_sumo_to_id_stop[id_fromedge_sumo],
                                      self.id_edge_sumo_to_id_stop[id_toedge_sumo],
                                      id_fromedge_sumo,
                                      id_toedge_sumo)

                    id_person_sumo = virtualpop.get_id_sumo_from_id(id_person)
                    if id_person_to_origs_dests.has_key(id_person_sumo):
                        id_person_to_origs_dests[id_person_sumo].append(data_orig_dest)
                    else:
                        id_person_to_origs_dests[id_person_sumo] = [data_orig_dest]
            # print '  prtdests = ',id_person_to_origs_dests[id_person_sumo]

        # print '   id_person_to_origs_dests=\n',id_person_to_origs_dests
        self.id_person_to_origs_dests = id_person_to_origs_dests

        # this is only used for crazy person stage detection
        # angles_stop =

        # various initianilizations
        for id_stop, id_edge_sumo in zip(ids, self.ids_stop_to_ids_edge_sumo[ids]):
            # set allocation index to last possible berth
            self.inds_berth_alight_allocated[id_stop] = len(self.ids_berth_alight[id_stop])
            self.inds_berth_board_allocated[id_stop] = len(self.ids_berth_board[id_stop])

            self.ids_vehs_alight_allocated[id_stop] = []
            self.ids_vehs_board_allocated[id_stop] = []

            self.ids_vehs_sumo_prev[id_stop] = set([])
            self.ids_persons_sumo_prev[id_stop] = set([])
            #self.ids_persons_sumo_boarded [id_stop] = []
            self.waittimes_persons[id_stop] = {}
            self.ids_vehs[id_stop] = []
            self.ids_vehs_toallocate[id_stop] = []

            self.ids_vehs_prog[id_stop] = []

        # decompressor detector edge set
        self.ids_detectoredge_decompr = set(self.parent.decompressors.ids_detectoredge.get_value())

        #    traci.edge.subscribe(id_edge_sumo, [traci.constants.VAR_ARRIVED_VEHICLES_IDS])
        updatedata_berth = berths.prepare_sim(process)

        return [(self.time_update.get_value(), self.process_step),
                ]+updatedata_berth

    def process_step(self, process):
        simtime = process.simtime
        print 79*'_'
        print 'PrtStops.process_step at', simtime
        net = self.get_scenario().net
        ptstops = net.ptstops
        berths = self.get_berths()
        lanes = net.lanes
        ids_edge_sumo = net.edges.ids_sumo
        vehicles = self.parent.prtvehicles
        virtualpop = self.get_scenario().demand.virtualpop
        ids = self.get_ids()

        for id_stop, id_edge_sumo, ids_veh_sumo_prev, ids_person_sumo_prev in\
            zip(ids, self.ids_stop_to_ids_edge_sumo[ids],
                self.ids_vehs_sumo_prev[ids],
                self.ids_persons_sumo_prev[ids]):
            print '  '+60*'.'
            print '  process id_stop,id_edge_sumo', id_stop, id_edge_sumo
            if 0:  # id_stop==1:

                # print '    ids_berth_alight',self.ids_berth_alight[id_stop]
                # print '    ids_berth_board',self.ids_berth_board[id_stop]
                print '    ids_vehs', self.ids_vehs[id_stop]
                print '    ids_vehs_toallocate', self.ids_vehs_toallocate[id_stop]
                print '    inds_berth_alight_allocated', self.inds_berth_alight_allocated[id_stop]
                print '    ids_vehs_alight_allocated', self.ids_vehs_alight_allocated[id_stop]
                print '    ids_vehs_board_allocated', self.ids_vehs_board_allocated[id_stop]
                # print '    id_veh_lead prt.%d'%self.ids_veh_lead[id_stop]
                # print '    ids_vehs_prog',self.ids_vehs_prog[id_stop]

                print '    iiinds_berth_alight_allocated', self.inds_berth_alight_allocated[id_stop]
                print '    iiinds_berth_board_allocated', self.inds_berth_board_allocated[id_stop]
                # print '    numbers_person_wait',self.numbers_person_wait[id_stop]

                # print '    flow_person',self.flows_person[id_stop]
                # print '    waittimes_persons',self.waittimes_persons[id_stop]

                # print '    waittimes_tot',self.waittimes_tot[id_stop]

                # no longer print '    ids_persons_sumo_boarded',self.ids_persons_sumo_boarded[id_stop]
                # print '    times_lastboard',self.times_lastboard[id_stop]

            if 0:
                for id_veh_sumo in self.ids_vehs_sumo_prev[id_stop]:
                    print '    stopstate ', id_veh_sumo, bin(traci.vehicle.getStopState(id_veh_sumo))[2:], traci.vehicle.getRoute(id_veh_sumo)

            if 0:
                self.get_berthqueues(id_stop)

            # check for new vehicle arrivals/departures
            ids_veh_sumo = set(traci.edge.getLastStepVehicleIDs(id_edge_sumo))
            # print '    ids_veh_sumo_prev=',ids_veh_sumo_prev
            # print '    ids_veh_sumo=',ids_veh_sumo

            if ids_veh_sumo_prev != ids_veh_sumo:
                ids_veh_entered = vehicles.get_ids_from_ids_sumo(get_entered_vehs(ids_veh_sumo, ids_veh_sumo_prev))
                ids_veh_left = vehicles.get_ids_from_ids_sumo(list(ids_veh_sumo_prev.difference(ids_veh_sumo)))
                for id_veh in ids_veh_entered:
                    self.enter(id_stop, id_veh)

                for id_veh in ids_veh_left:
                    self.exit(id_stop, id_veh)
                self.ids_vehs_sumo_prev[id_stop] = ids_veh_sumo
                # print '    ids_veh_sumo_entered',ids_veh_sumo_entered
                # print '    ids_veh_sumo_left',ids_veh_sumo_left

            # check whether allocated vehicles arrived at alighting berths
            ids_veh_remove = []
            for id_veh in self.ids_vehs_alight_allocated[id_stop]:
                # TODO: here we could also check vehicle position
                # print '   isStopped',vehicles.get_id_sumo(id_veh),traci.vehicle.isStopped(vehicles.get_id_sumo(id_veh))
                if traci.vehicle.isStopped(vehicles.get_id_sumo(id_veh)):
                    ids_veh_remove.append(id_veh)
                    id_berth_alight = vehicles.ids_berth[id_veh]
                    berths.ids_veh[id_berth_alight] = id_veh
                    berths.states[id_berth_alight] = BERTHSTATES['alighting']
                    vehicles.alight(id_veh)

            for id_veh in ids_veh_remove:
                self.ids_vehs_alight_allocated[id_stop].remove(id_veh)

            # check whether we can move vehicles from alighting to
            # boarding berths

            # TODO: here we must check if berth in boarding zone are free
            # AND if they are occupied with empty vehicles, those
            # vehicles need to be kicked out...but only in case
            # new vehicles are waiting to be allocated

            ids_berth_alight = self.ids_berth_alight[id_stop][::-1]
            ids_berth_board = self.ids_berth_board[id_stop][::-1]

            if True:  # len(self.ids_vehs_alight_allocated[id_stop])==0:
                # all vehicles did arrive in alight position

                # identify berth and vehicles ready to forward to boarding
                ids_veh_forward = []
                ids_berth_forward = []
                has_not_arrived = False
                for id_berth_alight, id_veh in zip(
                    ids_berth_alight,
                    berths.ids_veh[ids_berth_alight],
                ):
                    # print '    check alight->board  for veh prt.%d'%id_veh,'at berth',id_berth_alight,berths.states[id_berth_alight], berths.states[id_berth_alight]==BERTHSTATES['free']

                    if id_veh >= 0:  # is there a waiting vehicle

                        # make sure, that there are no vehicles which are
                        # allocated for alight in front of the present vehicle
                        ind = self.ids_vehs[id_stop].index(id_veh)
                        # print '    check alloc veh in front',self.ids_vehs[id_stop][:ind],self.ids_vehs_alight_allocated[id_stop]

                        if not set(self.ids_vehs[id_stop][:ind]).isdisjoint(self.ids_vehs_alight_allocated[id_stop]):
                            # print '      allocated veh in front! Stop forwarding'
                            break

                        elif vehicles.is_completed_alighting(id_veh):
                            ids_veh_forward.append(id_veh)
                            ids_berth_forward.append(id_berth_alight)
                        else:
                            # print '    vehicle has prt.%d not finished alighting...'%id_veh
                            # stop allocating berth in board zone
                            # to prevent allocations behind non-allocated vehicles
                            break

                n_veh_alloc = len(ids_veh_forward)

                if n_veh_alloc > 0:

                    if self.inds_berth_board_allocated[id_stop] > n_veh_alloc:
                        queues = self.get_berthqueues(id_stop)
                    else:
                        queues = None

                    # print '  found %d veh at id_stop=%d to berth alloc with board alloc index %d'%(n_veh_alloc,id_stop, self.inds_berth_board_allocated [id_stop])
                    # if queues is not None:
                    # print '    queues',queues
                    for id_berth_alight, id_veh in zip(
                        ids_berth_forward,
                        ids_veh_forward,
                    ):

                        id_berth_board = self.allocate_board(id_stop, n_veh_alloc, queues)
                        # print '    try allocate id_veh=prt.%d for berth id_berth_board=%d'%(id_veh,id_berth_board)
                        if id_berth_board >= 0:
                            # print '     send vehicle id_veh %d to id_berth_board %d'%(id_veh,id_berth_board)#,berths.stoppositions[id_berth_board]
                            n_veh_alloc -= 1
                            berths.ids_veh[id_berth_alight] = -1

                            berths.states[id_berth_alight] = BERTHSTATES['free']

                            vehicles.control_stop_board(id_veh, id_stop, id_berth_board,
                                                        id_edge_sumo=self.ids_stop_to_ids_edge_sumo[id_stop],
                                                        position=berths.stoppositions[id_berth_board],
                                                        )
                            self.ids_vehs_board_allocated[id_stop].append(id_veh)

            # if all allocated vehicles found their berth and all berths are free, then
            # reset  alight allocation index
            # print '  check for reset of alight allocation index',self.inds_berth_alight_allocated[id_stop], len(self.ids_vehs_alight_allocated[id_stop])==0, np.all(berths.states[ids_berth_alight]==BERTHSTATES['free'])

            if (len(self.ids_vehs_alight_allocated[id_stop]) == 0) & np.all(berths.states[ids_berth_alight] == BERTHSTATES['free']):

                # print '    reset inds_berth_alight_allocated',self.inds_berth_alight_allocated[id_stop],'->',len(self.ids_berth_alight[id_stop])
                self.inds_berth_alight_allocated[id_stop] = len(self.ids_berth_alight[id_stop])

                # try to allocate unallocated vehicles
                ids_veh_remove = []
                for id_veh in self.ids_vehs_toallocate[id_stop]:
                    id_berth = self.allocate_alight(id_stop)
                    if id_berth < 0:
                        # allocation failed
                        # print '  do nothing, vehicle %d continues to wait for allocation'%id_veh
                        pass
                    else:
                        # command vehicle to go to berth for alighting
                        # print '     send waiting vehicle id_veh %d to id_berth_alight %d'%(id_veh,id_berth)#,berths.stoppositions[id_berth]
                        self.parent.prtvehicles.control_stop_alight(id_veh, id_stop, id_berth,
                                                                    id_edge_sumo=self.ids_stop_to_ids_edge_sumo[id_stop],
                                                                    position=self.get_berths().stoppositions[id_berth],
                                                                    )
                        self.ids_vehs_alight_allocated[id_stop].append(id_veh)
                        ids_veh_remove.append(id_veh)

                for id_veh in ids_veh_remove:
                    self.ids_vehs_toallocate[id_stop].remove(id_veh)

            # else:
            #    if len(self.ids_vehs_toallocate[id_stop])>0:
            #        print '    HUSTON: there are vehs to allocate but cannot reset berthind'
            #        print '      ids_vehs_toallocate',vehicles.ids_sumo[self.ids_vehs_toallocate[id_stop]]
            # print '      ids_vehs_toallocate',vehicles.ids_sumo[self.ids_vehs_toallocate[id_stop]]
            # print '      self.ids_vehs_alight_allocated[id_stop]',self.ids_vehs_alight_allocated[id_stop]
            # print '      berths.ids_veh',berths.ids_veh[ids_berth_alight]
            # print '      berths.states[ids_berth_alight]',berths.states[ids_berth_alight]

            # check whether allocated vehicles arrived at boarding berths
            ids_veh_remove = []
            for id_veh in self.ids_vehs_board_allocated[id_stop]:
                # TODO: here we could also check vehicle position
                if traci.vehicle.isStopped(vehicles.get_id_sumo(id_veh)):
                    ids_veh_remove.append(id_veh)
                    id_berth_board = vehicles.ids_berth[id_veh]
                    berths.ids_veh[id_berth_board] = id_veh
                    berths.states[id_berth_board] = BERTHSTATES['boarding']
                    vehicles.board(id_veh,
                                   id_edge_sumo=self.ids_stop_to_ids_edge_sumo[id_stop])

                    self.parent.vehicleman.indicate_trip_empty(id_veh, id_stop, simtime)
                    # vehicle could become potentially the lead vehicle
                    self.try_set_leadveh(id_stop, id_veh)

            for id_veh in ids_veh_remove:
                self.ids_vehs_board_allocated[id_stop].remove(id_veh)

            # if all allocated vehicles for board area
            # found their berth and all berths are free, then
            # reset  allocation index
            # print '  check for reset of board berth', (self.inds_berth_board_allocated[id_stop] == 0),(len(self.ids_vehs_board_allocated[id_stop])==0), np.all(berths.states[ids_berth_board]==BERTHSTATES['free'])
            if (self.inds_berth_board_allocated[id_stop] == 0) & (len(self.ids_vehs_board_allocated[id_stop]) == 0):

                if np.all(berths.states[ids_berth_board] == BERTHSTATES['free']):
                    # print '    reset inds_berth_board_allocated to',len(self.ids_berth_board[id_stop])
                    self.inds_berth_board_allocated[id_stop] = len(self.ids_berth_board[id_stop])
                # else:
                #    print '    unfree boarding zone',berths.states[ids_berth_board]
            else:
                # print '   no reset: ids_vehs_board_allocated',self.ids_vehs_board_allocated[id_stop]
                # print '      berths.ids_veh',berths.ids_veh[ids_berth_board]
                # print '      berths.states[ids_berth_alight]',berths.states[ids_berth_board]
                pass
            # check for new person entering/left the station edge
            ids_person_sumo = set(traci.edge.getLastStepPersonIDs(id_edge_sumo))

            n_enter = 0
            if ids_person_sumo_prev != ids_person_sumo:

                if 0:
                    print '  change\n  id_person_sumo', ids_person_sumo
                    print '  ids_person_sumo_prev', ids_person_sumo_prev
                # print '  dir(traci.person)',dir(traci.person)
                # for id_person_sumo in ids_person_sumo:
                #    print '  id_person_sumo',id_person_sumo,traci.person.getRoadID(id_person_sumo),traci.person.getVehicle(id_person_sumo)

                # deal with persons who left the edge
                # NEW: this is done later when loaded vehicles are investigated

                #ids_person_sumo_left = ids_person_sumo_prev.difference(ids_person_sumo)
                # print '  ids_person_sumo_left',ids_person_sumo_left
                # for id_person_sumo in ids_person_sumo_left:
                #    print '  id_person_sumo_left pers',id_person_sumo,id_edge_sumo,traci.person.getRoadID(id_person_sumo),traci.person.getVehicle(id_person_sumo)
                #    #print '  ids_person_sumo',ids_person_sumo
                #    # tricky: if the person who left the edge id_edge_sumo
                #    # shows still id_edge_sumo then this person is in a vehicle
                #    if traci.person.getRoadID(id_person_sumo) == id_edge_sumo:
                #        #print '  person boarded: pers',id_person_sumo,traci.person.getLanePosition(id_person_sumo)
                #        self.ids_persons_sumo_boarded[id_stop].append(id_person_sumo)
                #        self.waittimes_tot[id_stop] -= simtime - self.waittimes_persons[id_stop][id_person_sumo]
                #        del self.waittimes_persons[id_stop][id_person_sumo]
                #        self.numbers_person_wait[id_stop] -= 1

                # deal with persons who entered the edge
                ids_person_sumo_entered = ids_person_sumo.difference(ids_person_sumo_prev)
                for id_person_sumo in ids_person_sumo_entered:
                    # print '  entered id_person_sumo',id_person_sumo,traci.person.getRoadID(id_person_sumo)
                    if self.id_person_to_origs_dests.has_key(id_person_sumo):
                        id_edge_sumo_dests = self.id_person_to_origs_dests[id_person_sumo]
                        # check if person still has a PRT trip

                        if len(id_edge_sumo_dests) > 0:
                            # check if next trip has origin edge equal to edge of this stop
                            if id_edge_sumo_dests[0][2] == id_edge_sumo:
                                # print '  add to waittimes_persons',id_person_sumo
                                self.waittimes_persons[id_stop][id_person_sumo] = simtime
                                n_enter += 1

                                # communicate person entry to vehman
                                self.parent.vehicleman.note_person_entered(
                                    id_stop, id_person_sumo, id_edge_sumo_dests[0][1])

                            # else:
                            #    print 'WARNING: person %s starts with % insted of %s.'%(id_person_sumo,id_edge_sumo_dests[0][2],id_edge_sumo)

                self.numbers_person_wait[id_stop] += n_enter
                self.ids_persons_sumo_prev[id_stop] = ids_person_sumo

            self.waittimes_tot += self.numbers_person_wait*self.time_update.get_value()

            timeconst_flow = self.timeconst_flow.get_value()
            self.flows_person[id_stop] = timeconst_flow*self.flows_person[id_stop] + \
                (1.0-timeconst_flow)*float(n_enter)/self.time_update.get_value()

            if 0:
                for id_person_sumo in ids_person_sumo_prev:
                    print '    ids_person_sumo=%s pos = %.2f ' % (id_person_sumo, traci.person.getLanePosition(id_person_sumo))
                # nomore print '    ids_persons_sumo_boarded',self.ids_persons_sumo_boarded[id_stop]

            # check if boarding is completed in load area,
            # starting with last vehicle
            ids_berth_board = self.ids_berth_board[id_stop][::-1]
            for id_berth_board, id_veh in zip(
                ids_berth_board,
                berths.ids_veh[ids_berth_board],
            ):
                if id_veh >= 0:  # is there a waiting vehicle
                    id_veh_sumo = vehicles.get_veh_if_completed_boarding(id_veh)
                    if id_veh_sumo:
                        id_person_sumo = self.init_trip_occupied(id_stop,
                                                                 id_berth_board,
                                                                 id_veh,
                                                                 id_veh_sumo,
                                                                 simtime)
                        if id_person_sumo is not None:
                            # do some statistics here
                            self.waittimes_tot[id_stop] -= simtime - self.waittimes_persons[id_stop][id_person_sumo]
                            del self.waittimes_persons[id_stop][id_person_sumo]
                            self.numbers_person_wait[id_stop] -= 1

            # check if there are passengers in the vehicles which wait for
            # alight allocate
            # TODO: can be replaced by a single instruction
            n_pax = 0
            for id_veh in self.ids_vehs_alight_allocated[id_stop]+self.ids_vehs_toallocate[id_stop]:
                if vehicles.states[id_veh] == VEHICLESTATES['occupiedtrip']:
                    n_pax += 1
            # print '    n_pax' ,n_pax
            # check whether to foreward vehicles in boarding berth

            # no foreward if all berth are free occupied vehicles
            if np.all(berths.states[ids_berth_board] == BERTHSTATES['free']):
                # print '    foreward all occupied id_stop,ids_berth_board',id_stop,ids_berth_board
                #self.foreward_boardzone(id_stop, ids_berth_board)
                self.times_lastboard[id_stop] = 10**4  # reset clock if all are free

            # foreward if there are passengers in unallocated vehicles
            elif (self.numbers_person_wait[id_stop] == 0) & (n_pax > 0):
                # passengers arriving, no persons waiting
                # kick out immediately
                self.foreward_boardzone(id_stop, ids_berth_board, simtime)

            # elif (self.numbers_person_wait[id_stop]>0) & (n_pax>0) & (self.times_lastboard[id_stop] == 10**4):
            elif ((self.numbers_person_wait[id_stop] > 0) | (n_pax > 0)) & (self.times_lastboard[id_stop] == 10**4):
                # passengers arriving but still persons boarding
                # reset kick-out counter
                self.times_lastboard[id_stop] = simtime

            elif simtime - self.times_lastboard[id_stop] > self.time_kickout.get_value():
                # print '  call foreward_boardzone timeout',process.simtime,self.times_lastboard[id_stop],process.simtime - self.times_lastboard[id_stop]
                self.foreward_boardzone(id_stop, ids_berth_board, simtime)

            # check whether a programmed vehicle can be started
            if self.types[id_stop] == STOPTYPES['group']:
                self.start_vehicles_platoon(id_stop, process)
            else:
                self.start_vehicles(id_stop, process)

    def start_vehicles(self, id_stop, process):
        print 'start_vehicles=\n', self.ids_vehs_prog[id_stop]
        i = 0
        vehicles = self.parent.prtvehicles
        ids_vehs_prog = self.ids_vehs_prog[id_stop]
        for time_start, id_veh, id_stop_target, is_started in ids_vehs_prog:
            if process.simtime > time_start:
                if not is_started:
                    id_veh_sumo = vehicles.get_id_sumo(id_veh)
                    if traci.vehicle.isStopped(id_veh_sumo):
                        # print '  route prt.%d from stop  %d to %d'%(id_veh,id_stop, id_stop_target)
                        route, duration = self.route_stop_to_stop(id_stop, id_stop_target)
                        # print '  route prt.%d from %d to %d'%(id_veh,self.ids_stop_to_ids_edge[id_stop],self.ids_stop_to_ids_edge[id_stop_target]),route
                        vehicles.reschedule_trip(id_veh,
                                                 route_sumo=self.get_scenario().net.edges.ids_sumo[route]
                                                 )
                        ids_vehs_prog[i][3] = True
                        #self.parent.prtvehicles.control_slow_down( id_veh, speed = 6.0/3.6)
                        # print '  limit MaxSpeed',id_veh_sumo
                        traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)
            i += 1

    def route_stop_to_stop(self, id_stop_from, id_stop_to):
        # route
        return self.parent.get_route(self.ids_stop_to_ids_edge[id_stop_from],
                                     self.ids_stop_to_ids_edge[id_stop_to]
                                     )

    def start_vehicles_platoon(self, id_stop, process, timeout_platoon=40, n_platoon_max=8):
        # print 'start_vehicles_platoon id_stop, times_plat_accumulate',id_stop,self.times_plat_accumulate[id_stop]
        # print '  ids_vehs_prog=\n',self.ids_vehs_prog[id_stop]

        if self.times_plat_accumulate[id_stop] < 0:
            # print '  accumulation has not even started'
            return

        vehicles = self.parent.prtvehicles
        ids_vehs_prog = self.ids_vehs_prog[id_stop]
        inds_platoon = []
        i = 0  # len(ids_vehs_prog)
        id_veh_nextplatoon = -1
        for time_start, id_veh, id_stop_target, is_started in ids_vehs_prog:  # [::-1]:

            if not is_started:
                if len(inds_platoon) == 0:
                    # check if first vehicle in platoon is stopped
                    if traci.vehicle.isStopped(vehicles.get_id_sumo(id_veh)):
                        inds_platoon.append(i)
                    else:
                        break
                else:
                    # append to platoon
                    inds_platoon.append(i)

            i += 1

        # print '  trigger platoon?', inds_platoon,len(inds_platoon),n_platoon_max,process.simtime - self.times_plat_accumulate[id_stop],timeout_platoon
        if len(inds_platoon) == 0:
            return

        if (process.simtime - self.times_plat_accumulate[id_stop] > timeout_platoon)\
                | (len(inds_platoon) >= n_platoon_max):

            # platoon release triggered
            self._trigger_platoon(id_stop, inds_platoon)

    def get_decompressoredge(self, route):
        """
        Returns first occurance of a decompressoredge edge route from 
        id_stop to id_stop_target.
        Returns -1 if nor decompressoredge is found.
        """
        # print 'get_decompressoredge route',route
        # print '  self.ids_detectoredge_decompr',self.ids_detectoredge_decompr
        set_detectoredge = self.ids_detectoredge_decompr.intersection(set(route))
        # print '  set_detectoredge',set_detectoredge
        if len(set_detectoredge) == 0:
            return -1
        else:
            #ids_detectoredge = list(set_detectoredge)
            id_detectoredge_first = -1
            ind_min = 10**8
            for id_detectoredge in set_detectoredge:
                if route.index(id_detectoredge) < ind_min:
                    id_detectoredge_first = id_detectoredge
            return id_detectoredge_first

    def _trigger_platoon(self, id_stop, inds_platoon):
        # print 'trigger_platoon inds_platoon',inds_platoon
        ids_vehs_prog = self.ids_vehs_prog[id_stop]
        vehicles = self.parent.prtvehicles

        self.times_plat_accumulate[id_stop] = -1

        time_start_pre, id_veh_pre, id_stop_target_pre, is_prog_pre = ids_vehs_prog[inds_platoon[0]]
        vehicles.reschedule_trip(id_veh_pre, self.ids_stop_to_ids_edge_sumo[id_stop_target_pre])
        #self.parent.prtvehicles.control_slow_down( id_veh_pre, speed = 6.0/3.6)
        id_veh_pre_sumo = vehicles.get_id_sumo(id_veh_pre)

        traci.vehicle.setMaxSpeed(id_veh_pre_sumo, 6.0/3.6)

        ids_vehs_prog[inds_platoon[0]][3] = True
        # one vehicle platoon, no followers
        if len(inds_platoon) > 1:
            # try to concatenate followers

            # search first detectoredge in route
            route_pre, traveltime = self.route_stop_to_stop(id_stop, id_stop_target_pre)
            id_detectoredge_pre = self.get_decompressoredge(route_pre)

            for i in xrange(1, len(inds_platoon)):
                #time_start_pre, id_veh_pre, id_stop_target_pre, is_prog_pre  = ids_vehs_prog[inds_platoon[i-1]]
                time_start, id_veh, id_stop_target, is_prog = ids_vehs_prog[inds_platoon[i]]
                route, traveltime = self.route_stop_to_stop(id_stop, id_stop_target)
                id_detectoredge = self.get_decompressoredge(route)
                id_veh_sumo = vehicles.get_id_sumo(id_veh)

                # print '    check prt.%d'%ids_vehs_prog[inds_platoon[i]][1],'with leader prt.%d'%ids_vehs_prog[inds_platoon[i-1]][1],'same target',id_stop_target == id_stop_target_pre

                # print '        route',route,'route_pre',route_pre
                # print '        id_detectoredge',id_detectoredge,'id_detectoredge_pre',id_detectoredge_pre
                # if id_stop_target == id_stop_target_pre:

                # for platooning either same decompressor or same target
                if ((id_detectoredge == id_detectoredge_pre) & (id_detectoredge_pre != -1))\
                        | (id_stop_target == id_stop_target_pre):
                    # check also if leader stands immediately in from of follower

                    info = traci.vehicle.getLeader(id_veh_sumo, dist=10.0)
                    # print '      check vehicle in front info',info
                    if info is not None:
                        id_veh_pre_sumo, dist = info
                        if dist < 5.0:
                            # print '      concatenate',id_veh,'with',id_veh_pre
                            self.parent.prtvehicles.concatenate(id_veh, id_veh_pre)

                # schedule and tell that vehicle has been scheduled
                self.parent.prtvehicles.reschedule_trip(id_veh, self.ids_stop_to_ids_edge_sumo[id_stop_target])
                ids_vehs_prog[inds_platoon[i]][3] = True

                id_detectoredge_pre = id_detectoredge
                route_pre = route
                #time_start_pre = time_start
                id_veh_pre = id_veh
                id_stop_target_pre = id_stop_target
                #is_prog_pre = is_prog

                # print '  limit MaxSpeed',id_veh_sumo
                traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)
                #self.parent.prtvehicles.control_slow_down( id_veh, speed = 6.0/3.6)

    def try_set_leadveh(self, id_stop, id_veh):

        if self.ids_veh_lead[id_stop] >= 0:
            # print 'try_set_leadveh leader already defined',id_stop,id_veh,self.ids_veh_lead[id_stop]
            return False
        else:
            ind_queue = self.ids_vehs[id_stop].index(id_veh)
            # print 'try_set_leadveh id_stop, id_veh,ind_queue',id_stop, 'prt.%d'%id_veh,ind_queue,len(self.ids_vehs[id_stop]),len(self.ids_vehs_prog[id_stop])

            if ind_queue == 0:  # len(self.ids_vehs[id_stop])-1:
                # print  '  id_veh is new leader because last position',self.ids_vehs[id_stop].index(id_veh)
                # print  '  ids_vehs',self.ids_vehs[id_stop]
                self.set_leadveh(id_stop, id_veh)
                return True

            elif len(self.ids_vehs_prog[id_stop]) == ind_queue:
                # print  '   id_veh is new leader because all vehicles in front are already programmed'
                self.set_leadveh(id_stop, id_veh)
                return True
            # elif len(self.ids_vehs_prog[id_stop])>0:
            #    if (self.ids_vehs[id_stop][ind_queue-1] == self.ids_vehs_prog[id_stop][-1][1]) :
            #        print  '   id_veh is new leader because next vehicle %d is programmed'%(self.ids_vehs[id_stop][ind_queue-1],)
            #        self.set_leadveh(id_stop, id_veh)
            #        return True
            #    else:
            #        return False

            else:
                return False

    def set_leadveh(self, id_stop, id_veh):
        # print 'set_leadveh id_stop=%d, prt.%d'%( id_stop,id_veh)
        self.ids_veh_lead[id_stop] = id_veh

    def program_leadveh(self, id_stop, id_veh, id_stop_target, time_start):
        # print 'program_leadveh prt.%d  from id_stop %d to id_stop_target %d at %d'%(id_veh, id_stop,id_stop_target,time_start),'check leader',id_veh == self.ids_veh_lead[id_stop]

        # check also if occupied in the meanwhile?? need to know emptytrip or not...
        if id_veh == self.ids_veh_lead[id_stop]:
            # check in vehman:if self.parent.prtvehicles.is_still_empty(id_veh):

            if self.parent.prtvehicles.states[id_veh] == VEHICLESTATES['boarding']:
                id_berth_board = self.parent.prtvehicles.ids_berth[id_veh]
                self.init_trip_empty(id_stop, id_berth_board, id_veh, time_start, is_ask_vehman=False)

            self.ids_veh_lead[id_stop] = -1

            # format for programmed vehicle list:
            # [time_start, id_veh, id_stop_target,is_started]
            self.ids_vehs_prog[id_stop].append([time_start, id_veh, id_stop_target, False])

            if self.types[id_stop] == STOPTYPES['group']:
                # in platoon mode...
                # set a stop for this vehicle if there are only started, programmed
                # vehicles in front , or no vehicle

                # check if this vehicle needs to stop in front of the stopline
                # in order to hold up other vehicles in the platoon
                # print '    check stopline',len(self.ids_vehs_prog[id_stop])
                # print '     ids_vehs_prog ',self.ids_vehs_prog[id_stop]
                is_stop = True
                for i in range(len(self.ids_vehs_prog[id_stop])-2, -1, -1):
                    # print '    check prt.%d'%self.ids_vehs_prog[id_stop][i][1],'started',self.ids_vehs_prog[id_stop][i][3]
                    if not self.ids_vehs_prog[id_stop][i][3]:  # is already started
                        is_stop = False
                        break

                if is_stop:
                    # make this vehicle stop at stopline and reset platoon timer
                    self.parent.prtvehicles.set_stop(
                        id_veh, self.ids_stop_to_ids_edge_sumo[id_stop], self.stoplines[id_stop])
                    self.times_plat_accumulate[id_stop] = time_start

            # try make previous vehicle the lead  vehicle
            ind_queue = self.ids_vehs[id_stop].index(id_veh)
            # print '  ids_vehs',self.ids_vehs[id_stop]
            # print '  ind_queue,queuelen,ok',ind_queue,len(self.ids_vehs[id_stop]),len(self.ids_vehs[id_stop]) > ind_queue+1
            if len(self.ids_vehs[id_stop]) > ind_queue+1:
                id_veh_newlead = self.ids_vehs[id_stop][ind_queue+1]
                # print '  id_veh_newlead, state',id_veh_newlead, self.parent.prtvehicles.states[id_veh_newlead]
                if self.parent.prtvehicles.states[id_veh_newlead] in LEADVEHICLESTATES:
                    self.set_leadveh(id_stop, id_veh_newlead)

            # print '  new lead veh prt.%d'%(self.ids_veh_lead[id_stop],)
            return True

        else:
            print '  no leader prt.%d exists' % (id_veh,)
            return False

    def init_trip_occupied(self, id_stop, id_berth, id_veh, id_veh_sumo, simtime):
        # TODO: actually a berth method??
        berths = self.get_berths()

        #id_veh_sumo = self.parent.prtvehicles.get_id_sumo(id_veh)
        n_pax = traci.vehicle.getPersonNumber(id_veh_sumo)
        # print 'init_trip_occupied', id_stop, id_berth, 'veh=%s'%id_veh_sumo,'simtime',simtime,'n_pax',n_pax

        # identify whic of the boarding persons is in the
        # vehicle which completed boarding
        dist_min = np.inf
        id_person_sumo_inveh = None
        stoppos = berths.stoppositions[id_berth]

        for id_person_sumo in self.ids_persons_sumo_prev[id_stop]:
            # print '  check veh of person',id_person_sumo,traci.person.getVehicle(id_person_sumo),id_veh_sumo

            # here we check whether person id_person_sumo at stop id_stop
            # is already sitting in vehicle  id_veh_sumo
            if traci.person.getVehicle(id_person_sumo) == id_veh_sumo:
                id_person_sumo_inveh = id_person_sumo

            #d = abs(stoppos - traci.person.getLanePosition(id_person_sumo))
            # if d<dist_min:
            #    dist_min = d
            #    id_person_sumo_inveh = id_person_sumo

        if id_person_sumo_inveh is not None:
            print '  found person %s in veh %s' % (id_person_sumo_inveh, id_veh_sumo)

            # program vehicle to person's destination
            # print '    found person,origs_dests',id_person_sumo_inveh,self.id_person_to_origs_dests[id_person_sumo_inveh]
            id_stop_orig, id_stop_dest, id_edge_sumo_from, id_edge_sumo_to = \
                self.id_person_to_origs_dests[id_person_sumo_inveh].pop(0)
            # print '    found person', id_person_sumo_inveh,'from', id_stop_orig, id_edge_sumo_from,' to' , id_edge_sumo_to, id_stop_dest

            stopline = self._get_stopline(id_stop, simtime)
            # print '    simtime', simtime

            self.parent.prtvehicles.init_trip_occupied(
                id_veh, self.ids_stop_to_ids_edge_sumo[id_stop],
                stopline,
            )

            # self.ids_persons_sumo_boarded[id_stop].remove(id_person_sumo_inveh)
            self.times_lastboard[id_stop] = simtime
            berths.states[id_berth] = BERTHSTATES['free']
            berths.ids_veh[id_berth] = -1
            # self.ids_vehs_outset[id_stop].add(id_veh)
            self.try_set_leadveh(id_stop, id_veh)
            self.parent.vehicleman.init_trip_occupied(id_veh, id_stop, id_stop_dest, simtime)
            return id_person_sumo_inveh

        else:
            print 'WARNING: on stop %d edge %s, berth %d no person found inside vehicle prt.%d' % (id_stop, self.ids_stop_to_ids_edge_sumo[id_stop], id_berth, id_veh)
            return None

    def _get_stopline(self, id_stop, simtime):
        if self.types[id_stop] == STOPTYPES['group']:
            # print '    platooning...',id_stop,simtime
            if self.times_plat_accumulate[id_stop] < 0:  # len(self.ids_vehs_prog[id_stop]) == 0:
                # print '      first in platoon-> stop it at exit-line',simtime
                #stopline = self.stoplines[id_stop]
                # actually not clear who will arrive first at the stopline
                # therefore do not stop. Stop must be set duruing rescheduling
                stopline = None
                self.times_plat_accumulate[id_stop] = simtime
                # print '    times_plat_accumulate',self.times_plat_accumulate[id_stop],simtime
            else:
                # print '      not first, let it approach previous veh.'
                stopline = None

        else:
            # print '    no platooning: all vehicles stop and wait for start'
            stopline = self.stoplines[id_stop]
        # print '    times_plat_accumulate',self.times_plat_accumulate[id_stop],'simtime',simtime
        # print '    stopline',stopline
        return stopline

    def init_trip_empty(self, id_stop, id_berth, id_veh, simtime, is_ask_vehman=True):
        # print 'Stops.init_trip_empty  id_stop, id_berth, id_veh, is_ask_vehman', id_stop, id_berth, 'prt.%d'%id_veh, is_ask_vehman,'simtime',simtime
        # TODO: actually a berth method??
        berths = self.get_berths()

        berths.states[id_berth] = BERTHSTATES['free']
        berths.ids_veh[id_berth] = -1

        # print '  proceed to stopline',self.stoplines[id_stop]
        stopline = self._get_stopline(id_stop, simtime)
        self.parent.prtvehicles.init_trip_empty(
            id_veh,
            self.ids_stop_to_ids_edge_sumo[id_stop],
            stopline)

        if is_ask_vehman:
            self.try_set_leadveh(id_stop, id_veh)
            #id_stop_target = self.parent.vehicleman.init_trip_empty(id_veh, id_stop)
            self.parent.vehicleman.init_trip_empty(id_veh, id_stop, simtime)

            # print 'init_trip_empty for',id_veh,' from',id_stop,'to',id_stop_target,id_edge_sumo_target
            #self.parent.prtvehicles.init_trip_empty(id_veh, self.ids_stop_to_ids_edge_sumo[id_stop_target])

        # else:
        #    self.parent.prtvehicles.init_trip_empty(id_veh, self.ids_stop_to_ids_edge_sumo[id_stop], -1)

        # self.ids_vehs_outset[id_stop].add(id_veh)

    def foreward_boardzone(self, id_stop,  ids_berth_board, simtime):
        print 'foreward_boardzone', id_stop, ids_berth_board, 'simtime', simtime
        berths = self.get_berths()
        #ids_berth_board = self.ids_berth_board[id_stop][::-1]
        # inds_o berths.states[ids_berth_board] != BERTHSTATES['free']
        for id_berth, state in zip(ids_berth_board, berths.states[ids_berth_board]):
            # print '    id_berth,boarding?,id_veh',id_berth, state== BERTHSTATES['boarding'],berths.ids_veh[id_berth]
            if state == BERTHSTATES['boarding']:
                self.init_trip_empty(id_stop, id_berth, berths.ids_veh[id_berth], simtime)

        self.times_lastboard[id_stop] = 10**4  # reset last board counter

    def enter(self, id_stop, id_veh):
        print 'enter id_stop, id_veh', id_stop, 'prt.%d' % id_veh

        self.parent.prtvehicles.decatenate(id_veh)

        self.ids_vehs[id_stop].append(id_veh)

        # tell vehman that veh arrived
        #self.numbers_veh_arr[id_stop] -= 1
        self.parent.vehicleman.conclude_trip(id_veh, id_stop)

        self.numbers_veh[id_stop] += 1
        id_berth = self.allocate_alight(id_stop)
        if id_berth < 0:
            # print '  allocation failed, command vehicle to slow down and wait for allocation'
            self.ids_vehs_toallocate[id_stop].append(id_veh)
            self.parent.prtvehicles.control_slow_down(id_veh)
        else:
            # command vehicle to go to berth for alighting
            id_veh_sumo = self.parent.prtvehicles.ids_sumo[id_veh]  # for debug only
            # print '  id_veh_sumo',id_veh_sumo
            #pos = traci.vehicle.getLanePosition(id_veh_sumo)
            if 0:
                print '     send entering vehicle id_veh %d, pos=%.2f to id_berth_alight %d at pos %.2fm' % (id_veh, traci.vehicle.getLanePosition(id_veh_sumo), id_berth, self.get_berths().stoppositions[id_berth])
                print '       ids_ghost', self.parent.prtvehicles.ids_ghosts[id_veh]
                print '       ids_leader', self.parent.prtvehicles.ids_leader[id_veh]
                print '       ids_follower', self.parent.prtvehicles.ids_follower[id_veh]

            self.parent.prtvehicles.control_stop_alight(id_veh, id_stop, id_berth,
                                                        id_edge_sumo=self.ids_stop_to_ids_edge_sumo[id_stop],
                                                        position=self.get_berths().stoppositions[id_berth],
                                                        )
            self.ids_vehs_alight_allocated[id_stop].append(id_veh)

    def exit(self, id_stop, id_veh):
        # print 'exit prt.%d at stop %d'%(id_veh,id_stop)
        self.ids_vehs[id_stop].remove(id_veh)
        #id_stop_target = self.parent.vehicleman.start_trip(id_veh, id_stop)
        #self.parent.prtvehicles.reschedule_trip(id_veh, self.ids_stop_to_ids_edge_sumo[id_stop_target])
        #ind_veh = -1
        # print '  ids_vehs_prog=\n',self.ids_vehs_prog[id_stop]
        i = 0
        for time_start, id_veh_prog, id_stop_target, is_prog in self.ids_vehs_prog[id_stop]:
            if id_veh_prog == id_veh:
                self.ids_vehs_prog[id_stop].pop(i)
                break
            i = +1

        # self.ids_vehs_prog[id_stop].remove(id_veh)

        self.numbers_veh[id_stop] -= 1

    def allocate_alight(self, id_stop):

        # print 'allocate_alight',id_stop
        #self.inds_berth_alight_allocated [id_stop] = len(self.ids_berth_alight[id_stop])
        ind_berth = self.inds_berth_alight_allocated[id_stop]

        if ind_berth == 0:
            # no free berth :(
            return -1
        else:
            ind_berth -= 1
            self.inds_berth_alight_allocated[id_stop] = ind_berth
            return self.ids_berth_alight[id_stop][ind_berth]

    def allocate_board(self, id_stop, n_alloc, queues):
        """
        Return successive berth ID to be allocated for boarding
        at given stop ID.
        n_alloc is the number of vehicles which remain to be 
        allocated. 
        """

        #self.inds_berth_alight_allocated [id_stop] = len(self.ids_berth_alight[id_stop])
        ind_berth = self.inds_berth_board_allocated[id_stop]
        # print 'allocate_board id_stop, n_alloc,ind_berth',id_stop, n_alloc,ind_berth
        if ind_berth == 0:
            # no free berth :(
            return -1
        else:
            if queues is None:

                # less or equal allocation positions in board zone
                # then vehicles to be allocated
                ind_berth -= 1
                # print '  allocate in order ind_berth=',ind_berth
                self.inds_berth_board_allocated[id_stop] = ind_berth
                return self.ids_berth_board[id_stop][ind_berth]
            else:
                # there are more allocation positions in board zone
                # then vehicles to be allocated
                ind_berth -= 1
                id_berth = self.ids_berth_board[id_stop][ind_berth]
                # print '  check queue, start with ind_berth=%d,n_alloc=%d, id_berth=%d, queue=%d, pos=%.2fm'%(ind_berth,n_alloc,id_berth,queues[id_berth],self.get_berths().stoppositions[id_berth]),queues[id_berth] == 0,ind_berth >= n_alloc
                while (queues[id_berth] == 0) & (ind_berth >= n_alloc):
                    ind_berth -= 1
                    id_berth = self.ids_berth_board[id_stop][ind_berth]
                    # print '  check queue, start with ind_berth=%d,n_alloc=%d, id_berth=%d, queue=%d, pos=%.2fm'%(ind_berth,n_alloc,id_berth,queues[id_berth],self.get_berths().stoppositions[id_berth]),queues[id_berth] == 0,ind_berth >= n_alloc

                self.inds_berth_board_allocated[id_stop] = ind_berth
                return id_berth

    def get_berthqueues(self, id_stop):
        # currently not used
        # print 'get_berthqueues',id_stop
        # TODO: use stop angle and person angle to detect waiting persons
        ids_berth_board = self.ids_berth_board[id_stop]
        stoppositions = self.get_berths().stoppositions[ids_berth_board]
        counters = np.zeros(len(stoppositions), dtype=np.int32)
        # print '  stoppositions',stoppositions
        for id_person_sumo in self.waittimes_persons[id_stop].keys():
            position = traci.person.getLanePosition(id_person_sumo)
            # print '    position',position
            dists = np.abs(stoppositions-position)
            # print '  dists',dists,np.any(dists<5)
            if np.any(dists < 0.8):
                ind_berth = np.argmin(dists)
                counters[ind_berth] += 1

        queues = {}
        for id_berth, count in zip(ids_berth_board, counters):
            queues[id_berth] = count

        # print '  queues=\n',queues
        return queues

    def make_from_net(self):
        """
        Make prt stop database from PT stops in network.
        """
        print 'make_from_net'
        self.clear()
        net = self.get_scenario().net
        ptstops = net.ptstops

        ids_ptstop = ptstops.get_ids()
        id_mode_prt = self.parent.id_prtmode

        #ids_edges = net.lanes.ids_edge[ptstops.ids_lane[ids_ptstop]]
        #ids_lanes = net.edges.ids_lanes[ids_edges]
        ids_lane = ptstops.ids_lane[ids_ptstop]
        #edgelengths = net.edges.lengths

        for id_stop, id_lane, position_from, position_to in zip(
            ids_ptstop,
            # ids_lanes,
            ids_lane,
            ptstops.positions_from[ids_ptstop],
            ptstops.positions_to[ids_ptstop],
        ):
            # get allowed modes of lane with index 1
            modes_allow = net.lanes.ids_modes_allow[id_lane]
            # print '  check id_stop, modes_allow, position_from, position_to',id_stop, modes_allow, position_from, position_to
            if id_mode_prt in modes_allow:
                self.make(id_stop,
                          position_from,
                          position_to)

        self.parent.make_fstar(is_update=True)
        self.parent.make_times_stop_to_stop()

    def make(self, id_ptstop, position_from, position_to):
        """
        Initialize a new prt stop and generate berth.
        """
        id_stop = self.add_row(ids_ptstop=id_ptstop)
        ids_berth = self.get_berths().make(id_stop, position_from=position_from,
                                           position_to=position_to)
        n_berth = len(ids_berth)
        n_berth_alight = int(0.5*n_berth)
        n_berth_board = n_berth-n_berth_alight
        self.ids_berth_alight[id_stop] = ids_berth[0:n_berth_alight]
        self.ids_berth_board[id_stop] = ids_berth[n_berth_alight:n_berth]
        return id_stop


class VehicleAdder(Process):
    def __init__(self,  vehicles, logger=None, **kwargs):
        print 'VehicleAdder.__init__', vehicles, vehicles.parent.get_ident()
        self._init_common('vehicleadder', name='Vehicle adder',
                          logger=logger,
                          info='Add vehicles to PRT stops of network.',
                          )
        self._vehicles = vehicles

        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.n_vehicles = attrsman.add(cm.AttrConf('n_vehicles', kwargs.get('n_vehicles', -1),
                                                   groupnames=['options'],
                                                   perm='rw',
                                                   name='Number of vehicles',
                                                   info='Number of PRT vehicles to be added to the network. Use -1 to fill all present PRT stations.',
                                                   ))

    def do(self):
        # print 'VehicleAdder.do'
        self._vehicles.add_to_net(n=self.n_vehicles)
        return True


class PrtVehicles(am.ArrayObjman):

    def __init__(self, ident, prtservices, **kwargs):
        # print 'PrtVehicles vtype id_default',vtypes.ids_sumo.get_id_from_index('passenger1')
        self._init_objman(ident=ident,
                          parent=prtservices,
                          name='PRT Veh.',
                          info='PRT vehicle database. These are shared vehicles.',
                          version=0.1,
                          **kwargs)

        self._init_attributes()

    def _init_attributes(self):
        vtypes = self.get_scenario().demand.vtypes
        net = self.get_scenario().net

        version = self.get_version()
        if version < 0.1:
            self.delete('speed_max')
            self.delete('time_emergency')
            self.delete('decel_comfort')
            self.delete('decel_emergency')
            self.delete('speed_max')

        self.add(cm.AttrConf('time_update', 0.5,
                             groupnames=['parameters'],
                             name='Update time',
                             info="Update time for traci controlled vehicles, es. at merge points.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('speedmode_follower', 1,
                             groupnames=['parameters'],
                             name='Follower speed mode',
                             info="""Follower speed mode. The single bits have the following meaning:
                                                bit0: Regard safe speed 
                                                bit1: Regard maximum acceleration 
                                                bit2: Regard maximum deceleration - 
                                                bit3: Regard right of way at intersections 
                                                bit4: Brake hard to avoid passing a red light
                                        """,
                             ))

        self.add(cm.AttrConf('factor_speed_follower', 2.0,
                             groupnames=['parameters'],
                             name='Follower speed factor',
                             info="""Follower speed factor.""",
                             ))

        self.add(cm.AttrConf('accel_follower', 5.0,
                             groupnames=['parameters'],
                             name='Follower acceleration',
                             info="Follower acceleration.",
                             unit='m/s^2',
                             ))

        self.add(cm.AttrConf('decel_follower', 5.0,
                             groupnames=['parameters'],
                             name='Follower deceleration',
                             info="Follower deceleration.",
                             unit='m/s^2',
                             ))

        self.add(cm.AttrConf('decel_emergency_follower', 5.0,
                             groupnames=['parameters'],
                             name='Follower Emergency deceleration',
                             info="Follower Emergency deceleration.",
                             unit='m/s^2',
                             ))

        self.add(cm.AttrConf('dist_min_follower', 0.0,
                             groupnames=['parameters'],
                             name='Follower min. distance',
                             info="Follower minimum distance",
                             unit='m',
                             ))

        self.add(cm.AttrConf('tau_follower', 0.0,
                             groupnames=['parameters'],
                             name='Follower reaction time',
                             info="Follower reaction time.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('n_ghosts_max', 4,
                             groupnames=['parameters'],
                             name='Max ghosts',
                             info="Maximum number of ghost vehicles. Ghost vehicles are used in merge controls.",
                             ))

        # TODO: add/update vtypes here
        self.add_col(SumoIdsConf('Veh', xmltag='id'))

        id_vtype = self.make_vtype(is_renew=version < 0.1)

        if not hasattr(self, 'ids_vtype'):

            self.add_col(am.IdsArrayConf('ids_vtype', vtypes,
                                         id_default=id_vtype,
                                         groupnames=['state'],
                                         name='Veh. type',
                                         info='PRT vehicle type.',
                                         #xmltag = 'type',
                                         ))
        else:
            # this imposes parameters to alresdy existing data
            self.ids_vtype[self.get_ids()] = id_vtype

        self.add_col(am.ArrayConf('states', default=VEHICLESTATES['init'],
                                  groupnames=['_private'],
                                  dtype=np.int32,
                                  choices=VEHICLESTATES,
                                  name='state',
                                  info='State of vehicle.',
                                  ))

        # self.add_col(am.IdsArrayConf( 'ids_targetprtstop', self.parent.prtstops,
        #                            groupnames = ['parameters'],
        #                            name = 'Target stop ID',
        #                            info = 'ID of current target PRT stop.',
        #                            ))

        self.add_col(am.IdsArrayConf('ids_currentedge', net.edges,
                                     groupnames=['state'],
                                     name='Current edge ID',
                                     info='Edge ID of most recent reported position.',
                                     ))

        # self.add_col(am.IdsArrayConf( 'ids_targetedge', net.edges,
        #                            groupnames = ['state'],
        #                            name = 'Target edge ID',
        #                            info = 'Target edge ID to be reached. This can be either intermediate target edges (), such as a compressor station.',
        #                            ))

        self.set_version(0.1)

    def _init_constants(self):

        self.do_not_save_attrs([
            'length',
            'tau', 'ids_berth',
            'are_update', 'velocities',
            'odos', 'ids_ghosts',
            'diststomerge0', 'diststomerge0_ghosts',
            'dists0', 'odos0_vehicles', 'odos0_ghosts',
            'ids_leader', 'ids_follower', 'lengths_plat',
        ])

    def get_net(self):
        return self.parent.get_scenario().net

    def make_vtype(self, is_renew=False):
        print 'make_vtype PRT'
        vtypes = self.get_scenario().demand.vtypes
        prttype = 'PRT'

        if (not vtypes.ids_sumo.has_index(prttype)) | is_renew:
            if vtypes.ids_sumo.has_index(prttype):
                vtypes.del_row(vtypes.ids_sumo.get_id_from_index(prttype))
            id_vtype = vtypes.add_vtype(prttype,
                                        accel=2.5,
                                        decel=2.5,
                                        decel_apparent=2.5,  # followe should not be freightened
                                        decel_emergency=5.0,
                                        sigma=0.0,
                                        length=3.5,
                                        width=1.6,
                                        height=1.7,
                                        number_persons=1,
                                        capacity_persons=1,
                                        dist_min=0.3,
                                        tau=0.5,
                                        speed_max=60.0/3.6,
                                        factor_speed=0.9,
                                        deviation_speed=0.1,  # slight deviation for better following
                                        # emissionclass= 'HBEFA3/zero',# defined in electricity model
                                        id_mode=self.parent.id_prtmode,  # specifies mode for demand
                                        color=np.array((255, 240, 0, 255), np.float32)/255.0,
                                        shape_gui='evehicle',
                                        times_boarding=1.5,
                                        times_loading=20.0,
                                        sublane_alignment_lat='center',
                                        sublane_speed_max_lat=0.5,
                                        sublane_gap_min_lat=0.24,
                                        sublane_alignment_eager=1000000.0,
                                        #
                                        power_max=5000.0,
                                        mass=800.0,
                                        area_front_surface=3.5*1.6,
                                        coefficient_drag_air=0.4,
                                        moment_inertia_internal=0.01,
                                        coefficient_drag_radial=0.5,
                                        coefficient_drag_roll=0.005,
                                        efficiency_propulsion=0.9,
                                        #
                                        eprofile='prt',
                                        capacity_battery=2000.0,
                                        efficiency_reuperation=0.4,
                                        speed_charging=0.03,
                                        )

        else:
            id_vtype = vtypes.ids_sumo.get_id_from_index(prttype)

        # speedmode
        # https://sumo.dlr.de/docs/TraCI/Change_Vehicle_State.html
        # bit0: Regard safe speed
        # bit1: Regard maximum acceleration
        # bit2: Regard maximum deceleration -
        # bit3: Regard right of way at intersections
        # bit4: Brake hard to avoid passing a red light

        #self._speedmode_leader = 7
        #self._speedmode_follower = 6

        # leader and follower parameters are used when switching
        # between leader and follower in concatenate/deconcatenate
        self._speedmode_leader = 7
        self._speedmode_follower = self.speedmode_follower.get_value()

        self._factor_speed_leader = vtypes.factors_speed[id_vtype]
        self._factor_speed_follower = self.factor_speed_follower.get_value()

        self._accel_leader = vtypes.accels[id_vtype]
        self._accel_follower = self.accel_follower.get_value()

        self._decel_leader = vtypes.decels[id_vtype]
        self._decel_follower = self.decel_follower.get_value()

        self._decel_emergency_leader = vtypes.decels_emergency[id_vtype]
        self._decel_emergency_follower = self.decel_emergency_follower.get_value()

        self._dist_min_leader = vtypes.dists_min[id_vtype]
        self._dist_min_follower = self.dist_min_follower.get_value()

        self._tau_leader = vtypes.taus[id_vtype]
        self._tau_follower = self.tau_follower.get_value()

        # these are additional, somehow parameters
        # TODO: redundant parameters with leader and follower parameters
        # should be removed

        self.length = vtypes.lengths[id_vtype]
        self.speed_max = vtypes.speeds_max[id_vtype]
        self.accel = vtypes.accels[id_vtype]
        self.decel = vtypes.decels[id_vtype]

        self.decel_emergency = vtypes.decels_emergency[id_vtype]
        self.tau = vtypes.taus[id_vtype]
        self.dist_min = vtypes.dists_min[id_vtype]
        self.factor_speed = vtypes.factors_speed[id_vtype]

        return id_vtype

    def get_length(self):
        return self.length

    def prepare_sim(self, process):
        print 'PrtVehicles.prepare_sim'
        if len(self) == 0:
            return []

        ids = self.get_ids()
        net = self.get_scenario().net
        #nodes = net.nodes
        #edges = net.edges
        #lanes = net.lanes
        #ids_edge_sumo = edges.ids_sumo

        id_prtmode = self.parent.id_prtmode

        # here take parameters from first vtype and assume that all are the same
        # this call ensures that most recent parameters are considered
        id_vtype = self.make_vtype()

        #ptstops = net.ptstops
        lanes = net.lanes
        #ids_edge_sumo = net.edges.ids_sumo

        n_id_max = np.max(ids)+1
        n_ghosts_max = self.n_ghosts_max.get_value()
        self.ids_berth = -1*np.ones(n_id_max, dtype=np.int32)

        self.are_update = np.zeros(n_id_max, dtype=np.bool)
        self.velocities = np.zeros(n_id_max, dtype=np.float32)
        self.odos = np.zeros(n_id_max, dtype=np.float32)

        self.ids_ghosts = -1*np.ones((n_id_max, n_ghosts_max), dtype=np.int32)

        self.diststomerge0 = np.inf * np.ones((n_id_max, n_ghosts_max), dtype=np.float32)
        self.diststomerge0_ghosts = np.inf * np.ones((n_id_max, n_ghosts_max), dtype=np.float32)
        self.dists0 = np.inf * np.ones((n_id_max, n_ghosts_max), dtype=np.float32)

        self.odos0_vehicles = -1 * np.ones((n_id_max, n_ghosts_max), dtype=np.float32)
        self.odos0_ghosts = -np.inf * np.ones((n_id_max, n_ghosts_max), dtype=np.float32)

        # platooning
        # id of leader of this vehicle (vehicle in front), -1 = platoonleader
        self.ids_leader = -1 * np.ones(n_id_max, dtype=np.int32)

        # id of follower of this vehicle (vehicle behind), -1 = last veh
        self.ids_follower = -1 * np.ones(n_id_max, dtype=np.int32)

        # total length of platoon
        self.lengths_plat = np.zeros(n_id_max, dtype=np.float32)

        return [(self.time_update.get_value(), self.process_step), ]

    def add_ghost(self, id_veh, id_ghost, dist_to_merge_veh, dist_to_merge_ghost,
                  is_substitute=False):
        ids_ghosts = list(self.ids_ghosts[id_veh])
        # if id_ghost in ids_ghosts:
        #    # reconfigure existing
        #    ind_ghost = self.ids_ghosts[id_veh].index(id_ghost)
        #    id_ghost_old = self.ids_ghosts[id_veh][ind_ghost]
        #    # try to delete old ghost
        #    self.del_ghost(id_veh, id_ghost_old)
        # else:
        # add new ghost
        print 'add_ghost id_veh %d id_ghost %d' % (id_veh, id_ghost)  # ,self.ids_ghosts.shape

        if -1 not in ids_ghosts:
            # print 'ERROR: no more ghosts available, ids_ghosts',ids_ghosts
            # sys.exit(1)
            # print '  overwrite last ghost'
            # here we could sunstitute the ghost with the longest distance
            ind_ghost = len(ids_ghosts)-1
        else:
            ind_ghost = ids_ghosts.index(-1)

            if is_substitute:
                id_ghost_prev = ids_ghosts[ind_ghost]
                if id_ghost_prev > -1:
                    self.stop_update(id_ghost_prev)
                    if ind_ghost > 0:
                        ind_ghost -= 1

        if ind_ghost > 0:
            print 'WARNING: unusual number of ghosts, ids_ghosts', ids_ghosts
            # sys.exit(1)

        self.ids_ghosts[id_veh][ind_ghost] = id_ghost
        self.diststomerge0[id_veh][ind_ghost] = dist_to_merge_veh
        self.diststomerge0_ghosts[id_veh][ind_ghost] = dist_to_merge_ghost

        # nose to nose distances
        self.dists0[id_veh][ind_ghost] = dist_to_merge_veh - dist_to_merge_ghost

        # get absolute running distances
        self.odos0_vehicles[id_veh][ind_ghost] = get_traci_odo(self.get_id_sumo(id_veh))
        self.odos0_ghosts[id_veh][ind_ghost] = get_traci_odo(self.get_id_sumo(id_ghost))

        # both ghosts and vehicles need update
        self.start_update(id_veh)
        self.start_update(id_ghost)

    def del_ghosts(self, id_veh):
        if self.ids_ghosts[id_veh][0] == -1:
            # id_veh has no ghosts
            return
        else:
            for id_ghost in self.ids_ghosts[id_veh]:
                if id_ghost > -1:
                    self.del_ghost(id_veh, id_ghost)

    def del_ghost(self, id_veh, id_ghost):
        # print 'del_ghost id_veh %d id_ghost %d'%(id_veh, id_ghost)
        if id_ghost in self.ids_ghosts[id_veh]:

            ind_ghost = list(self.ids_ghosts[id_veh]).index(id_ghost)
            self.ids_ghosts[id_veh][ind_ghost] = -1

            self.diststomerge0[id_veh][ind_ghost] = np.inf
            self.diststomerge0_ghosts[id_veh][ind_ghost] = np.inf
            self.dists0[id_veh][ind_ghost] = np.inf

            self.odos0_vehicles[id_veh][ind_ghost] = -1.0
            self.odos0_ghosts[id_veh][ind_ghost] = -np.inf

            self.stop_update(id_veh)
            self.stop_update(id_ghost)
        else:
            # veh has not such ghost
            pass

    def del_all_ghosts(self, id_veh):
        if self.ids_ghosts[id_veh][0] == -1:
            # id_veh has no ghosts
            return

        for id_ghost in self.ids_ghosts[id_veh]:
            if id_ghost > -1:
                self.del_ghost(id_veh, id_ghost)

        # just to be sure ...
        self.stop_update(id_veh)

    def switch_off_control(self, id_veh):
        """Direct way to switch of SUMO control of vehicles"""
        # print 'switch_off_control id_veh',id_veh
        traci.vehicle.setSpeedMode(self.ids_sumo[id_veh], 6)  # 6=respect max accel/decel

    def switch_on_control(self, id_veh):
        """Direct way to switch of SUMO control of vehicles"""
        # print 'switch_on_control id_veh',id_veh
        traci.vehicle.setSpeedMode(self.ids_sumo[id_veh], self._speedmode_leader)

    def start_update(self, id_veh):
        """Start updating control by ghosts"""
        self.are_update[id_veh] = True
        traci.vehicle.setSpeedMode(self.ids_sumo[id_veh], 6)  # 6=respect max accel/decel

    def stop_update(self, id_veh):
        """Stop updating control by ghosts"""
        if np.all(self.ids_ghosts[id_veh] == -1):  # id_veh has no ghosts?

            # attention stopping from being conrolled by merge means
            # handing control back to SUMO.
            # followers are not controlled by the merge process
            traci.vehicle.setSpeedMode(self.ids_sumo[id_veh], self._speedmode_leader)
            if id_veh not in self.ids_ghosts:  # id_veh is no ghost ?
                self.are_update[id_veh] = False  # stop updating

    def process_step(self, process):
        simtime = process.simtime
        print 79*'_'
        print 'PrtVehicles.process_step at', simtime
        net = self.get_scenario().net
        vehicles = self.parent.prtvehicles
        ids = self.get_ids()
        if len(ids) == 0:
            return
        time_update = self.time_update.get_value()
        decel_emergency = self.decel_emergency
        # print '  update',len(np.flatnonzero(self.are_update[ids])),'vehicles'
        ids_update = ids[self.are_update[ids]]
        # print '  ids_update',ids_update

        # print '  *Debug:'
        #ids_debug = [307, 236, 41, 231, 208, 44, 249, 229, 136]
        #ids_debug = [276,277,278]
        #
        # for id_veh, id_follower, id_leader in zip(ids_debug,self.ids_follower[ids_debug], self.ids_leader[ids_debug]):
        #    print '  *id_veh',id_veh,'id_follower',id_follower,'id_leader',id_leader

        # loop through vehicles that need speed modifications
        # these are vehicles which have ghosts or vehicles which are
        # in merge processes
        for id_veh, id_veh_sumo in zip(ids_update, self.ids_sumo[ids_update]):
            # update odometer and speed
            self.velocities[id_veh] = get_traci_velocity(id_veh_sumo)
            self.odos[id_veh] = get_traci_odo(id_veh_sumo)

            if 0:
                ids_ghost = self.ids_ghosts[id_veh]
                print '  %7s' % id_veh_sumo, 'ghosts', ids_ghost, self.ids_leader[id_veh], "lp=%.1fm" % self.lengths_plat[id_veh]
                #odo = self.odos[id_veh]
                #delta_vehs = odo-self.odos0_vehicles[id_veh]
                #delta_ghosts = self.odos[ids_ghost] - self.odos0_ghosts[id_veh]
                # print '    delta_vehs, delta_ghosts',min(delta_vehs), min(delta_ghosts)#, self.dists0[id_veh]
                # print '    dists', min(self.dists0[id_veh] + delta_ghosts - delta_vehs)

                #self.diststomerge0[id_veh][ind_ghost] = np.inf
                #self.diststomerge0_ghosts[id_veh][ind_ghost] = np.inf

        # pick vehicle ids that must be controlled due to a present ghost
        ids_contr = ids_update[self.ids_ghosts[ids_update][:, 0] > -1]
        # print '  ids_contr',ids_contr
        # print '  self.ids_ghosts[ids_update][:,0]',self.ids_ghosts[ids_update][:,0]
        # print '  inds',self.ids_ghosts[ids_update][:,0]>-1
        n_contr = len(ids_contr)
        n_ghosts_max = self.n_ghosts_max.get_value()

        # no vehicles with ghost, nothing to control
        if n_contr == 0:
            return

        # get gosts
        ids_ghosts = self.ids_ghosts[ids_contr]

        # print '  self.odos[ids_contr]',self.odos[ids_contr].reshape(n_contr,1)
        # print '  self.odos0_vehicles[ids_contr]',self.odos0_vehicles[ids_contr]

        # distance made since merge zone is entered
        deltas_vehs = self.odos[ids_contr].reshape(n_contr, 1)-self.odos0_vehicles[ids_contr]
        deltas_ghosts = self.odos[ids_ghosts] - self.odos0_ghosts[ids_contr]

        #dists = self.dists0[ids_contr] + deltas_ghosts - deltas_vehs
        # print '  dists',dists
        #dists_min = np.min(self.dists0[ids_contr] + deltas_ghosts - deltas_vehs, 1) - (self.length+self.lengths_plat[ids_contr])

        # get distance between nose of vehicle and tail of ghost which is closest
        dists_min = np.min(self.dists0[ids_contr] + deltas_ghosts - deltas_vehs -
                           self.lengths_plat[ids_ghosts], 1) - self.length

        # ??????? minimum dist to merge over all vehicles -> remove, not in use
        #diststomerge_min = np.min(self.diststomerge0[ids_contr] - deltas_vehs , 1)
        #self.diststomerge0[id_veh][ind_ghost] = dist_to_merge_veh
        # self.diststomerge0_ghosts[id_veh][ind_ghost] =

        # print '  dists_min',dists_min
        velocities = self.velocities[ids_contr]

        # print '  velocities_ghosts masked\n',self.velocities[ids_ghosts]

        #mask_ghosts = INF*(ids_ghosts==-1).astype(np.float32)
        # print '  mask_ghosts\n',mask_ghosts
        velocities_ghosts = self.velocities[ids_ghosts] + INF*(ids_ghosts == -1)

        # print '  velocities_ghosts masked\n',velocities_ghosts
        #
        # velocities of slowest ghosts
        velocities_ghost_min = np.min(self.velocities[ids_ghosts] + INF*(ids_ghosts == -1), 1)
        # print '  velocities_ghost_min\n',velocities_ghost_min
        dists_safe = self.tau*velocities + 0.5/decel_emergency*velocities*velocities
        dists_comf = self.tau*velocities + 0.5/self.decel * \
            (velocities*velocities-velocities_ghost_min*velocities_ghost_min)

        # print '  dists_safe',dists_safe
        # print '  dists_comf',dists_comf
        #dists_crit = np.max(np.concatenate((dists_safe.reshape(n_contr,1),dists_comf.reshape(n_contr,1)),1),1)
        # print '  dists_crit',dists_crit

        deltas_crit = dists_min-np.max(np.concatenate((dists_safe.reshape(n_contr, 1),
                                                       dists_comf.reshape(n_contr, 1)), 1), 1)
        #deltas_crit = dists_min-dists_safe
        # print '  deltas_crit',deltas_crit
        inds_slow = deltas_crit < 0
        inds_accel = (deltas_crit > 0) & (velocities < 0.8*velocities_ghost_min)
        # print '  inds_slow',inds_slow

        if 0:
            for id_sumo, velocity, velocitiy_ghost_min,\
                dist_safe, dist_comf, dist_min, delta_crit,\
                is_slow, is_accel, length_plat\
                in zip(self.ids_sumo[ids_contr],
                       velocities,
                       velocities_ghost_min,
                       dists_safe,
                       dists_comf,
                       dists_min,
                       deltas_crit,
                       inds_slow, inds_accel, self.lengths_plat[ids_contr],
                       # diststomerge_min,
                       ):

                if is_slow:
                    a = '-'
                elif is_accel:
                    a = '+'
                else:
                    a = '='
                # print 'id_sumo, velocity, velocitiy_ghost_min,dist_safe,dist_comf,dist_min,delta_crit,is_slow,is_accel, length_plat\n',\
                # id_sumo, velocity, velocitiy_ghost_min,\
                # dist_safe,dist_comf,dist_min,delta_crit,\
                #is_slow,is_accel, length_plat

                th = (dist_min + self.length)/velocity
                ds_check = self.tau*velocity + 0.5/decel_emergency*velocity*velocity
                # print '    %7s: v=%3.1f vg=%3.1f dh=%4.1f th=%4.1fs ds=%4.1f dc=%4.1f lp=%3.1f %s'%(id_sumo, velocity, velocitiy_ghost_min,dist_min,th,dist_safe,dist_comf,length_plat,a)

        fact_urgent = np.ones(dists_safe.shape, dtype=np.float32)  # np.clip(dists_safe/diststomerge_min,0.0,1.0)

        v_delta_brake = time_update*self.decel*fact_urgent
        for id_sumo, velocity, dv in zip(self.ids_sumo[ids_contr[inds_slow]], velocities[inds_slow], v_delta_brake):
            # print '    deccel %s from %.2f to %.2fm/s dv = %.2fm/s'%(id_sumo, velocity, velocity-dv,dv)
            if velocity-dv > 0:  # upset when negative velocities
                traci.vehicle.slowDown(id_sumo, velocity-dv, time_update)

        v_delta_accel = time_update*self.accel*fact_urgent
        for id_sumo, velocity, dv in zip(self.ids_sumo[ids_contr[inds_accel]], velocities[inds_accel], v_delta_accel):
            #dv = time_update*self.accel
            # print '    accel %s from %.2f to %.2fm/s dv = %.2fm/s'%(id_sumo, velocity, velocity+dv,dv)
            traci.vehicle.slowDown(id_sumo, velocity+dv, time_update)

        # print '  deltas_vehs',deltas_vehs
        # print '  self.ids_ghosts[ids_contr]',self.ids_ghosts[ids_contr]
        # print '  self.odos[self.ids_ghosts[ids_contr]]',self.odos[self.ids_ghosts[ids_contr]]
        # print '  self.odos0_ghosts[id_veh]',self.odos0_ghosts[ids_contr]
        # print '  deltas_ghosts',deltas_ghosts
        # print '  dists',dists
        # for id_contr, delta_vehs, delta_ghosts,dist in zip(ids_contr,deltas_vehs,deltas_ghosts,dists):
        #    print '  veh',id_contr,'dist',dist

        #self.length = vtypes.lengths[id_vtype]
        #self.speed_max = vtypes.speeds_max[id_vtype]
        #self.accel = vtypes.accels[id_vtype]
        #self.decel = vtypes.decels[id_vtype]
        #self.tau = vtypes.taus[id_vtype]

    def reset_speedmode(self, id_veh_sumo):
        print 'reset_speedmode', id_veh_sumo
        # speed mode (0xb3)
        # Per default, the vehicle is using the given speed regarding the safe gap, the maximum acceleration, and the maximum deceleration. Furthermore, vehicles follow the right-of-way rules when approaching an intersection and if necessary they brake hard to avoid driving across a red light. One can control this behavior using the speed mode (0xb3) command, the given integer is a bitset (bit0 is the least significant bit) with the following fields:
        # 1 bit0: Regard safe speed
        # 2 bit1: Regard maximum acceleration
        # 4 bit2: Regard maximum deceleration
        # 8 bit3: Regard right of way at intersections
        # 16 bit4: Brake hard to avoid passing a red light
        #  1+2+4
        # traci.vehicle.setSpeedMode(id_veh_sumo,7)
        #self.speed_max = vtypes.speeds_max[id_vtype]
        #self.accel = vtypes.accels[id_vtype]
        traci.vehicle.slowDown(id_veh_sumo, self.speed_max, self.speed_max/self.accel)
        # pass

    def concatenate(self, id_veh, id_veh_pre):
        print 'concatenate prt.%d' % id_veh, 'behind  prt.%d' % id_veh_pre
        # print '  >>'
        self.ids_leader[id_veh] = id_veh_pre
        self.ids_follower[id_veh_pre] = id_veh

        id_veh_sumo = self.get_id_sumo(id_veh)

        if 0:
            print 'follower params'
            print '  speedmode', self._speedmode_follower
            print '  factor_speed', self._factor_speed_follower
            print '  decel', self._decel_emergency_follower, '= decel_emergency'
            print '  accel_follower', self._accel_follower
            print '  dist_min', self._dist_min_follower
            print '  tau', self._tau_follower

        # if 0:
        #    traci.vehicle.setSpeedFactor(id_veh_sumo,2.0)# +random.uniform(-0.01,0.01)
        #    traci.vehicle.setImperfection(id_veh_sumo,0.0)
        #    traci.vehicle.setAccel(id_veh_sumo,3.5)
        #    traci.vehicle.setMinGap(id_veh_sumo,0.01)
        #    traci.vehicle.setTau(id_veh_sumo,0.2)

        # if 1:
        traci.vehicle.setSpeedMode(id_veh_sumo, self._speedmode_follower)
        traci.vehicle.setSpeedFactor(id_veh_sumo, self._factor_speed_follower)
        # traci.vehicle.setImperfection(id_veh_sumo,0.0)
        traci.vehicle.setDecel(id_veh_sumo, self._decel_emergency_follower)
        traci.vehicle.setAccel(id_veh_sumo, self._accel_follower)
        traci.vehicle.setMinGap(id_veh_sumo, self._dist_min_follower)
        traci.vehicle.setTau(id_veh_sumo, self._tau_follower)

        if self.lengths_plat[id_veh] > 0.1:
            self._update_concatenate(id_veh, self.lengths_plat[id_veh])
        else:
            self._update_concatenate(id_veh, 0.0)

    def _update_concatenate(self, id_veh, length_plat):
        """
        Propagates length to the first vehicle of the platoon
        """
        # print '_update_concatenate prt.%s, length_plat=%.1f, length=%.1f,'%(id_veh,length_plat,self.length),'id_leader',self.ids_leader[id_veh]
        if self.ids_leader[id_veh] == -1:
            # first vehicle
            # print '  first vehicle prt.%s'%id_veh,length_plat
            self.lengths_plat[id_veh] = length_plat
        else:
            # propagate platoon length
            self.lengths_plat[id_veh] = 0.0
            self._update_concatenate(self.ids_leader[id_veh], length_plat + self.length)

    def decatenate(self, id_veh):
        print 'decatenate prt.%d' % id_veh

        id_leader = self.ids_leader[id_veh]
        # print '  id_leader',id_leader
        if id_leader > -1:
            id_veh_sumo = self.get_id_sumo(id_veh)
            if self.ids_leader[id_leader] == -1:
                # leader of this vehicle is first in platoon
                # this should be normally the case if a platoon is
                # broken up from the first vehicle backwards

                # calculate new shortened platoon length
                # TODO
                self.lengths_plat[id_veh] = self.lengths_plat[id_leader]-self.length
            else:
                print 'WARNING in decatenate: platoon broken up in the middel at', id_veh_sumo
                self.lengths_plat[id_veh] = 0.0

            # this vehicle will become first in the platoon
            self.ids_leader[id_veh] = -1
            self.ids_follower[id_leader] = -1
            # if 0:
            #    traci.vehicle.setSpeedFactor(id_veh_sumo, 1.5)#+random.uniform(-0.01,0.01)
            #    traci.vehicle.setMinGap(id_veh_sumo,0.3)
            #    traci.vehicle.setImperfection(id_veh_sumo,0.0)
            #    traci.vehicle.setTau(id_veh_sumo,0.8)
            #    traci.vehicle.setAccel(id_veh_sumo,2.5)

            # if 1:
            # reset parameters
            traci.vehicle.setSpeedMode(id_veh_sumo, self._speedmode_leader)

            # rest are leader values taken from type def
            traci.vehicle.setSpeedFactor(id_veh_sumo, self.factor_speed)  # +random.uniform(-0.01,0.01)
            traci.vehicle.setMinGap(id_veh_sumo, self.dist_min)
            # traci.vehicle.setImperfection(id_veh_sumo,1.0)
            traci.vehicle.setTau(id_veh_sumo, self.tau)
            traci.vehicle.setDecel(id_veh_sumo, self.decel)
            traci.vehicle.setAccel(id_veh_sumo, self.accel)
        else:
            # it is a leader itself and does not need to decatenate
            # remove platoon length
            self.lengths_plat[id_veh] = 0.0

    def get_platoonleader(self, id_veh):
        while self.ids_leader[id_veh] > -1:
            id_veh = self.ids_leader[id_veh]
        return id_veh

    def get_platoontail(self, id_veh):
        while self.ids_follower[id_veh] > -1:
            id_veh = self.ids_follower[id_veh]
        return id_veh

    def get_platoon(self, id_veh):
        print 'get_platoon', id_veh
        ids_veh = [id_veh, ]
        id_veh = self.ids_follower[id_veh]
        # print '  id_veh',id_veh
        while id_veh > -1:
            ids_veh.append(id_veh)
            id_veh = self.ids_follower[id_veh]
            print '  id_veh', id_veh

        print '   ids_veh', ids_veh
        return ids_veh

    def get_entered_left(self, id_edge_sumo, ids_veh_previous_sumo):
        """
        Returns:
            array with SUMO IDs of entered vehicles during last poll
            array with SUMO IDs of left vehicles during last poll
            array with SUMO IDs current vehicles on id_edge_sumo
        Attention: in the returned entered/left lists,
        the first vehicle in the list entered/left first 
        """
        ids_veh_new_sumo = traci.edge.getLastStepVehicleIDs(id_edge_sumo)
        # print 'get_entered_left ids_veh_new_sumo=',ids_veh_new_sumo
        len_prev = len(ids_veh_previous_sumo)
        len_new = len(ids_veh_new_sumo)

        if len_prev == 0:
            return ids_veh_new_sumo, [], ids_veh_new_sumo

        if len_new == 0:
            return [], ids_veh_previous_sumo, []

        ind_enter = 0

        for id_veh_sumo in ids_veh_new_sumo:

            # if ind_enter+1 == len_prev:
            #    ind_enter = len_new -1
            #    break

            if id_veh_sumo == ids_veh_previous_sumo[0]:
                break

            ind_enter += 1
        # print '  ind_enter',ind_enter,ids_veh_new_sumo[0:ind_enter],ids_veh_new_sumo[ind_enter-1::-1]
        #ids_entered_sumo = ids_veh_new_sumo[0:ind_enter]

        ind_leave = len_prev
        for id_veh_sumo in ids_veh_previous_sumo[::-1]:

            if id_veh_sumo == ids_veh_new_sumo[-1]:
                break

            ind_leave -= 1
        # print '  ind_leave',ind_leave, ids_veh_previous_sumo[ind_leave:],ids_veh_previous_sumo[:ind_leave:-1]
        #ids_leave_sumo = ids_veh_previous_sumo[ind_leave:]

        # return ids_entered_sumo, ids_leave_sumo, ids_veh_new_sumo
        # return reversed lists
        return ids_veh_new_sumo[0:ind_enter][::-1], ids_veh_previous_sumo[ind_leave:][::-1], ids_veh_new_sumo

        # TODO: optimize!!!
        # return ids_veh_new_sumo[ind_enter-1::-1], ids_veh_previous_sumo[:ind_leave:-1], ids_veh_new_sumo

    def control_stop(self, id_veh, laneindex=0):

        id_veh_sumo = self.get_id_sumo(id_veh)
        speed = traci.vehicle.getSpeed(id_veh_sumo)
        pos = traci.vehicle.getLanePosition(id_veh_sumo)
        stopline = pos + 3.0 + 0.5/self.decel*speed**2
        #time_slowdown = np.abs((speed0-speed)/self.decel)

        print 'control_stop', id_veh_sumo, 'v = %.2f at pos %.1fm to stop at %.1fm on %s' % (speed, pos, stopline, traci.vehicle.getRoadID(id_veh_sumo))
        traci.vehicle.setStop(id_veh_sumo,
                              traci.vehicle.getRoadID(id_veh_sumo),
                              pos=stopline,
                              laneIndex=laneindex,
                              )

    def control_speedup(self, id_veh):

        id_veh_sumo = self.get_id_sumo(id_veh)
        print 'control_speedup', id_veh_sumo, 'isStopped', traci.vehicle.isStopped(id_veh_sumo), self.speed_max

        if traci.vehicle.isStopped(id_veh_sumo):
            traci.vehicle.resume(id_veh_sumo)

        traci.vehicle.setMaxSpeed(id_veh_sumo, self.speed_max)
        #self.control_slow_down(id_veh, self.speed_max)

    def control_slow_down(self, id_veh, speed=1.0, time_slowdown=None):
        print 'control_slow_down', self.get_id_sumo(id_veh), speed, time_slowdown
        id_veh_sumo = self.get_id_sumo(id_veh)
        if time_slowdown is None:
            speed0 = traci.vehicle.getSpeed(id_veh_sumo)

            time_slowdown = np.abs((speed0-speed)/self.decel)
            # print '    speed0=%.2fm/s, time_slowdown = %.2fs, dv=%.2fm/s'%(speed0,time_slowdown,speed0-speed)

        traci.vehicle.slowDown(id_veh_sumo, speed, time_slowdown)
        #self.speed_max = vtypes.speeds_max[id_vtype]
        #self.accel = vtypes.accels[id_vtype]
        #self.decel = vtypes.decels[id_vtype]

    def control_stop_alight(self, id_veh, id_stop, id_berth,
                            id_edge_sumo=None,
                            position=None,
                            ):
        id_veh_sumo = self.get_id_sumo(id_veh)
        p = traci.vehicle.getLanePosition(id_veh_sumo)
        print 'control_stop_alight', id_veh_sumo, p, '->', position, 'id_berth', id_berth
        #d = position - p
        #v = traci.vehicle.getSpeed(id_veh_sumo)
        #d_save = 1.0/(2*2.5)*(v**2)
        # print '  v=',v
        # print '  d,d_save',d,d_save
        self.states[id_veh] = VEHICLESTATES['forewarding']
        self.ids_berth[id_veh] = id_berth
        traci.vehicle.setStop(self.get_id_sumo(id_veh),
                              id_edge_sumo,
                              pos=position,
                              flags=0,
                              laneIndex=1,
                              )

    def control_stop_board(self, id_veh, id_stop, id_berth,
                           id_edge_sumo=None,
                           position=None,
                           ):

        id_veh_sumo = self.get_id_sumo(id_veh)
        print 'control_stop_board', id_veh_sumo, id_stop, id_berth, id_edge_sumo, 'pos=%.2f,target %.2f' % (traci.vehicle.getLanePosition(id_veh_sumo), position)
        # print '  v=',traci.vehicle.getSpeed(id_veh_sumo)

        # print 'control_stop_board',id_veh_sumo,traci.vehicle.getLanePosition(id_veh_sumo),'->',position,id_berth
        self.ids_berth[id_veh] = id_berth
        self.states[id_veh] = VEHICLESTATES['forewarding']
        # print '  StopState=',bin(traci.vehicle.getStopState(id_veh_sumo ))[2:]
        if traci.vehicle.isStopped(id_veh_sumo):
            traci.vehicle.resume(id_veh_sumo)

        traci.vehicle.setStop(id_veh_sumo,
                              id_edge_sumo,
                              startPos=position-4.0,
                              pos=position,
                              flags=2,  # park and trigger 1+2,#
                              laneIndex=1,
                              )

    def alight(self, id_veh):
        # print 'alight',self.get_id_sumo(id_veh)
        # TODO: necessary to keep copy of state?
        self.states[id_veh] = VEHICLESTATES['alighting']
        # traci.vehicle.getStopState(self.get_id_sumo(id_veh))
        # VEHICLESTATES = {'init':0,'waiting':1,'boarding':2,'alighting'

    def board(self, id_veh, id_edge_sumo=None, position=None):
        # print 'board',self.get_id_sumo(id_veh)
        # TODO: necessary to keep copy of state?
        self.states[id_veh] = VEHICLESTATES['boarding']
        #id_veh_sumo = self.get_id_sumo(id_veh)
        # print 'board',id_veh_sumo,'stopstate',bin(traci.vehicle.getStopState(id_veh_sumo ))[2:]
        # print '  ',dir(traci.vehicle)
        # traci.vehicle.getLastStepPersonIDs()
        # traci.vehicle.getStopState(self.get_id_sumo(id_veh))
        # VEHICLESTATES = {'init':0,'waiting':1,'boarding':2,'alighting'
        #traci.vehicle.setRoute(id_veh_sumo, [id_edge_sumo])
        # traci.vehicle.resume(id_veh_sumo)

        # traci.vehicle.setStop(  self.get_id_sumo(id_veh),
        #                        traci.vehicle.getRoadID(id_veh_sumo),
        #                        pos = traci.vehicle.getLanePosition(id_veh_sumo),
        #                        flags= 2,#
        #                        laneIndex= 1,
        #                        )
        # print 'board ',id_veh_sumo, traci.vehicle.getStopState(id_veh_sumo )# bin(traci.vehicle.getStopState(id_veh_sumo ))[2:]

    def set_stop(self, id_veh, id_edge_sumo, stopline, laneindex=1):
        print 'set_stop', self.get_id_sumo(id_veh), stopline
        traci.vehicle.setStop(self.get_id_sumo(id_veh),
                              id_edge_sumo,
                              pos=stopline,
                              laneIndex=laneindex,
                              )

    def reached_stop_sumo(self, id_veh_sumo):
        state = traci.vehicle.getStopState(id_veh_sumo)
        # print 'reached_stop',id_veh_sumo,bin(state),bin(state)[-1] == '1'
        return bin(state)[-1] == '1'

    def is_completed_alighting(self, id_veh):
        # print 'is_completed_alighting',self.get_id_sumo(id_veh),self.states[id_veh],self.states[id_veh] == VEHICLESTATES['alighting'],traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)),type(traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)))
        if self.states[id_veh] == VEHICLESTATES['alighting']:
            if traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)) == 0:
                # print '  id_veh_sumo',self.get_id_sumo(id_veh),'completed alighting'
                self.states[id_veh] = VEHICLESTATES['await_forwarding']
                return True
            else:
                # somebody is still in the vehicle
                return False
        elif self.states[id_veh] == VEHICLESTATES['await_forwarding']:
            return True
        else:
            print 'WARNING: strange vehicle state %s while alighting prt.%d' % (self.states[id_veh], id_veh)
            return True

    def is_completed_boarding(self, id_veh):
        # print 'is_completed_boarding',self.get_id_sumo(id_veh),self.states[id_veh],self.states[id_veh] == VEHICLESTATES['boarding'],traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)),type(traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)))
        if self.states[id_veh] == VEHICLESTATES['boarding']:
            if traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)) == 1:
                # print 'is_completed_boarding',self.get_id_sumo(id_veh),'completed boarding'
                self.states[id_veh] = VEHICLESTATES['waiting']
                return True
            else:
                False

        else:
            return True

    def get_veh_if_completed_boarding(self, id_veh):
        # print 'get_veh_if_completed_boarding',self.get_id_sumo(id_veh),self.states[id_veh],self.states[id_veh] == VEHICLESTATES['boarding'],traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)),type(traci.vehicle.getPersonNumber(self.get_id_sumo(id_veh)))

        if self.states[id_veh] == VEHICLESTATES['boarding']:
            id_veh_sumo = self.get_id_sumo(id_veh)
            if traci.vehicle.getPersonNumber(id_veh_sumo) >= 1:
                # print 'get_veh_if_completed_boarding',id_veh_sumo,'completed boarding'
                self.states[id_veh] = VEHICLESTATES['waiting']
                return id_veh_sumo
            else:
                return ''

        else:
            return ''

    def init_trip_occupied(self, id_veh, id_edge_sumo, stopline=None):
        id_veh_sumo = self.get_id_sumo(id_veh)
        print 'init_trip_occupied', self.get_id_sumo(id_veh), 'from edge', id_edge_sumo, stopline
        # print '  current route:',traci.vehicle.getRoute(id_veh_sumo)
        self.states[id_veh] = VEHICLESTATES['occupiedtrip']

        # print '  StopState=',bin(traci.vehicle.getStopState(id_veh_sumo ))[2:]
        if traci.vehicle.isStopped(id_veh_sumo):
            traci.vehicle.resume(id_veh_sumo)
        #traci.vehicle.changeTarget(self.get_id_sumo(id_veh), id_edge_sumo_dest)
        #traci.vehicle.changeTarget(self.get_id_sumo(id_veh), id_edge_sumo_dest)
        if stopline is not None:
            traci.vehicle.setStop(id_veh_sumo,
                                  id_edge_sumo,
                                  pos=stopline,
                                  laneIndex=1,
                                  )
        else:
            speed_crawl = 1.0
            time_accel = 4.0
            #traci.vehicle.slowDown(id_veh_sumo, speed_crawl, time_accel)

    def init_trip_empty(self, id_veh, id_edge_sumo, stopline=None):
        print 'Vehicles.init_trip_empty', self.get_id_sumo(id_veh), id_edge_sumo, stopline
        self.states[id_veh] = VEHICLESTATES['emptytrip']
        id_veh_sumo = self.get_id_sumo(id_veh)
        if traci.vehicle.isStopped(id_veh_sumo):
            traci.vehicle.resume(id_veh_sumo)
        #traci.vehicle.changeTarget(id_veh_sumo, id_edge_sumo_to)
        if stopline is not None:
            # if stopline>=0:
            # print '  Route=',traci.vehicle.getRoute(id_veh_sumo)
            # print '  Position=',traci.vehicle.getLanePosition(id_veh_sumo),stopline
            # print '  StopState=',bin(traci.vehicle.getStopState(id_veh_sumo ))[2:]

            traci.vehicle.setStop(id_veh_sumo,
                                  id_edge_sumo,
                                  pos=stopline,
                                  laneIndex=1,
                                  )
        else:
            speed_crawl = 1.0
            time_accel = 4.0
            #traci.vehicle.slowDown(id_veh_sumo, speed_crawl, time_accel)

    def reschedule_trip_sumo(self, id_veh_sumo, id_edge_sumo_to=None, route_sumo=None):
        print 'reschedule_trip_sumo', id_veh_sumo, id_edge_sumo_to, route_sumo
        if traci.vehicle.isStopped(id_veh_sumo):
            traci.vehicle.resume(id_veh_sumo)

        if route_sumo is not None:
            # set entire route
            traci.vehicle.setRoute(id_veh_sumo, route_sumo)
        else:
            # set new target and let SUMO do the routing
            traci.vehicle.changeTarget(id_veh_sumo, id_edge_sumo_to)

        #
        #traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)
        # self.reset_speedmode(id_veh_sumo)
        #traci.vehicle.slowDown(id_veh_sumo, 13.0, 5.0)

    def reschedule_trip(self, id_veh, id_edge_sumo_to=None, route_sumo=None):
        print 'reschedule_trip', self.get_id_sumo(id_veh), id_edge_sumo_to, route_sumo
        id_veh_sumo = self.get_id_sumo(id_veh)
        if traci.vehicle.isStopped(id_veh_sumo):
            traci.vehicle.resume(id_veh_sumo)

        if route_sumo is not None:
            # set entire route
            traci.vehicle.setRoute(id_veh_sumo, route_sumo)
        else:
            # set new target and let SUMO do the routing
            traci.vehicle.changeTarget(id_veh_sumo, id_edge_sumo_to)

        #
        # self.reset_speedmode(id_veh_sumo)
        # limit speed to run slowly on exit line
        # the merge will take over speed control

        #traci.vehicle.setMaxSpeed(id_veh_sumo, 6.0/3.6)
        # print '    set speed to',traci.vehicle.getMaxSpeed(id_veh_sumo)
        #traci.vehicle.slowDown(id_veh_sumo, 13.0, 5.0)

    def add_to_net(self, n=-1, length_veh_av=4.0):
        """
        Add n PRT vehicles to network
        If n = -1 then fill up stops with vehicles.
        """
        # print 'PrtVehicles.make',n,length_veh_av
        # self.clear()
        net = self.get_scenario().net
        ptstops = net.ptstops
        prtstops = self.parent.prtstops
        lanes = net.lanes
        ids_prtstop = prtstops.get_ids()
        ids_ptstop = prtstops.ids_ptstop[ids_prtstop]
        ids_veh = []
        n_stop = len(prtstops)

        for id_prt, id_edge, pos_from, pos_to in zip(
            ids_prtstop,
            lanes.ids_edge[ptstops.ids_lane[ids_ptstop]],
            ptstops.positions_from[ids_ptstop],
            ptstops.positions_to[ids_ptstop],
        ):
            # TODO: here we can select depos or distribute a
            # fixed number of vehicles or put them into berth
            # print '  ',pos_to,pos_from,int((pos_to-pos_from)/length_veh_av)
            if n > 0:
                n_veh_per_stop = int(float(n)/n_stop+0.5)
            else:
                n_veh_per_stop = int((pos_to-pos_from)/length_veh_av)
            # print '  n,n_stop,n_veh_per_stop',n,n_stop,n_veh_per_stop
            for i in range(n_veh_per_stop):
                id_veh = self.add_row(ids_stop_target=id_prt,
                                      ids_currentedge=id_edge,
                                      )

                self.ids_sumo[id_veh] = self.get_id_sumo(id_veh)
                ids_veh.append(id_veh)

        return ids_veh

    # def write_veh
    #

    def get_scenario(self):
        return self.parent.get_scenario()

    def get_vtypes(self):
        """
        Returns a set with all used PRT vehicle types.
        """
        # print 'Vehicles_individual.get_vtypes',self.cols.vtype
        return set(self.ids_vtype.get_value())

    def get_id_sumo(self, id_veh):
        return 'prt.%s' % (id_veh)

    def get_id_from_id_sumo(self, id_veh_sumo):
        if len(id_veh_sumo.split('.')) == 2:
            prefix, id_veh = id_veh_sumo.split('.')
            if prefix == 'prt':
                return int(id_veh)
            else:
                return -1
        return -1

    def get_ids_from_ids_sumo(self, ids_veh_sumo):
        n = len(ids_veh_sumo)
        ids = np.zeros(n, np.int32)
        for i in xrange(n):
            ids[i] = self.get_id_from_id_sumo(ids_veh_sumo[i])
        return ids

    def get_id_line_xml(self):
        return 'prt'


class PrtStrategy(StrategyMixin):
    def __init__(self, ident, parent=None,
                 name='Personal Rapid Transit Strategy',
                 info='With this strategy, the person uses Personla Rapid Transit as main transport mode.',
                 **kwargs):

        self._init_objman(ident, parent, name=name, info=info, **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))
        # specific init
        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        # print 'StrategyMixin._init_attributes'
        pass

    def _init_constants(self):
        #virtualpop = self.get_virtualpop()
        #stagetables = virtualpop.get_stagetables()

        #self._walkstages = stagetables.get_stagetable('walks')
        #self._ridestages = stagetables.get_stagetable('rides')
        #self._activitystages = stagetables.get_stagetable('activities')

        #self._plans = virtualpop.get_plans()
        #
        # print 'AutoStrategy._init_constants'
        # print dir(self)
        # self.get_attrsman().do_not_save_attrs(['_activitystages','_ridestages','_walkstages','_plans'])

        modes = self.get_virtualpop().get_scenario().net.modes
        self._id_mode_bike = modes.get_id_mode('bicycle')
        self._id_mode_auto = modes.get_id_mode('passenger')
        self._id_mode_moto = modes.get_id_mode('motorcycle')
        self._id_mode_bus = modes.get_id_mode('bus')
        self.get_attrsman().do_not_save_attrs([
            '_id_mode_bike', '_id_mode_auto', '_id_mode_moto', '_id_mode_bus'
        ])

    def set_prtservice(self, prtservice):
        #self.add( cm.ObjConf( prtservice, is_child = False,groups = ['_private']))
        self.prtservice = prtservice
        self.get_attrsman().do_not_save_attrs(['prtservice'])

    def preevaluate(self, ids_person):
        """
        Preevaluation strategies for person IDs in vector ids_person.

        Returns a preevaluation vector with a preevaluation value 
        for each person ID. The values of the preevaluation vector are as follows:
            -1 : Strategy cannot be applied
            0  : Stategy can be applied, but the preferred mode is not used
            1  : Stategy can be applied, and preferred mode is part of the strategy
            2  : Strategy uses predomunantly preferred mode

        """
        n_pers = len(ids_person)
        persons = self.get_virtualpop()
        preeval = np.zeros(n_pers, dtype=np.int32)

        # TODO: here we could exclude by age or distance facilities-stops

        # put 0 for persons whose preference is not public transport
        preeval[persons.ids_mode_preferred[ids_person] != self.prtservice.id_prtmode] = 0

        # put 2 for persons with car access and who prefer cars
        preeval[persons.ids_mode_preferred[ids_person] == self.prtservice.id_prtmode] = 2

        print '  PrtStrategy.preevaluate', len(np.flatnonzero(preeval))
        return preeval

    def plan(self, ids_person, logger=None):
        """
        Generates a plan for these person according to this strategie.
        Overriden by specific strategy.
        """
        print 'PrtStrategy.pan', len(ids_person)
        #make_plans_private(self, ids_person = None, mode = 'passenger')
        # routing necessary?
        virtualpop = self.get_virtualpop()
        scenario = virtualpop.get_scenario()
        plans = virtualpop.get_plans()  # self._plans
        demand = scenario.demand
        #ptlines = demand.ptlines

        walkstages = plans.get_stagetable('walks')
        prtstages = plans.get_stagetable('prttransits')
        activitystages = plans.get_stagetable('activities')

        activities = virtualpop.get_activities()
        activitytypes = demand.activitytypes
        prtstops = self.prtservice.prtstops

        if len(prtstops) == 0:
            # no prt stops, no prt plans
            return True

        net = scenario.net
        edges = net.edges
        lanes = net.lanes
        modes = net.modes
        landuse = scenario.landuse
        facilities = landuse.facilities

        times_est_plan = plans.times_est
        # here we can determine edge weights for different modes

        # this could be centralized to avoid redundance
        plans.prepare_stagetables(['walks', 'prttransits', 'activities'])
        # must be after preparation:
        if self.prtservice.times_stop_to_stop is None:
            self.prtservice.make_times_stop_to_stop()
        times_stop_to_stop = self.prtservice.times_stop_to_stop

        ids_person_act, ids_act_from, ids_act_to\
            = virtualpop.get_activities_from_pattern(0, ids_person=ids_person)

        if len(ids_person_act) == 0:
            print 'WARNING in TrasitStrategy.plan: no eligible persons found.'
            return False

        # temporary maps from ids_person to other parameters
        nm = np.max(ids_person_act)+1
        map_ids_plan = np.zeros(nm, dtype=np.int32)
        #ids_plan_act = virtualpop.add_plans(ids_person_act, id_strategy = self.get_id_strategy())
        map_ids_plan[ids_person_act] = virtualpop.add_plans(ids_person_act, id_strategy=self.get_id_strategy())

        map_times = np.zeros(nm, dtype=np.int32)
        map_times[ids_person_act] = activities.get_times_end(ids_act_from, pdf='unit')

        # set start time to plans (important!)
        plans.times_begin[map_ids_plan[ids_person_act]] = map_times[ids_person_act]

        map_ids_fac_from = np.zeros(nm, dtype=np.int32)
        map_ids_fac_from[ids_person_act] = activities.ids_facility[ids_act_from]

        n_plans = len(ids_person_act)
        print 'TrasitStrategy.plan n_plans=', n_plans

        # make initial activity stage
        ids_edge_from = facilities.ids_roadedge_closest[map_ids_fac_from[ids_person_act]]
        poss_edge_from = facilities.positions_roadedge_closest[map_ids_fac_from[ids_person_act]]
        # this is the time when first activity starts
        # first activity is normally not simulated

        names_acttype_from = activitytypes.names[activities.ids_activitytype[ids_act_from]]
        durations_act_from = activities.get_durations(ids_act_from)
        times_from = map_times[ids_person_act]-durations_act_from
        #times_from = activities.get_times_end(ids_act_from, pdf = 'unit')

        for id_plan,\
            time,\
            id_act_from,\
            name_acttype_from,\
            duration_act_from,\
            id_edge_from,\
            pos_edge_from  \
            in zip(map_ids_plan[ids_person_act],
                   times_from,
                   ids_act_from,
                   names_acttype_from,
                   durations_act_from,
                   ids_edge_from,
                   poss_edge_from):

            id_stage_act, time = activitystages.append_stage(
                id_plan, time,
                ids_activity=id_act_from,
                names_activitytype=name_acttype_from,
                durations=duration_act_from,
                ids_lane=edges.ids_lanes[id_edge_from][0],
                positions=pos_edge_from,
            )

        ##

        ind_act = 0

        # main loop while there are persons performing
        # an activity at index ind_act
        while len(ids_person_act) > 0:
            ids_plan = map_ids_plan[ids_person_act]

            times_from = map_times[ids_person_act]

            names_acttype_to = activitytypes.names[activities.ids_activitytype[ids_act_to]]
            durations_act_to = activities.get_durations(ids_act_to)

            ids_fac_from = map_ids_fac_from[ids_person_act]
            ids_fac_to = activities.ids_facility[ids_act_to]

            centroids_from = facilities.centroids[ids_fac_from]
            centroids_to = facilities.centroids[ids_fac_to]
            dists_from_to = np.sqrt(np.sum((centroids_to - centroids_from)**2, 1))
            dists_walk_max = virtualpop.dists_walk_max[ids_person_act]

            # origin edge and position
            ids_edge_from = facilities.ids_roadedge_closest[ids_fac_from]
            poss_edge_from = facilities.positions_roadedge_closest[ids_fac_from]

            # destination edge and position
            # attention, this methods assumes that all stops are reachable by foot
            ids_edge_to = facilities.ids_roadedge_closest[ids_fac_to]
            poss_edge_to = facilities.positions_roadedge_closest[ids_fac_to]

            # find closest prt stop!!
            ids_stop_from, ids_stopedge_from = prtstops.get_closest(centroids_from)
            ids_stop_to, ids_stopedge_to = prtstops.get_closest(centroids_to)

            poss_stop_from = prtstops.get_waitpositions(ids_stop_from,  offset=-0.5)
            poss_stop_to = prtstops.get_waitpositions(ids_stop_to, offset=-0.5)

            i = 0.0
            for id_person, id_plan, time_from, id_act_from, id_act_to, name_acttype_to, duration_act_to, id_edge_from, pos_edge_from, id_edge_to, pos_edge_to, id_stop_from, id_stopedge_from, pos_stop_from, id_stop_to, id_stopedge_to, pos_stop_to, dist_from_to, dist_walk_max\
                    in zip(ids_person_act, ids_plan, times_from, ids_act_from, ids_act_to, names_acttype_to, durations_act_to,  ids_edge_from, poss_edge_from, ids_edge_to, poss_edge_to, ids_stop_from, ids_stopedge_from, poss_stop_from, ids_stop_to, ids_stopedge_to, poss_stop_to, dists_from_to, dists_walk_max):
                n_pers = len(ids_person_act)
                if logger:
                    logger.progress(i/n_pers*100)
                i += 1.0
                print 79*'_'
                print '  id_plan=%d, id_person=%d, ' % (id_plan, id_person)

                if (dist_from_to < dist_walk_max) | (id_edge_from == -1) | (id_edge_to == -1) | (id_stop_from == id_stop_to):
                    print '    go by foot because distance is too short ', dist_from_to, 'edges', id_edge_from, id_edge_to, 'stops', id_stop_from, id_stop_to

                    id_stage_walk1, time = walkstages.append_stage(
                        id_plan, time_from,
                        id_edge_from=id_edge_from,
                        position_edge_from=pos_edge_from,
                        id_edge_to=id_edge_to,
                        position_edge_to=pos_edge_to,
                    )
                else:
                    id_stage_walk1, time = walkstages.append_stage(id_plan, time_from,
                                                                   id_edge_from=id_edge_from,
                                                                   position_edge_from=pos_edge_from,
                                                                   id_edge_to=id_stopedge_from,
                                                                   position_edge_to=pos_stop_from,  # -7.0,
                                                                   )

                    # take PRT
                    # self.ids_edge_to_ids_prtstop
                    id_stage_transit, time = prtstages.append_stage(
                        id_plan, time,
                        duration=times_stop_to_stop[id_stop_from, id_stop_to],
                        id_fromedge=id_stopedge_from,
                        id_toedge=id_stopedge_to,
                    )

                    # walk from final prtstop to activity
                    # print '    Stage for linktype %2d fromedge %s toedge %s'%(linktype, edges.ids_sumo[id_stopedge_to],edges.ids_sumo[id_edge_to] )
                    id_stage_walk2, time = walkstages.append_stage(id_plan, time,
                                                                   id_edge_from=id_stopedge_to,
                                                                   position_edge_from=pos_stop_to,
                                                                   id_edge_to=id_edge_to,
                                                                   position_edge_to=pos_edge_to,
                                                                   )

                # update time for trips estimation for this plan
                plans.times_est[id_plan] += time-time_from

                # define current end time without last activity duration
                plans.times_end[id_plan] = time

                id_stage_act, time = activitystages.append_stage(
                    id_plan, time,
                    ids_to=id_act_to,
                    names_totype=name_acttype_to,
                    durations=duration_act_to,
                    ids_lane=edges.ids_lanes[id_edge_to][0],
                    positions=pos_edge_to,
                )

                # store time for next iteration in case other activities are
                # following
                map_times[id_person] = time

            # select persons and activities for next setp
            ind_act += 1
            ids_person_act, ids_act_from, ids_act_to\
                = virtualpop.get_activities_from_pattern(ind_act, ids_person=ids_person_act)


class PrtTransits(StageTypeMixin):
    def __init__(self, ident, stages,
                 name='Ride on PRT',
                 info='Ride on Personal Rapid Transit network.',
                 # **kwargs,
                 ):

        print 'PrtTransits.__init__', ident, stages
        self.init_stagetable(ident,
                             stages, name=name,
                             info=info,
                             )
        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        edges = self.get_virtualpop().get_net().edges

        self.add_col(am.IdsArrayConf('ids_fromedge', edges,
                                     groupnames=['parameters'],
                                     name='Edge ID from',
                                     info='Edge ID of departure PRT station.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_toedge', edges,
                                     groupnames=['parameters'],
                                     name='Edge ID to',
                                     info='Edge ID of destination PRT station.',
                                     ))

    def set_prtservice(self, prtservice):
        self.add(cm.ObjConf(prtservice, is_child=False, groups=['_private']))

    def get_prtservice(self):
        return self.prtservice.get_value()

    def prepare_planning(self):

        prtservice = self.get_prtservice()
        print 'prttransits.prepare_planning', prtservice.times_stop_to_stop
        if prtservice.times_stop_to_stop is None:
            prtservice.make_times_stop_to_stop()
        print prtservice.times_stop_to_stop

    def append_stage(self, id_plan, time_start=-1.0,
                     duration=0.0,
                     id_fromedge=-1, id_toedge=-1, **kwargs):
        """
        Appends a PRT transit stage to plan id_plan.

        """
        # print 'PrtTransits.append_stage',id_stage

        id_stage, time_end = StageTypeMixin.append_stage(self,
                                                         id_plan,
                                                         time_start,
                                                         durations=duration,
                                                         ids_fromedge=id_fromedge,
                                                         ids_toedge=id_toedge,
                                                         )

        # add this stage to the vehicle database
        # ind_ride gives the index of this ride (within the same plan??)
        #ind_ride = self.parent.get_individualvehicles().append_ride(id_veh, id_stage)
        return id_stage, time_end

    def to_xml(self, id_stage, fd, indent=0):
        # <ride from="1/3to0/3" to="0/4to1/4" lines="train0"/>
        net = self.get_virtualpop().get_net()
        #ids_stoplane = net.ptstops.ids_lane
        #ids_laneedge = net.lanes.ids_edge
        ids_sumoedge = net.edges.ids_sumo

        #ind = self.get_ind(id_stage)
        fd.write(xm.start('ride', indent=indent))
        fd.write(xm.num('from', ids_sumoedge[self.ids_fromedge[id_stage]]))
        fd.write(xm.num('to', ids_sumoedge[self.ids_toedge[id_stage]]))
        fd.write(xm.num('lines', 'prt'))
        # if self.cols.pos_edge_from[ind]>0:
        #    fd.write(xm.num('departPos', self.cols.pos_edge_from[ind]))
        # if self.cols.pos_edge_to[ind]>0:
        #    fd.write(xm.num('arrivalPos', self.cols.pos_edge_to[ind]))

        fd.write(xm.stopit())  # ends stage


class VehicleMan(am.ArrayObjman):
    def __init__(self, ident, prtservices, **kwargs):
        self._init_objman(ident=ident,
                          parent=prtservices,
                          name='PRT vehicle management',
                          info='PRT vehicle management.',
                          #xmltag = ('additional','busStop','stopnames'),
                          version=0.0,
                          **kwargs)

        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        self.add(cm.AttrConf('time_update', 2.0,
                             groupnames=['parameters'],
                             name='Man. update time',
                             info="Update time for vehicle management.",
                             unit='s',
                             ))

        # self.time_update.set(5.0)

        self.add(cm.AttrConf('time_update_flows', 10.0,
                             groupnames=['parameters'],
                             name='Flow update time',
                             info="Update time for flow estimations.",
                             unit='s',
                             ))
        # self.time_update.set(10.0)
        # if hasattr(self,'time_flowaverage'):
        #    self.delete('time_flowaverage')

        self.add(cm.AttrConf('time_est_max', 1200,
                             groupnames=['parameters'],
                             name='prediction interval',
                             info="Prediction time range of vehicle management.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('time_search_occupied', 10.0,
                             groupnames=['parameters'],
                             name='Occupied search time',
                             info="Time interval for search of optimum departure time in case of an occupied vehicle trip.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('time_search_empty', 20.0,
                             groupnames=['parameters'],
                             name='Empty search time',
                             info="Time interval for search of optimum departure time in case of an empty vehicle trip.",
                             unit='s',
                             ))

        self.add(cm.AttrConf('weight_demand', 0.01,
                             groupnames=['parameters'],
                             name='Demand weight',
                             info="Weight of current demand at stations when assigning empty vehicles trips.",
                             ))

        self.add(cm.AttrConf('weight_flow', 1.0,
                             groupnames=['parameters'],
                             name='Flow weight',
                             info="Weight of flows (changes in demand over time) at stations when assigning empty vehicles trips.",
                             ))

        self.add(cm.AttrConf('constant_timeweight', 0.005,
                             groupnames=['parameters'],
                             name='Time weight const.',
                             info="Constant for the exponential decay function weighting the time instance in the optimization function.",
                             unit='1/s',
                             ))

    # def set_stops(self,vehicleman):
    #    self.add( cm.ObjConf( stops, is_child = False,groups = ['_private']))

    def _init_constants(self):
        self.do_not_save_attrs([
            'ids_stop', 'numbers_veh_arr', 'n_est_max',
            'inflows_sched', 'inflows_person', 'inflows_person_last',
            'ids_veh', 'ids_stop_target', 'ids_stop_current',
            'times_order', 'occupiedtrips_new', 'emptytrips_new',
            'are_emptytrips',
            'log_inflows_temp',
        ])

    def get_stops(self):
        return self.parent.prtstops

    def get_vehicles(self):
        return self.parent.prtvehicles

    def get_scenario(self):
        return self.parent.parent.get_scenario()

    def prepare_sim(self, process):
        print 'VehicleMan.prepare_sim'
        net = self.get_scenario().net
        # station management
        # self.ids_stop_to_ids_edge_sumo = np.zeros(np.max(ids)+1,dtype = np.object)

        self.ids_stop = self.get_stops().get_ids()

        if len(self.ids_stop) == 0:
            return []

        n_stoparray = np.max(self.ids_stop)+1
        self.numbers_veh_arr = np.zeros(n_stoparray, dtype=np.int32)

        #self.n_est_max = self.time_est_max.get_value()/self.time_update_flows.get_value()
        self.n_est_max = int(self.time_est_max.get_value()/self.time_update_flows.get_value())
        self.inflows_sched = np.zeros((n_stoparray, self.n_est_max), dtype=np.int32)
        self.inflows_person = np.zeros(n_stoparray, dtype=np.int32)
        self.inflows_person_last = np.zeros(n_stoparray, dtype=np.int32)
        # vehicle management
        self.ids_veh = self.get_vehicles().get_ids()

        if len(self.ids_veh) == 0:
            print 'WARNING: no PRT vehicles, please add PRT vehicles.'
            return []

        n_veharray = np.max(self.ids_veh)+1

        # could go in vehicle array
        self.ids_stop_target = -1*np.ones(n_veharray, dtype=np.int32)
        self.ids_stop_current = -1*np.ones(n_veharray, dtype=np.int32)
        self.times_order = 10**6*np.ones(n_veharray, dtype=np.int32)
        # self.occupiedtrips_new = []#?? put this is also in general time scheme with more urgency??
        #self.emptytrips_new = []

        # from veh arrays
        #self.are_emptytrips = np.zeros(n_veharray, dtype = np.bool)

        # logging
        results = process.get_results()
        if results is not None:
            time_update_flows = self.time_update_flows.get_value()
            n_timesteps = int((process.duration-process.time_warmup)/time_update_flows+1)
            results.prtstopresults.init_recording(n_timesteps, time_update_flows)

        self.log_inflows_temp = np.zeros(n_stoparray, dtype=np.int32)

        return [(self.time_update.get_value(), self.process_step),
                (self.time_update_flows.get_value(), self.update_flows),
                ]

    def update_flows(self, process):
        # print 'update flow prediction'
        self.inflows_sched[:, 0] = 0
        self.inflows_sched = np.roll(self.inflows_sched, -1)
        time_update_flows = self.time_update_flows.get_value()

        if process.simtime > process.time_warmup:
            stops = self.get_stops()
            ids_stop = self.ids_stop
            prtstopresults = process.get_results().prtstopresults
            const_time = 1.0/time_update_flows  # np.array([1.0/time_update_flows],dtype=np.float32)
            timestep = (process.simtime - process.simtime_start - process.time_warmup)/time_update_flows
            prtstopresults.record(timestep, ids_stop,
                                  inflows_veh=np.array(const_time * self.log_inflows_temp[ids_stop], dtype=np.float32),
                                  inflows_veh_sched=np.array(
                                      const_time * self.inflows_sched[ids_stop, 0], dtype=np.float32),
                                  numbers_person_wait=stops.numbers_person_wait[ids_stop],
                                  waittimes_tot=stops.waittimes_tot[ids_stop],
                                  inflows_person=np.array(const_time * self.inflows_person[ids_stop], dtype=np.float32),
                                  #inflows_person = stops.flows_person[ids_stop],
                                  )

        self.inflows_person_last = self.inflows_person.copy()
        self.inflows_person[:] = 0
        self.log_inflows_temp[:] = 0

    def process_step(self, process):
        print 79*'M'
        print 'VehicleMan.process_step'

        stops = self.get_stops()
        #vehicles = self.get_vehicles()

        has_programmed = True
        ids_veh_lead = stops.ids_veh_lead[self.ids_stop]
        inds_valid = np.flatnonzero(ids_veh_lead >= 0)
        while has_programmed:
            has_programmed = False

            if len(inds_valid) > 0:

                has_programmed |= self.push_occupied_leadvehs(ids_veh_lead[inds_valid], process)
                ids_veh_lead = stops.ids_veh_lead[self.ids_stop]
                inds_valid = np.flatnonzero(ids_veh_lead >= 0)
                # print '   self.ids_stop',self.ids_stop
                # print '   ids_veh_lead',ids_veh_lead
                has_programmed |= self.push_empty_leadvehs(ids_veh_lead[inds_valid], process)
                ids_veh_lead = stops.ids_veh_lead[self.ids_stop]
                inds_valid = np.flatnonzero(ids_veh_lead >= 0)

                #has_programmed |= self.pull_empty_leadvehs(ids_veh_lead[inds_valid],process)
                #ids_veh_lead = stops.ids_veh_lead[self.ids_stop]
                #inds_valid = np.flatnonzero(ids_veh_lead >= 0)
                # print '  main:ids_veh_lead',has_programmed,len(inds_valid), stops.ids_veh_lead[self.ids_stop]
            else:
                # no more leaders
                has_programmed = False
        # print '\n terminated vehicle man n_est_max',self.n_est_max
        # print '  inflows_sched=\n',self.inflows_sched

    def note_person_entered(self, id_stop, id_person_sumo, id_stop_dest):
        # here just estimate person flows
        self.inflows_person[id_stop] += 1

    def push_occupied_leadvehs(self, ids_veh_lead, process):
        n_timeslot_offset = 3
        n_searchint = int(self.time_search_occupied.get_value()/self.time_update_flows.get_value()+0.5)
        stops = self.get_stops()
        vehicles = self.get_vehicles()

        inds_valid = np.flatnonzero(vehicles.states[ids_veh_lead] == VEHICLESTATES['occupiedtrip'])
        if len(inds_valid) == 0:
            return False
        print 'push_occupied_leadvehs ids_veh_lead', ids_veh_lead[inds_valid]
        times_stop_to_stop = self.parent.times_stop_to_stop
        ids_stop_current = self.ids_stop_current[ids_veh_lead[inds_valid]]
        ids_stop_target = self.ids_stop_target[ids_veh_lead[inds_valid]]
        durations_est = times_stop_to_stop[ids_stop_current, ids_stop_target]
        # print '  duration_est, t_est_max',durations_est,self.n_est_max*self.time_update_flows.get_value()

        inds_time_min = n_timeslot_offset+np.array(np.clip(np.array(1.0*durations_est/self.time_update_flows.get_value(
        ), dtype=np.int32), 0, self.n_est_max-n_searchint-n_timeslot_offset), dtype=np.int32)
        inds_time_max = inds_time_min+n_searchint
        # print '  inds_time_min unclipped',n_timeslot_offset+np.array(1.0*durations_est/self.time_update_flows.get_value(),dtype=np.int32)
        # print '  inds_time_min   clipped',inds_time_min
        # print '  inds_time_max',inds_time_max, self.inflows_sched.shape

        for id_veh_lead, state, id_stop_current, id_stop_target, time_order, ind_min, ind_max, duration_est\
                in zip(
                    ids_veh_lead[inds_valid],
                    vehicles.states[ids_veh_lead[inds_valid]],
                    self.ids_stop_current[ids_veh_lead[inds_valid]],
                    self.ids_stop_target[ids_veh_lead[inds_valid]],
                    self.times_order[ids_veh_lead[inds_valid]],
                    inds_time_min,
                    inds_time_max,
                    durations_est,
                ):

            print '    check veh', id_veh_lead, state, 'id_stop_current', id_stop_current, 'id_stop_target', id_stop_target

            #VEHICLESTATES = {'init':0,'waiting':1,'boarding':2,'alighting':3,'emptytrip':4,'occupiedtrip':5,'forewarding':6}
            # if state == VEHICLESTATES['occupiedtrip']:

            #ids_stop = list(self.ids_stop)
            # ids_stop.remove(id_stop)
            #costs = (stops.numbers_person_wait[ids_stop]-stops.numbers_veh[ids_stop]-self.numbers_veh_arr[ids_stop])/self.parent.times_stop_to_stop[id_stop,ids_stop]
            #ids_stop [np.argmax(costs)]
            inds_time = np.arange(ind_min, ind_max)
            costs = self.inflows_sched[id_stop_target, inds_time]
            # print '    inds_time',inds_time
            # print '    inflows_sched',costs
            ind_time_depart = inds_time[np.argmin(costs)]

            time_depart = process.simtime + ind_time_depart*self.time_update_flows.get_value()-duration_est
            # print '      ind_time_depart, time_arr, duration_est',ind_time_depart,process.simtime + ind_time_depart*self.time_update_flows.get_value(),duration_est
            # print '      time_order' ,time_order,'time_depart',time_depart,'delay',time_depart-time_order

            stops.program_leadveh(id_stop_current, id_veh_lead, id_stop_target, time_depart)
            self.numbers_veh_arr[id_stop_target] += 1
            self.inflows_sched[id_stop_target, ind_time_depart] += 1

        return True  # at least one vehicle assigned

    def push_empty_leadvehs(self, ids_veh_lead, process):
        n_timeslot_offset = 3
        stops = self.get_stops()
        vehicles = self.get_vehicles()
        inds_valid = np.flatnonzero(vehicles.states[ids_veh_lead] == VEHICLESTATES['emptytrip'])
        if len(inds_valid) == 0:
            return False

        print 'push_empty_leadvehs ids_veh_lead', ids_veh_lead[inds_valid]
        times_stop_to_stop = self.parent.times_stop_to_stop
        ids_stop_current = self.ids_stop_current[ids_veh_lead[inds_valid]]

        n_stop_target = len(self.ids_stop)
        ids_stop_target = self.ids_stop.reshape(n_stop_target, 1)
        flow_person_est = (stops.flows_person[ids_stop_target] *
                           self.time_update_flows.get_value()).reshape(n_stop_target, 1)
        n_searchint = int(self.time_search_empty.get_value()/self.time_update_flows.get_value()+0.5)
        inds_search_base = n_timeslot_offset + \
            np.arange(n_searchint, dtype=np.int32) * np.ones((n_stop_target, n_searchint), dtype=np.int32)

        #self.numbers_veh = np.zeros(np.max(ids)+1, dtype = np.int32)
        #self.numbers_person_wait = np.zeros(np.max(ids)+1, dtype = np.int32)
        #self.flows_person = np.zeros(np.max(ids)+1, dtype = np.float32)
        #self.ids_veh_lead = -1*np.ones(np.max(ids)+1,dtype = np.int32)
        #self.ids_vehs_prog = np.zeros(np.max(ids)+1,dtype = np.object)
        is_started = False
        for id_veh_lead, id_stop_current, time_order, \
                in zip(
                    ids_veh_lead[inds_valid],
                    self.ids_stop_current[ids_veh_lead[inds_valid]],
                    self.times_order[ids_veh_lead[inds_valid]],
                ):

            #id_stop_target = self.get_stop_emptytrip(id_stop_current)

            durations_est = times_stop_to_stop[id_stop_current, ids_stop_target]
            ind_stop_current = np.flatnonzero(durations_est == 0)[0]
            print '    check veh', id_veh_lead, 'id_stop_current', id_stop_current, 'ind_stop_current', ind_stop_current
            inds_time_min = np.array(np.clip(np.array(1.0*durations_est/self.time_update_flows.get_value(),
                                                      dtype=np.int32), 0, self.n_est_max-n_searchint), dtype=np.int32)
            inds_search = inds_search_base + inds_time_min.reshape(n_stop_target, 1)
            timeweight = np.exp(-self.constant_timeweight.get_value() * inds_search*self.time_update_flows.get_value())

            # print '  demand',stops.numbers_person_wait[ids_stop_target]

            costs = (self.weight_demand.get_value() * stops.waittimes_tot[ids_stop_target]
                     + self.weight_flow.get_value() * (flow_person_est-self.inflows_sched[ids_stop_target, inds_search])
                     ) * timeweight
            # costs = (   self.weight_demand.get_value() * (  stops.numbers_person_wait[ids_stop_target])\
            #            + self.weight_flow.get_value() * (flow_person_est-self.inflows_sched[ids_stop_target, inds_search])\
            #         )* timeweight

            #costs = np.exp(+self.inflows_sched[ids_stop_target, inds_search]-flow_person_est)*timeweight
            #costs = (self.inflows_sched[ids_stop_target, inds_search]-flow_person_est)*timeweight
            costs[ind_stop_current, :] = -999999
            if 0:
                for ind, id_stop in zip(range(n_stop_target), ids_stop_target):
                    print 40*'.'
                    print '  id_stop', id_stop
                    print '     waittimes_tot', stops.waittimes_tot[id_stop]
                    print '     numbers_person_wait', stops.numbers_person_wait[id_stop]
                    print '     flow_person_est', flow_person_est[ind]
                    print '     inflows_sched', self.inflows_sched[id_stop, inds_search[ind]]
                    print '     delta flow', (flow_person_est[ind]-self.inflows_sched[id_stop, inds_search[ind]])
                    print '     demandcomp', self.weight_demand.get_value() * stops.numbers_person_wait[id_stop]
                    print '     flowcomp', self.weight_flow.get_value() * (flow_person_est[ind]-self.inflows_sched[id_stop, inds_search[ind]])
                    print '     arrivaltime est', inds_search[ind]*self.time_update_flows.get_value()
                    print '     flow_person_est', flow_person_est[ind]
                    print

                    # print '    flow_person_est',flow_person_#est
                    print '    timeweight', timeweight[ind]
                    print '    durations_est', durations_est[ind]
                    print '    inds_search_base', inds_search_base[ind]
                    # print '    inds_search unclipped\n',inds_search_base +np.array(1.0*durations_est/self.time_update_flows.get_value(),dtype=np.int32).reshape(n_stop_target,1)
                    print '    inds_search clipped  \n', inds_search[ind]
                    print '    inds_time_min', inds_time_min[ind]
                    # print '    ind_stop_current',ind_stop_current,durations_est[ind_stop_current]
                    print '    costs=\n', costs[ind]
            # constant_timeweight

            #
            ind_target = np.argmax(costs)
            ind_stop_target = ind_target/n_searchint
            #ind_time_arrive = ind_target%n_searchint+inds_time_min[ind_stop_target]
            ind_time_delta = ind_target % n_searchint
            ind_time_arrive = inds_search[ind_stop_target, ind_time_delta]
            if 0:
                print '    ind_target,n_searchint,ind_stop_target,ind_time_delta', ind_target, n_searchint, ind_target/n_searchint, ind_time_delta
                print '    ind_delta_depart,c_min', costs[ind_stop_target, ind_time_delta]
                print '    inds_time_min,ind_time_arrive', inds_time_min[ind_stop_target], ind_time_arrive

            id_stop_target = ids_stop_target[ind_stop_target][0]
            time_depart = process.simtime + ind_time_arrive * \
                self.time_update_flows.get_value()-durations_est[ind_stop_target]
            # print '\n     id_stop_target',id_stop_target
            # print '     time_order' ,time_order,'time_depart',time_depart,'delay',time_depart-time_order

            is_started = stops.program_leadveh(id_stop_current, id_veh_lead, id_stop_target, time_depart)
            if is_started:
                self.numbers_veh_arr[id_stop_target] += 1
                self.inflows_sched[id_stop_target, ind_time_arrive] += 1
            else:
                print 'WARNING in push_empty_leadvehs: no vehicle prt.%d in stop %d' % (id_veh_lead, id_stop_target)
        return is_started

    def pull_empty_leadvehs(self, ids_veh_lead, process):

        n_timeslot_offset = 2

        #self.numbers_veh = np.zeros(np.max(ids)+1, dtype = np.int32)
        #self.numbers_person_wait = np.zeros(np.max(ids)+1, dtype = np.int32)

        # inds_valid = np.flatnonzero(    (vehicles.states[ids_veh_lead] == VEHICLESTATES['waiting'])\
        #                                & (stops.numbers_person_wait[self.ids_stop_current[ids_veh_lead]] == 0))
        vehicles = self.get_vehicles()
        stops = self.get_stops()
        print 'pull_empty_leadvehs', ids_veh_lead
        # print '  bordingstate',vehicles.states[ids_veh_lead] == VEHICLESTATES['boarding']
        # print '  nowaits stop',stops.numbers_person_wait[self.ids_stop_current[ids_veh_lead]] ==0

        times_stop_to_stop = self.parent.times_stop_to_stop

        # get potential pull vehicles
        inds_valid = np.flatnonzero((vehicles.states[ids_veh_lead] == VEHICLESTATES['boarding'])
                                    & (stops.numbers_person_wait[self.ids_stop_current[ids_veh_lead]] == 0))

        # print '  states',vehicles.states[ids_veh_lead], VEHICLESTATES['boarding']
        # print '  numbers_person_wait',stops.numbers_person_wait[self.ids_stop_current[ids_veh_lead]]
        # print '  &',(vehicles.states[ids_veh_lead] == VEHICLESTATES['boarding'])\
        #                               & (stops.numbers_person_wait[self.ids_stop_current[ids_veh_lead]] == 0)
        # print '   origins inds_valid',inds_valid
        if len(inds_valid) == 0:
            # print '    all stops busy'
            return False

        #ids_veh_lead = ids_veh_lead[inds_valid]
        ids_stop_current = self.ids_stop_current[ids_veh_lead[inds_valid]]

        # print '  available for pulling ids_veh_lead',ids_veh_lead
        # print '    ids_stop_current',ids_stop_current
        # get potential target station with demand
        demands = stops.numbers_person_wait[self.ids_stop]\
            - (0.5*stops.numbers_veh[self.ids_stop]
               + self.numbers_veh_arr[self.ids_stop])
        inds_valid = np.flatnonzero(demands > 0)
        # print '  targets inds_valid',inds_valid
        if len(inds_valid) == 0:
            # print '  no demand'
            return False

        ids_stop_target = self.ids_stop[inds_valid]
        demands = demands[inds_valid]

        print '  ids_stop_current', ids_stop_current
        # print '  ids_stop_target',ids_stop_target
        # print '  demands',demands
        # calculate cost matrix with id_stop_current in rows and id_stop_target

        #n_origin = len(ids_stop_current)
        #n_targets = len(ids_stop_target)
        times = times_stop_to_stop[ids_stop_current, :][:, ids_stop_target]
        times[times == 0] = 99999
        timeweight = np.exp(-self.constant_timeweight.get_value() * times)
        #costs = times_stop_to_stop[ids_stop_current,:][:,ids_stop_target]
        #costs[costs == 0] = 99999
        # print '  timeweight\n',timeweight

        costs = timeweight * demands
        for id_stop_target, demand, number_veh, number_veh_arr\
                in zip(ids_stop_target, demands, stops.numbers_veh[ids_stop_target], self.numbers_veh_arr[ids_stop_target]):
            print '    id_stop_target', id_stop_target, 'dem', demand, 'n_veh', number_veh, 'n_veh_arr', number_veh_arr

        #costs = np.zeros(costs.size, np.float32)-demands

        inds_pull = np.argmax(costs, 1)
        # print '  costs\n',costs
        # print '  ->inds_pull,ids_stop_target',inds_pull,ids_stop_target[inds_pull]

        durations_est = times_stop_to_stop[ids_stop_current, ids_stop_target[inds_pull]]
        # print '  duration_est, t_est_max',durations_est,self.n_est_max*self.time_update_flows.get_value()

        inds_time_min = n_timeslot_offset+np.array(np.clip(np.array(1.0*durations_est/self.time_update_flows.get_value(
        ), dtype=np.int32), 0, self.n_est_max-1-n_timeslot_offset), dtype=np.int32)
        inds_time_max = inds_time_min+2
        # print '  inds_time_min',inds_time_min
        # print '  inds_time_max',inds_time_max

        is_started = False
        for id_veh_lead, state, id_stop_current, id_stop_target, time_order, ind_min, ind_max, duration_est\
                in zip(
                    ids_veh_lead[inds_valid],
                    vehicles.states[ids_veh_lead],
                    ids_stop_current,
                    ids_stop_target[inds_pull],
                    self.times_order[ids_veh_lead],
                    inds_time_min,
                    inds_time_max,
                    durations_est,
                ):

            print '    check veh prt.%d' % (id_veh_lead), state, 'id_stop_current', id_stop_current, 'id_stop_target', id_stop_target

            #VEHICLESTATES = {'init':0,'waiting':1,'boarding':2,'alighting':3,'emptytrip':4,'occupiedtrip':5,'forewarding':6}
            # if state == VEHICLESTATES['occupiedtrip']:

            #ids_stop = list(self.ids_stop)
            # ids_stop.remove(id_stop)
            #costs = (stops.numbers_person_wait[ids_stop]-stops.numbers_veh[ids_stop]-self.numbers_veh_arr[ids_stop])/self.parent.times_stop_to_stop[id_stop,ids_stop]
            #ids_stop [np.argmax(costs)]
            inds_time = np.arange(ind_min, ind_max)
            costs = self.inflows_sched[id_stop_current, inds_time]
            ind_time_depart = inds_time[np.argmin(costs)]

            self.inflows_sched[id_stop_target, ind_time_depart] += 1
            time_depart = process.simtime + ind_time_depart*self.time_update_flows.get_value()-duration_est

            # this is a workarouned that vehicle does first reach the stopline
            # before it gets rescheduled...try with stoplinecheck

            # if time_depart < process.simtime+15:
            #    time_depart = process.simtime+15
            # print '      ind_time_depart, time_arr, duration_est',ind_time_depart,process.simtime + ind_time_depart*self.time_update_flows.get_value(),duration_est
            # print '      time_order' ,time_order,'time_depart',time_depart,'delay',time_depart-time_order

            stops.program_leadveh(id_stop_current, id_veh_lead, id_stop_target, time_depart)
            self.numbers_veh_arr[id_stop_target] += 1
            is_started = True

        return is_started

    def init_trip_occupied(self, id_veh, id_stop_from, id_stop_to, time_order):
        # print 'init_trip_occupied from',id_veh, id_stop_from, id_stop_to
        # search closest stop
        #self.are_emptytrips[id_veh] = False

        self.ids_stop_current[id_veh] = id_stop_from
        self.ids_stop_target[id_veh] = id_stop_to
        self.times_order[id_veh] = time_order
        # print '  to stop',id_stop
        # return id_stop_to

    def init_trip_empty(self, id_veh, id_stop, time_order):
        # print 'VehMan.init_trip_empty id_veh prt.%d,id_stop %d'%(id_veh,id_stop)
        # search closest stop
        self.ids_stop_current[id_veh] = id_stop
        self.times_order[id_veh] = time_order
        #self.are_emptytrips[id_veh] = True
        #id_stop_target = ids_stop[random.randint(0,len(ids_stop)-1)]
        # print '  to stop',id_stop
        # return id_stop_target

    def indicate_trip_empty(self, id_veh, id_stop, time_order):
        # print 'indicate_trip_empty id_veh,id_stop',id_veh,id_stop
        # search closest stop
        self.ids_stop_current[id_veh] = id_stop
        self.times_order[id_veh] = time_order

    def get_stop_emptytrip(self, id_stop):
        stops = self.get_stops()
        ids_stop = list(self.ids_stop)
        ids_stop.remove(id_stop)
        costs = (stops.numbers_person_wait[ids_stop]-stops.numbers_veh[ids_stop] -
                 self.numbers_veh_arr[ids_stop])/self.parent.times_stop_to_stop[id_stop, ids_stop]

        return ids_stop[np.argmax(costs)]

    def conclude_trip(self, id_veh, id_stop):
        self.ids_stop_target[id_veh] = -1
        self.numbers_veh_arr[id_stop] -= 1

        # measures actually arrived vehicles at stop
        # accumulates counts over one flow measurement interval
        self.log_inflows_temp[id_stop] += 1


class PrtService(SimobjMixin, DemandobjMixin, cm.BaseObjman):
    def __init__(self, ident, simulation=None,
                 name='PRT service', info='PRT service',
                 **kwargs):
        # print 'PrtService.__init__',name

        self._init_objman(ident=ident, parent=simulation,
                          name=name, info=info, **kwargs)

        attrsman = self.set_attrsman(cm.Attrsman(self))

        # make PRTservice a demand object as link
        self.get_scenario().demand.add_demandobject(obj=self)

        self._init_attributes()
        self._init_constants()

    def get_scenario(self):
        return self.parent.parent

    def _init_attributes(self):
        # print 'PrtService._init_attributes',hasattr(self,'prttransit')
        attrsman = self.get_attrsman()
        scenario = self.get_scenario()
        # here we ged classes not vehicle type
        # specific vehicle type within a class will be generated later
        modechoices = scenario.net.modes.names.get_indexmap()

        # print '  modechoices',modechoices
        self.id_prtmode = attrsman.add(am.AttrConf('id_prtmode',  modechoices['custom1'],
                                                   groupnames=['options'],
                                                   choices=modechoices,
                                                   name='Mode',
                                                   info='PRT transport mode (or vehicle class).',
                                                   ))

        self.prtstops = attrsman.add(cm.ObjConf(PrtStops('prtstops', self)))
        self.compressors = attrsman.add(cm.ObjConf(Compressors('compressors', self)))
        self.decompressors = attrsman.add(cm.ObjConf(Decompressors('decompressors', self)))
        self.mergenodes = attrsman.add(cm.ObjConf(Mergenodes('mergenodes', self)))
        self.prtvehicles = attrsman.add(cm.ObjConf(PrtVehicles('prtvehicles', self)))
        self.vehicleman = attrsman.add(cm.ObjConf(VehicleMan('vehicleman', self)))

        #self.demo = attrsman.add( cm.ObjConf(TraciDemo('demo',self)))

        # --------------------------------------------------------------------
        # prt transit table
        # attention: prttransits will be a child of virtual pop,
        # and a link from prt service

        # if not hasattr(self,'prttransit'):
        virtualpop = self.get_scenario().demand.virtualpop
        prttransits = virtualpop.get_plans().add_stagetable('prttransits', PrtTransits)

        # print '  prttransits =',prttransits
        # add attribute as link
        # self.prttransits =  attrsman.add(\
        #                        cm.ObjConf(prttransits,is_child = False ),
        #                        is_overwrite = False,)
        prttransits.set_prtservice(self)

        prtstrategy = virtualpop.get_strategies().add_strategy(
            'prt', PrtStrategy, np.array([208, 50, 156, 230], np.float32)/255)
        # self.prttransits =  attrsman.add(\
        #                        cm.ObjConf(prttransits,is_child = False ),
        #                        is_overwrite = False,)
        prtstrategy.set_prtservice(self)

        # temporary attrfix
        #prtserviceconfig = self.parent.get_attrsman().prtservice
        #prtserviceconfig.groupnames = []
        #prtserviceconfig.add_groupnames(['demand objects'])

    def _init_constants(self):
        # print 'PrtService._init_constants',self,self.parent
        attrsman = self.get_attrsman()
        self.times_stop_to_stop = None
        self.fstar = None
        self._results = None

        attrsman.do_not_save_attrs(['times_stop_to_stop', 'fstar', '_results'
                                    'times_stop_to_stop', 'ids_edge_to_ids_prtstop',
                                    ])

    def get_vtypes(self):

        ids_vtypes = set(self.prtvehicles.ids_vtype.get_value())
        return ids_vtypes

    def get_writexmlinfo(self, is_route=False, is_plain=False, **kwargs):
        """
        Returns three array where the first array is the 
        begin time of the first vehicle and the second array is the
        write function to be called for the respectice vehicle and
        the third array contains the vehicle ids

        Method used to sort trips when exporting to route or trip xml file
        """
        print 'PRT.get_writexmlinfo'

        # time of first PRT vehicle(s) to be inserted
        virtualpop = self.get_scenario().demand.virtualpop
        t_start = virtualpop.get_time_depart_first()

        #t_start = 0.0
        # time betwenn insertion of consecutive vehicles at same stop
        t_delta = 10  # s

        n_veh = len(self.prtvehicles)
        times_depart = np.zeros(n_veh, dtype=np.int32)
        writefuncs = np.zeros(n_veh, dtype=np.object)
        writefuncs[:] = self.write_prtvehicle_xml
        ids_veh = self.prtvehicles.get_ids()

        id_edge_prev = -1
        i = 0
        t0 = t_start
        for id_edge in self.prtvehicles.ids_currentedge[ids_veh]:
            # print '  id_edge, t_start, id_edge_prev',id_edge, t0, id_edge_prev
            times_depart[i] = t0
            t0 += t_delta
            if id_edge != id_edge_prev:
                t0 = t_start
                id_edge_prev = 1*id_edge
            i += 1

        return times_depart, writefuncs, ids_veh

    def write_prtvehicle_xml(self,  fd, id_veh, time_begin, indent=2):
        # print 'write_prtvehicle_xml',id_veh, time_begin
        # TODO: actually this should go in prtvehicles
        #time_veh_wait_after_stop = 3600
        scenario = self.get_scenario()
        net = scenario.net

        #lanes = net.lanes
        edges = net.edges
        #ind_ride = rides.get_inds(id_stage)
        #id_veh = rides.ids_veh[id_stage]
        prtvehicles = self.prtvehicles
        #ptstops = net.ptstops
        #prtstops = self.parent.prtstops
        #ids_prtstop = prtstops.get_ids()
        #ids_ptstop = prtstops.ids_ptstop[id_stop]
        # lanes.ids_edge[ptstops.ids_lane[ids_ptstop]],
        #id_lane_from = parking.ids_lane[id_parking_from]
        #laneindex_from =  lanes.indexes[id_lane_from]
        #pos_from = parking.positions[id_parking_from]

        #id_parking_to = rides.ids_parking_to[id_stage]
        #id_lane_to = parking.ids_lane[id_parking_to]
        #laneindex_to =  lanes.indexes[id_lane_to]
        #pos_to = parking.positions[id_parking_to]

        # write unique veh ID to prevent confusion with other veh declarations
        fd.write(xm.start('vehicle id="%s"' % prtvehicles.get_id_sumo(id_veh), indent+2))

        fd.write(xm.num('depart', '%d' % time_begin))
        fd.write(xm.num('type', scenario.demand.vtypes.ids_sumo[prtvehicles.ids_vtype[id_veh]]))
        fd.write(xm.num('line', prtvehicles.get_id_line_xml()))
        fd.write(xm.stop())

        # write route
        fd.write(xm.start('route', indent+4))
        # print '  edgeindex[ids_edge]',edgeindex[ids_edge]
        fd.write(xm.arr('edges', [edges.ids_sumo[prtvehicles.ids_currentedge[id_veh]]]))

        # does not seem to have an effect, always starts at base????
        fd.write(xm.num('departPos', 'base'))
        #fd.write(xm.num('departLane', laneindex_from ))
        fd.write(xm.stopit())

        # write depart stop
        # fd.write(xm.start('stop',indent+4))
        #fd.write(xm.num('lane', edges.ids_sumo[lanes.ids_edge[id_lane_from]]+'_%d'%laneindex_from ))
        #fd.write(xm.num('duration', time_veh_wait_after_stop))
        #fd.write(xm.num('startPos', pos_from ))
        #fd.write(xm.num('endPos', pos_from + parking.lengths[id_parking_from]))
        #fd.write(xm.num('triggered', "True"))
        # fd.write(xm.stopit())

        fd.write(xm.end('vehicle', indent+2))

    # def make_stops_and_vehicles(self, n_veh = -1):
    #    self.prtstops.make_from_net()
    #    self.prtvehicles.make(n_veh)
    #    self.make_times_stop_to_stop()

    def prepare_sim(self, process):
        print 'prepare_sim', self.ident
        # print '  self.times_stop_to_stop',self.times_stop_to_stop

        if self.fstar is None:
            self.make_fstar()

        if self.times_stop_to_stop is None:
            print '  times_stop_to_stop'
            self.make_times_stop_to_stop()

        updatedata = self.prtvehicles.prepare_sim(process)
        updatedata += self.prtstops.prepare_sim(process)
        updatedata += self.compressors.prepare_sim(process)
        updatedata += self.decompressors.prepare_sim(process)
        updatedata += self.mergenodes.prepare_sim(process)
        updatedata += self.vehicleman.prepare_sim(process)
        #
        #updatedata += self.demo.prepare_sim(process)
        # print 'PrtService.prepare_sim updatedata',updatedata
        return updatedata

    def make_fstar(self, is_update=False):
        if (self.fstar is None) | is_update:
            self.fstar = self.get_fstar()
            self.edgetimes = self.get_times(self.fstar)

    def get_route(self, id_fromedge, id_toedge):
        """
        Centralized function to determin fastest route between
        PRT network edges.
        """
        duration, route = get_mincostroute_edge2edge(id_fromedge, id_toedge,
                                                     weights=self.edgetimes, fstar=self.fstar)

        return route, duration

    def make_times_stop_to_stop(self, fstar=None, times=None):
        print 'make_times_stop_to_stop'
        log = self.get_logger()
        if fstar is None:
            if self.fstar is None:
                self.make_fstar()

            fstar = self.fstar  # get_fstar()
            times = self.edgetimes  # self.get_times(fstar)

        if len(fstar) == 0:
            self.times_stop_to_stop = [[]]
            return

        ids_prtstop = self.prtstops.get_ids()
        if len(ids_prtstop) == 0:
            self.get_logger().w('WARNING: no PRT stops, no plans. Generate them!')
            return

        ids_edge = self.prtstops.get_edges(ids_prtstop)

        # check if all PRT stop edges are in fstar
        ids_ptstop = self.prtstops.ids_ptstop[ids_prtstop]  # for debug only
        is_incomplete_fstar = False
        for id_edge, id_stop, id_ptstop in zip(ids_edge, ids_prtstop, ids_ptstop):
            # print '  Found PRT stop %d, PT stop %d with id_edge %d '%(id_stop,id_ptstop, id_edge)
            if not fstar.has_key(id_edge):
                print 'WARNING in make_times_stop_to_stop: PRT stop %d, PT stop %d has no id_edge %d in fstar' % (id_stop, id_ptstop, id_edge)
                is_incomplete_fstar = True

        # check if fstar is complete (all to edges are in keys)
        ids_fromedge_set = set(fstar.keys())
        ids_sumo = self.get_scenario().net.edges.ids_sumo
        for id_fromedge in ids_fromedge_set:
            if not ids_fromedge_set.issuperset(fstar[id_fromedge]):
                is_incomplete_fstar = True
                ids_miss = fstar[id_fromedge].difference(ids_fromedge_set)
                print 'WARNING in make_times_stop_to_stop: incomplete fstar of id_fromedge = %d, %s' % (id_fromedge, ids_sumo[id_fromedge])
                for id_edge in ids_miss:
                    print '  missing', id_edge, ids_sumo[id_edge]

        if is_incomplete_fstar:
            return

        # print '  ids_prtstop,ids_edge',ids_prtstop,ids_edge
        n_elem = np.max(ids_prtstop)+1
        stop_to_stop = np.zeros((n_elem, n_elem), dtype=np.int32)

        ids_edge_to_ids_prtstop = np.zeros(np.max(ids_edge)+1, dtype=np.int32)
        ids_edge_to_ids_prtstop[ids_edge] = ids_prtstop

        ids_edge_target = set(ids_edge)

        for id_stop, id_edge in zip(ids_prtstop, ids_edge):
            # print '    route for id_stop, id_edge',id_stop, id_edge

            # remove origin from target
            ids_edge_target.discard(id_edge)

            costs, routes = edgedijkstra(id_edge,
                                         ids_edge_target=ids_edge_target,
                                         weights=times, fstar=fstar
                                         )

            # print '    ids_edge_target',ids_edge_target
            # print '    costs\n',   costs
            # print '    routes\n',   routes
            # for route in routes:
            #    if len(route)==0:
            #        print 'WARNING in make_times_stop_to_stop: empty route'
            #    else:
            #        print '    found route to id_edge, id_stop',route[-1],ids_edge_to_ids_prtstop[route[-1]],len(route)

            if costs is not None:
                # TODO: could be vectorialized, but not so easy
                for id_edge_target in ids_edge_target:
                    #stop_to_stop[id_edge,id_edge_target] = costs[id_edge_target]
                    # print '     stop_orig,stop_target,costs ',ids_edge_to_ids_prtstop[id_edge],ids_edge_to_ids_prtstop[id_edge_target],costs[id_edge_target]
                    # print '     stop_orig,costs ',ids_edge_to_ids_prtstop[id_edge],ids_sumo[id_edge]
                    # print '     stop_target',ids_edge_to_ids_prtstop[id_edge_target],ids_sumo[id_edge_target]
                    # print '     costs ',costs[id_edge_target]
                    # stop_to_stop[ids_edge_to_ids_prtstop[[id_edge,id_edge_target]]]=costs[id_edge_target]
                    if id_edge_target in costs:
                        stop_to_stop[ids_edge_to_ids_prtstop[id_edge],
                                     ids_edge_to_ids_prtstop[id_edge_target]] = costs[id_edge_target]
                    else:
                        print 'WARNING in make_times_stop_to_stop: unreacle station id_fromedge = %d, %s' % (id_edge_target, ids_sumo[id_edge_target])
                        is_incomplete_fstar = True

                # put back origin to targets (probably not the best way)
                ids_edge_target.add(id_edge)
                # print '    ids_edge_target (all)',ids_edge_target

            # print '    stop_to_stop',stop_to_stop
            # TODO: here we could also store the routes

        if is_incomplete_fstar:
            return False

        self.times_stop_to_stop = stop_to_stop
        self.ids_edge_to_ids_prtstop = ids_edge_to_ids_prtstop
        print '  times_stop_to_stop=\n', self.times_stop_to_stop
        return True

    def get_fstar(self):
        """
        Returns the forward star graph of the network as dictionary:
            fstar[id_fromedge] = set([id_toedge1, id_toedge2,...])
        """
        print 'get_fstar'
        net = self.get_scenario().net
        # prt mode
        id_mode = self.id_prtmode

        #ids_edge = self.get_ids()
        #fstar = np.array(np.zeros(np.max(ids_edge)+1, np.obj))
        fstar = {}
        connections = net.connections
        lanes = net.lanes

        #inds_con = connections.get_inds()
        #ids_fromlane = connections.ids_fromlane.get_value()[inds_con]
        #ids_tolane = connections.ids_tolane.get_value()[inds_con]

        ids_con = connections.get_ids()
        ids_fromlane = connections.ids_fromlane[ids_con]
        ids_tolane = connections.ids_tolane[ids_con]

        ids_mainmode_from = lanes.ids_mode[ids_fromlane]
        ids_mainmode_to = lanes.ids_mode[ids_tolane]

        #ids_modes_allow_from = lanes.ids_modes_allow[ids_fromlane]
        #ids_modes_allow_to = lanes.ids_modes_allow[ids_tolane]

        ids_fromedge = lanes.ids_edge[ids_fromlane]
        ids_toedge = lanes.ids_edge[ids_tolane]
        # print '  ids_fromedge',ids_fromedge
        # print '  ids_modes_allow',ids_modes_allow

        for id_fromedge, id_toedge, id_mode_allow_from, id_mode_allow_to, id_fromlane, id_tolane in\
            zip(ids_fromedge, ids_toedge, ids_mainmode_from, ids_mainmode_to,
                ids_fromlane, ids_tolane):

            if id_mode_allow_from == id_mode:
                if id_mode_allow_to == id_mode:

                    if fstar.has_key(id_fromedge):
                        fstar[id_fromedge].add(id_toedge)
                    else:
                        fstar[id_fromedge] = set([id_toedge])
            # if id_fromedge == 14048:
            #    print '  id_fromedge, id_toedge',id_fromedge, id_toedge,fstar.has_key(id_fromedge)
            #    print '  id_fromlane, id_tolane ',id_fromlane, id_tolane
            #    print '  id_mode_allow_from, id_mode_allow_to',id_mode_allow_from, id_mode_allow_to

        # for id_fromedge, id_toedge,ids_mode_allow_from,id_modes_allow_to  in\
        #                zip(ids_fromedge, ids_toedge, ids_modes_allow_from, ids_modes_allow_to):
        #    if len(ids_mode_allow_from)>0:
        #        if ids_mode_allow_from[-1] == id_mode:
        #            if len(id_modes_allow_to)>0:
        #                if id_modes_allow_to[-1] == id_mode:
        #
        #                    if fstar.has_key(id_fromedge):
        #                        fstar[id_fromedge].add(id_toedge)
        #                    else:
        #                        fstar[id_fromedge]=set([id_toedge])

        return fstar

    def get_times(self, fstar):
        """
        Returns freeflow travel times for all edges.
        The returned array represents the speed and the index corresponds to
        edge IDs.

        """
        if len(fstar) == 0:
            return []

        net = self.get_scenario().net
        #id_mode = net.modes.get_id_mode(mode)
        id_mode = self.id_prtmode
        # print 'get_times id_mode,is_check_lanes,speed_max',id_mode,is_check_lanes,speed_max
        ids_edge = np.array(fstar.keys(), dtype=np.int32)

        times = np.array(np.zeros(np.max(ids_edge)+1, np.float32))
        speeds = net.edges.speeds_max[ids_edge]

        # limit allowed speeds with max speeds of mode
        speeds = np.clip(speeds, 0.0, net.modes.speeds_max[id_mode])

        times[ids_edge] = net.edges.lengths[ids_edge]/speeds

        return times

    def config_results(self, results):
        print 'PrtService.config_results', results, id(results)
        # keep a link to results here because needed to
        # log data during simulation
        # this link should not be followed during save process
        #self._results = results

        tripresults = res.Tripresults('prttripresults', results,
                                      self.prtvehicles,
                                      self.get_scenario().net.edges,
                                      name='PRT trip results',
                                      info='Table with simulation results for each PRT vehicle. The results refer to the vehicle journey over the entire simulation period.',
                                      )
        results.add_resultobj(tripresults, groupnames=['Trip results'])

        prtstopresults = Stopresults('prtstopresults', results, self.prtstops)
        results.add_resultobj(prtstopresults, groupnames=['PRT stop results'])

    # def get_results(self):
    #    return self._results

    def process_results(self, results, process=None):
        pass


class Stopresults(am.ArrayObjman):
    def __init__(self, ident, results, prtstops,
                 name='Stop results',
                 info='Table with simulation results of stops generated from vehicle management.',
                 **kwargs):

        self._init_objman(ident=ident,
                          parent=results,  # main results object
                          info=info,
                          name=name,
                          **kwargs)

        self.add(cm.AttrConf('time_step', 5.0,
                             groupnames=['parameters'],
                             name='Step time',
                             info="Time of one recording step.",
                             unit='s',
                             ))

        self.add(cm.ObjConf(prtstops, is_child=False,  # groupnames = ['_private']
                            ))

        self.add_col(am.IdsArrayConf('ids_stop', prtstops,
                                     groupnames=['state'],
                                     is_index=True,
                                     name='PRT stop ID',
                                     info='ID of PRT stop.',
                                     ))

        attrinfos = [
            ('inflows_veh', {'name': 'Vehicle in-flows', 'unit': '1/s',
                             'dtype': np.float32, 'info': 'Vehicle flow into the stop over time.'}),
            ('inflows_veh_sched', {'name': 'Sched. vehicle in-flows', 'unit': '1/s',
                                   'dtype': np.float32, 'info': 'Scheduled vehicle flow into the stop over time.'}),
            ('inflows_person', {'name': 'Person in-flows', 'unit': '1/s',
                                'dtype': np.float32, 'info': 'Person flow into the stop over time.'}),
            ('numbers_person_wait', {'name': 'waiting person',         'dtype': np.int32,
                                     'info': 'Number of waiting persons at stop over time.'}),
            ('waittimes_tot', {'name': 'total wait time',         'dtype': np.float32,
                               'info': 'Wait times of all waiting persons at a stop over time.'}),
        ]

        for attrname, kwargs in attrinfos:
            self.add_resultattr(attrname, **kwargs)

    def get_dimensions(self):
        return len(self), len(self.inflows_veh.get_default())

    def get_prtstops(self):
        return self.prtstops.get_value()
        # return self.ids_stop.get_linktab()

    def init_recording(self, n_timesteps, time_step):
        print 'init_recording n_timesteps, time_step', n_timesteps, time_step, len(self.ids_stop.get_linktab().get_ids())
        self.clear()

        self.time_step.set_value(time_step)

        for attrconfig in self.get_stopresultattrconfigs():
            # print '  reset attrconfig',attrconfig.attrname
            attrconfig.set_default(np.zeros(n_timesteps, dtype=attrconfig.get_dtype()))
            attrconfig.reset()
            # print '  default=',attrconfig.get_default(),attrconfig.get_default().dtype
        ids_stop = self.get_prtstops().get_ids()
        # print '  ids_stop',ids_stop
        self.add_rows(n=len(ids_stop), ids_stop=ids_stop)

    def record(self, timestep, ids, **kwargs):
        inds = self.ids_stop.get_linktab().get_inds(ids)
        timestep_int = int(timestep)
        for attrname, values in kwargs.iteritems():
            # print '  record',attrname,'dtype',values.dtype,values.shape, 'array',getattr(self,attrname).get_value().dtype,'shape', getattr(self,attrname).get_value().shape
            # print '    inds',type(inds),inds.dtype,
            getattr(self, attrname).get_value()[inds, timestep_int] = values

    def get_stopresultattrconfigs(self):
        return self.get_attrsman().get_group_attrs('PRT results').values()

    def get_persons(self):
        return self.ids_person.get_linktab()

    def add_resultattr(self, attrname, **kwargs):
        self.add_col(am.ArrayConf(attrname, 0, groupnames=['PRT results', 'results'], **kwargs))

    def import_xml(self, sumo, datapaths):
        # no imports, data come from prtservice
        pass
