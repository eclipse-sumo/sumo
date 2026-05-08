"""Collection of network related tools to interface Unreal Engine"""
import os, sys
import numpy as np
from scipy.interpolate import interp1d
import pyproj

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.processes import Process
from agilepy.lib_base.geometry import in_boundingbox
import coremodules.misc.matplottools as mpt


import math


a = 6378137.0  # Semi-major axis of the Earth (meters)
f = 1 / 298.257223563  # Flattening factor
e2 = 2 * f - f ** 2  # Square of eccentricity
uescale = 100.0

# Convert degres to radian
def deg_to_rad(degrees):
    return degrees * (math.pi / 180)

# Convert lat/lon/alt to ECEF cordinates
def lat_lon_to_ecef(lat, lon, alt):
    lat_rad = deg_to_rad(lat)
    lon_rad = deg_to_rad(lon)
    
    N = a / math.sqrt(1 - e2 * math.sin(lat_rad) ** 2)
    
    X = (N + alt) * math.cos(lat_rad) * math.cos(lon_rad)
    Y = (N + alt) * math.cos(lat_rad) * math.sin(lon_rad)
    Z = (N * (1 - e2) + alt) * math.sin(lat_rad)
    
    return X, Y, Z


def ecef_to_enu_scaled(x, y, z, x_ref, y_ref, z_ref,lon_ref_rad,lat_ref_rad, scale=100):
    """Compute the scaled  ENU (East, North, Up) from ecef coordinates relative to the origin with scaling"""
    # Convert reference lat/lon to radians
    #lat_ref_rad = deg_to_rad(lat_ref)
    #lon_ref_rad = deg_to_rad(lon_ref)

    # Compute ECEF coordinates of the reference point
    #x_ref, y_ref, z_ref = lat_lon_to_ecef(lat_ref, lon_ref, alt_ref)
    
    # Differences in ECEF coordinates
    dx = x - x_ref
    dy = y - y_ref
    dz = z - z_ref
    
    # Transformation matrix to convert ECEF to ENU
    t = [
        [-math.sin(lon_ref_rad), math.cos(lon_ref_rad), 0],
        [-math.sin(lat_ref_rad) * math.cos(lon_ref_rad), -math.sin(lat_ref_rad) * math.sin(lon_ref_rad), math.cos(lat_ref_rad)],
        [math.cos(lat_ref_rad) * math.cos(lon_ref_rad), math.cos(lat_ref_rad) * math.sin(lon_ref_rad), math.sin(lat_ref_rad)]
    ]
    
    # Apply transformation with scaling and correct sign 
    east = (t[0][0] * dx + t[0][1] * dy + t[0][2] * dz) * scale
    north = -(t[1][0] * dx + t[1][1] * dy + t[1][2] * dz) * scale  # Miroring
    up = (t[2][0] * dx + t[2][1] * dy + t[2][2] * dz) * scale
    
    return east, north, up
    
def lonlat_to_enu_scaled(lon_target,lat_target, z_target, x_ref, y_ref, z_ref,lon_ref_rad,lat_ref_rad, scale=100):
    
    
    
    # Convert reference lat/lon to radians (already precalculated)
    #lat_ref_rad = deg_to_rad(lat_ref)
    #lon_ref_rad = deg_to_rad(lon_ref)

    # Compute ECEF coordinates of the reference point
    #x_ref, y_ref, z_ref = lat_lon_to_ecef(lat_ref, lon_ref, alt_ref)
    
    # Compute ECEF coordinates of the point
    x, y, z = lat_lon_to_ecef(lat_target, lon_target, z_target)
    #print ('    x',x,'y',y,'z',z)
    #print ('    x_ref',x_ref,'y_ref',y_ref,'z',z_ref)
    # Differences in ECEF coordinates
    dx = x - x_ref
    dy = y - y_ref
    dz = z - z_ref
    #print ('  dx',dx,'dy',dy,'dz',dz)
    # Transformation matrix to convert ECEF to ENU
    t = [
        [-math.sin(lon_ref_rad), math.cos(lon_ref_rad), 0],
        [-math.sin(lat_ref_rad) * math.cos(lon_ref_rad), -math.sin(lat_ref_rad) * math.sin(lon_ref_rad), math.cos(lat_ref_rad)],
        [math.cos(lat_ref_rad) * math.cos(lon_ref_rad), math.cos(lat_ref_rad) * math.sin(lon_ref_rad), math.sin(lat_ref_rad)]
    ]
    
    # Apply transformation with scaling and correct sign 
    east = (t[0][0] * dx + t[0][1] * dy + t[0][2] * dz) * scale
    north = -(t[1][0] * dx + t[1][1] * dy + t[1][2] * dz) * scale  # Miroring
    up = (t[2][0] * dx + t[2][1] * dy + t[2][2] * dz) * scale
    #print ('  up',up)
    return east, north, up
    
class UeNetExporter(Process):
    def __init__(self,  scenario, ident = 'uenetexporter', 
                        name = 'Unreal Engine Network Exporter',
                        rootfilepath = None,
                        rootname = None,
                        info = 'Exports different network shapes that are used to build the road network in Unreal Engine.',
                        logger = None, 
                        **kwargs):
        
        
        net = scenario.net
        
        if rootname is None:
            rootname = net.parent.get_rootfilename()
            
        rootdirpath = net.parent.get_workdirpath()
        

        rootfilepath = os.path.join(rootdirpath,rootname)
            
        print('UeNetExporter.__init__',rootfilepath)
        self._init_common(  ident, 
                            parent = scenario,
                            name = name, 
                            logger = logger,
                            info = info,
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        
        
        
                                            
        self.rootfilepath = attrsman.add(cm.AttrConf('rootfilepath',rootfilepath,
                                        groupnames = ['_private'],
                                        perm='rw', 
                                        name = 'Rootfilename', 
                                        metatype = 'filepath',
                                        info = """Root File path used for naming various UE export files.""",
                                        ))
        
        
        boundaries_default = net.get_boundaries(is_netboundaries = True)
        self.boundaries = attrsman.add(cm.ListConf('boundaries',kwargs.get('boundaries',boundaries_default), 
                                            groupnames = ['options'], 
                                            name = 'Explicit boundary box', 
                                            info = """Expicit boundary box of the format [x_min, y_min,x_max, y_max]. Empty list means no explicit bounding borders are applied.""",
                                            ))   
        
        
        self.is_lonlat = attrsman.add(cm.AttrConf('is_lonlat',kwargs.get('is_lonlat',False),
                                        groupnames = ['options'],
                                        perm='rw', 
                                        name = 'LON,LAT coordinates', 
                                        info = """Transform into LON,LAT coordinates instead of network or UE coordinates.""",
                                        ))
        
                                         
        #id_node_ref = attrsman.add(cm.AttrConf('id_node_ref',kwargs.get('id_node_ref',net.nodes.get_ids()[0]),
        #                                groupnames = ['options', '_private'],
        #                                perm='rw', 
        #                                name = 'ID reference node', 
        #                                info = """ID reference node will be the origin for the UE projection. Setting -1 there will be no UE projection.""",
        #                                ))
        
        kinds  = ['slinear', 'quadratic', 'cubic']
        
        self.method_interp = attrsman.add(cm.AttrConf('method_interp',kwargs.get('method_interp','slinear'),
                                        groupnames = ['options'],
                                        choices = ['None','slinear', 'quadratic', 'cubic','linear', 'nearest', 'zero', 'previous', 'next'],
                                        perm='rw', 
                                        name = 'Interpolation', 
                                        info = """Interpolation method.""",
                                        ))
                                        
        self.res_interp = attrsman.add(cm.AttrConf('res_interp',kwargs.get('res_interp',1.0),
                                        groupnames = ['options'],
                                        perm='rw', 
                                        name = 'Interpolation resolution', 
                                        unit = 'm',
                                        info = """Interpolation resolution.""",
                                        ))                                
                   
        self.is_preview = attrsman.add(cm.AttrConf('is_preview',kwargs.get('is_preview',False),
                                        groupnames = ['options'],
                                        perm='rw', 
                                        name = 'Show preview in plot', 
                                        info = """Show preview in plot.""",
                                        ))
        self.is_lf_win = attrsman.add(cm.AttrConf('is_lf_win',kwargs.get('is_lf_win',False),
                                        groupnames = ['options','recording'],
                                        perm='rw', 
                                        name = 'Use DOS LF', 
                                        info = """Use DOS Line Feed (LF) Character in output CSV files.""",
                                        ))          
                
    def do(self):
        print(self.ident+'.do')
        
        if self.is_preview:
            fig, ax = mpt.init_plot_fig_ax()
            #ax = mpt.init_plot()
        else:
            ax = None
        
        if self.is_lf_win:
                LF = '\r\n'
        else:
                LF = '\n'
                    
        nodeshapes = export_lanes(  self.parent.net, self.rootfilepath+'.uelanes.csv',
                                     self.boundaries, 
                                     is_lonlat = self.is_lonlat, 
                                     method_interp = self.method_interp,
                                     res_interp = self.res_interp,
                                     axis = ax,
                                     lf = LF,
                                     )
                                     
        export_nodes(   self.parent.net,  self.rootfilepath+'.uenodes.csv', 
                        self.boundaries, 
                        is_lonlat = self.is_lonlat, 
                        axis = ax, 
                        nodeshapes = nodeshapes,
                        lf = LF,
                        )
        
        if self.is_preview:
            
            ax.set_xlim([self.boundaries[0], self.boundaries[2]])
            ax.set_ylim([self.boundaries[1], self.boundaries[3]])
            ax.axis('equal')
            
            # Nop!
            #ax.set_aspect('equal', 'box')
            #mpt.plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            mpt.show_plot()
            
        return True


def get_id_node_ref(net):
    if len(net.nodes) > 0:
        return net.nodes.get_ids()[0]
    else:
        return -1
    
def export_lanes(   net, csvfilepath, bbox, 
                    is_lonlat = True, 
                    method_interp = 'None', 
                    res_interp = 1.0,
                    is_make_poly = True,
                    axis = None, 
                    sep = ',', 
                    lf = '\n',
                    ):
    
    
    csvfile = open(csvfilepath, 'w')
    lanes = net.lanes
    edges = net.edges
    nodes = net.nodes
    nodeshapes = {}
    
    is_interp = method_interp != 'None'
    
    
    id_tram = net.modes.get_id_mode('tram')
    id_pedestrian = net.modes.get_id_mode('pedestrian')
    id_car = net.modes.get_id_mode('passenger')
    id_bus = net.modes.get_id_mode('bus')
    id_bike = net.modes.get_id_mode('bicycle')
    
    ids_lane = lanes.get_ids()
    #print ('len(ids_lane)',len(ids_lane))
    
    proj_sumo = pyproj.Proj(net.get_projparams())
    netoffset_sumo = net.get_offset()
    
    id_node_ref = get_id_node_ref(net)
    if id_node_ref != -1:
        # reference node is given, so use its coordinates as origin of the UE projection
        x_ref,y_ref, alt_ref = net.nodes.coords[id_node_ref]
        lon_ref, lat_ref = proj_sumo(x_ref - netoffset_sumo[0], y_ref - netoffset_sumo[1], inverse = True)
        # Compute ECEF coordinates of the reference point
        x_ueref, y_ueref, z_ueref = lat_lon_to_ecef(lat_ref, lon_ref, alt_ref)
        lat_ref_rad = deg_to_rad(lat_ref)
        lon_ref_rad = deg_to_rad(lon_ref)
        
    if axis is not None:
        color_lane = "blue"
        color_lane_interp = 'm'
        color_lane_poly = 'cyan'
        alpha = 0.8
        mapscale = 1.0
        scale_width_lane = 0.8
        zorder = -100
        Polygon = mpt.mpl.patches.Polygon
        patches = []
        
    for id_lane, id_edge, laneindex, lanewidth, lanepolyline3d, id_fromnode, id_tonode in zip(  ids_lane, 
                                                                                                lanes.ids_edge[ids_lane], 
                                                                                                lanes.indexes[ids_lane],  
                                                                                                lanes.widths[ids_lane], 
                                                                                                lanes.shapes[ids_lane],
                                                                                                edges.ids_fromnode[lanes.ids_edge[ids_lane]],
                                                                                                edges.ids_tonode[lanes.ids_edge[ids_lane]],
                                                                                                ):
                 
        
        inds_point_valid = in_boundingbox(lanepolyline3d, bbox,  x_border = 0.0, y_border = 0.0)
        
        if (np.sum(inds_point_valid) >= 2): # at least 2 valif geom points
            shape = np.array(lanepolyline3d[inds_point_valid], dtype = np.float64)
            
            # prepare node shape database
            if not id_fromnode in nodeshapes:
                nodeshapes[id_fromnode] = []
            if not id_tonode in nodeshapes:
                nodeshapes[id_tonode] = []
            
            
            
            # do here interpolation with shape
            if is_interp:
                if len(shape) > 2:
                
                    # Linear length along the line:
                    distance = np.cumsum( np.sqrt(np.sum( np.diff(shape, axis=0)**2, axis=1 )) )
                    lanelength = distance[-1]
                    distance = np.insert(distance, 0, 0)/lanelength
                    
                    s = np.linspace(0, 1, int(lanelength/res_interp + 0.5))
                    # kinds  = ['slinear', 'quadratic', 'cubic']
                    interpolator =  interp1d(distance, shape, kind = method_interp, axis = 0)
                    shape_interp = interpolator(s)
                    
                    
                    
                else:
                    # no interpolation for 2 points
                    shape_interp = shape
                    
                
    
            # interpret lane type
            if lanes.get_accesslevel_lane(id_lane, id_tram) >= 1: 
                # exclusive tram or mixed 
                access = 'tram'
            
            #elif lanes.get_accesslevel_lane(id_lane, id_car) == 2:
            #    # exclusive car 
            #    access = 'car'
            # TODO: create a highway lane type where bicycles and peds are not allowed  
            
            elif lanes.get_accesslevel_lane(id_lane, id_bike) == 2:
                access = 'bicycle'
            
            elif lanes.get_accesslevel_lane(id_lane, id_pedestrian) >= 1:
                # foodpath with some specific other modes allowed 
                access = 'pedestrian'
                
            elif lanes.get_accesslevel_lane(id_lane, id_bus) >= 1:
                # reserved bus lane  with some specific other modes allowed 
                access = 'bus'
            
            else:
                access = 'mixed'
            
                    
            print (70*'.')
            print ('  Data of lane ID',id_lane,'index',laneindex,'id_edge',id_edge,f'width {lanewidth}m',access)#,'3D shape',lanepolyline3d)
            
            # plot if requested
            if axis is not None:
                #print ('  plot shape',shape)
                axis.add_patch(Polygon(  shape[:,:2]*mapscale, 
                                    linewidth = 1.0,#lanewidth * scale_width_lane,
                                    edgecolor = color_lane, 
                                    facecolor = None, 
                                    fill = False,
                                    alpha = alpha,
                                    zorder = zorder,
                                    closed = False,
                                    ))
                if is_interp:
                    axis.add_patch(Polygon( shape_interp[:,:2]*mapscale, 
                                    linewidth = 1.0,#lanewidth * scale_width_lane,
                                    edgecolor = color_lane_interp, 
                                    facecolor = None, 
                                    fill = False,
                                    alpha = alpha,
                                    zorder = zorder,
                                    closed = False,
                                    ))
            
            # now after plotting, copy interpolated shape to shape                        
            if is_interp:
                shape = shape_interp
            
            if is_make_poly:
                
                n_point = len(shape)
                halfwidth = 0.5*lanewidth
                #print ('  shape',shape) 
                shape_poly = np.zeros((2*n_point,3), dtype = np.float64)
                
                phi = 0.0
                for i, point in zip(range(n_point),shape):
                    x1,y1,z1 = point
                    if i ==0:
                        # first point
                        x2,y2,z2 = shape[i+1]
                        phi = np.arctan2(y2-y1,x2-x1)
                        
                    if (i > 0) & (i < (n_point-2)):
                        x0,y0,z0 = shape[i-1]
                        x2,y2,z2 = shape[i+1]
                        phi1 = np.arctan2(y1-y0,x1-x0)
                        phi2 = np.arctan2(y2-y1,x2-x1)
                        phi = 0.5 * (phi1 + phi2)
                        
                    elif i ==(n_point-1):
                        # last point
                        x0,y0,z0 = shape[i-1]
                        phi = np.arctan2(y1-y0,x1-x0)
                        
                        
                    #print ('    inds',i,2*n_point-1 - i,len(shape_poly),'phi',phi/np.pi*180,(phi+np.pi/2)/np.pi*180)
                    #print ('         x1',x1,'\ty1',y1)
                    #print ('         xf',x1+ halfwidth * np.cos(phi+np.pi/2),'\tyf',y1+ halfwidth * np.sin(phi+np.pi/2))
                    #print ('         xb',x1 + halfwidth * np.cos(phi-np.pi/2),'\tyb',y1 + halfwidth * np.sin(phi-np.pi/2))
                    
                    shape_poly[i,:] = [x1 + halfwidth * np.cos(phi+np.pi/2), y1+ halfwidth * np.sin(phi+np.pi/2), z1]
                    shape_poly[2*n_point-1 - i,:] = [x1 + halfwidth * np.cos(phi-np.pi/2), y1 + halfwidth * np.sin(phi-np.pi/2), z1]
                
                # update node shape database with 
                # geometry end points
                # here there are 2 corner poits at the start end end of the lane
                # make sure z coord of end point corrisponds to the z coord of the node
                shape_poly[0][2] = nodes.coords[id_fromnode][2]
                nodeshapes[id_fromnode].append(shape_poly[0])
                
                shape_poly[2 * n_point - 1][2] = nodes.coords[id_fromnode][2]
                nodeshapes[id_fromnode].append(shape_poly[2 * n_point - 1])
                
                shape_poly[n_point-1][2] = nodes.coords[id_tonode][2]
                nodeshapes[id_tonode].append(shape_poly[n_point-1])
                
                shape_poly[n_point][2] = nodes.coords[id_tonode][2]
                nodeshapes[id_tonode].append(shape_poly[n_point])
                
                shape = shape_poly
                
                # plot polygon    
                if axis is not None:
                    #print ('  shape_poly',shape_poly) 
                    axis.add_patch(Polygon( shape_poly[:,:2]*mapscale, 
                                    linewidth = 1.0,#lanewidth * scale_width_lane,
                                    edgecolor = color_lane, 
                                    facecolor = color_lane_poly, 
                                    fill = True,
                                    alpha = 0.5*alpha,
                                    zorder = zorder-1,
                                    closed = True,
                                    ))
                 
            else: # no interpolation
            
                # update node shape database with 
                # geometry end points
                nodeshapes[id_fromnode].append(shape[0])
                nodeshapes[id_tonode].append(shape[-1])
                
            csvfile.write(f'{id_edge:d}{sep:s}{laneindex:d}{sep:s}{lanewidth:.3f}{sep:s}'+access+sep)
            
            
            points = shape[:,:2] - netoffset_sumo
            #print ('points',points)
            lons, lats = proj_sumo(points[:,0],points[:,1], inverse = True)
            #print ('lons, lats',lons, lats)
            n_point = len(shape)
            i = 0
            phi = 0.0
            for point, lon, lat in zip(shape, lons, lats):
                x1,y1,z1 = point
                if i < (n_point-1):
                    x2,y2,z2 = shape[i+1]
                    phi = np.arctan2(y2-y1,x2-x1)
                    if is_lonlat:
                        csvfile.write(f'{lon:.6f} {lat:.6f} {z1:.3f} {phi:.3f}{sep:s}')
                    else:
                        if id_node_ref != -1:
                            east_scaled, north_scaled, up_scaled = lonlat_to_enu_scaled(lon, lat, z1, x_ueref, y_ueref, z_ueref, lon_ref_rad, lat_ref_rad, scale = uescale)
                            csvfile.write(f'{east_scaled:.6f} {north_scaled:.6f} {up_scaled:.6f} {phi:.3f}{sep:s}')
                        else:
                            csvfile.write(f'{x1:.3f} {y1:.3f} {z1:.3f} {phi:.3f}{sep:s}')
                    
                else:
                    # last point 
                    if is_lonlat:
                        csvfile.write(f'{lon:.6f} {lat:.6f} {z1:.3f} {phi:.3f}')
                    else:
                        if id_node_ref != -1:
                            east_scaled, north_scaled, up_scaled = lonlat_to_enu_scaled(lon, lat, z1, x_ueref, y_ueref, z_ueref, lon_ref_rad, lat_ref_rad, scale = uescale)
                            csvfile.write(f'{east_scaled:.6f} {north_scaled:.6f} {up_scaled:.6f} {phi:.3f}')
                        else:
                            csvfile.write(f'{x1:.3f} {y1:.3f} {z1:.3f} {phi:.3f}')
                
                i += 1
                
            csvfile.write(lf)
            
   
                                    
    csvfile.close()
    
    return nodeshapes

def export_nodes(   net, csvfilepath, bbox, is_lonlat = True, 
                    method_interp = 'None', 
                    axis = None, nodeshapes = None, 
                    sep = ',',lf = '\n',):
                    
    csvfile = open(csvfilepath, 'w')
    nodes = net.nodes
    edges = net.edges
    proj_sumo = pyproj.Proj(net.get_projparams())
    netoffset_sumo = net.get_offset()
    id_node_ref = get_id_node_ref(net)
    if id_node_ref != -1:
        # reference node is given, so use its coordinates as origin of the UE projection
        x_ref,y_ref, alt_ref = nodes.coords[id_node_ref]
        lon_ref, lat_ref = proj_sumo(x_ref - netoffset_sumo[0], y_ref - netoffset_sumo[1], inverse = True)
        
        print ('  lon_ref',lon_ref,'lat_ref',lat_ref)
        csvfile.write(f'{lon_ref:.6f} {lat_ref:.6f} {alt_ref:.3f}'+lf)

        # Compute ECEF coordinates of the reference point
        x_ueref, y_ueref, z_ueref = lat_lon_to_ecef(lat_ref, lon_ref, alt_ref)
        print ('  x_ueref',x_ueref,'y_ueref',y_ueref,'z_ueref',z_ueref)
        lat_ref_rad = deg_to_rad(lat_ref)
        lon_ref_rad = deg_to_rad(lon_ref)
    
        
        
        
    
    is_interp = method_interp != 'None'
    
    if axis is not None:
        color_node = "cyan"
        color_node_border = "gray"
        alpha = 0.9
        mapscale = 1.0
        zorder = -100
        Polygon = mpt.mpl.patches.Polygon
        
        
    ids_node = nodes.select_ids(in_boundingbox(nodes.coords.value, bbox,  x_border = 0.0, y_border = 0.0))
    for id_node, nodeshape, nodecoord, nodetype,   in zip(ids_node, nodes.shapes[ids_node], nodes.coords[ids_node], nodes.types[ids_node]):
        print ('\n  ID node',id_node)
        
        if nodeshapes is not None:
            # read nodeshape from custom node shape
            # for example precalculated from lane endings
            if id_node in nodeshapes:
                nodeshape = nodeshapes[id_node]
            
        if len(nodeshape) >= 3:# at least 3 points for a polygon
            nodeshapearray = np.array(nodeshape, dtype = np.float64)
            #edgepoints = []
            #for id_edge in nodes.ids_incoming[id_node]:
            #    edgepoints.append(edges.shapes[id_edge][-1])
            #for id_edge in nodes.ids_outgoing[id_node]:
            #    edgepoints.append(edges.shapes[id_edge][-1])
            #    
            #edgepoints = np.array(edgepoints, dtype = np.float32)
            #print ('    edgepoints',edgepoints)
            
            
            #print ('    nodeshape',nodeshape)
            csvfile.write(f'{id_node:d}{sep:s}{nodetype:d},')
            xc,yc,zc = nodecoord
            #xc,yc = project(np.array([xc,yc]), point0_sumo, scale) 
            phis = np.arctan2(nodeshapearray[:,1]-yc,nodeshapearray[:,0]-xc)
            inds_coords = np.argsort(phis)
            #print ('    phis',phis[inds_coords])
            #np.zeros(len(nodeshape),dtype = np.float64)
            
            # swap coords
            nodeshapearray = nodeshapearray[inds_coords]
            
            # do here interpolation with shape
            if is_interp:
                if len(nodeshapearray) > 2:
                
                    # Linear length along the line:
                    distance = np.cumsum( np.sqrt(np.sum( np.diff(nodeshapearray, axis=0)**2, axis=1 )) )
                    polylength = distance[-1]
                    distance = np.insert(distance, 0, 0)/polylength
                    
                    s = np.linspace(0, 1, int(polylength/res_interp + 0.5))
                    # kinds  = ['slinear', 'quadratic', 'cubic']
                    interpolator =  interp1d(distance, nodeshapearray, kind = method_interp, axis = 0)
                    shape_interp = interpolator(s)
                    
                    
                    
                else:
                    # no interpolation for 2 points
                    shape_interp = shape
            
            # start export with center
            lonc, latc = proj_sumo(xc-netoffset_sumo[0], yc-netoffset_sumo[1], inverse = True)
            lons, lats = proj_sumo(nodeshapearray[:,0] - netoffset_sumo[0], nodeshapearray[:,1] - netoffset_sumo[1], inverse = True)
            
            if is_lonlat:
                # export lo, lat coords
                
                csvfile.write(f'{lonc:.6f} {latc:.6f} {zc:.3f}{sep:s}')
                for lon, lat, z in zip(lons, lats,nodeshapearray[:,2]):
                    csvfile.write(f'{lon:.6f} {lat:.6f} {z:.3f}{sep:s}')
            else:
                
                if id_node_ref != -1:
                    # export project to UR coords
                    
                    east_scaled, north_scaled, up_scaled = lonlat_to_enu_scaled(lonc, latc, zc, x_ueref, y_ueref, z_ueref, lon_ref_rad, lat_ref_rad, scale = uescale)
                    csvfile.write(f'{east_scaled:.6f} {north_scaled:.6f} {up_scaled:.6f}{sep:s}')
                    #print ('    lonc',lonc,'latc',latc,'zc',zc)
                    #print ('    east',east_scaled,'nort',north_scaled,'up',up_scaled)
                    #sys.exit(0)
                    
                    for lon, lat, z in zip(lons, lats,nodeshapearray[:,2]):
                        east_scaled, north_scaled, up_scaled = lonlat_to_enu_scaled(lon, lat, z, x_ueref, y_ueref, z_ueref, lon_ref_rad, lat_ref_rad, scale = uescale)
                        csvfile.write(f'{east_scaled:.6f} {north_scaled:.6f} {up_scaled:.6f}{sep:s}')
                    
                else:
                    # export plain SUMO coords
                    csvfile.write(f'{xc:.3f} {yc:.3f} {zc:.3f}{sep:s}')
                    for (x1,y1,z1) in nodeshapearray[:]:
                        csvfile.write(f'{x1:.3f} {y1:.3f} {z1:.3f}{sep:s}')
            
            csvfile.write(lf)
            
            if axis is not None:
                #print ('  plot shape',nodeshapearray[:,:2])
                axis.add_patch(Polygon(  nodeshapearray[:,:2]*mapscale, 
                                    linewidth = 1.0,#lanewidth * scale_width_lane,
                                    edgecolor = color_node_border, 
                                    facecolor = color_node, 
                                    fill = True,
                                    alpha = alpha,
                                    zorder = zorder,
                                    closed = True,
                                    ))
                                    
        else:
            print ('  Failed number of vertices',len(nodeshape))
    
            
    csvfile.close()
