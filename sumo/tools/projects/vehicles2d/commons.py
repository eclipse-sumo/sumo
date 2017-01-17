#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    commons.py
@author  Marek Heinrich
@date    2014-11-17
@version $Id$
>>>>>>> .r17519

Commons module.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from math import pi


class StarNode():

    def __init__(self, xx, yy, tt, dd, lastNode,
                 sector=None, reached_by_angle=None):
        self.x_id = xx         # int
        self.y_id = yy         # int
        self.sector_id = sector
        self.reached_by_angle = reached_by_angle
        self.is_blocked = False
        self.tt = tt            # true distance traveled until here
        self.dd = dd            # estmated distance to destination
        self.full_costs = dd + tt

        self.previousNode = lastNode

        self.id = self.get_id()

    def get_id(self):
        if self.sector_id is None:
            return "%s_%s" % (self.x_id, self.y_id)
        else:
            return "%s_%s_%s" % (self.x_id, self.y_id, self.sector_id)

    def get_coords(self):
        if self.sector_id is None:
            return (self.x_id, self.y_id)
        else:
            return (self.x_id, self.y_id,  self.sector_id)


def reduce_angle_to_get_smalles_postive_equivialent(angle):
    if angle < 0:
        while angle < 0:
            angle += 2 * pi
    elif angle >= 2 * pi:
        while angle >= 2 * pi:
            angle -= 2 * pi

    return angle


class StarNodeC(StarNode):

    def __init__(self,
                 node_data, sector_id, reached_by_angle,
                 costs_till_here, estimated_remaining_costs,
                 previous_node, in_between_nodes=[]):

        self.node_data = node_data
        self.node_data_handler = NodeDataHandler(self.node_data)

        self.x_id = self.node_data_handler.x_id
        self.y_id = self.node_data_handler.y_id
        self.x_coord, self.y_coord = self.node_data_handler.get_center()
        self.sector_id = sector_id
        self.reached_by_angle = reduce_angle_to_get_smalles_postive_equivialent(
            reached_by_angle)
        self.costs_till_here = costs_till_here
        self.estimated_remaining_costs = estimated_remaining_costs
        self.full_costs = (self.costs_till_here
                           + self.estimated_remaining_costs)
        self.previous_node = previous_node
        self.in_between_nodes = in_between_nodes

        self.id = self.get_id()

# self.dd                         = dd            # estmated distance to destination
# self.tt                         = tt            # true distance traveled
# until here

#        self.full                       = self.full_costs
#        self.is_blocked                 = False


class ANList():

    def __init__(self, data, order_by=None):
        self.data = data
        self.index = -1
        self.order_by = order_by

    def __iter__(self):
        return self

    def next(self):
        if self.index == len(self.data) - 1:
            raise StopIteration
        self.index += 1
        if self.order_by == 'id':
            return self.data[self.index].id
        if self.order_by == 'tuple':
            return (self.data[self.index].x_id, self.data[self.index].y_id,
                    self.data[self.index].sector_id)
        else:
            return self.data[self.index]

    def get_by_id(self, id):
        for nn in self.data:
            if nn.id == id:
                return nn

    def get_by_tuple(self, tuple):
        for nn in self.data:
            if (nn.x_id == tuple[0] and nn.y_id == tuple[1]
                    and nn.sector_id == tuple[2]):
                return nn

    def get_min_node(self, pop=False):
        if len(self.data) == 0:
            return None
        else:
            ret = self.data[0]
            for nn in self.data:
                if nn.full_costs < ret.full_costs:
                    ret = nn
            if pop:
                self.data.remove(ret)
            return ret

    def get_tuples(self):
        return [nn.get_coords() for nn in self.data]

    def get_ids(self):
        return [nn.id for nn in self.data]


class DNList():

    def __init__(self, data, order_by=None):
        self.data = data
        self.index = -1
        self.order_by = order_by

    def __iter__(self):
        return self

    def next(self):
        if self.index == len(self.data) - 1:
            raise StopIteration
        self.index += 1
        if self.order_by == 'id':
            return self.data[self.index].id
        if self.order_by == 'tuple':
            return (self.data[self.index].x_id, self.data[self.index].y_id)
        else:
            return self.data[self.index]

    def get_by_id(self, id):
        for nn in self.data:
            if nn.id == id:
                return nn

    def get_by_tuple(self, tuple):
        for nn in self.data:
            if nn.x_id == tuple[0] and nn.y_id == tuple[1]:
                return nn

    def get_min_node(self, pop=False):
        if len(self.data) == 0:
            return None
        else:
            ret = self.data[0]
            for nn in self.data:
                if nn.full_costs < ret.full_costs:
                    ret = nn
            if pop:
                self.data.remove(ret)
            return ret

    def get_tuples(self):
        return [nn.get_coords() for nn in self.data]


# class AdAStarNode(StarNode):
#        def __init__(self, xx, yy, sec, tt, dd, lastNode):
#            StarNode.__init__(self, xx, yy, tt, dd, lastNode)
#            self.sector = sec
#            print self.sector
#            self.id = "%s_%s_%s" % self.get_coords()
#
#        def get_coords(self):
#            return (self.x_id,self.y_id, self.sector)


class NodeDataHandler():
    flaeche = 0
    x_id = 1
    y_id = 2
    is_blocked = 3

    def __init__(self, nodeDataTuple):
        flaeche_in, x_id_in, y_id_in, is_blocked_in = nodeDataTuple

        self.x_id = x_id_in
        self.y_id = y_id_in
        self.flaeche = flaeche_in
        self.is_blocked = is_blocked_in

        self.center_x, self.center_y = self.get_center()

    def get_center(self):
        return self.flaeche.get_cell_center((self.x_id, self.y_id))

#    def get_x_id(node_data):
#        return node_data[x_id]
#
#    def get_y_id(node_data):
#        return node_data[y_id]

    def get_x_and_y_id(node_data):
        return (node_data[x_id], node_data[y_id])
