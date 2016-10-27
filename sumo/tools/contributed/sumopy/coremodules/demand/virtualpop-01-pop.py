import numpy as np  
from numpy import random          
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlmanager as xm
#from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from agilepy.lib_base.misc import random_choice


class Plans(am.ArrayObjman):
    def __init__(self,polulation,  **kwargs):
        """Plans database."""
        self._init_objman(  ident='plans', 
                            parent = polulation, 
                            name = 'Plans', 
                            info = 'Mobility plan for virtual polulation.', 
                            #xmltag = ('plans','plan',None),
                            **kwargs)

                                                    
        self.add_col(am.IdsArrayConf( 'ids_person', polulation, 
                                        groupnames = ['state'], 
                                        name = 'Person ID', 
                                        info = 'Person ID to who this plan belongs to.',
                                        ))
                                        
        
                        
        self.add_col(am.ArrayConf( 'probabilities', 1.0,
                        name = 'Probability', 
                        info = 'Plan execution probability.',
                        ))
                        
        
        self.add_col(am.ArrayConf( 'times_est', 0.0,
                        name = 'Estim. time', 
                        info = 'Estimated time to execute plan.',
                        unit = 's',
                        ))
                                        
        self.add_col(am.ArrayConf( 'times_exec', 0.0,
                        name = 'Exec time', 
                        info = 'Last plan execution time from simulation run.',
                        unit = 's',
                        ))
        
          
                                            
        self.add_col(am.TabIdListArrayConf( 'stagelists',
                                    name = 'Stages',
                                    info = 'Sequence of stages of this plan.',
                                    ))
                                                  
        
                        
    def append_stage(self, id_plan, stage, id_stage):
        # test: stage = cm.TableEntry(stagetype, id_plan)
        #print '  stages[id_plan]',type(self.stages[id_plan]),self.stages[id_plan]
        #print '  self.cols.stages',self.parent.plans.cols.stages
        
        if self.stagelists[id_plan]==None:
            self.stagelists[id_plan]=[(stage, id_stage)]
        else:
            self.stagelists[id_plan].append((stage, id_stage)) 
    
    def get_timing_laststage(self,id_plan):
        """
        Return time_start and duration of last stage of plan id_plan
        """
        stages_current =  self.stagelists[id_plan]
        
        if stages_current!=None:
            stage_last, id_stage_last = stages_current[-1]
            return  stage_last.get_timing(id_stage_last)
        else:
            return -1, -1
        
     
                    
    def get_stages(self, id_plan):
        stages = self.stagelists[id_plan]
        if stages==None:
            return []
        else:
            return stages

class IndividualVehicles(am.ArrayObjman):
    
    def __init__(self, polulation, vtypes, edges, parking, **kwargs):
        
        self._init_objman(  ident='individualvehicles', 
                            parent = polulation, 
                            name = 'Indiv. Veh.', 
                            info = 'Individual vehicle database. These are privately owned vehicles.',
                            **kwargs)
        
        
                        
        self.add_col(am.IdsArrayConf( 'ids_vtype', vtypes, 
                                        groupnames = ['state'], 
                                        name = 'Veh. type', 
                                        info = 'Vehicle type.',
                                        xmltag = 'type',
                                        ))
                        
        self.add_col(am.IdlistsArrayConf( 'routes', edges,
                                            groupnames = ['_private'], 
                                            name = 'Route', 
                                            info = "The vehicle's route as a sequence of edge ids.",   
                                            ))
        
        self.add_col(am.IdlistsArrayConf( 'ids_parkings', parking,
                                            groupnames = ['state'], 
                                            name = 'Parking', 
                                            info = 'Sequence of parking IDs, this will cause the vehicle to stop successively at these parkings.',
                                            ))
                                            
        self.add_col(am.ArrayConf( 'times_exec', 0.0,
                        name = 'Exec time', 
                        info = 'Last plan execution time from simulation run.',
                        unit = 's',
                        ))
                                            
        parking.link_vehiclefleet(self)
    
    def get_vtypes(self):
        """
        Returns a set with all used vehicle types.
        """
        #print 'Vehicles_individual.get_vtypes',self.cols.vtype
        return set(self.ids_vtype.get_value())
        
        
    
    
    def route_between_parking(self, id_veh, id_parking_from, id_parking_to):
        """
        Sets/adds route and distance of ride with id_veh  
        between id_parking_from and id_parking_to
        Returns approximate duration.
        """
        print 'route_between_parking', id_veh, id_parking_from, id_parking_to
        ind = self.get_ind(id_veh)
        
        vtype = ids_vtype[id_veh]#self.cols.vtype[ind]
        #v_max = self.parent.get_demand().getVehicles().maxSpeed.get(vtype)
        #edges, dist = self.get_route_bet ween_parking(vtype, id_parking_from, id_parking_to)
        #ids_edge, duration_approx = minimum_costtree.get_ids_edge_duration_from_route(vtype, edges, v_max)

        ids_edge, dist, duration_approx = self.ids_parkings.get_linktab().get_route_between_parking(vtype, id_parking_from, id_parking_to)
        #print '  ids_edge',ids_edge 
        
        if not self._update_parking(id_veh, id_parking_from, id_parking_to):
            return -1
        
        self._update_route(ids_edge)
        
        

        print '  routes',self.routes[id_veh]
        return duration_approx
        
        
        
    def set_route(self, id_veh, route, id_parking_from, id_parking_to):  
        """
        This sets directly a route.
        The first and last edge of the route are assumed to be parking spaces.
        """
        print 'set_route id_veh',id_veh,route
        #ind_veh = self.get_ind(id_veh)
        if not self._update_parking(id_veh, id_parking_from, id_parking_to):
            return -1
        self._update_route(id_veh, ids_edge)
        

        
    def _update_parking(self, id_veh, id_parking_from, id_parking_to):
        ids_parking = self.ids_parking[id_veh]
        if ids_parking==None:
            print '  new parking'
            self.ids_parking[id_veh]=[id_parking_from, id_parking_to]
            
        
        elif ids_parking[-1] == id_parking_from:
            print '  append parking'
            ids_parking.append(id_parking_to)
            ids_edge.pop(0)
            
        
        else:
            print 'WARNING in route_between_parking veh %d not on parking %d but on parking %d'%(id_veh,id_parking_from,ids_parking[-1])
            #route_edges_bridge = self.get_route(vtype, parking[-1], id_parking_from)
            # this should generate an untriggered parking at parking[-1] 
            # and link parkings with route_edges_bridge
            return False
        
        return True
        
    def _update_route(self,id_veh, route):
        route_current = self.routes[id_veh]
        
        if ids_edge_current==None:
            self.routes[id_veh] = route
        else:
            route_current += route
    
class StageTypeMixin(am.ArrayObjman):
    def init_stagetype(self,ident, polulation,  name = '', info = "Stage of Plan"):
        
        self._init_objman(  ident=ident, parent = polulation, name = name,
                            info = info,
                            )
        
        
        self.add_col(am.IdsArrayConf( 'ids_plan', polulation.get_plans(), 
                                        groupnames = ['state'], 
                                        name = 'ID plan', 
                                        info = 'ID of plan.',
                                        xmltag = 'type',
                                        ))
                                        
        
                            
        self.add_col(am.ArrayConf( 'times_start', -1.0,
                            name = 'Start time', 
                            unit = 's',
                            info = 'Planned or estimated time when this stage starts. Value -1 means unknown.',
                            ))
                            
        self.add_col(am.ArrayConf( 'durations', -1.0,
                            name = 'Duration',
                            unit = 's', 
                            info = 'Planned or estimated Duration for this stage starts. Value -1 means unknown.',
                            ))
                            
    def append_stage( self, id_plan, time_start,  **kwargs):
        
        # try to fix timing
        #if time_start<0:
        #    time_start_prev, duration_prev = self.parent.plans.get_timing_laststage(id_plan)
        #    if (duration_prev>=0)&(time_start_prev>=0):
        #        time_start = time_start_prev+duration_prev
             
                    
        
        id_stage = self.suggest_id()
        print 'append_stage %s id_plan=%d, id_stage=%d, t=%d'%(self.name,id_plan,id_stage,time_start)
        #print '   kwargs=',kwargs
        for key in kwargs.keys():
            print '    %s=%s'%(key,kwargs[key])
        print '    --'
        self.set_row(id_stage,ids_plan = id_plan,  times_start = time_start, **kwargs)
        ids_plan.get_linktab().append_stage(id_plan, self, id_stage)
        

        return id_stage, time_start+self.duration[id_stage]
    
    def get_timing(self, id_stage):
        #ind = self.get_ind(id_stage)
        return self.time_start[id_stage], self.duration[id_stage]  
    
    def to_xml(self, id_stage, fd, indent = 0):
        """
        To be overridden by specific stage class.
        """
        pass
    
    
class Transits(StageTypeMixin):
    def __init__(self, population, publictransport):
        self.init_stagetype('transits', population, name='Ride on PT',
                            info = 'Ride on a single public transport line (no transfers).')
                            
        self.add_col(am.IdsArrayConf( 'ids_line', publictransport.ptlines, 
                                        groupnames = ['state'], 
                                        name = 'ID line', 
                                        info = 'ID of public transport line.',
                                        ))
        
        self.add_col(am.IdsArrayConf( 'ids_line', publictransport.stops, 
                                        groupnames = ['state'], 
                                        name = 'ID line', 
                                        info = 'ID of public transport line.',
                                        ))
                                        
        self.add_col(am.IdsArrayConf( 'ids_stop_from', publictransport.stops, 
                                        groupnames = ['state'], 
                                        name = 'Dep. station', 
                                        info = 'ID of departure station or bus stop.',
                                        ))
        
        self.add_col(am.IdsArrayConf( 'ids_stop_to', stops, 
                                        groupnames = ['state'], 
                                        name = 'Arr. station', 
                                        info = 'ID of arrival station or bus stop.',
                                        ))
                                                                                                                           
        
        
                            
        
                            
    
    
    

    
    def to_xml(self, id_stage, fd, indent = 0):
        # <ride from="1/3to0/3" to="0/4to1/4" lines="train0"/>
        ind = self.get_ind(id_stage)    
        fd.write(xm.start('ride',indent=indent))
        
        # generate string with all vehicles of this line during entire simulation
        ptlines = self.parent.get_ptlines()
        
        id_line = self.cols.id_line[ind]
        ids_veh = ptlines.get_ids_veh(id_line)
        
        #landuse = self.parent.get_landuse()
        #
        #edge_from, pos_from = landuse.get_edge_pos_parking(self.cols.id_parking_from[ind])
        #edge_to, pos_to = landuse.get_edge_pos_parking(self.cols.id_parking_to[ind])
        ptstops = self.parent.get_ptstops()
        
        
        fd.write(xm.num('from', ptstops.get_id_edge(self.cols.id_stop_from[ind]) ))
        fd.write(xm.num('to', ptstops.get_id_edge(self.cols.id_stop_to[ind]) ))
        fd.write(xm.arr('lines', ids_veh))
        #if self.cols.pos_edge_from[ind]>0:
        #    fd.write(xm.num('departPos', self.cols.pos_edge_from[ind]))
        #if self.cols.pos_edge_to[ind]>0:
        #    fd.write(xm.num('arrivalPos', self.cols.pos_edge_to[ind]))
        
        fd.write(xm.stopit())# ends stage   

class Rides(StageTypeMixin):
    def __init__(self,population, parking):
        self.init_stagetype('rides', population, name='Auto rides', 
                                info = 'Rides with privately owned vehicle.')
        
        # TODO: this structure needs review: the private vehicle is part of a person, not a stage
        # street parking at home and work could be in stage. Private garage is part of person...  
        self.add_col(am.IdsArrayConf( 'ids_veh', population.get_individualvehicles(), 
                                        groupnames = ['state'], 
                                        name = 'ID vehicle', 
                                        info = 'ID of private vehicle.',
                                        ))
                                        
          
                            
        #self._vehicles_individual = vehicles_individual
        self.add_col(am.IdsArrayConf( 'ids_parking_from', parking, 
                                        groupnames = ['state'], 
                                        name = 'ID dep. parking', 
                                        info = 'Parking ID at the departure of the ride starts.',
                                        ))
        self.add_col(am.IdsArrayConf( 'ids_parking_to', parking, 
                                        groupnames = ['state'], 
                                        name = 'ID arr. parking', 
                                        info = 'Parking ID  at the arrival of the ride.',
                                        ))   
        #self._init_constants()
                                     
    #def _init_constants(self):
    #    self.vehicles = self.ids_veh.get_linktab()                     
    
    def get_individualvehicles(self):
        return self.parent.get_individualvehicles()()
                            
    def append_stage( self, id_plan, time_start=-1.0,
                      ids_edge = [], duration = None,
                      **kwargs):
        
        id_stage, time_end = StageTypeMixin.append_stage(self, id_plan,time_start, **kwargs)
        # attention: here time_end is not valid because duration is not yet set
        ind = self.get_ind(id_stage)
        if duration==None:
            # this will route and compute duration
            duration = self.vehicles.route_between_parking(self.ids_veh.value[ind],self.ids_parking_from.value[ind], self.ids_parking_to.value[ind])
        else:
            # case wehn route has been already determined
            self._vehicles_individual.set_route(self.ids_veh.value[ind],self.ids_parking_from.value[ind], self.ids_parking_to.value[ind])

        self.duration[id_stage] = duration
        return id_stage, time_start+duration
    
    
    
    def to_xml(self, id_stage, fd, indent = 0):
        ind = self.get_ind(id_stage)    
        fd.write(xm.start('ride',indent=indent))
        landuse = self.parent.get_landuse()
        
        edge_from, pos_from = landuse.get_edge_pos_parking(self.cols.id_parking_from[ind])
        edge_to, pos_to = landuse.get_edge_pos_parking(self.cols.id_parking_to[ind])
        
        fd.write(xm.num('from', edge_from.getID()))
        fd.write(xm.num('to', edge_to.getID()))
        fd.write(xm.num('lines', self.cols.id_veh[ind]))
        #if self.cols.pos_edge_from[ind]>0:
        #    fd.write(xm.num('departPos', self.cols.pos_edge_from[ind]))
        #if self.cols.pos_edge_to[ind]>0:
        #    fd.write(xm.num('arrivalPos', self.cols.pos_edge_to[ind]))
        
        fd.write(xm.stopit())# ends stage   
       
class Walks(StageTypeMixin):
    def __init__(self,population, edges):
        self.init_stagetype('walks', population, name='Walks')
        
        # TODO: this structure needs review: the private vehicle is part of a person, not a stage
        # street parking at home and work could be in stage. Private garage is part of person...  
        self.add_col(am.IdsArrayConf( 'ids_edge_from', edges, 
                                        groupnames = ['state'], 
                                        name = 'ID Dep. edge', 
                                        info = 'Edge ID at departure of walk.',
                                        ))
        
        self.add_col(am.IdsArrayConf( 'ids_edge_to', edges, 
                                        groupnames = ['state'], 
                                        name = 'ID Arr. edge', 
                                        info = 'Edge ID where walk finishes.',
                                        ))
                            
        self.add_col(am.ArrayConf(  'positions_from', 0.0,
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_POS_DEPARTURE,
                                        perm='r', 
                                        name = 'Depart pos',
                                        unit = 'm',
                                        info = "Position on edge at the moment of departure.",
                                        xmltag = 'departPos',
                                        #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                        ))
                                        
        self.add_col(am.ArrayConf(  'positions_to', 0.0,
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_POS_ARRIVAL,
                                        perm='r', 
                                        name = 'Arrival pos',
                                        unit = 'm',
                                        info = "Position on edge at the moment of arrival.",
                                        xmltag = 'arrivalPos',
                                        #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                        ))
                                        
        
        
 
        
        self.add_col(am.ArrayConf(  'speeds', 0.0,
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_SPEED_ARRIVAL,
                                        perm='r', 
                                        name = 'Walk speed',
                                        unit = 'm/s',
                                        info = 'Average walking speed.',
                                        xmltag = 'speed',
                                        #xmlmap = get_inversemap(OPTIONMAP_SPEED_ARRIVAL),
                                        ))
                                                                                
        
    def to_xml(self, id_stage, fd, indent = 0):
        ind = self.get_ind(id_stage)    
        fd.write(xm.start('walk',indent=indent))
        fd.write(xm.num('from', self.cols.id_edge_from[ind]))
        fd.write(xm.num('to', self.cols.id_edge_to[ind]))
        if self.cols.pos_edge_from[ind]>0:
            fd.write(xm.num('departPos', self.cols.pos_edge_from[ind]))
        if self.cols.pos_edge_to[ind]>0:
            fd.write(xm.num('arrivalPos', self.cols.pos_edge_to[ind]))
        
        fd.write(xm.stopit())# ends walk

class Activities(StageTypeMixin):
    def __init__(self,population, lanes):
        self.init_stagetype('activities', population, name='Activities')
        
        # TODO: this structure needs review: the private vehicle is part of a person, not a stage
        # street parking at home and work could be in stage. Private garage is part of person...  
        self.add_col(am.IdsArrayConf( 'id_lane', lanes, 
                                        groupnames = ['state'], 
                                        name = 'ID lane', 
                                        info = 'Lane ID at which activity takes place.',
                                        ))
        self.add_col(am.ArrayConf(  'pos_lane', 0.0,
                            name = 'Lane pos.', 
                            unit = 'm',
                            info = 'Position on Lane on where activity takes place.',
                            xml = 'pos_start',
                            ))
                            
        
                            
        self.add_col(am.ArrayConf(  'activity', '',
                            dtype = np.object,
                            name = 'Activity', 
                            info = 'Type of activity performed during the stop.',
                            ))                         
                         
    def to_xml(self, id_stage, fd, indent = 0):
        
        # <stop lane="1/4to2/4_0" duration="20" startPos="40" actType="singing"/>
        
        ind = self.get_ind(id_stage)    
        fd.write(xm.start('stop',indent=indent))
        fd.write(xm.num('lane', self.cols.id_lane[ind]))
        fd.write(xm.num('startPos', self.cols.pos_lane[ind]))
        fd.write(xm.num('duration', self.cols.duration[ind]))
        fd.write(xm.num('actType', self.cols.activity[ind]))
        
        fd.write(xm.stopit())# ends activity   


class Virtualpolulation(am.ArrayObjman):
    def __init__(self, demand, **kwargs):
        self._init_objman(  ident='virtualpop', 
                            parent = demand, 
                            name = 'Virtual polulation', 
                            info = 'Contains information of each individual of the virtual population.', 
                            #xmltag = ('plans','plan',None),
                            **kwargs)
        
        scenario = demand.get_scenario()
        #--------------------------------------------------------------------                
        # plans table                
        self.add(cm.ObjConf(Plans(self))   )            
        #--------------------------------------------------------------------                
        # individual vehicles table                                     
        self.add(cm.ObjConf(IndividualVehicles(self, demand.vtypes, scenario.net.edges, scenario.landuse.parking))   ) 
        
        
        #--------------------------------------------------------------------                
        # walks table                
        #self.walks = self.add_tableman(Walks(self))
        self.add(cm.ObjConf(Walks(self, scenario.net.edges))   ) 
        
        
        
        #--------------------------------------------------------------------                
        # rides table   
        self.add(cm.ObjConf(Rides(self, scenario.landuse.parking))   ) 
             
        
        #--------------------------------------------------------------------                
        # pt rides table    
        # self.add(cm.ObjConf(Transits(self, publictransport))   ) 
                    
        #--------------------------------------------------------------------                
        # Activities table           
        self.add(cm.ObjConf(Activities(self, scenario.net.lanes))   ) 
            
        #-------------------------------------------------------------------- 
        # misc params
        # this is option for intermodal routing process 
        #self.add(AttrConf(  'dist_max_wait', kwargs.get('dist_max_wait',20.0),
        #                                groupnames = ['options'], 
        #                                perm='wr', 
        #                                unit = 'm',
        #                                name = 'Max dist. Wait', 
        #                                info = 'The maximum distance between two stops or platforms, when a person during transfer is not walking, but just waiting for the next bus/train to come.' ,
        #                                #xmltag = 'pos',
        #                                ))
        #-------------------------------------------------------------------- 
        # columns
        
        self.add_col(am.ArrayConf(  'identification ', '',
                                        dtype = np.object,
                                        name = 'Name', 
                                        info = 'Identification or name of person.',
                                        ))
                                        
        self.add_col(am.IdsArrayConf( 'ids_fac_home', scenario.landuse.facilities, 
                                        groupnames = ['state'], 
                                        name = 'ID home fac.', 
                                        info = 'Facility ID of home.',
                                        ))
                                        
        
        self.add_col(am.IdsArrayConf( 'ids_fac_activity', scenario.landuse.facilities, 
                                        groupnames = ['state'], 
                                        name = 'ID activity fac.', 
                                        info = 'Facility ID of location of main activity (work, study).',
                                        ))
                                                                        
        self.add_col(am.IdsArrayConf( 'ids_mode_preferred', scenario.net.modes, 
                                        name = 'ID preferred mode', 
                                        info = 'ID of preferred transport mode of person.',
                                        ))
                                        
                                        

                                                    
        self.add_col(am.ArrayConf(  'times_start', 0,
                                        name = 'Start time',
                                        unit = 's', 
                                        info = 'Time when person leaves home facility for doing some activity.',
                                        ))
                                        
        # this could be extended to a list with more plans
        self.add_col(am.IdsArrayConf( 'ids_plan', self.get_plans(), 
                                        name = 'ID Plan', 
                                        info = 'Currently best mobility plan ID of person.',
                                        ))
                                        
        self.add_col(am.IdlistsArrayConf( 'lists_ids_plan', self.get_plans(),
                                        name = 'IDs Plans', 
                                        info = 'List with the IDs of feasible mobility plans.', 
                                        ))
                                        
    def get_plans(self):
        return self.plans.get_value()
    
    def get_individualvehicles(self):
        return self.individualvehicles.get_value()
    
    def get_landuse(self):
        return self.parent.get_scenario().landuse
    
    def disaggregate_odflow(self,   time_start, time_end, id_mode, 
                                    id_zone_orig, id_zone_dest,tripnumber,
                                    probs_fac, ids_fac, 
                                    id_landusetype_orig,
                                    id_landusetype_dest,):
        print 'disaggregate_odflow',time_start, time_end, id_mode, id_zone_orig, id_zone_dest,tripnumber
        """
        Disaggregation of demand dem  from taz id_zone_orig to id_zone_dest with id_mode
        during time interval  time_start,time_end, and creation of persons
        which are parameterized accordingly.
        The facility type at origin will be  landusetype_orig
        and at destination landusetype_dest.
        
        
        The probability vector to start/end the trip in the different facilities
        of facility type id_landusetype_orig/id_landusetype_dest is given by the 
        dictionary probs_fac[id_zone][id_landusetype].
        The ids_fac is an array that contains the facility ids in correspondence
        to the probability vector. 
        These information are obtained from 
        
        probs_fac, ids_fac =  landuse.get_departure_probabilities()
        """
        print 'disaggregate_odm',tripnumber, id_mode,id_zone_orig,id_zone_dest
        probs_orig = probs_fac[id_zone_orig][id_landusetype_orig]
        probs_dest = probs_fac[id_zone_dest][id_landusetype_dest]
        
        print '  probs_orig',sum(probs_orig)#,'\n',probs_orig
        print '  probs_dest',sum(probs_dest)#,'\n',probs_dest
        
        # is there a chance to find facilities to locate persons in
        # origin and destination zone
        #print '  ',self.times_start.value.dtype,np.random.randint(time_start,time_end,tripnumber)
        #print '  ',self.ids_mode_preferred.value.dtype,id_mode * np.ones(tripnumber, np.int32),
        if (np.sum(probs_orig) > 0)&(np.sum(probs_dest) > 0): 
            return self.add_rows( n=tripnumber,
                                ids_fac_home = ids_fac[random_choice(tripnumber, probs_orig)],
                                ids_fac_activity = ids_fac[random_choice(tripnumber, probs_dest)],
                                ids_mode_preferred = id_mode * np.ones(tripnumber, dtype=np.int32),
                                times_start = np.random.randint(time_start,time_end,tripnumber)
                                )
        else:
            return []
                                
        
    def create_pop_from_ods(self, landusetype_orig = 'residential',landusetype_dest='industrial'):
        print 'create_pop_from_ods'
        demand = self.parent
        odflowtab = demand.odintervals.generate_odflows()
        
        probs_fac, ids_fac = self.get_landuse().facilities.get_departure_probabilities()
        landusetypes = self.get_landuse().landusetypes
        
        for id_flow in odflowtab.get_ids():
                self.disaggregate_odflow(   odflowtab.times_start[id_flow],
                                            odflowtab.times_end[id_flow],
                                            odflowtab.ids_mode[id_flow],
                                            odflowtab.ids_orig[id_flow],
                                            odflowtab.ids_dest[id_flow],
                                            odflowtab.tripnumbers[id_flow],
                                            probs_fac, ids_fac, 
                                            landusetypes.typekeys.get_id_from_index(landusetype_orig),
                                            landusetypes.typekeys.get_id_from_index(landusetype_dest),
                                            #landusetype_orig = 'residential',
                                            #landusetype_dest = 'industrial',
                                            #od_factypes=('residential','industrial','commercial','leisure')# no!!
                                            #od_factypes=('residential','industrial')
                                        )
        #return odflowtab
        
    def create_pop_from_ods_old(self):
        print 'create_pop_from_ods'
        
        _count_odm = 0
        
        demand = self.parent
        #modes = demand.get_net().modes
        landuse = self.get_landuse()
        zones = landuse.zones
        
        # probabilities[id_zone][ftype]
        probs_fac, ids_fac = landuse.get_departure_probabilities()
        print '  probs_fac\n',probs_fac
        print '  ids_fac\n',ids_fac
        
        
        for odm in demand.getOdms():
            t_interval = odm.getInterval()
            # go through all availlable modes
            print '  odm.getModes()',odm.getModes()
            for mode in odm.getModes():
                ods = odm.getOdsMode(mode)
                print '  mode',mode#,modes.keys()
                #print '  ods',ods
                if mode in modes.keys(): # valid mode?
                    
                    ods = odm.getOdsMode(mode)
                    #print '    ods.getIds',ods.getIds()
                    # go through all demands from district-to-district 
                    for id in ods.getIds():
                        # TODO: this is all very inefficient converting between ID, od and o,d
                        od = ods.getOdFromId(id)
                        dem = ods.getOdtrips(od)
                        _count_odm += dem # to check created demand
                        
                        # disaggregation of demand dem 
                        # from id_taz_orig to id_taz_dest with mode
                        # during time interval  (starttime, endtime)
                        self.disaggregate_odm(  dem, od, mode,t_interval, 
                                                probs_fac, ids_fac, 
                                                #od_factypes=('residential','industrial','commercial','leisure')# no!!
                                                od_factypes=('residential','industrial')
                                                )
                        
        
        print '  check total',_count_odm,len(self.persons)
        return
               
    