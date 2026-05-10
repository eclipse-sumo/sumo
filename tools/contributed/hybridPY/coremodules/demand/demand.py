import os, sys
import time


if  __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except Exception:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    hybridPYDIR = os.path.join(APPDIR,'..','..')
    sys.path.append(hybridPYDIR)


# Trip depart and arrival options, see
# http://www.sumo.dlr.de/userdoc/Definition_of_Vehicles,_Vehicle_Types,_and_Routes.html#A_Vehicle.27s_depart_and_arrival_parameter


from coremodules.modules_common import *
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import get_inversemap
from agilepy.lib_base.processes import Process

#from agilepy.lib_base.geometry import find_area
#from agilepy.lib_base.processes import Process,CmlMixin,ff,call
from coremodules.network.network import SumoIdsConf,MODES
from coremodules.network import routing
from coremodules.simulation import results as res
from .demandbase import *
from . import vehicles,origin_to_destination,virtualpop,turnflows, detectorflows
from . import publictransportservices as pt



try:
    try:
        import pyproj
        is_pyproj = True
    except Exception:
        from mpl_toolkits.basemap import pyproj
        is_pyproj = True

except Exception:
    is_pyproj = False

class Trips(TripsBase):
    def config_results(self, results):
        #print 'DEMAND.config_results'
        tripresults = res.Tripresults(          'tripresults', results,
                                                self,
                                                self.get_net().edges
                                                )


        results.add_resultobj(tripresults, groupnames = ['Trip results'])
        
        

    def process_results(self, results, process = None):
        pass

class Demand(cm.BaseObjman):
        def __init__(self, scenario=None, net = None, zones = None, name = 'Demand', info ='Transport demand', **kwargs):
            #print 'Demand.__init__',name,kwargs

            # we need a network from somewhere
            if net is None:
                net = scenario.net
                zones = scenario.landuse.zones

            self._init_objman(ident= 'demand', parent=scenario, name = name, info = info, **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))

            self.vtypes = attrsman.add(    cm.ObjConf( vehicles.VehicleTypes(self, net) ))

            self.activitytypes = attrsman.add( cm.ObjConf(ActivityTypes('activitytypes',self))  )

            self.trips = attrsman.add(     cm.ObjConf( Trips(self,net), groupnames = ['demand objects']))






            self.odintervals = attrsman.add( cm.ObjConf(\
                                            origin_to_destination.OdIntervals('odintervals',self, net, zones),
                                            ))

            self.turnflows = attrsman.add( cm.ObjConf(  turnflows.Turnflows('turnflows',self, net),
                                                        ))
            self._init_attributes()
            self._init_constants()


        def _init_attributes(self):
            attrsman = self.get_attrsman()

            scenario = self.parent
            #print 'Demand._init_attributes',scenario

            if scenario  is not None:
                self.detectorflows = attrsman.add( cm.ObjConf(  detectorflows.Detectorflows('detectorflows',self),
                                                        ))
                                                        
                self.ptlines = attrsman.add( cm.ObjConf( pt.PtLines('ptlines',self),
                                                            groupnames = ['demand objects'] )
                                                )
                self.virtualpop = attrsman.add( cm.ObjConf( virtualpop.Virtualpopulation('virtualpop',self),
                                                            groupnames = ['demand objects'] )
                                                )


        def _init_constants(self):
            self._xmltag_routes = "routes"
            self._xmltag_trips = "trips"

        def get_vtypes(self):
            return self.vtypes

        def get_scenario(self):
            return self.parent

        def get_net(self):
            return self.parent.net

        def get_tripfilepath(self):
            return self.get_scenario().get_rootfilepath()+'.trip.xml'

        def get_routefilepath(self):
            return self.get_scenario().get_rootfilepath()+'.rou.xml'

        def update_netoffset(self, deltaoffset):
            """
            Called when network offset has changed.
            Children may need to adjust their coordinates.
            """
            #self.odintervals.update_netoffset(deltaoffset)
            pass

        def add_demandobject(self, obj = None, ident = None, DemandClass = None, **kwargs):
            if obj is not None:
                ident = obj.get_ident()

            if not hasattr(self,ident):
                if obj is  None:
                    # make demandobject a child of demand

                    #if ident is None:
                    #    ident = obj.get_ident()
                    obj = DemandClass(ident, self, **kwargs)
                    is_child = True
                    #is_save = True
                else:
                    # link to simobject, which must be a child of another object
                    is_child = False # will not be saved but linked
                    #is_save = False



                attrsman = self.get_attrsman()

                attrsman.add( cm.ObjConf(   obj,
                                            groupnames = ['demand objects'],
                                            is_child = is_child,
                                            #is_save = is_save,
                                            ) )

                setattr(self,ident,obj)



            return getattr(self, ident)

        def get_demandobjects(self):
            #demandobjects = set([])
            #for ident, conf in self.get_group_attrs('').items():
            #    demandobjects.add(conf.get_value())
            demandobjects_clean = []
            for attrname, demandobject in self.get_attrsman().get_group_attrs('demand objects').items():
                if demandobject is not None:
                    demandobjects_clean.append(demandobject)
                else:
                    print('WARNING in get_demandobjects: found None as object',attrname)
                    self.get_attrsman().delete(attrname)
            return demandobjects_clean

        def get_time_depart_first(self):
            # print 'get_time_depart_first'
            time = 10**10
            for obj in self.get_demandobjects():
                #print '  obj',obj.ident,obj.get_time_depart_first()
                time = min(time,obj.get_time_depart_first())
            return time

        def get_time_depart_last(self):
            time = 0
            for obj in self.get_demandobjects():
                time = max(time,obj.get_time_depart_last())
            return time


        def remove_demandobject(self, demandobject):
            #self._demandobjects.discard(demandobject)
            self.get_attrsman().delete(demandobject.ident)

        def import_routealternatives_xml(self, filepath=None, demandobjects = None, 
                                            **kwargs
                                            ):
            """Imports alternative routes from SUMO rou xml file
            demandobjects: list of demand objects for which routes are read.
                           If None, routes are read into all demand objects 
            is_fastest: If true import fastest route and replace current route.
            """
            if demandobjects is None:
                demandobjects = self.get_demandobjects()
                
            try:
                fd = open(filepath,'r', encoding="utf-8")
                fd.close()
            except Exception:
                print('WARNING in import_routes_xml: could not open',filepath)
                return False
                
            for demandobj in demandobjects:
                print('   try to import alt routes from demandobj',demandobj)
                demandobj.import_routealternatives_xml(filepath, **kwargs)
                                                    
            return True                                        
            
        def import_routes_xml(self, filepath=None, demandobjects = None, 
                                is_clear_trips = False, is_generate_ids = False, 
                                is_overwrite_only = True):
            """Imports routes from SUMO rou xml file
            demandobjects: list of demand objects for which routes are read.
                           If None, routes are read into all demand objects 
            is_clear_trips: If True, clear all trips
            is_generate_ids: If True, generate new trip IDs
            is_overwrite_only: overwrite current route of existing trips
            """
            if demandobjects is None:
                demandobjects = self.get_demandobjects()

            #is_route = True # add edge ids, if available

            if filepath is None:
                filepath = self.get_routefilepath()
            print('import_routes_xml',filepath,demandobjects)
            try:
                fd = open(filepath,'r', encoding="utf-8")
                fd.close()
            except Exception:
                print('WARNING in import_routes_xml: could not open',filepath)
                return False
            
            for demandobj in demandobjects:
                print('   try to import routes from demandobj',demandobj)
                demandobj.import_routes_xml(filepath,   
                                                    is_clear_trips = is_clear_trips,
                                                    is_generate_ids = is_generate_ids,
                                                    is_overwrite_only = is_overwrite_only)
                                                    
            return True                                        
        
        def set_routes_sumo(self, ids_veh_sumo, ids_edges_sumo, demandobjects = None):
            if demandobjects is None:
                demandobjects = self.get_demandobjects()
            for demandobj in demandobjects:
                print('  set route for demandobj',demandobj)    
                demandobj.set_routes_sumo(ids_veh_sumo, ids_edges_sumo)
                                                        
        def export_routes_xml(  self, filepath=None, encoding = 'UTF-8',
                                demandobjects = None, is_route = True,
                                vtypeattrs_excluded = [],
                                is_plain = False,
                                is_exclude_pedestrians = False,):
            """
            Export routes available from the demand  to SUMO xml file.
            Method takes care of sorting trips by departure time.
            """

            if demandobjects is None:
                demandobjects = self.get_demandobjects()

            #is_route = True # add edge ids, if available

            if filepath is None:
                filepath = self.get_routefilepath()
            print('export_routes_xml',filepath,demandobjects)
            try:
                fd = open(filepath,'w', encoding="utf-8")
            except Exception:
                print('WARNING in export_routes_xml: could not open',filepath)
                return False



            fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
            fd.write(xm.begin(self._xmltag_routes))
            indent = 2


            times_begin = np.zeros((0),dtype = np.int32)
            writefuncs = np.zeros((0),dtype = np.object_)
            ids_trip = []# use list here to accomodate different id stuctures
            #ids_trip =np.zeros((0),dtype = np.int32)

            ids_vtype = set()
            for exportobj in demandobjects:
                print('  exportobj',exportobj)
                times, funcs, ids = exportobj.get_writexmlinfo( is_route=is_route, 
                                                                is_plain = is_plain,
                                                                is_exclude_pedestrians = is_exclude_pedestrians)
                print('    n_trips',len(times),'has vtypes',hasattr(exportobj,'get_vtypes'),'total trips',len(times_begin)+len(times))
                if len(times)>0:
                    times_begin = np.concatenate((times_begin, times),0)
                    writefuncs = np.concatenate((writefuncs, funcs),0)
                    #ids_trip = np.concatenate((ids_trip, ids),0)
                    ids_trip = ids_trip + list(ids)
                if hasattr(exportobj,'get_vtypes'):
                    # TODO:all export objects have get_vtypes except mapmatching
                    ids_vtype.update(exportobj.get_vtypes())
            
            # convert back to array to allow proper indexing
            ids_trip = np.array(ids_trip, dtype = np.object_)
            
            attrconfigs_excluded = []
            for attrname in vtypeattrs_excluded:
                attrconfigs_excluded.append(self.vtypes.get_config(attrname))
                
            self.vtypes.write_xml(   fd, indent=indent,
                                        ids = ids_vtype,
                                        is_print_begin_end = False,
                                        attrconfigs_excluded = attrconfigs_excluded,
                                        )

            # sort trips
            inds_trip = np.argsort(times_begin)

            #time0 = times_begin[inds_trip[0]]
            print ('  write trips',len(inds_trip),'to file')
            for writefunc, id_trip, time_begin in zip(\
                                            writefuncs[inds_trip],
                                            ids_trip[inds_trip],
                                            times_begin[inds_trip]):

                writefunc(fd,id_trip, time_begin,indent)

            fd.write(xm.end(self._xmltag_routes))
            fd.close()
            return filepath

        def get_ids_vtype(self):
            ids_vtype = set()
            for exportobj in demandobjects:
                if hasattr(exportobj,'get_vtypes'):
                    # TODO:all export objects have get_vtypes except mapmatching
                    ids_vtype.update(exportobj.get_vtypes())
            return list(ids_vtype)
        
        def get_ids_mode(self):
            return list(set(self.get_vtypes().ids_mode[self.get_ids_vtype()]))
            
        def import_xml(self, rootname, dirname=''):
            """
            Import trips and/or routes, if available.
            """

            filepath = os.path.join(dirname,rootname+'.trip.xml')
            if os.path.isfile(filepath):
                # import trips
                self.trips.import_trips_xml(filepath, is_generate_ids = False)

                # now try to add routes to existing trips
                filepath = os.path.join(dirname,rootname+'.rou.xml')
                if os.path.isfile(filepath):
                    self.trips.import_routes_xml(filepath, is_generate_ids = False, is_add = True)

                else:
                    self.get_logger().w('import_xml: files not found:'+filepath, key='message')


            else:
                self.get_logger().w('import_xml: files not found:'+filepath, key='message')


                # no trip file exists, but maybe just a route file with trips
                filepath = os.path.join(dirname,rootname+'.rou.xml')
                if os.path.isfile(filepath):
                    self.trips.import_routes_xml(filepath, is_generate_ids = False, is_add = False)

                else:
                    self.get_logger().w('import_xml: files not found:'+filepath, key='message')






class TaxiGenerator(Process):
    def __init__(self, demand, logger = None,**kwargs):
        
        self._init_common(  'taxigenerator', name = 'Taxi generator', 
                            parent = demand,
                            logger = logger,
                            info ='Generates taxi trips on specific zones.',
                            )
   
        
        attrsman = self.get_attrsman()
        scenario = self.parent.get_scenario()
        zones = scenario.landuse.zones
        
        
        self.n_taxi = attrsman.add(cm.AttrConf(  'n_taxi', 100,
                                    groupnames = ['options'], 
                                    name = 'Number of taxi',
                                    info = "Number of taxis to be generated.",
                                    )) 
        
        self.priority_max = attrsman.add(cm.AttrConf( 'priority_max', 8,
                                                groupnames = ['options'],
                                                name = 'Max. edge priority',
                                                perm = 'rw',
                                                info = "Maximum edge priority for which edges in a zone are considered departure or arrival edges.",
                                                ))
        
        self.speed_max = attrsman.add(cm.AttrConf( 'speed_max', 14.0,
                                                groupnames = ['options'],
                                                name = 'Max. edge speed',
                                                perm = 'rw',
                                                unit = 'm/s',
                                                info = "Maximum edge speed for which edges in a zone are considered departure or arrival edges.",
                                                ))
        
        
        time_start = self.parent.get_time_depart_first()
        


        self.time_start = attrsman.add(cm.AttrConf( 'time_start', kwargs.get('time_start',time_start),
                                                groupnames = ['options','timing'],
                                                name = 'Start time',
                                                perm = 'rw',
                                                info = 'Start time when first taxi appears, in seconds after midnight.',
                                                unit = 's',
                                                ))

        # default is to insert all taxis within the first 60s
        self.time_end = attrsman.add(cm.AttrConf( 'time_end', kwargs.get('time_end',time_start+60.0),
                                                groupnames = ['options','timing'],
                                                name = 'End time',
                                                perm = 'rw',
                                                info = 'Time when last taxi appears in seconds after midnight.',
                                                unit = 's',
                                                ))
                                                 
        #self.n_edges_min_length = attrsman.add(cm.AttrConf( 'n_edges_min_length', 1,
        #                                        groupnames = ['options'],
        #                                        name = 'Min. edge number length prob.',
        #                                        perm = 'rw',
        #                                        info = "Minimum number of edges for with the departure/arrival probability is proportional to the edge length.",
        #                                        ))
        
        #self.n_edges_max_length = attrsman.add(cm.AttrConf( 'n_edges_max_length', 500,
        #                                        groupnames = ['options'],
        #                                        name = 'Max. edge number length prob.',
        #                                        perm = 'rw',
        #                                        info = "Maximum number of edges for with the departure/arrival probability is proportional to the edge length.",
        #                                        ))
                                                
                                
        self.is_selected_zones = attrsman.add(cm.AttrConf(  'is_selected_zones', False,
                                    groupnames = ['options'], 
                                    name = 'Selected zones',
                                    info = "Place taxis only on edges of specified zone list.",
                                    )) 
        
        ids_zone = zones.get_ids()
        zonechoices = {}
        for id_zone, name_zone in zip(ids_zone, zones.ids_sumo[ids_zone]):
              zonechoices[name_zone] = id_zone
        #print '  zonechoices',zonechoices
        #make for each possible pattern a field for prob
        #if len(zonechoices) > 0:
        self.ids_zone = attrsman.add(cm.ListConf('ids_zone',[], 
                                                  groupnames = ['options'], 
                                                  choices = zonechoices,
                                                  name = 'Zones', 
                                                  info = """Zones where to place taxis. Taxis are distributed proportional to road lengths in zones.""",
                                                  ))   
        
        
        #self.is_refresh_zoneedges = attrsman.add(am.AttrConf(  'is_refresh_zoneedges', True,
        #                            groupnames = ['options'],
        #                            perm='rw', 
        #                            name = 'Refresh zone edges', 
        #                            info = """Identify all edges in all zones before generating the trips. 
        #                                      Dependent on the  will take some time.""",
        #                            ))
        
    #def _get_edgeweights(self, ids_edge, n_edges_min_length, n_edges_max_length, priority_max, speed_max):
    #    #print 'get_edgeweights ids_edge',ids_edge
    #    edges = self.get_edges()
    #    n_edges = len(ids_edge)
    #    if (n_edges > n_edges_min_length)&(n_edges < n_edges_max_length):
    #        return edges.lengths[ids_edge]*((edges.priorities[ids_edge]<priority_max) & (edges.speeds_max[ids_edge] < speed_max))
    #    else:
    #        return np.ones(n_edges,dtype = np.float32)*((edges.priorities[ids_edge]<priority_max) & (edges.speeds_max[ids_edge] < speed_max))
    
    
    def generate_taxi(self):
        """
        Generate taxis as trips in the trip database. 
        """
        tripnumber = self.n_taxi
        time_start = self.time_start
        time_end= self.time_end
        id_mode_ped = MODES['pedestrian']
        id_mode_taxi = MODES['taxi']
        scenario = self.parent.get_scenario()
        zones = scenario.landuse.zones
        trips = scenario.demand.trips
        edges = scenario.net.edges
        edgelengths = edges.lengths
        
        # define taxi and secondary mode, if appropriate
        ids_vtype_mode_taxi, prob_vtype_mode_taxi = scenario.demand.vtypes.select_by_mode(
            id_mode_taxi, is_share = True)
        
        #print '  ids_vtype_mode', ids_vtype_mode
        n_vtypes_taxi = len(ids_vtype_mode_taxi)
        
        if self.is_selected_zones:
            ids_zone = self.ids_zone
        else:
            ids_zone = zones.get_ids()
     
               
        fstar = edges.get_fstar(is_ignor_connections=False)
        times_taxi = edges.get_times(id_mode=id_mode_taxi, is_check_lanes=True)
  
        n_trips_generated = 0
        n_trips_failed = 0

        if self.is_selected_zones:
            # tale selected zones
            ids_zone = self.ids_zone
        else:
            # take all zones
            ids_zone = zones.get_ids()
        
        ids_edges_orig = [] #all accessible edges in all zones
        n_edges_orig = 0
        for id_zone in ids_zone:
            #id_orig = self.ids_orig[id_od]
            #id_dest = self.ids_dest[id_od]
            
            print('  check id_zone',id_zone)
            ids_edges_orig_raw = zones.ids_edges_inside[id_zone]
            
            #prob_edges_orig_raw = zones.probs_edges_orig[id_orig]
            
            # check accessibility of origin edges
            
            #prob_edges_orig = []
            #inds_lane_orig = []
            
            for i in range(len(ids_edges_orig_raw)):
                id_edge = ids_edges_orig_raw[i]
                # if check accessibility...
                ind_lane_depart_taxi = edges.get_laneindex_allowed(id_edge, id_mode_taxi)
                ind_lane_depart_ped = edges.get_laneindex_allowed(id_edge, id_mode_ped)
                
                
                #print '    O get_laneindex_allowed id_mode_taxi',id_mode_taxi,id_edge,edges.ids_sumo[id_edge],'ind_lane',ind_lane_depart
                if (ind_lane_depart_taxi >= 0)&(ind_lane_depart_ped >= 0):
                    ids_edges_orig.append(id_edge)
                    #prob_edges_orig.append(prob_edges_orig_raw[i])
                    #are_fallback_orig.append(False)
                    #inds_lane_orig.append(ind_lane_depart)
                   

                                    
            n_edges_orig = len(ids_edges_orig)
            
            print('\n    found',n_edges_orig,'valid zone edges')
        
        # update edge probabilities with suitable parameters
        # edge departure probabilities of all edges in all zones
        edgeprops =  edges.lengths[ids_edges_orig]*((edges.priorities[ids_edges_orig]<self.priority_max) & (edges.speeds_max[ids_edges_orig] < self.speed_max))

    
        # now create taxi trips    
        if (n_edges_orig > 0) & (tripnumber > 0):
            # normalize weights
            edgeprops = edgeprops/np.sum(edgeprops)
            
            # debug
            if 0:
                for id_edge, prob in zip(ids_edges_orig,edgeprops):
                    print('      orig id_edge',id_edge,'has prob',prob)
                
                        

            for d in range(int(tripnumber+0.5)):
                #print '      ------------'
                #print '      generte trip',d
                time_depart = np.random.uniform(time_start, time_end)
                
                i_orig = np.argmax(np.random.rand(n_edges_orig)*edgeprops)
                id_edge_orig = ids_edges_orig[i_orig]
                
                ## destination edge is origin edge
                # this is no problem as taxis will never leave the sim
                id_edge_dest = id_edge_orig
                route = [id_edge_orig] 
                
                ## Destination is next edge
                #
                #is_accessible = False
                #for id_edge_dest in fstar[id_edge_orig]:
                #     # if check accessibility...
                #    ind_lane_depart_taxi = edges.get_laneindex_allowed(id_edge_dest, id_mode_taxi)
                #    ind_lane_depart_ped = edges.get_laneindex_allowed(id_edge_dest, id_mode_ped)
                #    if (ind_lane_depart_taxi >= 0)&(ind_lane_depart_ped >= 0):
                #        is_accessible = True
                #        break
                #if not is_accessible:
                #    id_edge_dest = id_edge_orig
                #    route = [id_edge_orig]
                #else:
                #    route = [id_edge_orig,id_edge_dest]
                
                
                  
                id_vtype = ids_vtype_mode_taxi[0]
                
                # trip is from beginning to end of edge
                # however, taxi will not be eliminated at the 
                # end of edge but continue to next client
                id_trip = trips.make_trip(id_vtype=id_vtype,
                                              time_depart = time_depart,
                                              id_edge_depart = id_edge_orig,
                                              id_edge_arrival = id_edge_dest,
                                              ind_lane_depart = -5,#"first": 
                                              ind_lane_arrival = -1,#"current",
                                              position_depart = -4, #"base",
                                              position_arrival = -2, #"max",
                                              speed_depart = 0.0,
                                              speed_arrival = 0.0,
                                              route = route,
                                              line = 'taxi:fleetPrivate'
                                              )
                    
                n_trips_generated += 1
                
            print('  n_trips_generated', n_trips_generated, 'of',self.n_taxi)
            return True
            
        else:
            print('  no taxi created n_edges_orig',n_edges_orig,'tripnumber',tripnumber)
            return False
        
        
    def do(self):
        
        return self.generate_taxi()


class SAVGenerator(Process):
    def __init__(self, demand, logger = None,**kwargs):
        
        self._init_common(  'SAVgenerator', name = 'SAV generator', 
                            parent = demand,
                            logger = logger,
                            info ='Generates SAV trips on specific zones.',
                            )
   
        
        attrsman = self.get_attrsman()
        scenario = self.parent.get_scenario()
        zones = scenario.landuse.zones
        
        
        self.n_SAV = attrsman.add(cm.AttrConf(  'n_SAV', 100,
                                    groupnames = ['options'], 
                                    name = 'Number of SAV',
                                    info = "Number of SAVs to be generated.",
                                    )) 
        
        self.priority_max = attrsman.add(cm.AttrConf( 'priority_max', 8,
                                                groupnames = ['options'],
                                                name = 'Max. edge priority',
                                                perm = 'rw',
                                                info = "Maximum edge priority for which edges in a zone are considered departure or arrival edges.",
                                                ))
        
        self.speed_max = attrsman.add(cm.AttrConf( 'speed_max', 14.0,
                                                groupnames = ['options'],
                                                name = 'Max. edge speed',
                                                perm = 'rw',
                                                unit = 'm/s',
                                                info = "Maximum edge speed for which edges in a zone are considered departure or arrival edges.",
                                                ))
        
        
        time_start = self.parent.get_time_depart_first()
        


        self.time_start = attrsman.add(cm.AttrConf( 'time_start', kwargs.get('time_start',time_start),
                                                groupnames = ['options','timing'],
                                                name = 'Start time',
                                                perm = 'rw',
                                                info = 'Start time when first SAV appears, in seconds after midnight.',
                                                unit = 's',
                                                ))

        # default is to insert all SAVs within the first 60s
        self.time_end = attrsman.add(cm.AttrConf( 'time_end', kwargs.get('time_end',time_start+60.0),
                                                groupnames = ['options','timing'],
                                                name = 'End time',
                                                perm = 'rw',
                                                info = 'Time when last SAV appears in seconds after midnight.',
                                                unit = 's',
                                                ))
                                                 
        #self.n_edges_min_length = attrsman.add(cm.AttrConf( 'n_edges_min_length', 1,
        #                                        groupnames = ['options'],
        #                                        name = 'Min. edge number length prob.',
        #                                        perm = 'rw',
        #                                        info = "Minimum number of edges for with the departure/arrival probability is proportional to the edge length.",
        #                                        ))
        
        #self.n_edges_max_length = attrsman.add(cm.AttrConf( 'n_edges_max_length', 500,
        #                                        groupnames = ['options'],
        #                                        name = 'Max. edge number length prob.',
        #                                        perm = 'rw',
        #                                        info = "Maximum number of edges for with the departure/arrival probability is proportional to the edge length.",
        #                                        ))
                                                
                                
        self.is_selected_zones = attrsman.add(cm.AttrConf(  'is_selected_zones', False,
                                    groupnames = ['options'], 
                                    name = 'Selected zones',
                                    info = "Place SAVs only on edges of specified zone list.",
                                    )) 
        
        ids_zone = zones.get_ids()
        zonechoices = {}
        for id_zone, name_zone in zip(ids_zone, zones.ids_sumo[ids_zone]):
              zonechoices[name_zone] = id_zone
        #print '  zonechoices',zonechoices
        #make for each possible pattern a field for prob
        #if len(zonechoices) > 0:
        self.ids_zone = attrsman.add(cm.ListConf('ids_zone',[], 
                                                  groupnames = ['options'], 
                                                  choices = zonechoices,
                                                  name = 'Zones', 
                                                  info = """Zones where to place SAVs. SAVs are distributed proportional to road lengths in zones.""",
                                                  ))   
        
        
        #self.is_refresh_zoneedges = attrsman.add(am.AttrConf(  'is_refresh_zoneedges', True,
        #                            groupnames = ['options'],
        #                            perm='rw', 
        #                            name = 'Refresh zone edges', 
        #                            info = """Identify all edges in all zones before generating the trips. 
        #                                      Dependent on the  will take some time.""",
        #                            ))
        
    #def _get_edgeweights(self, ids_edge, n_edges_min_length, n_edges_max_length, priority_max, speed_max):
    #    #print 'get_edgeweights ids_edge',ids_edge
    #    edges = self.get_edges()
    #    n_edges = len(ids_edge)
    #    if (n_edges > n_edges_min_length)&(n_edges < n_edges_max_length):
    #        return edges.lengths[ids_edge]*((edges.priorities[ids_edge]<priority_max) & (edges.speeds_max[ids_edge] < speed_max))
    #    else:
    #        return np.ones(n_edges,dtype = np.float32)*((edges.priorities[ids_edge]<priority_max) & (edges.speeds_max[ids_edge] < speed_max))
    
    
    def generate_SAV(self):
        """
        Generate SAVs as trips in the trip database. 
        """
        tripnumber = self.n_SAV
        time_start = self.time_start
        time_end= self.time_end
        id_mode_ped = MODES['pedestrian']
        id_mode_taxi = MODES['taxi']
        scenario = self.parent.get_scenario()
        zones = scenario.landuse.zones
        trips = scenario.demand.trips
        edges = scenario.net.edges
        edgelengths = edges.lengths
        
        # define SAV and secondary mode, if appropriate
        ids_vtype_mode_taxi, prob_vtype_mode_taxi = scenario.demand.vtypes.select_by_mode(
            id_mode_taxi, is_share = True)
        
        #print '  ids_vtype_mode', ids_vtype_mode
        n_vtypes_taxi = len(ids_vtype_mode_taxi)
        
        if self.is_selected_zones:
            ids_zone = self.ids_zone
        else:
            ids_zone = zones.get_ids()
     
               
        fstar = edges.get_fstar(is_ignor_connections=False)
        times_taxi = edges.get_times(id_mode=id_mode_taxi, is_check_lanes=True)
  
        n_trips_generated = 0
        n_trips_failed = 0

        if self.is_selected_zones:
            # tale selected zones
            ids_zone = self.ids_zone
        else:
            # take all zones
            ids_zone = zones.get_ids()
        
        ids_edges_orig = [] #all accessible edges in all zones
        n_edges_orig = 0
        for id_zone in ids_zone:
            #id_orig = self.ids_orig[id_od]
            #id_dest = self.ids_dest[id_od]
            
            print('  check id_zone',id_zone)
            ids_edges_orig_raw = zones.ids_edges_inside[id_zone]
            
            #prob_edges_orig_raw = zones.probs_edges_orig[id_orig]
            
            # check accessibility of origin edges
            
            #prob_edges_orig = []
            #inds_lane_orig = []
            
            for i in range(len(ids_edges_orig_raw)):
                id_edge = ids_edges_orig_raw[i]
                # if check accessibility...
                ind_lane_depart_taxi = edges.get_laneindex_allowed(id_edge, id_mode_taxi)
                ind_lane_depart_ped = edges.get_laneindex_allowed(id_edge, id_mode_ped)
                
                
                #print '    O get_laneindex_allowed id_mode_taxi',id_mode_taxi,id_edge,edges.ids_sumo[id_edge],'ind_lane',ind_lane_depart
                if (ind_lane_depart_taxi >= 0)&(ind_lane_depart_ped >= 0):
                    ids_edges_orig.append(id_edge)
                    #prob_edges_orig.append(prob_edges_orig_raw[i])
                    #are_fallback_orig.append(False)
                    #inds_lane_orig.append(ind_lane_depart)
                   

                                    
            n_edges_orig = len(ids_edges_orig)
            
            print('\n    found',n_edges_orig,'valid zone edges')
        
        # update edge probabilities with suitable parameters
        # edge departure probabilities of all edges in all zones
        edgeprops =  edges.lengths[ids_edges_orig]*((edges.priorities[ids_edges_orig]<self.priority_max) & (edges.speeds_max[ids_edges_orig] < self.speed_max))

    
        # now create taxi trips    
        if (n_edges_orig > 0) & (tripnumber > 0):
            # normalize weights
            edgeprops = edgeprops/np.sum(edgeprops)
            
            # debug
            if 0:
                for id_edge, prob in zip(ids_edges_orig,edgeprops):
                    print('      orig id_edge',id_edge,'has prob',prob)
                
                        

            for d in range(int(tripnumber+0.5)):
                #print '      ------------'
                #print '      generte trip',d
                time_depart = np.random.uniform(time_start, time_end)
                
                i_orig = np.argmax(np.random.rand(n_edges_orig)*edgeprops)
                id_edge_orig = ids_edges_orig[i_orig]
                
                ## destination edge is origin edge
                # this is no problem as taxis will never leave the sim
                id_edge_dest = id_edge_orig
                route = [id_edge_orig] 
                
                ## Destination is next edge
                #
                #is_accessible = False
                #for id_edge_dest in fstar[id_edge_orig]:
                #     # if check accessibility...
                #    ind_lane_depart_taxi = edges.get_laneindex_allowed(id_edge_dest, id_mode_taxi)
                #    ind_lane_depart_ped = edges.get_laneindex_allowed(id_edge_dest, id_mode_ped)
                #    if (ind_lane_depart_taxi >= 0)&(ind_lane_depart_ped >= 0):
                #        is_accessible = True
                #        break
                #if not is_accessible:
                #    id_edge_dest = id_edge_orig
                #    route = [id_edge_orig]
                #else:
                #    route = [id_edge_orig,id_edge_dest]
                
                
                  
                id_vtype = ids_vtype_mode_taxi[1]
                #id_vtype = self.ids_sumo.get_id_from_index('SAV')
                
                # trip is from beginning to end of edge
                # however, taxi will not be eliminated at the 
                # end of edge but continue to next client

                ###
#                str(self.parent.vtypes.taxi_fleet_assignment())
                ###
                id_trip = trips.make_trip(id_vtype=id_vtype,
                                              time_depart = time_depart,
                                              id_edge_depart = id_edge_orig,
                                              id_edge_arrival = id_edge_dest,
                                              ind_lane_depart = -5,#"first": 
                                              ind_lane_arrival = -1,#"current",
                                              position_depart = -4, #"base",
                                              position_arrival = -2, #"max",
                                              speed_depart = 0.0,
                                              speed_arrival = 0.0,
                                              route = route,
                                              line = 'taxi:fleetSAV'
                                              )
                    
                n_trips_generated += 1
                
            print('  n_trips_generated', n_trips_generated, 'of',self.n_SAV)
            return True
            
        else:
            print('  no SAV created n_edges_orig',n_edges_orig,'tripnumber',tripnumber)
            return False
        
        
    def do(self):
        
        return self.generate_SAV()
        
class Tripscaler(Process):
    def __init__(self, demand, logger = None,**kwargs):
        
        self._init_common(  'tripscaler', name = 'Trip Scaler', 
                            parent = demand,
                            logger = logger,
                            info ='Scales up number of trips by replicating trips.',
                            )
   
        
        attrsman = self.get_attrsman()
      
        
        
        self.scale = attrsman.add(cm.AttrConf( 'scale', kwargs.get('scale',2.0),
                                                groupnames = ['options'],
                                                name = 'Trip scale',
                                                perm = 'rw',
                                                info = "Scling of trips, must be greater than one, no trips will be deleted.",
                                                ))
        
        self.time_delta_max = attrsman.add(cm.AttrConf( 'time_delta_max', kwargs.get('time_delta_max',14.0),
                                                groupnames = ['options'],
                                                name = 'Max. time spread',
                                                perm = 'rw',
                                                unit = 's',
                                                info = "Maximum absolute random time added to the departure time when doubling existing trips.",
                                                ))
        
        
       
        
 
        
        

    
        
    def do(self):
        self.parent.trips.scale_trips(self.scale, self.time_delta_max)
        return True

if __name__ == '__main__':
    ###############################################################################
    #print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    #from coremodules.scenario import scenario
    from coremodules.network import network
    logger = Logger()
    NETPATH = os.path.join(hybridPYDIR,'coremodules','network','testnet')
    net = network.Network(logger = logger)
    rootname = 'facsp2'
    net.import_xml(rootname, NETPATH)
    #net.read_sumonodes(os.path.join(NETPATH,'facsp2.nod.xml'))
    #net.read_sumoedges(os.path.join(NETPATH,'facsp2.edg.xml'))
    demand = Demand( net = net, logger = logger)
    #demand.set_net(net)
    #landuse.facilities.import_poly(os.path.join(NETPATH,'facsp2.poly.xml'))
    #landuse.import_xml(rootname, NETPATH)
    objbrowser(demand)
