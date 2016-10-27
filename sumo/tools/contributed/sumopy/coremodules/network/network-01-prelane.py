
import os, sys
from xml.sax import saxutils, parse, handler
if  __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    SUMOPYDIR = os.path.join(APPDIR,'..','..')
    sys.path.append(os.path.join(SUMOPYDIR))

            
import numpy as np            
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am

from agilepy.lib_base.geometry import *

MODES = [           #"public_emergency",  # deprecated
                    #"public_authority",  # deprecated
                    #"public_army",       # deprecated
                    #"public_transport",  # deprecated
                    #"transport",         # deprecated
                    #"lightrail",         # deprecated
                    #"cityrail",          # deprecated
                    #"rail_slow",         # deprecated
                    #"rail_fast",         # deprecated
                    "private",           
                    "emergency",  
                    "authority",  
                    "army",       
                    "vip",               
                    "passenger",         
                    "hov",               
                    "taxi",              
                    "bus",               
                    "coach",               
                    "delivery",          
                    "truck",         
                    "trailer",         
                    "tram",        
                    "rail_urban",        
                    "rail",        
                    "rail_electric",        
                    "motorcycle",        
                    "moped",        
                    "bicycle",           
                    "pedestrian",
                    "evehicle",
                    "custom1",           
                    "custom2",
                    ]
                                
class OsmIdsConf(am.ArrayConf):
    """
    Sumo id array coniguration
    """
    #def __init__(self, **attrs):
    #    print 'ColConf',attrs
    def __init__(self, refname):
        am.ArrayConf.__init__(self, attrname='ids_osm', default = '',
                                    dtype = 'object',
                                    perm='r', 
                                    is_index = True,
                                    name = 'ID '+refname,
                                    info = refname + ' ID of SUMO network',
                                    )
                                        


                    
class Modes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions
    def __init__(self, parent, modes, **kwargs):
        ident = 'modes'
        self._init_objman(ident=ident, parent=parent, name = 'Transport Modes', **kwargs)
        self.add_col(am.ArrayConf( 'names', '',
                                    dtype = np.object,
                                    perm='r', 
                                    is_index = True,
                                    name = 'Name',
                                    info = 'Name of mode. Used as key for implementing acces restrictions on edges as well as demand modelling.',
                                    ))
        
        
class Lanes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions
    def __init__(self, parent, edges, modes, **kwargs):
        ident = 'lanes'
        self._init_objman(ident=ident, parent=parent, name = 'Lanes', **kwargs)
        
        self.add_col(am.ArrayConf( 'indexes', 1,
                                            dtype = np.int32,
                                            perm='r', 
                                            name = 'Lane index',
                                            info = 'The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one).',
                                            ))
                                        
        self.add_col(am.ArrayConf( 'widths', 3.5,
                                            dtype = np.float32,
                                            perm='rw', 
                                            name = 'Width',
                                            info = 'Lane width.',
                                            ))
                                        
        self.add_col(am.NumArrayConf('speeds_max', 50.0/3.6,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'rw', 
                                            name = 'Max speed',
                                            unit = 'm/s', 
                                            info = 'Maximum speed on lane.',
                                            ))
                                            
        self.add_col(am.NumArrayConf('offsets_end', 0.0,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'r', 
                                            name = 'End offset',
                                            unit = 'm', 
                                            info = 'Move the stop line back from the intersection by the given amount (effectively shortening the lane and locally enlarging the intersection).',
                                            ))
                                            
                                        
        self.add_col(am.IdlistsArrayConf( 'modes_allow', modes,
                                            name = 'Allowed', 
                                            info = 'Allowed modes on this lane.',   
                                            ))
                                        
        self.add_col(am.IdlistsArrayConf( 'modes_disallow', modes,
                                            name = 'Disallow', 
                                            info = 'Disallowed modes on this lane.',   
                                            ))
                                        
        
        self.add_col(am.IdsArrayConf( 'ids_edge', edges, 
                                            groupnames = ['state'], 
                                            name = 'ID edge', 
                                            info = 'ID of edge in which the lane is contained.',
                                            ))
        
        self.add_col(am.ListArrayConf( 'shapes', 
                                            groupnames = ['_private'], 
                                            perm='rw', 
                                            name = 'Shape',
                                            unit = 'm',
                                            info = 'List of 3D Shape coordinates to describe polyline.',
                                            ))
    def make( self, ind, **kwargs):
        # edges = self.ids_edge.get_linktab()
        
        
        return self.add_row(   indexes = ind,
                               widths = kwargs.get('width',None),
                               speeds_max = kwargs.get('speed_max',None),
                               offsets_end = kwargs.get('offset_end',None),
                               modes_allow = kwargs.get('mode_allow',None),
                               modes_disallow = kwargs.get('mode_disallow',None),
                               ids_edge = kwargs.get('id_edge',None),
                               shapes = kwargs.get('shapes',None),
                            )
                                            
                                            
                                            
class Edges(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions
    def __init__(self, parent, **kwargs):
            ident = 'edges'
            self._init_objman(ident=ident, parent=parent, name = 'Edges', **kwargs)
            self.add_col(OsmIdsConf('Edge'))
            
            
                                    
            self.add_col(am.ArrayConf( 'types', '',
                                            dtype = np.object,
                                            perm='rw', 
                                            name = 'Type',
                                            info = 'Edge reference type.',
                                            ))
                                
            self.add_col(am.ArrayConf( 'nums_lanes', 1,
                                            dtype = np.int32,
                                            perm='r', 
                                            name = '# of lanes',
                                            info = 'Number of lanes.',
                                            ))
            
            self.add_col(am.NumArrayConf('speeds_max', 50.0/3.6,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'rw', 
                                            name = 'Max speed',
                                            unit = 'm/s', 
                                            info = 'Maximum speed on edge.',
                                            ))
                                            
            self.add_col(am.ArrayConf( 'priorities', 1,
                                            dtype = np.int32,
                                            perm='rw', 
                                            name = 'Priority',
                                            info = 'Road priority (1-9).',
                                            ))
                                
            self.add_col(am.NumArrayConf('lengths', 0.0,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'r', 
                                            name = 'Length',
                                            unit = 'm', 
                                            info = 'Edge length.',
                                            ))
            
            self.add_col(am.NumArrayConf('widths', 0.0,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'r', 
                                            name = 'Width',
                                            unit = 'm', 
                                            info = 'Edge width.',
                                            ))
                                                                            
            self.add_col(am.ListArrayConf( 'shapes', 
                                            groupnames = ['_private'], 
                                            perm='rw', 
                                            name = 'Shape',
                                            unit = 'm',
                                            info = 'List of 3D Shape coordinates to describe polyline.',
                                            ))
                                            
            self.add_col(am.ArrayConf( 'types_spread', 0,
                                            choices = {\
                                                "right": 0,
                                                "center": 1, 
                                                },
                                            dtype = np.int32,
                                            perm='rw', 
                                            name = 'Spread type',
                                            info = 'Determines how the lanes are spread with respect to main link coordinates.',
                                            ))
                                
            self.add_col(am.ArrayConf( 'names', '',
                                            dtype = np.object,
                                            perm='rw', 
                                            name = 'Name',
                                            info = 'Road name, for visualization only.',
                                            ))
                                            
            self.add_col(am.NumArrayConf('offsets_end', 0.0,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'r', 
                                            name = 'End offset',
                                            unit = 'm', 
                                            info = 'Move the stop line back from the intersection by the given amount (effectively shortening the edge and locally enlarging the intersection).',
                                            ))
            
            self.add_col(am.NumArrayConf('widths_lanes_default', 3.5,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'rw', 
                                            name = 'Default lane width',
                                            unit = 'm', 
                                            info = 'Default lane width for all lanes of this edge in meters (used for visualization).',
                                            ))
                                            
            self.add_col(am.NumArrayConf('widths_sidewalk', -1.0,
                                            dtype = np.float32,
                                            groupnames = ['state'], 
                                            perm = 'rw', 
                                            name = 'Sidewalk width',
                                            unit = 'm', 
                                            info = 'Adds a sidewalk with the given width (defaults to -1 which adds nothing).',
                                            ))       
                                    
    def set_nodes(self, nodes):
        # set ref to nodes table, once initialized
        self.add_col(am.IdsArrayConf( 'ids_fromnode', nodes, 
                                            groupnames = ['state'], 
                                            name = 'ID from-node', 
                                            info = 'ID of node at the beginning of the edge.',
                                            ))
    
    
        self.add_col(am.IdsArrayConf( 'ids_tonode', nodes,
                                            groupnames = ['state'], 
                                            name = 'ID to-node', 
                                            info = 'ID of node at the end of the edge.',
                                            ))
                                        
    def set_lanes(self, lanes):
        if 0:
            self.add_col(IdlistsArrayConf( 'ids_lanes', lanes,
                                            groupnames = ['state'], 
                                            name = 'IDs Lanes', 
                                            info = 'List with IDs of lanes.',   
                                            ))
    
                 
    def make( self, id_sumo = '', 
                    id_fromnode_sumo = 0,
                    id_tonode_sumo = 0,
                    type_edge = '',
                    num_lanes = 1,
                    speed_max = 50.0/3.6,
                    priority = 1,
                    #length = 0.0,
                    shape = None,
                    type_spread = 'right',
                    name = '',
                    offset_end = 0.0,
                    #width_lanes_default = 3.5,
                    #width_sidewalk_default = -1.0
                    ):
        nodes = self.ids_tonode.get_linktab()
        id_fromnode = nodes.ids_osm.get_id_from_index(id_fromnode_sumo)
        id_tonode = nodes.ids_osm.get_id_from_index(id_tonode_sumo)
        if shape == None:
            shape = [ nodes.coords[id_fromnode], nodes.coords[id_tonode] ]
            #print 'Edges.make: shape =',shape
        #print 'Edges.make'
        #print '  shape',shape,type(shape)
         
        return self.add_row(   ids_osm = id_sumo,
                        ids_fromnode = id_fromnode,
                        ids_tonode = id_tonode,
                        types = type_edge,
                        nums_lanes = num_lanes,
                        speeds_max = speed_max,
                        priorities = priority,
                        #lengths = length,
                        shapes = shape,
                        types_spread = self.types_spread.choices[type_spread],
                        names = name,
                        offsets_end = offset_end,
                        #widths_lanes_default = width_lanes,
                        #widths_sidewalk = width_sidewalk,
                        )           
    def update(self):
        #print 'Edges.update'
        self.widths.value = self.nums_lanes.value * self.widths_lanes_default.value \
                            + (self.widths_sidewalk.value >= 0) * (self.widths_sidewalk.value-self.widths_lanes_default.value)
        
        #print '  self.widths.values =  \n',self.widths.value
        polylines = polypoints_to_polylines(self.shapes.value)
        #print '  polylines[0:4]=\n',polylines[0:4]
        #print '  polylines[3].shape',polylines[3].shape
        self.lengths.value = get_length_polylines(polypoints_to_polylines(self.shapes.value))
        
        
        
                                                          


class Nodes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Node_Descriptions
    def __init__(self, parent, **kwargs):
        ident = 'nodes'
        self._init_objman(ident=ident, parent=parent, name = 'Nodes', **kwargs)
        
        self.add_col(OsmIdsConf('Node'))
                    
        self.add_col(am.ArrayConf( 'coords',  np.zeros(3, dtype=np.float32),
                            dtype=np.float32,
                            groupnames = ['state'],
                            perm='r', 
                            name = 'Coords',
                            unit = 'm',
                            info = 'Node center coordinates.',
                            ))
                            
        
        
        self.add_col(am.ArrayConf( 'types', 0,
                                choices = {\
                                    "priority":0, 
                                    "traffic_light":1, 
                                    "right_before_left":2, 
                                    "unregulated":3, 
                                    "priority_stop":4, 
                                    "traffic_light_unregulated":5, 
                                    "allway_stop":6, 
                                    "rail_signal":7,
                                    "zipper":8, 
                                    "traffic_light_right_on_red":9, 
                                    "rail_crossing":10,
                                    "dead_end":11,
                                    },
                                dtype = np.int32,
                                perm='rw', 
                                name = 'Type',
                                info = 'Node type.',
                                ))
                                
        self.add_col(am.ArrayConf( 'types_tl', 0,
                                choices = {\
                                    "none":0,
                                    "static":1, 
                                    "actuated":2,
                                    },
                                dtype = np.int32,
                                perm='rw', 
                                name = 'TL type',
                                info = 'Traffic light type.',
                                ))
                                
        # TODO: this should become ID conf as soon as TLS is implemented
        self.add_col(am.ArrayConf( 'ids_tl_prog',  0,
                            dtype=np.int32,
                            groupnames = ['state'],
                            perm='rw', 
                            name = 'ID TL Prog',
                            info = 'ID of traffic light programme. Nodes with the same tl-value will be joined into a single program.',
                            ))
                                                    
        self.add_col(am.ArrayConf( 'radii',  1.5,
                            dtype=np.float32,
                            groupnames = ['state'],
                            perm='rw', 
                            name = 'Turn rad',
                            unit = 'm',
                            info = 'optional turning radius (for all corners) for that node.',
                            ))
                            
        self.add_col(am.ArrayConf( 'are_keep_clear',  True,
                            dtype=np.bool,
                            groupnames = ['state'],
                            perm='rw', 
                            name = 'keep clear',
                            info = 'Whether the junction-blocking-heuristic should be activated at this node.',
                            ))
                                
                                
    
    def set_edges(self, edges):
        if 0:
            self.add_col(am.IdlistsArrayConf( 'ids_incoming', edges,
                                                groupnames = ['state'], 
                                                name = 'ID incoming', 
                                                info = 'ID list of incoming edges.',
                                                ))
                                            
            self.add_col(am.IdlistsArrayConf(   'ids_outgoing', edges,
                                                groupnames = ['state'], 
                                                name = 'ID outgoing', 
                                                info = 'ID list of outgoing edges.',
                                                ))
                                            
    def make( self, id_sumo = '', nodetype = 'priority', coord = [], 
                type_tl = 'Static', id_tl_prog = 0,
                radius = 1.5, is_keep_clear = True):
                 
        return self.add_row( ids_osm = id_sumo, 
                types = self.types.choices[nodetype], 
                coords = coord, 
                types_tl = self.types_tl.choices[type_tl],
                ids_tl_prog = id_tl_prog,
                radii = radius,
                are_keep_clear = is_keep_clear,
                )
        
    
                                        

class Network(cm.BaseObjman):
        def __init__(self, parent=None, name = 'Network', **kwargs):
            #print 'Network.__init__',name,kwargs
            self._init_objman(ident= 'net', parent=parent, name = name, **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            
            self._version = attrsman.add(cm.AttrConf(  '_version', '0.13',
                                    groupnames = ['aux'], 
                                    perm='r', 
                                    is_save = True,
                                    name = 'Network version', 
                                    info = 'Network version (not SUMO version)',
                                    ))
                                        

                                        
            self.modes = attrsman.add(   cm.ObjConf( Modes(self) ) )
            self.modes.add_rows(len(MODES), names = MODES)
            
            self.nodes = attrsman.add(   cm.ObjConf( Nodes(self) ) )
            self.edges = attrsman.add(   cm.ObjConf( Edges(self) ) )
            self.lanes = attrsman.add(   cm.ObjConf( Lanes(self, edges, modes) ) )
            
            self.edges.set_nodes(self.nodes)
            self.edges.set_lanes(self.lanes)
            self.nodes.set_edges(self.edges)
            
            
            
            self._offset = attrsman.add(cm.AttrConf(  '_offset', np.array([0.0,0.0]),
                                        groupnames = ['params',], 
                                        perm='r', 
                                        name = 'Offset', 
                                        info = 'Network offset in WEP coordinates'
                                        ))
            
            
            self._projparams = attrsman.add(cm.AttrConf(  '_projparams', "!",
                                        groupnames = ['params',], 
                                        perm='r', 
                                        name = 'Projection', 
                                        info = 'Projection parameters'
                                        ))
            
                                     
            self._boundaries = attrsman.add(cm.AttrConf(  '_boundaries', np.array([0.0,0.0,0.0,0.0]),
                                        groupnames = ['params',], 
                                        perm='r', 
                                        name = 'Boundaries',
                                        unit = 'm', 
                                        info = 'Network boundaries'
                                        ))
                                        
            self._boundaries_orig = attrsman.add(cm.AttrConf(  '_boundaries_orig', np.array([0.0,0.0,0.0,0.0]),
                                        groupnames = ['params',], 
                                        perm='r', 
                                        name = 'Orig. boundaries', 
                                        info = 'Original network boundaries'
                                        ))
            
                                        
            
        def set_version(self, version):
            self._version =  version

        
        def get_version(self):
            return self._version
    
        def set_offset(self, offset):
            self._offset = offset
            
        def get_offset(self):
            return self._offset
        
        def set_boundaries(self, convBoundary, origBoundary = None):
            """
            Format of Boundary box
             [MinX, MinY ,MaxX, MaxY ]
            
            """
            self._boundaries = convBoundary
            if origBoundary == None:
                self._boundaries_orig = self._boundaries
            else:
                self._boundaries_orig = origBoundary
                
        def get_boundaries(self):
            return self._boundaries, self._boundaries_orig
    
        def merge_boundaries(self, convBoundary, origBoundary = None):
            """
            Format of Boundary box
             [MinX, MinY ,MaxX, MaxY ]
            
            """
            #print 'mergeBoundaries'
            self._boundaries = self.getBoundaryUnion(convBoundary,self._boundaries)
            if origBoundary == None:
                self._boundaries_orig = self._boundaries
            else:
                self._boundaries_orig = self.get_boundary_union(origBoundary,self._boundaries_orig)
            #print '  self._boundaries_orig =',self._boundaries_orig
            #print '  self._boundaries =',self._boundaries
        
        def get_boundary_union(self,BB1,BB2):
            return [min(BB1[0],BB2[0]),min(BB1[1],BB2[1]),max(BB1[2],BB2[2]) ,max(BB1[3],BB2[3])]      
            
        def get_projparams(self):
            return self._proj_parameter
        
        
        def set_projparams(self, projparams="!"):
            #print 'setprojparams',projparams
            self._projparams = projparams
        
        def import_xml(self, rootname, dirname, ):
            edgefilepath = os.path.join(dirname,rootname+'.edg.xml')
            nodefilepath = os.path.join(dirname,rootname+'.nod.xml')
            if os.path.isfile(edgefilepath) & os.path.isfile(nodefilepath):
                self.import_sumonodes(nodefilepath)
                self.import_sumoedges(edgefilepath)
            else:
                self.get_logger().w('import_sumonodes: files not found', key='message')
                
        def import_sumonodes(self, filename, logger = None,**others):
            #print 'import_sumonodes',filename
            self.get_logger().w('import_sumonodes', key='message')
            reader = SumoNodeReader(self, **others)
            try:
                parse(filename, reader)
            except KeyError:
                print >> sys.stderr, "Please mind that the network format has changed in 0.16.0, you may need to update your network!"
                raise
        
        def import_sumoedges(self, filename, logger = None, **others):
            #print 'import_sumoedges',filename
            logger = self.get_logger()
            logger.w('import_sumoedges', key='message')
            reader = SumoEdgeReader(self, **others)
            try:
                parse(filename, reader)
                self.edges.update()
            except KeyError:
                print >> sys.stderr, "Please mind that the network format has changed in 0.16.0, you may need to update your network!"
                raise
            
        def add_node(self, **kwargs):
            return self.nodes.make(**kwargs)
            
        def add_edge(self, **kwargs):
            #print 'add_edge\n',
            #for key, value in kwargs.iteritems():
            #    print '  ',key,type(value),value
            return self.edges.make(**kwargs)
        
        def add_lane(self, **kwargs):
            #print 'add_lane\n',
            #for key, value in kwargs.iteritems():
            #    print '  ',key,type(value),value
            return self.lanes.make(**kwargs)

class SumoNodeReader(handler.ContentHandler):
    """Reads a network, storing the edge geometries, lane numbers and max. speeds"""

    def __init__(self, net, **others):
        self._net = net
        self._currentNode = None
        
        self._offset_delta = np.array([0.0,0.0])
        self._isNew = len(self._net.nodes)==0
            


    def startElement(self, name, attrs):
        #print 'startElement',name
        #if attrs.has_key('id'): print attrs['id']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #else: print '.'
        
        if name == 'nodes':
            version =self._net.get_version()
            if self._isNew |(version==attrs['version']):
                self._net.set_version(attrs['version'])
            else:
                print 'WARNING: merge with incompatible net versions %s versus %s.'%(version, attrs['version'])
             
        elif  name == 'location': # j.s
            #print 'startElement',name,self._isNew
            netOffsetStrings = attrs['netOffset'].strip().split(",")
            offset = np.array([float(netOffsetStrings[0]), float(netOffsetStrings[1])])
            offset_prev = self._net.get_offset()
            if self._isNew: 
                self._net.set_offset(offset)
                #print '  offset_prev,offset',offset_prev,offset,type(offset)
            else:
                
                self._offset_delta = offset-offset_prev
                self._net.set_offset(offset)
                #print '  offset_prev,offset,self._offset_delta',offset_prev,offset,type(offset),self._offset_delta
            
            convBoundaryStr =attrs['convBoundary'].strip().split(",")
            origBoundaryStr =attrs['origBoundary'].strip().split(",")
            #print '  convBoundaryStr',convBoundaryStr
            #print '  origBoundary',origBoundaryStr
            
            if self._isNew: 
                self._net.set_boundaries([   float(convBoundaryStr[0]), 
                                            float(convBoundaryStr[1]),
                                            float(convBoundaryStr[2]),
                                            float(convBoundaryStr[3])],
                                        [   float(origBoundaryStr[0]), 
                                            float(origBoundaryStr[1]),
                                            float(origBoundaryStr[2]),
                                            float(origBoundaryStr[3])]
                                        )
            else:
                self._net.merge_boundaries([   float(convBoundaryStr[0]), 
                                            float(convBoundaryStr[1]),
                                            float(convBoundaryStr[2]),
                                            float(convBoundaryStr[3])],
                                        [   float(origBoundaryStr[0]), 
                                            float(origBoundaryStr[1]),
                                            float(origBoundaryStr[2]),
                                            float(origBoundaryStr[3])]
                                        )
            if self._isNew: 
                if attrs.has_key('projParameter'):
                    self._net.set_projparams(attrs['projParameter'])
            else:
                if attrs.has_key('projParameter'):
                    if self._net.get_projparams()!=attrs['projParameter']:
                        print 'WARNING: merge with incompatible projections %s versus %s.'%(self._net.getprojparams(), attrs['projparams'])
                

                
        elif name == 'node':
            if attrs['id'][0]!=':':# no internal node
                x0,y0=self._offset_delta
                self._net.add_node( id_sumo = attrs['id'], 
                                    nodetype = str(attrs.get('type','priority')), 
                                    coord = [ float(attrs['x'])-x0, float(attrs['y'])-y0, float(attrs.get('z',0.0)) ],  
                                    type_tl = attrs.get('tlType','static'),
                                    radius = attrs.get('radius',1.5),
                                    is_keep_clear = attrs.get('keepClear',True))
                

class NetReader(handler.ContentHandler):
    """Reads a network, storing the edge geometries, lane numbers and max. speeds"""

    def __init__(self, **others):
        self._net = others.get('net', Network())
        self._currentEdge = None
        self._currentEdgeInt = None
        self._currentNode = None
        self._currentLane = None
        self._currentShape = ""
        self._withPhases = others.get('withPrograms', True)
        self._withConnections = others.get('withConnections', True)
        self._withFoes = others.get('withFoes', True)
        self._offset_delta = np.array([0.0,0.0])
        self._isNew = len(self._net.nodes)==0
            


    def startElement(self, name, attrs):
        #print 'startElement',name,
        #if attrs.has_key('id'): print attrs['id']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #else: print '.'
        
        if name == 'net':# j.s
            version, xmlns, xsi=self._net.getVersion()
            if self._isNew |(version==attrs['version']):
                self._net.set_version(attrs['version'],attrs['xmlns:xsi'], attrs['xsi:noNamespaceSchemaLocation'])
            else:
                print 'WARNING: merge with incompatible net versions %s versus %s.'%(version, attrs['version'])
             
        if  name == 'location': # j.s
            #print 'startElement',name
            netOffsetStrings = attrs['netOffset'].strip().split(",")
            offset = np.array([float(netOffsetStrings[0]), float(netOffsetStrings[1])])
            offset_prev = self._net.get_offset()
            if self._isNew: 
                self._net.set_offset(offset)
                #print '  offset_prev,offset',offset_prev,offset
            else:
                
                self._offset_delta = offset-offset_prev
                self._net.set_offset(offset)
                #print '  offset_prev,offset,self._offset_delta',offset_prev,offset,self._offset_delta
            
            convBoundaryStr =attrs['convBoundary'].strip().split(",")
            origBoundaryStr =attrs['origBoundary'].strip().split(",")
            #print '  convBoundaryStr',convBoundaryStr
            #print '  origBoundary',origBoundaryStr
            
            if self._isNew: 
                self._net.set_boundaries([   float(convBoundaryStr[0]), 
                                            float(convBoundaryStr[1]),
                                            float(convBoundaryStr[2]),
                                            float(convBoundaryStr[3])],
                                        [   float(origBoundaryStr[0]), 
                                            float(origBoundaryStr[1]),
                                            float(origBoundaryStr[2]),
                                            float(origBoundaryStr[3])]
                                        )
            else:
                self._net.merge_boundaries([   float(convBoundaryStr[0]), 
                                            float(convBoundaryStr[1]),
                                            float(convBoundaryStr[2]),
                                            float(convBoundaryStr[3])],
                                        [   float(origBoundaryStr[0]), 
                                            float(origBoundaryStr[1]),
                                            float(origBoundaryStr[2]),
                                            float(origBoundaryStr[3])]
                                        )
            if self._isNew: 
                if attrs.has_key('projParameter'):
                    self._net.set_projparams(attrs['projParameter'])
            else:
                if attrs.has_key('projparams'):
                    if self._net.get_projparams()!=attrs['projparams']:
                        print 'WARNING: merge with incompatible projections %s versus %s.'%(self._net.getprojparams(), attrs['projparams'])
                
        
        
                
        if name == 'edge':
            #if not attrs.has_key('function') or attrs['function'] != 'internal':
            function = attrs.get('function', '')
            if function == '':
                prio = -1
                if attrs.has_key('priority'):
                    prio = int(attrs['priority'])
                

                name = ""
                if attrs.has_key('name'):
                    name = attrs['name']
                self._currentEdge = self._net.addEdge(str(attrs['id']),
                    str(attrs['from']), str(attrs['to']), prio, function, name)
                # TODO: add spreadType in ('right','center'), width for lanes

                    
                if attrs.has_key('shape'):
                    self.processShape(self._currentEdge, attrs['shape'], offset = self._offset_delta)
                    
            #elif attrs.has_key('function') and attrs['function'] == 'internal':
            #    function = attrs['function']
            #    name = attrs.get('name','')
            #    self._currentEdgeInt =  self._net.addEdgeInternal(attrs['id'], function, name)
                
            #else:
            #    if function in ['crossing', 'walkingarea']:
            #        self._net._crossings_and_walkingAreas.add(attrs['id'])
                    
            #    self._currentEdge = None
            #    self._currentEdgeInt = None
                
        
                
        if name == 'junction':
            if attrs['id'][0]!=':':
                x0,y0=self._offset_delta
                self._currentNode = self._net.addNode(str(attrs['id']), str(attrs['type']), 
                                                        [ float(attrs['x'])-x0, float(attrs['y'])-y0 ], 
                                                        attrs['incLanes'].split(" "),
                                                        attrs.get('intLanes','').split(" "), )
        
        
        
                
        

    def characters(self, content):
        if self._currentLane!=None:
            self._currentShape = self._currentShape + content


    def endElement(self, name):
        
        if name == 'edge':
            if self._currentEdge!=None:
                if self._currentEdge._shape is None:
                    self._currentEdge.rebuildShape();
                self._currentEdge = None
            if self._currentEdgeInt!=None:
                if self._currentEdgeInt._shape is None:
                    self._currentEdgeInt.rebuildShape();
                self._currentEdgeInt = None
                
        

    def getShape(self, shapeString, offset = [0.0,0.0]):
        cshape = []
        es = shapeString.rstrip().split(" ")
        for e in es:
            p = e.split(",")
            cshape.append((float(p[0])-offset[0], float(p[1]) - offset[1]))
        return np.array(cshape)
        #object.setShape(cshape)
        #id_check = '135982482#2_0'
        #if object.getID()==id_check:
        #    print '  ',id_check,object.getShape()
   
   

       
class SumoEdgeReader(handler.ContentHandler):
    """Reads sumo edge file, storing the edge geometries, lane numbers and max. speeds"""

    def __init__(self, net, **others):
        self._net = net
        self._id_edge = -1
        #self._currentEdge = None
        #self._currentEdgeInt = None
        #self._currentLane = None
        self._currentShape = ""
        #self._withPhases = others.get('withPrograms', True)
        #self._withConnections = others.get('withConnections', True)
        #self._withFoes = others.get('withFoes', True)
        #self._offset = self._net.get_offset()
        self._offset_delta = np.array([0.0,0.0])
        #self._isNew = len(self._net.nodes)==0
            


    def startElement(self, name, attrs):
        #print 'startElement',name,
        #if attrs.has_key('id'): print attrs['id']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #else: print '.'
        

        if name == 'edge':
            #if not attrs.has_key('function') or attrs['function'] != 'internal':
            id_osmedge = attrs['id']
            self._id_edge = self._net.add_edge( id_sumo = id_osmedge, 
                                id_fromnode_sumo = str(attrs['from']),
                                id_tonode_sumo = str(attrs['to']),
                                type_edge = str(attrs.get('type','')),
                                num_lanes = int(attrs.get('numLanes',1)),
                                speed_max = float(attrs.get('speed',13.888)),
                                priority = int(attrs.get('priority',9)),
                                #length = 0.0,
                                shape = self.getShape(attrs.get('shape',''), offset = self._offset_delta),
                                type_spread = str(attrs.get('spreadType','right')),
                                name = str(attrs.get('name','')),
                                offset_end = float(attrs.get('endOffset',0.0)),
                                #width_lanes = 3.5,
                                #width_sidewalk = -1.0
                                )
                    
            
        elif name == 'lane': 
            
            if attrs.has_key('allow'):
                allow = attrs['allow'].split(' ')
            else:
                allow = None
                
            if attrs.has_key('disallow '):
                disallow  = attrs['disallow '].split(' ')
            else:
                disallow  = None
                        
            self._net.add_lane( index = ind,
                                width = float(attrs.get('width',3.5)),
                                speed_max = float(attrs.get('speed',13.888)),
                                offset_end = float(attrs.get('endOffset',0.0)),
                                mode_allow = allow,
                                mode_disallow = disallow ,
                                id_edge = self._id_edge,
                                shape  = self.getShape(attrs.get('shape',''), offset = self._offset_delta),
                            )

    #def characters(self, content):
    #    if self._currentLane!=None:
    #        self._currentShape = self._currentShape + content


    def endElement(self, name):
        
        if name == 'edge':
            self._id_edge = -1
    #        if self._currentEdge!=None:
    #            if self._currentEdge._shape is None:
    #                self._currentEdge.rebuildShape();
    #            self._currentEdge = None
    #        if self._currentEdgeInt!=None:
    #            if self._currentEdgeInt._shape is None:
    #                self._currentEdgeInt.rebuildShape();
    #            self._currentEdgeInt = None
                
        

    def getShape(self, shapeString, offset = [0.0,0.0]):
        cshape = []
        es = shapeString.rstrip().split(" ")
        for e in es:
            p = e.split(",")
            #if len(p)==2:
            #    p = c+['0.0']
            #elif 
            
            if len(p) == 2:
                # 2D coordinates with elevetion = 0
                cshape.append(np.array([float(p[0])-offset[0], float(p[1]) - offset[1], 0.0], np.float32))
            elif len(p) == 3:
                # 3D coordinates 
                
                #cshape.append(np.array([0,0,0],np.float))
                cshape.append(np.array([ float(p[0])-offset[0], float(p[1]) - offset[1], float(p[2]) ], np.float32) )
            else:
                #print 'WARNING: illshaped shape',e
                #cshape.append(np.array([0,0,0],np.float))
                return None
            
        return cshape
        #object.setShape(cshape)
        #id_check = '135982482#2_0'
        #if object.getID()==id_check:
        #    print '  ',id_check,object.getShape()





   

            
            
   
   
def import_net(filename, **others):
    netreader = NetReader(**others)
    try:
        parse(filename, netreader)
    except KeyError:
        print >> sys.stderr, "Please mind that the network format has changed in 0.16.0, you may need to update your network!"
        raise
    return netreader.getNet()

if __name__ == '__main__':          
    ###############################################################################
    #print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser 
    from agilepy.lib_base.logger import Logger
    #net = Network(logger = Logger())
    net = Network(logger = Logger())
    net.import_xml('facsp2', 'testnet')
    
    objbrowser(net)
    
    
