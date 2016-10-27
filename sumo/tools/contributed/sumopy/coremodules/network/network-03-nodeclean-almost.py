
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
    def __init__(self, parent, **kwargs):
        ident = 'modes'
        self._init_objman(ident=ident, parent=parent, name = 'Transport Modes', **kwargs)
        self.add_col(am.ArrayConf( 'names', '',
                                    dtype = np.object,
                                    perm='r', 
                                    is_index = True,
                                    name = 'Name',
                                    info = 'Name of mode. Used as key for implementing acces restrictions on edges as well as demand modelling.',
                                    ))
    def get_id_mode(self,modename):
        return self.names.get_id_from_index(modename) 
        
        
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
                                            name = 'IDs allowed', 
                                            info = 'Allowed modes on this lane.',   
                                            ))
                                        
        self.add_col(am.IdlistsArrayConf( 'modes_disallow', modes,
                                            name = 'IDs disallow', 
                                            info = 'Disallowed modes on this lane.',   
                                            ))
                                        
        
        self.add_col(am.IdsArrayConf( 'ids_mode', modes, 
                                            groupnames = ['state'], 
                                            name = 'Main mode ID', 
                                            info = 'ID of main mode of this lane.',
                                            ))
                                            
        self.add_col(am.IdsArrayConf( 'ids_edge', edges, 
                                            groupnames = ['state'], 
                                            name = 'ID edge', 
                                            info = 'ID of edge in which the lane is contained.',
                                            ))
        
        self.add_col(am.ListArrayConf( 'shapes', 
                                            #groupnames = ['_private'], 
                                            perm='rw', 
                                            name = 'Shape',
                                            unit = 'm',
                                            info = 'List of 3D Shape coordinates to describe polyline.',
                                            ))
    
    def get_edges(self):
        return self.parent.edges
    
    
    def make( self, **kwargs):
        edges = self.get_edges()
        id_edge = kwargs['id_edge']
        index = kwargs['index']
        #print 'Lanes.make',kwargs
        width = kwargs.get('width',-1)
        speed_max = kwargs.get('speed_max',-1)
        modes_allow = kwargs.get('modes_allow',[])
        
        if len(modes_allow)>0:
            id_mode = modes_allow[0] # pick first as major mode
        else:
            id_mode = -1 # no mode specified
            
        if index == 0:
            width_sidewalk_edge = edges.widths_sidewalk[id_edge]
            is_sidewalk_edge = width_sidewalk_edge>0
            is_sidewalk = (21 in modes_allow) # test for pedestrian sidewalk
        
        
        if speed_max<0:
            if index == 0:
                speed_max = 0.8 # default walk speed
            else:
                speed_max = edges.speeds_max[id_edge]
                    
        if width < 0:
            width = edges.widths_lanes_default[id_edge]
            
            if index == 0:
                if is_sidewalk_edge: # edge wants sidewalks
                    width = width_sidewalk_edge
                elif  (not is_sidewalk_edge)&is_sidewalk :# edge does not want sidewalks, but actually there is a sidewalk
                    width = 0.9# default sidewalk width
                    edges.widths_sidewalk[id_edge] = width
                
        
        # if sidewalk, then the edge attribute widths_sidewalk 
        # should be set to actual lane width in case it is less than zero
        elif index == 0: # width set for lane 0
            if  (not is_sidewalk_edge)&is_sidewalk :# edge does not want sidewalks, but actually there is a sidewalk
                edges.widths_sidewalk[id_edge] = width
                    

                    
        #if index == 0:  
        #      edges.widths_sidewalk[id_edge]= width     

                                         
        
        return self.add_row(   indexes = index,
                               widths = width,
                               speeds_max = speed_max,
                               offsets_end = kwargs.get('offset_end',None),
                               modes_allow = modes_allow,
                               modes_disallow = kwargs.get('modes_disallow',[]),
                               ids_mode = id_mode,
                               ids_edge = id_edge,
                               shapes = kwargs.get('shapes',[]), # if empty, then computation later from edge shape
                            )
    def reshape(self, id_edge):
        """
        Recalculate shape of all lanes contained in edge id_edge
        based on the shape information of this edge.
        """
        #
        #lanes = self.get_lanes()
        edges = self.parent.edges
        ids_lane = edges.ids_lane[id_edge]
        
        shape = edges.shapes[id_edge]
        #print 'reshape: edgeshape=',shape
        n_lanes = len(ids_lane)
        n_vert = len(shape)
        #print 'get_laneshapes',_id,n_lanes,n_vert
        
        #width = self.widths_lanes_default[_id]
        #print '  shape',  shape ,len(  shape)   
        v_ext_begin = (shape[0]-(shape[1]-shape[0])).reshape(1,3)
        v_ext_end = (shape[-1]+(shape[-1]-shape[-2])).reshape(1,3)
        
        exshape = np.concatenate((v_ext_begin ,shape,v_ext_end))[:,0:2]
        #print '  exshape',  exshape,len(  exshape)  
        vertex_delta_x = exshape[1:,0]-exshape[0:-1,0]
        vertex_delta_y = exshape[1:,1]-exshape[0:-1,1]
        
        angles = np.arctan2(vertex_delta_y,vertex_delta_x)
        #angles = np.mod(np.arctan2(vertex_delta_y,vertex_delta_x)+2*np.pi,2*np.pi)
        #angles_perb = 0.5 *(angles[1:]+angles[0:-1])-np.pi/2
        
        angles1 = angles[1:]
        angles2 = angles[0:-1]
        ind_discont = (angles1<-0.5*np.pi)&((angles2>0.5*np.pi)) | (angles2<-0.5*np.pi)&((angles1>0.5*np.pi)) 
        angle_sum = angles1+angles2
        angle_sum[ind_discont] += 2*np.pi
        
        #angles = np.mod(np.arctan2(vertex_delta_y,vertex_delta_x)+2*np.pi,2*np.pi)
        #angle_sum = angles[1:]+angles[0:-1]
        #ind_discont = angle_sum>2*np.pi
        #angle_sum[ind_discont] = angle_sum[ind_discont]-2*np.pi
        angles_perb = 0.5*angle_sum-np.pi/2
        
        dxn = np.cos(angles_perb)
        dyn = np.sin(angles_perb)
        
        #laneshapes = np.zeros((n_lanes,n_vert,3), np.float32)
        
        
        id_lane = ids_lane[0]
        widths = self.widths[ids_lane]#np.ones(n_lanes,np.float32)#lanes.widths[ids_lane]
        displacement =  np.cumsum(widths)
        displacement = displacement[-1]-displacement-0.5*widths+widths[-1]
        for i in range(n_lanes):
            id_lane = ids_lane[i]
            #print ' displacement[i] ',displacement[i]#,
            #print ' dxn ',dxn
            #print ' dyn ',dyn
            
            #if 1:#len(self.shapes[id_lane])==0: # make only if not existant
            laneshape = np.zeros(shape.shape, np.float32) 
            #print ' dx \n',dxn*displacement[i]
            #print ' dy \n',dyn*displacement[i]
            laneshape[:,0] = dxn*displacement[i] + shape[:,0]
            laneshape[:,1] = dyn*displacement[i] + shape[:,1]
            laneshape[:,2] = shape[:,2]
            self.shapes[id_lane] = laneshape
                    
                    
        #width = 0.5*lanes.widths[id_lane]
        #displacement = n_lanes*[[0,0.0]]
        #displacement = np.zeros(n_lanes, np.float32)
        #for id_lane in ids_lane:
        #    displacement[ lanes.indexes[id_lane]] = lanes.widths[id_lane]
            
        #displacement.sort()
        
        
        
        
        #if len(lanes.shapes[id_lane])==0: # make only if not existant
        #    laneshape[:,0] = dxn*width + shape[:,0]
        #    laneshape[:,1] = dyn*width + shape[:,1]
        #    laneshape[:,2] = shape[:,2]
        #    lanes.shapes[id_lane] = laneshape
            
        #if len(ids_lane) >1:   
        #    for id_lane in ids_lane[1:]:
        #        if len(lanes.shapes[id_lane])==0: # make only if not existant
        #            laneshape[:,0] = dxn*width + shape[:,0]
        #            laneshape[:,1] = dyn*width + shape[:,1]
        #            laneshape[:,2] = shape[:,2]
        #            lanes.shapes[id_lane] = laneshape
        #            width += lanes.widths[id_lane]
                                            
                                                
                                            
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
        if 1:
            self.add_col(am.IdlistsArrayConf( 'ids_lane', lanes,
                                            groupnames = ['state'], 
                                            name = 'IDs Lanes', 
                                            info = 'List with IDs of lanes.',   
                                            ))
    
                 
    def get_lanes(self):
        return self.parent.lanes
    
    def make( self, id_sumo = '', 
                    id_fromnode_sumo = 0,
                    id_tonode_sumo = 0,
                    type_edge = '',
                    num_lanes = 1,
                    speed_max = 50.0/3.6,
                    priority = 1,
                    #length = 0.0,
                    shape = [],
                    type_spread = 'right',
                    name = '',
                    offset_end = 0.0,
                    width_lanes_default = None,
                    width_sidewalk = -1,
                    ):
        nodes = self.ids_tonode.get_linktab()
        id_fromnode = nodes.ids_osm.get_id_from_index(id_fromnode_sumo)
        id_tonode = nodes.ids_osm.get_id_from_index(id_tonode_sumo)
        
        if len(shape) < 2: # insufficient shape data
            shape = np.array([ nodes.coords[id_fromnode], nodes.coords[id_tonode] ], np.float32)
            
        
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
                        widths_lanes_default = width_lanes_default,
                        widths_sidewalk = width_sidewalk,
                        )           
    
    
    def update(self):
        #print 'Edges.update'
        self.widths.value = self.nums_lanes.value * self.widths_lanes_default.value \
                            + (self.widths_sidewalk.value >= 0) * (self.widths_sidewalk.value-self.widths_lanes_default.value)
        
        #print '  self.widths.values =  \n',self.widths.value
        #polylines = polypoints_to_polylines(self.shapes.value)
        #print '  polylines[0:4]=\n',polylines[0:4]
        #print '  polylines[3].shape',polylines[3].shape
        self.lengths.value = get_length_polylines(polypoints_to_polylines(self.shapes.value))
    
    def update_lanes(self, id_edge, ids_lane):
        #print 'update_lanes',id_edge,self.ids_osm[id_edge] ,ids_lane,self.nums_lanes[id_edge]
        #if self._is_laneshape:
        #    laneshapes = edges.get_laneshapes(self._id_edge, )
        #    lanes.shapes[self._ids_lane[0]]
        if len(ids_lane) == 0:
            # no lanes given...make some with default values
            ids_lane = []
            lanes = self.get_lanes()
            for i in xrange(self.nums_lanes[id_edge]):
                id_lane = lanes.make(   index = i, id_edge = id_edge)
                ids_lane.append(id_lane)
        
        self.ids_lane[id_edge] = ids_lane
        
        
            
                        
    

        
        
                                                          


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
                            
        
        
        self.add_col(am.ArrayConf( 'radii',  5.0, 
                                    dtype=np.float32,
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    name = 'Radius',
                                    info = 'Node radius',
                                    ))
                                    
        self.add(cm.AttrConf(  'radius_default', 3.0, 
                                        groupnames = ['options'],
                                        perm='wr', 
                                        unit = 'm',
                                        name = 'Default radius', 
                                        info = 'Default node radius.',
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
                                                    
        self.add_col(am.ArrayConf( 'turnradii',  1.5,
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
                turnradius = 1.5, is_keep_clear = True):
                 
        return self.add_row( ids_osm = id_sumo, 
                types = self.types.choices[nodetype], 
                coords = coord, 
                types_tl = self.types_tl.choices[type_tl],
                ids_tl_prog = id_tl_prog,
                turnradii = turnradius,
                are_keep_clear = is_keep_clear,
                )
    # def clean_node(self, id_node):          
    
    def clean(self):
        print 'Nodes.clean',len(self)
        
        edges = self.parent.edges
        lanes = self.parent.lanes
        rad_min = self.radius_default.value
    
        for id_node in self.get_ids():
            print 79*'_'
            print '  node',id_node
            ind_node = self.get_inds(id_node)
            #if self.ids_osm[id_node]=="1657715091":
            #    print '\n\n\n',79*'='
            #    print '   coords',self.coords[id_node]
            #    print '   radii',self.radii[id_node]
            
            # distanza ad altri nodi
            #d = np.sum(np.abs(self.coords[id_node]-self.coords.value),1)
            #d = np.linalg.norm(self.coords[id_node]-self.coords.value,1)
            coords = self.coords[id_node]
            d = get_norm_2d(coords-self.coords.value)
            d[ind_node]=np.inf
            d_min = np.min(d)
            #print '  d_min',d_min
            ids_edge_out = edges.select_ids(edges.ids_fromnode.value == id_node)
            ids_edge_in = edges.select_ids(edges.ids_tonode.value == id_node)
            
            # estimate circumference of junction and determine node radius
            n_edges = len(ids_edge_in) + len(ids_edge_out)
            width_av = np.mean( np.concatenate( (edges.widths[ids_edge_in], edges.widths[ids_edge_out])) )
            
            # here we assume a node with 6 entrance sides and a and 2 average width edges per side
            #circum = 2.0*max(6,n_edges)*width_av
            circum = 2.2*max(2,n_edges)*width_av
            
            #print '  n_edges,width_av,radius',n_edges,width_av,max(6,n_edges)*width_av/(2*np.pi)
            radius = min( max(circum/(2*np.pi), rad_min), 0.4*d_min)
            self.radii[id_node] = radius
            
            for id_edge in ids_edge_in:
                print '    in edge',id_edge
                shape = edges.shapes[id_edge]
                n_shape = len(shape)
                # edges.shapes[id_edge][::-1]:
                for i in xrange(n_shape-1,-1,-1):
                    d = get_norm_2d(np.array([shape[i]-coords]))[0]
                    #print '      i,d,r',i , d, radius,d>radius
                    if d>radius:
                        print '        **',i,d, radius
                        break
                
                if i==n_shape-1:  
                    x,y = shape[-1][:2]
                    dx,dy = coords[:2] - shape[-1][:2]
                    dn = np.sqrt(dx*dx + dy*dy)
                    x1 = x + (d-radius)*dx/dn
                    y1 = y + (d-radius)*dy/dn
                    shape[-1][:2]= [x1,y1]
                    edges.shapes[id_edge] = shape
                    
                else:#elif i>0:
                    x,y = shape[i][:2]
                    #print 'shape',shape,
                    #dx,dy = shape[i+1][:2] - shape[i][:2]
                    dx,dy =coords[:2] - shape[i][:2]
                    dn = np.sqrt(dx*dx + dy*dy)
                    x1 = x + (d-radius)*dx/dn
                    y1 = y + (d-radius)*dy/dn
                    shape[i+1][:2]= [x1,y1]
                    edges.shapes[id_edge] = shape[:i+2]
                print '    x,y',x,y
                print '    x1,y1',x1,y1 
                #print '  shape[:i+2]',shape[:i+2]
                print '  shapes[id_edge]',edges.shapes[id_edge] 
                lanes.reshape(id_edge)
            
            for id_edge in ids_edge_out:
                print '    out edge',id_edge
                shape = edges.shapes[id_edge]
                n_shape = len(shape)
                # edges.shapes[id_edge][::-1]:
                for i in xrange(n_shape):
                    d = get_norm_2d(np.array([shape[i]-coords]))[0]
                    #print '      i,d,r',i , d, radius,d>radius
                    if d>radius:
                        print '        **',i,d, radius
                        break
                
                if i==0:  
                    x,y = coords[:2] 
                    dx,dy = shape[0][:2] - coords[:2] 
                    dn = np.sqrt(dx*dx + dy*dy)
                    x1 = x + (radius)*dx/dn
                    y1 = y + (radius)*dy/dn
                    shape[0][:2]= [x1,y1]
                    edges.shapes[id_edge] = shape
                    
                elif i<n_shape:
                    x,y = coords[:2] #shape[i-1][:2]
                    #print 'shape',shape,
                    #dx,dy = shape[i][:2]- shape[i-1][:2] 
                    dx,dy = shape[i][:2] - coords[:2] 
                    dn = np.sqrt(dx*dx + dy*dy)
                    x1 = x + (radius)*dx/dn
                    y1 = y + (radius)*dy/dn
                    shape[i-1][:2]= [x1,y1]
                    edges.shapes[id_edge] = shape[i-1:]
                print '    x,y',x,y
                print '    x1,y1',x1,y1 
                #print '  shape[:i+2]',shape[:i+2]
                print '  shapes[id_edge]',edges.shapes[id_edge]
                lanes.reshape(id_edge)  
                                        

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
            self.lanes = attrsman.add(   cm.ObjConf( Lanes(self, self.edges, self.modes) ) )
            
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
                self.nodes.clean()
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
            
        def get_id_mode(self,modename):
            return self.modes.get_id_mode(modename) 
           
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
                                    turnradius = attrs.get('radius',1.5),
                                    is_keep_clear = attrs.get('keepClear',True))
                


       
class SumoEdgeReader(handler.ContentHandler):
    """Reads sumo edge file, storing the edge geometries, lane numbers and max. speeds"""

    def __init__(self, net, **others):
        self._net = net
        self._id_edge = -1
        self._ids_lane = []
        self._modenames = net.modes.names
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
        #print 'startElement',name
        #if attrs.has_key('id'): print attrs['id']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        #else: print '.'
        

        if name == 'edge':
            #if not attrs.has_key('function') or attrs['function'] != 'internal':
            self._id_edge = self._net.add_edge( id_sumo = attrs['id'], 
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
                                #width = float(attrs.get('width ', None)),
                                #width_sidewalk = float(attrs.get('sidewalkWidth',None)),
                                width_lanes_default = float(attrs.get('width ', 3.0)),
                                width_sidewalk = float(attrs.get('sidewalkWidth',-1.0)),
                                )
            #self._is_laneshape = True
                    
            
        elif name == 'lane': 
            
            if attrs.has_key('allow'):
                allow = list(self._modenames.get_ids_from_indices(attrs['allow'].split(' ')))
            else:
                allow = []
                
            if attrs.has_key('disallow'):
                disallow  = list(self._modenames.get_ids_from_indices(attrs['disallow'].split(' ')))
            else:
                disallow  = []
            
            
                    
            id_lane = self._net.add_lane( index = float(attrs.get('index',-1)),
                                width = float(attrs.get('width',-1)),
                                speed_max = float(attrs.get('speed',-1)),
                                offset_end = float(attrs.get('endOffset',0.0)),
                                modes_allow = allow,
                                modes_disallow = disallow ,
                                id_edge = self._id_edge,
                                shapes  = self.getShape(attrs.get('shape',''), offset = self._offset_delta) ,
                            )
            self._ids_lane.append(id_lane)

    #def characters(self, content):
    #    if self._currentLane!=None:
    #        self._currentShape = self._currentShape + content

    
        
    def endElement(self, name):
        edges = self._net.edges
        lanes = self._net.lanes
        if name == 'edge':
            edges.update_lanes(self._id_edge, self._ids_lane)
            self._id_edge = -1
            self._ids_lane = []
            
            
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
                return np.zeros((0,2),np.float32)#None
            
        return np.array(cshape, np.float32) 
        #object.setShape(cshape)
        #id_check = '135982482#2_0'
        #if object.getID()==id_check:
        #    print '  ',id_check,object.getShape()





   

            
            
   
   


if __name__ == '__main__':          
    ###############################################################################
    #print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser 
    from agilepy.lib_base.logger import Logger
    #net = Network(logger = Logger())
    net = Network(logger = Logger())
    net.import_xml('facsp2', 'testnet')
    
    objbrowser(net)
    
    
