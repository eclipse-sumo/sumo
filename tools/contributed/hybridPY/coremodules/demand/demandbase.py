import os,time,sys
import numpy as np
from xml.sax import saxutils, parse, handler
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import random_choice, get_inversemap, dict_to_str
from coremodules.network import routing
from coremodules.network.network import SumoIdsConf,MODES


OPTIONMAP_POS_DEPARTURE = {"random": -1, "free": -2,
                           "random_free": -3, "base": -4, "last": -5, "first": -6}
OPTIONMAP_POS_ARRIVAL = {"random": -1, "max": -2}
OPTIONMAP_SPEED_DEPARTURE = {"random": -1, "max": -2}
OPTIONMAP_SPEED_ARRIVAL = {"current": -1}

OPTIONMAP_LANE_DEPART = {"random": -1, "free": -2,
                         "allowed": -3, "best": -4, "first": -5}
                         
OPTIONMAP_LANE_ARRIVAL = {"current": -1}


class TripoptionMixin:
    """
    Class mixin wich defines some trip options.
    To be used with different processes.
    """
    
    def add_posoptions(self):
        attrsman = self.get_attrsman()
        
        self.add_option('pos_depart', OPTIONMAP_POS_DEPARTURE['base'],
                        groupnames = ['options'],# 
                        cml = '--departpos',
                        name = 'Departure position',
                        unit = 'm',
                        info = "Departure position of vehicle.\n\nSpecial values:\n"\
                                 +dict_to_str(OPTIONMAP_POS_DEPARTURE, intend = 2),
                        cmlvaluemap = get_inversemap(OPTIONMAP_POS_DEPARTURE),
                        )
                        
        self.add_option('pos_arrival', OPTIONMAP_POS_ARRIVAL['max'],
                        groupnames = ['options'],# 
                        cml = '--arrivalpos',
                        name = 'Arival position',
                        unit = 'm',
                        info = "Arival position of vehicle.\n\nSpecial values:\n"\
                                 +dict_to_str(OPTIONMAP_POS_ARRIVAL, intend = 2),
                        cmlvaluemap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                        )
        
    def add_speedoptions(self):
        attrsman = self.get_attrsman()
        
        self.add_option('speed_depart', OPTIONMAP_SPEED_DEPARTURE['max'],
                        groupnames = ['options'],# 
                        cml = '--departspeed',
                        name = 'Departure speed',
                        unit = 'm/s',
                        info = "Departure speed of vehicle.\n\nSpecial values:\n"\
                                 +dict_to_str(OPTIONMAP_SPEED_DEPARTURE, intend = 2),
                        cmlvaluemap = get_inversemap(OPTIONMAP_SPEED_DEPARTURE),
                        )
                        
        self.add_option('speed_arrival', OPTIONMAP_SPEED_ARRIVAL['current'],
                        groupnames = ['options'],# 
                        cml = '--arrivalspeed',
                        name = 'Arival speed',
                        unit = 'm/s',
                        info = "Arival speed of vehicle.\n\nSpecial values:\n"\
                                 +dict_to_str(OPTIONMAP_SPEED_ARRIVAL, intend = 2),
                        cmlvaluemap = get_inversemap(OPTIONMAP_SPEED_ARRIVAL),
                        )
        
            
                        
    def add_laneoptions(self):
        attrsman = self.get_attrsman()
        
        self.add_option('ind_lane_depart', OPTIONMAP_LANE_DEPART['best'],
                        groupnames = ['options'],# 
                        cml = '--departlane',
                        name = 'Depart lane',
                        info = "Departure lane index. 0 is rightmost lane or sidewalk, if existant.\n\nSpecial values:\n"\
                                 +dict_to_str(OPTIONMAP_LANE_DEPART, intend = 2),
                        cmlvaluemap = get_inversemap(OPTIONMAP_LANE_DEPART),
                        )
                        
        
        
        self.add_option('ind_lane_arrival', OPTIONMAP_LANE_ARRIVAL['current'],
                        groupnames = ['options'],# 
                        cml = '--arrivallane',
                        name = 'Arrival lane',
                        info = "Arrival lane index. 0 is rightmost lane or sidewalk, if existant.\n\nSpecial values:\n"\
                                 +dict_to_str(OPTIONMAP_LANE_ARRIVAL, intend = 2),
                        cmlvaluemap = get_inversemap(OPTIONMAP_LANE_ARRIVAL),
                        )
                        
                       
        
    
class ModeShares(am.ArrayObjman):
    """
    Utility table with some default mode shares.
    """

    def __init__(self, ident, parent, modes, **kwargs):

        self._init_objman(ident, parent=parent, name='Mode shares',
                          version=0.0,
                          **kwargs)

        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['parameters'],
                                     name='Mode ID',
                                     info='Transport Mode ID.',
                                     ))

        self.add_col(am.ArrayConf('shares', '',
                                  dtype=np.float32,
                                  is_index=True,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Share',
                                  info='Mode share.',
                                  ))

        #self.add_col(am.ArrayConf( 'speeds_max', 50.0/3.6,
        #                            dtype = np.float32,
        #                            groupnames = ['parameters'],
        #                            perm='rw',
        #                            name = 'Max. Speed',
        #                            unit = 'm/s',
        #                            info = 'Maximum possible speed for this mode. Speed is used to estimate free flow link travel times, mainly for routig purposes. Note that speeds are usully limited by the lane speed attribute',
        #                            ))
        self._init_attributes()
        self.add_default()

    def _init_attributes(self, landuse=None):
        #self.add_col(SumoIdsConf('Activitytypes'))
        pass

    def add_share(self, mode, share):
        modes = self.ids_mode.get_linktab()
        return self.add_row(ids_mode=modes.get_id_from_formatted(mode),
                            shares=share)

    def add_default(self):
        """
        Sets the default maximum possible speed for certain modes.
        """
        self.add_share("pedestrian", 0.1)
        self.add_share("bicycle", 0.1)
        self.add_share("motorcycle", 0.1)
        self.add_share("passenger", 0.5)
        self.add_share("bus", 0.2)

    def get_modes_random(self, n):
        """
        Return a vector with mode IDs of length n.
        """
        ids = self.get_ids()
        ids_modes_all = self.ids_mode[ids]
        return ids_modes_all[random_choice(n, self.shares[ids])]


class ActivityTypes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions
    def __init__(self, ident, demand, **kwargs):

        self._init_objman(ident, parent=demand, name='Activity Types',
                          version=0.0,
                          xmltag=('actTypes', 'actType', 'names'),
                          **kwargs)

        self._init_attributes()
        self.add_default()

    def _init_attributes(self, landuse=None):
        #self.add_col(SumoIdsConf('Activitytypes'))

        self.add_col(am.ArrayConf('names', '',
                                  dtype=np.object_,
                                  is_index=True,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Type name',
                                  info='Human readable name of activity type.',
                                  ))

        self.add_col(am.ArrayConf('symbols', '',
                                  dtype=np.object_,
                                  perm='rw',
                                  is_index=True,
                                  name='Type symbol',
                                  info='Symbol of activity type name. Used to represent activity sequences.',
                                  ))

        self.add_col(am.ArrayConf('descriptions', '',
                                  dtype=np.object_,
                                  perm='rw',
                                  name='Description',
                                  info='Description of activity.',
                                  ))

        # this works only for first init
        #if landuse is not None:
        self.add_col(am.IdlistsArrayConf('ids_landusetypes', self.parent.get_scenario().landuse.landusetypes,
                                         name='Landuse types',
                                         info="Landuse type IDs, eher this activity type can take place.",
                                         ))

        self.add_col(am.ArrayConf('hours_begin_earliest', 0.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Earliest hour begin',
                                  unit='h',
                                  info='Default value for earliest hour when this activity can begin.',
                                  ))

        self.add_col(am.ArrayConf('hours_begin_latest', 1.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Latest begin hour',
                                  unit='h',
                                  info='Default value for latest hour when this activity can begin.',
                                  ))

        self.add_col(am.ArrayConf('durations_min', 6.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Min. Duration',
                                  unit='h',
                                  info='Default value for minimum activity duration for a person within a day.',
                                  ))

        self.add_col(am.ArrayConf('durations_max', 8.0,
                                  dtype=np.float32,
                                  groupnames=['parameters'],
                                  perm='rw',
                                  name='Max. Duration',
                                  unit='h',
                                  info='Default value for maximum activity duration for a person within a day.',
                                  ))

    def format_ids(self, ids):
        return ', '.join(self.names[ids])

    def get_id_from_formatted(self, idstr):
        return self.names.get_id_from_index(idstr)

    def get_ids_from_formatted(self, idstrs):
        return self.names.get_ids_from_indices_save(idstrs.split(','))

    def get_id_from_name(self, activitytypename):
        return self.names.get_id_from_index(activitytypename)

    def get_id_from_symbol(self, activitytypesymbol):
        return self.symbols.get_id_from_index(activitytypesymbol)

    def add_default(self):
        """
        Sets the default maximum possible speed for certain modes.
        """
        landusetypekeys = self.parent.get_scenario().landuse.landusetypes.typekeys
        self.add_row(names='none',
                     descriptions='None activity type. Will be skipped when planning.',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices([]),
                     symbols='n',
                     hours_begin_earliest=0.0,
                     hours_begin_latest=0.0,
                     durations_min=0.0,
                     durations_max=0.0,
                     )

        self.add_row(names='home',
                     descriptions='General home activity, like sleeping, eating, watching TV, etc.',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['residential', 'mixed']),
                     symbols='h',
                     hours_begin_earliest=-1.0,
                     hours_begin_latest=-1.0,
                     durations_min=7.0,
                     durations_max=8.0,
                     )

        self.add_row(names='work',
                     descriptions="""Work activity, for example work in
                     industry, offices or as employee at
                     educational facilities.""",
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['industrial', 'commercial', 'education', 'mixed']),
                     symbols='w',
                     hours_begin_earliest=8.5,
                     hours_begin_latest=9.0,
                     durations_min=6.0,
                     durations_max=9.0,
                     )

        self.add_row(names='education',
                     descriptions='Education activity, for example visiting courses at schools or at universities.',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['education', ]),
                     symbols='e',
                     hours_begin_earliest=8.0,
                     hours_begin_latest=10.0,
                     durations_min=4.0,
                     durations_max=6.0,
                     )

        self.add_row(names='shopping',
                     descriptions='Shopping activity',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['commercial', 'mixed']),
                     symbols='s',
                     hours_begin_earliest=16.0,
                     hours_begin_latest=19.0,
                     durations_min=0.2,
                     durations_max=2.0,
                     )

        self.add_row(names='leisure',
                     descriptions='Leisure activity',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['leisure', 'mixed']),
                     symbols='l',
                     hours_begin_earliest=12.0,
                     hours_begin_latest=15.0,
                     durations_min=1.0,
                     durations_max=3.0,
                     )

        self.add_row(names='others',
                     descriptions='Other activity, for example visiting friends, relatives, going bank, post office',
                     ids_landusetypes=landusetypekeys.get_ids_from_indices(
                         ['commercial','leisure','residential', 'mixed']),
                     symbols='o',
                     hours_begin_earliest=9.0,
                     hours_begin_latest=18.0,
                     durations_min=1.0,
                     durations_max=3.0,
                     )
                     
class DemandobjMixin:
    def export_trips_xml(self, filepath=None, encoding='UTF-8',
                         ids_vtype_exclude=[], **kwargs):
        """
        Export trips to SUMO xml file.
        Method takes care of sorting trips by departure time.
        """
        return False

    def get_writexmlinfo(self, is_route=False, **kwargs):
        """
        Returns three array where the first array is the
        begin time of the first vehicle and the second array is the
        write function to be called for the respective vehicle and
        the third array contains the vehicle ids

        Method used to sort trips when exporting to route or trip xml file
        """
        return [], [], []

    def config_results(self, results):
        #tripresults = res.Tripresults(          'tripresults', results,
        #                                        self,
        #                                        self.get_net().edges
        #                                        )
        #
        #
        #results.add_resultobj(tripresults, groupnames = ['Trip results'])
        pass

    def process_results(self, results, process=None):
        pass

    def get_time_depart_first(self):
        return np.inf

    def get_time_depart_last(self):
        return 0.0
    
    def import_routes_xml(self, routefilepath, **kwargs):
        """
        Import routes from filepath and store them somwhere.
        Demand object specific.
        """
        
        pass
    
    def set_routes_sumo(self, ids_veh_sumo, ids_edges_sumo):
        """
        Sets already existing routes.
        Keys are an array with SUMO IDs of the vehicles.
        Routes are represented as an array of lists of SUMO edge IDs
        """
        pass
        
    def import_routealternatives_xml(self, routefilepath, **kwargs):
        """
        Import route alternatives from filepath and store them somwhere.
        Demand object specific.
        """
        pass

class TripCounter(handler.ContentHandler):
    """Parses a SUMO route XML file and counts trips."""

    def __init__(self):
        self.n_trip = 0


    def startElement(self, name, attrs):
        #print 'startElement',name,self.n_trip
        if name == 'trip':
            self.n_trip += 1



class TripReader(handler.ContentHandler):
    """Reads trips from trip or route file into trip table"""

    def __init__(self, trips,  n_trip, vtype_default = None, id_vtype_default = None):
        #print 'RouteReader.__init__',demand.ident
        self._trips = trips
        demand = trips.parent

        net = demand.get_scenario().net

        self._ids_vtype_sumo = demand.vtypes.ids_sumo
        self._modemap = net.modes.names.get_indexmap()
        self._get_vtype_for_mode = demand.vtypes.get_vtype_for_mode
        
        
        if vtype_default is not None:
                id_vtype_default = demand.vtypes.ids_sumo.get_id_from_index(vtype_default)

                
        if id_vtype_default is None:
            self._id_vtype_default = demand.vtypes.get_ids()[0]
        else:
            self._id_vtype_default = id_vtype_default

        self._ids_edge_sumo = net.edges.ids_sumo

        self.ids_sumo = np.zeros(n_trip, np.object_)
        self.ids_vtype = np.zeros(n_trip, np.int32)
        self.times_depart = np.zeros(n_trip, np.int32)
        self.ids_edge_depart = np.zeros(n_trip, np.int32)
        self.ids_edge_arrival = np.zeros(n_trip, np.int32)
        self.inds_lane_depart = np.zeros(n_trip, np.int32)
        self.positions_depart = np.zeros(n_trip, np.float32)
        self.speeds_depart = np.zeros(n_trip, np.float32)
        self.inds_lane_arrival = np.zeros(n_trip, np.int32)
        self.positions_arrival = np.zeros(n_trip, np.float32)
        self.speeds_arrival = np.zeros(n_trip, np.float32)
        self.routes = np.zeros(n_trip, np.object_)

        self._ind_trip = -1

        self._has_routes = False
        self._ids_sumoedge_current = []
        self._id_sumoveh_current = None
        #self._time_depart = 0
        #self._attrs = {}
        #self._is_generate_ids = is_generate_ids
        self._intervals_current = ''

    #def _init_reader(self):

    def _get_id_vtype(self, attrs):
        vtype = str(attrs['type'])
        if self._ids_vtype_sumo.has_index(vtype):
            # vtype is known
            return self._ids_vtype_sumo.get_id_from_index(vtype)

        # vtype is not known, so check out whether vtype
        # is actually a mode
        elif vtype in self._modemap:
            # pick id_vtype according to its probability with
            # all vtypes of this mode
            id_vtype = self._get_vtype_for_mode(self._modemap[vtype])

            if id_vtype >=0:
                return id_vtype
            else:
                return self._id_vtype_default

        else:
            # no valid vtype, get a default
            return self._id_vtype_default




    def startElement(self, name, attrs):
        #<vehicle id="3_21" type="bus" depart="2520.00">
        #<route edges="bottom1to1/0 1/0to0/0 0/0tobottom0"/>
        #</vehicle>
        #print 'startElement',name
        if name == 'trip':
            #print '  startElement',attrs['id'],attrs['depart']
            self._ind_trip += 1

            self._id_sumoveh_current = attrs['id']
            self.ids_sumo[self._ind_trip] = self._id_sumoveh_current
            #print 'startElement ids_vtype',attrs['type'], self._ids_vtype_sumo.get_id_from_index(str(attrs['type']))

            self.ids_vtype[self._ind_trip] = self._get_id_vtype(attrs)
            self.times_depart[self._ind_trip] = int(float(attrs['depart']))

            if 'from' in attrs:
                self.ids_edge_depart[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['from']))
            if 'to' in attrs:
                self.ids_edge_arrival[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['to']))


            ind_lane_depart_raw = attrs.get('departLane', 'free')
            if ind_lane_depart_raw in OPTIONMAP_LANE_DEPART:
                self.inds_lane_depart[self._ind_trip] = OPTIONMAP_LANE_DEPART[ind_lane_depart_raw]
            else:
                self.inds_lane_depart[self._ind_trip] = int(ind_lane_depart_raw)

            positions_depart_raw = attrs.get('departPos', 'base')
            if positions_depart_raw in OPTIONMAP_POS_DEPARTURE:
                self.positions_depart[self._ind_trip] = OPTIONMAP_POS_DEPARTURE[positions_depart_raw]
            else:
                self.positions_depart[self._ind_trip] = float(positions_depart_raw)

            speed_depart_raw = attrs.get('departSpeed', 'max')
            if positions_depart_raw in OPTIONMAP_POS_DEPARTURE:
                self.speeds_depart[self._ind_trip] = OPTIONMAP_SPEED_DEPARTURE[speed_depart_raw]
            else:
                self.speeds_depart[self._ind_trip] = float(positions_depart_raw)

            ind_lane_arrival_raw = attrs.get('arrivalLane', 'current')
            if ind_lane_arrival_raw in OPTIONMAP_LANE_ARRIVAL:
                self.inds_lane_arrival[self._ind_trip] = OPTIONMAP_LANE_ARRIVAL[ind_lane_arrival_raw]
            else:
                self.inds_lane_arrival[self._ind_trip] = int(ind_lane_arrival_raw)

            positions_arrival_raw = attrs.get('arrivalPos', 'max')
            if positions_arrival_raw in OPTIONMAP_POS_ARRIVAL:
                self.positions_arrival[self._ind_trip] = OPTIONMAP_POS_ARRIVAL[positions_arrival_raw]
            else:
                self.positions_arrival[self._ind_trip] =  float(positions_arrival_raw)


            self.speeds_arrival[self._ind_trip] = attrs.get('arrivalSpeed', 0.0)




    def _get_kwargs(self):
        return {'ids_sumo' : self.ids_sumo,
                'times_depart' : self.times_depart,
                'ids_edge_depart' : self.ids_edge_depart,
                'ids_edge_arrival' : self.ids_edge_arrival,
                'inds_lane_depart' : self.inds_lane_depart,
                'positions_depart' : self.positions_depart,
                'speeds_depart' : self.speeds_depart,
                'inds_lane_arrival' : self.inds_lane_arrival,
                'positions_arrival' : self.positions_arrival,
                'speeds_arrival' : self.speeds_arrival,
                }



    def insert_trips(self, is_generate_ids = True):

        #print 'TripReader.insert_trips self.ids_vtype',self.ids_vtype
        kwargs = self._get_kwargs()
        ids_trips = self._trips.make_trips( self.ids_vtype,
                                            is_generate_ids = is_generate_ids,
                                            **kwargs)


        return ids_trips

class RouteCounter(handler.ContentHandler):
    """Parses a SUMO route XML file and counts trips routes and other stuff."""

    def __init__(self):
        self.n_veh = 0
        self.n_pers = 0
        self.n_rou = 0
        

    def startElement(self, name, attrs):
        #print 'startElement',name,'n_veh',self.n_veh,self.n_pers
        if name == 'vehicle':
            self.n_veh += 1
        elif name == 'person':
            self.n_pers += 1
        elif name == 'route':
            if 'id' in attrs:
                self.n_rou += 1
        


        
        
class RouteReader(TripReader):
    """Reads trips from trip or route file into trip table"""

    def __init__(self, trips,  counter):
        
        self._trips = trips
        n_veh = counter.n_veh
        n_per = counter.n_pers
        #n_rou = counter.n_rou
        n_trip = n_veh+n_per
        demand = trips.parent
        print('RouteReader.__init__',demand.ident,'reading',n_trip,'routes of which n_veh',n_veh)
        net = demand.get_scenario().net
        self._modemap = net.modes.names.get_indexmap()
        self._get_vtype_for_mode = demand.vtypes.get_vtype_for_mode

        self._ids_vtype_sumo = demand.vtypes.ids_sumo
        self._ids_edge_sumo = net.edges.ids_sumo

        self.ids_sumo = np.zeros(n_trip, np.object_)
        self.ids_sumo[:] = ''
        self.ids_vtype = np.zeros(n_trip, np.int32)
        self.times_depart = np.zeros(n_trip, np.int32)
        self.times_arrival = np.zeros(n_trip, np.int32)
        self.type = np.zeros(n_trip, np.object_)
        self.ids_edge_depart = np.zeros(n_trip, np.int32)
        self.ids_edge_arrival = np.zeros(n_trip, np.int32)
        self.inds_lane_depart = np.zeros(n_trip, np.int32)
        self.positions_depart = np.zeros(n_trip, np.float32)
        self.speeds_depart = np.zeros(n_trip, np.float32)
        self.inds_lane_arrival = np.zeros(n_trip, np.int32)
        self.positions_arrival = np.zeros(n_trip, np.float32)
        self.speeds_arrival = np.zeros(n_trip, np.float32)
        self.routes = np.zeros(n_trip, np.object_)

        self._ind_trip = -1

        self._has_routes = False
        self._ids_sumoedge_current = []
        self._id_sumoveh_current = None
        #self._time_depart = 0
        #self._attrs = {}
        #self._is_generate_ids = is_generate_ids
        self._intervals_current = ''

    def startElement(self, name, attrs):
        #<vehicle id="3_21" type="bus" depart="2520.00">
        #<route edges="bottom1to1/0 1/0to0/0 0/0tobottom0"/>
        #</vehicle>
        #print 'startElement',name
        if name == 'vehicle':
            #print '  startElement',attrs['id'],attrs['depart']
            self._ind_trip += 1

            self._id_sumoveh_current = attrs['id']
            self.ids_sumo[self._ind_trip] = self._id_sumoveh_current
            #print 'startElement ids_vtype',attrs['type'], self._ids_vtype_sumo.get_id_from_index(str(attrs['type']))

            self.ids_vtype[self._ind_trip] = self.ids_vtype[self._ind_trip] = self._get_id_vtype(attrs)

            self.times_depart[self._ind_trip] = int(float(attrs['depart']))
            if 'arrival' in attrs:
                self.times_arrival[self._ind_trip] = int(float(attrs['arrival']))
            else:
                # duarouter is not calculating arrival time in results!
                self.times_arrival[self._ind_trip] = 0.0
                
            self.type[self._ind_trip] = attrs['type']
            if 'from' in attrs:
                self.ids_edge_depart[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['from']))
            if 'to' in attrs:
                self.ids_edge_arrival[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['to']))


            ind_lane_depart_raw = attrs.get('departLane', 'free')
            if ind_lane_depart_raw in OPTIONMAP_LANE_DEPART:
                self.inds_lane_depart[self._ind_trip] = OPTIONMAP_LANE_DEPART[ind_lane_depart_raw]
            else:
                self.inds_lane_depart[self._ind_trip] = int(ind_lane_depart_raw)

            positions_depart_raw = attrs.get('departPos', 'base')
            if positions_depart_raw in OPTIONMAP_POS_DEPARTURE:
                self.positions_depart[self._ind_trip] = OPTIONMAP_POS_DEPARTURE[positions_depart_raw]
            else:
                self.positions_depart[self._ind_trip] = float(positions_depart_raw)

            speed_depart_raw = attrs.get('departSpeed', 'max')
            if speed_depart_raw in OPTIONMAP_SPEED_DEPARTURE:
                self.speeds_depart[self._ind_trip] = OPTIONMAP_SPEED_DEPARTURE[speed_depart_raw]
            else:
                self.speeds_depart[self._ind_trip] =  float(speed_depart_raw)
                

            ind_lane_arrival_raw = attrs.get('arrivalLane', 'current')
            if ind_lane_arrival_raw in OPTIONMAP_LANE_ARRIVAL:
                self.inds_lane_arrival[self._ind_trip] = OPTIONMAP_LANE_ARRIVAL[ind_lane_arrival_raw]
            else:
                self.inds_lane_arrival[self._ind_trip] = int(ind_lane_arrival_raw)

            positions_arrival_raw = attrs.get('arrivalPos', 'max')
            if positions_arrival_raw in OPTIONMAP_POS_ARRIVAL:
                self.positions_arrival[self._ind_trip] = OPTIONMAP_POS_ARRIVAL[positions_arrival_raw]
            else:
                self.positions_arrival[self._ind_trip] =  float(positions_arrival_raw)

            speed_arrival_raw = attrs.get('arrivalSpeed', 'current')
            if speed_arrival_raw in OPTIONMAP_SPEED_ARRIVAL:
                self.speeds_arrival[self._ind_trip] = OPTIONMAP_SPEED_ARRIVAL[speed_arrival_raw]
            else:
                self.speeds_arrival[self._ind_trip] =  float(speed_arrival_raw)
                
            


        if name == 'route':
            self._has_routes = True
            #print ' ',attrs.get('edges', '')
            self._ids_sumoedge_current = attrs.get('edges', '')
            self._intervals_current = attrs.get('intervals', '')

    #def characters(self, content):
    #    if (len(self._route_current)>0)&(self._intervals_current!=''):
    #        self._intervals_current = self._intervals_current + content

    def endElement(self, name):

        if name == 'vehicle':
            #print 'endElement',name,self._id_current,len(self._intervals_current)
            if (self._id_sumoveh_current is not None):
                ids_edge = []
                for id_sumoedge in self._ids_sumoedge_current.split(' '):
                    if not id_sumoedge in ('',' ',','):
                        if self._ids_edge_sumo.has_index(id_sumoedge):
                            ids_edge.append(self._ids_edge_sumo.get_id_from_index(id_sumoedge.strip()) )
                self.routes[self._ind_trip] = ids_edge

                if len(ids_edge)>=1:
                    self.ids_edge_depart[self._ind_trip] = ids_edge[0]
                    self.ids_edge_arrival[self._ind_trip] = ids_edge[-1]

                self._id_sumoveh_current = None
                #self._attrs = {}
                self._ids_sumoedge_current = []

        #elif name in ['routes','trips']:
        #    self.make_trips()

    def process_intervals(self):
        interval = []
        es = self._intervals_current.rstrip().split(" ")
        for e in es:
            p = e.split(",")
            interval.append((float(p[0]), float(p[1])))
        self._intervals_current = ''
        return interval

    def _get_kwargs(self, inds = None):
        if inds is None:
            inds = np.arange(len(self.ids_sumo))
        return {'ids_sumo' : self.ids_sumo[inds],
                'times_depart' : self.times_depart[inds],
                'times_arrival' : self.times_arrival[inds],
                'type' : self.type[inds],
                'ids_edge_depart' : self.ids_edge_depart[inds],
                'ids_edge_arrival' : self.ids_edge_arrival[inds],
                'inds_lane_depart' : self.inds_lane_depart[inds],
                'positions_depart' : self.positions_depart[inds],
                'speeds_depart' : self.speeds_depart[inds],
                'inds_lane_arrival' : self.inds_lane_arrival[inds],
                'positions_arrival' : self.positions_arrival[inds],
                'speeds_arrival' : self.speeds_arrival[inds],
                'ids_edges': self.routes[inds]
                }

    def insert_routes(self, is_generate_ids = True, is_add = False, is_overwrite_only = False):
        print('TripReader.insert_routes is_generate_ids',is_generate_ids, 'is_add',is_add,'is_overwrite_only',is_overwrite_only)
        
        # self._trips is scenario trip database
        # self.ids_sumo is a list of SUMO IDs read from xml file
        
        if is_overwrite_only & (not is_add):
            is_generate_ids = False
            is_add = False
            # get trip ids from xml file
            # ony import routes ids from existing sumo ids
            
            # this is index of self.ids_sumo to be overwritten
            inds = np.flatnonzero(np.array(self._trips.ids_sumo.has_indices(self.ids_sumo)))
            #print '  overwrite trip ids_sumo',self.ids_sumo[inds]
            
            ids_trip =  np.array(self._trips.ids_sumo.get_ids_from_indices(self.ids_sumo[inds]),dtype=np.int32)
            #print '  ids_trip',ids_trip
            #print '   n_trips',len(ids_trip)
            
            ids_routes, ids_trip = self._trips.make_routes( self.ids_vtype[inds],
                                                    #is_generate_ids = is_generate_ids,# depricated
                                                    routes = self.routes[inds],
                                                    ids_trip = ids_trip,
                                                    is_add = is_add,
                                                    **self._get_kwargs(inds=inds)
                                                    )
            #print '  ids_routes',ids_routes
            
        else:    
            if is_add:
                # here we add the newly read routes as route alternatives
                
                #is_generate_ids = False
                # get trip ids from xml file
                
                inds = np.flatnonzero(self.ids_sumo!= '')# ony import routes from specified sumo ids
                #print '  self.ids_sumo',self.ids_sumo[inds]
                #print '  inds',inds
                ids_trip = np.array( self._trips.ids_sumo.get_ids_from_indices_save(self.ids_sumo[inds]),dtype=np.int32)
                inds_valid = np.array(ids_trip,dtype = np.int32)>-1
                
                ids_routes, ids_trip = self._trips.make_routes( self.ids_vtype[inds[inds_valid]],
                                                    #is_generate_ids = is_generate_ids,# depricated
                                                    routes = self.routes[inds[inds_valid]],
                                                    ids_trip = ids_trip[inds_valid],
                                                    is_add = is_add,
                                                    **self._get_kwargs(inds=inds[inds_valid])
                                                    )
                
            if (not is_add) & is_generate_ids:
                # here we generate new trips and set current route
                
                inds = np.arange(len(self.routes))
                ids_trip = None
                
                #print '  ids_trip',ids_trip
                #print('   n_trips',len(self.routes))
                ids_routes, ids_trip = self._trips.make_routes( self.ids_vtype[inds],
                                                    is_generate_ids = is_generate_ids,
                                                    routes = self.routes[inds],
                                                    ids_trip = ids_trip,
                                                    is_add = is_add,
                                                    **self._get_kwargs(inds=inds)
                                                    )

        return ids_routes, ids_trip

class RouteAlternativesReader(RouteReader):
    """Reads alternative routes from XML file"""
    # <vehicle id="ptline.751.0" depart="78040.00" type="bus" fromTaz="gneE17" toTaz="149916593" departLane="free" departSpeed="max">
    #    <routeDistribution>
    #        <route cost="221.68" probability="0.6" edges="gneE17 28511775#0 28511775#4 149955196#0 149955196#1 149955196#2 26486220#0 26486220#2 26486228#1 479678162#0 479678162#1 26466212#0 26466212#1 26466212#2 26466212#3 26466212#4 26466212#5 150011437#1 150011437#2 150011437#3 149916610#0 149916610#1 808909663 808909662 -617824876#8 -617824876#6 -617824876#5 -617824876#3 -617824876#0 -400796239#12 -400796239#11 -400796239#9 -400796239#8 -400796239#7 -400796239#6 -400796239#0 -400796237#9 -400796237#8 -400796237#7 -400796237#6 -400796237#5 -400796237#4 -400796237#3 -400796237#2 -149916604 -26465947 -149955201 -455561979#7 -455561979#5 -455561979#3 -455561979#2 -455561979#1 -149916589#7 -149916589#6 -149916589#5 -149916589#4 -149916589#3 -149916589#2 -149916589#1 26466146#1 26466146#2 26466146#3 26466146#5 685786963 685786962 149916592#0 149916592#1 149916592#2 149916592#3 149916593"/>
    #        <route cost="226.78" probability="0.4" edges="gneE17 28511775#0 28511775#4 149955196#0 149955196#1 149955196#2 26486220#0 26486220#2 26486228#1 479678162#0 479678162#1 26466212#0 26466212#1 26466212#2 26466212#3 26466212#4 26466212#5 150011437#1 150011437#2 150011437#3 149916610#0 149916610#1 808909663 808909662 -617824876#8 -617824876#6 -617824876#5 -617824876#3 -617824876#0 -400796239#12 -400796239#11 -400796239#9 -400796239#8 -400796239#7 -400796239#6 -400796239#0 -400796237#9 -400796237#8 -400796237#7 -400796237#6 -400796237#5 -400796237#4 -400796237#3 -400796237#2 -149916604 -26465947 -149955201 -455561979#7 -455561979#5 -455561979#3 -455561979#2 -455561979#1 -149916589#7 -149916589#6 -149916589#5 -149916589#4 -149916589#3 -149916589#2 -149916589#1 26466146#1 26466146#2 26466146#3 26466146#5 685786963 685786962 149916592#0 149916592#1 149916592#2 149916592#3 149916593"/>
    #    </routeDistribution>
    # </vehicle>
    # 
    # 
    
    def __init__(self, trips,  counter):
        
        
        self._trips = trips
        n_veh = counter.n_veh
        n_per = counter.n_pers
        #n_rou = counter.n_rou
        n_trip = n_veh+n_per
        demand = trips.parent
        print('RouteAlternativesReader.__init__',demand.ident,'reading',n_trip,'routes')
        net = demand.get_scenario().net
        self._modemap = net.modes.names.get_indexmap()
        self._get_vtype_for_mode = demand.vtypes.get_vtype_for_mode

        self._ids_vtype_sumo = demand.vtypes.ids_sumo
        self._ids_edge_sumo = net.edges.ids_sumo

        self.ids_sumo = np.zeros(n_trip, np.object_)
        self.ids_sumo[:] = ''
        self.ids_vtype = np.zeros(n_trip, np.int32)
        self.times_depart = np.zeros(n_trip, np.int32)
        
        # this is an array of lists (the route  alternatives) of lists (the routes)
        self.routes = np.zeros(n_trip, np.object_)
        
        # this is an array of lists (the route  alternatives) of floats (the route costs)
        self.costs = np.zeros(n_trip, np.object_)
        
        # this is an array of lists (the route  alternatives) of floats (the route probabilities)
        self.probabilities = np.zeros(n_trip, np.object_)
        
        self._ind_trip = -1
        
        # this is a list (the route  alternatives) of lists (the routes)
        self._ids_sumoedges_current = []
        #self._costs_current = []
        #self._probabilities_current = []
        
        self._id_sumoveh_current = None

    def startElement(self, name, attrs):

        #print 'startElement',name
        if name == 'vehicle':
            #print '  startElement',attrs['id'],attrs['depart']
            self._ind_trip += 1

            self._id_sumoveh_current = attrs['id']
            self.ids_sumo[self._ind_trip] = self._id_sumoveh_current
            #print 'startElement ids_vtype',attrs['type'], self._ids_vtype_sumo.get_id_from_index(str(attrs['type']))
            ind = self._ind_trip
            self.ids_vtype[ind] = self.ids_vtype[self._ind_trip] = self._get_id_vtype(attrs)

            self.times_depart[ind] = int(float(attrs['depart']))
            
                
        if name == 'routeDistribution': 
           ind = self._ind_trip
           self.routes[ind] = []
           self.costs[ind]  = []
           self.probabilities[ind]  = []
           #self._ids_sumoedges_current = []
           #self._costs_current = []
           #self._probabilities_current = []


        if name == 'route':
                if (self._id_sumoveh_current is not None):
                    ind = self._ind_trip
                    self.costs[ind].append(float(attrs['cost']))
                    self.probabilities[ind].append(float(attrs['probability']))
                    
                    ids_edge = []
                    for id_sumoedge in attrs.get('edges', '').split(' '):
                        if not id_sumoedge in ('',' ',','):
                            if self._ids_edge_sumo.has_index(id_sumoedge):
                                ids_edge.append(self._ids_edge_sumo.get_id_from_index(id_sumoedge.strip()) )
                    self.routes[ind].append(ids_edge)
                    
                    #self._ids_sumoedges_current.append(attrs.get('edges', ''))
            
        
    def endElement(self, name):

        if name == 'vehicle':
            #print 'endElement',name,self._id_sumoveh_current
            if (self._id_sumoveh_current is not None):
                self._id_sumoveh_current = None
                
    def insert_routes(self, is_fastest, is_add = False):
        # TODO: we can also choose to import all alternatives and probabilities with is_add=True
        n_route = len(self.ids_sumo)
        print('RouteAlternativesReader.insert_routes is_fastest',is_fastest,'n_route',n_route)
        
        routes_chosen  = np.zeros(n_route, np.object_)
        for ind, routes, costs, probs in zip(range(n_route),self.routes, self.costs, self.probabilities):
            if is_fastest:
                ind_route = costs.index(min(costs))
            else:
                ind_route = probs.index(max(probs))
             
            routes_chosen[ind] =  routes[ind_route]        
        
        if 1:

            # get trip ids from xml file
            # ony import routes ids from existing sumo ids
            
            # this is index of self.ids_sumo to be overwritten
            inds = np.flatnonzero(np.array(self._trips.ids_sumo.has_indices(self.ids_sumo)))
            #print '  overwrite trip ids_sumo',self.ids_sumo[inds]
            
            ids_trip =  np.array(self._trips.ids_sumo.get_ids_from_indices(self.ids_sumo[inds]),dtype=np.int32)
            #print '  ids_trip',ids_trip
            #print '   n_trips',len(ids_trip)
            
            ids_routes, ids_trip = self._trips.make_routes( self.ids_vtype[inds],
                                                    #is_generate_ids = is_generate_ids,# depricated
                                                    routes = routes_chosen[inds],
                                                    ids_trip = ids_trip,
                                                    is_add = is_add,
                                                    )
            #print '  ids_routes',ids_routes
            


        return ids_routes, ids_trip
        
        
        
class Routes(am.ArrayObjman):
    def __init__(self, ident, trips, net, **kwargs):

        self._init_objman(  ident = ident,
                            parent = trips,
                            name = 'Routes',
                            info = 'Table with route info.',
                            xmltag = ('routes','route',None),
                            **kwargs)

        self.add_col(am.IdsArrayConf( 'ids_trip', trips,
                                        groupnames = ['state'],
                                        name = 'Trip ID',
                                        info = 'Route for this trip ID.',
                                        ))


        self.add_col(am.IdlistsArrayConf( 'ids_edges', net.edges,
                                        name = 'Edge IDs',
                                        info = 'List of edge IDs constituting the route.',
                                        xmltag = 'edges',
                                        ))

        
        self.add_col(am.ArrayConf(  'costs', 0.0,
                                        dtype=np.float32,
                                        perm='r',
                                        name = 'Costs',
                                        info = "Route costs.",
                                        xmltag = 'cost',# SUMO routing declares times as costs
                                        ))
        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        self.add_col(am.ArrayConf(  'times_est', 0,
                                        dtype=np.int32,
                                        perm='rw',
                                        name = 'Estimated time',
                                        unit = 's',
                                        info = "Estimated trip time of vehicle in seconds. Estimates with maximum edge velocity assumption. Must be an integer!",
                                        xmltag = 'cost',# SUMO routing declares times as costs
                                        ))
                                        
        self.add_col(am.ArrayConf( 'times_exec', 0.0,
                                        dtype = np.float32,
                                        name = 'Exec. time', 
                                        info = 'Execution time from simulation run.',
                                        unit = 's',
                                        ))
                                        
        self.add_col(am.ArrayConf(      'probabilities', 1.0,
                                        dtype=np.float32,
                                        perm='r',
                                        name = 'Probab.',
                                        info = "Route route choice probability.",
                                        xmltag = 'probability',
                                        ))

        self.add_col(am.ArrayConf(     'colors', np.ones(4,np.float32),
                                       dtype=np.float32,
                                       metatype = 'color',
                                       perm = 'rw',
                                       name = 'Color',
                                       info = "Route color. Color as RGBA tuple with values from 0.0 to 1.0",
                                       xmltag = 'color',
                                       ))
        

    def clear_routes(self):
        self.clear()

    def get_shapes(self, ids = None):
        if ids is None:
            ids = self.get_ids()
        n= len(ids)
        edges = self.ids_edges.get_linktab()
        shapes = np.zeros(n, dtype = np.object_)
        i = 0
        # TODO: if edge shapes were a list, the following would be possible:
        # np.sum(shapes)
        for ids_edge in self.ids_edges[ids]:
            routeshape = []
            #print '  ids_edge',ids_edge
            for shape in edges.shapes[ids_edge]:
                #print '  routeshape',routeshape
                #print '  shape',shape,type(shape)
                routeshape += list(shape)
            shapes[i] =  routeshape
            i += 1

        return shapes
    
    def estimate_times(self, ids_route = None):
        if ids_route is None:
            ids_route in self.get_ids()
        
        _get_edgetimes = self.parent.get_net().edges.get_times_cached
        _trips = self.parent
        _vtypes = _trips.parent.vtypes
        ids_mode = _vtypes.ids_mode[_trips.ids_vtype[self.ids_trip[ids_route]]]
        
        for id_route, id_mode, ids_edge in zip(ids_route, ids_mode, self.ids_edges[ids_route]):
            times = _get_edgetimes(id_mode, is_check_lanes = True)
            self.times_est[id_route] = np.sum(times[ids_edge])



class TripsBase(DemandobjMixin, am.ArrayObjman):
    def __init__(self, demand, net=None, **kwargs):
        #print 'Trips.__init__'
        self._init_objman(  ident='trips',
                            parent = demand,
                            name = 'Trips',
                            info = 'Table with trip and route info.',
                            xmltag = ('trips','trip','ids_sumo'),
                            version = 0.2,
                            **kwargs)


        self._init_attributes()
        self._init_constants()

#       TODO: right place for these variables? Is this how you add variables so any Trips instance gets it?
#        self._no_of_taxis = 0
#        self._no_of_SAVs = 0

    def _init_attributes(self):
        scenario = self.get_scenario()
        net =  self.get_net()
        self.add_col(SumoIdsConf('Trip', xmltag = 'id'))


        self.add_col(am.IdsArrayConf( 'ids_vtype', self.get_obj_vtypes(),
                                        groupnames = ['state'],
                                        name = 'Type',
                                        info = 'Vehicle type.',
                                        xmltag = 'type',
                                        ))

        #TODO:
        
        # self.add_col(am.IdsArrayConf( 'taxifleet', self.get_taxi_fleet(),
        
        # JS: does not seem to be used anymore...
        #self.add_col(am.ArrayConf( 'taxifleet', 'none',
        #                                groupnames = ['parameters'],
        #                                name = 'Taxi fleet',
        #                                info = 'Vehicles are assigned to the private Taxi fleet, the SAV (shared autonomous vehicle) fleet of taxis, or none.',
        #                                xmltag = 'line',
        #                                ))
        if hasattr(self,'taxifleet'):
            self.delete('taxifleet')
            
        self.add_col(am.ArrayConf(  'times_depart', 0,
                                        dtype=np.int32,
                                        perm='rw',
                                        name = 'Depart time',
                                        unit = 's',
                                        info = "Departure time of vehicle in seconds. Must be an integer!",
                                        xmltag = 'depart',
                                        ))

        self.add_col(am.ArrayConf(  'times_arrival_est', 0,
                                        dtype=np.int32,
                                        perm='rw',
                                        name = 'Estimated arrival time',
                                        unit = 's',
                                        info = "Estimated arrival time of vehicle in seconds. Estimate assumes that route is known and maximum allowed velocity can be achieved. Must be an integer!",
                                        ))
                                        
        self.add_col(am.IdsArrayConf( 'ids_edge_depart', net.edges,
                                        groupnames = ['state'],
                                        name = 'ID from-edge',
                                        info = 'ID of network edge where trip starts.',
                                        xmltag = 'from',
                                        ))

        self.add_col(am.IdsArrayConf( 'ids_edge_arrival', net.edges,
                                        groupnames = ['state'],
                                        name = 'ID to-edge',
                                        info = 'ID of network edge where trip ends.',
                                        xmltag = 'to',
                                        ))


        self.add_col(am.ArrayConf(  'inds_lane_depart', OPTIONMAP_LANE_DEPART["free"],
                                        dtype=np.int32,
                                        #choices = OPTIONMAP_LANE_DEPART,
                                        perm='r',
                                        name = 'Depart lane',
                                        info = "Departure lane index. 0 is rightmost lane or sidewalk, if existant.",
                                        xmltag = 'departLane',
                                        xmlmap = get_inversemap(OPTIONMAP_LANE_DEPART),
                                        ))


        self.add_col(am.ArrayConf(  'positions_depart', OPTIONMAP_POS_DEPARTURE["random_free"],
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_POS_DEPARTURE,
                                        perm='r',
                                        name = 'Depart pos',
                                        unit = 'm',
                                        info = "Position on edge at the moment of departure.",
                                        xmltag = 'departPos',
                                        xmlmap = get_inversemap(OPTIONMAP_POS_DEPARTURE),
                                        ))

        self.add_col(am.ArrayConf(  'speeds_depart', 0.0,
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_SPEED_DEPARTURE,
                                        perm='r',
                                        name = 'Depart speed',
                                        unit = 'm/s',
                                        info = "Speed at the moment of departure.",
                                        xmltag = 'departSpeed',
                                        xmlmap = get_inversemap(OPTIONMAP_SPEED_DEPARTURE),
                                        ))
        self.add_col(am.ArrayConf(  'inds_lane_arrival', OPTIONMAP_LANE_ARRIVAL["current"],
                                        dtype=np.int32,
                                        #choices = OPTIONMAP_LANE_ARRIVAL,
                                        perm='r',
                                        name = 'Arrival lane',
                                        info = "Arrival lane index. 0 is rightmost lane or sidewalk, if existant.",
                                        xmltag = 'arrivalLane',
                                        xmlmap = get_inversemap(OPTIONMAP_LANE_ARRIVAL),
                                        ))

        self.add_col(am.ArrayConf(  'positions_arrival', OPTIONMAP_POS_ARRIVAL["random"],
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_POS_ARRIVAL,
                                        perm='r',
                                        name = 'Arrival pos',
                                        unit = 'm',
                                        info = "Position on edge at the moment of arrival.",
                                        xmltag = 'arrivalPos',
                                        xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                        ))

        self.add_col(am.ArrayConf(  'speeds_arrival', 0.0,
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_SPEED_ARRIVAL,
                                        perm='r',
                                        name = 'Arrival speed',
                                        unit = 'm/s',
                                        info = "Arrival at the moment of departure.",
                                        xmltag = 'arrivalSpeed',
                                        xmlmap = get_inversemap(OPTIONMAP_SPEED_ARRIVAL),
                                        ))
# TODO:        
#        self.add_col(am.ArrayConf( 'no_of_SAVs',  0.0,
#                                        dtype=np.float32,
#                                        name = 'total number of SAVs',
#                                        info = 'total number of SAVs so far.',
#                                        ))
#
#        self.add_col(am.ArrayConf( 'no_of_taxis',  0.0,
#                                        dtype=np.float32,
#                                        name = 'total number of taxis',
#                                        info = 'total number of taxis so far.',
#                                        ))

        
                                        
        self.add( cm.ObjConf( Routes('routes', self, net)))

        # this could be extended to a list with more plans
        self.add_col(am.IdsArrayConf( 'ids_route_current', self.routes.get_value(),
                                        name = 'Route ID ',
                                        info = 'Currently chosen route ID.',
                                        ))

        #print '  self.routes.get_value()',self.routes.get_value()
        self.add_col(am.IdlistsArrayConf( 'ids_routes', self.routes.get_value(),
                                        name = 'IDs route alt.',
                                        info = 'IDs of route alternatives for this trip.',
                                        ))

        if 1:#self.get_version()<0.2:
            #self.inds_lane_depart.set_xmltag(None)
            #self.inds_lane_arrival.set_xmltag(None)
            self.inds_lane_depart.set_xmltag('departLane', xmlmap = get_inversemap(OPTIONMAP_LANE_DEPART))
            self.inds_lane_arrival.set_xmltag('arrivalLane', xmlmap = get_inversemap(OPTIONMAP_LANE_ARRIVAL))
            self.times_arrival_est.xmltag = None # correction from an older error
            
        self.set_version(0.2)

    def _init_constants(self):
        #self._method_routechoice = self.get_route_first




        self._xmltag_routes = "routes"
        self._xmltag_veh = "vehicle"
        self._xmltag_id = "id"
        self._xmltag_trip = "trip"
        self._xmltag_rou = "route"
        self._xmltag_person = 'person'

    def clear_trips(self):
        self.routes.get_value().clear_routes()
        self.clear()

    def clear_routes(self):
        self.routes.get_value().clear_routes()
        self.ids_route_current.reset()
        self.ids_routes.reset()

    def clear_route_alternatves(self):
        print ('clear_route_alternatves')
        ids_route_del = []
        ids_trip = self.get_ids()
        #ids_routes = self.ids_routes[self.get_ids()]
        for id_trip, id_route_current in zip(ids_trip,self.ids_route_current[ids_trip]):
            if self.ids_routes[id_trip] is not None:
                ids_route = set(self.ids_routes[id_trip])
                ids_route_del += list(ids_route.difference([id_route_current]))
                self.ids_routes[id_trip] = [id_route_current]
            
      
        self.routes.get_value().del_rows(ids_route_del)
        
    def get_id_from_id_sumo(self, id_veh_sumo):
        #print 'get_id_from_id_sumo',id_veh_sumo,len(id_veh_sumo.split('.')) == 1
        if len(id_veh_sumo.split('.')) == 1:
#TODO:
            return int(id_veh_sumo)
            #return id_veh_sumo
        return -1

    def get_routes(self):
        return self.routes.get_value()

    def get_obj_vtypes(self):
        return self.parent.vtypes

#TODO: taxi fleet getter
    def get_taxi_fleet(self):
        print(("fleet assignment = " + str(self.parent.vtypes.taxi_fleet_assignment)))
        return self.parent.vtypes.taxi_fleet_assignment

    def get_net(self):
        return self.get_scenario().net

    def get_scenario(self):
        return self.parent.get_scenario()

    def get_time_depart_first(self):
        if len(self)>0:
            return float(np.min(self.times_depart.get_value()))
        else:
            return np.inf

    def get_time_depart_last(self):
        if len(self)>0:
            return float(np.max(self.times_depart.get_value()))+600.0
        else:
            return 0.0

    def get_tripfilepath(self):
        return self.parent.get_tripfilepath()



    def get_routefilepath(self):
        return self.parent.get_routefilepath()

    
    def estimate_routedurations(self, ids_trip=None):
        if ids_trip is None:
            ids_trip = self.get_ids()
        self.get_routes().estimate_times(ids_route = self.ids_route_current[ids_trip])
        
    
    def estimate_times_arrival(self):
        """Estimate arrival times from route travel times"""
        # filter trips with valid current routes
        ids_trip = self.get_ids()
        ids_trip = ids_trip[self.ids_route_current[ids_trip] > -1]
        
        routes = self.get_routes()
        
        # add route time estimate to departure times
        self.times_arrival_est[ids_trip] = self.times_depart[ids_trip] + routes.times_est[self.ids_route_current[ids_trip]]
        
    
    def duaroute(self, is_export_net = False, is_export_trips = True,
                routefilepath = None,  weights = None, weightfilepath = None, 
                **kwargs):
            """
            Simple fastest path routing using duarouter.
            """
            print('duaroute')
            exectime_start = time.perf_counter()
            
            #routesattrname = self.get_routesattrname(routesindex)
            vtypes = self.parent.vtypes
            if (not os.path.isfile(self.get_tripfilepath()))|is_export_trips:
                ids_vtype_pedestrian = vtypes.select_by_mode(mode = 'pedestrian', is_sumoid = False)
                self.export_trips_xml(ids_vtype_exclude = ids_vtype_pedestrian)

            if (not os.path.isfile(self.get_net().get_filepath()))|is_export_net:
                self.get_net().export_netxml()


            if routefilepath is None:
                    routefilepath = self.get_routefilepath()

            if weights is not None:
                weightfilepath = self.get_net().edges.export_edgeweights_xml(
                                    filepath = weightfilepath,
                                    weights = weights, 
                                    time_begin = self.get_time_depart_first(), 
                                    time_end = self.get_time_depart_last())
                
            if routing.duaroute(  self.get_tripfilepath(), self.get_net().get_filepath(),
                            routefilepath, weightfilepath = weightfilepath, **kwargs):

                self.import_routes_xml(routefilepath, 
                                            is_clear_trips = False,
                                            is_generate_ids = False,
                                            is_overwrite_only = True,
                                            is_add = False)

                print('  exectime',time.perf_counter()-exectime_start)
                self.estimate_routedurations()
                self.estimate_times_arrival()
        
                return routefilepath

            else:
                return None



    def get_trips_for_vtype(self, id_vtype):
        return self.select_ids(self.ids_vtype.get_value() ==id_vtype)

    def get_vtypes(self):
        return set(self.ids_vtype.get_value())

    def route(self, is_check_lanes = True, is_del_disconnected = False, is_set_current = False):
        """
        Fastest path python router.
        """
        print('route is_check_lanes',is_check_lanes)
        # TODO: if too mant vtypes, better go through id_modes
        exectime_start = time.perf_counter()

        net = self.get_scenario().net
        edges = net.edges
        vtypes = self.parent.vtypes
        edges.clear_cache()
        ids_edges = []
        ids_trip = []
        costs = []

        ids_trip_disconnected = []

        
        for id_vtype in self.get_vtypes():
            id_mode = vtypes.ids_mode[id_vtype]

            # no routing for pedestrians
            if id_mode != net.modes.get_id_mode('pedestrian'):
                ids_trip_vtype = self.get_trips_for_vtype(id_vtype)
                #print '  id_vtype, id_mode',id_vtype,id_mode#,ids_trip_vtype
                
                weights = edges.get_times_cached(  id_mode = id_mode,
                                            speed_max = vtypes.speeds_max[id_vtype],
                                            is_check_lanes = is_check_lanes,
                                            modeconst_excl = -10.0, modeconst_mix = -5.0,
                                            )
                
                fstar = edges.get_fstar_cached(id_mode = id_mode)
                
                
                ids_alledges = edges.get_ids()
                #for id_edge,id_edge_sumo, weight in zip(ids_alledges,edges.ids_sumo[ids_alledges],weights[ids_alledges]):
                #    print '  id_edge',id_edge,id_edge_sumo,'weight',weights[id_edge_sumo]
                ids_edge_depart = self.ids_edge_depart[ids_trip_vtype]
                ids_edge_arrival = self.ids_edge_arrival[ids_trip_vtype]

                for id_trip,id_edge_depart,id_edge_arrival in zip(ids_trip_vtype, ids_edge_depart,  ids_edge_arrival):
                    
                    cost, route = routing.get_mincostroute_edge2edge(   id_edge_depart,
                                                                        id_edge_arrival,
                                                                        weights= weights,
                                                                        fstar = fstar)
                    
                    #if id_trip == 1:
                    #    print '    id_trip',id_trip,'id_edge_depart',id_edge_depart,'id_edge_arrival',id_edge_arrival
                    #    print '       route',route
                    #    print '       ids_sumo',edges.ids_sumo[route]
                    if len(route)>0:
                        ids_edges.append(route)
                        ids_trip.append(id_trip)
                        costs.append(cost)

                    else:
                        ids_trip_disconnected.append(id_trip)

        ids_route = self.routes.get_value().add_rows(   ids_trip = ids_trip,
                                                        ids_edges = ids_edges,
                                                        costs = costs,
                                                        )
        if is_set_current:
            self.ids_route_current[ids_trip] = ids_route
        else:
            self.add_routes(ids_trip, ids_route)
        
        print('  exectime',time.perf_counter()-exectime_start)

        if is_del_disconnected:
            print('  del disconnected routes',ids_trip_disconnected)
            self.del_rows(ids_trip_disconnected)
        
        self.estimate_routedurations(ids_trip)
        self.estimate_times_arrival()
        return ids_trip, ids_route


    def estimate_entered(self, method_routechoice = None, is_ignore_first_edge = False):
        """
        Estimates the entered number of vehicles for each edge.
        returns ids_edge and entered_vec
        """
        print('estimate_entered from routes',len(self),'is_ignore_first_edge',is_ignore_first_edge)
        # TODO: we could specify a mode
        if method_routechoice is None:
            method_routechoice = self.get_route_current # self.get_route_first

        ids_edges = self.routes.get_value().ids_edges
        counts = np.zeros(np.max(self.get_net().edges.get_ids())+1,int)
        #print '  n',np.max(self.get_net().edges.get_ids())+1,len(counts)
        for id_trip in self.get_ids():
            
            id_route = method_routechoice(id_trip)
            #print '    id_trip',id_trip,'id_route',id_route
            if id_route >-1:
                if is_ignore_first_edge:
                    # here the [1:] eliminates first edge as it is not entered
                    counts[ids_edges[id_route][1:]] +=1
                else:
                    counts[ids_edges[id_route]] +=1

        ids_edge = np.flatnonzero(counts)
        entered_vec = counts[ids_edge].copy()
        return ids_edge, entered_vec



    def import_trips_from_scenario(self, scenario2):
        """
        Import trips from another scenario.
        """
        print('import_trips_from_scenario',scenario2.ident)
        if not is_pyproj:
            print ("WARNING in import_trips_from_scenario: pyproj module not installed")
            return None

        scenario = self.get_scenario()
        demand = scenario.demand
        net = scenario.net
        edges = net.edges
        ids_edge_depart = demand.trips.ids_edge_depart
        ids_edge_arrival = demand.trips.ids_edge_arrival

        demand2 = scenario2.demand
        net2 = scenario2.net

        # copy all vtypes from scenario2

        # get vtypes of demand2 that are not in demand
        ids_veh2 = demand2.vtypes.get_ids()
        ids_vtype_sumo2 = demand2.vtypes.ids_sumo[ids_veh2]
        #print '  ids_vtype_sumo2',ids_vtype_sumo2
        #print '  ids_vtype_sumo',demand.vtypes.ids_sumo.get_value()
        ids_vtype_sumo_diff =  list(set(ids_vtype_sumo2).difference(demand.vtypes.ids_sumo.get_value()))
        #print '  ids_vtype_sumo_diff',ids_vtype_sumo_diff
        #for id_sumo2 in demand2.vtypes.ids_sumo.get_value():
        #    if demand.vtypes.has_index(id_sumo2):
        #       for attrconf in demand2.vtypes.get_group('parameters'):

        # copy all attributes from   ids_vtype_sumo_diff
        ids_vtype_diff = demand.vtypes.copy_cols(demand2.vtypes, ids = demand2.vtypes.ids_sumo.get_ids_from_indices(ids_vtype_sumo_diff))
        #ids_vtype_sumo = demand.vtypes.ids_sumo.get_value()
        #ids_vtype = demand.vtypes.get_ids_from_indices(ids_vtype_sumo)
        #ids_vtype2 = demand2.vtypes.ids_sumoget_ids_from_indices(ids_vtype_sumo)


        # map id_vtypes from scenario2 to present scenario
        vtypemap = np.zeros(np.max(ids_veh2)+1)
        vtypemap[ids_veh2] =  demand.vtypes.ids_sumo.get_ids_from_indices(ids_vtype_sumo2)

        #print '  vtypemap',vtypemap
        # copy trip parameters, by mapping trip types
        ids_trip2 = demand2.trips.get_ids()

        ids_trips = self.copy_cols(demand2.trips)# ids_trip in present scenrio


        # delete routes, that cannot be transferred (at the moment)
        self.ids_route_current[ids_trips] = -1
        self.ids_routes[ids_trips] = len(ids_trips) * [None]

        self.ids_vtype[ids_trips] = vtypemap[demand2.trips.ids_vtype[ids_trip2]]

        #print '  ids_trip2,ids_trips',ids_trip2,ids_trips

        ids_mode = demand.vtypes.ids_mode[self.ids_vtype[ids_trips]]
        # for each used mode, we need to select the network edges that are accessible by this mode
        ids_modeset = set(ids_mode)

        #maps_edge_laneind = {}
        #for id_mode in ids_modeset:
        #    ids_edge, inds_lane = edges.select_accessible(id_mode)
        #    maps_edge_laneind[id_mode] = dict(zip(ids_edge,inds_lane))



        # project depart points and arrival points
        proj_params = str(net.get_projparams())
        proj_params2 = str(net2.get_projparams())

        if (proj_params == '!')|(proj_params2 == '!'):
            print('WARNING in import_trips_from_scenario: unknown projections, use only offsets.',proj_params,proj_params2)
            is_proj = False

        elif proj_params == proj_params2:
            #  with identical projections, projecting is useless
            is_proj = False

        else:
            is_proj = True
            proj = pyproj.Proj(proj_params)
            proj2 = pyproj.Proj(proj_params2)

        offset = net.get_offset()
        offset2 = net2.get_offset()

        #if self._proj is None:
        #    self._proj, self._offset = self.parent.get_proj_and_offset()
        #x,y = self._proj(lons, lats)
        #return np.transpose(np.concatenate(([x+self._offset[0]],[y+self._offset[1]]),axis=0))


        # adjust edge ids check lane access
        n_failed = 0
        ids_trip_failed = set()

        for id_mode in ids_modeset:
            # make_segment_edge_map for all edges of this mode
            print('  make segment_edge_map for mode',id_mode)
            ids_edge_access, inds_lane_access = edges.select_accessible_mode(id_mode)
            print('    found accessible edges',len(ids_edge_access),len(edges.get_ids()))
            # dict(zip(ids_edge,inds_lane))
            edges.make_segment_edge_map(ids_edge_access)

            # select trips with id_mode
            ind_trips = np.flatnonzero(ids_mode == id_mode)

            print('  number of trips for this mode:', len(ind_trips))
            for id_trip,id_edge_depart2, id_edge_arrival2 in zip(  ids_trips[ind_trips],
                                                                demand2.trips.ids_edge_depart[ids_trip2[ind_trips]],
                                                                demand2.trips.ids_edge_arrival[ids_trip2[ind_trips]]
                                                                ):

                # match departure edge

                # treat special numbers of position
                pos2 = 0.0

                # get coordinate in scenario2
                x2,y2,z2 = net2.edges.get_coord_from_pos(id_edge_depart2, pos2)

                # project coord from scenario2 in present scenario
                if is_proj:
                    xp,yp = pyproj.transform(proj2,proj, x2-offset2[0], y2-offset2[1])
                else:
                    xp,yp = x2-offset2[0], y2-offset2[1]

                coord  = (xp+offset[0], yp+offset[1],z2)
                #print '  coord2 = ',(x2,y2,z2)
                #print '  coord  = ',coord
                # get edge id in present scenario
                id_edge_depart = edges.get_closest_edge(coord)


                # check eucledian distance
                #d = edges.get_dist_point_to_edge(coord, id_edge_depart)
                #print '    id_edge_depart,d,id_mode',id_edge_depart,d,id_mode
                print('    id_edge_depart',id_edge_depart,id_edge_depart in ids_edge_access)

                ids_edge_depart[id_trip] = id_edge_depart

                # match arrival edge

                # treat special numbers of position
                pos2 = 0.0

                # get coordinate in scenario2
                x2,y2,z2 = net2.edges.get_coord_from_pos(id_edge_arrival2, pos2)

                # project coord from scenario2 in present scenario
                if is_proj:
                    xp,yp = pyproj.transform(proj2,proj, x2-offset2[0], y2-offset2[1])
                else:
                    xp,yp = x2-offset2[0], y2-offset2[1]

                coord  = (xp+offset[0], yp+offset[1],z2)
                #print '  coord2 = ',(x2,y2,z2)
                #print '  coord  = ',coord
                # get edge id in present scenario
                id_edge_arrival = edges.get_closest_edge(coord)


                # check eucledian distance
                #d = edges.get_dist_point_to_edge(coord, id_edge_arrival)
                print('    id_edge_arrival',id_edge_arrival,id_edge_arrival in ids_edge_access)

                ids_edge_arrival[id_trip] = id_edge_arrival

        # redo segment map
        edges.make_segment_edge_map()



    def make_trip(self, is_generate_ids = True,  **kwargs):

        id_trip = self.add_row(   ids_vtype = kwargs.get('id_vtype',None),
                    times_depart = kwargs.get('time_depart',None),
                    ids_edge_depart = kwargs.get('id_edge_depart',None),
                    ids_edge_arrival = kwargs.get('id_edge_arrival',None),
                    inds_lane_depart = kwargs.get('ind_lane_depart',None),
                    positions_depart = kwargs.get('position_depart',None),
                    speeds_depart = kwargs.get('speed_depart',None),
                    inds_lane_arrival = kwargs.get('ind_lane_arrival',None),
                    positions_arrival = kwargs.get('position_arrival',None),
                    speeds_arrival = kwargs.get('speed_arrival',None),
                    ids_routes = [],
                    taxifleet = kwargs.get('line',None),
                    )

        if is_generate_ids:
            self.ids_sumo[id_trip] = str(id_trip)
        else:
            self.ids_sumo[id_trip] = kwargs.get('id_sumo',str(id_trip)) # id

        if 'route' in kwargs:
            route = kwargs['route']
            if len(route)>0:
                id_route = self.routes.get_value().add_row( ids_trip = id_trip,
                                                            ids_edges = kwargs['route']
                                                            )
                self.ids_route_current[id_trip]= id_route
                self.ids_routes[id_trip]= [id_route]

        return id_trip

    def make_trips(self, ids_vtype, is_generate_ids = True, **kwargs):
        print('make_trips len(ids_vtype) =',len(ids_vtype))
        #print '  kwargs=',kwargs
        ids_trip = self.add_rows(n = len(ids_vtype),
                        ids_vtype = ids_vtype,
                        times_depart = kwargs.get('times_depart',None),
                        ids_edge_depart = kwargs.get('ids_edge_depart',None),
                        ids_edge_arrival = kwargs.get('ids_edge_arrival',None),
                        inds_lane_depart = kwargs.get('inds_lane_depart',None),
                        positions_depart = kwargs.get('positions_depart',None),
                        speeds_depart = kwargs.get('speeds_depart',None),
                        inds_lane_arrival = kwargs.get('inds_lane_arrival',None),
                        positions_arrival = kwargs.get('positions_arrival',None),
                        speeds_arrival = kwargs.get('speeds_arrival',None),
                        #ids_routes = len(ids_vtype)*[[]],
                    )

        if is_generate_ids:
            self.ids_sumo[ids_trip] = ids_trip.astype(str)
        else:
            self.ids_sumo[ids_trip] = kwargs.get('ids_sumo',ids_trip.astype(str))
        return ids_trip

    def add_returntrips(self, timedelay, ids_trip = None):
        """Create return trips for ids_trip or for all trips if ids_trip = None.
        The timedelay is added to the departure time of all return trips.
        """
        if ids_trip is None:
            ids_trip = self.get_ids()
        
        return self.make_trips( ids_vtype = self.ids_vtype[ids_trip], is_generate_ids = True, 
                                times_depart = self.times_depart[ids_trip] + timedelay,
                                ids_edge_depart = self.ids_edge_arrival[ids_trip],# return!
                                ids_edge_arrival = self.ids_edge_depart[ids_trip],# return!
                                inds_lane_depart = self.inds_lane_depart[ids_trip],
                                positions_depart = self.positions_depart[ids_trip],
                                speeds_depart = self.speeds_depart[ids_trip],
                                inds_lane_arrival = self.inds_lane_arrival[ids_trip],
                                positions_arrival = self.positions_arrival[ids_trip],
                                speeds_arrival = self.speeds_arrival[ids_trip],
                               )
    
    def scale_trips(self, scale, time_delta):
        """Scale current trips by scale factor greater than one."""
        ids_trip = self.get_ids()
        n_trips = len(self)
        
        n_trips_required = int(n_trips*scale) - n_trips
        
        ids_source = ids_trip[np.random.randint(n_trips, size = n_trips_required)]
        
        return self.make_trips( ids_vtype = self.ids_vtype[ids_source], is_generate_ids = True, 
                                times_depart = self.times_depart[ids_source] + np.random.uniform(low=-time_delta, high=time_delta, size=n_trips_required),
                                ids_edge_depart = self.ids_edge_depart[ids_source],
                                ids_edge_arrival = self.ids_edge_arrival[ids_source],
                                inds_lane_depart = self.inds_lane_depart[ids_source],
                                positions_depart = self.positions_depart[ids_source],
                                speeds_depart = self.speeds_depart[ids_source],
                                inds_lane_arrival = self.inds_lane_arrival[ids_source],
                                positions_arrival = self.positions_arrival[ids_source],
                                speeds_arrival = self.speeds_arrival[ids_source],
                               )
                               
    def set_routes_sumo(self, ids_veh_sumo, ids_edges_sumo):
        """
        Sets already existing routes.
        Keys are an array with SUMO IDs of the vehicles.
        Routes are represented as an array of lists of SUMO edge IDs
        """
        print('Trips.set_routes_sumo ',len(ids_veh_sumo))
        routes = self.get_routes()
        get_ids_from_indices = self.get_scenario().net.edges.ids_sumo.get_ids_from_indices
        
        edges = self.get_scenario().net.edges
        n_rerouted = 0
        for id_veh_sumo, ids_edge_sumo in zip(ids_veh_sumo, ids_edges_sumo):
            id_veh = self.get_id_from_id_sumo(id_veh_sumo)
            if id_veh != -1:
                ids_edge_old = routes.ids_edges[self.ids_route_current[id_veh]]
                ids_edge_new = get_ids_from_indices(ids_edge_sumo)
                is_reroute = ids_edge_old != ids_edge_new
                n_rerouted += is_reroute
                if is_reroute:
                    print('  rerouted id_veh',id_veh,'starting at',self.times_depart[id_veh])
                    #print('  set route id_veh',id_veh,'id_route',self.ids_route_current[id_veh],'before',routes.ids_edges[self.ids_route_current[id_veh]]) 
                    routes.ids_edges[self.ids_route_current[id_veh]] = get_ids_from_indices(ids_edge_sumo)
                    #print('      after',routes.ids_edges[self.ids_route_current[id_veh]],edges.ids_sumo[routes.ids_edges[self.ids_route_current[id_veh]]]) 
                else:
                    print('  maintain id_veh',id_veh,'starting at',self.times_depart[id_veh])
                    #print '         route',edges.ids_sumo[ids_edge_new]
    
    def make_routes(self, ids_vtype, is_generate_ids = True, routes = None, ids_trip=None, is_add = True, **kwargs):
        """Generates or sets routes of trips, generates also trips if necessary
        ids_trip: trip IDs, 
                    if None then trip ID and route ID will be generated for each given route
                    if a list then routes will be associated with these trip IDs and routes will be replaced
                    but generated if route ID does not exist for given trip ID 
        is_add: if True then routes are added to the alternative route list
                if False then current routes will be set
        
        is_generate_ids: depricated, fully controlled by ids_trip
        """
        is_generate_ids = ids_trip is None
        print('make_routes is_generate_ids',is_generate_ids,'is_add',is_add)
        #print '  ids_trip',ids_trip
        
        
        
        if ids_trip is None: #is_generate_ids = is_generate_ids,
            print('  generate new trip IDs')
            ids_trip=self.make_trips(ids_vtype, is_generate_ids =is_generate_ids,  **kwargs)
            is_generate_ids = True
        else:
            if not is_add:
                print('  replace current route and create if not existent')
                ids_routes = self.ids_route_current[ids_trip]
                inds_new = np.flatnonzero(ids_routes == -1)
                #print '  inds_new',inds_new
                if len(inds_new)>0:
                    print('  complete %d non pre-existant route ids of %d trips'%(len(inds_new),len(ids_trip)))
                    # create new routes
                    ids_routes[inds_new] = self.routes.get_value().add_rows(  n = len(inds_new),
                                                                    ids_trip = ids_trip[inds_new],
                                                                    #ids_edges = routes[inds_new],
                                                                )
                else:
                    print('  all new routes have pre-existing routes')    
            else:
                # make new route IDs
                ids_routes = self.routes.get_value().add_rows(  n = len(ids_trip),
                                                                    ids_trip = ids_trip,
                                                                    #ids_edges = routes[inds_new],# later!!
                                                                )
                                                                
            is_generate_ids = False
            print('  set new routes to routes database',len(ids_routes),'routes set')
            self.routes.get_value().ids_edges[ids_routes] = routes
            
            if not is_add:
                print('  replace current route IDs',len(inds_new),'routes replaced')
                self.ids_route_current[ids_trip[inds_new]] = ids_routes[inds_new]
            else:
                print('  add new route IDs to alternatives',len(ids_trip),'routes added')
                self.add_routes(ids_trip, ids_routes)
            
            #if np.any(ids_routes == -1):
            #    is_generate_ids = True
        
        #print '  ids_trip =',ids_trip
        if is_generate_ids:
            print('  generate new route IDs')
            ids_routes = self.routes.get_value().add_rows(  n = len(ids_trip),
                                                            ids_trip = ids_trip,
                                                            #ids_edges = routes,
                                                        )
            self.routes.get_value().ids_edges[ids_routes] = routes
            
            #print '    ids_routes',ids_routes
            if not is_add:
                print('  set new current routes')
                self.ids_route_current[ids_trip] = ids_routes
            else:
                print('  add new route IDs to alternatives')
                self.add_routes(ids_trip, ids_routes)
            
        
            
            
        

        #no!:self.ids_routes[ids_trip] = ids_routes.reshape((-1,1)).tolist()# this makes an array of lists
        #print '  self.ids_routes.get_value()',self.ids_routes[ids_trip]
        #print '  ids_routes.reshape((-1,1)).tolist()',ids_routes.reshape((-1,1)).tolist()
        #print '  make_routes DONE'
        return ids_routes,ids_trip

    def add_routes(self, ids_trip, ids_routes):
        for id_trip, id_route in zip(ids_trip,ids_routes):
            # no!: self.ids_routes[id_trip].append(id_route)
            #print '  self.ids_routes[id_trip]',self.ids_routes[id_trip],id_route
            if self.ids_routes[id_trip] is None:
                self.ids_routes[id_trip] = [id_route] # this works!

            else:
                self.ids_routes[id_trip].append(id_route)

            #if self.ids_route_current[id_trip] == -1:
            #    self.ids_route_current[id_trip] = id_route

    def prepare_sim(self, process):
        return [] # [(steptime1,func1),(steptime2,func2),...]

    def export_trips_xml(self, filepath=None, encoding = 'UTF-8',
                            ids_vtype_exclude = [],ids_vtype_include = [],
                            vtypeattrs_excluded = []):
        """
        Export trips to SUMO xml file.
        Method takes care of sorting trips by departure time.
        """
        if filepath is None:
            filepath = self.get_tripfilepath()
        print('export_trips_xml',filepath)
        try:
            fd=open(filepath,'w', encoding="utf-8")
        except:
            print('WARNING in write_obj_to_xml: could not open',filepath)
            return False

        xmltag, xmltag_item, attrname_id = self.xmltag
        fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
        fd.write(xm.begin(xmltag))
        indent = 2

        ids_trip = self.times_depart.get_ids_sorted()
        ids_vtype = self.ids_vtype[ids_trip]
        #ids_vtypes_exclude = self.ids_vtype.get_ids_from_indices(vtypes_exclude)

        inds_selected = np.ones(len(ids_vtype), np.bool_)
        for id_vtype in ids_vtype_exclude:
            inds_selected[ids_vtype == id_vtype] = False
        ids_trip_selected = ids_trip[inds_selected]
        ids_vtype_selected =  set(ids_vtype[inds_selected])
        ids_vtype_selected.union(ids_vtype_include)
        #ids_vtypes_selected = set(ids_vtypes).difference(ids_vtypes_exclude)
        
        attrconfigs_excluded = []
        for attrname in vtypeattrs_excluded:
            attrconfigs_excluded.append(self.parent.vtypes.get_config(attrname))
            
            
        
        self.parent.vtypes.write_xml(   fd, indent=indent,
                                        ids = ids_vtype_selected,
                                        is_print_begin_end = False,
                                        attrconfigs_excluded = attrconfigs_excluded)


        self.write_xml( fd,     indent=indent,
                                ids = ids_trip_selected,
                                attrconfigs_excluded = [self.routes,
                                                        self.ids_routes,
                                                        self.ids_route_current,
                                                        #self.inds_lane_depart,
                                                        #self.inds_lane_arrival
                                                        ],
                                is_print_begin_end = False)

        fd.write(xm.end(xmltag))
        fd.close()
        return filepath


    def get_vtypes(self):
        return set(self.ids_vtype.get_value())

    def get_trips(self):
        # returns trip object, method common to all demand objects
        return self

    def get_writexmlinfo(self, is_route=False, is_exclude_pedestrians = False, **kwargs):
        """
        Returns three array where the first array is the
        begin time of the first vehicle and the second array is the
        write function to be called for the respectice vehicle and
        the third array contains the vehicle ids

        Method used to sort trips when exporting to route or trip xml file
        """
        
        ids = self.get_ids()
        
        if not is_exclude_pedestrians:
            # define different route write functions for pedestriand and vehicles
            n = len(ids)
            writefuncs = np.zeros(n, dtype = np.object_)
            inds_ped = self.parent.vtypes.ids_mode[self.ids_vtype[ids]] == MODES['pedestrian']
            writefuncs[inds_ped] = self.write_persontrip_xml
            if is_route:
                writefuncs[np.logical_not(inds_ped)& (self.ids_route_current[ids]>-1)] = self.write_vehroute_xml
                
                # vehicles must have a route, this makes sure that OD are connected
                writefuncs[np.logical_not(inds_ped)& (self.ids_route_current[ids]==-1)] = self.write_missingroute_xml
            else:
                # here we write vehicle trip, without explicit route export
                # routing will be performed during simulation
                writefuncs[np.logical_not(inds_ped)& (self.ids_route_current[ids]>-1)] = self.write_vehtrip_xml
                
                # vehicles must have a route, this makes sure that OD are connected
                writefuncs[np.logical_not(inds_ped)& (self.ids_route_current[ids]==-1)] = self.write_missingroute_xml
        else:
            # only  vehicle types without peds
            inds_noped = self.parent.vtypes.ids_mode[self.ids_vtype[ids]] != MODES['pedestrian']
            ids = ids[inds_noped]
            n = len(ids)
            writefuncs = np.zeros(n, dtype = np.object_)
            if is_route:
                writefuncs[ self.ids_route_current[ids]>-1] = self.write_vehroute_xml
                
                # vehicles must have a route, this makes sure that OD are connected
                writefuncs[self.ids_route_current[ids]==-1] = self.write_missingroute_xml
            else:
                # here we write vehicle trip, without explicit route export
                # routing will be performed during simulation
                writefuncs[self.ids_route_current[ids]>-1] = self.write_vehtrip_xml
                
                # vehicles must have a route, this makes sure that OD are connected
                writefuncs[self.ids_route_current[ids]==-1] = self.write_missingroute_xml
            
        return self.times_depart[ids], writefuncs, ids

    def write_missingroute_xml(self, fd, id_trip, time_begin, indent = 2):
        """
        Function called when respective vehicle has an invalid route
        """
        pass

    def write_vehroute_xml(self, fd, id_trip, time_begin, indent = 2):
        #print 'write_vehroute_xml',id_trip,time_begin
        id_route = self.ids_route_current[id_trip]# self.get_route_first(id_trip)#self._method_routechoice(id_trip)#


        if id_route>=0:# a valid route has been found
            # init vehicle route only if valid route exists
            fd.write(xm.start(self._xmltag_veh,indent))
        else:
            # init trip instead of route
            fd.write(xm.start(self._xmltag_trip,indent))

        #print '   make tag and id',_id
        fd.write(xm.num(self._xmltag_id, self.ids_sumo[id_trip]))

        #print ' write columns',len(scalarcolconfigs)>0,len(idcolconfig_include_tab)>0,len(objcolconfigs)>0
        #if self.taxifleet == 'taxi:fleetPrivate' or self.taxifleet =='default_line_fleet': #'taxi:fleetSAV':
        for attrconfig in [self.ids_vtype,
                                self.times_depart,
                                self.ids_edge_depart,
                                self.ids_edge_arrival,
                                self.inds_lane_depart,
                                self.positions_depart,
                                self.speeds_depart,
                                self.inds_lane_arrival,
                                self.positions_arrival,
                                self.speeds_arrival,
                                ]:
            #print '    attrconfig',attrconfig.attrname
            attrconfig.write_xml(fd,id_trip)


        if (id_route>=0):# a valid route has been found
            # write route id
            #fd.write(xm.num('route', id_route ))

            # instead of route id we write entire route here
            fd.write(xm.stop())
            fd.write(xm.start(self._xmltag_rou,indent+2))

            routes = self.routes.get_value()
            for attrconfig in [routes.ids_edges, routes.colors ]:
                #print '    attrconfig',attrconfig.attrname
                attrconfig.write_xml(fd,id_route)

            # end route and vehicle
            fd.write(xm.stopit())
            fd.write(xm.end(self._xmltag_veh,indent+2))

        else:
            # end trip without route
            fd.write(xm.stopit())

    
    def write_vehtrip_xml(self, fd, id_trip, time_begin, indent = 2):
        # vehicle trip write function
        # no route is written, even if it exisis

        # init trip instead of route
        fd.write(xm.start(self._xmltag_trip,indent))

        #print '   make tag and id',_id
        fd.write(xm.num(self._xmltag_id, self.ids_sumo[id_trip]))

        #print ' write columns',len(scalarcolconfigs)>0,len(idcolconfig_include_tab)>0,len(objcolconfigs)>0
        for attrconfig in [self.ids_vtype,
                                self.times_depart,
                                self.ids_edge_depart,
                                self.ids_edge_arrival,
                                self.inds_lane_depart,
                                self.positions_depart,
                                self.speeds_depart,
                                self.inds_lane_arrival,
                                self.positions_arrival,
                                self.speeds_arrival,]:
            #print '    attrconfig',attrconfig.attrname
            attrconfig.write_xml(fd,id_trip)
        
        # end trip without route
        fd.write(xm.stopit())
        
    def write_persontrip_xml(self, fd, id_trip, time_begin, indent = 2):
        # currently no routes are exported, only origin and destination edges

        fd.write(xm.start(self._xmltag_person, indent))

        self.ids_sumo.write_xml(fd,id_trip)
        self.times_depart.write_xml(fd,id_trip)
        self.ids_vtype.write_xml(fd,id_trip)
        fd.write(xm.stop())

        fd.write(xm.start('walk',indent=indent+2))
        #print 'write walk',id_trip,self.positions_depart[id_trip],self.positions_arrival[id_trip]
        self.ids_edge_depart.write_xml(fd,id_trip)
        if self.positions_depart[id_trip]>0:
            self.positions_depart.write_xml(fd,id_trip)

        self.ids_edge_arrival.write_xml(fd,id_trip)
        if self.positions_arrival[id_trip]>0:
            self.positions_arrival.write_xml(fd,id_trip)

        fd.write(xm.stopit())# ends walk
        fd.write(xm.end(self._xmltag_person, indent=indent))
    
    
    def get_route_current(self, id_trip):
        return self.ids_route_current[id_trip]
        
        
    def get_route_first(self, id_trip):
    
        
        ids_route = self.ids_routes[id_trip]
        if ids_route is None:
            return -1
        elif len(ids_route)>0:
            return ids_route[0]
        else:
            return -1 # no route found


    def import_routealternatives_xml(self, filepath=None, demandobjects = None, 
                                            is_fastest = True, is_add = False,
                                            ):
        """Imports alternative routes from SUMO rou xml file
        demandobjects: list of demand objects for which routes are read.
                       If None, routes are read into all demand objects 
        is_fastest: If true import fastest route and replace current route.
        """
        print('import_routes_xml from %s add trips %s'%(filepath,is_add))
        
        counter = RouteCounter()
        parse(filepath, counter)
        reader = RouteAlternativesReader(self, counter)
        if 1:
            parse(filepath, reader)
            #print '  call insert_routes  is_add',is_add
            reader.insert_routes(is_fastest =is_fastest, is_add = is_add)
            
        #except KeyError:
        #    print >> sys.stderr, "Error: Problems with reading routes!"
        #    raise
    
    def import_routes_xml(self, filepath,  is_clear_trips = False,
                            is_generate_ids = True, is_add = False,
                            is_overwrite_only = False):
        """
        Imports routes from SUMO rou xml file
        
        is_clear_trips: If True, clear all trips
        is_generate_ids: If True, generate new trip IDs
        is_add: Add routes to route alternatives of existing trips
        is_overwrite_only: overwrite current route of existing trips
        """
        print('import_routes_xml from %s generate new routes %s, clear trips %s add trips %s'%(filepath,is_generate_ids,is_clear_trips,is_add))
        if is_clear_trips:
            self.clear_trips()

        counter = RouteCounter()
        parse(filepath, counter)
        reader = RouteReader(self, counter)
        try:
            parse(filepath, reader)
            print('  call insert_routes is_generate_ids',is_generate_ids, 'is_add',is_add,'is_overwrite_only',is_overwrite_only)
            reader.insert_routes(is_generate_ids = is_generate_ids,
                                    is_add=is_add, is_overwrite_only = is_overwrite_only)
        except KeyError:
            print("Error: Problems with reading routes!", file=sys.stderr)
            raise

    def import_trips_xml(self, filepath,  is_clear_trips = False, is_generate_ids = True):
        print('import_trips_xml from %s generate own trip '%(filepath))
        if is_clear_trips:
            self.clear_trips()

        counter = TripCounter()
        parse(filepath, counter)
        reader = TripReader(self, counter.n_trip)
        print('  n_trip=',counter.n_trip)

        try:
            parse(filepath, reader)
            reader.insert_trips(is_generate_ids = is_generate_ids)
        except KeyError:
            print("Error: Problems with reading trips!", file=sys.stderr)
            raise

    
