"""
This plugin provides methods to run and analyze PRT networks.

   
"""
import os, sys
import numpy as np
from copy import copy
#import random
from agilepy.lib_base.processes import Process
#from xml.sax import saxutils, parse, handler
from collections import OrderedDict


from coremodules.modules_common import *
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import find_char_in_string
#from agilepy.lib_base.geometry import find_area
#from agilepy.lib_base.processes import Process,CmlMixin,ff,call
from coremodules.network.network import SumoIdsConf
from coremodules.network.routing import edgedijkstra, get_mincostroute_edge2edge
from coremodules.simulation import sumo
from coremodules.simulation.sumo import traci

#from coremodules.demand.demandbase import DemandobjMixin
from coremodules.simulation.simulationbase import SimobjMixin


from coremodules.simulation import results as res


get_traci_odo = traci.vehicle.getDistance
get_traci_velocity = traci.vehicle.getSpeed
get_traci_route_all = traci.vehicle.getRoute
get_traci_routeindex = traci.vehicle.getRouteIndex
get_traci_distance = traci.vehicle.getDrivingDistance
get_traci_edge = traci.vehicle.getRoadID

def get_traci_route(id_veh_sumo):
    return get_traci_route_all(id_veh_sumo)[get_traci_routeindex(id_veh_sumo):]

def get_entered_vehs(ids_veh_sumo_current, ids_veh_sumo_before):
    ids_veh_entered_sumo = np.array(list(ids_veh_sumo_current.difference(ids_veh_sumo_before)), dtype = np.object_)
    n_entered = len(ids_veh_entered_sumo)
    positions = np.zeros(n_entered, dtype = np.float32)
    for i, id_veh_sumo in zip(range(n_entered),ids_veh_entered_sumo):
        positions[i] = traci.vehicle.getLanePosition(id_veh_sumo)

    return list(ids_veh_entered_sumo[positions.argsort()])[::-1]

INF = 10.0**10 
INFINT =  10**10  

class Legs(am.ArrayObjman):
        def __init__(self, ident, tlclusters, ids_detector_in=[], ids_edge_out = []):
        
            self._init_objman(  ident=ident, parent = tlclusters, 
                                name = 'Legs',
                                info = "Data on Legs controlling traffic lights.",
                                version = 0.0,
                            )   
            
            #self.add_col(am.ArrayConf(  'legnames', 
            #                                default = '',
            #                                dtype = 'object',
            #                                groupnames = ['parameters'], 
            #                                perm = 'wr', 
            #                                is_index = False,
            #                                name = 'Leg name',
            #                                info = 'Name of leg.',
            #                                ))
            
            edges = self.parent.get_scenario().net.edges
            detectors = self.parent.get_scenario().demand.detectorflows.detectors
            
            
            
            
            self.add_col(am.IdsArrayConf( 'ids_detector_in', detectors,
                                            groupnames = ['parameters'],
                                            name = 'ID detector in',
                                            info = 'ID of incoming detector.',
                                            ))
            
            # TODO: in the future we may generate detectors at the end of id_edge_out
            # and use those instead of the edge to count traffic                               
            self.add_col(am.IdsArrayConf( 'ids_edge_out', edges,
                                            groupnames = ['parameters'],
                                            name = 'ID edge out',
                                            info = 'ID of outgoing edge of leg, the one entering the TLS.',
                                            ))
            
            
                                            
            # attention, phase IDs refer to different programs, depends on the TLL                               
            self.add_col(am.ArrayConf( 'ids_phases', "",
                                            groupnames = ['parameters'],
                                            dtype = np.object_,
                                            perm='rw',
                                            name = 'Phase indexes',
                                            info = 'List with phase indexes that are green for the outgoing edge og this leg.',
                                            ))                                                                
            
            
                
            self._init_attributes()
            self._init_constants()
        
        
        
        def _init_constants(self):
            self.do_not_save_attrs([    'map_id_detector_sumo_to_id_leg',
                                        'counts_in','counts_in_laststep',
                                        'counts_out','counts_out_laststep',
                                        'ids_veh_sumo_laststep',
                                        'queues','are_greenphase',
                                        'is_record_results',
                                        ])
        
        
        def _init_attributes(self):
            self.add(cm.AttrConf(  'const_queue', 0.1,
                                        groupnames = ['options'], 
                                        name = 'Const. Queue',
                                        info = "Queue control constant, to control the influence of queue length of legs on green time increase.",
                                        ))
            
        
    
            
            
            
        def prepare_sim(self, process):
            print('Legs.prepare_sim',self.ident)
            net = self.parent.get_scenario().net
            detectors = self.parent.get_scenario().demand.detectorflows.detectors
            lanes = net.lanes
            ids_leg = self.get_ids()
            self.ids_edge_out_sumo = net.edges.ids_sumo[self.ids_edge_out[ids_leg]]
            
            n_legs = len(self)
            ids_detector = self.ids_detector_in[ids_leg]
            print('  ids_detector',ids_detector)
            print('  ids_lanes',detectors.ids_lanes[ids_detector])
            # create a disctionary with sumo detector ID as key
            # and leg ID as value
            self.map_id_detector_sumo_to_id_leg = {}
            for id_leg, id_detector,  ids_lane  in zip(\
                                                ids_leg,
                                                ids_detector,
                                                detectors.ids_lanes[ids_detector],
                                                ):
                for ind_lane in lanes.indexes[ids_lane]:
                    self.map_id_detector_sumo_to_id_leg[str(id_detector)+'/'+str(ind_lane)] =  id_leg
                    
            print ('  map_id_detector_sumo_to_id_leg',self.map_id_detector_sumo_to_id_leg)    
            
            self.counts_in = np.zeros(n_legs+1,dtype = np.int32)
            self.counts_in_laststep = np.zeros(n_legs+1,dtype = np.int32) # for generating data only
            
            self.counts_out = np.zeros(n_legs+1,dtype = np.int32)
            self.counts_out_laststep = np.zeros(n_legs+1,dtype = np.int32) # for generating data only
            
            self.ids_veh_sumo_laststep = np.ones(n_legs+1,dtype = np.object_)
            for i in range(n_legs+1):
                self.ids_veh_sumo_laststep[i]=set()
            
            self.queues = np.zeros(n_legs+1,dtype = np.int32)
            self.are_greenphase = np.zeros(n_legs+1,dtype = np.int32)
            
            self.is_record_results = self.parent.is_record_results
            
        def process_step(self, process):
            # called in clusters process_step
            simtime = process.simtime
            ids_leg = self.get_ids()
            
            # update counter for incoming vehicles, reading detectors
            for id_detector_sumo, id_leg in self.map_id_detector_sumo_to_id_leg.items():
                self.counts_in[id_leg] += traci.inductionloop.getLastIntervalVehicleNumber(id_detector_sumo)
            
            # update counter for outgoing vehicles, checking vehicles that left the edge
            for id_leg, id_edge_sumo in zip(ids_leg, self.ids_edge_out_sumo):
                ids_veh_sumo = set(traci.edge.getLastStepVehicleIDs(id_edge_sumo))
                self.counts_out[id_leg] += len(self.ids_veh_sumo_laststep[id_leg].difference(ids_veh_sumo))
                self.ids_veh_sumo_laststep[id_leg] = ids_veh_sumo
                
            # update queue
            self.queues[ids_leg] += (self.counts_in[ids_leg]-self.counts_in_laststep[ids_leg] )\
                                            - (self.counts_out[ids_leg] - self.counts_out_laststep[ids_leg] )
            
            self.queues[ids_leg] = np.clip(self.queues[ids_leg],0.0,np.infty)
            
            
            # write results if required
            if self.is_record_results:
                results = process.get_results()
                if results is not None:
                    #ids_leg = self.get_ids()
                    results.legresults.record(simtime - process.simtime_start, ids_leg,
                                    detecteds_in =  self.counts_in[ids_leg]-self.counts_in_laststep[ids_leg],
                                    detecteds_out =  self.counts_out[ids_leg]-self.counts_out_laststep[ids_leg],
                                    lengths_queue = self.queues[ids_leg],
                                    are_greenphase = self.are_greenphase[ids_leg],
                                    # stretchs_greenphase
                                    )
            self.counts_in_laststep[ids_leg] = 1*self.counts_in[ids_leg]
            self.counts_out_laststep[ids_leg] = 1*self.counts_out[ids_leg]
        
        def set_phase(self,ids_leg, id_phase):
            for id_leg in ids_leg:
                # check if given phase is in the green phases of given legs
                self.are_greenphase[id_leg] = id_phase in self.ids_phases[id_leg]
            
        def get_desired_durations(self, id_leg, durations_desired):
            """
            Calculate desired phase duration of phases associated with id_leg.
            durations_desired is an array with default durations.
            """
            # this is the implemented TL signal control law
            # again, in durations array phase index starts with 0, not 1
            durations_desired[np.array(self.ids_phases[id_leg], dtype = np.int32)-1] *= 1.0 + self.const_queue.get_value() * self.queues[id_leg] 
            
                
class TlControl(am.ArrayObjman):
        def __init__(self, ident, tlclusters, name = 'Cluster1'):
        
            self._init_objman(  ident=ident, parent = tlclusters, name = name,
                            info = "Tl control instance for one cluster "+name,
                            version = 0.0,
                            )
                            
       
            self._init_attributes()
            self._init_constants()   
            self.max_durations={}
            
                                            
            
                                
            
        def get_scenario(self):
            return self.parent.get_scenario()
        
        def get_legs(self):
            return self.parent.get_legs()
                
        def _init_constants(self):
            pass
            
            #self.do_not_save_attrs(['pair_to_id','queues_in_out','vehicles_in_pairs'])
        
        
        def _init_attributes(self):
            
            self.add(cm.AttrConf(  'time_cycle_min', 120,
                                        groupnames = ['options'], 
                                        name = 'Minimum cycle time',
                                        unit = 's',
                                        info = "Minimum cycle time for all traffic lightss this cluster control.This is also the initial cycle time.",
                                        )) 
                                        
            self.add(cm.AttrConf(  'time_cycle_max', 180,
                                        groupnames = ['options'], 
                                        name = 'Maximum cycle time',
                                        unit = 's',
                                        info = "Maximum cycle time for all traffic lightss this cluster control.This is the upper limit to the cluster's cycle time.",
                                        ))
                                        
            tlss = self.get_scenario().net.tlss
            tlls = self.get_scenario().net.tlss.tlls.get_value()
            
            self.add_col(am.IdsArrayConf( 'ids_tls', tlss,
                                            groupnames = ['parameters'],
                                            name = 'ID Tls',
                                            info = 'ID of traffic light system.',
                                            ))
                                            
            
            self.add_col(am.IdsArrayConf( 'ids_tll', tlls,
                                            groupnames = ['parameters'],
                                            name = 'ID Tll',
                                            info = 'ID of traffic light logic, tis is the traffic light program.',
                                            ))
                                            
            
                                            
            self.add_col(am.IdlistsArrayConf( 'ids_legs', self.get_legs(),
                                            groupnames = ['parameters'],
                                            name = 'Leg IDs',
                                            info = 'List of Leg IDs controlling this traffic light.',
                                            ))
                                            
            
            

        
        def prepare_sim(self, process):
            print('TlClusters.prepare_sim',self.ident)
            tlss = self.get_scenario().net.tlss
            tlls = tlss.tlls.get_value() 
            
            self.control_method = self.parent.control_method.get_value()
            
            ids_tlc = self.get_ids()
            n_tlc = len(self)
            
            # list with default durations for each traffic light of the cluster
            # durations = self.durations_phases_default[id_tlc]
            
            self.durations_phases_default = np.zeros(n_tlc+1,dtype = np.object_)
            self.durations_phases_desired = np.zeros(n_tlc+1,dtype = np.object_)
            self.durations_phases_current = np.zeros(n_tlc+1,dtype = np.object_)
            
            # phase of each traffic light in last simulation step
            # initialize all with phase 1
            self.ids_phase_last = np.ones(n_tlc+1,dtype = np.int32)
            
            # desired cycle times array, which varies from tl to tl,
            # but needs to be scaled to a unique cycle time
            self.times_cycle_desired = np.ones(n_tlc+1,dtype = np.float32)
            
            
            # time stretch factor for the queue controlled phases
            # default is one (corrisponds to no stretch)
            #self.stretches = np.ones(n_tlc+1,dtype = np.int32)
            
            time_cycle_min = self.time_cycle_min.get_value()
            ids_tls = self.ids_tls[ids_tlc]
            for id_tlc, id_tls, id_tll in zip(ids_tlc,ids_tls, tlss.ids_tlls[ids_tls]):
                print('  id_tlc',id_tlc)
                prog = [] # parsim program
                prog = tlls.programs[id_tll][0] # the first available program (there is only one currently)
                
                ids_phases = prog.get_ids()
                durations = 1.0*prog.durations[ids_phases]
                #tls_defs = traci.trafficlight.getCompleteRedYellowGreenDefinition(id_tll)
                #logic = tls_defs[0]  # Only one logic usually
                #phases = logic.phases
                #phase_index = traci.trafficlight.getPhase(id_tll)
                #self.max_durations = phases[phase_index].maxDur
                print(f'duration:{durations}')
                
                
                #durations_min = 1.0*prog.durations_min[ids_phases]
                # scale phase duration, just to be sure initial 
                # cycle times are the same for all TLs in the cluster
                if np.sum(durations) != time_cycle_min:
                    durations *= time_cycle_min/np.sum(durations)
                
                # define initial phase durations
                # and copy to current phase durations
                self.durations_phases_default[id_tlc] = list(durations)
                self.durations_phases_current[id_tlc] = list(durations)
                #print ('    durations len',len(self.durations_phases_current[id_tlc]),'=',self.durations_phases_current[id_tlc])
            
            # JUNK-------------------------------------------------    
            #for id_tls_sumo,  in tlss.ids_sumo[self.ids_tls[ids]]:
            #    id_phase_last
            #    id_phase = traci.trafficlight.getPhase(id_tls_sumo)
            #    # Returns the id of the current program.
            #    #print (traci.trafficlight.getProgram(id_tls_sumo))
            #    #logic = traci.trafficlight.getAllProgramLogics(id_tls_sumo)[0]
            #    #print ('\n\nProgramLogic',logic)
            #    #print ('dir(logic)',dir(logic))
                
            
            #getPhase(self, tlsID)
            #    getPhase(string) -> integer
            #     
            #    Returns the index of the current phase within the list of all phases of
            #    the current program.
            
            #getPhaseDuration(self, tlsID)
            #getPhaseDuration(string) -> double
            # 
            #Returns the total duration of the current phase (in seconds). This value
            #is not affected by the elapsed or remaining duration of the current phase.
                    
                
            # Returns the time in seconds for which the current phase has been active
            #getSpentDuration(self, tlsID)
            
            # Returns the absolute simulation time at which the traffic light is
            # schedule to switch to the next phase (in seconds).
            #getNextSwitch(self, tlsID)
            #getNextSwitch(string) -> double
     
            #getSpentDuration(self, tlsID)
            #getSpentDuration(string) -> double
            # 
            #Returns the time in seconds for which the current phase has been active
        
        def process_step(self, process):
            
            
            time_cycle_min = self.time_cycle_min.get_value()
            tlss = self.get_scenario().net.tlss
            tlls = tlss.tlls.get_value() 
            legs = self.parent.get_legs()
            ids_tlc = self.get_ids()
            n_tlc = len(self)
            
               
            time_cycle_min = self.time_cycle_min.get_value()
            ids_tls = self.ids_tls[ids_tlc]
            mode = self.parent.phase_control_mode.get_value()
            
            
            is_new_cycle = False
            for id_tlc, id_tls, id_tls_sumo, id_tll, durations_phase  in zip(\
                    ids_tlc,ids_tls, tlss.ids_sumo[ids_tls], tlss.ids_tlls[ids_tls] , self.durations_phases_current[ids_tlc]):
                id_phase = traci.trafficlight.getPhase(id_tls_sumo)+1 # id_phase starts with 1, sumo phase starts with 0
                
                 ######
                #ids_phases = prog.get_ids()
                #prog = tlls.programs[id_tll][0] # the first available program (there is only one currently)
               # max_durations = 1.0*prog.durations[ids_phases]
                
                ######
           
                
                
                if id_phase <= len(durations_phase): # this is to exclude tls which went crazy
                
                    print ('    id_tlc',id_tlc,'id_tls_sumo',id_tls_sumo,'id_phase',id_phase,self.ids_phase_last[id_tlc])
                    print ('          duration_phase',durations_phase[id_phase-1])# id_phase starts with 1, sumo phase starts with 0
                    
                    
                    if (id_phase != self.ids_phase_last[id_tlc]) & (self.ids_phase_last[id_tlc] != -1):
                        if id_phase < self.ids_phase_last[id_tlc]:
                            # restart of cycle
                            # recompute durations 
                            is_new_cycle = True
                            # will trigger recomputing of duration of whole cluster
                            # phase durations will be set below, after recalculating 
                            # all phases of all tlc
                        else:
                            # set duration for this phase
                            traci.trafficlight.setPhaseDuration(id_tls_sumo, durations_phase[id_phase-1])# id_phase starts with 1, sumo phase starts with 0
                        
                        # update legs with phase, this is necessary for recording only
                        legs.set_phase(self.ids_legs[id_tlc], id_phase)
                        
                        self.ids_phase_last[id_tlc] = id_phase
                else:
                    self.ids_phase_last[id_tlc] = -1 # indicate tls failure
                
                
            if self.control_method != 'none':
                # now do recomputing and resetting of all traffic lights in this cluster
                if is_new_cycle:
                    # determine new desired phase duration based on queue length for each traffic light
                    print ('  new cycle:')
                    for id_tlc,id_tls, ids_leg, durations_phase_default,  in zip(ids_tlc,ids_tls,self.ids_legs[ids_tlc],self.durations_phases_default[ids_tlc]):
                        durations_desired = np.array(durations_phase_default, dtype = np.float32)
                        if self.ids_phase_last[id_tlc] != -1:
                        
                            print ('    check id_tlc',id_tlc,'id_tlc',id_tlc)
                            for id_leg in ids_leg:
                                # get desired phase duration from leg 
                                # here is where the duratios of corrisponding phases are 
                                # adjusted depending on queue length
                                if self.control_method == 'queue':
                                    legs.get_desired_durations(id_leg, durations_desired)
                                else:
                                    # other control methods, adjusting durations_desired
                                    pass
                        
                        self.durations_phases_desired[id_tlc] = list(durations_desired)
                        print ('        durations_phase_desired',durations_desired)
                        print ('        durations_phase_default',durations_phase_default)
                        self.times_cycle_desired[id_tlc] = durations_desired.sum()
                        print ('        times_cycle_desired',self.times_cycle_desired[id_tlc],'default',sum(durations_phase_default))
                        
                    # determine global cluster cycle time for all TL in the cluster by 
                    # taking the maximum of all desired cycle times
                    # bit clipping at times_cycle_min and times_cycle_max
                    time_cycle_cluster = np.max(np.clip(self.times_cycle_desired[ids_tlc], 
                                                            self.time_cycle_min.get_value(), 
                                                            self.time_cycle_max.get_value()
                                                            ))
                    
                    print ('  new time_cycle_cluster',time_cycle_cluster)                                        
                    # use time_cycle_cluster to scale phase duration 
                    #max_durations = traci.trafficlight.getPhaseDuration.max_durations(id_tls_sumo)
                    for id_tlc, id_tls_sumo  in zip(ids_tlc, tlss.ids_sumo[ids_tls]):
                        self.durations_phases_current[id_tlc] = list(np.array(self.durations_phases_desired[id_tlc]) * time_cycle_cluster / self.times_cycle_desired[id_tlc])
                        # here set actually duration of first phase which is the current one
                        traci.trafficlight.setPhase(id_tls_sumo,0)# make sure that all TL are in the first phase 0
                       
                        
                        tls_defs = traci.trafficlight.getCompleteRedYellowGreenDefinition(id_tls_sumo)
                        logic = tls_defs[0]  # Only one logic usually
                        phases = logic.phases
                        phase_index = traci.trafficlight.getPhase(id_tls_sumo)
                        max_durations = phases[phase_index].maxDur
                        min_durations = phases[phase_index].minDur
                        current_phase_duration = self.durations_phases_current[id_tlc][0]
                        
                        
                        
                        def is_all_green(phase_state):
                            """
                            Check if all lanes are green in the state string.
                            """
                            return any(c.lower() =='g' for c in phase_state)
                        
                        if self.ids_phase_last[id_tlc] != -1:
                        
                            if mode == 'green phase check':
                                """set phase duration  on any phase which has green phases"""
    
                                if is_all_green(phases[phase_index].state):
                                    # if all lanes are green, set the current phase duration
                                    # to the desired duration
                                    
                                    if min_durations <= current_phase_duration <= max_durations:
                                        traci.trafficlight.setPhaseDuration(id_tls_sumo, current_phase_duration)
                                    else:
                                        """set phase duration only if it is less than the maximum and higher than the minimum duration"""
                                        safe_duration = np.clip(current_phase_duration, min_durations, max_durations)
                                        traci.trafficlight.setPhaseDuration(id_tls_sumo, safe_duration)
                                        # note that SUMO phase index starts with 0 while id_phase = 1
                                        #traci.trafficlight.setPhaseDuration(id_tls_sumo, self.durations_phases_current[id_tlc][0])
                            if mode == 'duration phase': 
                                """set phases for phases which has a certain green time or higher  """
                                if phases[phase_index].duration > 6.0:
                                    # if the phase has a certain green time or higher, set the phase duration
                                    # to the desired duration
                                    if min_durations <= current_phase_duration <= max_durations:
                                        traci.trafficlight.setPhaseDuration(id_tls_sumo, current_phase_duration)
                                    else:
                                        """set phase duration only if it is less than the maximum and higher than the minimum duration"""
                                        safe_duration = np.clip(current_phase_duration, min_durations, max_durations)
                                        traci.trafficlight.setPhaseDuration(id_tls_sumo, safe_duration) 
                            
                            if mode == 'combined':
                                """ check both phases state and minimum duration to change the phase duration  """  
                                if is_all_green(phases[phase_index].state) and phases[phase_index].duration > 6.0:
                                    # if all lanes are green and the phase has a certain green time or higher, set the phase duration
                                    # to the desired duration
                                    if min_durations <= current_phase_duration <= max_durations:
                                        traci.trafficlight.setPhaseDuration(id_tls_sumo, current_phase_duration)
                                    else:
                                        """set phase duration only if it is less than the maximum and higher than the minimum duration"""
                                        safe_duration = np.clip(current_phase_duration, min_durations, max_durations)
                                        traci.trafficlight.setPhaseDuration(id_tls_sumo, safe_duration)       
                                
                            
       
       
       
        def identify_phases(self):
            """
            Identify the phases in the Tll that is effected by 
            traffic flows in each leg.
            """                            
            legs = self.get_legs()
            tlls = self.ids_tll.get_linktab()
            
            connections = self.get_scenario().net.connections
            lanes = self.get_scenario().net.lanes
            tlss = self.get_scenario().net.tlss
            ids_tlc = self.get_ids()    
            for id_tls, id_tll, ids_leg in zip(self.ids_tls[ids_tlc],self.ids_tll[ids_tlc], self.ids_legs[ids_tlc]):
            
                
                    
                prog = tlls.programs[id_tll] # the first available program (there is only one currently)
                if 0:
                    #if id_tls_debug == id_tls:
                    print ('\n  id_tls',id_tls)
                    #print ('  prog',prog,len(prog))
                    #print ('  ids_con',ids_con)
                    #print ('  ids_phases',ids_phases)
                    #print ('  prog',prog,len(prog))
                    #print ('  edge_to_connumbers',edge_to_connumbers)
                    
                ids_phases = prog.get_ids()
                ids_con = tlss.ids_cons[id_tls]
                n_cons = len(ids_con)
                ids_edge_con = lanes.ids_edge[connections.ids_fromlane[ids_con]]
                #print ('    ids_edge_con =',ids_edge_con)
                for id_leg, id_edge in zip(ids_leg, legs.ids_edge_out[ids_leg]):
                    #inds_edge_con_ok = ids_edge_con == id_edge
                    ids_phase_edge = []
                    #print('    id_edge',id_edge)
                    for id_phase,duration, state in zip(ids_phases,prog.durations[ids_phases],prog.states[ids_phases]): 
                        inds_green = np.unique(find_char_in_string(state[:n_cons],'g')+find_char_in_string(state[:n_cons],'G'))
                        #print ('      state[:n_cons]',state[:n_cons])
                        #print ('      inds_green',inds_green)
                        if len(inds_green)>0:
                            if id_edge in ids_edge_con[inds_green]:
                                ids_phase_edge.append(id_phase)
                            
                    legs.ids_phases[id_leg] = ids_phase_edge
                        
                    
                    
                    
class TlClusters(SimobjMixin,am.ArrayObjman):
        def __init__(   self, ident='tlclusters', simulation=None,
                        name = 'Traffic Light Clusters', 
                        info ='Holds a list of cluters, each controlling a set of traffic lights.',
                        **kwargs): 
                        
            #print('TlClusters.__init__',name,'parent',simulation)
            
            self._init_objman(  ident = ident, 
                                parent = simulation, 
                                name = name, 
                                info = info,
                                )
                                
            
            self.add(cm.AttrConf(  'is_enable', kwargs.get('is_enable',False),
                                        groupnames = ['options'], 
                                        name = 'Enable',
                                        info = "Enable Traffic light controle during TRACI simulation.",
                                        ))
            
                                        
            self.add(cm.ObjConf(Legs('legs',self),
                                groupnames = ['state'], 
                                name = 'Legs', 
                                info = 'Legs object.',
                                ))
           
            
            
                                
            self._init_attributes( **kwargs)
            self._init_constants()
            
        
        
        def get_scenario(self):
            return self.parent.parent
        
        def get_legs(self):
            return self.legs.get_value()
            
        
        
        def _init_constants(self):
            pass
            
            #self.do_not_save_attrs(['pair_to_id','queues_in_out','vehicles_in_pairs'])
        
        
        
        def _init_attributes(self, **kwargs):
            #print ('TlClusters._init_attributes_local kwargs',kwargs)
            scenario = self.get_scenario()
            # no, update with time_sample of process, which corrisponds                             
            # to the sampling interval of the sensors
            #self.add(cm.AttrConf(  'time_update', kwargs.get('time_update',1.0),
            #                            groupnames = ['options'], 
            #                            name = 'Update time',
            #                            info = "Update time of the TL control.",
            #                            unit = 's',
            #                            ))
            self.add(cm.AttrConf(  'control_method', kwargs.get('control_method','none'),
                                        choices = ['none','queue'],
                                        groupnames = ['options'], 
                                        name = 'Control method',
                                        info = "Control method.",
                                        )) 
            self.add(cm.AttrConf(  'is_record_results', kwargs.get('is_record_results',True),
                                        groupnames = ['options'], 
                                        name = 'Record results',
                                        info = "Record simulation results.",
                                        ))  
            
            self.add_col(am.ArrayConf(  'clusternames', 
                                            default = '',
                                            dtype = 'object',
                                            groupnames = ['parameters'], 
                                            perm = 'wr', 
                                            name = 'Cluster name',
                                            info = 'Name of Cluster.',
                                            ))
                                        
                                        
            self.add_col(cm.ObjsConf('tlcontrols',
                                     groupnames=['parameter'],
                                     name='TL control',
                                     info='Traffic ligh control object.',
                                     ))
                                     
            self.add(cm.AttrConf('phase_control_mode', kwargs.get('phase_control_mode','combined'),
                                            choices=['green phase check', 'duration phase', 'combined'],
                                            groupnames=['options'],
                                            name='Phase control mode',
                                            info='Select method to adjust phase durations: green phase check, duration phase, combined',
                                            ))
            
                                            
            
            
                              
            
            
                                        
        
                    
        def prepare_sim(self, process):
            print('TlClusters.prepare_sim',self.ident)
            
            time_sample = process.time_sample # this is the global result sampling time
            
            if (len(self) > 0) & self.is_enable.get_value():
                ids = self.get_ids()
                for tlcontrol in self.tlcontrols[ids]:
                    tlcontrol.prepare_sim(process)
                
                self.legs.get_value().prepare_sim(process)
                
                results = process.get_results()
                if results is not None:
                    n_timesteps = int((process.duration-process.time_warmup)/time_sample+1)
                    #print('  duration',process.duration,'time_sample',time_sample,'n_timesteps')
                    results.legresults.init_recording(n_timesteps, time_sample)
                    
                return [    (process.time_sample, self.process_step),]   
                    
            else:
                print('  no TL Clusters configured.')
                return []
            
        def process_step(self, process):
            simtime = process.simtime
            debug = 2
            ids = self.get_ids() # get all cluster ids
            edges = self.get_scenario().net.edges
            ids_veh_sumo = set(traci.vehicle.getIDList())
            legs = self.legs.get_value()
            print(79*'=')
            print('TlClusters.process_step simtime',simtime)
        
            #if self.ids_veh_sumo_laststep is None:
            #    self.ids_veh_sumo_laststep = copy(ids_veh_sumo)
            #    ids_veh_sumo_new = set([])
            #else:
            #    ids_veh_sumo_new = ids_veh_sumo.difference(self.ids_veh_sumo_laststep)
            
            legs.process_step(process)
                
            for tlcontrol in self.tlcontrols[ids]:
                tlcontrol.process_step(process)
            
            
            
        def config_simresults(self, results):
            #print ('TLC.config_simresults',results, id(results))
            if self.is_record_results:
                _legresults = LegResults(  'legresults', results,
                                                self, 
                                                )
                results.add_resultobj(_legresults, groupnames = ['traffic light control'])      
                
        def process_results(self, _results, process = None):
            """
            Transfer recorded results in result object.
            """
            pass
                    
class LegResults(am.ArrayObjman):
    def __init__(self, ident, results, 
                            tlcontrol,
                             name = 'Leg results', 
                             info = 'Table with simulation results from TLC legs involved in the Control process.',
                             **kwargs):
        
        self._init_objman(  ident = ident, 
                            parent = results, # main results object
                            info = info, 
                            name = name, 
                            **kwargs)
        
        
        self._tlcontrol = tlcontrol
        self._init_attributes( **kwargs)
        self._init_constants()
            
        
        
        
    
    
    def _init_attributes(self, **kwargs):
    
    
    
        self.add_col(am.ArrayConf( 'ids_leg', -1, 
                                            groupnames = ['state'], 
                                            is_index = True,
                                            name = 'Leg ID', 
                                            info = 'ID of controlled TL Leg.',
                                            ))
        
        #self._tlcontrol.legs.get_value().print_attrs()
        self.add_col(am.ArrayConf( 'ids_detector_in', -1, 
                                            groupnames = ['state'], 
                                            is_index = False,
                                            name = 'Detector ID', 
                                            info = 'ID of detector.',
                                            ))
        attrinfos = [\
                ('detecteds_in', {'name':'Detected incoming',    'dtype':np.int32, 'info':'Detected number of incoming vehicles for each time step.'}),
                ('detecteds_out', {'name':'Detected outgoing',  'dtype':np.int32, 'info':'Detected number of outgoing vehicles for each time step.'}),
                ('lengths_queue', {'name':'Queue length',  'dtype':np.int32, 'info':'Estimated number of vehicles in the queue.'}),
                #('stretchs_greenphase', {'name':'Green phase stretch','dtype':np.float32 , 'info':'Green phase stretchfactor.'}),
                ('are_greenphase', {'name':'Green phase',  'dtype':np.bool_, 'info':'If True, traffic light is green at least for one lane entering the traffic ligh from this leg.'}),
                ]
                
        for attrname, kwargs in attrinfos:
            self.add_resultattr(attrname, **kwargs)
            
            
    def _init_constants(self):
        self.do_not_save_attrs(['_tlcontrol',
                ])
                    
    def get_dimensions(self):
        return len(self),len(self.detected_in.get_default())
    
    
    def init_recording(self, n_timesteps, time_step):
        """
        Initialize results attributes.
        The array sizes are know now.
        Called by prepare_sim of TlClusters
        """
        print('TLC init_recording n_timesteps, time_step',n_timesteps, time_step)
        self.clear()    
        legs = self.parent.get_scenario().simulation.tlclusters.legs.get_value()
        #self.time_step.set_value(time_step)
        
        for attrconfig in self.get_attrconfigs_record():
            #print '  reset attrconfig',attrconfig.attrname
            attrconfig.set_default(np.zeros(n_timesteps, dtype = attrconfig.get_dtype()) )
            attrconfig.reset()
            # print '  default=',attrconfig.get_default(),attrconfig.get_default().dtype
        ids_leg = legs.get_ids()
        #print '  ids_stop',ids_stop
        self.add_rows(n=len(ids_leg), ids_leg = ids_leg, ids_detector_in = legs.ids_detector_in[ids_leg])
    
    def record(self, timestep, ids, **kwargs):
        legs = self.parent.get_scenario().simulation.tlclusters.legs.get_value()
        inds = legs.get_inds(ids)
        timestep_int = int(timestep)
        for attrname, values in kwargs.items():
            print('  record',attrname,'dtype',values.dtype,values.shape, 'array',getattr(self,attrname).get_value().dtype,'shape', getattr(self,attrname).get_value().shape)
            #print '    inds',type(inds),inds.dtype,
            getattr(self,attrname).get_value()[inds,timestep_int] = values
        
            
    def get_attrconfigs_record(self):
        return list(self.get_attrsman().get_group_attrs('recorded').values())
            
    def get_persons(self):
        return self.ids_person.get_linktab()
                                                        
    def add_resultattr(self, attrname, **kwargs):
        self.add_col(am.ArrayConf(  attrname, 0, groupnames = ['recorded','results'],**kwargs))
       
    def import_xml(self, sumo, datapaths):
        # no imports, data come from prtservice
        pass 
                            


        
class TlClusterImporter(Process):
    def __init__(self, tlclusters,  logger = None, **kwargs):
                                    
        print('TlClusterImporter.__init__')
        

           
        self._init_common(  'tlclusterimporter', 
                            parent = tlclusters,
                            name = 'TlCluster Importer', 
                            logger = logger,
                            info ='Import traffic light cluster information from a CSV file.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        self.clustername = attrsman.add(cm.AttrConf(  'clustername',kwargs.get('clustername','Cluster1'),
                                                        groupnames = ['options'],#['options'],#['_private'],
                                                        perm='rw',
                                                        name = 'Cluster name',
                                                        info = 'Cluster name.',
                                                        ))
                                                        
        self.is_clear = attrsman.add(cm.AttrConf(  'is_clear',kwargs.get('is_clear',True),
                                                        groupnames = ['options'],#['options'],#['_private'],
                                                        perm='rw',
                                                        name = 'Clear all current TL clusters',
                                                        info = 'Clear all current traffic light clusters.',
                                                        ))
                                                        
        
        self.clusterfilepath = attrsman.add(cm.AttrConf(  'clusterfilepath',kwargs.get('clusterfilepath',tlclusters.get_scenario().get_rootfilepath()+'_tlc.csv'),
                                                        groupnames = ['options'],#['options'],#['_private'],
                                                        perm='rw',
                                                        name = 'Cluster file',
                                                        metatype = 'filepath',
                                                        info = 'Cluster description.',
                                                        ))
                        
        
                            
    def do(self):
        print('TlClusterImporter.do clusterfilepath',self.clusterfilepath)
        # links
        
        
        tlclusters = self.parent
        if self.is_clear:
            tlclusters.clear()
            
        id_cluster = tlclusters.suggest_id()
        # generate new TL control
        _tlcontrol = TlControl( 'tlc'+str(id_cluster), tlclusters, name = self.clustername)
        tlclusters.add_row(_id = id_cluster, clusternames = self.clustername, tlcontrols = _tlcontrol)
                    
        tlss = tlclusters.get_scenario().net.tlss
        tlls = tlss.tlls.get_value()
        legs = tlclusters.get_legs()
        logger = self.get_logger()
        
        
        
        f=open(self.clusterfilepath,'r', encoding="utf-8")
        sep = ','
        
        i_line = 1
        id_tls_last = -1
        ids_leg = []
        id_tlc = -1
        for line in f.readlines():
            if line[0] != '#':
                cols = line.split(sep)
                
                id_tls = int(cols[0].strip())
                id_tll = tlss.ids_tlls[id_tls][0]# take first program
                print('  id_tlc',id_tlc,'id_tls',id_tls,'id_tls_last',id_tls_last,id_tls != id_tls_last)
                if id_tls != id_tls_last:
                    if id_tlc != -1: # tlc entry already generated?
                        print('    id_tlc',id_tlc,'ids_leg',ids_leg)
                        _tlcontrol.ids_legs[id_tlc] = ids_leg
                         
                    # add traffic light logic to cluster
                    id_tlc = _tlcontrol.add_row(ids_tls = id_tls, ids_tll = id_tll)
                    ids_leg = []
                    id_tls_last = id_tls
                
                id_detector = int(cols[1].strip())
                
                id_edge_out = int(cols[2].strip())
                print('    id_detector',id_detector,'id_edge_out',id_edge_out)
                ids_leg.append( legs.add_row(   ids_detector_in = id_detector,
                                                ids_edge_out = id_edge_out  
                                            ))
                
    
            i_line +=1
            
        if id_tlc != -1:
            _tlcontrol.ids_legs[id_tlc] = ids_leg
        
        _tlcontrol.identify_phases()
            
        f.close()
        
        #tlclusters.print_attrs()
        
        return True
        
class LegResultsExporter(Process):
    def __init__(self,legresults = None,logger = None, **kwargs):
        """
        NOT IN USE!!
        """
        print('LegResultsExporter.__init__')
        

           
        self._init_common(  'legresultsexporter', 
                            parent = legresults,
                            name = 'LegResultsExporter', 
                            logger = logger,
                            info ='Exporter leg results to text file(s).',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        self.interfilepath = attrsman.add(cm.AttrConf(  'legflowfilepath',kwargs.get('legflowfilepath','legflows.txt'),
                                                        groupnames = ['options'],#['options'],#['_private'],
                                                        perm='rw',
                                                        name = 'TlCluster file',
                                                        metatype = 'filepath',
                                                        info = 'TlCluster description file.',
                                                        ))
                        
        
                            
    def do(self):
        print('TlClusterExporter.do interfilepath',self.interfilepath)
        # links
        
        legresults = self.parent
        logger = self.get_logger()
        
        fd=open(self.legflowfilepath,'w', encoding="utf-8")
        sep = ','
        
        # dadada

        
        fd.close()
        
        
        
        
        return True
        
