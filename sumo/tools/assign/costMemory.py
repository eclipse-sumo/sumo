#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file    duaIterate.py
@author  Jakob Erdmann
@date    2012-03-14
@version $Id$

Perform smoothing of edge costs across successive iterations of duaIterate

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os,sys
from collections import defaultdict
from xml.sax import saxutils, make_parser, handler

class EdgeMemory:
    def __init__(self, cost):
        self.cost = cost
        self.seen = True

    def update(self, cost, memory_factor):
        self.cost = self.cost * memory_factor + cost * (1 - memory_factor) 
        self.seen = True

    def decay_unseen(self, memory_factor):
        if not self.seen:
            self.update(0, memory_factor)
            return True
        return False


class CostMemory(handler.ContentHandler):
    # memorize the weighted average of edge costs
    def __init__(self, cost_attribute):
        # the cost attribute to parse (i.e. 'traveltime')
        self.cost_attribute = cost_attribute.decode('utf8')
        # the duaIterate iteration index
        self.iteration = None
        # the main data store: for every interval and edge id we store costs and
        # whether data was seen in the last call of load_costs()
        # (start,end) -> (edge_id -> EdgeMemory)
        self.intervals = defaultdict(dict) 
        # the intervall currently being parsed
        self.current_interval = None
        # the combined weigth of all previously loaded costs
        self.memory_weight = 0
        # update is done according to: memory * memory_factor + new * (1 - memory_factor)
        self.memory_factor = None
        # differences between the previously loaded costs and the memorized costs
        self.errors = None
        # some statistics
        self.num_loaded = 0
        self.num_decayed = 0


    def startElement(self, name, attrs):
        if name == 'interval':
            interval_key = (float(attrs['begin']), float(attrs['end']))
            self.current_interval = self.intervals[interval_key]
        if name == 'edge':
            id = attrs['id']
            if attrs.has_key(self.cost_attribute): # may be missing for some
                self.num_loaded += 1
                cost = float(attrs[self.cost_attribute])
                if id in self.current_interval:
                    edgeMemory = self.current_interval[id]
                    self.errors.append(edgeMemory.cost - cost)
                    edgeMemory.update(cost, self.memory_factor)
                else:
                    self.errors.append(0)
                    self.current_interval[id] = EdgeMemory(cost)


    def load_costs(self, dumpfile, iteration, weight):
        # load costs from dumpfile and update memory according to weight and iteration
        if weight <= 0:
            sys.stderr.write("Skipped loading of costs because the weight was %s but should have been > 0\n" % weight)
            return
        assert(weight > 0)
        if self.iteration == None and iteration != 0:
            print "Warning: continuing with empty memory"
        # update memory weights. memory is a weighted average across all runs
        self.memory_factor = self.memory_weight / (self.memory_weight + weight)
        self.memory_weight += weight
        self.iteration = iteration
        self.errors = []
        # mark all edges as unseen
        for edges in self.intervals.itervalues():
            for edgeMemory in edges.itervalues():
                edgeMemory.seen = False
        # parse costs
        self.num_loaded = 0
        parser = make_parser()
        parser.setContentHandler(self)
        parser.parse(dumpfile)
        # decay costs of unseen edges
        self.num_decayed = 0
        for edges in self.intervals.itervalues():
            for edgeMemory in edges.itervalues():
                if edgeMemory.decay_unseen(self.memory_factor):
                    self.num_decayed += 1


    def write_costs(self, weight_file):
        with open(weight_file, 'w') as f:
            f.write('<netstats>\n')
            for (start, end), edge_costs in self.intervals.iteritems():
                f.write('    <interval begin="%d" end="%d">\n' % (start,end))
                for id, edgeMemory in edge_costs.iteritems():
                    f.write('        <edge id="%s" %s="%s"/>\n' % (id, self.cost_attribute, edgeMemory.cost))
                f.write('    </interval>\n')
            f.write('</netstats>\n')

    def avg_error(self, values=None):
        if not values:
            values = self.errors
        if len(values) > 0:
            return sum(values) / len(values)
        else:
            return 0

    def avg_abs_error(self):
        return self.avg_error(map(abs,self.errors))

    def mean_error(self, values=None):
        if not values:
            values = self.errors
        values.sort()
        if values:
            return values[len(values)/2]

    def mean_abs_error(self):
        return self.mean_error(map(abs,self.errors))

    def loaded(self):
        return self.num_loaded

    def decayed(self):
        return self.num_decayed

