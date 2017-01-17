#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    dijkstra_base.py
@author  Marek Heinrich
@date    2014-11-17
@version $Id$

Dijkstra module.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import math

from commons import NodeDataHandler, StarNode, DNList
#from commons import StarNode as DijkstraNode


class Dijkstra():

    #    class DijkstraNode():
    #        def __init__(self, xx, yy, tt, dd, lastNode):
    # self.x_id  = xx         # int
    # self.y_id  = yy         # int
    #            self.is_blocked = False
    # self.tt = tt            # true distance traveled until here
    # self.dd = dd            # estmated distance to destination
    #            self.full = dd + tt
    #            self.previousNode = lastNode
    #            self.id = "%s_%s" % self.get_coords()
    #
    #        def get_coords(self):
    #            return (self.x_id,self.y_id)

    #    class DNList():
    #        def __init__(self, data, order_by=None):
    #            self.data = data
    #            self.index = -1
    #            self.order_by = order_by
    #        def __iter__(self):
    #            return self
    #        def next(self):
    #            if self.index == len(self.data) - 1:
    #                raise StopIteration
    #            self.index += 1
    #            if self.order_by == 'id':
    #                return self.data[self.index].id
    #            if self.order_by == 'tuple':
    #                return (self.data[self.index].x_id, self.data[self.index].y_id)
    #            else:
    #                return self.data[self.index]
    #        def get_by_id(self, id):
    #            for nn in self.data:
    #                if nn.id == id:
    #                    return nn
    #        def get_by_tuple(self, tuple):
    #            for nn in self.data:
    #                if nn.x_id == tuple[0] and nn.y_id == tuple[1]:
    #                    return nn
    #        def get_min_node(self, pop=False):
    #            if len(self.data) == 0:
    #                return None
    #            else:
    #                ret = self.data[0]
    #                for nn in self.data:
    #                    if nn.full < ret.full:
    #                        ret = nn
    #                if pop:
    #                    self.data.remove(ret)
    #                return ret
    #        def get_tuples(self):
    #            return [nn.get_coords() for nn in self.data]

    def __init__(self, flaeche, start_node, end_node, vehilce=None):
        self.start = start_node
        self.end = end_node
        self.flaeche = flaeche

        if not (self.flaeche.is_valid_node_pos(start_node) and
                self.flaeche.is_valid_node_pos(end_node)):
            raise StandardError
        if start_node == end_node:
            raise StandardError

        self.reached_dest_node = None
        self.iteration_step = 0
        self.open_nodes_list = []
        self.closed_nodes_list = []
        self.path = []

        # gbm bergamond

    def get_distance_between_points(self, node_1, node_2):
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

    def get_node_list(self, DNList, type=None):
        if type == 'tuples':
            return [nn.get_coords() for nn in DNList]
        else:
            return [nn for nn in DNList]

    def get_open_nodes(self, type=None):
        return self.get_node_list(self.open_nodes_list, type)

    def get_closed_nodes(self, type=None):
        return self.get_node_list(self.closed_nodes_list, type)

    def step(self, visual=False):
        self.iteration_step += 1

        # first step ever
        if len(self.open_nodes_list) == 0 and len(self.closed_nodes_list) == 0:
            self.open_nodes_list.append(StarNode(self.start[0], self.start[1],
                                                 0, self.get_distance_between_points((self.start),
                                                                                     (self.end)),
                                                 None))
            return False    # Algorithm is not jet finished

        current_node = DNList(self.open_nodes_list).get_min_node(pop=True)

        # even if the dest. node has been seen before,
        # the path is not prooven to be the shortest

        # until it has been teared from the open list
        if current_node.get_coords() == self.end:
            self.reached_dest_node = current_node
            return True  # finished / found

        # only coords are returned
        suspicious_nodes = self.flaeche.get_neighbours(
            current_node.get_coords())
        for nn in suspicious_nodes:
            # is in closed_list -> ignore
            closedDNL = DNList(self.closed_nodes_list, 'tuple')
            if not nn in closedDNL:

                # skip if blocked
                if self.flaeche.is_blocked(nn):
                    continue

                sus_node = StarNode(nn[0], nn[1],
                                    tt=current_node.tt +
                                    self.get_distance_between_points(
                                        current_node.get_coords(), nn),
                                    dd=self.get_distance_between_points(
                                        nn, (self.end)),
                                    lastNode=current_node)

                # is in open_list -> event. update open list
                openDNL = DNList(self.open_nodes_list, 'tuple')
                # returns None if not in list
                some_open_node = openDNL.get_by_tuple(nn)
                if some_open_node != None:
                    if sus_node.full_costs < some_open_node.full_costs:
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
        self.draw_open_closed(step)

    def run(self, visual=False):
        while not self.step(visual):
            pass

    def rebuild_path(self, some_node=None):
        if self.reached_dest_node == None:
            raise StandardError("algorithm must be run first successfully")
        elif some_node == None:
            some_node = self.reached_dest_node
#        print some_node.get_coords()
        self.path[0:0] = [some_node]
        if some_node.get_coords() == self.start:
            return
        self.rebuild_path(some_node.previousNode)

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
        for nn in DNList(self.open_nodes_list).get_tuples():
            self.flaeche.vis_add_open(nn)
        self.flaeche.vis_show()

    def draw_closed(self):
        self.flaeche.vis_reset()
        for nn in DNList(self.closed_nodes_list).get_tuples():
            self.flaeche.vis_add_closed(nn)
        self.flaeche.vis_show()

    def draw_open_closed(self, step=None):
        self.flaeche.vis_reset()
        self.flaeche.vis_add_start(self.start)
        self.flaeche.vis_add_end(self.end)
        for nn in DNList(self.open_nodes_list).get_tuples():
            self.flaeche.vis_add_open(nn)
        del(nn)
        for nn in DNList(self.closed_nodes_list).get_tuples():
            self.flaeche.vis_add_closed(nn)
        for xx in range(self.flaeche.cluster_length_x):
            for yy in range(self.flaeche.cluster_length_y):
                if self.flaeche.cluster[xx][yy] != None:
                    if self.flaeche.cluster[xx][yy][NodeDataHandler.is_blocked]:
                        self.flaeche.vis_add_blocked((xx, yy))
        self.flaeche.vis_show(step_num=step)

    def draw_path(self, final=False):
        self.flaeche.vis_reset()
        self.flaeche.vis_add_start(self.start)
        self.flaeche.vis_add_end(self.end)
        for nn in DNList(self.open_nodes_list).get_tuples():
            self.flaeche.vis_add_open(nn)
        del(nn)
        for nn in DNList(self.closed_nodes_list).get_tuples():
            self.flaeche.vis_add_closed(nn)
        del(nn)
        for xx in range(self.flaeche.cluster_length_x):
            for yy in range(self.flaeche.cluster_length_y):
                if self.flaeche.cluster[xx][yy] != None:
                    if self.flaeche.cluster[xx][yy][NodeDataHandler.is_blocked]:
                        self.flaeche.vis_add_blocked((xx, yy))
        for nn in DNList(self.path).get_tuples():
            self.flaeche.vis_add_path(nn)
        if final:
            self.flaeche.vis_show(step_num=self.iteration_step)
        else:
            self.flaeche.vis_show()
