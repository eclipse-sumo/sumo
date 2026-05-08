
from xml.sax import saxutils, parse, handler
import os, sys, shutil
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.processes import Process
from xml.sax import parse, handler
try:
    import pyproj
except:
    from mpl_toolkits.basemap import pyproj
    
from .matsim_base import  *
pathsep = os.path.sep
#from coremodules.network.network import SumoIdsConf

                          
duration_last_activity = 3*3600 # this is arbitrary, but does not have any effect on the simuation

class PublicTransport(cm.BaseObjman):
        def __init__(self, parent=None, name = 'PublicTransport', **kwargs):
            print ('PublicTransport',parent,name)
            self._init_objman(  ident= 'pt', parent=parent, name = name,
                                #xmltag = 'net',# no, done by netconvert
                                version = 0.1,
                                **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            #self._init_attributes()
            self.ptstops = attrsman.add(   cm.ObjConf( PtStops(self) ) )
            self.ptlines = attrsman.add(   cm.ObjConf( PtLines(self) ) )

        def _init_attributes(self):
            attrsman = self.get_attrsman()
            self.population_config = attrsman.add(cm.AttrConf( 'population_config','path to file',
                            groupnames = ['options'], 
                            name = 'population_config', 
                            info = "population_config.",
                            ))
            self.general_config = attrsman.add(cm.AttrConf( 'general_config','path to file',
                            groupnames = ['options'], 
                            name = 'general_config', 
                            info = "general_config",
                            ))
#         self.add_col(am.IdlistsArrayConf( 'ids_stops', net.ptstops,
#                                            groupnames = ['parameters'], 
#                                            name = 'PT stop IDs', 
#                                            info = 'Sequence of IDs of stops or stations of a public transort line.',   
#                                            ))
            
class PtStops(am.ArrayObjman):
    def __init__(   self, parent,
                    **kwargs):
        ident = 'ptstops'
        self._init_objman( ident=ident, parent=parent, name = 'ptstops',
                            #xmltag = ('ptstops'),
                            version = 0.0,
                            info = 'ptstops',
                            **kwargs)
        self._init_attributes()

    def _init_attributes(self):

        self.add_col(am.ArrayConf('id', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'id',
                                info = 'id',
                                #xmltag = 'id',
                                ))
        
        self.add_col(am.ArrayConf( 'coords',  np.zeros(3, dtype=np.float64),
                            dtype = np.float64,
                            groupnames = ['state'],
                            perm = 'r',
                            name = 'Coords',
                            unit = 'm',
                            info = 'Node center coordinates.',
                            ))
        
        self.add_col(am.ArrayConf('linkRefId', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'linkRefId',
                                info = 'Referenz to MATSim Link',
                                #xmltag = 'linkRefId',
                                ))
        self.add_col(am.ArrayConf('isBlocking', default = False,
                                dtype = bool,
                                perm = 'r',
                                is_index = False,
                                name = 'isBlocking',
                                info = 'isBlocking',
                                #xmltag = 'isBlocking',
                                ))
class PtLines(am.ArrayObjman):
    def __init__(   self, parent,
                    **kwargs):
        ident = 'ptlines'
        self._init_objman( ident=ident, parent=parent, name = 'ptlines',
                            #xmltag = ('ptlines'),
                            version = 0.0,
                            info = 'ptlines',
                            **kwargs)
        
        _matsim = self.parent.parent
        net = _matsim.net
        pt = self.parent 
        self._init_attributes()

    def _init_attributes(self):

        self.add_col(am.ArrayConf('transitline_id', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'transitline_id',
                                info = 'transitline_id',
                                #xmltag = 'transitline_id',
                                ))
        
        self.add_col(am.ArrayConf('transitroute_id', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'transitroute_id',
                                info = 'transitroute_id',
                                #xmltag = 'transitroute_id',
                                ))
        self.add_col(am.ArrayConf('mode', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'mode',
                                info = 'mode',
                                #xmltag = 'mode',
                                ))
        self.add_col(am.ListArrayConf( 'ids_stops',
                                            groupnames = ['parameters'], 
                                            name = 'PT stop IDs', 
                                            info = 'Sequence of IDs of stops or stations of a public transort line.',   
                                            ))
        self.add_col(am.ListArrayConf( 'stops_arrival_offset',
                                            groupnames = ['parameters'], 
                                            name = 'pt stop arrival offset', 
                                            info = 'pt stop arrival offset',   
                                            ))
        self.add_col(am.ListArrayConf( 'stops_departure_offset',
                                            groupnames = ['parameters'], 
                                            name = 'pt stop departure offset', 
                                            info = 'pt stop departure offset',   
                                            ))
        self.add_col(am.ListArrayConf( 'stops_await',
                                            groupnames = ['parameters'], 
                                            name = 'waiting at pt stop', 
                                            info = 'waiting at pt stop',   
                                            ))
        self.add_col(am.ListArrayConf( 'ids_route', 
                                            groupnames = ['parameters'], 
                                            name = 'List of Edges belonging to the route', 
                                            info = 'Sequence of IDs of edges of a public transort line.',   
                                            ))
        self.add_col(am.ListArrayConf( 'departure_id',
                                            groupnames = ['parameters'], 
                                            name = 'departure_id', 
                                            info = 'departure_id',   
                                            ))
        self.add_col(am.ListArrayConf( 'departure_time',
                                            groupnames = ['parameters'], 
                                            name = 'departure_time', 
                                            info = 'departure_time',   
                                            ))
        
        self.add_col(am.ListArrayConf( 'departure_veh',
                                            groupnames = ['parameters'], 
                                            name = 'departure_veh', 
                                            info = 'departure_veh',   
                                            ))
   
class PtStopParser(ParserMixin,handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self,_matsim, number,projparams_matsim, projectionmode):
        self._matsim = _matsim
        self.numer = number
        self._init_projection(projparams_matsim, projectionmode)
        self.ids_ptstops = np.zeros(number, dtype = object)
        self.coords = np.zeros((number,2), dtype = np.float64)
        self.linkRefId = np.zeros(number, dtype = object)
        self.isBlocking = np.zeros(number, dtype = bool)
        self.ind_node = 0
    
    def get_coords_nodes_sumo(self):
        coords_sumo = np.zeros((self.number,3), dtype = np.float64)
        coords_sumo[:,0], coords_sumo[:,1]=  self.get_coord_sumo(self.coords[:,0],self.coords[:,1])
        return coords_sumo
    
    def startElement(self, name, attrs):
        
        if name == 'stopFacility':
            self.ids_ptstops[self.ind_node] = attrs['id']
            self.coords[self.ind_node,:2]  = (attrs['x'],attrs['y'])
            self.linkRefId[self.ind_node] = attrs['linkRefId']
            self.isBlocking[self.ind_node] = attrs['isBlocking']
            self.ind_node +=1

class PtStopCounter(handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self):
        self.counter = 0

    # module # paramset1 #paramset2 #param 

    def startElement(self, name, attrs):
        
        if name == 'stopFacility':
            self.counter +=1

class PtLineParser(ParserMixin,handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self,_matsim):
        self._matsim = _matsim
        #self._init_projection(projparams_matsim, projectionmode)
        #self.ids_ptstops = np.zeros(number, dtype = object)
        #self.coords = np.zeros((number,2), dtype = np.float64)
        #self.linkRefId = np.zeros(number, dtype = object)
        #self.isBlocking = np.zeros(number, dtype = bool)
        #self.ind_node = 0
        self.transitline = np.zeros(1,object)
        self.transitroute = np.zeros(1,object)
        self.ids_ptstops = np.zeros(1,object)
        self.arrivalOffset = np.zeros(1,object)
        self.departureOffset = np.zeros(1,object)
        self.awaitDeparture = np.zeros(1,object)
        self.ids_edges = np.zeros(1,object)
        self.departure_id  = np.zeros(1,object)
        self.departure_time  = np.zeros(1,object)
        self.departure_vehicle  = np.zeros(1,object)
        self.mode = np.zeros(1,object)
        
        self.ids_ptstops[0] = list([])
        self.arrivalOffset[0] = list([])
        self.departureOffset[0] = list([])
        self.awaitDeparture[0] = list([])
        self.ids_edges[0] = list([])
        self.departure_id[0]  = list([])
        self.departure_time[0]  = list([])
        self.departure_vehicle[0]  = list([])
    
    def startElement(self, name, attrs):
        if name == 'transitLine':
            self.transitline[0] = attrs['id']
        if name == 'transitRoute':
            self.transitroute[0] = attrs['id']
        if name == 'stop':
            #refId="22529_opnv.link:pt_22529_opnv" arrivalOffset="00:01:00" departureOffset="00:01:00" awaitDeparture="true"
            self.ids_ptstops[0].append(attrs['refId'])
            if 'arrivalOffset' in attrs:
                self.arrivalOffset[0].append(attrs['arrivalOffset'])
            else:
                self.arrivalOffset[0].append('')
            
            if 'departureOffset' in attrs:
                self.arrivalOffset[0].append(attrs['departureOffset'])
            else:
                self.arrivalOffset[0].append('')
            self.awaitDeparture[0].append(attrs['awaitDeparture']) 
        
        if name == 'link':
            #refId="22529_opnv.link:pt_22529_opnv" arrivalOffset="00:01:00" departureOffset="00:01:00" awaitDeparture="true"
            self.ids_edges[0].append(attrs['refId'])
        
        if name == 'departure':
            #<departure id="1060609_12:09:00_opnv" departureTime="12:09:00" vehicleRefId="veh_27323_bus_opnv"/>
            self.departure_id[0].append(attrs['id'])
            self.departure_time[0].append(attrs['departureTime'])
            self.departure_vehicle[0].append(attrs['vehicleRefId'])
            
        if name == 'transportMode':
            self.mode[0] = attrs

    def endElement(self, name):
        if name == 'transitRoute':
            # Prüfe den Datentyp, auf welche die Listen gemappt werden
            # Hier ist soeben noch ein Fehler: ValueError: setting an array element with a sequence
            self._matsim.pt.ptlines.add_rows(None, #suggest id
                            transitline_id = self.transitline,
                            transitroute_id = self.transitroute,
                            mode = self.mode,
                            ids_stops = self.ids_ptstops,
                            stops_arrival_offset = self.arrivalOffset,
                            stops_departure_offset = self.departureOffset,
                            stops_await = self.awaitDeparture,
                            ids_route = self.ids_edges,
                            departure_id = self.departure_id,
                            departure_time = self.departure_time,
                            departure_veh = self.departure_vehicle,
                            )
            self.ids_ptstops[0] = list([])
            self.arrivalOffset[0] = list([])
            self.departureOffset[0] = list([])
            self.awaitDeparture[0] = list([])
            self.ids_edges[0] = list([])
            self.departure_id[0]  = list([])
            self.departure_time[0]  = list([])
            self.departure_vehicle[0]  = list([])

class PtLineCounter(handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self):
        self.counter = 0

    # module # paramset1 #paramset2 #param 

    def startElement(self, name, attrs):
        
        if name == 'stopFacility':
            self.counter +=1

class PtImport(Process):

    def __init__(self,  matsim, configpath = '',
                        logger = None):
        #
        #
        #
        self._init_common(  'Import Public Transport', name = 'MATSIM import configuration', 
                            logger = logger,
                            info ='Imports MATSIM network XML file into network database.',
                            )
        self._scenario = matsim.get_scenario()
        self._matsim = matsim
        self._net = matsim.net
        self.projparams_matsim = matsim.net.projparams
        self.projmode = matsim.net.projmode        
        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.schedulepath = attrsman.add(cm.AttrConf('schedulepath',configpath,
                        groupnames = ['options'],# 
                        name = 'schedulepath', 
                        wildcards = 'Net XML file (*.xml)|*.xml*',
                        metatype = 'filepath',
                        info = 'MATSIM pt schedule file to be imported.',
                        ))
    
    def do(self):

        if os.path.exists(self.schedulepath):
            self._matsim.pt.ptstops.clear()
            self._matsim.pt.ptlines.clear()
            parser = PtLineParser(self._matsim)
            parse(self.schedulepath, parser)

            parser = PtStopCounter()
            parse(self.schedulepath, parser)
            counts = parser.counter
            parser2 = PtStopParser(self._matsim,counts,self.projparams_matsim,self.projmode)
            parse(self.schedulepath, parser2)
            #Problem: Parser läuft noch, während der Dialog beendet wird

            self._matsim.pt.ptstops.add_rows(None, #suggest id
                            id = parser2.ids_ptstops,
                            coords = parser2.get_coords_nodes_sumo(),
                            linkRefId = parser2.linkRefId,
                            isBlocking = parser2.isBlocking,
                            )
            print ('PtStop Parsing done')
            return True

        