
from xml.sax import saxutils, parse, handler
import os, sys
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.processes import Process
from  agilepy.lib_base.geometry import is_polyline_intersect_polygon
from collections import Counter
from agilepy.lib_base.geometry import *
try:
    import pyproj
except:
    from mpl_toolkits.basemap import pyproj
    
from .matsim_base import  *
pathsep = os.path.sep
#from coremodules.network.network import SumoIdsConf

                          
duration_last_activity = 3*3600 # this is arbitrary, but does not have any effect on the simuation

class Network(cm.BaseObjman):
        def __init__(self, parent=None, name = 'Network', **kwargs):
            print ('Matsim Network.__init__',parent,name)
            self._init_objman(  ident= 'net', parent=parent, name = name,
                                #xmltag = 'net',# no, done by netconvert
                                version = 0.1,
                                **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            self._init_attributes()

        def _init_attributes(self):
            attrsman = self.get_attrsman()
            self.projparams = attrsman.add(cm.AttrConf( 'projparams','epsg:31468',
                            groupnames = ['options'], 
                            name = 'proj. param', 
                            info = "MATSIM network projection parameters for example 'epsg:31468'. If string is empty then LAT/LON coordinates are assumed.",
                            ))
            self.projmode = attrsman.add(cm.AttrConf( 'projmode','no projection',
                            groupnames = ['options'], 
                            name = 'projmode', 
                            info = "Definition, whether the MATSim Network was converted to SUMO coordinates or not.",
                            ))             
            #self.modes = attrsman.add(   cm.ObjConf( Modes(self) ) )
            self.nodes = attrsman.add(   cm.ObjConf( Nodes(self) ) )
            self.edges = attrsman.add(   cm.ObjConf( Edges(self) ) )
            #self.edges.set_nodes(self.nodes)     

        
            
class Nodes(am.ArrayObjman):
    def __init__(   self, parent,
                    **kwargs):
        ident = 'nodes'
        self._init_objman( ident=ident, parent=parent, name = 'Nodes',
                            xmltag = ('nodes','node','ids_matsim'),
                            version = 0.0,
                            info = 'MATSIM network nodes.',
                            **kwargs)
        self._init_attributes()

    def _init_attributes(self):


        self.add_col(am.ArrayConf('ids_matsim', default = '',
                                dtype = object,
                                perm = 'r',
                                is_index = True,
                                name = 'Node ID',
                                info = 'Matsim Node ID',
                                xmltag = 'id',
                                ))
                                
        self.add_col(am.ArrayConf( 'coords',  np.zeros(3, dtype=np.float64),
                            dtype = np.float64,
                            groupnames = ['state'],
                            perm = 'r',
                            name = 'Coords',
                            unit = 'm',
                            info = 'Node center coordinates.',
                            ))



        self.add_col(am.ArrayConf( 'radii',  5.0,
                                    dtype=np.float32,
                                    groupnames = ['state'],
                                    perm='rw',
                                    name = 'Radius',
                                    unit = 'm',
                                    info = 'Node radius',
                                    ))
    
    def get_closest(self, coord, n = 1):
        """
        Returns the closest prt stop for each coord in coords vector.
        """

        
        coords_stop = self.coords.value
        ids = self.get_ids()
        #inds_closest = np.zeros(n,dtype=np.int32)
        
        distances = np.sum((coord - coords_stop)**2, axis=1)
        if n == 1:
            ind_closest = np.argmin(distances)
        else:
            ind_closest = np.argsort(distances)[:n]
        #inds_closest[i] = ind_closest
        distances_closest = np.sqrt(distances[ind_closest])
        ids_node_closest = ids[ind_closest]
        #ids_edge_closest = ids_edge_stop[inds_closest]
        
        return ids_node_closest,distances_closest                               
                                    
class Edges(am.ArrayObjman):
    def __init__(self, parent, **kwargs):
            ident = 'edges'
            self._init_objman(ident=ident, parent=parent,
                                name = 'Edges',
                                xmltag = ('links','link','ids_matsim'),
                                version = 0.2,
                                **kwargs)

            self._init_attributes()
            self._init_constants()

    def _init_attributes(self):


        # <link id="39530" from="27590690" to="20980545" 
        # length="163.0601556628046" 
        # freespeed="8.333333333333334" 
        # capacity="600.0" permlanes="1.0" 
        # oneway="1" modes="car" />
        self.add_col(am.ArrayConf('ids_matsim', default = '',
                                dtype = object,
                                perm = 'r',
                                is_index = True,
                                name = 'Link ID',
                                info = 'Matsim Link ID',
                                xmltag = 'id',
                                ))


        
        
        # set ref to nodes table, once initialized
        self.add_col(am.IdsArrayConf( 'ids_fromnode', self.parent.nodes,
                                            groupnames = ['state'],
                                            name = 'ID from-node',
                                            info = 'ID of node at the beginning of the link.',
                                            xmltag = 'from',
                                            ))


        self.add_col(am.IdsArrayConf( 'ids_tonode', self.parent.nodes,
                                            groupnames = ['state'],
                                            name = 'ID to-node',
                                            info = 'ID of node at the end of the link.',
                                            xmltag = 'to',
                                            ))
        
        self.add_col(am.ArrayConf( 'types', '',
                                            dtype = object,
                                            perm='rw',
                                            name = 'Type',
                                            info = 'Edge reference MATSim type.',
                                            xmltag = 'type', # should not be exported?
                                            ))
                                            
        self.add_col(am.NumArrayConf('lengths', 0.0,
                                        dtype = np.float32,
                                        groupnames = ['state'],
                                        perm = 'r',
                                        name = 'Length',
                                        unit = 'm',
                                        info = 'Link length.',
                                        xmltag = 'length ',
                                        ))
        
        self.add_col(am.NumArrayConf('speeds_max', 50.0/3.6,
                                        dtype = np.float32,
                                        groupnames = ['state'],
                                        perm = 'rw',
                                        name = 'Max speed',
                                        unit = 'm/s',
                                        info = 'Maximum speed on edge.',
                                        xmltag = 'freespeed',
                                        ))

                                 
        self.add_col(am.ArrayConf( 'nums_lane', 1.0,
                                        dtype = np.float32,
                                        perm='rw',
                                        name = '# of lanes',
                                        info = 'Number of lanes.',
                                        xmltag = 'permlanes',
                                        ))
                                        
        self.add_col(am.ArrayConf( 'capacities', 0.0,
                                        dtype = np.float32,
                                        perm='rw',
                                        name = 'capacity',
                                        info = 'Carrying capacity in vehicles per hour.',
                                        unit = 'vphpd',
                                        xmltag = 'capacity',
                                        ))
        
        self.add_col(am.ArrayConf('modes_matsim', default = '',
                                dtype = object,
                                perm = 'r',
                                is_index = True,
                                name = 'Modes Matsim',
                                info = 'Modes specified in MATSim',
                                xmltag = 'id',
                                ))
        
        
                                                                    
        self.add_col(am.ListArrayConf( 'shapes',
                                        groupnames = ['_private'],
                                        perm='rw',
                                        name = 'Shape',
                                        unit = 'm',
                                        info = 'Link shape as list of 3D shape coordinates representing a polyline.',
                                        is_plugin = True,
                                        xmltag = 'shape',
                                        ))
    def _init_constants(self):
        self._segvertices = None
        self._edgeinds = None
        self._seginds = None
        self._segvertexinds = None
        self.clear_cache()
        self.do_not_save_attrs(['_segvertices','_edgeinds','_seginds','_segvertexinds'])
    
    def clear_cache(self):
        self._timesmap = {}
        self._fstarmap = {}
        self.do_not_save_attrs(['_timesmap','_fstarmap'])

    def get_times(self,  speed_max = 50, ids_edge = None ):
        """
        eturns freeflow travel times for all edges
        The returned array represents the travel time that corresponds to
        edge IDs.

        If is_check_lanes is True, then the lane speeds are considered where
        the respective mode is allowed.

        If not allowed on a particular edge,
        then the respective edge travel time is nan.

        modeconst_excl and modeconst_mix are constants added to the
        time if the respective edge provides exclusive or reserver mixed
        access for the specifird mode

        """
        #print 'get_times id_mode,is_check_lanes,speed_max',id_mode,is_check_lanes,speed_max,'is_precise',is_precise
        if ids_edge is None:
            ids_edge = self.get_ids()
                
        times = np.zeros(np.max(ids_edge)+1, np.float32)
        speeds = self.speeds_max[ids_edge]

        if speed_max is not None:
            speeds = np.clip(speeds, 0.0, speed_max)
            
        times[ids_edge] = self.lengths[ids_edge]/speeds
            


        return times
    
    def get_fstar(self, is_return_lists = False, is_return_arrays = False):
        """
        Returns the forward star graph of the network as dictionary:
            fstar[id_edge] = set([id_toedge1, id_toedge2,...])

            if is_return_lists = True then a list of edges is the value
            of fstar

            if is_return_arrays = True then a numpy array of edges is the value
            of fstar

            if is_ignor_connections = True then all possible successive edges
            are considered, disregarding the actual connections

        """
        #print 'get_fstar id_mode',id_mode,'is_return_lists',is_return_lists,'is_return_arrays',is_return_arrays
        #ids_edge = self.get_ids()
        #fstar = np.array(np.zeros(np.max(ids_edge)+1, np.obj))
        fstar = {}

        # here we ignore connections and look at the
        # outgoing edges of node database
        # not even mode is checked
        #ids_outgoing = self.parent.nodes.ids_outgoing
        ids = self.get_ids()
        for id_edge, id_tonode in zip(ids,self.ids_tonode[ids]):
            #ids_edge_outgoing = ids_outgoing[id_tonode]
            ids_edge_outgoing = self.select_ids(self.ids_fromnode.value == id_tonode)
            if ids_edge_outgoing is not None:
                fstar[id_edge] = set(ids_edge_outgoing)
            else:
                fstar[id_edge] = set()
                    
            #else:# actually the get_times or get_distance method should eliminate not accessible edges
                # check also if id_mode has access at both ends of each connection
                # ids_lane = lanes.get_ids()
                # has_access = np.zeros(np.max(ids_lane)+1, dtype = bool)
                # ids_fromedge = list(set(lanes.ids_edge[ids_lane[lanes.get_accesslevel_lanes(ids_lane, id_mode) > -1]]))
                # for id_edge, id_tonode in zip(ids_fromedge,self.ids_tonode[ids_fromedge]):
                #    ids_edge_outgoing = ids_outgoing[id_tonode]
                #    if ids_edge_outgoing is not None:
                #        fstar[id_edge] = set()
                #        for id_edge in ids_edge_outgoing
                #            if 
                #            fstar[id_edge].add(id_toedge)
                #    else:
                #        fstar[id_edge] = set()
                
        if  is_return_lists|is_return_arrays:
            for id_edge in self.get_ids():
                ids_toedges = list(fstar[id_edge])
                if is_return_arrays:
                    fstar[id_edge] = np.array(ids_toedges, dtype = np.int32)
                else:
                    fstar[id_edge] = ids_toedges
        #print '  fstar',fstar
        return fstar
    
    def get_bstar(self, is_return_lists = False, is_return_arrays = False,
                        is_ignor_connections = False, id_mode = None):
        """
        Returns the backward star graph of the network as dictionary:
            bstar[id_edge] = set([id_fromedge1, id_fromedge2,...])

            if is_return_lists = True then a list of edges is the value
            of bstar

            if is_return_arrays = True then a numpy array of edges is the value
            of bstar

            if is_ignor_connections = True then all possible preceding edges
            are considered, disregarding the actual connections

        """
        #ids_edge = self.get_ids()
        #fstar = np.array(np.zeros(np.max(ids_edge)+1, np.obj))
        bstar = {}
        
        # here we ignore connections and look at the
        # outgoing edges of node database

        #ids_incoming = self.parent.nodes.ids_incoming
        ids = self.get_ids()
        for id_edge, id_fromnode in zip(ids,self.ids_fromnode[ids]):
            ids_edge_incoming = self.select_ids(self.ids_tonode.value == id_fromnode) # ids_incoming[id_fromnode] 
            if ids_edge_incoming is not None:
                bstar[id_edge] = set(ids_edge_incoming)
            else:
                bstar[id_edge] = set()

        if  is_return_lists|is_return_arrays:
            for id_edge in self.get_ids():
                ids_fromedges = list(bstar[id_edge])
                if is_return_arrays:
                    # the intermediate step set-> list->array is required!
                    bstar[id_edge] = np.array(ids_fromedges, dtype = np.int32)
                else:
                    bstar[id_edge] = ids_fromedges
        return bstar
    
    def make_segment_edge_map(self, ids = None, is_laneshapes = False):
        """
        Generates a vertex matrix with line segments of all edges
        and a map that maps each line segment to edge index.
        """
        # TODO: _seginds not correctly constructed for given ids

        # here we can make some selection on edge inds
        if ids is None:
            inds = self.get_inds()
        else:
            inds = self.get_inds(ids)
        print('make_linevertices',len(inds))

        linevertices = np.zeros((0,2,3),np.float32)
        vertexinds = np.zeros((0,2),np.int32)
        polyinds = []



        lineinds = []
        #linecolors = []
        #linecolors_highl = []
        linebeginstyles = []
        lineendstyles = []


        i = 0
        ind_line = 0

        if is_laneshapes:
            ids_lanes = self.ids_lanes.get_value()
            laneshapes = self.parent.lanes.shapes
        else:
            polylines = self.shapes.get_value()#[inds]



        #polylines = self.shapes[inds]
        #print '  len(polylines)',len(polylines)
        for ind in inds:

            if is_laneshapes:
                polyline = laneshapes[ids_lanes[ind][0]]
            else:
                polyline = polylines[ind]

            n_seg = len(polyline)
            #print '  =======',n_seg#,polyline


            if n_seg >1:
                polyvinds = list(range(n_seg))
                #print '  polyvinds\n',polyvinds
                vi = np.zeros(  ( 2*n_seg-2),np.int32)
                vi[0]=polyvinds[0]
                vi[-1]=polyvinds[-1]

                # Important type conversion!!
                v = np.zeros(  ( 2*n_seg-2,3),np.float32)
                v[0]=polyline[0]
                v[-1]=polyline[-1]
                if len(v)>2:

                    #print 'v[1:-1]',v[1:-1]
                    #print 'v=\n',v
                    #m = np.repeat(polyline[1:-1],2,0)
                    #print 'm\n',m,m.shape,m.dtype
                    #v[1:-1] = m
                    v[1:-1] = np.repeat(polyline[1:-1],2,0)
                    vi[1:-1] = np.repeat(polyvinds[1:-1],2)
                #vadd = v.reshape((-1,2,3))
                #print '  v\n',v
                #print '  vi\n',vi

                n_lines = int(len(v)/2)
                #print '  v\n',v
                polyinds += n_lines*[ind]
                lineinds.append(np.arange(ind_line,ind_line+n_lines))
                ind_line += n_lines
                #print '  polyinds\n',polyinds,n_lines
                #linecolors += n_lines*[colors[ind]]
                #linecolors_highl += n_lines*[colors_highl[ind]]

                #print '  linebeginstyle',linebeginstyle,beginstyles[ind]


            else:
                # empty polygon treatment
                v = np.zeros(  ( 0,3),np.float32)
                vi = np.zeros(  ( 0),np.int32)


            linevertices = np.concatenate((linevertices, v.reshape((-1,2,3))))
            vertexinds = np.concatenate((vertexinds, vi.reshape((-1,2))))
            #print '  linevertex\n',linevertices
            i +=1
        self._segvertices = linevertices

        self._edgeinds = np.array(polyinds, np.int32)
        self._seginds = lineinds
        self._segvertexinds = np.array(vertexinds, np.int32)

    def get_inds_seg_from_id_edge(self, id_edge):
        #print 'get_inds_seg_from_id_edge id_edge, ind_edge',id_edge,self.get_ind(id_edge)
        return self._seginds[self.get_ind(id_edge)]
    
    def get_dist_point_to_edge(self, p, id_edge,
                                is_ending = True,
                                is_detect_initial = False,
                                is_detect_final = False,
                                is_return_segment = False):

        """
        Returns eucledian distance from a point p to a given edge.
        As a second argument it returns the coordinates of the
        line segment (x1,y1,x2,y2) which is closest to the point.
        
        If is_ending is True, the end of segment detection is enabled:
    
        If is_detect_initial is True then a point whose projection is beyond
        the start of the segment will result in a NaN distance.
        
        If is_detect_final is True then a point whose projection is beyond
        the end of the segment will result in a NaN distance.
        
        """
        inds_seg = self.get_inds_seg_from_id_edge(id_edge)
        vertices = self._segvertices
        x1 = vertices[inds_seg,0,0]
        y1 = vertices[inds_seg,0,1]

        x2 = vertices[inds_seg,1,0]
        y2 = vertices[inds_seg,1,1]

        dists2 = get_dist_point_to_segs(    p[0:2],x1,y1,x2,y2,
                                        is_ending = is_ending,
                                        is_detect_initial = is_detect_initial,
                                        is_detect_final = is_detect_final
                                        )
        if is_detect_final|is_detect_initial:
            are_finals = np.isnan(dists2)
            #print '  dists2',dists2
            #print '  are_finals',are_finals
            if np.all(are_finals):# point outside all segments of edge
                if is_return_segment:
                    return np.nan, [np.nan,np.nan,np.nan,np.nan]
                else:
                    return np.nan
            else:
                dists2[are_finals] = np.inf




        ind_min = np.argmin(dists2)
        if is_return_segment:
            return  np.sqrt(dists2[ind_min]), (x1[ind_min],y1[ind_min],x2[ind_min],y2[ind_min])
        else:
            return  np.sqrt(dists2[ind_min])

    def get_closest_edge(self,p, is_get2 = False, n_best = 0, d_max = np.inf,
                        is_ending=True, is_detect_initial = False, 
                        is_detect_final = False,
                        accesslevels = None):
        """
        Returns edge id which is closest to point p.
        Requires execution of make_segment_edge_map
        """
        #print 'get_closest_edge'#,p
        if len(self)==0:
            return np.array([],int)

        if self._segvertices is None:
            self.make_segment_edge_map()

        vertices = self._segvertices
        x1 = vertices[:,0,0]
        y1 = vertices[:,0,1]

        x2 = vertices[:,1,0]
        y2 = vertices[:,1,1]

        #print '  type(x1)',type(x1[0]),'type(p)',type(p[0])
        #print '  x1', x1
        #print '  x2', x2
        #halfwidths = 0.5*self.get_widths_array()[self._polyinds]
        d2 = get_dist_point_to_segs(p[0:2],x1,y1,x2,y2, is_ending=is_ending,
                                       is_detect_initial = is_detect_initial, 
                                       is_detect_final = is_detect_final)
        
        if is_detect_initial | is_detect_final | (not np.isinf(d_max)):
            # there may be nans if not projecting, replace by inf
            #inds_valid = np.logical_not(np.isnan(d2)) 
            
            #print '  ids_edge in range',set(self.get_ids(self._edgeinds[d2 < d_max**2]).tolist())
            #if 0:
            #    ind_range = d2 < d_max**2
            #    l=[]
            #    for d, id_edge in zip(np.sqrt(d2[ind_range]), self.get_ids(self._edgeinds[ind_range]) ):
            #        l.append((d, id_edge))
            #    l.sort()
            #    for  d, id_edge in l:
            #        print '    d',d,'id_edge',id_edge
                
            d2[d2 > d_max**2] = np.inf
            d2[np.isnan(d2)] = np.inf
            
            
            
            #inds_valid = ( d2<d_max**2 ) & np.logical_not(np.isnan(d2))
            #inds_valid = inds_valid[ np.flatnonzero( np.logical_not(np.isnan(d2[inds_valid]) ]
            inds_valid = np.logical_not(np.isinf(d2))
            #print '  inds_valid', np.flatnonzero(inds_valid)
            #print '  dists',np.sqrt(d2[inds_valid])
            #print '  ids_edge projecting:',set(self.get_ids(self._edgeinds[inds_valid]).tolist())
            #d2[np.isnan(d2)] = np.inf
            d2[np.logical_not(inds_valid)] = np.inf
        
        #print '  min(d)=',np.sqrt(np.min(d2)),'argmin=',np.argmin(d2),'id_edge=',self.get_ids(self._edgeinds[np.argmin(d2)])
            
        if is_get2:
            # return 2 best matches
            ind1 = np.argmin(d2)
            id_edge1 = self.get_ids(self._edgeinds[ind1])
            d2[ind1] = np.inf
            id_edge2 = self.get_ids(self._edgeinds[np.argmin(d2)])
            return [id_edge1, id_edge2]
        
        elif n_best>0:
            ind = np.argmin(d2)
            dist2_min = d2[ind]
            if not np.isinf(dist2_min):
                dists2_min = [dist2_min]
                id_edge_min = self.get_ids(self._edgeinds[ind])
                
                if accesslevels is not None:
                    #print '  id_edge_min',id_edge_min,'dists_min',np.sqrt(dists2_min),accesslevels[id_edge_min]
                    if accesslevels[id_edge_min]>-1:
                        ids_min = [id_edge_min]
                    else:
                        ids_min = []
                else:
                    ids_min = [id_edge_min]
            else:
                #print '  dist2_min',dist2_min,np.isinf(dist2_min)
                return [],[]
            
            
            #print '  min(d)=',np.sqrt(np.min(d2)),'argmin=',np.argmin(d2),'id_edge=',self.get_ids(self._edgeinds[np.argmin(d2)])
            is_cont = True
            while is_cont:
                d2[ind] = np.inf
                ind = np.argmin(d2)
                dist2_min = d2[ind]
                id_edge = self.get_ids(self._edgeinds[ind])
                #print '  check id_edge',id_edge,dist2_min,type(dist2_min)
                if np.isinf(dist2_min):
                    is_cont = False
                elif id_edge not in ids_min:
                    #print '  min(d)=',np.sqrt(np.min(d2)),'argmin=',np.argmin(d2),'id_edge=',self.get_ids(self._edgeinds[np.argmin(d2)])
                    #dist2_min = d2[ind]
                    if accesslevels is not None:
                        #print '  id_edge_min',id_edge,'dists_min',np.sqrt(dists2_min),accesslevels[id_edge]
                        if accesslevels[id_edge]>-1:
                            dists2_min.append(dist2_min)
                            ids_min.append(id_edge)
                    else:
                        dists2_min.append(dist2_min)
                        ids_min.append(id_edge)
                
                if  len(dists2_min) ==  n_best:
                     is_cont = False  
            #print '  found ids_edge',ids_min
            return ids_min, np.sqrt(dists2_min)
        
        else:
            return self.get_ids(self._edgeinds[np.argmin(d2)])

class NetCounter(ParserMixin,handler.ContentHandler):
    """Reads Matsim net xml file and counts links and nodes.
    """
    def __init__(self,  _matsim, projparams_matsim = '', projectionmode = ''):
        self._matsim = _matsim
        
        self.n_node = 0
        self.n_edge = 0
       
    def startElement(self, name, attrs):
        #print ('startElement',name,)
        
        
        if name == 'node':
            self.n_node +=1
            
        if name == 'link':
            self.n_edge += 1
           
        
                
class NetParser(ParserMixin,handler.ContentHandler):
    """Reads Matsim net xml file and puts all the data in a node 
    and an edge dictionary.
    Coordinates are transformed in the SUMO coordinate system.
    """
    def __init__(self,  _matsim, netcounter, projparams_matsim = '', projectionmode = ''):
        self._matsim = _matsim
        self._init_projection(projparams_matsim, projectionmode)
        self.n_node= netcounter.n_node
        self.n_edge= netcounter.n_edge
        
        self.ids_node_matsim = np.zeros(self.n_node, dtype = object)
        self.coords_matsim = np.zeros((self.n_node,2), dtype = np.float64)
        
        self.ids_edge_matsim = np.zeros(self.n_edge, dtype = object)
        self.ids_fromnode_matsim = np.zeros(self.n_edge, dtype = object)
        self.ids_tonode_matsim = np.zeros(self.n_edge, dtype = object)
        self.lengths = np.zeros(self.n_edge, dtype = np.float32)
        self.speeds_max = np.zeros(self.n_edge, dtype = np.float32)
        self.capacities = np.zeros(self.n_edge, dtype = np.float32)
        self.nums_lane = np.zeros(self.n_edge, dtype = np.float32)
        self.modes = np.zeros(self.n_edge, dtype = object)
        self.types = np.zeros(self.n_edge, dtype = object)

        self.ind_node = 0
        self.ind_edge = 0
    
    def get_coords_nodes_sumo(self):
        coords_sumo = np.zeros((self.n_node,3), dtype = np.float64)
        coords_sumo[:,0], coords_sumo[:,1]=  self.get_coord_sumo(self.coords_matsim[:,0],self.coords_matsim[:,1])
        return coords_sumo
        
    def startElement(self, name, attrs):
        #print ('startElement',name,)
        
        
        if name == 'node':
            # <node id="1002798313" x="2119.952732461621" y="298.94399969186634" 
            self.ids_node_matsim[self.ind_node] = attrs['id']
            self.coords_matsim[self.ind_node,:2]  = (attrs['x'],attrs['y'])
            self.ind_node +=1
            #print ('  node ID',attrs['id'],'x,y',self.nodes[attrs['id']])
        
        if name == 'link':
            # <link id="99714" from="513855541" to="564135" 
            # print ('  link ID',attrs['id'])
            #self.edges[attrs['id']] = (attrs['from'],attrs['to'],attrs['length'],
            #      attrs['freespeed'],attrs['capacity'],float(attrs['permlanes']),attrs['oneway'],attrs['modes'])
            
            self.ids_edge_matsim[self.ind_edge] = attrs['id']
            self.ids_fromnode_matsim[self.ind_edge] = attrs['from']
            self.ids_tonode_matsim[self.ind_edge] = attrs['to']
            self.lengths[self.ind_edge] = attrs['length']
            self.speeds_max[self.ind_edge]= attrs['freespeed']
            self.capacities[self.ind_edge] = attrs['capacity']
            self.nums_lane[self.ind_edge] = float(attrs['permlanes'])
            if 'modes' in attrs:
                self.modes[self.ind_edge] = attrs['modes']
                
                if "car" in attrs['modes']:
                    self.types[self.ind_edge] = "car"
                elif "bus" in attrs['modes']:
                    self.types[self.ind_edge] = "bus"
                elif "rail" in attrs['modes']:
                    self.types[self.ind_edge] = "rail"
                elif "tram" in attrs['modes']:
                    self.types[self.ind_edge] = "tram" 
                else:
                    self.types[self.ind_edge] = "" 
            else:
                self.modes[self.ind_edge] = "car"
                self.types[self.ind_edge] = "car"
                
            self.ind_edge += 1
        
   
            
class NetImporter(MatsimProcessMixin, Process):
    def __init__(self,  matsim, netfilepath = '',
                        logger = None, **kwargs):
        #print ('NetImporter.__init__',matsim)
        self._init_common(  'netimporter', name = 'MATSIM net importer', 
                            logger = logger,
                            info ='Imports MATSIM network XML file into network database.',
                            )
        self._scenario = matsim.get_scenario()
        self._matsim = matsim
        self._net = matsim.net
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()
                
        attrsman = self.set_attrsman(cm.Attrsman(self))

        if netfilepath  is None:
            netfilepath =os.path.join(rootdirpath,rootname+'.net.xml')
        
        self.netfilepath = attrsman.add(cm.AttrConf('netfilepath',kwargs.get('netfilepath',netfilepath),
                        groupnames = ['options'],# 
                        name = 'Matsim Net file', 
                        wildcards = 'Net XML file (*.xml)|*.xml*',
                        metatype = 'filepath',
                        info = 'MATSIM network file to be imported.',
                        ))
                                        
        
        self.init_proj_attributes(**kwargs)
                         
                            
    def do(self):
         
        self._net.projparams = self.projparams_matsim
        self._net.projmode = self.projectionmode
        edges = self._matsim.net.edges
        nodes = self._matsim.net.nodes
        if self.netfilepath != '':
            print ('Network Counting...')
            # count nodes and edges
            netcounter = NetCounter(self._matsim)
            parse(self.netfilepath, netcounter)
            
            
            print ('Network Counting done:',netcounter.n_node,'nodes and',netcounter.n_edge,'edges')
            
            # parse network
            netparser = NetParser(  self._matsim, netcounter,
                                    projparams_matsim = self.projparams_matsim,
                                    projectionmode = self.projectionmode,
                                    )
                                    
            parse(self.netfilepath, netparser)
            print ('Network Parsing done. Configure Net database.')
       
            nodes.add_rows( n = netcounter.n_node,
                            ids_matsim = netparser.ids_node_matsim,
                            coords = netparser.get_coords_nodes_sumo(),
                            )
            
            inds_valid = np.zeros(netcounter.n_edge, dtype = bool)
            ids_fromnode = np.zeros(netcounter.n_edge, dtype = np.int64)
            ids_tonode = np.zeros(netcounter.n_edge, dtype = np.int64)
            shapes_edge = np.zeros(netcounter.n_edge, dtype = object)
            
            ids_node = nodes.get_ids()
            for ind, fromnode, tonode in zip(\
                                    np.arange(netcounter.n_edge),
                                    netparser.ids_fromnode_matsim,
                                    netparser.ids_tonode_matsim,
                                    ):
                if nodes.ids_matsim.has_index(fromnode) & nodes.ids_matsim.has_index(tonode): 
                    inds_valid[ind] = True
                    ids_fromnode[ind] = nodes.ids_matsim.get_id_from_index(fromnode)
                    ids_tonode[ind] = nodes.ids_matsim.get_id_from_index(tonode)
                    #print (' id_fromnode ',ids_fromnode[ind],'id_tonode',ids_tonode[ind],ids_fromnode[ind] in ids_node,ids_tonode[ind] in ids_node)
                    #print ('     fromcoord',nodes.coords[ids_fromnode[ind]],'tocoord',nodes.coords[ids_tonode[ind]])
                    shapes_edge[ind] = [nodes.coords[ids_fromnode[ind]],nodes.coords[ids_tonode[ind]]]                
            
            n_edges_valid = np.sum(inds_valid)
            ids_edge = edges.add_rows(  n = n_edges_valid,
                                        ids_matsim = netparser.ids_edge_matsim[inds_valid],
                                        ids_fromnode = ids_fromnode[inds_valid],
                                        ids_tonode = ids_tonode[inds_valid],
                                        lengths = netparser.lengths[inds_valid],
                                        speeds_max = netparser.speeds_max[inds_valid],
                                        capacities = netparser.capacities[inds_valid],
                                        nums_lane = netparser.nums_lane[inds_valid],
                                        types = netparser.types[inds_valid],
                                        modes_matsim = netparser.modes[inds_valid],
                                        shapes = shapes_edge[inds_valid],
                                        )
            edges.shapes[ids_edge] = shapes_edge[inds_valid]
            
            
        return True

class BeamNetImporter(ParserMixin,MatsimProcessMixin, Process):
    def __init__(self,  matsim, netfilepath = '',
                        logger = None, **kwargs):
        print ('BeamNetImporter.__init__',matsim)
        self._init_common(  'beamnetimporter', name = 'BEAM net importer', 
                            logger = logger,
                            info ='Imports BEAM network CSV file into network database.',
                            )
        self._scenario = matsim.get_scenario()
        self._matsim = matsim
        self._net = matsim.net
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()
                
        attrsman = self.set_attrsman(cm.Attrsman(self))

        if netfilepath  is None:
            netfilepath =os.path.join(rootdirpath,rootname+'.net.csv')
        
        self.netfilepath = attrsman.add(cm.AttrConf('netfilepath',kwargs.get('netfilepath',netfilepath),
                        groupnames = ['options'],# 
                        name = 'BEAM Net file', 
                        wildcards = 'Net CSV file (*.csv)|*.csv*',
                        metatype = 'filepath',
                        info = 'BEAM CSV network file to be imported.',
                        ))
        # make SF the default
        
        self.init_proj_attributes(  projparams_matsim = kwargs.get('projparams_matsim','+proj=utm +zone=10 +ellps=GRS80 +datum=NAD83'), 
                                    projectionmode = kwargs.get('projectionmode','custom projection')
                                    ) 
                            
    def do(self):
        
        #line = (zones.coords[id_zone_orig], zones.coords[id_zone_dest])
        #for shape in zones.shapes[ids_zone_cross_filter]:
        #print '        intersect',shape,is_polyline_intersect_polygon(line, shape, is_lineinterpolate = True)
        #if is_polyline_intersect_polygon(line, shape, is_lineinterpolate = True):
        #     pass    
        self._net.projparams = self.projparams_matsim
        edges = self._matsim.net.edges
        nodes = self._matsim.net.nodes
        if self.netfilepath != '':
            # parse network
            self.read_network()
            
            n_nodes = len(self.nodes)
            coord_array = np.zeros((n_nodes, 3), dtype = np.float32)
            coord_array[:,:2] = list(self.nodes.values())
            #ans = self.get_coord_sumo(coord_array[:,0], coord_array[:,1])
            #print ('ans=', len(ans),type(ans),ans)
            #print ('  shape',ans[0].shape)
            coord_array[:,0],coord_array[:,1] = self.get_coord_sumo(coord_array[:,0], coord_array[:,1])
            #coord_array[:,:2] = 
            
            print ('Found',n_nodes,'nodes')
            nodes.add_rows( n = n_nodes,
                            ids_matsim = list(self.nodes.keys()),
                            coords = coord_array,
                            )
            
            n_edges = len(self.edges)
            print ('Found',n_edges,'edges')
            inds_valid = np.zeros(n_edges, dtype = bool)
            i = 0
            for id_edge_matsim, (fromnode, tonode,length,freespeed,capacity,permlanes,oneway, modes) in self.edges.items():
                #print ('  edge',i,'check nodes',fromnode, tonode)
                inds_valid[i] = nodes.ids_matsim.has_index(fromnode) & nodes.ids_matsim.has_index(tonode)
                i += 1
            
            
            n_edges_valid = np.sum(inds_valid)
            
            ids_edge = edges.add_rows(  n = n_edges_valid,
                                        ids_matsim = np.array(list(self.edges.keys()), dtype = object)[inds_valid],
                                        )
            print ('created edge table with',n_edges_valid,len(ids_edge),'valid edges')    
            i = 0
            j = 0
            for id_edge_matsim, (fromnode, tonode,length,freespeed,capacity,permlanes,oneway, modes) in self.edges.items():
                if inds_valid[i]:
                    print ('  edge',i)
                    id_edge = ids_edge[j]
                    id_fromnode = nodes.ids_matsim.get_id_from_index(fromnode)
                    id_tonode = nodes.ids_matsim.get_id_from_index(tonode)
                    edges.ids_fromnode[id_edge] = id_fromnode
                    edges.ids_tonode[id_edge] =  id_tonode
                    edges.lengths[id_edge] = length
                    edges.speeds_max[id_edge] = freespeed
                    edges.capacities[id_edge] = capacity
                    edges.nums_lane[id_edge] = permlanes
                    edges.shapes[id_edge] = [nodes.coords[id_fromnode],nodes.coords[id_tonode]]
                    j += 1 
                i += 1
            
      
        return True
        
    def read_network(self):
        self._init_projection(self.projparams_matsim, self.projectionmode)
        self.nodes = {}
        self.edges = {}
        sep = ","
        f = open(self.netfilepath, "r")
        data = f.readline()
        
        for line in f.readlines():
            linkId,linkLength,linkFreeSpeed,linkCapacity,numberOfLanes,linkModes,attributeOrigId,attributeOrigType,fromNodeId,toNodeId,fromLocationX,fromLocationY,toLocationX,toLocationY = line.split(sep)
            
            # get raw coords of nodes and transform later using vectors
            if fromNodeId not in self.nodes:
                self.nodes[fromNodeId] = fromLocationX,fromLocationY #self.get_coord_sumo(fromLocationX,fromLocationY)
            
            if toNodeId not in self.nodes:
                self.nodes[toNodeId] = toLocationX,toLocationY #self.get_coord_sumo(toLocationX,toLocationY)
                
            self.edges[linkId] = (fromNodeId, toNodeId, linkLength, linkFreeSpeed, linkCapacity, numberOfLanes, False, linkModes)
            

        f.close()

class MATSimNetExport(ParserMixin):

    def __init__(self,  matsim, savefilepath = '',
                        logger = None):
        #
        #
        #
        self._scenario = matsim.get_scenario()
        self._matsim = matsim
        self._net = matsim.net
        self.projparams_matsim = matsim.net.projparams
        self.projmode = matsim.net.projmode
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()
        self.savefilepath  = savefilepath


    
    def do(self):

        self._init_projection(self.projparams_matsim, self.projmode)

        simfile = open(self.savefilepath + 'network.xml' , 'w', encoding="utf-8")

        simfile.write(\
                        """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE network SYSTEM "http://www.matsim.org/files/dtd/network_v1.dtd">\n<network>\n<!-- ====================================================================== -->\n\t<nodes>\n""")
                # <node id="1000056237" x="4422218.544286327" y="5375842.302200025" />

        for id in self._net.nodes.get_ids():
            x_sumo, y_sumo = self._net.nodes.get_row(id)['coords'][0],self._net.nodes.get_row(id)['coords'][1]
            #ACHTUNG: Die Koordinaten müssen zurück in EPSG:31468 transformiert werden
            x_matsim,y_matsim = self.get_coord_matsim(x_sumo,y_sumo)
            simfile.write('\t\t<node id=\"'+ self._net.nodes.get_row(id)['ids_matsim'] +'\" x=\"' + str(x_matsim) +'\" y=\"' + str(y_matsim) + '\"/>\n')


        simfile.write('\t</nodes>')
        simfile.write('\t<links capperiod="01:00:00" effectivecellsize="7.5" effectivelanewidth="3.75">')
        #<link id="99999" from="280511774" to="399363" length="269.75324405938596" freespeed="16.666666666666668" capacity="2000.0" permlanes="2.0" oneway="1" modes="car" origid="307544229" type="secondary" />
                
        for id in self._net.edges.get_ids():

            from_node = self._net.nodes.get_row(self._net.edges.get_row(id)['ids_fromnode'])['ids_matsim']
            to_node = self._net.nodes.get_row(self._net.edges.get_row(id)['ids_tonode'])['ids_matsim']
                    
            simfile.write('\t\t<link id=\"'+ self._net.edges.get_row(id)['ids_matsim'] +'\" from=\"' + from_node +'\" to=\"' + to_node +'\" length=\"' + str(self._net.edges.get_row(id)['lengths']) +'\" freespeed=\"' + str(self._net.edges.get_row(id)['speeds_max'])+'\" capacity=\"' + str(self._net.edges.get_row(id)['capacities'])+'\" permlanes=\"' + str(self._net.edges.get_row(id)['nums_lane'])+'\" oneway=\"1\" modes=\"' + self._net.edges.get_row(id)['modes_matsim']+ '\" type=\"secondary\"/>\n')


        simfile.write(\
        """</links>\n
        <!-- ====================================================================== -->\n
        </network>\n""")
        simfile.close()

    
