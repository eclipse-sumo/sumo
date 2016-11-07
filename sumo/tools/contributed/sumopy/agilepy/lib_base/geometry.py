import numpy as np


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
        lengths[i] = np.sum(np.sqrt(np.sum((v[:, 1, :] - v[:, 0, :])**2, 1)))
        i += 1
    return lengths


def get_length_polypoints(polylines):
    # print 'get_length_polypoints'
    # v = np.array([[[0.0,0.0,0.0],[1,0.0,0.0]],[[1,0.0,0.0],[1,2,0.0]],[[1,2.0,0.0],[1,2,3.0]] ])
    lengths = np.zeros(len(polylines), np.float)
    i = 0
    for v in polylines:
        # print '  v=\n',v,v.shape
        a = np.array(v, np.float32)
        lengths[i] = np.sum(np.sqrt(np.sum((a[1:, :] - a[:-1, :])**2, 1)))
        i += 1
    return lengths


def polypoints_to_polylines(polypoints):
    # np.zeros((0,2,3),np.float32)
    linevertices = np.array([None] * len(polypoints), np.object)
    #polyinds = []
    #lineinds = []

    ind = 0
    ind_line = 0
    for polyline in polypoints:
        # Important type conversion!!
        v = np.zeros((2 * len(polyline) - 2, 3), np.float32)
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

    for j in xrange(1, len(polyline)):
        x2, y2, z2 = polyline[j]
        seglength = np.linalg.norm([x2 - x1, y2 - y1])
        pos_edge += seglength

        if (pos >= pos_edge_pre) & (pos <= pos_edge):

            dxn = (x2 - x1) / seglength
            dyn = (y2 - y1) / seglength
            u1 = (pos - pos_edge_pre)

            u2 = (pos + length - pos_edge_pre)
            return [[x1 + u1 * dxn, y1 + u1 * dyn, z2], [x1 + u2 * dxn, y1 + u2 * dyn, z2]]

        x1, y1 = x2, y2
        pos_edge_pre = pos_edge

    x1, y1, z1 = polyline[-1]
    dxn = (x2 - x1) / seglength
    dyn = (y2 - y1) / seglength
    u1 = (pos - pos_edge_pre)
    u2 = (pos + length - pos_edge_pre)
    return [[x2, y2, z2], [x2 + u2 * dxn, y1 + u2 * dyn, z2]]


def get_coord_on_polyline_from_pos(polyline, pos):
    pos_edge = 0.0
    pos_edge_pre = 0.0
    x1, y1, z1 = polyline[0]

    for j in xrange(1, len(polyline)):
        x2, y2, z2 = polyline[j]
        length = np.linalg.norm([x2 - x1, y2 - y1])
        pos_edge += length

        if (pos >= pos_edge_pre) & (pos <= pos_edge):
            u = (pos - pos_edge_pre) / length
            x = x1 + u * (x2 - x1)
            y = y1 + u * (y2 - y1)
            return x, y, z2

        x1, y1 = x2, y2
        pos_edge_pre = pos_edge

    return x2, y2, z2


def get_pos_on_polyline_from_coord(polyline, coord):
    xc, yx, zc = coord
    n_segs = len(polyline)

    d_min = 10.0**8
    x_min = 0.0
    y_min = 0.0
    j_min = 0
    p_min = 0.0
    pos = 0.0
    x1, y1, z1 = shape[0]
    for j in xrange(1, n_segs):
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
        pos += np.linalg.norm([x2 - x1, y2 - y1])
        x1, y1 = x2, y2

    x1, y1 = polyline[j_min - 1]
    return p_min + np.linalg.norm([x_min - x1, y_min - y1])


def shortest_dist(x1, y1, x2, y2, x3, y3):  # x3,y3 is the point
    """
    Shortest distance between point (x3,y3) and line (x1,y1-> x2,y2).
    Returns distance and projected point on line.
    """

    px = x2 - x1
    py = y2 - y1

    something = px * px + py * py
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

    dist = np.sqrt(dx * dx + dy * dy)

    return dist, x, y


def get_dist_point_to_segs(p, y1, x1, y2, x2, is_ending=True):
    """
    Minimum Distance between a Point p = (x,y) and a Line segments ,
    where vectors x1, y1 are the first  points and x2,y2 are the second points 
    of the line segments.
    Written by Paul Bourke,    October 1988
    http://astronomy.swin.edu.au/~pbourke/geometry/pointline/

    Rewritten in vectorial form by Joerg Schweizer
    """

    y3, x3 = p

    d = np.zeros(len(y1), dtype=np.float32)

    dx21 = (x2 - x1)
    dy21 = (y2 - y1)

    lensq21 = dx21 * dx21 + dy21 * dy21

    # indexvector for all zero length lines
    iz = (lensq21 == 0)

    dy = y3 - y1[iz]
    dx = x3 - x1[iz]

    d[iz] = dx * dx + dy * dy

    lensq21[iz] = 1.0  # replace zeros with 1.0 to avoid div by zero error

    u = (x3 - x1) * dx21 + (y3 - y1) * dy21
    u = u / lensq21

    x = x1 + u * dx21
    y = y1 + u * dy21

    if is_ending:
        ie = u < 0
        x[ie] = x1[ie]
        y[ie] = y1[ie]
        ie = u > 1
        x[ie] = x2[ie]
        y[ie] = y2[ie]

    dx30 = x3 - x
    dy30 = y3 - y
    d[~iz] = (dx30 * dx30 + dy30 * dy30)[~iz]
    return d


def is_inside_triangles(p, x1, y1, x2, y2, x3, y3):
    """
    Returns a binary vector with True if point p is 
    inside a triangle.
    x1,y1,x2,y2,x3,y3 are vectors with the 3 coordiantes of the triangles.
    """
    alpha = ((y2 - y3) * (p[0] - x3) + (x3 - x2) * (p[1] - y3)) \
        / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3))

    beta = ((y3 - y1) * (p[0] - x3) + (x1 - x3) * (p[1] - y3)) \
        / ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3))

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
        a += (x * oy - y * ox)
        p += abs((x - ox) + (y - oy) * 1j)
        ox, oy = x, y
    return a / 2, p


def find_area(array):
    """
    Single polygon with 2D coordinates.
    """
    # TODO: check, there are negative A!!!!
    # print 'find_area',array
    a = 0
    ox, oy = array[0]
    for x, y in array[1:]:
        a += (x * oy - y * ox)
        ox, oy = x, y

    # print '  =',np.abs(a/2)
    return np.abs(a / 2)


def is_point_in_polygon(point, poly):
    """
    Scalar!
    """
    is_3d = len(point) == 3

    if is_3d:
        x, y, z = point
        p1x, p1y, p1z = poly[0]
    else:
        x, y = point
        p1x, p1y = poly[0]
    n = len(poly)
    inside = False

    for i in range(n + 1):
        if is_3d:
            p2x, p2y, p2z = poly[i % n]
        else:
            p2x, p2y = poly[i % n]
        if y > min(p1y, p2y):
            if y <= max(p1y, p2y):
                if x <= max(p1x, p2x):
                    if p1y != p2y:
                        xints = (y - p1y) * (p2x - p1x) / (p2y - p1y) + p1x
                    if p1x == p2x or x <= xints:
                        inside = not inside
        p1x, p1y = p2x, p2y

    return inside


def is_polyline_intersect_polygon(polyline, polygon):
    for p in polyline:
        if is_point_in_polygon(p, polygon):
            return True
    return False


def is_polyline_in_polygon(polyline, polygon):
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
    v_ext_begin = (shape[0] - (shape[1] - shape[0])).reshape(1, 3)
    v_ext_end = (shape[-1] + (shape[-1] - shape[-2])).reshape(1, 3)

    exshape = np.concatenate((v_ext_begin, shape, v_ext_end))[:, 0:2]
    # print '  exshape',  exshape,len(  exshape)
    vertex_delta_x = exshape[1:, 0] - exshape[0:-1, 0]
    vertex_delta_y = exshape[1:, 1] - exshape[0:-1, 1]

    angles = np.arctan2(vertex_delta_y, vertex_delta_x)
    #angles = np.mod(np.arctan2(vertex_delta_y,vertex_delta_x)+2*np.pi,2*np.pi)
    #angles_perb = 0.5 *(angles[1:]+angles[0:-1])-np.pi/2

    angles1 = angles[1:]
    angles2 = angles[0:-1]
    ind_discont = (angles1 < -0.5 * np.pi) & ((angles2 > 0.5 * np.pi)
                                              ) | (angles2 < -0.5 * np.pi) & ((angles1 > 0.5 * np.pi))
    angle_sum = angles1 + angles2
    angle_sum[ind_discont] += 2 * np.pi

    #angles = np.mod(np.arctan2(vertex_delta_y,vertex_delta_x)+2*np.pi,2*np.pi)
    #angle_sum = angles[1:]+angles[0:-1]
    #ind_discont = angle_sum>2*np.pi
    #angle_sum[ind_discont] = angle_sum[ind_discont]-2*np.pi
    return 0.5 * angle_sum - np.pi / 2


################################################################
# old
def angle2D(p1, p2):
    theta1 = math.atan2(p1[1], p1[0])
    theta2 = math.atan2(p2[1], p2[0])
    dtheta = theta2 - theta1
    while dtheta > 3.1415926535897932384626433832795:
        dtheta -= 2.0 * 3.1415926535897932384626433832795
    while dtheta < -3.1415926535897932384626433832795:
        dtheta += 2.0 * 3.1415926535897932384626433832795
    return dtheta


def is_point_within_polygon(pos, shape):
    angle = 0.
    pos = np.array(pos, float)
    shape = np.array(shape, float)
    for i in range(0, len(shape) - 1):
        p1 = ((shape[i][0] - pos[0]), (shape[i][1] - pos[1]))
        p2 = ((shape[i + 1][0] - pos[0]), (shape[i + 1][1] - pos[1]))
        angle = angle + angle2D(p1, p2)
    i = len(shape) - 1
    p1 = ((shape[i][0] - pos[0]), (shape[i][1] - pos[1]))
    p2 = ((shape[0][0] - pos[0]), (shape[0][1] - pos[1]))
    angle = angle + angle2D(p1, p2)
    return math.fabs(angle) >= 3.1415926535897932384626433832795
