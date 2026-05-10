
from xml.sax import saxutils, parse, handler
import os, sys
import numpy as np
try:
    import pyproj
except Exception:
    from mpl_toolkits.basemap import pyproj
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am

# SF
# modes [u'car', u'car_hov2', u'walk_transit', u'hov3_teleportation', u'car_hov3', u'walk', u'hov2_teleportation', u'ride_hail', u'ride_hail_pooled', u'bike']
# activitynames [u'Home', u'othdiscr', u'social', u'shopping', u'eatout', u'work', u'school', u'atwork', u'escort', u'othmaint', u'univ']


MATSIM_SUMO_MODEMAP = {'car':4, 'pt':5, 'walk':1, 'bike':2, 'motorbike':3,\
                        'walk_transit':5,}

MATSIM_ACTIVITYMAP = {'work':3, 'home':2, 'education':4, 'other':6, 'shopping':5,\
                      'Home':2, 'othdiscr':6,'social':6,'eatout':6,'school':4,'atwork':3,'escort':4,'othmaint':6, 'univ':4}
  
PROJECTION_MODEMAP = {'custom projection':1,'no projection':0,'from LON, LAT coordinates':2}
TIMEFORMATMAP = {'hh:mm:ss':0,'seconds after midnight':1} 

class ParserMixin:

    def _init_projection(self, projparams_matsim = 'EPSG:31468', projectionmode = 'no projection'):
        
        print ('Init Projections:')
        scenario = self._matsim.get_scenario()
        self._id_projectionmode = PROJECTION_MODEMAP[projectionmode]
        self._projparams = scenario.net._projparams
        self._projoffset = scenario.net._offset
        self._in_boundaries = scenario.net.in_boundaries 
        self._in_boundaries_matsim = self.in_boundaries
        self._offset = scenario.net.get_offset()
        print ('  SUMO net offset',scenario.net.get_offset() )
        
        self._projparams_matsim = projparams_matsim
        self._projparams_sumo = scenario.net._projparams
        self._inProj = pyproj.Proj(projparams_matsim)
        print ('projparams in',projparams_matsim)



        if self._id_projectionmode != 0: 
            self.coord_matsim = pyproj.CRS(self._projparams_matsim)  
            self.coord_sumo = pyproj.CRS(self._projparams_sumo) 
            self.transformer_I = pyproj.Transformer.from_crs(self.coord_sumo, self.coord_matsim, always_xy=True)

            self._outProj = pyproj.Proj(scenario.net.get_projparams())
            print ('projparams out',scenario.net.get_projparams())
            print ('sumo boundaries',scenario.net.get_boundaries(is_netboundaries = True))
            x_min, y_min ,x_max, y_max =  scenario.net.get_boundaries(is_netboundaries = True)
            x_min_matsim, y_min_matsim = self.get_coord_matsim(x_min, y_min)
            x_max_matsim, y_max_matsim = self.get_coord_matsim(x_max, y_max)
            self._boundaries = np.array([x_min_matsim, y_min_matsim, x_max_matsim, y_max_matsim])
            print ('matsim boundaries',self._boundaries)
        # transform sumo BB in Matsim BB
        # Format of Boundary box
        # [MinX, MinY ,MaxX, MaxY ]


        
        
        # test from user 1421 
        #  <activity end_time="16:34:08" start_time="17:07:58" type="work" x="552957.0854378104" y="4182746.663831072"> 
        #probe = np.array([[552957.0854378104,4182746.663831072],[553043.,  4182661.],[ -122. ,      38. ]])
        #print (" check",probe ,"in bounds",self.in_boundaries(probe) )
        
    def in_boundaries(self, points, x_border = 0.0, y_border = 0.0):
            """
            Tests if the given points are in
            the network boundaries, in matsim coordinates.

            Returns a binary vector with one element for each point
            If an element is True then the corrisponding 
            point in within the networks bounding box. 
            Otherwise the point is outside.
            Elevation is ignored.
            Numpy Array Format of points:
             [[x1,y1,z1],[x2,y2,z2],...]
        
                

            Returns False otherwise
            """
            return ( (self._boundaries[2]-x_border >= points[:,0]) & (self._boundaries[0]+x_border <= points[:,0]) &
                   (self._boundaries[3]-y_border >= points[:,1]) & (self._boundaries[1]+y_border <= points[:,1]) )   
     
    
                   
    # def get_coord_sumo(self, x_matsim,y_matsim):
    #     #print ('get_coord_sumo mode',self._id_projectionmode,'offset',self._offset)
    #     if self._id_projectionmode == 1:
    #         #code ursprung
    #         #x2,y2 = pyproj.transform(self._inProj,self._outProj,x_matsim,y_matsim)
    #         #return x2 + self._offset[0], y2 + self._offset[1] 
    #         #code neu:
    #         projparams_matsim = self._projparams_matsim # 'epsg:31468'
    #         projparams_latlong = 'epsg:4326'
    #         proj_matsim = pyproj.Proj(init = projparams_matsim)
    #         proj_latlong = pyproj.Proj(init = projparams_latlong)       
    #         matsim_lon, matsim_lat = pyproj.transform(proj_matsim,proj_latlong,x_matsim,y_matsim)
    #         print(self._projparams_matsim)
    #         projparams_sumo = self._projparams_sumo #'proj=utm +zone=32 +ellps=WGS84 +datum=WGS84'
    #         x2,y2=pyproj.transform(proj_latlong, projparams_sumo, matsim_lon, matsim_lat)
    #         print(self._projparams)
    #         return x2 + self._offset[0], y2 + self._offset[1] 
        
    #     elif self._id_projectionmode == 2:
    #         x2,y2 = self._inProj(x_matsim,y_matsim) # lon, lat 
    #         return x2 + self._offset[0], y2 + self._offset[1] 
        
    #     else:
    #         return  x_matsim,y_matsim
       
    # def get_coord_matsim(self, x_sumo,y_sumo):
    #     #print ('get_coord_matsim mode',self._id_projectionmode,'offset',self._offset)
    #     if self._id_projectionmode == 1:
    #         #print ('  coords pre',x_sumo-self._offset[0],y_sumo-self._offset[1])
    #         x2,y2 = pyproj.transform(self._outProj,self._inProj,x_sumo-self._offset[0],y_sumo-self._offset[1])
    #         return x2 , y2  
    #     elif self._id_projectionmode == 2:
    #         x2,y2 = self._inProj(x_sumo-self._offset[0],y_sumo-self._offset[1], inverse = True) # lon, lat 
    #         return x2,y2
    #     else:
    #         return  x_sumo,y_sumo
    
    def get_coord_matsim(self, x_sumo,y_sumo):
        #print ('get_coord_matsim mode',self._id_projectionmode,'offset',self._offset)
        if self._id_projectionmode == 1:
            x2,y2 = self.transformer_I.transform(x_sumo-self._offset[0],y_sumo-self._offset[1])
            return x2 , y2  
        elif self._id_projectionmode == 2:
            x2,y2 = self._inProj(x_sumo-self._offset[0],y_sumo-self._offset[1], inverse = True) # lon, lat 
            return x2,y2
        else:
            return  x_sumo,y_sumo
                       
    def get_coord_sumo(self, x_matsim,y_matsim):
        if self._id_projectionmode == 1:
            coord_matsim = pyproj.CRS(self._projparams_matsim)  
            coord_sumo = pyproj.CRS(self._projparams_sumo) 
            transformer_I = pyproj.Transformer.from_crs(coord_matsim, coord_sumo, always_xy=True)
            x2,y2 = transformer_I.transform(x_matsim,y_matsim)
            return x2 + self._offset[0], y2 + self._offset[1] 
        
        elif self._id_projectionmode == 2:
            x2,y2 = self._inProj(x_matsim,y_matsim) # lon, lat 
            return x2 + self._offset[0], y2 + self._offset[1] 
        
        else:
            return  x_matsim,y_matsim
         
    def get_seconds_from_string(self, timestring):
        if self._id_timeformat == 0: 
            time_end_strings = timestring.split(':')
            return 3600*int(time_end_strings[0]) + 60*int(time_end_strings[1]) + int(time_end_strings[2])
        else:
            # print ('  type(timestring)',timestring,type(timestring))
            return int(timestring)

class MatsimProcessMixin:
    def init_proj_attributes(self,**kwargs):
        attrsman = self.get_attrsman()
        self.projectionmode = attrsman.add(cm.AttrConf( 'projectionmode',kwargs.get('projectionmode','custom projection'),
                            groupnames = ['options'], 
                            choices = list(PROJECTION_MODEMAP.keys()),
                            perm='rw', 
                            name = 'Projection Mode', 
                            info = 'Defines the projection method with which the Matsim coordinates are transformed in SUMO network coordindinates.',
                            ))    
                                                  
        self.projparams_matsim = attrsman.add(cm.AttrConf( 'projparams_matsim',kwargs.get('projparams_matsim','epsg:31468'),
                            groupnames = ['options'], 
                            name = 'Custom proj. param', 
                            info = "MATSIM network projection parameters for example 'epsg:31468'. If string is empty then LAT/LON coordinates are assumed.",
                            ))
        
    def _init_importer(self):
        self.ids_fringenode = {}
        self.coords_fringenode = {}
        
    def get_id_fringenode(self,coord, id_mode ):
        
        diff_coords = self.coords_fringenode[id_mode] - coord
        dists2 = diff_coords[:,0]**2 + diff_coords[:,1]**2
        ind_mindist = np.argmin(dists2)
        return self.ids_fringenode[id_mode][ind_mindist], np.sqrt(dists2[ind_mindist])
    
                            
    def make_fringenodes(self, priority_min = 10, numlanes_min = 1, id_mode = 4): 
        edges = self._net.edges
        nodes = self._net.nodes
        accesslevels = edges.get_accesslevels(id_mode)
        ids_node = nodes.get_ids()
        n_nodes = len(ids_node)
        inds_fringe = np.zeros(n_nodes, dtype = bool)
        for i,ids_edge_incoming,ids_edge_outgoing in zip(np.arange(n_nodes), nodes.ids_incoming[ids_node],nodes.ids_outgoing[ids_node] ): 
            if ids_edge_incoming is None:
                n_in = 0
                
            else:
                n_in = len(ids_edge_incoming) 
                
                
            if n_in == 1:
                if ids_edge_outgoing is None:
                    n_out = 0
                else:
                    n_out = len(ids_edge_outgoing)
            
                if n_out == 1:
                        ids_fringeedge= [ ids_edge_incoming[0], ids_edge_outgoing[0]]
                        if (np.min(edges.nums_lanes[ids_fringeedge]) >= numlanes_min) & (np.min(accesslevels[ids_fringeedge]) >= 0):
                            inds_fringe[i] = np.min(edges.priorities[ids_fringeedge]) > priority_min
        
        self.ids_fringenode[id_mode] = ids_node[inds_fringe] 
        self.coords_fringenode[id_mode] = nodes.coords[self.ids_fringenode[id_mode]]                      
