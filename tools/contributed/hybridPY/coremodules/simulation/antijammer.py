
"""
AntijamRouter module

Anti Jam routing prevents traffic jams in real time and modifies routes of vehicles accordingly
"""
import os, sys
import numpy as np
from collections import OrderedDict
from .sumo import traci, tc
from copy import copy
from agilepy.lib_base.processes import Process
#from xml.sax import saxutils, parse, handler

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
#from coremodules.demand.demandbase import DemandobjMixin
from coremodules.simulation.simulationbase import SimobjMixin
from coremodules.simulation.sumo import SumoTraci
from coremodules.network.routing import  get_mincostroute_edge2edge

class Antijammer(SumoTraci):
        def  _init_special(self, **kwargs):
            """
            Special initializations. To be overridden.
            """
            print('Antijammer._init_special')
            attrsman = self.get_attrsman()
            scenario = self.get_scenario()
            
            self.is_enable = attrsman.add(cm.AttrConf( 'is_enable',kwargs.get('is_enable',True),
                                        groupnames = ['options','antijam'], 
                                        name = 'Enable antijam routing',
                                        info = "If True, the antijam process is actively changing routes in order to avoid traffic jams.",
                                        ))
            
            self.time_update_antijammer = attrsman.add(cm.AttrConf( 'time_update_antijammer', kwargs.get('time_update_antijammer',60.0),
                                        groupnames = ['options','antijam'], 
                                        name = 'Antijammer update time',
                                        info = "Antijammer update time. Negative values means disabeled",
                                        unit = 's',
                                        ))
            
            self.is_update_routes_antijam = attrsman.add(cm.AttrConf( 'is_update_routes_antijam',kwargs.get('time_update_antijammer',True),
                                        groupnames = ['options','antijam'], 
                                        name = 'Update routes from antijam',
                                        info = "If True, all routes which have been changed by the anti-jammer will be updated in the scenario.",
                                        ))
            
            self.length_edge_min_antijam = attrsman.add(cm.AttrConf( 'length_edge_min_antijam',kwargs.get('length_edge_min_antijam',60.0),
                                        groupnames = ['options','antijam'], 
                                        name = 'Min edge length for antijam',
                                        unit = 'm',
                                        info = "Minimum length of edge to be controlled by antijam.",
                                        ))
            
            self.const_lowpass_antijam = attrsman.add(cm.AttrConf( 'const_lowpass_antijam',kwargs.get('const_lowpass_antijam',0.5),
                                        groupnames = ['options','antijam'], 
                                        name = 'Update constant',
                                        info = "Low pass filter constant with which antijam parameters are updated.",
                                        ))
                                                                                                                            
            self.factor_edgetime_initial = attrsman.add(cm.AttrConf( 'factor_edgetime_initial',kwargs.get('factor_edgetime_initial',2.0),
                                        groupnames = ['options','antijam'], 
                                        name = 'Initial edgetime factor',
                                        info = "Factor multiplied with the freeflow edge travel times in order to determine the initial edge travel times.",
                                        ))
            self.speedfactor_jam = attrsman.add(cm.AttrConf( 'speedfactor_jam', kwargs.get('speedfactor_jam',0.5),
                                        groupnames = ['options','antijam'], 
                                        name = 'Speedfactor jam',
                                        info = "Speedfactor to detect traffic jams. Jam if average velocity is less than speedfactor* max allowed speed",
                                        ))
                                        
            self.occupancy_jam = attrsman.add(cm.AttrConf( 'occupancy_jam', kwargs.get('occupancy_jam',0.5),
                                        groupnames = ['options','antijam'], 
                                        name = 'Occupancy jam',
                                        info = "If occupancy of an edge is higher than this value, then jam is detected.",
                                        ))    
            
            self.speedfactor_end_of_jam = attrsman.add(cm.AttrConf( 'speedfactor_end_of_jam',kwargs.get('speedfactor_end_of_jam',0.7),
                                        groupnames = ['options','antijam'], 
                                        name = 'Speedfactor end of jam',
                                        info = "Speedfactor to detect end of traffic jams. End of jam Jam if average velocity is greater than speedfactor* max allowed speed",
                                        ))
                                        
            self.occupancy_end_of_jam = attrsman.add(cm.AttrConf( 'occupancy_end_of_jam', kwargs.get('occupancy_end_of_jam',0.3),
                                        groupnames = ['options','antijam'], 
                                        name = 'Occupancy end of jam',
                                        info = "If occupancy of an edge is lower than this value, then jam has vanished.",
                                        ))    
            
            self.occupancy_target = attrsman.add(cm.AttrConf( 'occupancy_target', kwargs.get('occupancy_target',0.7),
                                        groupnames = ['options','antijam'], 
                                        name = 'Occupancy target',
                                        info = "Target occupancy is the edge occupancy that shall be achieved for a congested edge. If negative, the target speed factor is used instead to achieve a target speed on congested edges.",
                                        ))
                                                                    
            self.speedfactor_target = attrsman.add(cm.AttrConf( 'speedfactor_target', kwargs.get('speedfactor_target',0.33),
                                        groupnames = ['options','antijam'], 
                                        name = 'Speedfactor target',
                                        info = "Target speedfactor means that average target edge speed equals to speedfactor* max allowed speed",
                                        ))
                                        
            self.gain_reroute = attrsman.add(cm.AttrConf( 'gain_reroute', kwargs.get('gain_reroute',3.0),
                                        groupnames = ['options','antijam'], 
                                        name = 'Reroute gain',
                                        info = "Reroute gain is the factor used to calculate the target reroute share from the speed factore difference between current speed factor and target speed factor.",
                                        ))                           
            
            self.share_reroute_max = attrsman.add(cm.AttrConf( 'share_reroute_max', kwargs.get('share_reroute_max',0.66),
                                        groupnames = ['options','antijam'], 
                                        name = 'Maximum reroute share',
                                        info = "Maximum share of vehicles approaching a congested edge that can be rerouted."
                                        ))               
                                        
            self.dist_to_edge_congest_min= attrsman.add(cm.AttrConf( 'dist_to_edge_congest_min', kwargs.get('dist_to_edge_congest_min',400.0),
                                        groupnames = ['options','antijam'], 
                                        name = 'Min. reroute distance to congested edge',
                                        unit = 'm',
                                        info = "When vehicle should reroute, distance of vehicle to congested edge must be larger than this distance.",
                                        )) 
            
            self.dist_to_edge_congest_max= attrsman.add(cm.AttrConf( 'dist_to_edge_congest_max', kwargs.get('dist_to_edge_congest_max',5000.0),
                                        groupnames = ['options','antijam'], 
                                        name = 'Max. reroute distance to congested edge',
                                        unit = 'm',
                                        info = "When vehicle should reroute, distance of vehicle to congested edge must be smaller than this distance.",
                                        )) 
                                                                    
            self.timefactor_deviate_accept = attrsman.add(cm.AttrConf( 'timefactor_deviate_accept',kwargs.get('timefactor_deviate_accept',1.5),
                                        groupnames = ['options','antijam'], 
                                        name = 'Accepted deviation time factor',
                                        info = "Accepted factor that increased travel time due to rerouting with respect to free-flow .",
                                        ))
                                        
            self.time_deviate_accept = attrsman.add(cm.AttrConf( 'time_deviate_accept', kwargs.get('time_deviate_accept',900.0),
                                        groupnames = ['options','antijam'], 
                                        name = 'Accepted deviation time',
                                        unit = 's',
                                        info = "Accepted increase in travel time with respect to free-flow, no matter of the factor in travel time icreas..",
                                        ))
            
            
            self.ids_mode_antijam  = scenario.net.modes.names.get_ids_from_indices_save(['passenger','private','motorcycle'])  
            
            
        def prepare_sim_special(self):
            print('Antijammer.prepare_sim_special',self.is_enable)
            # overridden by antijammer 
            # NO SUBSCRIPTION!!!
            # will pass values at each sim step, and slows down sim 
            attrsman = self.get_attrsman()
            scenario = self.get_scenario()
            
            
            
            if self.is_enable:
            
                
                edges = scenario.net.edges
                ids_edge = edges.get_ids()
                
                if not hasattr(self,'ids_edge_antijam'):
                    # first time antijam is called
                    # it is a special preparation under Traci, add it to the simobjects directly and do not return 
                    self.simobjects.append([self.simtime_start, self.time_update_antijammer, self.process_step_antijammer])
                    #print '  self.simobjects',self.simobjects
                
                ids_edge_antijam  = set([])
                for id_mode in self.ids_mode_antijam:
                    ids_edge_antijam.update(ids_edge[  np.logical_and(edges.get_accesslevels(id_mode)[ids_edge]>=0 , edges.lengths[ids_edge] > self.length_edge_min_antijam)])
                
                
                self.ids_edge_antijam = np.array( list(ids_edge_antijam), dtype = np.int32)
                
                #spam = OrderedDict([('s',(1,2)),('p',(3,4)),('a',(5,6)),('m',(7,8))])
                self.edges_congest = OrderedDict() # id_edge_sumo is key, dict with attributes is value
                #self.speeds_edge_congest = [] 
                self.vehicles_in_edges_congest = OrderedDict() # id_veh_sumo, list with congested edges as value
                self.vehicles_routes_congest = OrderedDict() 
                self.vehicles_rerouted = OrderedDict() 
                self.ids_veh_sumo_laststep = None
                self._timesmap = {}
                self._fstarmap = {}
                self.edgetimes = edges.get_times(id_mode = self.ids_mode_antijam[0])# used for quick routecost calc
                self.traveltime_infinity = 10.0**8
                attrsman.do_not_save_attrs(['edgetimes','traveltime_infinity','edges_congest','vehicles_in_edges_congest','vehicles_routes_congest','_timesmap','_fstarmap'])
            
                
           

        
        def process_step_antijammer(self, process):
            simtime = process.simtime
            debug = 2
            edges = self.get_scenario().net.edges
            vtypes = self.get_scenario().demand.vtypes
            ids_edge = self.ids_edge_antijam 
            ids_mode = list(self._timesmap.keys())
            # get all vehicles currently in simulation
            ids_veh_sumo = set(traci.vehicle.getIDList())
            print(79*'_')
            print('Antijammer at',simtime,'edges',len(self.edges_congest),len(ids_edge),len(edges),'veh',len(self.vehicles_in_edges_congest),len(ids_veh_sumo))
            
            
            
            
            
            #print '  n_veh',len(ids_veh_sumo)
            
            # identify newly inserted vehicles
            if self.ids_veh_sumo_laststep is None:
                self.ids_veh_sumo_laststep = copy(ids_veh_sumo)
                ids_veh_sumo_new = set([])
            else:
                ids_veh_sumo_new = ids_veh_sumo.difference(self.ids_veh_sumo_laststep)
                
            # TODO: remove all vehicles from database that finished their trip
            #id_veh_debug_sumo = u'1061'
            #ids_veh_disappeared_sumo = self.ids_veh_sumo_laststep.difference(ids_veh_sumo)
            #print '  finished trips',ids_veh_disappeared_sumo
            #print '    debug id_veh_sumo',id_veh_debug_sumo,'last',id_veh_debug_sumo in self.ids_veh_sumo_laststep,'now', id_veh_debug_sumo in ids_veh_sumo, id_veh_debug_sumo in ids_veh_disappeared_sumo
            for id_veh_sumo in self.ids_veh_sumo_laststep.difference(ids_veh_sumo):
                self.del_veh(id_veh_sumo)
            
            for id_edge, id_edge_sumo, speed_max in zip(ids_edge, edges.ids_sumo[ids_edge],edges.speeds_max[ids_edge]):
                
                #traci.edge.getEffort(id_edge_sumo, simtime)
                # getLastStepHaltingNumber(self, edgeID)
                speed = traci.edge.getLastStepMeanSpeed(id_edge_sumo)
                occupancy = traci.edge.getLastStepOccupancy(id_edge_sumo)
                # getLastStepVehicleNumber(self, edgeID)
                # getTraveltime(self, edgeID)
                # getWaitingTime(self, edgeID) # sum of wait of all veh 
                
                #if id_edge_sumo == 'D4D1.1976':
                #    print '   DEBUG id_edge_sumo',id_edge_sumo,'speed',speed,speed_max,'occupancy',occupancy
                #    if id_edge_sumo in self.edges_congest:
                #        print '  ids_veh',self.edges_congest[id_edge_sumo]['ids_veh']
                
                if debug > 5:
                    print('  id_edge_sumo %s v=%.1f v_max=%.1f speedfac=%.1f, occ=%.2f'%(id_edge_sumo,speed,speed_max,speed/speed_max,occupancy),(speed < self.speedfactor_jam * speed_max) , (occupancy > self.occupancy_jam ))
                
                if (speed < self.speedfactor_jam * speed_max) & (occupancy > self.occupancy_jam ):
                    if debug > 5:   
                        print('  >congested id_edge_sumo',id_edge_sumo,speed,speed_max,'occ',occupancy,'in db',id_edge_sumo not in self.edges_congest)
                    if id_edge_sumo not in self.edges_congest:
                        if debug > 5:  
                            print('    =>added')
                        self.edges_congest[id_edge_sumo] = {'ids_veh':[],
                                                            'are_rerouted':[],
                                                            'odos':[],
                                                            # here put values at the limit case to avoid shocks 
                                                            'speedfac': self.speedfactor_jam, #speed/speed_max,
                                                            'occupancy': self.occupancy_jam,  #occupancy,
                                                            }
                        #self.ids_edge_sumo_congest.append(id_edge_sumo)
                        #self.speeds_edge_congest.append(speed)
                        # go though all vehicles (not just the new vehicles) and add those with e route through congested edge
                        for id_veh_sumo in ids_veh_sumo:# 
                            self.check_add_veh(id_veh_sumo, id_edge_sumo)
                            #if is_added:
                            #    print '    added new id_veh_sumo',id_veh_sumo,'to id_edge_sumo',id_edge_sumo
                            #    print '      edge congested',self.vehicles_in_edges_congest[id_veh_sumo]
                            
                        
                            
                        
                
                
                # update edge travel times
                # attention: only edge times with a minimum length are updated, rest are freeeflow
                if id_edge_sumo in self.edges_congest:
                        # always update edge speed, used later
                        
                        #ind_edge = self.edges_congest.keys().index(id_edge_sumo)
                        
                        # 
                        #self.speeds_edge_congest[ind_edge] = speed
                        
                        # leave congested eges at their current travel time
                        #for id_mode in self._timesmap.keys():
                        #    self._timesmap[id_mode][id_edge] = self.traveltime_infinity
                        
                        self.edges_congest[id_edge_sumo]['speedfac'] = self.const_lowpass_antijam* self.edges_congest[id_edge_sumo]['speedfac'] + (1.0-self.const_lowpass_antijam) * speed/speed_max
                        self.edges_congest[id_edge_sumo]['occupancy'] =  self.const_lowpass_antijam* self.edges_congest[id_edge_sumo]['occupancy'] + (1.0-self.const_lowpass_antijam) * occupancy
                        print('    updated id_edge_sumo',id_edge_sumo,'with speedf %.2fav, %.2f,  occ %.2fav, %.2f'%(self.edges_congest[id_edge_sumo]['speedfac'],speed/speed_max,self.edges_congest[id_edge_sumo]['occupancy'],occupancy)) 
                
                        # now check here if the averaged speedfactor and occupancy still indicate congestion
                        # and if not, remove this link
                        if (self.edges_congest[id_edge_sumo]['speedfac'] > self.speedfactor_end_of_jam) & (self.edges_congest[id_edge_sumo]['occupancy'] < self.occupancy_end_of_jam ):
                            # remove from list when passed the link or arrived
                            if debug > 5:
                                print('  >uncongested again id_edge_sumo',id_edge_sumo, 'remove from database')
                                print('    ids_veh',self.edges_congest[id_edge_sumo]['ids_veh'])
                            #ind_edge = self.edges_congest.keys().index(id_edge_sumo)
                            #self.speeds_edge_congest.pop(ind_edge)
                            
                            for id_veh_sumo in copy(self.edges_congest[id_edge_sumo]['ids_veh']):
                                #print '    delete now id_veh_sumo',id_veh_sumo
                                self.del_veh_from_edge_from_edge(id_veh_sumo, id_edge_sumo, data = None)
                                #print '    deleted id_veh_sumo',id_veh_sumo
                                
                            # reset edgetraveltimes for routing
                            id_edge_rem = edges.ids_sumo.get_id_from_index(id_edge_sumo)
                            for id_mode in ids_mode:
                                self._timesmap[id_mode][id_edge_rem] = traci.edge.getTraveltime(id_edge_sumo)
                                
                            del self.edges_congest[id_edge_sumo]
                        
                        else:
                           if debug > 5:
                                if id_edge_sumo in self.edges_congest:
                                    print('  >uncongested id_edge_sumo',id_edge_sumo, 'remains in DB')
                                else:
                                    print('  >uncongested id_edge_sumo',id_edge_sumo, 'not in DB')
                
                
                else:
                        # update of not (yet) congested links
                        for id_mode in ids_mode:
                            self._timesmap[id_mode][id_edge] = self.const_lowpass_antijam* self._timesmap[id_mode][id_edge] + (1.0-self.const_lowpass_antijam) * traci.edge.getTraveltime(id_edge_sumo)
                
                
            # update edge travel times with new congested edges
            #self.update_weights()
            
            
            # update newly inserted and removed vehicles
            for id_edge_sumo, data in self.edges_congest.items():
                # identify vehicles that passed the congested edge
                # use the odometer to detect initial and current distance to edge
                ids_veh_remove = []
                for id_veh_sumo, odo in zip(data['ids_veh'], data['odos']):
                    if traci.vehicle.getDistance(id_veh_sumo) > odo:
                        # odometer has reached the target congested edge
                        ids_veh_remove.append(id_veh_sumo)
                
                # remove those vehicles
                for id_veh_sumo in ids_veh_remove:
                    self.del_veh_from_edge_from_edge(id_veh_sumo, id_edge_sumo, data)
                    
                # add newly inserted vehicles that have a route through this edge
                for id_veh_sumo in ids_veh_sumo_new:
                    self.check_add_veh(id_veh_sumo, id_edge_sumo)
                    #if is_added:
                    #    print '    added new id_veh_sumo',id_veh_sumo,'to id_edge_sumo',id_edge_sumo
                    #    print '      edge congested',self.vehicles_in_edges_congest[id_veh_sumo]
                    
            # now we have an updated list of vehicles approaching
            
            # strategy:
            # for each congested link, keep a list of approaching vehicles 
            ind_edge = 0
            for id_edge_sumo, data in self.edges_congest.items():
                # need some info on the dist to edge and whether already rerouted
                #if debug > 0:
                #    print '\n  check vehicles for congested id_edge_sumo',id_edge_sumo,'speedfac %.2f %d veh'%(data['speedfac'],len(data['ids_veh']))
                #    #print '    V:',data['ids_veh']
                #    #print '    R:',data['are_rerouted']
                    
                
                # the congested edge id_edge_sumo
                n_veh = len(data['ids_veh'])
                
                
                    
                
                    
                if n_veh > 0:
                    # arg sort vehicles by distance to congested edge
                    dists_to_edge_congest = np.zeros(n_veh, dtype = np.float32)
                    for ind, id_veh_reroute_sumo, odo in  zip(range(n_veh), data['ids_veh'], data['odos']):
                        dists_to_edge_congest[ind] = odo - traci.vehicle.getDistance(id_veh_reroute_sumo)
                    inds_sorted = np.argsort(dists_to_edge_congest)
                    
                    
                    # reroute a defined share of approaching vehicles and note as rerouted
                    if self.occupancy_target > 0:
                        delta = data['occupancy'] - self.occupancy_target
                    else:
                        delta = self.speedfactor_target - data['speedfac']
                    
                    share_reroute_target = min(max(0, self.gain_reroute * delta), self.share_reroute_max)
                    
                    share_rerouted = float(np.sum(data['are_rerouted']))/float(n_veh)
                    if debug > 0:
                        print(70*'.')
                        print('   edge %s:speedfac=%.2f,occ=%.1f,delta=%.2f,share*=%.2f,share=%.2f, %d veh'%(id_edge_sumo,data['speedfac'],data['occupancy'], delta,share_reroute_target,share_rerouted,n_veh))
                    
                    # reroute vehicles in queue until rerouting share is reached
                    n_trials_max = n_veh
                    ind = 0
                    while (share_rerouted < share_reroute_target) & (ind < n_trials_max):
                        
                        # this will get the vehicle index , sorted from small to higher distances to congested edge
                        #ind_veh = inds_sorted[ind%n_veh]
                        ind_veh = inds_sorted[ind]
                        
                        if not data['are_rerouted'][ind_veh]:
                            # vehicle has not been rerouted already
                            id_veh_reroute_sumo = data['ids_veh'][ind_veh]
                            
                            
                            #route_old = traci.vehicle.getRoute(id_veh_reroute_sumo)
                            route_old = self.vehicles_routes_congest[id_veh_reroute_sumo]
                            id_edge_current_sumo = traci.vehicle.getRoadID(id_veh_reroute_sumo)
                            #print '\n    check reroute id_veh_sumo',id_veh_reroute_sumo,'id_edge_current_sumo',id_edge_current_sumo,'on route',id_edge_current_sumo in route_old,'dist',data['odos'][ind_veh]-traci.vehicle.getDistance(id_veh_reroute_sumo)
                            if (id_edge_current_sumo in route_old)&(not self.vehicles_rerouted[id_veh_reroute_sumo]):
                                # current edge is route, this is not the case if the vehicle is on a junction
                                # if vehicle is on a junction, it cannot be rerouted and throws anerror when setting route
                                #dist_to_edge_congest = data['odos'][ind_veh]-traci.vehicle.getDistance(id_veh_reroute_sumo)
                                dist_to_edge_congest = dists_to_edge_congest[ind_veh]
                                id_edge_current = edges.ids_sumo.get_id_from_index(id_edge_current_sumo)
                                    
                                dist_to_end_edge_current = edges.lengths[id_edge_current] - traci.vehicle.getLanePosition(id_veh_reroute_sumo)
                                
                                
                                #traci.vehicle.rerouteTraveltime(id_veh_reroute_sumo, currentTravelTimes=True)
                                #route = traci.vehicle.getRoute(id_veh_reroute_sumo)
                                #print '  check id_veh_sumo',id_veh_reroute_sumo,'id_edge_current',id_edge_current,'dist_to_end_edge_current',dist_to_end_edge_current,edges.lengths[id_edge_current] ,traci.vehicle.getLanePosition(id_veh_reroute_sumo),'occ',traci.edge.getLastStepOccupancy(id_edge_current_sumo) 
                                
                                #ind_edge_current = traci.vehicle.getRouteIndex(id_veh_reroute_sumo)
                                ind_edge_current = route_old.index(id_edge_current_sumo)
                                ind_edge_congested = route_old.index(id_edge_sumo)
                                
                                # in order to prevent deadlocks at the end of the edge where vehicles cannot change lanes,
                                # rerouting can take place only if the distance to end of current edge is more than 20
                                # and the current congection level is less than 70% 
                                if (dist_to_end_edge_current > 40.0)&(traci.edge.getLastStepOccupancy(id_edge_current_sumo) < 0.7) & (ind_edge_current < ind_edge_congested) & (dist_to_edge_congest > self.dist_to_edge_congest_min):
                                
                                    
                                    #print '    start routing'
                                    id_edge_dest = edges.ids_sumo.get_id_from_index(route_old[-1])
                                    #route = traci.simulation.findRoute( id_edge_current_sumo, route_old[-1], vType=traci.vehicle.getTypeID(id_veh_reroute_sumo), depart=-1.0, routingMode=0).edges
                                    
                                    
                                    id_type = vtypes.ids_sumo.get_id_from_index(traci.vehicle.getTypeID(id_veh_reroute_sumo))
                                    id_mode = vtypes.ids_mode[id_type]
                                    #print '  id_type',id_type,traci.vehicle.getTypeID(id_veh_reroute_sumo),'id_mode',id_mode
                                    cost, ids_edge_new = get_mincostroute_edge2edge(   id_edge_current, id_edge_dest,
                                                                                    weights = self.get_weighs(id_mode), 
                                                                                    fstar = self.get_fstar(id_mode),
                                                                                    )
                                    route = edges.ids_sumo[ids_edge_new].tolist()
                                    
                                    # calculate deviated travel time with respect to previous travel time
                                    ids_edge_old = edges.ids_sumo.get_ids_from_indices(route_old)
                                    traveltime_old = np.sum(self.edgetimes[ids_edge_old[ind_edge_current:]])
                                    traveltime_new = np.sum(self.edgetimes[ids_edge_new])
                                    timefactor_deviate = traveltime_new/traveltime_old
                                    time_deviate = traveltime_new - traveltime_old
                                    if debug > 5:
                                        print('     >try reroute id_veh_sumo',id_veh_reroute_sumo,'avoid', id_edge_sumo not in route)
                                        #print '        ind_edge_current,',ind_edge_current,len(route_old)
                                        print('        ids_edge_old',ids_edge_old[ind_edge_current:])
                                        print('        ids_edge_new',ids_edge_new)
                                        #print '        weights',type(self.get_weighs(id_mode)),self.get_weighs(id_mode)[route_old[ind_edge_current:]]
                                        print('        timefactor_deviate',timefactor_deviate)
                                        print('        time_deviate',time_deviate)
                                        print('        traveltime_old',traveltime_old)
                                        print('        traveltime_new',traveltime_new)
                                        print('        route_old',route_old[:ind_edge_current-1])
                                        print('        route_new',route)
                                        
                                        
                                    # TODO: check also if vehicle is able to take the new route in time
                                    
                                    if (id_edge_sumo not in route) & ((timefactor_deviate < self.timefactor_deviate_accept) | (time_deviate < self.time_deviate_accept)):
                                        # check if new route also avoids more congested links than old route
                                        ids_edge_congested_by_veh_sumo = set(self.vehicles_in_edges_congest[id_veh_reroute_sumo])
                                        #print '      congested old',ids_edge_congested_by_veh_sumo
                                        n_edge_congested_old = len(ids_edge_congested_by_veh_sumo)
                                        ids_edge_congested_by_veh_new_sumo = set(route).intersection(ids_edge_congested_by_veh_sumo)
                                        #print '      congested new',ids_edge_congested_by_veh_new_sumo
                                        n_edge_congested_new = len(ids_edge_congested_by_veh_new_sumo)
                                        #print '      n_edge_congested',n_edge_congested_old,n_edge_congested_new,n_edge_congested_new<n_edge_congested_old,ids_edge_congested_by_veh_sumo,ids_edge_congested_by_veh_new_sumo
                                        
                                        
                                        if 1: #n_edge_congested_new < n_edge_congested_old:
                                            # rerouting resulted in a reduced number of congested edges
                                            # go through all congested edges where the vehicle is no longer involved
                                            # and set it as rerouted
                                            #print '      de congested',ids_edge_congested_by_veh_sumo.difference(ids_edge_congested_by_veh_new_sumo)
                                            for id_edge_sumo_decongested in ids_edge_congested_by_veh_sumo.difference(ids_edge_congested_by_veh_new_sumo):#set of decongested edges:
                                                # if debug > 5:
                                                #print '        set id_veh_sumo',id_veh_reroute_sumo,'rerouted for congested id_edge_sumo',id_edge_sumo_decongested
                                                ind_veh = self.edges_congest[id_edge_sumo_decongested]['ids_veh'].index(id_veh_reroute_sumo)
                                                self.edges_congest[id_edge_sumo_decongested]['are_rerouted'][ind_veh] = True
                                                #print '        V',self.edges_congest[id_edge_sumo_decongested]['ids_veh']
                                                #print '        R',self.edges_congest[id_edge_sumo_decongested]['are_rerouted']
                                            #print '        id_edge_current_sumo',id_edge_current_sumo,traci.vehicle.getRoadID(id_veh_reroute_sumo),route
                                            traci.vehicle.setRoute(id_veh_reroute_sumo, route)
                                            #c = traci.vehicle.getColor(id_veh_reroute_sumo)
                                            #traci.vehicle.setColor(id_veh_reroute_sumo, (int(0.7*c[0]),int(0.7*c[1]),int(0.7*c[2]),255)) 
                                            traci.vehicle.setColor(id_veh_reroute_sumo, (255,30,30,255)) 
                                            # adjust part of old route
                                            route_old_remain = route_old[:ind_edge_current]
                                            #print '  route_old_remain',route_old_remain
                                            
                                            #print '  >>set route ',ind_edge_current,route_old_remain+route
                                            
                                            
                                            self.vehicles_routes_congest[id_veh_reroute_sumo] = route_old_remain + route
                                            self.vehicles_rerouted[id_veh_reroute_sumo]= True
                                            #print '  >>set route id_veh_sumo',id_veh_reroute_sumo
                                            #print '  from',route_old
                                            #print '    to',self.vehicles_routes_congest[id_veh_reroute_sumo]
                                            
                                            #print '  check after set',self.vehicles_routes_congest[id_veh_reroute_sumo]
                                            share_rerouted = float(np.sum(data['are_rerouted']))/float(n_veh)
                                            #print '     successfully rerouted id_veh_sumo',id_veh_reroute_sumo,'share_rerouted',share_rerouted
                                        
                                        else:
                                            pass
                                            #print '      rerouting does not decrese the number of congested edge',id_edge_sumo,'avoid',n_edge_congested_new,'old' ,n_edge_congested_old
                                            #print '        route',route
                                            #print '        dist_to_edge_congest',dist_to_edge_congest
                                            #print '        odo',data['odos'][ind_veh],traci.vehicle.getDistance(id_veh_reroute_sumo)
                                          
                                            
                                    else:
                                        pass
                                        #print '      unsuccessfully rerouted id_veh_sumo',id_veh_reroute_sumo
                                        #print '        route',route
                                        #print '        dist_to_edge_congest',dist_to_edge_congest
                                        #print '        odo',data['odos'][ind_veh],traci.vehicle.getDistance(id_veh_reroute_sumo)
                                        
                                else:
                                    pass
                                    #print '      unsuccessfully rerouted id_veh_sumo',id_veh_reroute_sumo
                                    #print '        too late to deviate ind_edge_current',ind_edge_current,'ind_edge_congested',ind_edge_congested
                                    #print '        dist_to_edge_congest',dist_to_edge_congest
                                    
                            else:
                                pass
                                #print '      unsuccessfully rerouted id_veh_sumo',id_veh_reroute_sumo
                                #print '        vehicle is on a junction',traci.vehicle.getRoadID(id_veh_reroute_sumo)
                       
                        ind += 1
                        #print '     ind',ind,'rerouted',np.sum(data['are_rerouted']),'share_rerouted %.2f'%share_rerouted,'OK',share_rerouted>=share_reroute_target
                else:
                    pass
                    #print 'WARNING: strange, no more vehicles approaching congested edge!'
                
                #if debug > 9:
                #    print '\n  check vehicles for congested id_edge_sumo',id_edge_sumo
                #    print '    V:',data['ids_veh']
                #    print '    R:',data['are_rerouted']
                    
                ind_edge += 1
            #print '  debug: id_veh_sumo 217',

            self.ids_veh_sumo_laststep = copy(ids_veh_sumo)
                    
        def del_veh(self, id_veh_sumo):    
            """Check if vehicle exists, remove from edge database and delete completely, if not rerouted"""
            #print 'del_veh id_veh_sumo',id_veh_sumo,id_veh_sumo in self.vehicles_in_edges_congest
            if id_veh_sumo in self.vehicles_in_edges_congest:
                #print '    edges_congest',self.vehicles_in_edges_congest[id_veh_sumo],'rr',self.vehicles_rerouted[id_veh_sumo]
                for id_edge_sumo in copy(self.vehicles_in_edges_congest[id_veh_sumo]):
                    self.del_veh_from_edge_from_edge(id_veh_sumo, id_edge_sumo, is_del_veh = False)
                
                if not self.vehicles_rerouted[id_veh_sumo]:
                    #print 'del_veh: completely remove id_veh_sumo',id_veh_sumo
                    self._del_veh_data(id_veh_sumo)
        
        def del_veh_from_edge_from_edge(self, id_veh_sumo, id_edge_sumo, data = None, is_del_veh = True):    
            """Check is vehicle is on edge id_edge_sumo and remove, if true
            Will return True if vehicle has been rerouted, otherwise False.
            """
            #print 'del_veh_from_edge id_veh_sumo',id_veh_sumo,'id_edge_sumo',id_edge_sumo,id_edge_sumo in self.edges_congest
            if data is None:
                data = self.edges_congest[id_edge_sumo]
            if id_veh_sumo in data['ids_veh']:
                ind_veh = data['ids_veh'].index(id_veh_sumo)
                id_veh_sumo_check = data['ids_veh'].pop(ind_veh)
                data['are_rerouted'].pop(ind_veh)

                #print '    removed id_veh_sumo',id_veh_sumo_check,'from',id_edge_sumo,'rr',self.vehicles_rerouted[id_veh_sumo]
                
                data['odos'].pop(ind_veh)
                self.vehicles_in_edges_congest[id_veh_sumo].remove(id_edge_sumo) 
                
                if is_del_veh & (not self.vehicles_rerouted[id_veh_sumo]):
                    if len(self.vehicles_in_edges_congest[id_veh_sumo]) == 0:
                        # vehicle is no longer on any congested edges
                        #print '    completely remove id_veh_sumo',id_veh_sumo_check
                        self._del_veh_data(id_veh_sumo)
                
            
                
        def _del_veh_data(self, id_veh_sumo):
            # remove vehicle 
            del self.vehicles_in_edges_congest[id_veh_sumo]
            del self.vehicles_routes_congest[id_veh_sumo]  
            del self.vehicles_rerouted[id_veh_sumo]
            #print '    del_veh_from_edge_from_edge: removed id_veh_sumo',id_veh_sumo,'from id_edge_sumo',id_edge_sumo
                    
        def check_add_veh(self, id_veh_sumo, id_edge_sumo):
            """Checks whether to id_veh_sumo runs through congested edge id_edge_sumo
               if true, add the vehicle to the edge's queue
            """
            debug = 1
            vtypes = self.get_scenario().demand.vtypes
            id_type = vtypes.ids_sumo.get_id_from_index(traci.vehicle.getTypeID(id_veh_sumo))
            id_mode = vtypes.ids_mode[id_type]
            
            if id_mode in self.ids_mode_antijam:
                is_in_database = id_veh_sumo  in self.vehicles_in_edges_congest
                if is_in_database:
                    route = self.vehicles_routes_congest[id_veh_sumo]
                else:
                    route = list(traci.vehicle.getRoute(id_veh_sumo)) # attention getRoute returns tuple
                
                if (id_edge_sumo in route) & (id_veh_sumo not in self.edges_congest[id_edge_sumo]['ids_veh']):
                    id_edge_current_sumo = traci.vehicle.getRoadID(id_veh_sumo)
                    edges = self.get_scenario().net.edges
                    length_edge = edges.lengths[edges.ids_sumo.get_id_from_index(id_edge_sumo)]
                    dist_to_edge_congest = traci.vehicle.getDrivingDistance(id_veh_sumo, id_edge_sumo, 0.0)# - length_edge
                    if debug > 5:
                        print('    check_add_veh: id_veh_sumo',id_veh_sumo,'on',id_edge_sumo,'currently',id_edge_current_sumo,'dist_to_edge_congest',dist_to_edge_congest)
                        print('      in route', id_edge_sumo in route,'route',route)
                        
                    if (dist_to_edge_congest > self.dist_to_edge_congest_min)&(id_edge_current_sumo != id_edge_sumo):
                        # distance is far enough from congested edge and it is not on congested edge
                        if id_edge_current_sumo in route:
                            ind_edge_congested = route.index(id_edge_sumo)
                            #TODO: traci.vehicle.getRouteIndex( vehID)
                            ind_edge_current = route.index(traci.vehicle.getRoadID(id_veh_sumo))
                            if debug > 5:
                                print('     found id_veh_sumo',id_veh_sumo,'ind_edge_congested',ind_edge_congested,ind_edge_current,ind_edge_current < ind_edge_congested)
                            is_add = ind_edge_current < ind_edge_congested - 1 # < we want to have at least 1 edge between old and new route to allow for lane change
                        else:
                            # this happens if vehicle is on an internal edge
                            # add anyway if veh is after the congested edge it will get eliminated later
                            is_add = True
                    else:
                        is_add = False
                        if debug > 5:
                                print('     too late for rerouting, dist too close to congested edge')
                        
                        
                    #print '       ids_veh',self.edges_congest[id_edge_sumo]['ids_veh']
                    if is_add:
                        #odo = np.array( self.edges_congest[id_edge_sumo]['odos'], dtype = np.float32)
                        
                        self.edges_congest[id_edge_sumo]['ids_veh'].append(id_veh_sumo)
                        self.edges_congest[id_edge_sumo]['are_rerouted'].append(False)
                        self.edges_congest[id_edge_sumo]['odos'].append( traci.vehicle.getDistance(id_veh_sumo)+dist_to_edge_congest)
                        
                        if is_in_database:
                            # add this edge to an already existing vehicle
                            self.vehicles_in_edges_congest[id_veh_sumo].append(id_edge_sumo)
                        
                        else:
                            # vehicle has not yet passed by the congested edge
                            self.vehicles_in_edges_congest[id_veh_sumo] = [id_edge_sumo]
                            self.vehicles_routes_congest[id_veh_sumo] = route
                            self.vehicles_rerouted[id_veh_sumo]= False
                        if debug > 5:
                            print('      check_add_veh: added id_veh_sumo',id_veh_sumo,'to %s in %.2fm'%(id_edge_sumo,dist_to_edge_congest))
                            
                        return True
                    
                    else:
                        # too close or after to congested edge
                        return False  
                
                else:
                    # congested edge not in route    
                    return False  
            
            # wrong mode
            return False
        
        
        
        def get_weighs(self, id_mode, is_check_lanes = True):
            """
            Returns edge weighs for id_mode 
            """
            #print 'get_weighs_and_fstar'
            
            
            if id_mode not in self._timesmap:
                edges = self.get_scenario().net.edges
                self._timesmap[id_mode] = self.factor_edgetime_initial * edges.get_times(  id_mode = id_mode,
                                                                is_check_lanes = is_check_lanes,
                                                                )   
                self.update_weights( id_mode = id_mode)
       
            
            return self._timesmap[id_mode]
        
        def update_weights(self, id_mode = None):
            if id_mode is None:
                ids_mode = list(self._timesmap.keys())
            else:
                ids_mode = [id_mode]
                
            ids_edge_congest = self.get_scenario().net.edges.ids_sumo.get_ids_from_indices(list(self.edges_congest.keys()))
            
            for id_mode in ids_mode:
                self._timesmap[id_mode][ids_edge_congest] = 10.0**8
            
        
        def get_fstar(self, id_mode,  is_return_arrays = True, is_ignor_connections = False):
            """
            Returns edge  fstar for id_mode 
            """
            #print 'get_fstar'
            
            
            if id_mode not in self._fstarmap:   
                edges = self.get_scenario().net.edges   
                self._fstarmap[id_mode] = edges.get_fstar(  id_mode = id_mode,
                                                                is_ignor_connections = is_ignor_connections,
                                                                is_return_arrays = is_return_arrays,
                                                            )
            
            return self._fstarmap[id_mode]                                             
        
        def finish_sim(self):
            # change routes
            
            # detect and remove routes that have not been changed 
            #vehicles_routes_unchanged = []
            #for id_veh_sumo, ids_edge_cong_sumo in self.vehicles_in_edges_congest.items():
            #    print '  check id_veh_sumo',id_veh_sumo,ids_edge_cong_sumo
            #    is_rerouted = False
            #    for id_edge_sumo in ids_edge_cong_sumo:
            #        ind = self.edges_congest[id_edge_sumo]['ids_veh'].index(id_veh_sumo)
            #        if self.edges_congest[id_edge_sumo]['are_rerouted'][ind]:
            #            is_rerouted = True
            #            break
            #        
            #    if not is_rerouted:
            #        vehicles_routes_unchanged.append(id_veh_sumo)
            
            #for id_veh_sumo in vehicles_routes_unchanged:
            #    self._del_veh_data(id_veh_sumo)
            
            if self.is_update_routes_antijam:
                #for id_veh_sumo , route in     self.vehicles_routes_congest.items():
                #    print '  check route of id_veh_sumo',id_veh_sumo,'route',route
                self.get_scenario().demand.set_routes_sumo(list(self.vehicles_routes_congest.keys()), list(self.vehicles_routes_congest.values()))     
                
            SumoTraci.finish_sim(self)
           

            
        
   
                
        
        
     
            
        
