#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    main.py
@author  Marek Heinrich
@date    2014-11-17
@version $Id$

Main module.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import sys
import inspect
try:
    from PIL import Image
    from PIL import ImageDraw
    from PIL import ImageColor
    haveImaging = True
except ImportError:
    print >> sys.stderr, "No imaging"
    haveImaging = False
    pass
import numpy
import math

from collections import namedtuple

from dijkstra_base import Dijkstra
from commons import NodeDataHandler, StarNodeC, ANList


MAX_CELL_EXTENTION = 5


class Vessel():

    def __init__(self, flaeche, hull_points=None):
        if hull_points is None:
            self.hull_points = [
                (0, 0), (10, 10), (10, 30), (-10, 30), (-10, 10), (0, 0)]
        else:
            self.hull_points = hull_points
#        self.hull_points = [(0, 0), (1, 1), (1, 3), (-1, 3), (-1, 1), ( 0, 0)]
        self.transformed_hull_points = self.hull_points
        self.flaeche = flaeche
        self.rotation = 0
        self.driving = None  # einschlag???
        self.x = None
        self.y = None
        self.r = None
        self.predefined_shadow_shapes = []
        self.predefined_shadow_shapes_blured = []

        self.generate_predefined_shadow_shapes()

    def physics_stearing(self, driving, delta):
        delta = float(delta)

        driving_sign = 1 if driving >= 0 else -1

        if driving == 0:
            x_stroke = delta
            y_stroke = 0
            delta_angle = 0

        else:

            driving_radius = self.r / abs(driving)
            delta_angle = delta / driving_radius

            if False:
                print
                print 'driving_sign', driving_sign
                print 'driving_radius', driving_radius

                print 'delta_angle', delta_angle
                print 'math.cos(delta_angle', math.cos(delta_angle)
                print '(1 - math.cos(delta_angle))', (1 - math.cos(delta_angle))

            x_stroke = math.sin(delta_angle) * driving_radius
            y_stroke = (1 - math.cos(delta_angle)) * \
                driving_sign * driving_radius

        return self.transform_coord_and_angle((x_stroke, y_stroke,
                                               driving_sign * delta_angle),
                                              self.rotation,
                                              (self.x, self.y))

    def get_num_hull_segments(self):
        return len(self.transformed_hull_points) - 1

    def get_hull_segment(self, num=0):
        assert num < self.get_num_hull_segments(), "hull segment out of range"
        return ((self.transformed_hull_points[num], self.transformed_hull_points[num + 1]))

    def get_hull_segments(self):
        return [self.get_hull_segment(seg) for seg in range(self.get_num_hull_segments())]

    def transform_coord_and_angle(self, p, omega, offset):
        new_x, new_y = self.transform_coord((p[0], p[1]), omega, offset)
        return (new_x, new_y, p[2] + omega)

    def transform_coord(self, p, omega, offset):
        #        scale = 1
        xx = float(p[0])
        yy = float(p[1])
        xx_0 = offset[0]
        yy_0 = offset[1]
        if xx == 0 and yy == 0:
            ret_x = xx_0
            ret_y = yy_0
        else:
            ret_x = (
                math.sqrt(xx**2 + yy**2) * math.cos(math.atan2(yy, xx) + omega)) + xx_0
            ret_y = (
                math.sqrt(xx**2 + yy**2) * math.sin(math.atan2(yy, xx) + omega)) + yy_0
        return (ret_x, ret_y)

    def transform_coord_to_ego(self, p, omega, offset):
        """transform from global to ego"""
        xx_0 = offset[0]
        yy_0 = offset[1]
        XX = float(p[0])
        YY = float(p[1])
        xx = XX - xx_0
        yy = YY - yy_0

        ret_x_stroke = (
            math.sqrt(xx**2 + yy**2) * math.cos(math.atan2(yy, xx) - omega))
        ret_y_stroke = (
            math.sqrt(xx**2 + yy**2) * math.sin(math.atan2(yy, xx) - omega))

        return (ret_x_stroke, ret_y_stroke)

    def transform_hull_points(self, omega, offset):
        self.transformed_hull_points = [self.transform_coord(ii, omega, offset)
                                        for ii in self.hull_points]

        return self.transformed_hull_points

    def generate_predefined_shadow_shapes(self, verbose=False):
        """computes the shadowed shapes for all secotors """

        # save the hull_points as they were before
        transformed_hull_points_before = self.transformed_hull_points

        # compute the shadowed cells for the center of each sector

        for sector in range(self.flaeche.sectors):
            self.predefined_shadow_shapes.append(self.get_black_shade(
                self.flaeche.get_angle_from_sector_id(sector),
                (0.5 * self.flaeche.scale, 0.5 * self.flaeche.scale),
                visual=False))

        # blur
        # compute the blured shadowed cells for each sector,
        # these are the cells that are shadowed from the center of the
        #  * current sector
        #  * the sectors left and right
        # in case a vessel is very tight at the edge of the current sector,
        # the shadow of the center might not be accurate

        blur = True
        if blur:
            for sector_center in range(self.flaeche.sectors):

                # create an empty entry - not using default_dict
                self.predefined_shadow_shapes_blured.append(None)

                sector_left = sector_center - 1
                sector_right = sector_center + 1

                if sector_left < 0:
                    sector_left += self.flaeche.sectors
                if sector_left >= self.flaeche.sectors:
                    sector_left -= self.flaeche.sectors

                if sector_right < 0:
                    sector_right += self.flaeche.sectors
                if sector_right >= self.flaeche.sectors:
                    sector_right -= self.flaeche.sectors

                if verbose:

                    print
                    print sector_center
                    print 'sector_left', sector_left
                    print 'sector_right', sector_right

                    print self.predefined_shadow_shapes[sector_left]
                    print self.predefined_shadow_shapes[sector_center]
                    print self.predefined_shadow_shapes[sector_right]

                # create an empty entry - not using default_dict
                self.predefined_shadow_shapes_blured.append(None)
                self.predefined_shadow_shapes_blured[sector_center] = list(
                    set().union(*[self.predefined_shadow_shapes[sector_left],
                                  self.predefined_shadow_shapes[sector_center],
                                  self.predefined_shadow_shapes[sector_right]]
                                ))
                if verbose:
                    print self.predefined_shadow_shapes_blured[sector_center]

        # reset to org. hull_points
        self.transformed_hull_points = transformed_hull_points_before

    def get_predefiend_shadow_shape_negatives(self, sector_id):
        """returns the precomputed shadowed shapes of the given secotor """

        # check sector_id
        self.flaeche.is_valid_sector_id(sector_id)

        # return a predefined shadow
        return self.predefined_shadow_shapes_blured[sector_id]

    def get_predefiend_shadow_shape_from_cell_id(self, cell_id):
        """returns the precomputed shadowed shapes for the given position andgiven secotor """

        # check if sector_id is a tuple
        if isinstance(cell_id, str):
            cell_id = self.flaeche.convert_cell_id_to_tuple(cell_id)

        # check sector_id
        self.flaeche.is_valid_node_pos(cell_id)

        cell_id_x, cell_id_y, sector_id = cell_id

        blured_default = self.predefined_shadow_shapes_blured[sector_id]
        blured_for_this_cell = [(node[0] + cell_id_x, node[1] + cell_id_y)
                                for node in blured_default]

        # return the predefinined shadow
        return sorted(blured_for_this_cell)

    def guess_best_way_to_point(self, from_point, omega_from, to_point,
                                guess='best', verbose=False):

        # convert points to ego_coordinates

        to_point_ego = self.transform_coord_to_ego(p=to_point,
                                                   omega=omega_from, offset=from_point)
        x_to_ego, y_to_ego = to_point_ego

        # calculate guess, using left turn

        center_to_destination_left = math.sqrt(
            (x_to_ego)**2 + (y_to_ego + self.r)**2)

        if center_to_destination_left >= self.r:

            angle_in_rectangle_left = math.acos(
                self.r / center_to_destination_left)
            angle_to_destination_left = math.atan2(
                -y_to_ego - self.r, x_to_ego) + 0.5 * math.pi
            if angle_to_destination_left < 0:
                angle_to_destination_left += 2 * math.pi

            angle_to_travel_left = (angle_to_destination_left -
                                    angle_in_rectangle_left)

            while angle_to_travel_left >= 2 * math.pi:
                angle_to_travel_left -= 2 * math.pi

            while angle_to_travel_left < 0:
                angle_to_travel_left += 2 * math.pi

            angle_to_travel_distance_left = angle_to_travel_left * self.r
            tangent_distance_left = math.sqrt(center_to_destination_left**2
                                              - self.r**2)
            total_distance_left = (tangent_distance_left
                                   + angle_to_travel_distance_left)

            if verbose:
                print
                print
                print 'from_point, to_point', from_point, to_point
                print 'x_to_ego:', x_to_ego, (x_to_ego)**2
                print 'y_to_ego:', y_to_ego, 'y_to_ego + r :', y_to_ego + self.r, (y_to_ego
                                                                                   + self.r)**2

                print 'r:', self.r, 'center_to_dest', center_to_destination_left

                print 'angle_in_rectangle_left', angle_in_rectangle_left * 180 / math.pi
                print 'angle_to_destination_left', angle_to_destination_left * 180 / math.pi
                print 'angle_to_travel_left', angle_to_travel_left * 180 / math.pi
                print 'tangent_distance_left', tangent_distance_left
                print 'total_distance_left', total_distance_left

        else:
            angle_to_travel_left = None
            angle_to_travel_distance_left = None
            tangent_distance_left = None
            total_distance_left = None

        # calculate guess, using right turn

        center_to_destination_right = math.sqrt(
            (x_to_ego)**2 + (y_to_ego - self.r)**2)

        if center_to_destination_right >= self.r:
            angle_in_rectangle_right = math.acos(
                self.r / center_to_destination_right)
#            angle_to_destination_right      = math.atan2( -y_to_ego + self.r, x_to_ego )
            angle_to_destination_right = - \
                math.atan2(-y_to_ego + self.r, x_to_ego) + 0.5 * math.pi

            if angle_to_destination_right < 0:
                angle_to_destination_right += 2 * math.pi

            angle_to_travel_right = angle_to_destination_right - \
                angle_in_rectangle_right

            while angle_to_travel_right >= 2 * math.pi:
                angle_to_travel_right -= 2 * math.pi

            while angle_to_travel_right < 0:
                angle_to_travel_right += 2 * math.pi

            angle_to_travel_distance_right = angle_to_travel_right * self.r
            tangent_distance_right = math.sqrt(center_to_destination_right**2
                                               - self.r**2)
            total_distance_right = (tangent_distance_right
                                    + angle_to_travel_distance_right)

            if verbose:
                print
                print
                print 'from_point, to_point', from_point, to_point
                print 'x_to_ego:', x_to_ego, (x_to_ego)**2
                print 'y_to_ego:', y_to_ego, 'y_to_ego + r :', y_to_ego + self.r,  (y_to_ego
                                                                                    + self.r)**2

                print 'r:', self.r, 'center_to_dest_right', center_to_destination_right

                print 'angle_in_rectangle_right', angle_in_rectangle_right * 180 / math.pi
                print 'angle_to_destination_right', angle_to_destination_right * 180 / math.pi
                print 'angle_to_travel_right', angle_to_travel_right * 180 / math.pi
                print 'tangent_distance_right', tangent_distance_right
                print 'total_distance_right', total_distance_right

        else:
            angle_to_travel_right = None
            angle_to_travel_distance_right = None
            tangent_distance_right = None
            total_distance_right = None

        # evaluate guesses

        return_dict = {'left':  ('left',
                                 angle_to_travel_left, angle_to_travel_distance_left,
                                 tangent_distance_left, total_distance_left),
                       'right': ('right',
                                 angle_to_travel_right, angle_to_travel_distance_right,
                                 tangent_distance_right, total_distance_right)
                       }

        if guess == 'best':
            if total_distance_left is None:
                guess = 'right'
            elif total_distance_right is None:
                guess = 'left'
            elif total_distance_left < total_distance_right:
                guess = 'left'
            else:
                guess = 'right'

        assert (return_dict['left'] is not None and
                return_dict['right'] is not None), 'point is inreachable -eighter left or right'

        return return_dict[guess]

    def guess_best_way_to_point_old(self, from_point, omega_from, to_point,
                                    guess='best', verbose=False):

        # convert points to ego_coordinates

        to_point_ego = self.transform_coord_to_ego(p=to_point,
                                                   omega=omega_from, offset=from_point)
        x_to_ego, y_to_ego = to_point_ego

        # calculate guess, using left turn

        center_to_destination_left = math.sqrt(
            (x_to_ego)**2 + (y_to_ego + self.r)**2)

        if center_to_destination_left >= self.r:

            angle_in_rectangle_left = math.acos(
                self.r / center_to_destination_left)
#            angle_in_rectangle_left        = math.asin(self.r/center_to_destination_left)
            angle_to_destination_left = math.atan2(-
                                                   y_to_ego - self.r, x_to_ego)
            if angle_to_destination_left < 0:
                angle_to_destination_left += 2 * math.pi

            angle_to_travel_left = (angle_to_destination_left -
                                    angle_in_rectangle_left + math.pi / 2)

            while angle_to_travel_left >= 2 * math.pi:
                angle_to_travel_left -= 2 * math.pi

            angle_to_travel_distance_left = angle_to_travel_left * self.r
            tangent_distance_left = math.sqrt(center_to_destination_left**2
                                              - self.r**2)
            total_distance_left = (tangent_distance_left
                                   + angle_to_travel_distance_left)

            if verbose:
                print
                print
                print 'from_point, to_point', from_point, to_point
                print 'x_to_ego:', x_to_ego, (x_to_ego)**2
                print 'y_to_ego:', y_to_ego, 'y_to_ego + r :', y_to_ego + self.r, (y_to_ego
                                                                                   + self.r)**2

                print 'r:', self.r, 'center_to_dest', center_to_destination_left

                print 'angle_in_rectangle_left', angle_in_rectangle_left * 180 / math.pi
                print 'angle_to_destination_left', angle_to_destination_left * 180 / math.pi
                print 'angle_to_travel_left', angle_to_travel_left * 180 / math.pi
                print 'tangent_distance_left', tangent_distance_left
                print 'total_distance_left', total_distance_left

        else:
            angle_to_travel_left = None
            angle_to_travel_distance_left = None
            tangent_distance_left = None
            total_distance_left = None

        # calculate guess, using right turn

        center_to_destination_right = math.sqrt(
            (x_to_ego)**2 + (y_to_ego - self.r)**2)

        if center_to_destination_right >= self.r:
            angle_in_rectangle_right = math.acos(
                self.r / center_to_destination_right)
            angle_to_destination_right = math.atan2(
                -y_to_ego + self.r, x_to_ego)

            if angle_to_destination_right > 0:
                angle_to_destination_right -= 2 * math.pi

            angle_to_destination_right -= 0.5 * math.pi
            angle_to_travel_right = angle_to_destination_right + \
                angle_in_rectangle_right
            angle_to_travel_right = -angle_to_travel_right

            while angle_to_travel_right >= 2 * math.pi:
                angle_to_travel_right -= 2 * math.pi

            angle_to_travel_distance_right = angle_to_travel_right * self.r
            tangent_distance_right = math.sqrt(center_to_destination_right**2
                                               - self.r**2)
            total_distance_right = (tangent_distance_right
                                    + angle_to_travel_distance_right)

            if verbose:
                print
                print
                print 'from_point, to_point', from_point, to_point
                print 'x_to_ego:', x_to_ego, (x_to_ego)**2
                print 'y_to_ego:', y_to_ego, 'y_to_ego + r :', y_to_ego + self.r,  (y_to_ego
                                                                                    + self.r)**2

                print 'r:', self.r, 'center_to_dest_right', center_to_destination_right

                print 'angle_in_rectangle_right', angle_in_rectangle_right * 180 / math.pi
                print 'angle_to_destination_right', angle_to_destination_right * 180 / math.pi
                print 'angle_to_travel_right', angle_to_travel_right * 180 / math.pi
                print 'tangent_distance_right', tangent_distance_right
                print 'total_distance_right', total_distance_right

        else:
            angle_to_travel_right = None
            angle_to_travel_distance_right = None
            tangent_distance_right = None
            total_distance_right = None

        # evaluate guesses

        return_dict = {'left':  ('left',
                                 angle_to_travel_left, angle_to_travel_distance_left,
                                 tangent_distance_left, total_distance_left),
                       'right': ('right',
                                 angle_to_travel_right, angle_to_travel_distance_right,
                                 tangent_distance_right, total_distance_right)
                       }

        if guess == 'best':
            if total_distance_left is None:
                guess = 'right'
            elif total_distance_right is None:
                guess = 'left'
            elif total_distance_left < total_distance_right:
                guess = 'left'
            else:
                guess = 'right'

        assert (return_dict['left'] is not None and
                return_dict['right'] is not None), 'points is inreachable -eighter left or right'

        return return_dict[guess]

    def get_inclination(self, lower, upper):
        # check that this has not been the last point
        #start_point = self.my_transformed_hull_points[ii]
        #end_point = self.my_transformed_hull_points[ii+1]

        upper_x = numpy.float(upper[0])
        upper_y = numpy.float(upper[1])
        lower_x = numpy.float(lower[0])
        lower_y = numpy.float(lower[1])

        # identical points
        if (upper_x == lower_x and
                upper_y == lower_y):

            raise StandardError("points are identical")

        # horizontal line
        elif upper_y == lower_y:
            inclination = float(0)

        # vertical - infinite incl.
        else:

            upper_x = numpy.float64(upper[0])
            upper_y = numpy.float64(upper[1])
            lower_x = numpy.float64(lower[0])
            lower_y = numpy.float64(lower[1])

            inclination = (upper_y - lower_y) / (upper_x - lower_x)

        return inclination

#    def get_intersection_points(self, start, end, scale):
    def get_intersection_points(self, start, end):
        scale = self.flaeche.scale
        if start[0] < end[0]:
            start_x, start_y = start
            end_x, end_y = end
        else:
            start_x, start_y = end
            end_x, end_y = start

        norm = math.sqrt((start_x - end_x)**2 + (start_y - end_y)**2)

        assert scale > 0, "scale should not be zero"

        incl = self.get_inclination((start_x, start_y), (end_x, end_y))
        intersects_x = []
        intersects_y = []

        if str(incl) == 'inf' or str(incl) == '-inf':
            neg_incl = -1 if str(incl) == '-inf' else 1

            nn = 0
            while True:
                current_y = (math.floor(start_y / scale)) * \
                    scale + + neg_incl * nn * scale
                current_norm = abs(start_y - current_y)

                if current_norm > norm:
                    break

                if ((current_y >= min(start_y, end_y) and current_y <= max(start_y, end_y))):
                    intersects_y.append((start_x, current_y))

                nn += 1

        else:
            nn = 0

            offset_y = start_y - incl * start_x
            neg_incl = -1 if incl < 0 else 1

            if incl == 0:
                neg_incl = -1 if start_x > end_x else 1

            while True:
                current_x = (math.floor(start_x / scale)) * \
                    scale + neg_incl * nn * scale
                current_y = incl * current_x + offset_y

                current_norm = math.sqrt(
                    (start_x - current_x)**2 + (start_y - current_y)**2)

                if current_norm > norm:
                    break

                if ((current_x >= min(start_x, end_x) and current_x <= max(start_x, end_x)) and
                        (current_y >= min(start_y, end_y) and current_y <= max(start_y, end_y))):
                    intersects_x.append((current_x, current_y))
                nn += 1

            if incl == 0:
                pass
            else:
                nn = 0
                while True:
                    current_y = (math.floor(start_y / scale)) * \
                        scale + neg_incl * nn * scale
                    current_x = (current_y - offset_y) / incl

                    current_norm = math.sqrt(
                        (start_x - current_x)**2 + (start_y - current_y)**2)

                    if current_norm > norm:
                        break

                    if ((current_x >= min(start_x, end_x) and current_x <= max(start_x, end_x)) and
                            (current_y >= min(start_y, end_y) and current_y <= max(start_y, end_y))):
                        intersects_y.append((current_x, current_y))
                    nn += 1

        return (sorted(intersects_x), sorted(intersects_y))

    def get_grey_shade(self, omega, offset, visual=False):

        self.transform_hull_points(omega, offset)

        intersection_points = []
        for hull_seg in self.get_hull_segments():
            intersection_points_x, intersection_points_y = self.get_intersection_points(
                hull_seg[0], hull_seg[1])
            intersection_points += intersection_points_x
            intersection_points += intersection_points_y
        greys = []

        for ip in intersection_points:
            greys_tmp = self.flaeche.get_cells(ip)
            for gt in greys_tmp:
                if gt not in greys:
                    greys.append(gt)

        if visual:
            ii = 0
            for gg in greys:
                ii += 1
                self.flaeche.vis_add_colored_point(gg, 'gray')

        return greys

    def get_black_shade(self, omega, offset, visual=False):
        self.transform_hull_points(omega, offset)

        blacks = []
        greys = sorted(self.get_grey_shade(omega, offset, visual))

        min_max = {}

        for gg in greys:
            if gg[1] not in min_max:
                min_max.update({gg[1]: (gg[0], gg[0])})
            else:
                old_min_y = min_max[gg[1]][0]
                old_max_y = min_max[gg[1]][1]
                if gg[0] < old_min_y:
                    min_max.update({gg[1]: (gg[0], old_max_y)})
                elif gg[0] > old_max_y:
                    min_max.update({gg[1]: (old_min_y, gg[0])})
                else:
                    pass

        for gg in min_max:
            blacks += [(rr, gg)
                       for rr in range(min_max[gg][0], min_max[gg][1] + 1)]

        if visual:
            ii = 0
            for gg in blacks:
                ii += 1
                self.flaeche.vis_add_colored_point(gg, (0, 0, 0), 'black')

        return sorted(blacks)

    def get_radius_to_point_ego(self, point):
        """always in ego coordinates"""

        xx = float(point[0])
        yy = float(point[1])

        def nearly_zero(num):
            digit = 8
            if math.floor(abs(num * 10**digit)) == 0:
                return True
            else:
                return False

        # case1) x == 0, y == 0
        assert not (nearly_zero(xx) and nearly_zero(yy)), """points must be different
        (0,0) means use the same point"""

        # case2) x == 0 ; y != 0; 180° turn
        if nearly_zero(xx):
            rr = abs(yy / 2)

            if yy > 0:
                aa = math.pi
            else:
                aa = - math.pi

        # case3) yy == 0; x != 0; means go straight forward or backward
        elif nearly_zero(yy):
            if xx > 0:
                rr = float('inf')
                aa = 0
            elif xx < 0:
                rr = float('-inf')
                aa = 0
            else:
                assert False, """yy is zero so xx must not be zero,
                this case should have been already handled before"""

        else:
            aa = 2 * math.atan2(yy, xx)
        #    if yy < 0:
        #        aa = -aa
            rr = abs(
                math.sqrt(xx**2 + yy**2) / 2 / math.sin(math.atan2(yy, xx)))

#            print 'xx', xx
#            print 'yy', yy
#            print 'aa', aa
#            print 'ag', aa * 360 / 2 / math.pi

        return (rr, aa)

    def get_points_on_line(self, start_point, end_point, omega, scale_fraction=10):
        """return multiple points along a line between start end point
           there is no difference between global and ego coordinates

           the angle omega has no impact on the calculations, but is appended to the
           result, so the values returned have the same format as when used
           get_points_on_the_circle_ego
        """

        some_points = []
        start_point_x, start_point_y = start_point
        end_point_x,   end_point_y = end_point

        start_point_x = float(start_point_x)
        start_point_y = float(start_point_y)
        end_point_x = float(end_point_x)
        end_point_y = float(end_point_y)

        assert(start_point != end_point), "points must be different"

        if start_point_x == end_point_x:
            full_way = end_point_y - start_point_y
            num_steps = int(
                math.floor(abs(full_way / (self.flaeche.scale / scale_fraction))))
            delta_y = full_way / num_steps

            for step in range(num_steps + 1):
                some_point_x = start_point_x
                some_point_y = start_point_y + step * delta_y
                some_points.append((some_point_x, some_point_y, omega))

        else:
            inclination = (end_point_y - start_point_y) / \
                (end_point_x - start_point_x)
            full_way = math.sqrt(
                (end_point_y - start_point_y)**2 + (end_point_x - start_point_x)**2)
            num_steps = int(
                math.floor(full_way / (self.flaeche.scale / scale_fraction)))
            delta_x = (end_point_x - start_point_x) / num_steps

            for step in range(num_steps + 1):
                some_point_x = start_point_x + step * delta_x
                some_point_y = start_point_y + step * delta_x * inclination
                some_points.append((some_point_x, some_point_y, omega))

        return sorted(some_points)

    def get_points_on_the_circle_ego(self, radius_to_point, angle_ego, scale_fraction=10):
        """return multiple points along the circle in ego coordinates

        radius to point = the radius which is needed to reach the point
        angle ego       = the angle traveled untill the point is reached
        """

        some_points = []
        some_point_x, some_point_y, current_partly_angle = (0.0, 0.0, 0.0)

        full_way = abs(radius_to_point * angle_ego)
        num_steps = math.floor(
            full_way / (self.flaeche.scale / scale_fraction))
        partly_angle = angle_ego / num_steps

        while (abs(current_partly_angle) < abs(angle_ego)):
            some_points.append(
                (some_point_x, some_point_y, current_partly_angle))
            current_partly_angle += partly_angle
            if current_partly_angle >= 0:
                some_point_x = radius_to_point * math.sin(current_partly_angle)
            else:
                some_point_x = radius_to_point * \
                    math.sin(current_partly_angle + math.pi)
            if current_partly_angle >= 0:
                some_point_y = radius_to_point * \
                    (1 - math.cos(current_partly_angle))
            else:
                some_point_y = radius_to_point * \
                    (math.cos(current_partly_angle) - 1)

        return some_points

    def get_reachables(self, step_length=None, legacy=True):
        if legacy:
            return self.get_reachables_old(step_length)
        else:
            self.get_reachable_center_points(
            )

    def get_reachable_center_points(self, offset, omega, test_result=None):
        """

        returns  (xx, yy, xx_stroke, yy_stroke,
                  ....
                  radius_to_point, angle, yaw))

        """
        assert self.r is not None, 'vehicle turning radius was not defined'

        # get inner-ego-boundig-box
        inner_aa = self.transform_coord((0, -self.r), omega, offset)
        inner_bb = self.transform_coord((0,  self.r), omega, offset)
        inner_cc = self.transform_coord((self.r,  self.r), omega, offset)
        inner_dd = self.transform_coord((self.r, -self.r), omega, offset)

        # get outer-ego-boundig-box
        outer_aa = self.transform_coord((-self.r,  2 * self.r), omega, offset)
        outer_bb = self.transform_coord((self.r,  2 * self.r), omega, offset)
        outer_cc = self.transform_coord((self.r, -2 * self.r), omega, offset)
        outer_dd = self.transform_coord((-self.r, -2 * self.r), omega, offset)

        # get global bounding box
        upper_left = (min(outer_aa[0], outer_bb[0], outer_cc[0], outer_dd[0]),
                      min(outer_aa[1], outer_bb[1], outer_cc[1], outer_dd[1]))
        upper_right = (max(outer_aa[0], outer_bb[0], outer_cc[0], outer_dd[0]),
                       min(outer_aa[1], outer_bb[1], outer_cc[1], outer_dd[1]))
        lower_left = (min(outer_aa[0], outer_bb[0], outer_cc[0], outer_dd[0]),
                      max(outer_aa[1], outer_bb[1], outer_cc[1], outer_dd[1]))
        lower_right = (max(outer_aa[0], outer_bb[0], outer_cc[0], outer_dd[0]),
                       max(outer_aa[1], outer_bb[1], outer_cc[1], outer_dd[1]))

        # upper_left corner may be shriked back to
        # canvas size

        new_upper_left_x = upper_left[0]
        new_upper_left_y = upper_left[1]

        if upper_left[0] < 0:
            new_upper_left_x = 0

        # this should never happen, because then the left  and the rigth borders
        # are outside the boundary
        assert upper_left[
            0] < self.flaeche.cluster_length_x * self.flaeche.scale

        if upper_left[1] < 0:
            new_upper_left_y = 0

        # should not happen
        assert upper_left[
            1] < self.flaeche.cluster_length_y * self.flaeche.scale

        upper_left = (new_upper_left_x, new_upper_left_y)

        #### upper right ######

        new_upper_right_x = upper_right[0]
        new_upper_right_y = upper_right[1]

        # if this happens everything is outside the boundaries
        assert upper_right[0] > 0

        if upper_right[0] > self.flaeche.cluster_length_x * self.flaeche.scale:
            new_upper_right_x = self.flaeche.cluster_length_x * \
                self.flaeche.scale

        if upper_right[1] < 0:
            new_upper_right_y = 0

        assert upper_right[
            1] < self.flaeche.cluster_length_y * self.flaeche.scale

        upper_right = (new_upper_right_x, new_upper_right_y)

        # lower_left

        new_lower_left_x = lower_left[0]
        new_lower_left_y = lower_left[1]

        if lower_left[0] < 0:
            new_lower_left_x = 0

        assert lower_left[0] < self.flaeche.cluster_length_x * \
            self.flaeche.scale  # left limits

        assert lower_left[1] > 0     # lower limits
        if lower_left[1] > self.flaeche.cluster_length_y * self.flaeche.scale:
            new_lower_left_y = self.flaeche.cluster_length_y * \
                self.flaeche.scale

        lower_left = (new_lower_left_x, new_lower_left_y)

        # lower right

        new_lower_right_x = lower_right[0]
        new_lower_right_y = lower_right[1]

        assert lower_right[0] > 0  # right_limits

        if lower_right[0] > self.flaeche.cluster_length_x * self.flaeche.scale:
            new_lower_right = self.flaeche.cluster_length_x * \
                self.flaeche.scale

        assert lower_right[1] > 0  # lower limits
        if lower_right[1] > self.flaeche.cluster_length_y * self.flaeche.scale:
            new_lower_right = self.flaeche.cluster_length_y * \
                self.flaeche.scale

        lower_right = (new_lower_right_x, new_lower_right_y)

        # cell centers in global bounding box

        cell_upper_left = self.flaeche.get_cell(upper_left)
        cell_upper_right = self.flaeche.get_cell(upper_right)
        cell_lower_left = self.flaeche.get_cell(lower_left)
        cell_lower_right = self.flaeche.get_cell(lower_right)

        this_node_data = NodeDataHandler(self.flaeche.cluster[cell_upper_left[0]]
                                                             [cell_upper_left[1]])
        center_upper_left = this_node_data.get_center()

        this_node_data = NodeDataHandler(self.flaeche.cluster[cell_upper_right[0]]
                                         [cell_upper_right[1]])
        center_upper_right = this_node_data.get_center()

        this_node_data = NodeDataHandler(self.flaeche.cluster[cell_lower_left[0]]
                                                             [cell_lower_left[1]])
        center_lower_left = this_node_data.get_center()

        this_node_data = NodeDataHandler(self.flaeche.cluster[cell_lower_right[0]]
                                         [cell_lower_right[1]])
        center_upper_left = this_node_data.get_center()

        # center points

        xx_start = cell_upper_left[0]
        xx_end = cell_upper_right[0]
        yy_start = cell_upper_left[1]
        yy_end = cell_lower_left[1]

        extention = min(
            int(math.ceil(2 * self.r / self.flaeche.scale)), MAX_CELL_EXTENTION)

        xx_extended_start = xx_start - extention
        xx_extended_end = xx_end + extention
        yy_extended_start = yy_start - extention
        yy_extended_end = yy_end + extention

        global_ego_center_points = []
        global_ego_center_points_extention = []

        # get current cell
        current_cell = self.flaeche.get_cell(
            self.transform_coord((0, 0), 0, offset)
        )

        for cxx in range(xx_extended_start, xx_extended_end + 1):
            for cyy in range(yy_extended_start, yy_extended_end + 1):

                if cxx < 0 or cyy < 0:
                    continue

                # skip current cell
                if cxx == current_cell[0] and cyy == current_cell[1]:
                    continue

                this_node_data = NodeDataHandler(
                    self.flaeche.cluster[cxx][cyy])
                center_in_global_coords = this_node_data.get_center()
                center_in_ego_coords = self.transform_coord_to_ego(center_in_global_coords,
                                                                   omega, offset)

                ####  WHY INTEGERS ???? !!!! #######
                # center_in_ego_coords    = (int(center_in_ego_coords[0]),
                #                           int(center_in_ego_coords[1]))

                if (xx_start <= cxx <= xx_end and
                        yy_start <= cyy <= yy_end):
                    global_ego_center_points.append(
                        (center_in_global_coords[0], center_in_global_coords[1],
                         center_in_ego_coords[0], center_in_ego_coords[1]))
                else:
                    global_ego_center_points_extention.append(
                        (center_in_global_coords[0], center_in_global_coords[1],
                         center_in_ego_coords[0], center_in_ego_coords[1]))

        # sort center points into zones

        zone_zero = []
        zone_one = []
        zone_two = []
        zone_three = []

        for xx, yy, xx_stroke, yy_stroke in global_ego_center_points:

            # check if zone zero (within turn loop - unreachable)
            if yy_stroke < 0:
                sr = math.sqrt(xx_stroke**2 + (yy_stroke + self.r)**2)
            else:
                sr = math.sqrt(xx_stroke**2 + (yy_stroke - self.r)**2)

            if sr < self.r:
                zone_zero.append((xx, yy, xx_stroke, yy_stroke))
            else:
                if xx_stroke < 0:
                    zone_three.append((xx, yy, xx_stroke, yy_stroke))
                elif (0 <= xx_stroke < self.r and
                      abs(yy_stroke) <= self.r):
                    zone_one.append((xx, yy, xx_stroke, yy_stroke))
                else:
                    zone_two.append((xx, yy, xx_stroke, yy_stroke))

        return_list_name = None
        if test_result is not None:
            if test_result == 'get_inner_ego_bounding_box':
                return_item = (inner_aa, inner_bb, inner_cc, inner_dd)
            elif test_result == 'get_outer_ego_bounding_box':
                return_item = (outer_aa, outer_bb, outer_cc, outer_dd)
            elif test_result == 'get_global_bounding_box':
                return_item = (
                    upper_left, upper_right, lower_right, lower_left)
            elif test_result == 'get_all_center_points':
                return_list_name = global_ego_center_points
            elif test_result == 'get_zone_zero_center_points':
                return_list_name = zone_zero
            elif test_result == 'get_zone_one_center_points':
                return_list_name = zone_one
            elif test_result == 'get_zone_two_center_points':
                return_list_name = zone_two
            elif test_result == 'get_zone_three_center_points':
                return_list_name = zone_three
            elif test_result == 'get_extention_center_points':
                return_list_name = global_ego_center_points_extention
            if return_list_name is not None:
                return_item = [(xx, yy)
                               for xx, yy, xx_stroke, yy_stroke in return_list_name]
            return return_item

        yaw_max_left = 0
        yaw_min_left = None
        yaw_max_right = 0
        yaw_min_right = None

        reachables = []

        def set_reachables(zone,
                           ll_yaw_max_left,  ll_yaw_min_left,
                           ll_yaw_max_right, ll_yaw_min_right):

            for xx, yy, xx_stroke, yy_stroke in zone:  # zone_one:

                radius_to_point, angle_ego = self.get_radius_to_point_ego((xx_stroke,
                                                                           yy_stroke))

                print
                print 'reachables:'
                print 'radius_to_point , angle_ego:', radius_to_point, angle_ego

                # get streight elements
                if radius_to_point == float('inf') or radius_to_point == float('-inf'):
                    trajectory = self.get_points_on_line(start_point=(0, 0),
                                                         end_point=(
                                                             xx_stroke, yy_stroke),
                                                         omega=omega,
                                                         scale_fraction=10)
                    print("here")
                    print 'xx, yy', xx, yy
                    print 'xx_stroke, yy_stroke', xx_stroke, yy_stroke
                    print trajectory

                # get trajectory over bypased nodes/cells
                else:
                    trajectory = self.get_points_on_the_circle_ego(radius_to_point=radius_to_point,
                                                                   angle_ego=angle_ego,
                                                                   scale_fraction=10)

                # convert to global
                trajectory = [self.transform_coord_and_angle(pp,
                                                             omega=omega,
                                                             offset=offset)
                              for pp in trajectory]

                cells_in_beetween = self.flaeche.convert_trajectroy_points_to_bypassed_cells(
                    trajectory)

                # get yaw:
                yaw = float(self.r) / radius_to_point

                if yy_stroke >= 0:
                    if yaw > ll_yaw_max_left:
                        ll_yaw_max_left = yaw
                    if ll_yaw_min_left is None or ll_yaw_min_left > yaw:
                        ll_yaw_min_left = yaw
                elif yy_stroke < 0:
                    if yaw > ll_yaw_max_right:
                        ll_yaw_max_right = yaw
                    if ll_yaw_min_right is None or ll_yaw_min_right > yaw:
                        ll_yaw_min_right = yaw

                #angle = omega + angle_ego if angle_ego > 0 else -omega + angle_ego
                angle_global = omega + angle_ego

            #   print 'angle_global, omega, angle_ego', angle_global, omega, angle_ego
            #   if angle == 0:
            #       radius_to_point = float('inf')

                reachables.append((xx, yy, xx_stroke, yy_stroke,
                                   cells_in_beetween,
                                   angle_ego, radius_to_point, angle_global, yaw))

            return (ll_yaw_max_left, ll_yaw_min_left, ll_yaw_max_right, ll_yaw_min_right)

        zone_three_extended = zone_three + global_ego_center_points_extention
        zone_inspection_order = [zone_one, zone_two, zone_three_extended]

        # need more points?

        # a point a option is found close enought to the max
        # radius (75%) - both left and right

        # a point is found close enough (yaw < 0.15) to the straight forward course
        # eighter left or right.

        def found_enough_points():
            if yaw_min_left is None or yaw_min_right is None:
                return False
            if yaw_max_right < 0.75 or yaw_max_left < 0.75:
                return False
            elif min(yaw_min_left, yaw_min_right) > 0.15:
                return False
            else:
                return True

        next_zone_to_inspect = 0
        while (not found_enough_points() and
               next_zone_to_inspect < len(zone_inspection_order)):

            yaw_max_left, yaw_min_left, yaw_max_right, yaw_min_right = set_reachables(
                zone_inspection_order[next_zone_to_inspect],
                yaw_max_left, yaw_min_left, yaw_max_right, yaw_min_right)

#            print 'yaw_max_left', yaw_max_left, 'yaw_min_left', yaw_min_left
# print 'yaw_max_right', yaw_max_right, 'yaw_min_right', yaw_min_right

            next_zone_to_inspect += 1

        return reachables

    def get_reachables_old(self, step_length=None):
        if step_length is None:
            step_length = self.flaeche.scale / 10

        assert self.x is not None
        assert self.y is not None
        assert self.rotation is not None

        current_cell = self.flaeche.get_cell((self.x, self.y))

        found_left = False
        found_right = False

        known_cells_streight = []
        known_cells_streight.append(self.flaeche.get_cell((self.x, self.y)))

        known_cells_left = []
        known_cells_streight.append(self.flaeche.get_cell((self.x, self.y)))

        known_cells_right = []
        known_cells_streight.append(self.flaeche.get_cell((self.x, self.y)))

        ii = 0
        while (not found_left or not found_right):
            found_left = False
            found_right = False

            ii += 1
            delta = ii * step_length

            new_pos_streight = self.physics_stearing(driving=0, delta=delta)
            new_pos_left = self.physics_stearing(driving=-1, delta=delta)
            new_pos_right = self.physics_stearing(driving=1, delta=delta)

            new_cell_streight = self.flaeche.get_cell((new_pos_streight[0],
                                                       new_pos_streight[1]))
            new_cell_left = self.flaeche.get_cell((new_pos_left[0],
                                                   new_pos_left[1]))
            new_cell_right = self.flaeche.get_cell((new_pos_right[0],
                                                    new_pos_right[1]))

            if new_cell_streight not in known_cells_streight:
                known_cells_streight.append(new_cell_streight)

            if new_cell_left not in known_cells_left:
                known_cells_left.append(new_cell_left)

            if new_cell_right not in known_cells_left:
                known_cells_right.append(new_cell_right)

            # find one cell left and right, that can't be reached streight

            straigth_left = set(known_cells_streight).union(known_cells_left)
            straigth_right = set(known_cells_streight).union(known_cells_right)

            only_left = set(known_cells_left).difference(straigth_right)
            only_right = set(known_cells_right).difference(straigth_left)

            if len(only_left) > 0:
                found_left = True
                # print 'only_left', only_left

            if len(only_right) > 0:
                found_right = True
                # print 'only_right', only_right

            if False:  # debug
                print
                print 'ii: %i ' % ii
                print 'self.x %s, self.y %s, current_cell %s ' % (str(self.x),
                                                                  str(self.y),
                                                                  str(current_cell))
                print 'delta: %s' % delta

                print 'new_pos_streight: %s, new_cell_streight %s ' % (str(new_pos_streight),
                                                                       str(new_cell_streight))
                print 'new_pos_left: %s, new_cell_left %s ' % (str(new_pos_left),
                                                               str(new_cell_left))
                print 'new_pos_right: %s, new_cell_right %s ' % (str(new_pos_right),
                                                                 str(new_cell_right))

                print 'found_left:', found_left, 'found_right:', found_right

                print 'known_cells_streight', known_cells_streight
                print 'known_cells_left',  known_cells_left
                print 'known_cells_right', known_cells_right

                print '----------------------------------------------------'

        reachables = list(
            set(known_cells_streight).union(known_cells_left).union(known_cells_right))
        reachables.remove(current_cell)

        return sorted(reachables)


class AdAStar():

    def __init__(self, start_node, end_node, flaeche=None, vessel=None):
        self.start = start_node
        self.end = end_node
        self.vessel = vessel
        self.flaeche = self.vessel.flaeche

        if not (self.flaeche.is_valid_node_sector_pos(start_node) and
                self.flaeche.is_valid_node_sector_pos(end_node)):
            raise StandardError, 'Start and/or end points are not leagal for flaeche %s %s' % (
                start_node, end_node)
        if start_node == end_node:
            raise StandardError, 'Start point must not be equal to end point'

        self.reached_dest_node = None
#       self.iteration_step    = 0
        self.iteration_step = -1
        self.open_nodes_list = []
        self.closed_nodes_list = []
        self.path = []
        self.current_node_copy = None

        # gbm bergamond

    def get_distance_between_points(self, node_1, node_2):
        if len(node_1) != 3 or len(node_2) != 3:
            raise StandardError(
                'for ada always use three-tuple %s %s' % (node_1,  node_2))
        for node in [node_1, node_2]:
            if not self.flaeche.is_valid_node_pos(node):
                raise StandardError('node not legal %s' % node)
        if node_1[0] == node_2[0] and node_1[1] == node_2[1]:
            return 0

        return self.flaeche.scale * math.sqrt(math.pow(node_1[0] - node_2[0], 2) + math.pow(node_1[1] - node_2[1], 2))

    def get_distance_to_end(self, point):
        if self.flaeche.is_valid_node_pos(point):
            return self.get_distance_between_points(point, self.end)
        else:
            raise StandardError("Node %s is not valid" % point)

    def get_node_list(self, ANList, type=None):
        if type == 'tuples':
            return [nn.get_coords() for nn in ANList]
        else:
            return [nn for nn in ANList]

    def get_open_nodes(self, type=None):
        return self.get_node_list(self.open_nodes_list, type)

    def get_closed_nodes(self, type=None):
        return self.get_node_list(self.closed_nodes_list, type)

    def step(self, visual=False, verbose=False):
        self.iteration_step += 1
        print self.iteration_step

#        if verbose:
        if True:
            print '\n\nxxx step %s xxxxxxxxxxxxxxxxxxxxxxxxxxxxx' % self.iteration_step

        # first step ever
        if len(self.open_nodes_list) == 0 and len(self.closed_nodes_list) == 0:

            fn_data = self.flaeche.get_node_data((self.start[0],
                                                  self.start[1]))
            fn_sector_id = self.start[2]
            fn_reached_by_angle = self.flaeche.get_angle_from_sector_id(
                self.start[2])
            fn_costs_till_here = 0.0
            fn_estimated_remaining_costs = self.vessel.guess_best_way_to_point(
                from_point=self.flaeche.get_cell_center((self.start[0],
                                                         self.start[1])),
                omega_from=self.flaeche.get_angle_from_sector_id(
                    self.start[2]),
                to_point=self.flaeche.get_cell_center((self.end[0],
                                                       self.end[1])),
                guess='best')[-1] / float(self.vessel.
                                          speed)
            fn_previous_node = None
            fn_in_between_nodes = []

            first_node = StarNodeC(node_data=fn_data,
                                   sector_id=fn_sector_id,
                                   reached_by_angle=fn_reached_by_angle,
                                   costs_till_here=fn_costs_till_here,
                                   estimated_remaining_costs=fn_estimated_remaining_costs,
                                   previous_node=fn_previous_node,
                                   in_between_nodes=fn_in_between_nodes)

            self.open_nodes_list.append(first_node)
            return False    # Algorithm is not jet finished

        # Note: what shall happen if open_list = void but closed list is populated
        # ? terminat algorithm  successlessly ?

        # get the node with the best combined cost value
        current_node = ANList(self.open_nodes_list).get_min_node(pop=True)
        self.current_node_copy = current_node
        self.current_node_copy = current_node
#        print '----'
#        print 'costs', self.current_node_copy.id, self.current_node_copy.full_costs
#        print

        # Success critieria:
        # even if the destination node has been seen before,
        # the path is not prooven to be the shortest
        # until it has been teared from the open list
        if (current_node.get_coords()[0:2] == self.end[0:2]):
            self.reached_dest_node = current_node

            print '########################'
            print '########################'
            print current_node.id
            print

            return True  # finished / found

        # normal iteration step

        # get all possible next nodes
        # (only coords are returned)

        suspicious_center_points = self.vessel.get_reachable_center_points(
            (current_node.x_coord, current_node.y_coord),
            self.flaeche.get_angle_from_sector_id(current_node.sector_id))  # aka vessel.rotation,

        speed = self.vessel.speed
        assert(speed is not None), "vessels speed must be set"
        assert(speed > 0), "vessels speed must be greater than 0"


#        suspicious_nodes = [ convert_to_c_star_node(center_point) for center_point in suspicious_center_points  ]

        HereNode = namedtuple('HereNode',
                              ['cell_x_id', 'cell_y_id', 'sector_id',
                               'cell_center_pos_x', 'cell_center_pos_y',
                               'angle_global', 'angle_travel', 'radius', 'length', 'durration',
                               'cells_in_between'])

        suspicious_nodes_a = []
        for mm in suspicious_center_points:

            cell_x_id = self.flaeche.get_cell((mm[0], mm[1]))[0]
            cell_y_id = self.flaeche.get_cell((mm[0], mm[1]))[1]

            angle_travel = mm[-4]
            angle_global = mm[-2]
            radius = mm[-3]

            cells_in_between = mm[-5]
            print
            print 'cells_in_between',
            print current_node.x_id, current_node.y_id, '->',
            print cell_x_id, cell_y_id, ':', cells_in_between

            sector_id = self.flaeche.get_sector_id_from_angle(angle_global)

            cell_center_pos_x, cell_center_pos_y = self.flaeche.get_cell_center((cell_x_id,
                                                                                 cell_y_id))

            if radius == float('inf') or radius == float('-inf'):
                length = self.get_distance_between_points((current_node.x_id,
                                                           current_node.y_id,
                                                           current_node.sector_id),
                                                          (cell_x_id,
                                                           cell_y_id,
                                                           sector_id))
            else:
                length = radius * abs(angle_travel)
            assert (length > 0), (cell_x_id, cell_y_id)

            durration = length / speed

            hn = HereNode(cell_x_id,
                          cell_y_id,
                          sector_id,
                          cell_center_pos_x,
                          cell_center_pos_y,
                          angle_global,
                          angle_travel,
                          radius,
                          length,
                          durration,
                          cells_in_between

                          )

#            print 'hn', hn

            suspicious_nodes_a.append(hn)

        suspicious_nodes = suspicious_nodes_a

        for nn in suspicious_nodes:
            # is in closed_list -> ignore
            closedDNL = ANList(self.closed_nodes_list, 'tuple')
            if not (nn.cell_x_id, nn.cell_y_id, nn.sector_id) in closedDNL:

                # check all shadow_nodes of the current
                # if there are blocked ones among

                shadow_current = self.vessel.get_predefiend_shadow_shape_from_cell_id(
                    (nn.cell_x_id,
                     nn.cell_y_id,
                     nn.sector_id))

                # skip if blocked
                if not self.flaeche.all_nodes_in_list_are_valid_and_not_blocked(shadow_current):
                    print 'blocked', (nn.cell_x_id, nn.cell_y_id)
                    continue

                # get all the shadows of the bypassed nodes

                shadow_of_all_bypassed_nodes = set()
                for bypassed_node in nn.cells_in_between:
                    shadow_of_one_bypassed_node = set(
                        self.vessel.get_predefiend_shadow_shape_from_cell_id(bypassed_node))
                    shadow_of_all_bypassed_nodes.update(
                        shadow_of_one_bypassed_node)

                shadow_of_all_bypassed_nodes = list(
                    shadow_of_all_bypassed_nodes)
# print 'shadow_of_all_bypassed_nodes', shadow_of_all_bypassed_nodes

                # check all bypassed nodes if there
                # are blocked nodes in the shadow
                if not self.flaeche.all_nodes_in_list_are_valid_and_not_blocked(
                        shadow_of_all_bypassed_nodes):
                    print 'blocked in between', (nn.cell_x_id, nn.cell_y_id)
                    continue


# skip if blocked
#                if self.flaeche.is_blocked((nn.cell_x_id, nn.cell_y_id)):
#                    print 'blocked', (nn.cell_x_id, nn.cell_y_id)
#                    continue

# skip if bypassed nodes are blocked
#                bypassed_nodes_contain_blocked = False
#                for bypassed_node in nn.cells_in_between:
#                    if self.flaeche.is_blocked((bypassed_node[0], bypassed_node[1])):
#                        print 'blocked in between', (nn.cell_x_id, nn.cell_y_id)
#                        bypassed_nodes_contain_blocked = True

#                if bypassed_nodes_contain_blocked:
#                    continue


#                if nn.cell_x_id ==  13 and nn.cell_y_id == 6 and nn.sector_id == 2:
#                    import pdb; pdb.set_trace()

                sn_reached_by_angle = nn.angle_global
                sn_lastNode = current_node

                sn_data = self.flaeche.get_node_data((nn.cell_x_id,
                                                      nn.cell_y_id))
                sn_sector_id = nn.sector_id
                sn_costs_till_here = current_node.costs_till_here + \
                    nn.durration

                if False:
                    print
                    print '## calculation ################'
                    print 'direction'
                    print '.         angle_to_travel_left'
                    print '.         .                angle_to_travel_distance_left, ',
                    print 'tangent_distance_left, total_distance_left)'

                    print
                    print 'c-n: ', current_node.id,
                    print 'costs till current node :', current_node.costs_till_here
                    print 'costs from current node to new open node :', nn.durration
                    print 'circle travel to new open node :', nn.angle_travel
                    print 'new open node : ', nn.cell_x_id, nn.cell_y_id, nn.sector_id,  '-> ',
                    print self.end[0], self.end[1]
                    print self.vessel.guess_best_way_to_point(
                        from_point=self.flaeche.get_cell_center((nn.cell_x_id,
                                                                 nn.cell_y_id)),
                        omega_from=nn.angle_global,
                        to_point=self.flaeche.get_cell_center((self.end[0],
                                                               self.end[1])),
                        guess='best',
                        verbose=False), 'new'
                    print self.vessel.guess_best_way_to_point_old(
                        from_point=self.flaeche.get_cell_center((nn.cell_x_id,
                                                                 nn.cell_y_id)),
                        omega_from=nn.angle_global,
                        to_point=self.flaeche.get_cell_center((self.end[0],
                                                               self.end[1])),
                        guess='best'), 'old'

                sn_estimated_remaining_costs = self.vessel.guess_best_way_to_point_old(
                    from_point=self.flaeche.get_cell_center((nn.cell_x_id,
                                                             nn.cell_y_id)),
                    omega_from=nn.angle_global,
                    to_point=self.flaeche.get_cell_center((self.end[0],
                                                           self.end[1])),
                    guess='best')[-1] / float(self.vessel.speed)

# print 'sum: ', sn_costs_till_here + sn_estimated_remaining_costs

                sn_previous_node = current_node
                sn_in_between_nodes = []

                sus_node = StarNodeC(node_data=sn_data,
                                     sector_id=sn_sector_id,
                                     reached_by_angle=sn_reached_by_angle,
                                     costs_till_here=sn_costs_till_here,
                                     estimated_remaining_costs=sn_estimated_remaining_costs,
                                     previous_node=sn_previous_node,
                                     in_between_nodes=sn_in_between_nodes)

                # is in open_list -> event. update open list
                openDNL = ANList(self.open_nodes_list, 'tuple')
                # returns None if not in list
                some_open_node = openDNL.get_by_tuple(
                    (nn.cell_x_id, nn.cell_y_id, nn.sector_id))

                if some_open_node != None:
                    if sus_node.full_costs < some_open_node.full_costs:
                        #                       print 'fount in open list!!'
                        self.open_nodes_list.remove(some_open_node)
                        self.open_nodes_list.append(sus_node)
                # neigther nor -> append to open list
                else:
                    self.open_nodes_list.append(sus_node)
                del(some_open_node)
        self.closed_nodes_list.append(current_node)

        if visual:
            self.vis_debug(self.iteration_step)

        return False  # not jet finshed

    def vis_debug(self, step):
        #        print
        #        print self.iteration_step, 'start end'
        #        self.draw_start_end_node()

        #        print
        #        print self.iteration_step, 'open'
        #        self.draw_open()

        #        print
        #        print self.iteration_step, 'closed'
        #        self.draw_closed()

        #        print
        #        print self.iteration_step, 'open closed'

        print(self.vessel,
              self.current_node_copy.x_coord,
              self.current_node_copy.y_coord,
              self.current_node_copy.sector_id,
              self.vessel.r,
              self.vessel.r * math.pi
              )

        # draw nodes in open and closed list
        self.draw_open_closed(step)
        self.flaeche.draw_course_ghost_ship(self.vessel,
                                            self.current_node_copy.x_coord,
                                            self.current_node_copy.y_coord,
                                            self.current_node_copy.sector_id,
                                            self.vessel.r,
                                            self.vessel.r * math.pi
                                            )

    def run(self, visual=False, verbose=False):
        while not self.step(visual, verbose):
            pass

    def rebuild_path(self, some_node=None, first=True):
        self.rebuild_path_recursive(some_node, first)

    def rebuild_path_recursive(self, some_node=None, first=False):
        if self.reached_dest_node == None:
            raise StandardError("algorithm must be run first successfully")
        elif some_node == None and first:
            some_node = self.reached_dest_node
        elif some_node == None and not first:
            assert False, 'Something went wrong when storing the previous node!'
        self.path[0:0] = [some_node]
        if some_node.get_coords()[0:2] == self.start[0:2]:    # before 1
            return
        self.rebuild_path_recursive(some_node.previous_node)

    def patch_path(self):
        assert len(
            self.path) > 0, 'path has a length of zero, probably has not been rebuit'

        ii = 0
        last_node = None
        curr_node = self.path[0]

        while ii < len(self.path) - 1:
            ii += 1
            last_node = curr_node
            curr_node = self.path[ii]

            continous = self.flaeche.neighbours((last_node.x_id, last_node.y_id),
                                                (curr_node.x_id, curr_node.y_id))

            if not continous:

                #                print
                #                print last_node.id, curr_node.id, continous

                start = (last_node.x_id, last_node.y_id)
                end = (curr_node.x_id, curr_node.y_id)
                myD = Dijkstra(self.flaeche, start, end)
                myD.run()
                myD.rebuild_path()

                ############################
                # this all is untested and subject to change
                # interpol nodes

                inter_last_node = last_node
                inclination_groth = (
                    curr_node.sector_id - last_node.sector_id) / len(myD.path)
#                tt_groth             = (curr_node.tt - last_node.tt) / len(myD.path)
                tt_groth = (
                    curr_node.costs_till_here - last_node.costs_till_here) / len(myD.path)
                counter = 0

                path_gap_fill = []

                for gg in myD.path[1:-1]:
                    counter += 1
#                    inter_node = StarNodeC(
#                        node_data = self.flaeche.get_node_data((gg.x_id, gg.y_id)),
#                        sector= int(math.floor(last_node.sector + counter * inclination_groth)),
#                        tt        = math.floor(last_node.tt        + counter * tt_groth),
# dd        = 0, ############!!!!!! not calculated here be aware it's fake
#                        reached_by_angle = None,
#                        lastNode  = inter_last_node)

                    inter_node = StarNodeC(
                        node_data=self.flaeche.get_node_data(
                            (gg.x_id, gg.y_id)),
                        sector_id=int(
                            math.floor(last_node.sector_id + counter * inclination_groth)),
                        # !!!!!! not calculated here be aware it's fake
                        reached_by_angle=0,
                        costs_till_here=math.floor(
                            last_node.costs_till_here + counter * tt_groth),
                        estimated_remaining_costs=1,
                        previous_node=inter_last_node
                    )

                    inter_last_node = inter_node

                    path_gap_fill.append(inter_node)

                self.path[ii:ii] = path_gap_fill
                ii += len(path_gap_fill)
                del(path_gap_fill)

    def draw_start_end_node(self):
        self.flaeche.vis_reset()
        self.flaeche.vis_add_start(self.start)
        self.flaeche.vis_add_end(self.end)
        for xx in range(self.flaeche.cluster_length_x):
            for yy in range(self.flaeche.cluster_length_y):
                if self.flaeche.cluster[xx][yy] != None:
                    if self.flaeche.cluster[xx][yy].is_blocked:
                        self.flaeche.vis_add_blocked((xx, yy))

        self.flaeche.vis_show()

    def draw_open(self):
        self.flaeche.vis_reset()
        for nn in ANList(self.open_nodes_list).get_tuples():
            self.flaeche.vis_add_open(nn)
        self.flaeche.vis_show()

    def draw_closed(self):
        self.flaeche.vis_reset()
        for nn in ANList(self.closed_nodes_list).get_tuples():
            self.flaeche.vis_add_closed(nn)
        self.flaeche.vis_show()

    def draw_open_closed(self, step=None):
        self.flaeche.vis_reset()
        open_nodes = ANList(self.open_nodes_list).get_tuples()
        assert len(open_nodes) > 0
        for nn in open_nodes:
            self.flaeche.vis_add_open(nn)

        del(nn)
        for nn in ANList(self.closed_nodes_list).get_tuples():
            self.flaeche.vis_add_closed(nn)
        for xx in range(self.flaeche.cluster_length_x):
            for yy in range(self.flaeche.cluster_length_y):
                if self.flaeche.cluster[xx][yy] != None:
                    if self.flaeche.cluster[xx][yy][NodeDataHandler.is_blocked]:
                        self.flaeche.vis_add_blocked((xx, yy))
        self.flaeche.vis_add_start(self.start)
        self.flaeche.vis_add_end(self.end)
        self.flaeche.vis_show(step_num=step)

    def draw_path(self, final=False, vessel=None):
        self.flaeche.vis_reset()
        self.flaeche.vis_add_start(self.start)
        self.flaeche.vis_add_end(self.end)
        for nn in ANList(self.open_nodes_list).get_tuples():
            self.flaeche.vis_add_open(nn)
        del(nn)
        for nn in ANList(self.closed_nodes_list).get_tuples():
            self.flaeche.vis_add_closed(nn)
        del(nn)
        for xx in range(self.flaeche.cluster_length_x):
            for yy in range(self.flaeche.cluster_length_y):
                if self.flaeche.cluster[xx][yy] != None:
                    if self.flaeche.cluster[xx][yy][NodeDataHandler.is_blocked]:
                        self.flaeche.vis_add_blocked((xx, yy))

        for nn in self.path:
            # self.flaeche.vis_add_path(NodeDataHandler.get_x_and_y_id(nn.node_data))

            self.flaeche.vis_add_path((nn.x_id, nn.y_id))
#            print ('path_final', nn.id, self.flaeche.get_angle_from_sector_id(nn.sector_id),
# self.flaeche.get_angle_from_sector_id(nn.sector_id) * 180/math.pi,
# nn.reached_by_angle)
            if vessel is not None:
                vessel.transform_hull_points(nn.reached_by_angle,
                                             self.flaeche.get_possition_from_cell_center_id(
                                                 (nn.x_id, nn.y_id)
                                             ))
                self.flaeche.vis_add_poly(
                    vessel.transformed_hull_points, 'red', width=2)

        # this is just for drawing the vessel's shape somewhere check it's
        # orientation
        if True:
            if vessel is not None:
                vessel.transform_hull_points(0, (242, 195))
                self.flaeche.vis_add_poly(
                    vessel.transformed_hull_points, 'orange')
#                vessel.transform_hull_points(math.pi/4, (240, 240))
#                self.flaeche.vis_add_poly(vessel.transformed_hull_points, 'orange')

#                vessel.transform_hull_points(math.pi/2, (240, 280))
#                self.flaeche.vis_add_poly(vessel.transformed_hull_points, 'orange')

        if final:
            self.flaeche.vis_show(step_num=self.iteration_step)
        else:
            self.flaeche.vis_show()


# Node Data is all stored in tuples for perfomance
#  creation of 1000 000 objects:
#            real    0m1.781s mit Grundlast
#            user    0m1.751s
#            sys     0m0.027s
#
#   tuples
#            real    0m0.335s mit Grundlast
#            user    0m0.302s
#            sys     0m0.031s


class Flaeche():

    def __init__(self, xdim, ydim, scale, sectors=16, output=None):
        self.abs_xdim = xdim    # meters
        self.abs_ydim = ydim    # meters
        self.scale = scale   # lengh of cluster element

        # sectors = 2, 4, 8, 16 ---  2**n
        # sectors = 2**4

        # sector ids: 0 ... sectors-1
        # e.g.        0 ... 15

        self.sectors = sectors

        if output is None:
            self.output = 'Flaeche'
        else:
            self.output = output

        # abrunden keine angebrochenen felder
        self.cluster_length_x = int(self.abs_xdim / self.scale)
        self.cluster_length_y = int(self.abs_ydim / self.scale)

        self.tile_length = self.scale
        self.tile_line = 1

        self.cluster = []
        self.vis_cluster = []
        self.top_layer = []

        self.load_node_data()
        self.vis_reset()

        self.polies = []
        self.points = []
        self.im = None
        self.draw = None
        self.init_image()

    def get_empty_gird_copy(self):
        """returns an complete nude grid of the same dimentions, but
        containing absolutely no information"""

        return Flaeche(xdim=self.abs_xdim,
                       ydim=self.abs_ydim,
                       scale=self.scale,
                       sectors=self.sectors,
                       output=None
                       )

    def reset_cluster(self):
        self.cluster = []
        for xx in range(self.cluster_length_x):
            yyyy = []
            for yy in range(self.cluster_length_y):
                yyyy.append((self, xx, yy, False))
            self.cluster.append(yyyy)

    def load_node_data(self, blocked_nodes=None):
        if blocked_nodes == None:
            self.reset_cluster()
            return

        self.cluster = []
        for xx in range(self.cluster_length_x):
            yyyy = []
            for yy in range(self.cluster_length_y):
                if (xx, yy) in blocked_nodes:
                    #yyyy.append(NodeData(self, xx, yy, True))
                    yyyy.append((self, xx, yy, True))
                else:
                    yyyy.append((self, xx, yy, False))
            self.cluster.append(yyyy)

    def is_valid_coord_pos(self, (xx, yy)):
        if xx < 0:
            return False
        elif yy < 0:
            return False
        elif xx > self.abs_xdim or yy > self.abs_ydim:
            return False
        else:
            return True

    def is_valid_coord_angle_pos(self, (xx, yy, zz)):
        if (self.is_valid_coord_pos((xx, yy)) and
                0 <= angle < 2 * math.pi):
            return True
        else:
            return False

    def is_blocked(self, (xx, yy)):
        assert self.is_valid_node_pos((xx, yy))
        this_node_data = NodeDataHandler(self.cluster[xx][yy])
        if this_node_data.is_blocked:
            return True
        else:
            return False

    def is_valid_node_pos(self, node_tuple):

        # handle sector information if given
        if len(node_tuple) == 3:
            xx, yy, sector = node_tuple

            if not self.is_valid_node_sector_pos((xx, yy, sector)):
                return False

# used to be code dublication:
# sector number is too large
#            if sector >= self.sectors:
#                return False
# sector number should never be below 0
#            if sector < 0:
#                return False

        else:
            xx, yy = node_tuple

        # check xx and yy coordinates
        if xx < 0:
            return False
        elif xx >= self.cluster_length_x:
            return False
        elif yy < 0:
            return False
        elif yy >= self.cluster_length_y:
            return False
        else:
            #   if self.is_blocked((xx,yy)):
            #       return False
            #   else:
            return True

            # everything else
            return True

    def all_nodes_in_list_are_valid(self, node_list):
        """all nodes in the list are nodes that are leagal,
        e.g. outside the area"""

        for node in node_list:
            if not self.is_valid_node_pos(node):
                #               print 'node', node
                return False

        return True

    def all_nodes_in_list_are_valid_and_not_blocked(self, node_list):
        """all nodes in the list are nodes that are leagal,
        e.g. __not_blocked__ or outside the area"""

        if not self.all_nodes_in_list_are_valid(node_list):
            return False

        for node in node_list:
            if self.is_blocked((node[0], node[1])):
                return False

        return True

    def is_valid_node_sector_pos(self, (xx, yy, sector)):
        if (self.is_valid_node_pos((xx, yy)) and
                0 <= sector < self.sectors):
            return True
        else:
            return False

    def is_valid_sector_id(self, sector_id):
        assert(type(sector_id) is int), 'sector_id must be an integer'
        assert(sector_id >= 0), 'sector_id must be greater equal zero'
        assert(
            sector_id < self.sectors), 'sector_id is greater then the number of sectors'
        return True

    def get_angle_from_sector_id(self, sector_id):
        assert (sector_id < self.sectors)
        assert (sector_id >= 0), sector_id
        assert int(sector_id) == sector_id

        angle = sector_id * 2 * math.pi / self.sectors
        return angle

# return (sector * 2 * math.pi / self.sectors) + 0.5 * 2 * math.pi /
# self.sectors

    def get_node_data(self, (xx, yy)):
        assert self.is_valid_node_pos(
            (xx, yy)), 'self.is_valid_node_pos((%s, %s))' % (xx, yy)
        return self.cluster[xx][yy]

    def get_neighbours(self, (xx, yy)):
        neighbours = [(xx - 1, yy + 1),
                      (xx, yy + 1),
                      (xx + 1, yy + 1),
                      (xx - 1, yy),
                      (xx + 1, yy),
                      (xx - 1, yy - 1),
                      (xx, yy - 1),
                      (xx + 1, yy - 1)]
        rr = [pp for pp in neighbours if self.is_valid_node_pos(pp)]
        return sorted(rr)

    def neighbours(self, point_one, point_two):
        assert self.is_valid_node_pos(point_one)
        assert self.is_valid_node_pos(point_two)
        assert point_one != point_two

        if point_two in self.get_neighbours(point_one):
            return True
        else:
            return False

    def get_cell(self, p):
        """return the cell, a point is located in,
           this is always the cell-corner with the smallest coordinates - upper left

                |       |
             ---x----------
                |       |<
                |   X   |< not included
                |       |<   
             ----------------- 
                |       | ^^^not included

        """
        # check if is vallid node
        assert self.is_valid_coord_pos(p), p

        return (int(math.floor(float(p[0]) / self.scale)), int(math.floor(float(p[1]) / self.scale)))

    def get_cell_center(self, p):
        return ((float(p[0]) + 0.5) * self.scale,
                (float(p[1]) + 0.5) * self.scale)

    def get_cell_and_sector(self, p, angle):
        """ returns the cell id and the sector id (no angle)of the sector"""
        cell = self.get_cell(p)
        sector_center = self.get_sector_id_from_angle(angle)
        return (cell, sector_center)

#    def get_sector_center_from_angle(self, angle)
#        """ returns the angle to the center of the sector"""
#        pass
#        return sector_center

    def convert_cell_id_to_tuple(self, cell_id):
        #        assert (type(cell_id) == 'str')
        components = [int(cc) for cc in cell_id.split('_')]
        assert (len(components) == 3), 'wrong cell_id formant %s' % cell_id
        return (components[0], components[1], components[2])

    def get_possition_from_cell_center_id(self, (center_id_x, center_id_y)):
        assert center_id_x >= 0
        assert center_id_x <= self.cluster_length_x

        assert center_id_y >= 0
        assert center_id_y <= self.cluster_length_y

        return ((center_id_x + 0.5) * self.scale, (center_id_y + 0.5) * self.scale)

    def get_sector_id_from_angle(self, angle):
        """ returns the id of the sector the angle is in"""
        sector_length = 2 * math.pi / self.sectors
        sector_center = int(math.floor(float((angle)) / sector_length))
        if angle > (0.5 + sector_center) * sector_length:
            sector_center += 1
        sector_center = sector_center % self.sectors

        return sector_center

    def get_cells(self, p):
        """returns the list of cells, a point is located in,

           this can be more than one cell if the point is on
           a border edge


                |       |
             --------------
                |     x |
                |  XX   |
                |       |
             --------------
                |       |


                |       |
             --------------
                |       |
            XX  x  XX   |
                |       |
             -------
                |       |


                |       |
             --------------
                |       |
                |  XX   |
                |       |
              -------x---
                |       |
                |  XX   | 
                |       |


                |       |
            XX  |  XX   |
                |       |
              --x---------
                |       |
            XX  |  XX   |
                |       |


        """

        def xxx(point):
            int(math.floor(float(p[0]) / self.scale))

        rest_x = round(float(p[0]) % self.scale, 7)
        rest_y = round(float(p[1]) % self.scale, 7)

        vertical_one = True if rest_x == 0 else False
        horizontal_one = True if rest_y == 0 else False

        if not vertical_one and not horizontal_one:
            return [(int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))]

        elif vertical_one and not horizontal_one:
            ret_0 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))

            ret_1 = (int(math.floor(float(p[0]) / self.scale)) - 1,
                     int(math.floor(float(p[1]) / self.scale)))
            ret = [ret_0, ret_1]
#            if not ret_1[0] < 0:
#               ret.append(ret_1)

            return sorted(ret)

        elif not vertical_one and horizontal_one:
            ret_0 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))

            ret_1 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)) - 1)

            ret = [ret_0, ret_1]
#            if not ret_1[1] < 0:
#            ret.append(ret_1)

            return sorted(ret)

        elif vertical_one and horizontal_one:
            ret_0 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))

            ret_1 = (int(math.floor(float(p[0]) / self.scale)) - 1,
                     int(math.floor(float(p[1]) / self.scale)))

            ret_2 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)) - 1)

            ret_3 = (int(math.floor(float(p[0]) / self.scale)) - 1,
                     int(math.floor(float(p[1]) / self.scale)) - 1)
            ret = [ret_0, ret_1, ret_2, ret_3]


#            if not ret_1[0] < 0:
#               ret.append(ret_1)
#            if not ret_2[1] < 0:
#               ret.append(ret_2)
#            if ret_3[0] >= 0 and  ret_3[1] >= 0:
#               ret.append(ret_3)

        return sorted(ret)

    def get_cells_old(self, p):
        """returns the list of cells, a point is located in,

           this can be more than one cell if the point is on
           a border edge


                |       |
             --------------
                |     x |
                |  XX   |
                |       |
             --------------
                |       |


                |       |
             --------------
                |       |
            XX  x  XX   |
                |       |
             -------
                |       |


                |       |
             --------------
                |       |
                |  XX   |
                |       |
              -------x---
                |       |
                |  XX   | 
                |       |


                |       |
            XX  |  XX   |
                |       |
              --x---------
                |       |
            XX  |  XX   |
                |       |


        """

        rest_x = round(float(p[0]) % self.scale, 7)
        rest_y = round(float(p[1]) % self.scale, 7)

        vertical_one = True if rest_x == 0 else False
        horizontal_one = True if rest_y == 0 else False

        if not vertical_one and not horizontal_one:
            return [(int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))]

        elif vertical_one and not horizontal_one:
            ret_0 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))

            ret_1 = (int(math.floor(float(p[0]) / self.scale)) - 1,
                     int(math.floor(float(p[1]) / self.scale)))
            ret = [ret_0]
            if not ret_1[0] < 0:
                ret.append(ret_1)

            return sorted(ret)

        elif not vertical_one and horizontal_one:
            ret_0 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))

            ret_1 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)) - 1)

            ret = [ret_0]
            if not ret_1[1] < 0:
                ret.append(ret_1)

            return sorted(ret)

        elif vertical_one and horizontal_one:
            ret_0 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)))

            ret_1 = (int(math.floor(float(p[0]) / self.scale)) - 1,
                     int(math.floor(float(p[1]) / self.scale)))

            ret_2 = (int(math.floor(float(p[0]) / self.scale)),
                     int(math.floor(float(p[1]) / self.scale)) - 1)

            ret_3 = (int(math.floor(float(p[0]) / self.scale)) - 1,
                     int(math.floor(float(p[1]) / self.scale)) - 1)
            ret = [ret_0]

            if not ret_1[0] < 0:
                ret.append(ret_1)
            if not ret_2[1] < 0:
                ret.append(ret_2)
            if ret_3[0] >= 0 and ret_3[1] >= 0:
                ret.append(ret_3)

        return sorted(ret)

    def convert_trajectroy_points_to_bypassed_cells(self, trajectory):
        """eats a list of trajectory points
        for each point in the list, the corresponding cell is determined
        all cell_ids are kept in a set to prevent dublictions
        finaly a list of all cells is returned
        """

        nodes_in_between_set = set()

        for traj_point in trajectory:
            traj_point_x = traj_point[0]
            traj_point_y = traj_point[1]
            traj_point_angle = traj_point[2]

            cell_id_x, cell_id_y = self.get_cell((traj_point_x, traj_point_y))
            sector_id = self.get_sector_id_from_angle(traj_point_angle)

            nodes_in_between_set.add((cell_id_x, cell_id_y, sector_id))

        nodes_in_between_list = sorted(list(nodes_in_between_set))

        return nodes_in_between_list

    class visNode():

        def __init__(self, xx,  yy, color, description=None):
            self.x_id = xx
            self.y_id = yy
            self.description = description
            self.color = color

    def vis_reset(self):
        self.vis_cluster = [[None for yy in range(
            self.cluster_length_y)] for xx in range(self.cluster_length_x)]

    def vis_update(self, some_vis_node):
        # assert     some_vis_node is vis_node
        self.vis_cluster[some_vis_node.x_id][
            some_vis_node.y_id] = some_vis_node

    def vis_add_colored_point(self, point, color, text=None):
        vis_node = self.visNode(point[0], point[1], color, text)
        self.vis_update(vis_node)

    def vis_add_start(self, point):
        self.vis_add_colored_point(point, (0, 100, 100), 'start')

    def vis_add_end(self, point):
        self.vis_add_colored_point(point, (0, 100, 100), 'end')

    def vis_add_open(self, point):
        #        self.vis_add_colored_point(point, (0, 0, 500), 'open node')
        self.vis_add_colored_point(point, (150, 150, 500), 'open node')

    def vis_add_closed(self, point):
        #        self.vis_add_colored_point(point, (300, 200, 200), 'closed node')
        self.vis_add_colored_point(point, (120, 120, 500), 'closed node')

    def vis_add_path(self, point):
        #        self.vis_add_colored_point(point, (0, 200, 0), 'path node')
        self.vis_add_colored_point(point, (80, 80, 250), 'path node')

    def vis_add_blocked(self, point):
        self.vis_add_colored_point(point, (100, 100, 100), 'blocked node')

    def vis_add_current(self, point):
        self.vis_add_colored_point(point, (255, 105, 180), 'current node')

    def vis_add_green(self, points):
        assert (isinstance(points, list))
        for pp in points:
            self.vis_add_colored_point(pp, (0, 200, 0), 'green shadow')

    def vis_add_red(self, points):
        assert (isinstance(points, list))
        for pp in points:
            self.vis_add_colored_point(pp, (200, 0, 0), 'red shadow')

    def vis_add_grey(self, points):
        assert (isinstance(points, list))
        for pp in points:
            self.vis_add_colored_point(pp, (100, 100, 100), 'grey shadow')

    def vis_add_black(self, points):
        assert (isinstance(points, list))
        for pp in points:
            self.vis_add_colored_point(pp, (0, 0, 0), 'black shadow')

    def vis_add_reachable(self, points):
        if isinstance(points, tuple):
            self.vis_add_colored_point(point, (255, 165, 0), 'reachable node')
        elif isinstance(points, list):
            for pp in points:
                self.vis_add_colored_point(pp, (255, 165, 0), 'reachable node')


# def block_nodes(self, nodes_to_block): # tupel list
#            for node in nodes_to_block:
#                node_x, node_y = node
#                if node_x < self.cluster_length_x and node_y <= self.cluster_length_y:
#                    self.cluster[node_x][node_y].block_node()

    # visualisation
    def get_node_box(self, x_val, y_val):
        upper_left_x = x_val * self.tile_length
        upper_left_y = y_val * self.tile_length
        lower_right_x = (x_val + 1) * self.tile_length
        lower_right_y = (y_val + 1) * self.tile_length
        return (upper_left_x, upper_left_y, lower_right_x, lower_right_y)

    def init_image(self):
        self.image_length_x = self.cluster_length_x * self.tile_length
        self.image_length_y = self.cluster_length_y * self.tile_length

        if haveImaging:
            #            self.im = Image.new("RGB", (int(self.image_length_x),
            # int(self.image_length_y)), "white")
            self.im = Image.new("RGB", (int(self.image_length_x),
                                        # int(self.image_length_y)), 'blue')
                                        int(self.image_length_y)), 'rgb(0%,0%,20%)')

            self.draw = ImageDraw.Draw(self.im)

    def draw_course(self, vessel, r, delta):
        args = []
        args.append(vessel)
        args.append(r)
        args.append(delta)

        self.top_layer.append((self.draw_course_function, args))

    def draw_course_ghost_ship(self, vessel, xx, yy, sector_id, r, delta):
        """ the draw course functions makes heavy use of the
        vessels true position.
        For visualizing while pathfinding, the true vessel's position
        will not be altered.

        For not writing a new function, a copy of the orginal vessel
        will be made and will be set up with the position of interest.

        Then this ghost ship will be handed over to the normal function.

        """

        gost_vessel = Vessel(flaeche=vessel.flaeche,
                             hull_points=vessel.hull_points
                             )

        gost_vessel.x = xx
        gost_vessel.y = yy
        gost_vessel.rotation = vessel.flaeche.get_angle_from_sector_id(
            sector_id)

        args = []
        args.append(gost_vessel)
        args.append(r)
        args.append(delta)

        self.top_layer.append((self.draw_course_function, args))

    def draw_course_function(self, vessel, r, delta):

        prolong = 1.1
        delta = float(delta * prolong)

        current_pos_x = vessel.x
        current_pos_y = vessel.y
        current_rot = vessel.rotation
        current_rot_deg = current_rot * 360 / 2 / math.pi

        angle = int(delta / r * 360 / 2 / math.pi)

        # get centerpoint:

        # centerpoint in vessel's ego view
        ego_center_left = (0, -r)
        ego_center_right = (0, r)

        ego_streight = (delta, 0)

        # transform center_point to global
        global_center_left = vessel.transform_coord(ego_center_left,  current_rot,
                                                    (current_pos_x, current_pos_y))
        global_center_right = vessel.transform_coord(ego_center_right, current_rot,
                                                     (current_pos_x, current_pos_y))

        global_streight = vessel.transform_coord(ego_streight,     current_rot,
                                                 (current_pos_x, current_pos_y))

        global_box_left = (int(global_center_left[0] - r), int(global_center_left[1] - r),
                           int(global_center_left[0] + r), int(global_center_left[1] + r))

        global_box_right = (int(global_center_right[0] - r), int(global_center_right[1] - r),
                            int(global_center_right[0] + r), int(global_center_right[1] + r))

        # draw course
        if False:
            self.draw.line(
                [(current_pos_x, current_pos_y), global_streight], 'blue')

            self.draw.arc(global_box_left,
                          int(90 - angle + current_rot_deg),
                          int(90 + current_rot_deg),  'red')  # end_angle, start_angle

            self.draw.arc(global_box_right,
                          int(-90 + current_rot_deg),
                          int(-90 + angle + current_rot_deg),  'green')

    def vis_add_poly(self, my_poly, color=None, width=None):
        if color is None:
            color = 'red'
#            color = 'green'
        if width is None:
            width = 1
        self.polies.append((my_poly, color, width))

    def vis_add_single_point(self, point, color=None):
        if color is None:
            color = 'black'
        self.points.append((point, color))

    def vis_show(self, my_poly=None, step_num=None):
        if not haveImaging:
            return
        # draw the grid
        for xx in range(self.cluster_length_x):

            for yy in range(self.cluster_length_y):
                if self.vis_cluster[xx][yy] != None:
                    self.draw.rectangle(self.get_node_box(xx, yy),
                                        self.vis_cluster[xx][yy].color, outline=120)
                else:
                    self.draw.rectangle(self.get_node_box(xx, yy),
                                        #                                        'white', outline=120)
                                        'rgb(80%,80%,100%)', outline=120)

        if len(self.polies) > 0:
            for poly in self.polies:
                self.draw.line(poly[0], fill=poly[1], width=poly[2])

        if len(self.points) > 0:
            for po in self.points:
                self.draw.point(po[0], fill=po[1])

        if my_poly is not None:
            self.draw.line(my_poly, fill='red')

        filename = self.output + '.bmp'
        if step_num is not None:
            filename = self.output + '%05i' % step_num + '.bmp'

        # draw course lines
#        for fun in self.top_layer:
#            fun[0](*fun[1])

        if step_num is not None and len(self.top_layer) > 0:
            if len(self.top_layer) >= step_num - 1:
                self.top_layer[
                    step_num - 2][0](*self.top_layer[step_num - 2][1])

        else:
            for fun in self.top_layer:
                fun[0](*fun[1])

        self.im.save(filename)

#        im.show()


class Layer():

    """Layer should provide an easy way to scetch up some Data 
       from a Flaeche"""

    def __init__(self, flaeche):
        self.image_length_x = flaeche.cluster_length_x * flaeche.tile_length
        self.image_length_y = flaeche.cluster_length_y * flaeche.tile_length

        self.canvas = Image.new(
            "RGB", (self.image_length_x, self.image_length_y), "white")
        self.draw_object = ImageDraw.Draw(self.canvas)

    def draw_poly(self, my_poly, start=None):
        """draw polygon onto the layer, therefore convert positions"""
        scale = 10
        offset_x = 0
        offset_y = 0
        my_poly = [((pp[0] + offset_x) * scale, (pp[1] + offset_y) * scale)
                   for pp in my_poly]

        self.draw_object.line(my_poly, fill='red')
#        self.draw_object.line(my_poly, fill='green', width=5)

    def show(self):
        self.canvas.show()

# write to stdout
#im.save(sys.stdout, "PNG")


def make_movie(file_base):
    import subprocess
    subprocess.call(["convert",  "-set",  "delay", "3",
                     "-colors", "16",
                     "-dispose", "1",
                     "-loop", "1",
                     "-scale", "100%",
                     file_base + "*.bmp",
                     file_base + ".gif"])

if __name__ == '__main__':
    print '...'


#                sn_node_data = self.flaeche.get_node_data((nn.cell_x_id,nn.cell_y_id))
#                sn_sector    = nn.sector_id
#                sn_tt        = current_node.tt + nn.durration
#                sn_dd        = self.vessel.guess_best_way_to_point(
#                                 from_point = self.flaeche.get_cell_center((nn.cell_x_id,
#                                                                            nn.cell_y_id)),
#                                 omega_from = nn.angle,
#                                 to_point   = self.flaeche.get_cell_center((self.end[0],
#                                                                            self.end[1])),
#                                 guess='best')[-1]


#                sus_node =  StarNodeC(
#                    node_data =self.flaeche.get_node_data((nn.cell_x_id,nn.cell_y_id)),
#                    sector    =nn.sector_id,
#                    tt        =current_node.tt + nn.durration,
#
#                    dd        =sn_dd,#
#
# dd        =self.vessel.guess_best_way_to_point(
##                                 from_point = (nn.cell_x_id, nn.cell_y_id),
##                                 omega_from = nn.angle,
# to_point   = self.flaeche.get_cell_center((self.end[0],
# self.end[1])),
# guess='best')[-1],
#
# dd        = self.get_distance_between_points((nn[0], nn[1]),
# (self.end[0], self.end[1])) /speed,
#                    reached_by_angle = nn.angle,
#                    lastNode  =current_node)

# zone checker code
def zone_checker():
    for ii in range(0):  # dummy remove
        check_zone = False
        if check_zone:

            # zone = any point in the rectengual between the sus_node
            # and the current node

            zone_x_start = int(min(current_node.x_id, nn.cell_x_id))
            zone_x_end = int(max(current_node.x_id, nn.cell_x_id))
            zone_y_start = int(min(current_node.y_id, nn.cell_y_id))
            zone_y_end = int(max(current_node.y_id, nn.cell_y_id))

            if zone_x_start == zone_x_end:
                zone_x_end += 1
            if zone_y_start == zone_y_end:
                zone_y_end += 1

            one_in_zone_is_bocked = False

            zone = [(z_ii, z_jj) for z_ii in range(zone_x_start, zone_x_end)
                    for z_jj in range(zone_y_start, zone_y_end)]

            illegal_points = 0
            for z_pp in zone:
                if not self.flaeche.is_valid_node_pos(z_pp):
                    illegal_points += 1
                    continue
                if self.flaeche.is_blocked(z_pp):
                    one_in_zone_is_bocked = True

            assert illegal_points < len(
                zone), 'all zone nodes are illegal, must be wrong'

            if one_in_zone_is_bocked:
                continue   # skip points

            shade_check = True  # False
            if shade_check:

                # check black shade
                # get black shade of destination point
                # get coord_center to destination point
                offset = self.flaeche.get_possition_from_cell_center_id(
                    (current_node.x_id, current_node.y_id))
                fake_omega = current_node.reached_by_angle

                all_zone_nodes_black_shade_ok = True
                for cc in zone + [(nn.cell_x_id, nn.cell_y_id)]:
                    black_shade = self.vessel.get_black_shade(
                        fake_omega, offset, visual)
                    for bb in black_shade:
                        #                           print bb
                        if not self.flaeche.is_valid_node_pos(bb):
                            # print 'not valid', bb
                            all_zone_nodes_black_shade_ok = False
                            break
                        if self.flaeche.is_blocked(bb):
                            #                               print 'blocked', bb
                            all_zone_nodes_black_shade_ok = False
                            break

                if not all_zone_nodes_black_shade_ok:
                    continue   # skip points

                del(offset, fake_omega, black_shade, bb, cc)
