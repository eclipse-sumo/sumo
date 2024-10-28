# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2024 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    geometry.py
# @author  Joerg Schweizer
# @date    2012

import numpy as np
try:
    from shapely.geometry import MultiPoint, Polygon, Point, LineString
    IS_SHAPELY = True
except:
    IS_SHAPELY = False


# >>> from geometry import *
# >>> cv = [(0,0),(0,1),(0,2),(0,3),(3,3),(3,0)]
# >>> cc = [(0,0),(0,1),(1,1),(1,2),(0,2),(0,3),(3,3),(3,0)]
# >>>
# >>> p=Point(0.5,2)
# >>> policc=Polygon(cc)
# >>> policv=Polygon(cv)
# >>> is_point_in_polygon(p,cv, is_use_shapely = True)
# >>> pc = [0.5,2]
# >>> is_point_in_polygon(pc,cv, is_use_shapely = True)
# True
# >>> is_point_in_polygon(pc,cc, is_use_shapely = True)
# False
# >>> is_point_in_polygon(pc,cc, is_use_shapely = False)
# False
# >>> is_point_in_polygon(pc,cv, is_use_shapely = False)
# True

def get_norm_2d(vertex3d):
    # print 'get_norm_2d',vertex3d.shape
    return np.sqrt(np.sum(vertex3d[:, :2]**2, 1))
    # print '  r',r.shape
    # return r


def get_length_polylines(polylines):
    # print 'get_length_polylines'
    # v = np.array([[[0.0,0.0,0.0],[1,0.0,0.0]],[[1,0.0,0.0],[1,2,0.0]],[[1,2.0,0.0],[1,2,3.0]] ])
    lengths = np.zeros(len(polylines), np.float)
    i = 0
    for v in polylines:
        # print '  v=\n',v,v.shape
        # print '  v[:,0,:]\n',v[:,0,:]
        # print '  v[:,1,:]\n',v[:,1,:]
        lengths[i] = np.sum(np.sqrt(np.sum((v[:, 1, :]-v[:, 0, :])**2, 1)))
        i += 1
    return lengths


def get_length_polypoints(polylines):
    # print 'get_length_polypoints'
    # v = np.array([[[0.0,0.0,0.0],[1,0.0,0.0]],[[1,0.0,0.0],[1,2,0.0]],[[1,2.0,0.0],[1,2,3.0]] ])
    lengths = np.zeros(len(polylines), np.float)
    i = 0
    for v in polylines:
        # print '  v=\n',v
        a = np.array(v, np.float32)
        # print '  a=\n',a,a.shape
        lengths[i] = np.sum(np.sqrt(np.sum((a[1:, :]-a[:-1, :])**2, 1)))
        i += 1
    return lengths


def polypoints_to_polylines(polypoints):
    linevertices = np.array([None]*len(polypoints), np.object)  # np.zeros((0,2,3),np.float32)
    #polyinds = []
    #lineinds = []

    ind = 0
    ind_line = 0
    for polyline in polypoints:
        # Important type conversion!!
        v = np.zeros((2*len(polyline)-2, 3), np.float32)
        v[0] = polyline[0]
        v[-1] = polyline[-1]
        if len(v) > 2:
            v[1:-1] = np.repeat(polyline[1:-1], 2, 0)

        #n_lines = len(v)/2
        #polyinds += n_lines*[ind]
        # lineinds.append(np.arange(ind_line,ind_line+n_lines))
        #ind_line += n_lines

        linevertices[ind] = v.reshape((-1, 2, 3))
        #linevertices = np.concatenate((linevertices, v.reshape((-1,2,3))))

        ind += 1

    return linevertices  # , lineinds, polyinds


def get_vec_on_polyline_from_pos(polyline, pos, length, angle=0.0):
    """
    Returns a vector ((x1,y1,z1),(x2,y2,z2))
    where first coordinate is the point on the polyline at position pos
    and the second coordinate is length meters ahead with an angle 
    with respect to the direction of the polyline.

    TODO: Attention angle not yet implemented
    """
    pos_edge = 0.0
    pos_edge_pre = 0.0
    x1, y1, z1 = polyline[0]

    for j in range(1, len(polyline)):
        x2, y2, z2 = polyline[j]
        seglength = np.linalg.norm([x2-x1, y2-y1])
        pos_edge += seglength

        if (pos >= pos_edge_pre) & (pos <= pos_edge):

            dxn = (x2-x1)/seglength
            dyn = (y2-y1)/seglength
            u1 = (pos-pos_edge_pre)

            u2 = (pos+length-pos_edge_pre)
            return np.array([[x1 + u1 * dxn, y1 + u1 * dyn, z2], [x1 + u2 * dxn, y1 + u2 * dyn, z2]], np.float32)

        x1, y1 = x2, y2
        pos_edge_pre = pos_edge

    x1, y1, z1 = polyline[-1]
    dxn = (x2-x1)/seglength
    dyn = (y2-y1)/seglength
    u1 = (pos-pos_edge_pre)
    u2 = (pos+length-pos_edge_pre)
    return np.array([[x2, y2, z2], [x2 + u2 * dxn, y1 + u2 * dyn, z2]], np.float32)


def get_coord_on_polyline_from_pos(polyline, pos):
    pos_edge = 0.0
    pos_edge_pre = 0.0
    x1, y1, z1 = polyline[0]

    for j in range(1, len(polyline)):
        x2, y2, z2 = polyline[j]
        length = np.linalg.norm([x2-x1, y2-y1])
        pos_edge += length

        if (pos >= pos_edge_pre) & (pos <= pos_edge):
            u = (pos-pos_edge_pre)/length
            x = x1 + u * (x2-x1)
            y = y1 + u * (y2-y1)
            return np.array([x, y, z2], np.float32)

        x1, y1 = x2, y2
        pos_edge_pre = pos_edge

    return np.array([x2, y2, z2], np.float32)


def get_coord_angle_on_polyline_from_pos(polyline, pos):
    """
    Return coordinate and respective angle
    at position pos on the polygon.
    """
    pos_edge = 0.0
    pos_edge_pre = 0.0
    x1, y1, z1 = polyline[0]

    for j in range(1, len(polyline)):
        x2, y2, z2 = polyline[j]
        length = np.linalg.norm([x2-x1, y2-y1])
        pos_edge += length

        if (pos >= pos_edge_pre) & (pos <= pos_edge):
            u = (pos-pos_edge_pre)/length
            dx = (x2-x1)
            dy = (y2-y1)
            x = x1 + u * dx
            y = y1 + u * dy
            return np.array([x, y, z2], np.float32), np.arctan2(dy, dx)

        dx = (x2-x1)
        dy = (y2-y1)
        x1, y1 = x2, y2
        pos_edge_pre = pos_edge

    return np.array([x2, y2, z2], np.float32), np.arctan2(dy, dx)


def get_pos_on_polyline_from_coord(polyline, coord):
    xc, yc, zc = coord
    n_segs = len(polyline)

    d_min = 10.0**8
    x_min = 0.0
    y_min = 0.0
    j_min = 0
    p_min = 0.0
    pos = 0.0
    x1, y1, z1 = polyline[0]
    for j in range(1, n_segs):
        x2, y2, z2 = polyline[j]
        d, xp, yp = shortest_dist(x1, y1, x2, y2, xc, yc)
        # print '    x1,y1=(%d,%d)'%(x1,y1),',x2,y2=(%d,%d)'%(x2,y2),',xc,yc=(%d,%d)'%(xc,yc)
        # print '    d,x,y=(%d,%d,%d)'%shortest_dist(x1,y1, x2,y2, xc,yc)
        if d < d_min:
            d_min = d
            # print '    **d_min=',d_min,[xp,yp]
            x_min = xp
            y_min = yp
            j_min = j
            p_min = pos
        # print '    pos',pos,[x2-x1,y2-y1],'p_min',p_min
        pos += np.linalg.norm([x2-x1, y2-y1])
        x1, y1 = x2, y2

    x1, y1, z1 = polyline[j_min-1]
    return p_min+np.linalg.norm([x_min-x1, y_min-y1])


def shortest_dist(x1, y1, x2, y2, x3, y3):  # x3,y3 is the point
    """
    Shortest distance between point (x3,y3) and line (x1,y1-> x2,y2).
    Returns distance and projected point on line.
    """

    px = x2-x1
    py = y2-y1

    something = px*px + py*py
    if something > 0:
        u = ((x3 - x1) * px + (y3 - y1) * py) / float(something)
    else:
        u = 0

    # clip and return infinite distance if not on the line
    if u > 1:
        u = 1
        # return 10.0**8,x1 +  px,y1 +  py

    elif u < 0:
        u = 0
        # return 10.0**8,x1 ,y1

    x = x1 + u * px
    y = y1 + u * py

    dx = x - x3
    dy = y - y3

    # Note: If the actual distance does not matter,
    # if you only want to compare what this function
    # returns to other results of this function, you
    # can just return the squared distance instead
    # (i.e. remove the sqrt) to gain a little performance

    dist = np.sqrt(dx*dx + dy*dy)

    return dist, x, y


def get_dist_point_to_segs(p, y1, x1, y2, x2, is_ending=True,
                           is_detect_initial=False,
                           is_detect_final=False,
                           #is_return_pos = False
                           ):
    """
    Minimum square distance between a Point p = (x,y) and a Line segments ,
    where vectors x1, y1 are the first  points and x2,y2 are the second points 
    of the line segments.

    If is_detect_initial is True then a point whose projection is beyond
    the start of the segment will result in a NaN distance.

    If is_detect_final is True then a point whose projection is beyond
    the end of the segment will result in a NaN distance.

    If is_return_pos then the position on the section is returned.

    Written by Paul Bourke,    October 1988
    http://astronomy.swin.edu.au/~pbourke/geometry/pointline/

    Rewritten in vectorial form by Joerg Schweizer
    """

    y3, x3 = p

    d = np.zeros(len(y1), dtype=np.float32)

    dx21 = (x2-x1)
    dy21 = (y2-y1)

    lensq21 = dx21*dx21 + dy21*dy21

    # indexvector for all zero length lines
    iz = (lensq21 == 0)

    dy = y3-y1[iz]
    dx = x3-x1[iz]

    d[iz] = dx*dx + dy*dy

    lensq21[iz] = 1.0  # replace zeros with 1.0 to avoid div by zero error

    u = (x3-x1)*dx21 + (y3-y1)*dy21
    u = u / lensq21  # normalize position

    x = x1 + u * dx21
    y = y1 + u * dy21

    if is_ending:
        if not is_detect_initial:
            ie = u < 0
            x[ie] = x1[ie]
            y[ie] = y1[ie]

        if not is_detect_final:
            ie = u > 1
            x[ie] = x2[ie]
            y[ie] = y2[ie]

    dx30 = x3-x
    dy30 = y3-y
    d[~iz] = (dx30*dx30 + dy30*dy30)[~iz]

    if is_detect_final:
        d[iz | (u > 1)] = np.nan

    if is_detect_initial:
        d[iz | (u < 0)] = np.nan

    return d


def is_inside_triangles(p, x1, y1, x2, y2, x3, y3):
    """
    Returns a binary vector with True if point p is 
    inside a triangle.
    x1,y1,x2,y2,x3,y3 are vectors with the 3 coordiantes of the triangles.
    """
    alpha = ((y2 - y3)*(p[0] - x3) + (x3 - x2)*(p[1] - y3)) \
        / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3))

    beta = ((y3 - y1)*(p[0] - x3) + (x1 - x3)*(p[1] - y3)) \
        / ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3))

    gamma = 1.0 - alpha - beta
    return (alpha > 0) & (beta > 0) & (gamma > 0)

# from http://www.arachnoid.com/area_irregular_polygon/index.html


def find_area_perim(array):
    """
    Scalar!
    """
    a = 0
    p = 0
    ox, oy = array[0]
    for x, y in array[1:]:
        a += (x*oy-y*ox)
        p += abs((x-ox)+(y-oy)*1j)
        ox, oy = x, y
    return a/2, p


def find_area(array, is_use_shapely=True):
    """
    Single polygon with 2D coordinates.
    """
    # TODO: check, there are negative A!!!!
    # print 'find_area',array
    if len(array) >= 3:
        if IS_SHAPELY & is_use_shapely:
            return Polygon(np.array(array)[:, :2]).area

        else:
            a = 0
            ox, oy = array[0]
            for x, y in array[1:]:
                a += (x*oy-y*ox)
                ox, oy = x, y

            # print '  =',np.abs(a/2)
            return np.abs(a/2)
    else:
        return 0.0


def get_polygonarea_fast(x, y):
    """
    Returns area in polygon represented by x,y vectors.
    Attention: last point is not first point.
    """
    # https://stackoverflow.com/questions/24467972/calculate-area-of-polygon-given-x-y-coordinates
    return 0.5*np.abs(np.dot(x, np.roll(y, 1))-np.dot(y, np.roll(x, 1)))


def is_point_in_polygon(point, poly, is_use_shapely=True):
    """
    Scalar!
    """
    if len(poly) >= 3:
        if IS_SHAPELY & is_use_shapely:
            pol = Polygon(np.array(poly)[:, :2])
            return Point(point[:2]).within(pol)
        else:
            is_3d = len(point) == 3

            if is_3d:
                x, y, z = point
                p1x, p1y, p1z = poly[0]
            else:
                x, y = point
                p1x, p1y = poly[0]
            n = len(poly)
            inside = False

            for i in range(n+1):
                if is_3d:
                    p2x, p2y, p2z = poly[i % n]
                else:
                    p2x, p2y = poly[i % n]
                if y > min(p1y, p2y):
                    if y <= max(p1y, p2y):
                        if x <= max(p1x, p2x):
                            if p1y != p2y:
                                xints = (y-p1y)*(p2x-p1x)/(p2y-p1y)+p1x
                            if p1x == p2x or x <= xints:
                                inside = not inside
                p1x, p1y = p2x, p2y

            return inside

    else:
        return False


def is_polyline_intersect_polygon(polyline, polygon, is_use_shapely=True, is_lineinterpolate=True):
    if IS_SHAPELY & is_use_shapely:
        poly = Polygon(np.array(polygon)[:, :2])
        if is_lineinterpolate:
            # requires that any line interpolation between 2 points
            #  intersects the polygon
            return LineString(np.array(polyline)[:, :2]).intersects(poly)
        else:
            # requires that at least one point is inside the polygon
            return MultiPoint(np.array(polyline)[:, :2]).intersects(poly)

    else:
        # WARNING: this dows not work if no point of the polyline
        # resides within the polygon
        for p in polyline:
            if is_point_in_polygon(p, polygon):
                return True
        return False


def is_polyline_in_polygon(polyline, polygon, is_use_shapely=True):
    if IS_SHAPELY & is_use_shapely:
        poly = Polygon(np.array(polygon)[:, :2])
        return MultiPoint(np.array(polyline)[:, :2]).within(poly)

    else:
        for p in polyline:
            if not is_point_in_polygon(p, polygon):
                return False
        return True


def get_angles_perpendicular(shape):
    """
    Returns the angle vector angles_perb which is perpendicular to the given shape.
    The normalized 
    dxn = np.cos(angles_perb)
    dyn = np.sin(angles_perb)
    """

    n_vert = len(shape)
    # print 'get_laneshapes',_id,n_lanes,n_vert

    #width = self.widths_lanes_default[_id]
    # print '  shape',  shape ,len(  shape)
    v_ext_begin = (shape[0]-(shape[1]-shape[0])).reshape(1, 3)
    v_ext_end = (shape[-1]+(shape[-1]-shape[-2])).reshape(1, 3)

    exshape = np.concatenate((v_ext_begin, shape, v_ext_end))[:, 0:2]
    # print '  exshape',  exshape,len(  exshape)
    vertex_delta_x = exshape[1:, 0]-exshape[0:-1, 0]
    vertex_delta_y = exshape[1:, 1]-exshape[0:-1, 1]

    angles = np.arctan2(vertex_delta_y, vertex_delta_x)
    #angles = np.mod(np.arctan2(vertex_delta_y,vertex_delta_x)+2*np.pi,2*np.pi)
    #angles_perb = 0.5 *(angles[1:]+angles[0:-1])-np.pi/2

    angles1 = angles[1:]
    angles2 = angles[0:-1]
    ind_discont = (angles1 < -0.5*np.pi) & ((angles2 > 0.5*np.pi)) | (angles2 < -0.5*np.pi) & ((angles1 > 0.5*np.pi))
    angle_sum = angles1+angles2
    angle_sum[ind_discont] += 2*np.pi

    #angles = np.mod(np.arctan2(vertex_delta_y,vertex_delta_x)+2*np.pi,2*np.pi)
    #angle_sum = angles[1:]+angles[0:-1]
    #ind_discont = angle_sum>2*np.pi
    #angle_sum[ind_discont] = angle_sum[ind_discont]-2*np.pi
    return 0.5*angle_sum-np.pi/2


def rotate_vertices(vec_x, vec_y, alphas=None,
                    sin_alphas=None, cos_alphas=None,
                    is_array=False):
    """
    Rotates all vertices around
    """
    # print 'rotate_vertices',vec_x, vec_y

    if alphas is not None:
        sin_alphas = np.sin(alphas)
        cos_alphas = np.cos(alphas)
    # print '  sin_alphas',sin_alphas
    # print '  cos_alphas',cos_alphas
    #deltas = vertices - offsets
    #vertices_rot = np.zeros(vertices.shape, np.float32)
    #n= size(vec_x)
    vec_x_rot = vec_x*cos_alphas - vec_y*sin_alphas
    vec_y_rot = vec_x*sin_alphas + vec_y*cos_alphas
    # print '  vec_x_rot',vec_x_rot
    # print '  vec_y_rot',vec_y_rot
    if is_array:
        # print '   concatenate', np.concatenate((vec_x_rot.reshape(-1,1), vec_y_rot.reshape(-1,1)),1)
        return np.concatenate((vec_x_rot.reshape(-1, 1), vec_y_rot.reshape(-1, 1)), 1)
    else:
        return vec_x_rot, vec_y_rot


T = np.array([[0, -1], [1, 0]])


def line_intersect(a1, a2, b1, b2):
    """
    Works for multiple points in each of the input arguments, i.e., a1, a2, b1, b2 can be Nx2 row arrays of 2D points.
    https://stackoverflow.com/questions/3252194/numpy-and-line-intersections
    """
    da = np.atleast_2d(a2 - a1)
    db = np.atleast_2d(b2 - b1)
    dp = np.atleast_2d(a1 - b1)
    dap = np.dot(da, T)
    denom = np.sum(dap * db, axis=1)
    num = np.sum(dap * dp, axis=1)
    return np.atleast_2d(num / denom).T * db + b1


def line_intersect2(a1, a2, b1, b2):
    """
    Works for multiple points in each of the input arguments, i.e., a1, a2, b1, b2 can be Nx2 row arrays of 2D points.
    https://stackoverflow.com/questions/3252194/numpy-and-line-intersections
    Returns also a scale factor which indicates wheter the intersection
    is in positive or negative direction of the b vector.
    """
    da = np.atleast_2d(a2 - a1)
    db = np.atleast_2d(b2 - b1)
    dp = np.atleast_2d(a1 - b1)
    dap = np.dot(da, T)
    denom = np.sum(dap * db, axis=1)
    num = np.sum(dap * dp, axis=1)
    la = np.atleast_2d(num / denom).T
    return la * db + b1, la


def get_diff_angle_clockwise(p1, p2):
    """
    Returns the clockwise angle between vector 
    0 -> p1 and 0 -> p2

    p1=np.array([[x11,x11,x13,...],[y11,y12,y13,...]])
    p2 =np.array([[x21,x21,x23,...],[y21,y22,y23,...]])

    """
    ang1 = np.arctan2(*p1[::-1])
    ang2 = np.arctan2(*p2[::-1])
    # return np.rad2deg((ang1 - ang2) % (2 * np.pi))
    if hasattr(ang1, '__iter__'):
        return anglediffs(ang1, ang2)
    else:
        return anglediff(ang1, ang2)
################################################################
# old


def anglediff(a1, a2):
    """Compute the smallest difference between two angle arrays.
    Parameters
    ----------
    a1, a2 : np.ndarray
        The angle arrays to subtract
    Returns
    -------
    out : np.ndarray
        The difference between a1 and a2
    """

    dtheta = a1 - a2
    while dtheta > np.pi:
        dtheta -= 2.0*np.pi
    while dtheta < -np.pi:
        dtheta += 2.0*np.pi

    return dtheta


def anglediffs(a1, a2, deg=False):
    """Compute the smallest difference between two angle arrays.
    Parameters
    ----------
    a1, a2 : np.ndarray
        The angle arrays to subtract
    deg : bool (default=False)
        Whether to compute the difference in degrees or radians
    Returns
    -------
    out : np.ndarray
        The difference between a1 and a2
    """
    print('anglediffs', a1, a2)
    return wrapanglediffs(a1 - a2, deg=deg)


def wrapanglediffs(diff, deg=False):
    """Given an array of angle differences, make sure that they lie
    between -pi and pi.
    Parameters
    ----------
    diff : np.ndarray
        The angle difference array
    deg : bool (default=False)
        Whether the angles are in degrees or radians
    Returns
    -------
    out : np.ndarray
        The updated angle differences
    """

    if deg:
        base = 360
    else:
        base = np.pi * 2

    i = np.abs(diff) > (base / 2.0)
    out = diff.copy()
    out[i] -= np.sign(diff[i]) * base
    return out

# find indees where 2 arrays are identical
#idx = np.argwhere(np.diff(np.sign(f - g)) != 0).reshape(-1) + 0


def azimut_from_origin(p, origin=[0, 0]):
    # determine the azimut angle relative to a 2D point
    p = np.array(p) - np.array(origin)
    if p[0] > 0 and p[1] > 0:
        return np.arctan(p[0]/p[1])
    if p[0] > 0 and p[1] < 0:
        return np.arctan(p[0]/p[1])+np.pi
    if p[0] < 0 and p[1] < 0:
        return np.arctan(p[0]/p[1])+np.pi
    if p[0] < 0 and p[1] > 0:
        return np.arctan(p[0]/p[1])+2*np.pi


def angle2D(p1, p2):
    theta1 = math.atan2(p1[1], p1[0])
    theta2 = math.atan2(p2[1], p2[0])
    #dtheta = theta2 - theta1;
    # while dtheta > np.pi:
    #    dtheta -= 2.0*np.pi
    # while dtheta < -np.pi:
    #    dtheta += 2.0*np.pi
    return wrapanglediffs(theta2 - theta1)


def is_point_within_polygon(pos, shape):
    angle = 0.
    pos = np.array(pos, float)
    shape = np.array(shape, float)
    for i in range(0, len(shape)-1):
        p1 = ((shape[i][0] - pos[0]), (shape[i][1] - pos[1]))
        p2 = ((shape[i+1][0] - pos[0]), (shape[i+1][1] - pos[1]))
        angle = angle + angle2D(p1, p2)
    i = len(shape)-1
    p1 = ((shape[i][0] - pos[0]), (shape[i][1] - pos[1]))
    p2 = ((shape[0][0] - pos[0]), (shape[0][1] - pos[1]))
    angle = angle + angle2D(p1, p2)
    return math.fabs(angle) >= np.pi


if __name__ == '__main__':
    a1 = 0.0
    a2 = +0.1
    print('a1', a1/np.pi*180)
    print('a2', a2/np.pi*180)
    print('anglediff(a1, a2)', anglediff(a1, a2)/np.pi*180)
