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
#from agilepy.lib_base.misc import get_inversemap
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
        def __init__(self, ident, parent, ids_edge_in_sumo=[], ids_edge_out_sumo=[],legnames=[]):
        
            self._init_objman(  ident=ident, parent = parent, 
                                name = 'Legs',
                                info = "Legs info of intersection "+parent.get_name(),
                                version = 0.0,
                            )   
            
            self.add_col(am.ArrayConf(  'legnames', 
                                            default = '',
                                            dtype = 'object',
                                            groupnames = ['parameters'], 
                                            perm = 'wr', 
                                            is_index = False,
                                            name = 'Leg name',
                                            info = 'Name of leg.',
                                            ))
                                            
            self.add_col(am.ArrayConf( 'ids_edge_in_sumo',
                                            default = '',
                                            dtype = 'object',
                                            is_index = True,
                                            groupnames = ['parameters'], 
                                            name = 'ID incoming', 
                                            info = 'ID of incoming edge.',
                                            ))
            
            self.add_col(am.ArrayConf( 'ids_edge_out_sumo',
                                            default = '',
                                            dtype = 'object',
                                            is_index = True,
                                            groupnames = ['parameters'], 
                                            name = 'ID outgoing', 
                                            info = 'ID of outgoing edge.',
                                            ))
                                            
            if (len(legnames) == len(ids_edge_in_sumo)) & (len(ids_edge_in_sumo) == len(ids_edge_out_sumo)):
                
                ids_leg = self.add_rows(  n=len(legnames),
                                            legnames = legnames,
                                            ids_edge_in_sumo = ids_edge_in_sumo,
                                            ids_edge_out_sumo = ids_edge_out_sumo,
                                         )    
                
            self._init_attributes()
            self._init_constants()
        
        
        def _init_constants(self):
            self.edge_in_to_id = OrderedDict()
            #self.queues_arrival = OrderedDict()
            #self.queues_in = OrderedDict()
            #self.queues_out = OrderedDict()
            
            
            self.vehicles_to_id_leg_in = OrderedDict()
            self.vehicles_to_id_leg_out = OrderedDict()
            
            # 0=aproaching 1=entered analysis area 2= inside junction 3=output buffer 4=exited analysis area
            self.vehicles_state = OrderedDict()
            
            # simulation time when vehicle enters the analysing zone 
            self.vehicles_time_in = OrderedDict()
            
            # simulation time when vehicle enters the intersection itself
            self.vehicles_time_enter = OrderedDict()
            
            # odometer where veh is entering out edge
            self.vehicles_odo_out = OrderedDict()
            
            # odometer where veh is entering the analysis zone
            self.vehicles_odo_in = OrderedDict()
            
            # odometer where veh is entering out edge, not the analysis zone
            self.vehicles_odo_edge_out = OrderedDict()
            
            # odometer where veh is leaving the analysis zone
            self.vehicles_odo_out = OrderedDict()
            
            # simulation time when vehicle exits the intersection itself
            self.vehicles_time_out = OrderedDict()
            
            # waiting times during analyses range
            self.vehicles_time_wait = OrderedDict()
                    
            self.do_not_save_attrs(['vehicles_odo_in','vehicles_odo_edge_out','vehicles_odo_out','edge_in_to_id','vehicles_to_id_leg_in','vehicles_time_in','vehicles_to_id_leg_out','vehicles_state','vehicles_time_enter','vehicles_time_out','vehicles_time_wait'])
        
        
        def _init_attributes(self):
            
            
            self.add_col(am.ArrayConf(  'counts', 0,
                                            dtype=np.int32,
                                            perm='r', 
                                            name = 'Count',
                                            info = "Number of vehicles entering each leg.",
                                        ))
            self.add_col(am.ArrayConf(  'counts_out', 0,
                                            dtype=np.int32,
                                            perm='r', 
                                            name = 'Count',
                                            info = "Number of vehicles leaving through each leg.",
                                        ))
                                        
            
                                        
                                                       
            
                                                                    
            self.add_col(am.ArrayConf(  'speeds', 0.0,
                                            dtype=np.float32,
                                            perm= 'r', 
                                            name = 'Speed',
                                            unit = 'm/s',
                                            info = "Average speeds within the analysed zone for vehicles entering  from the respective leg.",
                                        ))
                                        
            self.add_col(am.ArrayConf(  'speeds_max', 0.0,
                                            dtype = np.float32,
                                            perm= 'r', 
                                            name = 'Speed max',
                                            unit = 'm/s',
                                            info = "Maximum speed within the analysed zone from all vehicles entering from the respective leg.",
                                        ))
            
            self.add_col(am.ArrayConf(  'times_approach', 0.0,
                                            dtype=np.float32,
                                            perm= 'r', 
                                            name = 'Approach time',
                                            unit = 's',
                                            info = "Average aproach time from entering the analysed zone to the actual intersection on respective leg.",
                                        ))
                                        
            
        
        def prepare_sim(self, process):
            print('Legs.prepare_sim',self.ident)
            ids = self.get_ids()
            
            # configure temporary databases
            for id_leg, id_edge_in_sumo,id_edge_out_sumo in zip(ids,self.ids_edge_in_sumo[ids],self.ids_edge_out_sumo[ids]):
                self.edge_in_to_id[id_edge_in_sumo] = id_leg
                
            

        def print_vehicles(self, ids_veh_sumo = None):
            if ids_veh_sumo is None:
                ids_veh_sumo = np.array(list(self.vehicles_state.keys()))
            
            for id_veh_sumo in ids_veh_sumo:
                ts = self.vehicles_time_in[id_veh_sumo]
                te = self.vehicles_time_enter[id_veh_sumo]
                tx = self.vehicles_time_out[id_veh_sumo]
                t1 = te - ts
                t2 = tx - te
                s = self.vehicles_state[id_veh_sumo]
                dist = self.vehicles_odo_out[id_veh_sumo] - self.vehicles_odo_in[id_veh_sumo] 
                print('     id_veh_sumo',id_veh_sumo,'\ts',s,'ts=%d, te=%d, tx=%d, oi=%d, oo=%d'%(ts,te,tx,self.vehicles_odo_in[id_veh_sumo],self.vehicles_odo_out[id_veh_sumo]), end=' ')
                if s == 4:
                    print('dur=%.1f, t1 =%d, t2=%d, tw=%d, dist=%.1f'%(tx-ts,t1,t2,self.vehicles_time_wait[id_veh_sumo],dist))
                else:
                    print('')
        
        def process_vehicles(self, simtime):
            debug = 2
            print('legs.process_vehicles',id(self),self.get_ident_abs())
            #edges = self.parent.parent.get_scenario().net.edges
            ids_leg = self.get_ids()
            ids_veh_sumo = np.array(list(self.vehicles_state.keys()))
            states = np.array(list(self.vehicles_state.values()))
            vehicles_to_id_leg_in = np.array(list(self.vehicles_to_id_leg_in.values()))
            vehicles_to_id_leg_out = np.array(list(self.vehicles_to_id_leg_out.values()))
            
            # get distance from which analyses starts
            dist_ana_in  = self.parent.parent.dist_ana_in.get_value()
            dist_ana_out = self.parent.parent.dist_ana_out.get_value()
            
            # detect stopped vehicles and add waiting time
            speed_stopped = self.parent.parent.speed_stopped.get_value()
            time_update = self.parent.parent.time_update.get_value()
            #for id_veh_sumo in ids_veh_sumo:
            #    # attention: vehicles can disapprea before reaching end of analysis area
            #    try:
            #        if get_traci_velocity(id_veh_sumo) < speed_stopped:
            #            self.vehicles_time_wait[id_veh_sumo] += time_update
            #    except:
            #        pass
            
            # check vehicles in db if time to put from arrival to input queue
            for id_leg, id_edge_in_sumo in zip(ids_leg,self.ids_edge_in_sumo[ids_leg]):
                #print '  check arrivals',id_leg,'id_edge_in_sumo',id_edge_in_sumo
                
                for id_veh_sumo in ids_veh_sumo[(states == 0) & (vehicles_to_id_leg_in == id_leg)]:
                    dist_to_controledge = get_traci_distance(id_veh_sumo, id_edge_in_sumo,0.0) 
                    
                    
                    if dist_to_controledge <= dist_ana_in:
                        # enterer analyses area
                        self.vehicles_state[id_veh_sumo] = 1
                        self.vehicles_time_in[id_veh_sumo] = simtime
                        self.vehicles_odo_in[id_veh_sumo] = get_traci_odo(id_veh_sumo)
                        #print '    to in queue: id_veh_sumo',id_veh_sumo,'id_edge_in_sumo',id_edge_in_sumo,'dist',dist_to_controledge
                   
                
                        

            # check vehicles in db if time to put from input to output queue
            for id_leg, id_edge_in_sumo in zip(ids_leg,self.ids_edge_in_sumo[ids_leg]):
                #print '  check in queue',id_leg,'id_edge_in_sumo',id_edge_in_sumo
                
                for id_veh_sumo in ids_veh_sumo[(states == 1) & (vehicles_to_id_leg_in == id_leg)]:
                    dist_to_controledge = get_traci_distance(id_veh_sumo, id_edge_in_sumo,0.0) 
                    
                    #print '      id_veh_sumo',id_veh_sumo,'dist_to_controledge',dist_to_controledge
                    if dist_to_controledge <= -dist_ana_out:
                        #  passed entry edge, entering inner intersection
                        self.vehicles_state[id_veh_sumo] = 2
                        self.vehicles_time_enter[id_veh_sumo] = simtime
                        
                        #print '    to out queue: id_veh_sumo',id_veh_sumo,'id_edge_in_sumo',id_edge_in_sumo,'dist',dist_to_controledge
                   
                
                   
            # check  vehicles in db arrived at outgoing edge
            for id_leg, id_edge_out_sumo in zip(ids_leg,self.ids_edge_out_sumo[ids_leg]):
                #print '  check out edge',id_leg,'id_edge_out_sumo',id_edge_out_sumo
                
                for id_veh_sumo in ids_veh_sumo[(states == 2) & (vehicles_to_id_leg_out == id_leg)]:
                    id_edge_sumo = get_traci_edge(id_veh_sumo)
                    #print '      id_veh_sumo',id_veh_sumo,'id_edge_sumo',id_edge_sumo,id_edge_sumo == id_edge_out_sumo
                    
                    if id_edge_sumo == id_edge_out_sumo:
                        #  arrived at exit edge, leaving intersection
                        self.vehicles_state[id_veh_sumo] = 3
                        self.vehicles_odo_edge_out[id_veh_sumo] = get_traci_odo(id_veh_sumo)
                        #print '    to exit queue: id_veh_sumo',id_veh_sumo,'id_edge_out_sumo',id_edge_out_sumo
                   
                   
                       
            # check  vehicles in db to remove them from output queue
            for id_leg, id_edge_out_sumo in zip(ids_leg,self.ids_edge_out_sumo[ids_leg]):
                #print '  check out edge',id_leg,'id_edge_in_sumo',id_edge_out_sumo
                
                for id_veh_sumo in ids_veh_sumo[(states == 3) & (vehicles_to_id_leg_out == id_leg)]:
                    # attention: vehicles can disapprea before reaching end of analysis area
                    # =>take care of beforehand when detecting vehicles that left the sim
                    odo = get_traci_odo(id_veh_sumo)
                    dist_to_controledge = self.vehicles_odo_edge_out[id_veh_sumo] - odo
                    #print '      id_veh_sumo',id_veh_sumo,'dist_to_controledge',dist_to_controledge
                    
                    if dist_to_controledge <= 0.0:
                        #  passed exit edge, leaving analysis zone
                        self.vehicles_state[id_veh_sumo] = 4
                        self.vehicles_time_out[id_veh_sumo] = simtime
                        self.vehicles_odo_out[id_veh_sumo] = odo
                        #print '    to exit queue: id_veh_sumo',id_veh_sumo,'id_edge_out_sumo',id_edge_out_sumo,'dist',dist_to_controledge
                   

            #self.print_vehicles(ids_veh_sumo)
        
        def process_results(self, results = None):
            print('legs.process_results')
            # actually results are currently in local database and not processed into the results obj
            
            ids_leg = self.get_ids() 
            intersection = self.parent
            time_ana_start = intersection.parent.time_ana_start.get_value()
            
            
            for id_veh_sumo, state, id_leg_in, id_leg_out, time_in, time_enter, time_out,time_wait, odo_in,odo_out   in\
                zip(    list(self.vehicles_state.keys()), 
                        list(self.vehicles_state.values()), 
                        list(self.vehicles_to_id_leg_in.values()), 
                        list(self.vehicles_to_id_leg_out.values()), 
                        list(self.vehicles_time_in.values()), 
                        list(self.vehicles_time_enter.values()), 
                        list(self.vehicles_time_out.values()), 
                        list(self.vehicles_time_wait.values()), 
                        list(self.vehicles_odo_in.values()), 
                        list(self.vehicles_odo_out.values()), 
                        ):
                if state == 4: # completed analysis area
                    id_edge_in_sumo = self.ids_edge_in_sumo[id_leg_in]
                    id_edge_out_sumo = self.ids_edge_out_sumo[id_leg_out]
                    id_pair = intersection.pair_to_id[(id_edge_in_sumo, id_edge_out_sumo)]
                    duration = float(time_out-time_in)
                    dist = float(odo_out-odo_in)
                    #if duration < 0.01:
                    #    speed = 0.0
                    #else:
                    speed = dist/duration
                        
                    #print '  id_veh_sumo',id_veh_sumo,'time_in',time_in,'time_out',time_out,'speed',speed,
                    #self.print_vehicles([id_veh_sumo])
                    
                    if speed > self.speeds_max[id_leg_in]:
                        self.speeds_max[id_leg_in] = speed
                        
                    if speed > intersection.speeds_max[id_pair]:
                        intersection.speeds_max[id_pair] = speed
                        
                    if duration < intersection.durations_min[id_pair]:
                        intersection.durations_min[id_pair] = duration
                        
                        
                    if time_in >= time_ana_start:       
                        self.counts[id_leg_in] += 1
                        self.counts_out[id_leg_out] += 1
                        
                        intersection.counts[id_pair] += 1
                        intersection.durations[id_pair] += duration
                        #print '  id_veh_sumo',id_veh_sumo,'id_pair',id_pair,'c',intersection.counts[id_pair],'dur=%.1f durCum=%.f durMin=%.f'%(duration,intersection.durations[id_pair], intersection.durations_min[id_pair])
                        
                        self.speeds[id_leg_in] += speed
                        self.times_approach[id_leg_in] += time_enter-time_in
                        #intersection.waits[id_pair] += time_wait
                        intersection.speeds[id_pair] += speed
            
            # average by dividing by counts for each leg
            for id_leg, count in zip(ids_leg, self.counts[ids_leg]) :
                if count > 0:
                    #print '  id',id_leg,'count',count
                    self.speeds[id_leg] /= count
                    self.times_approach[id_leg] /= count
                    
  
                 
        def check_add_veh(self, id_veh_sumo, route, ind_route):
            print('legs.check_add_veh',id_veh_sumo)
            if id_veh_sumo not in self.vehicles_to_id_leg_in:
                routeset = set(route)
                ids_leg = self.get_ids()
                ids_edge_veh_sumo = routeset.intersection(self.ids_edge_in_sumo[ids_leg])
                if len(ids_edge_veh_sumo)>0:
                    id_edge_veh_in_sumo = ids_edge_veh_sumo.pop()
                    ind_in = route.index(id_edge_veh_in_sumo)
                else:
                    id_edge_veh_in_sumo = ""
                    ind_in = 9999
                    
                ids_edge_veh_sumo = routeset.intersection(self.ids_edge_out_sumo[ids_leg])
                if len(ids_edge_veh_sumo)>0:
                    id_edge_veh_out_sumo = ids_edge_veh_sumo.pop()
                    ind_out = route.index(id_edge_veh_out_sumo)
                else:
                    id_edge_veh_out_sumo = ""
                    ind_out = -1
                    
                #print '    ind_route',ind_route,'id_edge_veh_in_sumo',id_edge_veh_in_sumo,ind_in,'id_edge_veh_out_sumo',id_edge_veh_out_sumo,ind_out
                
                if (id_edge_veh_in_sumo != "") & (id_edge_veh_out_sumo != "")&(ind_in < ind_out) & (ind_route < ind_in):
                    id_leg_in = self.ids_edge_in_sumo.get_id_from_index(id_edge_veh_in_sumo)
                    id_leg_out = self.ids_edge_out_sumo.get_id_from_index(id_edge_veh_out_sumo) 
                    
                    self.vehicles_to_id_leg_in[id_veh_sumo] = id_leg_in
                    self.vehicles_to_id_leg_out[id_veh_sumo] = id_leg_out
                    
                    self.vehicles_state[id_veh_sumo] = 0
                    self.vehicles_time_in[id_veh_sumo] = -1.0
                    self.vehicles_time_enter[id_veh_sumo] = -1.0
                    self.vehicles_time_out[id_veh_sumo] = -1.0
                    self.vehicles_time_wait[id_veh_sumo] = -1.0
                    self.vehicles_odo_edge_out[id_veh_sumo] = -1.0
                    self.vehicles_odo_out[id_veh_sumo] = -1.0
                    self.vehicles_odo_in[id_veh_sumo] = -1.0
                    #self.queues_in[id_leg_in].append(id_veh_sumo)
                    #self.queues_out[id_leg_out].append(id_veh_sumo)
                    #self.counts[id_leg_in] += 1
                    
                    # make veh databas (id_from, id_to, time_enter)
                    #print '  added id_veh_sumo',id_veh_sumo,'id_leg_in',id_leg_in,'id_leg_out',id_leg_out
                    
                    return  True
                    
                else:
                    # intersection not on route of vehicle 
                    return False
            else:
                print('WARNING in check_add_veh: vehicle',id_veh_sumo,'already in database')
                return False
                
        def check_del_veh(self, id_veh_sumo):
            #print 'legs.check_del_veh',id_veh_sumo
            # here we delete only vehicles which are in the system but did not complete
            
            if id_veh_sumo in self.vehicles_to_id_leg_in:
                if self.vehicles_state[id_veh_sumo] != 4:
                    del self.vehicles_to_id_leg_in[id_veh_sumo] 
                    del self.vehicles_to_id_leg_out[id_veh_sumo]
                    del self.vehicles_state[id_veh_sumo]
                    del self.vehicles_time_in[id_veh_sumo]
                    del self.vehicles_time_enter[id_veh_sumo]
                    del self.vehicles_time_wait[id_veh_sumo] 
                    del self.vehicles_time_out[id_veh_sumo]
                    del self.vehicles_odo_out[id_veh_sumo]
                    del self.vehicles_odo_edge_out[id_veh_sumo]
                    del self.vehicles_odo_in[id_veh_sumo]
                    
class Intersection(am.ArrayObjman):
        def __init__(self, ident, parent,  name = '',  ids_edge_in_sumo=[], ids_edge_out_sumo=[], legnames=[]):
        
            self._init_objman(  ident=ident, parent = parent, name = name,
                            info = "Intersection instance for intersection "+name,
                            version = 0.0,
                            )
                            
            edges = self.parent.get_scenario().net.edges
            
            self.add(cm.ObjConf(Legs('legs',self,
                                        ids_edge_in_sumo = ids_edge_in_sumo, 
                                        ids_edge_out_sumo = ids_edge_out_sumo,
                                        legnames = legnames
                                        ),
                                groupnames = ['state'], 
                                name = 'Legs', 
                                info = 'Legs object.',
                                ))
                                
            self.add_col(am.ArrayConf(  'pairnames', 
                                            default = '',
                                            dtype = 'object',
                                            groupnames = ['parameters'], 
                                            perm = 'wr', 
                                            is_index = False,
                                            name = 'Leg Pair',
                                            info = 'Name of Pair.',
                                            ))
                                            
            self.add_col(am.ArrayConf( 'ids_edge_in_sumo',
                                            default = '',
                                            dtype = 'object',
                                            is_index = False,
                                            groupnames = ['parameters'], 
                                            name = 'ID incoming', 
                                            info = 'ID of incoming edge.',
                                            ))
            
            self.add_col(am.ArrayConf( 'ids_edge_out_sumo', 
                                            default = '',
                                            dtype = 'object',
                                            is_index = False,
                                            groupnames = ['parameters'], 
                                            name = 'ID outgoing', 
                                            info = 'ID of outgoing edge.',
                                            ))
            
            
            # fill rows with in-out pairs
            if (len(legnames) == len(ids_edge_in_sumo)) & (len(ids_edge_in_sumo) == len(ids_edge_out_sumo)):
                for   legname_from, id_edge_in_sumo in   zip(legnames,   ids_edge_in_sumo, ):
                    for  legname_to, id_edge_out_sumo in  zip(legnames,   ids_edge_out_sumo,):
                        id_pair = self.add_row(   pairnames = legname_from+'>'+legname_to,
                                        ids_edge_in_sumo = id_edge_in_sumo,
                                        ids_edge_out_sumo = id_edge_out_sumo,
                                        )    
                
            self._init_attributes()
            self._init_constants()
        
        
        def _init_constants(self):
            self.pair_to_id = OrderedDict()
            self.queues_in_out = OrderedDict()
            #self.vehicles_in_pairs = OrderedDict()
            ids = self.get_ids()
            
            
            
            # configure temporary databases
            for id_pair, id_edge_in_sumo,id_edge_out_sumo in zip(ids,self.ids_edge_in_sumo[ids],self.ids_edge_out_sumo[ids]):
                        
                        self.pair_to_id[(id_edge_in_sumo, id_edge_out_sumo)] = id_pair
                        self.queues_in_out[(id_edge_in_sumo, id_edge_out_sumo)] = []
                        
            
            
            self.do_not_save_attrs(['pair_to_id','queues_in_out','vehicles_in_pairs'])
        
        def get_legs(self):
            return self.legs.get_value()
        
        def _init_attributes(self):
            self.add_col(am.ArrayConf(  'counts', 0,
                                            dtype=np.int32,
                                            perm='r', 
                                            name = 'Enter count',
                                            info = "Number of vehicles entering between leg pairs.",
                                        ))
                                        
            
                                        
            self.add_col(am.ArrayConf(  'delays', 0.0,
                                            dtype=np.float32,
                                            perm='r', 
                                            name = 'Delay',
                                            unit = 's',
                                            info = "Average delay with respect to free flow conditions within the analysed zone between leg pairs.",
                                        ))
                                        
                                                       
            self.add_col(am.ArrayConf(  'durations', 0.0,
                                            dtype=np.float32,
                                            perm='r', 
                                            name = 'Duration',
                                            unit = 's',
                                            info = "Average travel duration within the analysed zone between leg pairs.",
                                        ))
            
                                        
            self.add_col(am.ArrayConf(  'durations_min', 10.0**6,
                                            dtype = np.float32,
                                            perm = 'r', 
                                            name = 'Min duration',
                                            unit = 's',
                                            info = "Minimum trip duration within the analysed zone from all vehicles entering from the respective leg.",
                                        ))
                                        
            self.add_col(am.ArrayConf(  'speeds', 0.0,
                                            dtype=np.float32,
                                            perm='r', 
                                            name = 'Speed',
                                            unit = 'm/s',
                                            info = "Average speeds within the analysed zone for vehicles entering  from the respective leg.",
                                        ))
                                        
            self.add_col(am.ArrayConf(  'speeds_max', 0.0,
                                            dtype=np.float32,
                                            perm='r', 
                                            name = 'Speed max',
                                            unit = 'm/s',
                                            info = "Maximum speed within the analysed zone for vehicles entering from the respective leg.",
                                        ))
                                        

                    
                    
                                                      
        def check_add_veh(self, id_veh_sumo, route, ind_route):
            legs = self.get_legs()
            
            is_added = legs.check_add_veh(id_veh_sumo, route, ind_route)
            return is_added
        
        def check_del_veh(self, id_veh_sumo):
            legs = self.get_legs()
            # here we delete only vehicles which are in the system but did not complete
            legs.check_del_veh(id_veh_sumo)
        
        def prepare_sim(self, process):
            print('Intersection.prepare_sim',self.ident)
            self.get_legs().prepare_sim(process)
            
        def process_vehicles(self, simtime):
            debug = 2
            print('Intersection.process_vehicles',self.ident)
            #edges = self.parent.get_scenario().net.edges
            #print 49*'.'
            #print '  process_vehicles',self.ident, self.get_name()
            self.get_legs().process_vehicles(simtime)
            #check if new vehicle is less than  dist_ana
            
        def process_results(self, results):
            legs = self.get_legs()
            legs.process_results(results)
            ids_pair = self.get_ids()
            for id_pair, count in zip(ids_pair, self.counts[ids_pair]) :
                if count > 0:
                    print('  id_pair',id_pair,'count',count)
                    
                    self.speeds[id_pair] /= count
                    #self.waits[id_pair] /= count
                    self.delays[id_pair] = (self.durations[id_pair] - count*self.durations_min[id_pair])/count
                    self.durations[id_pair] /= count # average 
                    print('  delay',self.delays[id_pair],self.durations[id_pair]-self.durations_min[id_pair])
        
        def export_to_csv(self,fd, sep):
            legs = self.get_legs()
            legs.process_results(results)
            ids_pair = self.get_ids()
            for id_pair, count, duration, delay in zip(ids_pair, self.counts[ids_pair],self.durations[ids_pair], self.delays[ids_pair], self.speeds[ids_pair], self.waits[ids_pair]) :
                if count > 0:
                    print('  id_pair',id_pair,'count',count)
                    fd.write()
                    
                    
                    
def __init__(self, parent, net, is_add_default=True, **kwargs):
        #print 'VehicleTypes.__init__ is_add_default',is_add_default
        self._init_objman(  ident='vtypes', 
                            parent=parent, 
                            name = 'Vehicle Types', 
                            info = 'Table of all available vehicle types, each with specific physical characteristics. Each vehicle can be used multiple times in the simulation', 
                            xmltag = ('vTypes','vType','ids_sumo'),
                            version = 0.4, 
                            **kwargs
                            )
                            
class IntersectionAnalysis(SimobjMixin,am.ArrayObjman):
        def __init__(   self, ident='interana', simulation=None,
                        name = 'Intersection analysis', info ='Intersection analyser monitors and records results of intersectios identified by a polygon',
                        **kwargs):
            #print('IntersectionAnalyzer.__init__',name,'parent',simulation)
            
            self._init_objman(  ident = ident, 
                                parent = simulation, 
                                name = name, 
                                info = info,
                                )
                                
            # make IntersectionAnalyzer a demand object as link
            # self.get_scenario().demand.add_demandobject(obj = self)
            
            self._init_attributes( **kwargs)
            self._init_constants()
            
        
        
        def get_scenario(self):
            return self.parent.parent
        
        def _init_attributes(self, **kwargs):
            #print 'PrtService._init_attributes',hasattr(self,'prttransit')
            attrsman = self.get_attrsman()
            scenario = self.get_scenario()
            
            
                                        
            self.add(cm.AttrConf(  'time_update', kwargs.get('time_update',1.0),
                                        groupnames = ['options'], 
                                        name = 'Update time',
                                        info = "Analyzis update time.",
                                        unit = 's',
                                        ))
            
            self.add(cm.AttrConf(  'time_ana_start', kwargs.get('time_ana_start',600),
                                        groupnames = ['options'], 
                                        name = 'Start time',
                                        info = "This the time in seconds after when analysis starts.",
                                        unit = 's',
                                        ))
                                                                           
            self.add(cm.AttrConf(  'dist_ana_in', kwargs.get('dist_ana_in',10.0),
                                        groupnames = ['options'], 
                                        name = 'incoming analysis distance',
                                        info = "A vehicle will be monotored if it is positioned less than this distance before the end point incoming edges.",
                                        unit = 'm',
                                        ))
            
            self.add(cm.AttrConf(  'dist_ana_out', kwargs.get('dist_ana_out',10.0),
                                        groupnames = ['options'], 
                                        name = 'outgoing analysis distance',
                                        info = "A vehicle will be monotored if it is positioned less than this distance before the end point incoming edges.",
                                        unit = 'm',
                                        ))
                                                                
            self.add(cm.AttrConf(  'speed_stopped', kwargs.get('speed_stopped',0.1),
                                        groupnames = ['options'], 
                                        name = 'stopping speed',
                                        info = "Speed below which a vehicle is considered being stopped. This means the vehicle is waiting.",
                                        unit = 'm/s',
                                        ))
                                        
            self.add_col(am.ArrayConf( 'names', 
                                        default = '',
                                        dtype = 'object',
                                        groupnames = ['parameters'], 
                                        name = 'Name', 
                                        info = 'Name of intersection.',
                                        ))
                                
            self.add_col(cm.ObjsConf( 'intersections', 
                                        groupnames = ['parameters'], 
                                        name = 'Intersections', 
                                        info = 'Intersection object.',
                                        )) 
                                        
        def _init_constants(self):
            #print 'IntersectionAnalysis._init_constants',self,self.parent
            attrsman = self.get_attrsman()
            self.ids_veh_sumo_laststep = None
            attrsman.do_not_save_attrs(['ids_veh_sumo_laststep'])
         
        def prepare_sim(self, process):
            print('IntersectionAnalysis.prepare_sim',self.ident)
            
            if len(self) > 0:
                ids = self.get_ids()
                for intersection in self.intersections[ids]:
                    intersection.prepare_sim(process)
                
                return [    (self.time_update.get_value(),self.process_step),]   
                    
            else:
                print('  no intersections to analyse')
                return []
            
            
        def add_intersection(self, name = None, ids_edge_in_sumo = [], ids_edge_out_sumo=[], legnames = []):
            id_inter = self.suggest_id()
            if name is None:
                name = str(id_inter)
            print('add_intersection', name)
            if len(ids_edge_in_sumo) > 0:
                
                intersection = Intersection('inter%02d'%id_inter, self, 
                                            name = name,
                                            ids_edge_in_sumo = ids_edge_in_sumo,
                                            ids_edge_out_sumo = ids_edge_out_sumo,
                                            legnames = legnames,
                                            )
                                            
                self.add_row(   _id = id_inter,
                                names = name,
                                intersections = intersection,
                                )
                return intersection
                
            return None
            
        def process_step(self, process):
            simtime = process.simtime
            debug = 2
            ids = self.get_ids() # get all intersection ids
            edges = self.get_scenario().net.edges
            ids_veh_sumo = set(traci.vehicle.getIDList())
            print(79*'=')
            print('IntersectionAnalysis.process_step simtime',simtime)
        
            if self.ids_veh_sumo_laststep is None:
                self.ids_veh_sumo_laststep = copy(ids_veh_sumo)
                ids_veh_sumo_new = set([])
            else:
                ids_veh_sumo_new = ids_veh_sumo.difference(self.ids_veh_sumo_laststep)
                
            

            for id_veh_sumo in ids_veh_sumo_new:
                route = list(traci.vehicle.getRoute(id_veh_sumo))
                ind_route = traci.vehicle.getRouteIndex(id_veh_sumo)
                
                
                for intersection in self.intersections[ids]:
                    #print '  check',intersection.get_name()
                    is_added = intersection.check_add_veh(id_veh_sumo, route, ind_route)
                    #if is_added:
                    #    print '  added id_veh_sumo',id_veh_sumo
                    #    #break
                        
            for id_veh_sumo in self.ids_veh_sumo_laststep.difference(ids_veh_sumo):
                for intersection in self.intersections[ids]:
                    # here we delete only vehicles which are no longer in te simulation and did not complete analyses zone
                    intersection.check_del_veh(id_veh_sumo)
                        
            for intersection in self.intersections[ids]:
                    intersection.process_vehicles(simtime)
            
            # keep list of current vehicles for next step
            self.ids_veh_sumo_laststep = copy(ids_veh_sumo)

            

        

        def process_results(self, results, **kwargs):
            for intersection in self.intersections[self.get_ids()]:
                intersection.process_results(results)
                
        def export_results_csv(self, rootfilepath):
            ids = self.get_ids()
            for _id, intersection in zip(ids, self.intersections[ids]):
                ids_leg = intersection.get_ids()
                ids_valid = intersection.select_ids(intersection.counts.get_value() > 0)
                #print '  intersection %03d.csv'%_id,'ids_valid',ids_valid
                #print '    counts',intersection.counts[ids_leg]
                #print '    counts',intersection.counts.get_value()
                
                intersection.export_csv(rootfilepath+'.inter%03d.csv'%_id, 
                                    sep = ',', 
                                    ids = ids_valid, 
                                    show_parentesis = True, 
                                    #name_id='ID',
                                    is_header = True)

class IntersectionAdder(Process):
    def __init__(self, ident = 'intersectionadder', parent = None, 
                                logger = None, **kwargs):
                                    
        print('Intersection Adder.__init__')
        

           
        self._init_common(  ident, 
                            parent = parent,
                            name = 'Intersection Adder', 
                            logger = logger,
                            info ='Add new intersection structure to database.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        self.intersectionname = attrsman.add(am.AttrConf(  'intersectionname', 
                                    default = '',
                                    perm = 'rw', 
                                    groupnames = ['options'], 
                                    #is_index = True,
                                    name = 'Intersection name',
                                    info = 'Name of intersection or roundabout to be analyzed. Optional data.',
                                    ))
                            
        self.ids_edge_in_sumo = attrsman.add(cm.ListConf('ids_edge_in_sumo',kwargs.get('ids_edge_in_sumo',[]),sep = ',',
                                                        groupnames = ['options'], 
                                                        name = 'SUMO IDs incoming edges',
                                                        info = "SUMO IDs of incoming edges.",
                                                        ))
            
        self.ids_edge_out_sumo = attrsman.add(cm.ListConf('ids_edge_out_sumo',kwargs.get('ids_edge_out_sumo',[]),sep = ',',
                                                        groupnames = ['options'], 
                                                        name = 'SUMO IDs outgoing edges',
                                                        info = "SUMO IDs of outgoing edges.",
                                                        ))
                                                        
        self.legnames = attrsman.add(cm.ListConf('legnames',kwargs.get('legnames',[]),sep = ',',
                                                        groupnames = ['options'], 
                                                        name = 'Leg names',
                                                        info = "Names of each leg, separated by colon.",
                                                        ))    
                            
    def do(self):
        print('ActivityCompleter.do')
        # links
        
        interana = self.parent
        
        logger = self.get_logger()
        logger.w('Add intersection ' + self.intersectionname)
        interana.add_intersection(  name = self.intersectionname, 
                                    legnames = self.legnames, 
                                    ids_edge_in_sumo = self.ids_edge_in_sumo,
                                    ids_edge_out_sumo = self.ids_edge_out_sumo
                                    )
        return True
        
class IntersectionImporter(Process):
    def __init__(self, ident = 'intersectionimporter', parent = None, 
                                logger = None, **kwargs):
                                    
        print('IntersectionImporter.__init__')
        

           
        self._init_common(  ident, 
                            parent = parent,
                            name = 'Intersection Importer', 
                            logger = logger,
                            info ='Import intersection description file to database.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        self.is_clear = attrsman.add(cm.AttrConf(  'is_clear',kwargs.get('is_clear',True),
                                                        groupnames = ['options'],#['options'],#['_private'],
                                                        perm='rw',
                                                        name = 'Clear curtrent intersections',
                                                        info = 'Clear curtrent intersections.',
                                                        ))
                                                        
        self.interfilepath = attrsman.add(cm.AttrConf(  'interfilepath',kwargs.get('interfilepath','intersections.txt'),
                                                        groupnames = ['options'],#['options'],#['_private'],
                                                        perm='rw',
                                                        name = 'Intersection file',
                                                        metatype = 'filepath',
                                                        info = 'Intersection description file.',
                                                        ))
                        
        
                            
    def do(self):
        print('IntersectionImporter.do interfilepath',self.interfilepath)
        # links
        
        interana = self.parent
        logger = self.get_logger()
        
        if self.is_clear:
            interana.clear()
        
        f=open(self.interfilepath,'r', encoding="utf-8")
        sep = ','
        
        i_line = 1
        intername = ''
        ids_edge_in_sumo = []
        ids_edge_out_sumo = []
        legnames = []
        
        for line in f.readlines():
            cols = line.split(sep)
            
            i = 0
            while i < len(cols):
                cols[i] = cols[i].strip()
                i += 1
                
            print('    cols=',cols)
            if len(cols)==1:
                if cols[0] == '':
                    #
                    pass
                else:
                    # new intersection starts with name
                    intername = cols[0]
                        
            else:
                if intername != '':
                    if len(ids_edge_in_sumo) == 0:
                          ids_edge_in_sumo = cols
                    elif len(ids_edge_out_sumo) == 0:
                          ids_edge_out_sumo = cols
                    elif len(legnames) == 0:
                          legnames = cols
                          # description finished insert intersecion
                        
                          logger.w('Add intersection ' + intername)
                          interana.add_intersection(  name = intername, 
                                        legnames = legnames, 
                                        ids_edge_in_sumo = ids_edge_in_sumo,
                                        ids_edge_out_sumo = ids_edge_out_sumo
                                        )
                          intername = ''
                          ids_edge_in_sumo = []
                          ids_edge_out_sumo = []
                          legnames = []

            i_line +=1
        
        f.close()
        
        
        
        
        return True
        
class IntersectionExporter(Process):
    def __init__(self, ident = 'intersectionexporter', parent = None, 
                                logger = None, **kwargs):
                                    
        print('IntersectionExporter.__init__')
        

           
        self._init_common(  ident, 
                            parent = parent,
                            name = 'Intersection Exporter', 
                            logger = logger,
                            info ='Exporter intersection results to text file.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        self.interfilepath = attrsman.add(cm.AttrConf(  'interfilepath',kwargs.get('interfilepath','intersections.txt'),
                                                        groupnames = ['options'],#['options'],#['_private'],
                                                        perm='rw',
                                                        name = 'Intersection file',
                                                        metatype = 'filepath',
                                                        info = 'Intersection description file.',
                                                        ))
                        
        
                            
    def do(self):
        print('IntersectionExporter.do interfilepath',self.interfilepath)
        # links
        
        interana = self.parent
        logger = self.get_logger()
        
        fd=open(self.interfilepath,'w', encoding="utf-8")
        sep = ','
        
        for intersection in interana.intersections[ids]:
            intersection.export_to_csv(fd, sep)

        
        fd.close()
        
        
        
        
        return True
        
