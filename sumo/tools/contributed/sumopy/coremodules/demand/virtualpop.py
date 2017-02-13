import numpy as np
from numpy import random
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
#from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from coremodules.network import routing
from agilepy.lib_base.misc import random_choice


class Plans(am.ArrayObjman):

    def __init__(self, polulation,  **kwargs):
        """Plans database."""
        self._init_objman(ident='plans',
                          parent=polulation,
                          name='Plans',
                          info='Mobility plan for virtual polulation.',
                          #xmltag = ('plans','plan',None),
                          **kwargs)

        self.add_col(am.IdsArrayConf('ids_person', polulation,
                                     groupnames=['state'],
                                     name='Person ID',
                                     info='Person ID to who this plan belongs to.',
                                     ))

        self.add_col(am.ArrayConf('probabilities', 1.0,
                                  name='Probability',
                                  info='Plan execution probability.',
                                  ))

        self.add_col(am.ArrayConf('times_est', 0.0,
                                  name='Estim. time',
                                  info='Estimated time to execute plan.',
                                  unit='s',
                                  ))

        self.add_col(am.ArrayConf('times_exec', 0.0,
                                  name='Exec time',
                                  info='Last plan execution time from simulation run.',
                                  unit='s',
                                  ))

        self.add_col(am.TabIdListArrayConf('stagelists',
                                           name='Stages',
                                           info='Sequence of stages of this plan.',
                                           ))

    def append_stage(self, id_plan, stage, id_stage):
        # test: stage = cm.TableEntry(stagetype, id_plan)
        # print 'Plans.append_stage',id_plan, stage, id_stage

        if self.stagelists[id_plan] == None:
            self.stagelists[id_plan] = [(stage, id_stage)]
        else:
            self.stagelists[id_plan].append((stage, id_stage))
        # print '  after append
        # stagelists[id_plan]',type(self.stagelists[id_plan]),self.stagelists[id_plan]

    def get_timing_laststage(self, id_plan):
        """
        Return time_start and duration of last stage of plan id_plan
        """
        stages_current = self.stagelists[id_plan]

        if stages_current != None:
            stage_last, id_stage_last = stages_current[-1]
            return stage_last.get_timing(id_stage_last)
        else:
            return -1, -1

    def get_stages(self, id_plan):
        stages = self.stagelists[id_plan]
        if stages == None:
            return []
        else:
            return stages


class IndividualVehicles(am.ArrayObjman):

    def __init__(self, ident, polulation, vtypes, edges, parking, **kwargs):
        print 'IndividualVehicles vtype id_default', vtypes.ids_sumo.get_id_from_index('passenger1')
        self._init_objman(ident=ident,
                          parent=polulation,
                          name='Indiv. Veh.',
                          info='Individual vehicle database. These are privately owned vehicles.',
                          **kwargs)

        self.add_col(am.IdsArrayConf('ids_vtype', vtypes,
                                     id_default=vtypes.ids_sumo.get_id_from_index(
                                         'passenger1'),
                                     groupnames=['state'],
                                     name='Veh. type',
                                     info='Vehicle type.',
                                     #xmltag = 'type',
                                     ))

        self.add_col(am.IdsArrayConf('ids_person', polulation,
                                     groupnames=['state'],
                                     name='ID person',
                                     info='ID of person who ownes the vehicle.',
                                     ))

        self.add_col(am.IdlistsArrayConf('routes', edges,
                                         groupnames=['_private'],
                                         name='Route',
                                         info="The vehicle's route as a sequence of edge ids.",
                                         ))

        self.add_col(am.IdlistsArrayConf('ids_parkings', parking,
                                         groupnames=['state'],
                                         name='Parking',
                                         info='Sequence of parking IDs, this will cause the vehicle to stop successively at these parkings.',
                                         ))

        self.add_col(am.ArrayConf('times_exec', 0.0,
                                  name='Exec time',
                                  info='Total route execution time from simulation run of last plan.',
                                  unit='s',
                                  ))

        parking.link_vehiclefleet(self)

    def get_vtypes(self):
        """
        Returns a set with all used vehicle types.
        """
        # print 'Vehicles_individual.get_vtypes',self.cols.vtype
        return set(self.ids_vtype.get_value())

    def route_between_parking(self, id_veh, id_parking_from, id_parking_to):
        """
        Sets/adds route and distance of ride with id_veh  
        between id_parking_from and id_parking_to
        Returns approximate duration.
        """
        # print 'route_between_parking', id_veh, id_parking_from, id_parking_to
        ind = self.get_ind(id_veh)

        vtype = ids_vtype[id_veh]  # self.cols.vtype[ind]
        #v_max = self.parent.get_demand().getVehicles().maxSpeed.get(vtype)
        # edges, dist = self.get_route_bet ween_parking(vtype, id_parking_from, id_parking_to)
        #ids_edge, duration_approx = minimum_costtree.get_ids_edge_duration_from_route(vtype, edges, v_max)

        ids_edge, dist, duration_approx = self.parent.get_route_between_parking(
            id_parking_from, id_parking_to, vtype)
        # print '  ids_edge',ids_edge

        if not self._update_parking(id_veh, id_parking_from, id_parking_to):
            return -1

        self._update_route(ids_edge)

        # print '  routes',self.routes[id_veh]
        return duration_approx

    def set_route(self, id_veh, route, id_parking_from, id_parking_to):
        """
        This sets directly a route.
        The first and last edge of the route are assumed to be parking spaces.
        """
        # print 'set_route id_veh',id_veh,route
        #ind_veh = self.get_ind(id_veh)
        if not self._update_parking(id_veh, id_parking_from, id_parking_to):
            return -1
        self._update_route(id_veh, route)

    def _update_parking(self, id_veh, id_parking_from, id_parking_to):
        # print '_update_route',id_veh,id_parking_from, id_parking_to
        ids_parking = self.ids_parkings[id_veh]
        if ids_parking == None:
            # print '  new parking'
            self.ids_parkings[id_veh] = [id_parking_from, id_parking_to]

        elif ids_parking[-1] == id_parking_from:
            # print '  append parking'
            ids_parking.append(id_parking_to)
            # ids_edge.pop(0)????

        else:
            print 'WARNING in route_between_parking veh %d not on parking %d but on parking %d' % (id_veh, id_parking_from, ids_parking[-1])
            #route_edges_bridge = self.get_route(vtype, parking[-1], id_parking_from)
            # this should generate an untriggered parking at parking[-1]
            # and link parkings with route_edges_bridge
            return False

        return True

    def _update_route(self, id_veh, route):
        # print '_update_route',id_veh,route
        route_current = self.routes[id_veh]

        if route_current == None:
            self.routes[id_veh] = route
        else:
            route_current += route


class StageTypeMixin(am.ArrayObjman):

    def init_stagetype(self, ident, polulation,  name='', info="Stage of Plan"):

        self._init_objman(ident=ident, parent=polulation, name=name,
                          info=info,
                          )

        self.add_col(am.IdsArrayConf('ids_plan', polulation.get_plans(),
                                     groupnames=['state'],
                                     name='ID plan',
                                     info='ID of plan.',
                                     xmltag='type',
                                     ))

        self.add_col(am.ArrayConf('times_start', -1.0,
                                  name='Start time',
                                  unit='s',
                                  info='Planned or estimated time when this stage starts. Value -1 means unknown.',
                                  ))

        self.add_col(am.ArrayConf('durations', -1.0,
                                  name='Duration',
                                  unit='s',
                                  info='Planned or estimated Duration for this stage starts. Value -1 means unknown.',
                                  ))

    def append_stage(self, id_plan, time_start,  **kwargs):

        # try to fix timing
        # if time_start<0:
        #    time_start_prev, duration_prev = self.parent.plans.get_timing_laststage(id_plan)
        #    if (duration_prev>=0)&(time_start_prev>=0):
        #        time_start = time_start_prev+duration_prev

        id_stage = self.add_row(
            ids_plan=id_plan,  times_start=time_start, **kwargs)
        # print 'STAGE.appended stage %s id_plan=%d, id_stage=%d, t=%d'%(self.get_name(),id_plan,id_stage,time_start)
        # for key in kwargs.keys():
        #    print '    %s=%s'%(key,kwargs[key])
        # print '    --id_plan, self, id_stage',id_plan, self,
        # id_stage#,self.ids_plan.get_linktab()
        self.ids_plan.get_linktab().append_stage(id_plan, self, id_stage)
        # print '  plan appended',id_plan, self, id_stage

        return id_stage, time_start + self.durations[id_stage]

    def get_timing(self, id_stage):
        #ind = self.get_ind(id_stage)
        return self.times_start[id_stage], self.durations[id_stage]

    def to_xml(self, id_stage, fd, indent=0):
        """
        To be overridden by specific stage class.
        """
        pass


class Transits(StageTypeMixin):

    def __init__(self, ident, population, publictransport, name='Ride on PT', info='Ride on a single public transport line (no transfers).'):
        self.init_stagetype(ident, population, name=name, info=info)

        self.add_col(am.IdsArrayConf('ids_line', publictransport.ptlines,
                                     groupnames=['state'],
                                     name='ID line',
                                     info='ID of public transport line.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_line', publictransport.stops,
                                     groupnames=['state'],
                                     name='ID line',
                                     info='ID of public transport line.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_stop_from', publictransport.stops,
                                     groupnames=['state'],
                                     name='Dep. station',
                                     info='ID of departure station or bus stop.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_stop_to', stops,
                                     groupnames=['state'],
                                     name='Arr. station',
                                     info='ID of arrival station or bus stop.',
                                     ))

    def to_xml(self, id_stage, fd, indent=0):
        # <ride from="1/3to0/3" to="0/4to1/4" lines="train0"/>
        ind = self.get_ind(id_stage)
        fd.write(xm.start('ride', indent=indent))

        # generate string with all vehicles of this line during entire
        # simulation
        ptlines = self.parent.get_ptlines()

        id_line = self.cols.id_line[ind]
        ids_veh = ptlines.get_ids_veh(id_line)

        #landuse = self.parent.get_landuse()
        #
        #edge_from, pos_from = landuse.get_edge_pos_parking(self.cols.id_parking_from[ind])
        #edge_to, pos_to = landuse.get_edge_pos_parking(self.cols.id_parking_to[ind])
        ptstops = self.parent.get_ptstops()

        fd.write(xm.num('from', ptstops.get_id_edge(
            self.cols.id_stop_from[ind])))
        fd.write(xm.num('to', ptstops.get_id_edge(self.cols.id_stop_to[ind])))
        fd.write(xm.arr('lines', ids_veh))
        # if self.cols.pos_edge_from[ind]>0:
        #    fd.write(xm.num('departPos', self.cols.pos_edge_from[ind]))
        # if self.cols.pos_edge_to[ind]>0:
        #    fd.write(xm.num('arrivalPos', self.cols.pos_edge_to[ind]))

        fd.write(xm.stopit())  # ends stage


class Rides(StageTypeMixin):

    def __init__(self, ident, population, parking, name='Auto rides', info='Rides with privately owned vehicle.'):
        self.init_stagetype(ident, population, name=name, info=info)

        # TODO: this structure needs review: the private vehicle is part of a person, not a stage
        # street parking at home and work could be in stage. Private garage is
        # part of person...
        self.add_col(am.IdsArrayConf('ids_veh', population.get_individualvehicles(),
                                     groupnames=['state'],
                                     name='ID vehicle',
                                     info='ID of private vehicle.',
                                     ))

        #self._vehicles_individual = vehicles_individual
        self.add_col(am.IdsArrayConf('ids_parking_from', parking,
                                     groupnames=['state'],
                                     name='ID dep. parking',
                                     info='Parking ID at the departure of the ride starts.',
                                     ))
        self.add_col(am.IdsArrayConf('ids_parking_to', parking,
                                     groupnames=['state'],
                                     name='ID arr. parking',
                                     info='Parking ID  at the arrival of the ride.',
                                     ))
        # self._init_constants()

    # def _init_constants(self):
    #    self.vehicles = self.ids_veh.get_linktab()

    def get_individualvehicles(self):
        return self.parent.get_individualvehicles()()

    def append_stage(self, id_plan, time_start=-1.0,
                     ids_edge=[], duration=None,
                     **kwargs):

        id_stage, time_end = StageTypeMixin.append_stage(
            self, id_plan, time_start)
        # print 'Rides.append_stage',id_stage
        # print '  kwargs',kwargs
        self.set_row(id_stage,
                     ids_veh=kwargs['id_veh'],
                     ids_parking_from=kwargs['id_parking_from'],
                     ids_parking_to=kwargs['id_parking_to'],
                     )

        # attention: here time_end is not valid because duration is not yet set
        ind = self.get_ind(id_stage)
        # print '
        # duration,ids_parking_from,ids_parking_to',duration,self.ids_parking_from[id_stage],self.ids_parking_to[id_stage],ids_edge

        if duration == None:
            # this will route and compute duration
            duration = self.parent.get_individualvehicles().route_between_parking(
                self.ids_veh.value[ind], self.ids_parking_from.value[ind], self.ids_parking_to.value[ind])
        else:
            # case wehn route has been already determined
            self.parent.get_individualvehicles().set_route(self.ids_veh.value[
                ind], ids_edge, self.ids_parking_from.value[ind], self.ids_parking_to.value[ind])

        self.durations[id_stage] = duration
        return id_stage, time_start + duration

    def to_xml(self, id_stage, fd, indent=0):
        #lanes = self.parent.get_scenario().net.lanes
        scenario = self.parent.get_scenario()

        edgeindex = scenario.net.edges.ids_sumo
        parking = scenario.landuse.parking

        ind = self.get_ind(id_stage)
        fd.write(xm.start('ride', indent=indent))

        id_edge_from, pos_from = parking.get_edge_pos_parking(
            self.ids_parking_from.value[ind])
        id_edge_to, pos_to = parking.get_edge_pos_parking(
            self.ids_parking_to.value[ind])

        # edgeindex.get_index_from_id(self.ids_edge_to.value[ind])
        fd.write(xm.num('from', edgeindex[id_edge_from]))
        fd.write(xm.num('to',  edgeindex[id_edge_to]))
        fd.write(xm.num('lines', self.ids_veh.value[ind]))
        # if self.cols.pos_edge_from[ind]>0:
        #    fd.write(xm.num('departPos', self.cols.pos_edge_from[ind]))
        # if self.cols.pos_edge_to[ind]>0:
        #    fd.write(xm.num('arrivalPos', self.cols.pos_edge_to[ind]))

        fd.write(xm.stopit())  # ends stage


class Walks(StageTypeMixin):

    def __init__(self, ident, population, edges, name='Walks', info='walk from a position on a lane to another position of another lane.'):
        self.init_stagetype(ident, population, name=name, info=info)

        # TODO: this structure needs review: the private vehicle is part of a person, not a stage
        # street parking at home and work could be in stage. Private garage is
        # part of person...
        self.add_col(am.IdsArrayConf('ids_edge_from', edges,
                                     groupnames=['state'],
                                     name='ID Dep. edge',
                                     info='Edge ID at departure of walk.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_edge_to', edges,
                                     groupnames=['state'],
                                     name='ID Arr. edge',
                                     info='Edge ID where walk finishes.',
                                     ))

        self.add_col(am.ArrayConf('positions_from', 0.0,
                                  dtype=np.float32,
                                  #choices = OPTIONMAP_POS_DEPARTURE,
                                  perm='r',
                                  name='Depart pos',
                                  unit='m',
                                  info="Position on edge at the moment of departure.",
                                  xmltag='departPos',
                                  #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                  ))

        self.add_col(am.ArrayConf('positions_to', 0.0,
                                  dtype=np.float32,
                                  #choices = OPTIONMAP_POS_ARRIVAL,
                                  perm='r',
                                  name='Arrival pos',
                                  unit='m',
                                  info="Position on edge at the moment of arrival.",
                                  xmltag='arrivalPos',
                                  #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                  ))

        self.add_col(am.ArrayConf('speeds', 0.8,
                                  dtype=np.float32,
                                  #choices = OPTIONMAP_SPEED_ARRIVAL,
                                  perm='r',
                                  name='Walk speed',
                                  unit='m/s',
                                  info='Average walking speed.',
                                  xmltag='speed',
                                  #xmlmap = get_inversemap(OPTIONMAP_SPEED_ARRIVAL),
                                  ))

    def append_stage(self, id_plan, time_start=-1.0,
                     ids_edge=[], duration=None,
                     **kwargs):

        id_stage, time_end = StageTypeMixin.append_stage(
            self, id_plan, time_start)
        # print 'Walks.append_stage',id_stage
        # print '  kwargs',kwargs
        self.set_row(id_stage,
                     ids_edge_from=kwargs['id_edge_from'],
                     ids_edge_to=kwargs['id_edge_to'],
                     positions_from=kwargs['position_edge_from'],
                     positions_to=kwargs['position_edge_to'],
                     speeds=kwargs.get('speed', 0.8),
                     )

        return id_stage, time_end

    def to_xml(self, id_stage, fd, indent=0):
        #scenario = self.parent.get_scenario()
        #edges = scenario.net.edges
        edgeindex = self.ids_edge_from.get_linktab().ids_sumo
        ind = self.get_ind(id_stage)
        fd.write(xm.start('walk', indent=indent))
        fd.write(xm.num('from', edgeindex[self.ids_edge_from.value[ind]]))
        fd.write(xm.num('to', edgeindex[self.ids_edge_to.value[ind]]))
        if self.positions_from.value[ind] > 0:
            fd.write(xm.num('departPos', self.positions_from.value[ind]))
        if self.positions_to[ind] > 0:
            fd.write(xm.num('arrivalPos', self.positions_to.value[ind]))

        fd.write(xm.stopit())  # ends walk


class Activities(StageTypeMixin):

    def __init__(self, ident, population, lanes, name='Activities'):
        self.init_stagetype(ident, population, name=name,
                            info='Do some activity  at a position of a lane.')

        # TODO: this structure needs review: the private vehicle is part of a person, not a stage
        # street parking at home and work could be in stage. Private garage is
        # part of person...
        self.add_col(am.IdsArrayConf('id_lane', lanes,
                                     groupnames=['state'],
                                     name='ID lane',
                                     info='Lane ID at which activity takes place.',
                                     ))
        self.add_col(am.ArrayConf('pos_lane', 0.0,
                                  name='Lane pos.',

                                  unit='m',
                                  info='Position on Lane on where activity takes place.',
                                  xml='pos_start',
                                  ))

        self.add_col(am.ArrayConf('activity', '',
                                  dtype=np.object,
                                  name='Activity',
                                  info='Type of activity performed during the stop.',
                                  ))

    def to_xml(self, id_stage, fd, indent=0):

        # <stop lane="1/4to2/4_0" duration="20" startPos="40" actType="singing"/>

        ind = self.get_ind(id_stage)
        fd.write(xm.start('stop', indent=indent))
        fd.write(xm.num('lane', self.cols.id_lane[ind]))
        fd.write(xm.num('startPos', self.cols.pos_lane[ind]))
        fd.write(xm.num('duration', self.cols.duration[ind]))
        fd.write(xm.num('actType', self.cols.activity[ind]))

        fd.write(xm.stopit())  # ends activity


class Virtualpolulation(am.ArrayObjman):

    def __init__(self, demand, **kwargs):
        self._init_objman(ident='virtualpop',
                          parent=demand,
                          name='Virtual polulation',
                          info='Contains information of each individual of the virtual population.',
                          #xmltag = ('plans','plan',None),
                          **kwargs)

        scenario = demand.get_scenario()
        #--------------------------------------------------------------------
        # plans table
        self.add(cm.ObjConf(Plans(self)))
        #--------------------------------------------------------------------
        # individual vehicles table
        self.add(cm.ObjConf(IndividualVehicles('individualvehicles', self,
                                               demand.vtypes, scenario.net.edges, scenario.landuse.parking)))

        #--------------------------------------------------------------------
        # walks table
        #self.walks = self.add_tableman(Walks(self))
        self.add(cm.ObjConf(Walks('walks', self, scenario.net.edges)))

        #--------------------------------------------------------------------
        # rides table
        self.add(cm.ObjConf(Rides('rides', self, scenario.landuse.parking)))

        #--------------------------------------------------------------------
        # pt rides table
        # self.add(cm.ObjConf(Transits(self, publictransport))   )

        #--------------------------------------------------------------------
        # Activities table
        self.add(
            cm.ObjConf(Activities('activities', self, scenario.net.lanes)))

        #--------------------------------------------------------------------
        # misc params
        # this is option for intermodal routing process
        # self.add(AttrConf(  'dist_max_wait', kwargs.get('dist_max_wait',20.0),
        #                                groupnames = ['options'],
        #                                perm='wr',
        #                                unit = 'm',
        #                                name = 'Max dist. Wait',
        #                                info = 'The maximum distance between two stops or platforms, when a person during transfer is not walking, but just waiting for the next bus/train to come.' ,
        #                                #xmltag = 'pos',
        #                                ))
        #--------------------------------------------------------------------
        # columns

        self.add_col(am.ArrayConf('identification ', '',
                                  dtype=np.object,
                                  name='Name',
                                  info='Identification or name of person.',
                                  ))

        self.add_col(am.IdsArrayConf('ids_fac_home', scenario.landuse.facilities,
                                     groupnames=['state'],
                                     name='ID home fac.',
                                     info='Facility ID of home.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_fac_activity', scenario.landuse.facilities,
                                     groupnames=['state'],
                                     name='ID activity fac.',
                                     info='Facility ID of location of main activity (work, study).',
                                     ))

        self.add_col(am.IdsArrayConf('ids_mode_preferred', scenario.net.modes,
                                     name='ID preferred mode',
                                     info='ID of preferred transport mode of person.',
                                     ))

        self.add_col(am.ArrayConf('times_start', 0,
                                  name='Start time',
                                  unit='s',
                                  info='Time when person leaves home facility for doing some activity.',
                                  ))

        # this could be extended to a list with more plans
        self.add_col(am.IdsArrayConf('ids_plan', self.get_plans(),
                                     name='ID Plan',
                                     info='Currently best mobility plan ID of person.',
                                     ))

        self.add_col(am.IdlistsArrayConf('lists_ids_plan', self.get_plans(),
                                         name='IDs Plans',
                                         info='List with the IDs of feasible mobility plans.',
                                         ))

    def get_plans(self):
        return self.plans.get_value()

    def get_individualvehicles(self):
        return self.individualvehicles.get_value()

    def get_landuse(self):
        return self.parent.get_scenario().landuse

    def get_scenario(self):
        return self.parent.get_scenario()

    def disaggregate_odflow(self,   time_start, time_end, id_mode,
                            id_zone_orig, id_zone_dest, tripnumber,
                            probs_fac, ids_fac,
                            id_landusetype_orig,
                            id_landusetype_dest,):
        print 'disaggregate_odflow', time_start, time_end, id_mode, id_zone_orig, id_zone_dest, tripnumber
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
        print 'disaggregate_odm', tripnumber, id_mode, id_zone_orig, id_zone_dest
        probs_orig = probs_fac[id_zone_orig][id_landusetype_orig]
        probs_dest = probs_fac[id_zone_dest][id_landusetype_dest]

        print '  probs_orig', sum(probs_orig)  # ,'\n',probs_orig
        print '  probs_dest', sum(probs_dest)  # ,'\n',probs_dest

        # is there a chance to find facilities to locate persons in
        # origin and destination zone
        # print '  ',self.times_start.value.dtype,np.random.randint(time_start,time_end,tripnumber)
        # print '  ',self.ids_mode_preferred.value.dtype,id_mode *
        # np.ones(tripnumber, np.int32),
        if (np.sum(probs_orig) > 0) & (np.sum(probs_dest) > 0):
            return self.add_rows(n=tripnumber,
                                 ids_fac_home=ids_fac[
                                     random_choice(tripnumber, probs_orig)],
                                 ids_fac_activity=ids_fac[
                                     random_choice(tripnumber, probs_dest)],
                                 ids_mode_preferred=id_mode *
                                 np.ones(tripnumber, dtype=np.int32),
                                 times_start=np.random.randint(
                                     time_start, time_end, tripnumber)
                                 )
        else:
            return []

    def create_pop_from_ods(self, landusetype_orig='residential', landusetype_dest='industrial'):
        print 'create_pop_from_ods'
        demand = self.parent
        odflowtab = demand.odintervals.generate_odflows()

        probs_fac, ids_fac = self.get_landuse(
        ).facilities.get_departure_probabilities()
        landusetypes = self.get_landuse().landusetypes

        for id_flow in odflowtab.get_ids():
            self.disaggregate_odflow(odflowtab.times_start[id_flow],
                                     odflowtab.times_end[id_flow],
                                     odflowtab.ids_mode[id_flow],
                                     odflowtab.ids_orig[id_flow],
                                     odflowtab.ids_dest[id_flow],
                                     odflowtab.tripnumbers[id_flow],
                                     probs_fac, ids_fac,
                                     landusetypes.typekeys.get_id_from_index(
                                         landusetype_orig),
                                     landusetypes.typekeys.get_id_from_index(
                                         landusetype_dest),
                                     #landusetype_orig = 'residential',
                                     #landusetype_dest = 'industrial',
                                     # od_factypes=('residential','industrial','commercial','leisure')# no!!
                                     # od_factypes=('residential','industrial')
                                     )
        # return odflowtab

    def get_route_between_parking(self, id_parking_from, id_parking_to, id_veh=-1):
        """
        Return route and distance of ride with vehicle type  vtype
        between id_parking_from and id_parking_to

        """
        # print 'get_route_between_parking',id_parking_from, id_parking_to
        scenario = self.parent.get_scenario()
        edges = scenario.net.edges
        lanes = scenario.net.lanes
        # print   self.get_demand().getVehicles().cols.maxSpeed
        #v_max = self.get_demand().getVehicles().maxSpeed.get(vtype)
        parking = scenario.landuse.parking

        ids_lanes = parking.ids_lane[[id_parking_from, id_parking_to]]
        id_edge_from, id_edge_to = lanes.ids_edge[ids_lanes]
        pos_from, pos_to = parking.positions[[id_parking_from, id_parking_to]]

        # print '  id_edge_from, id_edge_to=',id_edge_from, id_edge_to
        costs, tree = routing.edgedijkstra(
            id_edge_from, scenario.net.nodes, scenario.net.edges, set([id_edge_to, ]))
        dist, route = routing.get_mincostroute_edge2edge(
            id_edge_from, id_edge_to, costs, tree)

        # here is a big problem: starting with the successive node of edge_from
        # may result that the first edge of the route is not  connected with edge_from
        # And arriving at the preceding node of edge_to may result that from
        # the last edge in route the edge_to is not connected.

        #route = [edge_from]+route+[edge_to]
        dist = dist - pos_from - (edges.lengths[id_edge_to] - pos_to)
        # print 'get_route_between_parking',    dist,type(route),route#[-1].getLength()
        # print '  pos_from ,pos_to ', pos_from ,pos_to v=s/t

        # TODO: this should be a method of edges, icluding position and id_mode
        duration_approx = np.sum(
            edges.lengths[route] / edges.speeds_max[route])

        return route, dist, duration_approx

    def get_route(self,  id_edge_from, id_edge_to, pos_from, pos_to, id_mode=-1):
        print 'get_route', id_edge_from, id_edge_to
        scenario = self.parent.get_scenario()
        edges = scenario.net.edges
        #lanes = scenario.net.lanes
        costs, tree = routing.edgedijkstra(
            id_edge_from, scenario.net.nodes, edges, set([id_edge_to, ]))
        dist, route = routing.get_mincostroute_edge2edge(
            id_edge_from, id_edge_to, costs, tree)
        dists = edges.lengths[route]

        # TODO: this should be a method of edges, icluding position and id_mode
        durations_approx = dists / edges.speeds_max[route]
        return route, dists, durations_approx

    def add_plans(self, ids_person):
        ids_plan = self.plans.value.add_rows(
            n=len(ids_person), ids_person=ids_person)
        self.ids_plan[ids_person] = 1 * ids_plan
        for id_person, id_plan in zip(ids_person, ids_plan):
            if self.lists_ids_plan[id_person] == None:
                self.lists_ids_plan[id_person] = [id_plan]
            else:
                self.lists_ids_plan[id_person].append(id_plan)
        return ids_plan

    def make_plans_private(self, ids_person=None):

        if ids_person == None:
            # print '  ids_mode_preferred',self.ids_mode_preferred.value
            # print '  private',MODES['private']
            # print '  ',self.ids_mode_preferred == MODES['private']

            ids_person = self.select_ids(
                self.ids_mode_preferred.value == MODES['private'])

        ids_plan = self.add_plans(ids_person)

        n_plans = len(ids_plan)

        print 'make_plans_private n_plans=', n_plans
        landuse = self.get_landuse()
        facilities = landuse.facilities
        parking = landuse.parking

        scenario = self.parent.get_scenario()
        #edges = scenario.net.edges
        lanes = scenario.net.lanes
        # experimental: give all persons a car
        # print '  self.plans.cols.id_person',self.plans.cols.id_person
        # print '  self.plans.cols.probab',self.plans.cols.probab
        # print '  self.plans.cols.stages',self.plans.cols.stages
        # create an individual  vehicle for each person
        # here vehicle is created for id_pers, type variations dependent on person can be made
        # print '
        # ids_vtype',self.parent.vtypes.ids_sumo.get_id_from_index('passenger1')
        ids_veh = self.individualvehicles.value.add_rows(n=n_plans,
                                                         ids_person=ids_person,
                                                         ids_vtype=self.parent.vtypes.ids_sumo.get_id_from_index(
                                                             'passenger1') * np.ones(n_plans),
                                                         )

        inds_pers = self.get_inds(ids_person)
        # self.persons.cols.mode_preferred[inds_pers]='private'

        times_start = self.times_start.value[inds_pers]
        inds_fac_home = facilities.get_inds(self.ids_fac_home.value[inds_pers])
        inds_fac_activity = facilities.get_inds(
            self.ids_fac_activity.value[inds_pers])

        centroids_home = facilities.centroids.value[inds_fac_home]
        centroids_activity = facilities.centroids.value[inds_fac_activity]

        ids_edge_home = facilities.ids_roadedge_closest.value[inds_fac_home]
        poss_edge_home = facilities.positions_roadedge_closest.value[
            inds_fac_home]

        # this method will find and occupy parking space
        ids_parking_home, inds_parking_home = parking.assign_parking(
            ids_veh, centroids_home)
        ids_parking_activity, inds_parking_activity = parking.assign_parking(
            ids_veh, centroids_activity)

        ids_lane_parking_home = parking.ids_lane.value[inds_parking_home]
        ids_edge_parking_home = lanes.ids_edge[ids_lane_parking_home]
        poss_edge_parking_home = parking.positions.value[inds_parking_home]

        ids_lane_parking_activity = parking.ids_lane.value[
            inds_parking_activity]
        ids_edge_parking_activity = lanes.ids_edge[ids_lane_parking_activity]
        poss_edge_parking_activity = parking.positions.value[
            inds_parking_activity]

        ids_edge_activity = facilities.ids_roadedge_closest.value[
            inds_fac_activity]
        poss_edge_activity = facilities.positions_roadedge_closest.value[
            inds_fac_activity]

        i = 0
        for id_person, id_plan, time_start, id_veh, id_edge_home, pos_edge_home, id_edge_parking_home, pos_edge_parking_home, id_parking_home, id_parking_activity, id_edge_parking_activity, pos_edge_parking_activity, id_edge_activity, pos_edge_activity\
                in zip(ids_person, ids_plan, times_start, ids_veh, ids_edge_home, poss_edge_home, ids_edge_parking_home, poss_edge_parking_home, ids_parking_home, ids_parking_activity, ids_edge_parking_activity, poss_edge_parking_activity, ids_edge_activity, poss_edge_activity):
            self.plans.value.set_row(id_plan, ids_person=id_person)

            time = time_start

            # prerouting car trip to see whether work place is accessible or whether it is worth while taking the car
            # TODO...we have already identified parking edges!!
            ids_edge_car, dists_car, duration_approx_car = self.get_route_between_parking(
                id_parking_home, id_parking_activity, id_veh)
            print 79 * '_'
            print '  id_plan=%d, id_person=%d, ids_veh=%d, n_edges=%d' % (id_plan, id_person,  id_veh, len(ids_edge_car))
            # simple check for reachibility of destination
            if len(ids_edge_car) > 0:
                # print '  found route between parling duration_approx_car',duration_approx_car
                # preroute walking
                ids_edge_walk, dists_walk, durations_walk = self.get_route(
                    id_edge_parking_activity, id_edge_activity, pos_edge_home, pos_edge_activity, id_mode=MODES['pedestrian'])

                # walk from nearest street at home facility to car parking
                ids_edge_walk, dists_walk, durations_walk = self.get_route(
                    id_edge_home, id_edge_parking_home, pos_edge_home, pos_edge_parking_home, id_mode=MODES['pedestrian'])
                id_stage, time = self.walks.value.append_stage(id_plan, time, duration=sum(durations_walk),
                                                               id_edge_from=id_edge_home, position_edge_from=pos_edge_home,
                                                               id_edge_to=id_edge_parking_home, position_edge_to=pos_edge_parking_home -
                                                               7.0,
                                                               )

                # self.append_stage(  id_plan, self.walks,
                #                    id_edge_from = id_edge_home, pos_edge_from = pos_edge_home,
                #                    id_edge_to = id_edge_parking, pos_edge_to = pos_parking,
                #                    speed = self.speed_walking)

                # ride from  car parking to road edge near activity
                id_stage, time = self.rides.value.append_stage(id_plan, time,
                                                               id_veh=id_veh,
                                                               ids_edge=ids_edge_car,
                                                               duration=duration_approx_car,
                                                               id_parking_from=id_parking_home,
                                                               id_parking_to=id_parking_activity,
                                                               )
                # walk from car parking to activity facility
                ids_edge_walk, dists_walk, durations_walk = self.get_route(
                    id_edge_parking_activity, id_edge_activity, pos_edge_home, pos_edge_activity, id_mode=MODES['pedestrian'])

                id_stage, time = self.walks.value.append_stage(id_plan, time, duration=sum(durations_walk),
                                                               id_edge_from=id_edge_parking_activity, position_edge_from=pos_edge_parking_activity -
                                                               7.0,
                                                               id_edge_to=id_edge_activity, position_edge_to=pos_edge_activity,
                                                               )
            else:
                print '  no route between parking, better walk.'

    def export_sumoxml(self, filepath=None, encoding='UTF-8'):
        """
        Export trips to SUMO xml file.
        """
        if filepath == None:
            filepath = self.get_tripfilepath()
        print 'export_sumoxml', filepath
        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in write_obj_to_xml: could not open', filepath
            return False
        #xmltag, xmltag_item, attrname_id = self.xmltag
        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        fd.write(xm.begin('routes'))
        indent = 2

        #ids_modes_used = set(self.parent.vtypes.ids_mode[self.ids_vtype.get_value()])
        #----------------------
        # write vtypes, pedestrian included
        ids_vtypes = set(self.individualvehicles.value.ids_vtype.get_value())
        # print '  vtypes',vtypes
        #vtypes = vtypes.union(ptlines.get_vtypes())
        # print '  vtypes',vtypes
        ids_vtypes.add(MODES['pedestrian'])
        self.parent.vtypes.write_xml(fd, indent=indent,
                                     ids=ids_vtypes,
                                     is_print_begin_end=False)

        self._export_plans(fd, indent)

        fd.write(xm.end('routes'))
        fd.close()
        return filepath

    def _export_plans(self, fd, indent=0):
        if len(self) == 0:
            return

        scenario = self.parent.get_scenario()
        edges = scenario.net.edges
        lanes = scenario.net.lanes
        landuse = self.get_landuse()
        # ptlines=self.get_ptlines()
        persons = self  # instance holding all arrays for persons
        # instance holding all arrays for individual vehicles
        ivehs = self.individualvehicles.value
        parking = landuse.parking  # instance holding all arrays for parking
        plans = self.plans.value

        # sort users by initial time
        # print '  persons.times_start.value',persons.times_start.value
        # print '  persons.get_inds()',persons.get_inds()
        times_sorted = np.concatenate(
            (persons.times_start.value.reshape(-1, 1),  persons.get_inds().reshape(-1, 1)), 1).tolist()
        times_sorted.sort()
        # print '  times_sorted',times_sorted
        inds_pers = np.array(times_sorted, int)[:, 1]
        # print '  inds_pers',inds_pers

        # used to start first vehcile appearance
        time_first, ind_first = times_sorted[0]

        vehindex = self.parent.vtypes.ids_sumo  # vehindex.get_index_from_ids()
        edgeindex = edges.ids_sumo
        # pt
        # self.parent.get_ptlines().to_routes(fd,2)

        #----------------------
        # write vehicles first
        inds_veh = ivehs.get_inds()
        for id_veh, id_vtype, ids_edge, ids_parking in zip(ivehs.get_ids(inds_veh), ivehs.ids_vtype.value[inds_veh], ivehs.routes.value[inds_veh], ivehs.ids_parkings.value[inds_veh]):
            if (ids_edge != None) & (ids_parking != None):
                if len(ids_edge) > 0:
                    # print "  vehicle id=", id_veh,ids_edge, ids_parking

                    inds_parking = parking.get_inds(ids_parking)
                    # first depart position  equals position of first parking
                    pos_depart = parking.positions.value[
                        inds_parking][0]  # get pos of first parking

                    fd.write(xm.start('vehicle id="%s"' % id_veh, indent + 2))
                    fd.write(xm.num('depart', '%.1f' % time_first))
                    fd.write(xm.num('type', vehindex[id_vtype]))
                    fd.write(xm.stop())

                    # write route
                    fd.write(xm.start('route', indent + 4))
                    fd.write(xm.arr('edges', edgeindex[ids_edge], indent + 6))
                    fd.write(xm.num('departPos', pos_depart))
                    # depart lane is 1 , 0 would be on the side-walk)
                    fd.write(xm.num('departLane', lanes.indexes[
                             parking.ids_lane[ids_parking[0]]]))
                    fd.write(xm.stopit())

                    # write stops
                    ids_lane_parking = parking.ids_lane.value[inds_parking]
                    ids_edge_parking = lanes.ids_edge[ids_lane_parking]
                    laneindexes_parking = lanes.indexes[ids_lane_parking]

                    poss_end_parking = parking.positions[inds_parking]
                    poss_start_parking = poss_end_parking - \
                        parking.lengths[inds_parking]
                    for id_edge_parking, laneindex_parking, pos_start_parking, pos_end_parking in zip(ids_edge_parking, laneindexes_parking, poss_start_parking, poss_end_parking):
                        fd.write(xm.start('stop', indent + 4))
                        fd.write(xm.num('lane', edges.ids_sumo[
                                 id_edge_parking] + '_%d' % laneindex_parking))
                        fd.write(xm.num('duration', 3000))
                        fd.write(xm.num('startPos', pos_start_parking))
                        fd.write(xm.num('endPos', pos_end_parking))
                        fd.write(xm.num('triggered', "True"))
                        fd.write(xm.stopit())

                    fd.write(xm.end('vehicle', indent + 2))

        #----------------------
        # write plans

        # print '  inds_pers',inds_pers
        # print '  times_start[inds_pers]',persons.times_start.value[inds_pers]
        for time_start, id_pers, stages in zip(persons.times_start.value[inds_pers], persons.get_ids(inds_pers), plans.stagelists[persons.ids_plan.value[inds_pers]]):
            print '  time_start,id_pers', time_start, id_pers, stages != None
            if stages != None:
                # TODO: self.persons.to_xml()??..pass all attrs?
                fd.write(xm.start('person', indent=indent + 2))
                fd.write(xm.num('id', id_pers))
                fd.write(xm.num('depart', time_start))
                fd.write(xm.num('type', 'pedestrian'))
                fd.write(xm.stop())

                for stage, id_stage in stages:
                    stage.to_xml(id_stage, fd, indent + 4)

                fd.write(xm.end('person', indent=indent + 2))
