import os, sys
from xml.sax import saxutils, parse, handler
if  __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    SUMOPYDIR = os.path.join(APPDIR,'..','..')
    sys.path.append(SUMOPYDIR)


# Trip depart and arrival options, see
# http://www.sumo.dlr.de/userdoc/Definition_of_Vehicles,_Vehicle_Types,_and_Routes.html#A_Vehicle.27s_depart_and_arrival_parameter

OPTIONMAP_POS_DEPARTURE = { "random":-1,"free":-2,"random_free":-3,"base":-4,"last":-5,"first":-6} 
OPTIONMAP_POS_ARRIVAL = { "random":-1,"max":-2} 
OPTIONMAP_SPEED_DEPARTURE = {"random":-1,"max":-2} 
OPTIONMAP_SPEED_ARRIVAL = {"current":-1}
OPTIONMAP_LANE_DEPART = {"random":-1,"free":-2,"allowed":-3,"best":-4,"first":-5}
OPTIONMAP_LANE_ARRIVAL = {"current":-1}

from coremodules.modules_common import *
import numpy as np            
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlmanager as xm
from agilepy.lib_base.misc import get_inversemap
#from agilepy.lib_base.geometry import find_area
from agilepy.lib_base.processes import Process,CmlMixin,ff,call
from coremodules.network.network import SumoIdsConf

import vehicles,origin_to_destination,virtualpop,turnflows                   
 

def route(tripfilepath, netfilepath, routefilepath, options = '-v --ignore-errors'):
            #  do not use options: --repair --remove-loops
            cmd = 'duarouter '+options+' --trip-files %s --net-file %s --output-file %s'\
                    %(  ff(tripfilepath), ff(netfilepath),ff(routefilepath))
            return call(cmd)

class Router(Process):
    def __init__(self, trips, net, logger = None,**kwargs):
        
        self._init_common(  'router', name = 'Router', 
                            logger = logger,
                            info ='Generates routes from trips.',
                            )
        self._trips = trips
        self._net = net
        
        attrsman = self.get_attrsman()
        self.add_option('netfilepath',netfilepath,
                        groupnames = ['options'],# this will make it show up in the dialog
                        cml = '--sumo-net-file',
                        perm='rw', 
                        name = 'Net file', 
                        wildcards = 'Net XML files (*.net.xml)|*.net.xml',
                        metatype = 'filepath',
                        info = 'SUMO Net file in XML format.',
                        )
        
        self.workdirpath = attrsman.add(cm.AttrConf(  'workdirpath',rootdirpath,
                        groupnames = ['_private'],#['options'],#['_private'], 
                        perm='r', 
                        name = 'Workdir', 
                        metatype = 'dirpath',
                        info = 'Working directory for this scenario.',
                        ))
                        
        self.ident_scenario = attrsman.add(cm.AttrConf(  'ident_scenario',ident_scenario,
                        groupnames = ['_private'], 
                        perm='r', 
                        name = 'Scenario shortname', 
                        info = 'Scenario shortname is also rootname of converted files.',
                        ))
                                        
        
        self.is_clean_nodes = attrsman.add(cm.AttrConf(  'is_clean_nodes',is_clean_nodes,
                        groupnames = ['options'], 
                        perm='rw', 
                        name = 'Clean Nodes', 
                        info = 'If set, then shapes around nodes are cleaned up.',
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
        #if len(bn)>0:
        #    self.ident_scenario = bn[0]
    
    def do(self):
        self.update_params()
        cml = self.get_cml()+' --plain-output-prefix '+filepathlist_to_filepathstring(os.path.join(self.workdirpath, self.ident_scenario))
        #print 'SumonetImporter.do',cml
        #import_xml(self, rootname, dirname, is_clean_nodes = True)
        self.run_cml(cml)
        if self.status == 'success':
             self._net.import_xml(self.ident_scenario, self.workdirpath, is_clean_nodes = self.is_clean_nodes)
            
        #print 'do',self.newident
        #self._scenario = Scenario(  self.newident, 
        #                                parent = None, 
        #                                workdirpath = self.workdirpath,
        #                                logger = self.get_logger(),
        #                                )

    def get_net(self):
        return self._net
    
class Demand(cm.BaseObjman):
        def __init__(self, scenario=None, net = None, zones = None, name = 'Demand', info ='Transport demand', **kwargs):
            #print 'Network.__init__',name,kwargs
            
            # we need a network from somewhere
            if net == None:
                net = scenario.net
                zones = scenario.landuse.zones
                
            self._init_objman(ident= 'demand', parent=scenario, name = name, info = info, **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            
            self.vtypes = attrsman.add(    cm.ObjConf( vehicles.VehicleTypes(self, net) ) )
            self.trips = attrsman.add(     cm.ObjConf( Trips(self,net)) )
            
            
            self.odintervals = attrsman.add( cm.ObjConf(origin_to_destination.OdIntervals('odintervals',self, net, zones) ))
            self.turnflows = attrsman.add( cm.ObjConf(turnflows.Turnflows('turnflows',self, net) ))
            if scenario != None:
                self.virtualpop = attrsman.add( cm.ObjConf(virtualpop.Virtualpolulation(self) ))
            #print 'Demand',self.odintervals#,self.odintervals.times_start
            #print ' ',dir(self.odintervals)
        
        def get_scenario(self):
            return self.parent
        
        
                
            
            
        
            
        
                       
                  
class Trips(am.ArrayObjman):
    def __init__(self, demand, net, **kwargs):
        
        self._init_objman(  ident='trips', 
                            parent = demand, 
                            name = 'Trips', 
                            info = 'Table with trip and route info.',
                            xmltag = ('trips','trip','ids_sumo'),
                            **kwargs)
        
        self.add_col(SumoIdsConf('Trip', xmltag = 'id'))
        
        self.add_col(am.IdsArrayConf( 'ids_vtype', demand.vtypes, 
                                        groupnames = ['state'], 
                                        name = 'Type', 
                                        info = 'Vehicle type.',
                                        xmltag = 'type',
                                        ))
        
        self.add_col(am.ArrayConf(  'times_depart', 0,
                                        dtype=np.int32,
                                        perm='rw', 
                                        name = 'Depart time',
                                        info = "Departure time of vehicle in seconds. Must be an integer!",
                                        xmltag = 'depart',
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
                                        
        #self.add( cm.ObjConf( net, is_child = False,groups = ['_private']))
        self.configure_routes()
        
                                            
        #self.add_col(am.ArrayConf(  'colors', np.ones(4,np.float32),
        #                               dtype=np.float32,
        #                               metatype = 'color',
        #                               perm='rw', 
        #                               name = 'Color',
        #                               info = "This generated vehicle's color. Color as RGB tuple with values from 0.0 to 1.0",
        #                               )) 
                                        
        #attrsman = self.get_attrsman()                                                  

        
    def clear_trips(self):
        self.clear()
    
    def configure_routes(self, routesname = None, info = None):
        routesindex = len(self.get_group('routes'))
        attrname = self.get_routesattrname (routesindex)
        if routesname == None:
            if routesindex == 0:
                 routesname = 'Route'
            else:
                routesname = 'Route[%s]'%routesindex
                
        if info == None:
            if routesindex == 0:
                info = 'Route as a list of edges IDs.'
            else:
                info = 'Route alternative %s as a list of edges IDs.'%routesindex
            
        self.add_col(am.IdlistsArrayConf( attrname, self.get_net().edges,
                                            groupnames = ['routes','_private'], 
                                            name = routesname, 
                                            info = info,   
                                            ))    
    
    def get_net(self):
        return self.parent.get_scenario().net
    
    def get_scenario(self):
        return self.parent.get_scenario()
    
    def get_routesattrname(self, routesindex):
        return 'routes_%03d'%routesindex
    
    def get_tripfilepath(self):
        dirname = self.get_scenario().get_workdirpath()
        ident = self.get_scenario().get_ident()
        return os.path.join(dirname,ident+'.trip.xml')
    
    def get_routesindex_max(self):
        return len(self.get_group('routes'))-1
    
    def get_routefilepath(self,routesindex = None):
        if routesindex == None:
            routesindex = len(self.get_group('routes'))
        dirname = self.get_scenario().get_workdirpath()
        ident = self.get_scenario().get_ident()
        return os.path.join(dirname,ident+'_%03d.rou.xml'%routesindex)
    
    def route(self, routesindex = 0, is_export_net = False, is_export_trips = False, **kwargs):
            
            routesattrname = self.get_routesattrname(routesindex)
            
            if (not os.path.isfile(self.get_tripfilepath()))|is_export_trips:
                self.export_sumoxml()
                
            if (not os.path.isfile(self.get_net().get_filepath()))|is_export_net:
                self.get_net().export_netxml()
                
            return route(  self.get_tripfilepath(), self.get_net().get_filepath(), 
                            self.get_routefilepath(routesindex), **kwargs)
        
    
    def make_trip(self, routesindex = None, **kwargs):
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
                        routes = kwargs.get('routes',None),
                    )
        
        self.ids_sumo[id_trip] = kwargs.get('id_sumo',str(id_trip)) # id
        
        if kwargs.has_key('route'):
            if routesindex == None:
                routesindex = self.get_routesindex_max()
            routesattrname = self.get_routesattrname(routesindex)
            getattr(self,routesattrname)[id_trip] =  kwargs['route']
        
        return id_trip
    
    def make_trips(self, ids_vtype, routesindex = None, **kwargs):
        
        ids_trip = self.add_rows(ids_vtype = ids_vtype,
                        times_depart = kwargs.get('times_depart',None),
                        ids_edge_depart = kwargs.get('ids_edge_depart',None),
                        ids_edge_arrival = kwargs.get('ids_edge_arrival',None),
                        inds_lane_depart = kwargs.get('inds_lane_depart',None),
                        positions_depart = kwargs.get('positions_depart',None),
                        speeds_depart = kwargs.get('speeds_depart',None),
                        inds_lane_arrival = kwargs.get('inds_lane_arrival',None),
                        positions_arrival = kwargs.get('positions_arrival',None),
                        speeds_arrival = kwargs.get('speeds_arrival',None),
                        #routes = kwargs.get('routes',None),
                    )
        
        self.ids_sumo[ids_trip] = kwargs.get('ids_sumo',np.array(ids_trip,np.str)) 
        
        if kwargs.has_key('routes'):
            if routesindex == None:
                routesindex = self.get_routesindex_max()
            routesattrname = self.get_routesattrname(routesindex)
            getattr(self,routesattrname)[ids_trip] =  kwargs['routes']
            
        return ids_trip
    
    def set_route(self,id_trip, ids_edges, routesname = 'routes'):
        getattr(self,routesname)[id_trip] = ids_edges
   
    def export_sumoxml(self, filepath=None, encoding = 'UTF-8'):
        """
        Export trips to SUMO xml file.
        Method takes care of sorting trips by departure time.
        """
        if filepath == None:
            filepath = self.get_tripfilepath()
        print 'export_sumoxml',filepath
        try:
            fd=open(filepath,'w')
        except:
            print 'WARNING in write_obj_to_xml: could not open',filepath
            return False
        
        xmltag, xmltag_item, attrname_id = self.xmltag
        fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
        fd.write(xm.begin(xmltag))
        indent = 2
        
        #ids_modes_used = set(self.parent.vtypes.ids_mode[self.ids_vtype.get_value()])
        self.parent.vtypes.write_xml(   fd, indent=indent, 
                                        ids = set(self.ids_vtype.get_value()), 
                                        is_print_begin_end = False)
        self.write_xml( fd,indent=indent,
                                ids = self.times_depart.get_ids_sorted(),
                                is_print_begin_end = False)
        
        fd.write(xm.end(xmltag))
        fd.close()
        return filepath
    
    def import_sumoxml(self, filepath, routesindex = None, is_clear_trips = False):
        print 'import_sumoxml from %s generate own trip routesindex=%s'%(filepath, routesindex)
        if is_clear_trips:
            self.clear_trips()
            
        tripcounter = TripCounter()
        parse(filepath, tripcounter)
        routereader = TripReader(self, tripcounter.n_trip, routesindex)
        try:
            parse(filepath, routereader)
        except KeyError:
            print >> sys.stderr, "Error: Problems with reading routes!"
            raise
        
    
class TripCounter(handler.ContentHandler):
    """Parses a SUMO route XML file and counts trips."""

    def __init__(self):
        self.n_trip = 0
        
    
    def startElement(self, name, attrs):
        #print 'startElement',name,self.n_edge,self.n_lane,self.n_roundabout
        if name == 'vehicle': 
            self.n_trip += 1
            
    

        
class TripReader(handler.ContentHandler):
    """Reads trips from trip or route file into trip table"""

    def __init__(self, trips,  n_trip, routesindex = None, is_generate_ids=False):
        #print 'RouteReader.__init__',demand.ident
        self._trips = trips
        demand = trips.parent
        self._routesindex = routesindex
           
        net = demand.get_scenario().net
        
        self._ids_vtype_sumo = demand.vtypes.ids_sumo
        self._ids_edge_sumo = net.edges.ids_sumo
        
        self.ids_sumo = np.zeros(n_trip, np.object)
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
        self.routes = np.zeros(n_trip, np.object)
        
        self._ind_trip = -1         
        
        
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
            
            self.ids_vtype[self._ind_trip] = self._ids_vtype_sumo.get_id_from_index(str(attrs['type']))
            self.times_depart[self._ind_trip] = int(float(attrs['depart']))
            
            if attrs.has_key('from'):
                self.ids_edge_depart[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['from']))
            if attrs.has_key('to'):
                self.ids_edge_arrival[self._ind_trip] = self._ids_edge_sumo.get_id_from_index(str(attrs['to']))
            
            
            ind_lane_depart_raw = attrs.get('departLane', 'free')
            if OPTIONMAP_LANE_DEPART.has_key(ind_lane_depart_raw):
                self.inds_lane_depart[self._ind_trip] = OPTIONMAP_LANE_DEPART[ind_lane_depart_raw]
            else:
                self.inds_lane_depart[self._ind_trip] = int(ind_lane_depart_raw)
                
            positions_depart_raw = attrs.get('departPos', 'base')
            if OPTIONMAP_POS_DEPARTURE.has_key(positions_depart_raw):
                self.positions_depart[self._ind_trip] = OPTIONMAP_POS_DEPARTURE[positions_depart_raw]
            else:
                self.positions_depart[self._ind_trip] = float(positions_depart_raw)
                
            self.speeds_depart[self._ind_trip] = attrs.get('departSpeed', 0.0)
            
            
            ind_lane_arrival_raw = attrs.get('arrivalLane', 'current')
            if OPTIONMAP_LANE_ARRIVAL.has_key(ind_lane_arrival_raw):
                self.inds_lane_arrival[self._ind_trip] = OPTIONMAP_LANE_ARRIVAL[ind_lane_arrival_raw]
            else:
                self.inds_lane_arrival[self._ind_trip] = int(ind_lane_arrival_raw)
            
            positions_arrival_raw = attrs.get('arrivalPos', 'max')
            if OPTIONMAP_POS_ARRIVAL.has_key(positions_arrival_raw):
                self.positions_arrival[self._ind_trip] = OPTIONMAP_POS_ARRIVAL[positions_arrival_raw]
            else:
                self.positions_arrival[self._ind_trip] =  float(positions_arrival_raw)
            
            
            self.speeds_arrival[self._ind_trip] = attrs.get('arrivalSpeed', 0.0)
            
            
            
            
            #OPTIONMAP_POS_DEPARTURE = { "random":-1,"free":-2,"random_free":-3,"base":-4} 
            #OPTIONMAP_POS_ARRIVAL = { "random":-1,"max":-2} 
            #OPTIONMAP_SPEED_DEPARTURE = {"random":-1,"max":-2} 
            #OPTIONMAP_SPEED_ARRIVAL = {"current":-1}
            #OPTIONMAP_LANE_DEPART = {"random":-1,"free":-2,"departlane":-3}
            #OPTIONMAP_LANE_ARRIVAL = {"current":-1}

        if name == 'route':
            #print ' ',attrs.get('edges', '')
            self._ids_sumoedge_current = attrs.get('edges', '')
            self._intervals_current = attrs.get('intervals', '')
            
    #def characters(self, content):
    #    if (len(self._route_current)>0)&(self._intervals_current!=''):
    #        self._intervals_current = self._intervals_current + content
            
    def endElement(self, name):
        
        if name == 'vehicle':
            #print 'endElement',name,self._id_current,len(self._intervals_current)
            if (self._id_sumoveh_current!=None):
                ids_edge = []
                for id_sumoedge in self._ids_sumoedge_current.split(' '):
                    if not id_sumoedge in ('',' ',','):
                        ids_edge.append(self._ids_edge_sumo.get_id_from_index(id_sumoedge.strip()) )
                self.routes[self._ind_trip] = ids_edge
                
                if len(ids_edge)>=1:
                    self.ids_edge_depart[self._ind_trip] = ids_edge[0]
                    self.ids_edge_arrival[self._ind_trip] = ids_edge[-1]
  
                self._id_sumoveh_current = None
                #self._attrs = {}
                self._ids_sumoedge_current = []
        
        elif name in ['routes','trips']:
            self.write_to_trips()
                
    def process_intervals(self):
        interval = []
        es = self._intervals_current.rstrip().split(" ")
        for e in es:
            p = e.split(",")
            interval.append((float(p[0]), float(p[1])))
        self._intervals_current = ''
        return interval
    
    def write_to_trips(self):
        
        #print 'write_to_net'
        ids_trips = self._trips.make_trips( self.ids_vtype, self._routesindex,
                                            ids_sumo = self.ids_sumo,
                                            times_depart = self.times_depart,
                                            ids_edge_depart = self.ids_edge_depart,
                                            ids_edge_arrival = self.ids_edge_arrival,
                                            inds_lane_depart = self.inds_lane_depart,
                                            positions_depart = self.positions_depart,
                                            speeds_depart = self.speeds_depart,
                                            inds_lane_arrival = self.inds_lane_arrival,
                                            positions_arrival = self.positions_arrival,
                                            speeds_arrival = self.speeds_arrival,
                                            routes = self.routes,
                                            )
                        
                    
        return ids_trips
                                               
if __name__ == '__main__':          
    ###############################################################################
    #print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    #from coremodules.scenario import scenario
    from coremodules.network import network
    logger = Logger()
    NETPATH = os.path.join(SUMOPYDIR,'coremodules','network','testnet')
    net = network.Network(logger = logger)
    ident_scenario = 'facsp2'
    net.import_xml(ident_scenario, NETPATH)
    #net.read_sumonodes(os.path.join(NETPATH,'facsp2.nod.xml'))
    #net.read_sumoedges(os.path.join(NETPATH,'facsp2.edg.xml'))
    demand = Demand( net = net, logger = logger)
    #demand.set_net(net)
    #landuse.facilities.import_poly(os.path.join(NETPATH,'facsp2.poly.xml'))
    #landuse.import_xml(ident_scenario, NETPATH)
    objbrowser(demand)
    
    
