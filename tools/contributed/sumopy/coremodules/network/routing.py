# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
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

# @file    routing.py
# @author  Joerg Schweizer
# @date   2012

import os
import numpy as np


import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
#from agilepy.lib_base.misc import get_inversemap
#from agilepy.lib_base.geometry import find_area
from agilepy.lib_base.processes import Process, CmlMixin, ff, call, P, filepathlist_to_filepathstring, Options
#from coremodules.network.network import SumoIdsConf


class priorityDictionary(dict):
    def __init__(self):
        '''Initialize priorityDictionary by creating binary heap
            of pairs (value,key).  Note that changing or removing a dict entry will
            not remove the old pair from the heap until it is found by smallest() or
            until the heap is rebuilt.'''
        self.__heap = []
        dict.__init__(self)

    def smallest(self):
        '''Find smallest item after removing deleted items from heap.'''
        if len(self) == 0:
            raise IndexError, "smallest of empty priorityDictionary"
        heap = self.__heap
        while heap[0][1] not in self or self[heap[0][1]] != heap[0][0]:
            lastItem = heap.pop()
            insertionPoint = 0
            while 1:
                smallChild = 2*insertionPoint+1
                if smallChild+1 < len(heap) and \
                        heap[smallChild][0] > heap[smallChild+1][0]:
                    smallChild += 1
                if smallChild >= len(heap) or lastItem <= heap[smallChild]:
                    heap[insertionPoint] = lastItem
                    break
                heap[insertionPoint] = heap[smallChild]
                insertionPoint = smallChild
        return heap[0][1]

    def __iter__(self):
        '''Create destructive sorted iterator of priorityDictionary.'''
        def iterfn():
            while len(self) > 0:
                x = self.smallest()
                yield x
                del self[x]
        return iterfn()

    def __setitem__(self, key, val):
        '''Change value stored in dictionary and add corresponding
            pair to heap.  Rebuilds the heap if the number of deleted items grows
            too large, to avoid memory leakage.'''
        dict.__setitem__(self, key, val)
        heap = self.__heap
        if len(heap) > 2 * len(self):
            self.__heap = [(v, k) for k, v in self.iteritems()]
            self.__heap.sort()  # builtin sort likely faster than O(n) heapify
        else:
            newPair = (val, key)
            insertionPoint = len(heap)
            heap.append(None)
            while insertionPoint > 0 and val < heap[(insertionPoint-1)//2][0]:
                heap[insertionPoint] = heap[(insertionPoint-1)//2]
                insertionPoint = (insertionPoint-1)//2
            heap[insertionPoint] = newPair

    def setdefault(self, key, val):
        '''Reimplement setdefault to call our customized __setitem__.'''
        if key not in self:
            self[key] = val
        return self[key]

    def update(self, other):
        for key in other.keys():
            self[key] = other[key]


def dijkstra(id_node_start, nodes, edges, ids_node_target=None,  weights={}):
    """
    OUTDATED!!! see edgedijkstra
    Calculates minimum cost tree and minimum route costs from 
    id_node_start to all nodes of the network or to 
    target nodes given in set ids_node_target.
    Attention does not take into consideration missing connectors!!
    """
    # print '\n\ndijkstraPlain',id_node_start.getID()
    # dictionary of final distances
    D = {}
    # dictionary of predecessors
    P = {}
    # est.dist. of non-final vert.
    Q = priorityDictionary()
    Q[id_node_start] = 0
    for v in Q:
        D[v] = Q[v]

        if ids_node_target is not None:
            ids_node_target.discard(v)
            # if ids_node_target.discard(v):
            if len(ids_node_target) == 0:
                return (D, P)
        # print ' v=',v.getID(),len(v.getOutgoing())
        for id_edge in nodes.ids_outgoing[v]:
            # print '    ',edge.getID(),edge._to.getID()
            w = edges.ids_tonode[id_edge]
            #vwLength = D[v] + weights.get(edge,edge._cost)
            vwLength = D[v] + weights.get(id_edge, edges.lengths[id_edge])
            if w not in D and (w not in Q or vwLength < Q[w]):
                Q[w] = vwLength
                P[w] = id_edge
    return (D, P)


def edgedijkstra_backwards(id_edge_start, cost_limit,
                           weights=None, bstar=None):
    """
    Calculates minimum cost tree and minimum route costs from 
    id_edge_start to all edges of the network or to 
    target edges given in set ids_edge_target.

    """
    ids_origin = set()
    # print 'edgedijkstra_backwards id_edge_start',id_edge_start,'cost_limit',cost_limit
    # dictionary of final distances
    D = {}

    # dictionary of predecessors
    P = {}
    # est.dist. of non-final vert.

    if np.isnan(weights[id_edge_start]):
        print '  no access id_edge_start, weights', id_edge_start, weights[id_edge_start]
        return ([], {}, {})

    Q = priorityDictionary()
    Q[id_edge_start] = weights[id_edge_start]
    ids_edges_nochange = set()
    for e in Q:
        if (e not in ids_edges_nochange) & (e not in ids_origin):

            D[e] = Q[e]
            has_changed = False

            # print '  --------------'
            # print '  toedge',e,'ids_bedge',bstar[e]
            # print '    D=',D
            # print '    Q=',Q
            if not bstar.has_key(e):
                print 'WARNING in edgedijkstra: bstar has no edge', e
                print 'routes = \n', P
                return ([], None, P)

            for id_edge in bstar[e]:
                if 0:
                    weight_tot = D[e] + weights[id_edge]
                    newstate = '|'
                    if id_edge not in D:
                        newstate += '*D'
                    if id_edge not in Q:
                        newstate += '*Q'
                    elif weight_tot < Q[id_edge]:
                        newstate += '<Q'
                    else:
                        newstate += '>Q|'
                    print '    id_bedge', id_edge, 'w=%.2f,w_tot=%.2f' % (weights[id_edge], weight_tot), weights[id_edge] >= 0, D[e] + weights[id_edge] < cost_limit, id_edge not in D, (id_edge not in Q or weight_tot < Q[id_edge]), newstate

                if not np.isnan(weights[id_edge]):  # edge accessible?
                    weight_tot = D[e] + weights[id_edge]
                    if weight_tot < cost_limit:
                        if id_edge not in D and (id_edge not in Q or weight_tot < Q[id_edge]):
                            Q[id_edge] = weight_tot
                            P[id_edge] = e
                            has_changed = True
                    else:
                        # print '   **found origin',e
                        ids_origin.add(e)

            # print '  has_changed',e,has_changed
            if not has_changed:
                #    break
                ids_edges_nochange.add(e)

    # print '  P',P
    # print '  D',D
    return (ids_origin, D, P)  # returns in tree with all reachable destinations


def get_edges_orig_from_tree(id_edge_dest, tree):
    """Returns a list of edge origins with destination edge id_edge_dest
    from a tree in dictionary form"""
    # print 'get_edges_orig_from_tree id_edge_dest',id_edge_dest
    ids_orig = []
    ids_edge = [id_edge_dest]
    is_cont = True
    ids_edge_from = np.array(tree.keys(), dtype=np.int32)
    ids_edge_to = np.array(tree.values(), dtype=np.int32)
    if id_edge_dest not in ids_edge_to:
        return ids_orig
    while len(ids_edge) > 0:
        # print '    ids_edge',ids_edge
        ids_edge_new = []
        for id_edge in ids_edge:
            inds = np.flatnonzero(ids_edge_to == id_edge)
            # print '      id_edge',id_edge,'inds',inds
            if len(inds) == 0:
                ids_orig.append(id_edge)
                # print '      ids_orig',ids_orig
            else:
                ids_edge_new += ids_edge_from[inds].tolist()

        ids_edge = ids_edge_new

    return set(ids_orig)


def edgedijkstra(id_edge_start, ids_edge_target=None,
                 weights=None, fstar=None):
    """
    Calculates minimum cost tree and minimum route costs from 
    id_edge_start to all edges of the network or to 
    target edges given in set ids_edge_target.

    """
    ids_target = ids_edge_target.copy()
    # print 'edgedijkstra'
    # dictionary of final distances
    D = {}

    # dictionary of predecessors
    P = {}
    # est.dist. of non-final vert.

    if np.isnan(weights[id_edge_start]):
        print '  WARNING in edgedijkstra: no access id_edge_start, weights', id_edge_start, weights[id_edge_start]
        return ({}, {})

    Q = priorityDictionary()
    Q[id_edge_start] = weights[id_edge_start]

    for e in Q:
        D[e] = Q[e]

        if ids_target is not None:
            ids_target.discard(e)
            if len(ids_target) == 0:
                return (D, P)
        if not fstar.has_key(e):
            print 'WARNING in edgedijkstra: fstar has no edge', e
            print 'routes = \n', P
            return (None, P)
        for id_edge in fstar[e]:
            if not np.isnan(weights[id_edge]):  # edge accessible?
                weight_tot = D[e] + weights[id_edge]
                if id_edge not in D and (id_edge not in Q or weight_tot < Q[id_edge]):
                    Q[id_edge] = weight_tot
                    P[id_edge] = e
    return (D, P)  # returns in tree with all reachable destinations


def get_mincostroute_edge2edges(id_rootedge, ids_targetedge, D=None, P=None,
                                weights=None, fstar=None):
    """
    Returns costs and shortest paths from rootedge to a list of targetedges.
    D, P could be precalculated for rootnode with function dijkstraPlainEdge


    """
    if D is None:
        D, P = edgedijkstra(id_rootedge, set(ids_targetedge),
                            weights=weights, fstar=fstar)
    routes = []
    costs = []
    for id_targetedge in ids_targetedge:
        if P.has_key(id_targetedge):
            route = [id_targetedge]
            e = id_targetedge
            while e != id_rootedge:
                id_edge = P[e]
                route.append(id_edge)
                e = id_edge
            # route.append(e)
            route.reverse()
            routes.append(route)
            costs.append(D[id_targetedge])
        else:
            # this target edge has not been reached
            routes.append([])
            costs.append(-1.0)

    return costs, routes


def get_mincostroute_edge2edge(id_rootedge, id_targetedge, D=None, P=None,
                               weights=None, fstar=None):
    """
    Returns cost and shortest path from rootedge to a specific targetedge.
    D, P could be precalculated for rootnode with function dijkstraPlainEdge

    """
    # print 'get_mincostroute_edge2edge',id_rootedge, id_targetedge
    if D is None:
        D, P = edgedijkstra(id_rootedge, set([id_targetedge, ]),
                            weights=weights, fstar=fstar)

    route = [id_targetedge]
    if not P.has_key(id_targetedge):
        return 0.0, []

    e = id_targetedge
    while e != id_rootedge:
        # print '  follow',e
        id_edge = P[e]
        route.append(id_edge)
        e = id_edge
    # route.append(e)
    route.reverse()
    return D[id_targetedge], route


def get_mincostroute_node2node(id_rootnode, id_targetnode, D, P, edges):
    """
    Returns cost and shortest path from rootnode to a specific targetnode.
    D, P must be precalculated for rootnode with function dijkstraPlain

    """
    # print 'getMinCostRoute node_start=%s, edge_end =%s node_end=%s'%(rootnode.getID(),P[targetnode].getID(),targetnode.getID())
    id_node = id_targetnode
    route = []
    if not P.has_key(id_targetnode):
        return 0.0, []

    while id_node != id_rootnode:
        id_edge = P[id_node]
        route.append(id_edge)
        id_node = edges.ids_fromnode[id_edge]

    # for edge in route:
    #    print '        ',edge.getID()
    route.reverse()
    return D[id_targetnode], route


def duaroute(tripfilepath, netfilepath, routefilepath, weightfilepath=None,
             weightattribute='traveltime',
             options='-v --ignore-errors'):
    """
    Simple shortes path duaoute function
    """
    #  do not use options: --repair --remove-loops
    cmd = 'duarouter '+options+' --route-files %s --net-file %s --output-file %s'\
        % (ff(tripfilepath), ff(netfilepath), ff(routefilepath))

    if weightfilepath is not None:
        cmd += " --weight-files %s --weight-attribute %s" % (ff(weightfilepath), weightattribute)

    return call(cmd)


def init_random(self, **kwargs):
    optiongroup = 'random'
    self.add_option('is_timeseed', kwargs.get('is_timeseed', False),
                    groupnames=[optiongroup, 'options', ],
                    name='Time seed',
                    perm='rw',
                    info='Initialises the random number generator with the current system time.',
                    cml='--random',
                    )

    self.add_option('seed', kwargs.get('seed', 23423),
                    groupnames=[optiongroup, 'options', ],
                    name='Random seed',
                    perm='rw',
                    info='Initialises the random number generator with the given value.',
                    cml='--seed',
                    )


class RouterMixin(CmlMixin, Process):

    def init_options_time(self, **kwargs):
        optiongroup = 'time'

        self.add_option('time_begin', kwargs.get('time_begin', -1),
                        groupnames=[optiongroup, 'options', ],
                        name='Start time',
                        perm='rw',
                        info='Defines the begin time; Previous trips will be discarded. The value of  -1 takes all routes from the beginning.',
                        unit='s',
                        cml='--begin',
                        is_enabled=lambda self: self.time_begin >= 0.0,
                        )

        self.add_option('time_end', kwargs.get('time_end', kwargs.get('time_end', -1)),
                        groupnames=[optiongroup, 'options', ],
                        name='End time',
                        perm='rw',
                        info='Defines the end time; Later trips will be discarded; The value of -1 takes all routes to the end.',
                        unit='s',
                        cml='--end',
                        is_enabled=lambda self: self.time_end >= 0.0,
                        )

    def init_options_processing_common(self, **kwargs):
        optiongroup = 'processing'

        self.add_option('n_alternatives_max', kwargs.get('n_alternatives_max', 5),
                        name='Max. alternatives',
                        info='Maximum number of considered route alternatives.',
                        cml='--max-alternatives',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('is_ignore_errors', kwargs.get('is_ignore_errors', True),
                        name='Ignore disconnected',
                        info='Continue if a route could not be build.',
                        cml='--ignore-errors',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('n_threads', kwargs.get('n_threads', 0),
                        name='Parallel threads',
                        info="The number of parallel execution threads used for routing.",
                        cml='--routing-threads',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )
        attrsman = self.get_attrsman()
        self.is_update_current_routes = attrsman.add(cm.AttrConf('is_update_current_routes', kwargs.get('is_update_current_routes', True),
                                                                 groupnames=['options', 'misc'],
                                                                 perm='rw',
                                                                 name='Update current routes',
                                                                 info="Update current routes and or generate current routes if inexistent. If disabeled, new alternative routes are created.",
                                                                 ))

    def init_options_processing_dua(self, **kwargs):
        optiongroup = 'processing'

        self.add_option('time_preload', kwargs.get('time_preload', 200),
                        name='Preload time',
                        unit='s',
                        info='Load routes for the next number of seconds ahead.',
                        cml='--route-steps',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )
        # self.add_option('is_randomize_flows',kwargs.get('is_randomize_flows',False),
        #                name = 'Preload time',
        #                info = 'generate random departure times for flow input.',
        #                cml = '--randomize-flows',
        #                groupnames = [optiongroup,'options',],#
        #                perm='rw',
        #                )

        self.add_option('is_remove_loops', kwargs.get('is_remove_loops', False),
                        name='Remove loops',
                        info='Remove loops within the route; Remove turnarounds at start and end of the route. May cause errors!',
                        cml='--remove-loops',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('is_repair', kwargs.get('is_repair', False),
                        name='Repair',
                        info='Tries to correct a false route. May cause errors!',
                        cml='--repair',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('is_repair_from', kwargs.get('is_repair_from', False),
                        name='Repair start',
                        info='Tries to correct an invalid starting edge by using the first usable edge instead.',
                        cml='--repair.from',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('is_repair_to', kwargs.get('is_repair_to', False),
                        name='Repair end',
                        info='Tries to correct an invalid destination edge by using the last usable edge instead.',
                        cml='--repair.to',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('is_bulkrouting', kwargs.get('is_bulkrouting', False),
                        name='Bulk routing?',
                        info="Aggregate routing queries with the same origin.",
                        cml='--bulk-routing',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        # --weights.interpolate <BOOL>     Interpolate edge weights at interval boundaries; default: false
        # --weight-period <TIME>     Aggregation period for the given weight files; triggers rebuilding of Contraction Hierarchy; default: 3600
        # --weights.expand <BOOL>     Expand weights behind the simulation's end; default: false

        # --with-taz <BOOL>     Use origin and destination zones (districts) for in- and output; default: false

    def init_options_methods(self, **kwargs):
        optiongroup = 'methods'

        self.add_option('method_routechoice', kwargs.get('method_routechoice', 'logit'),
                        name='Routechoice method',
                        choices=['gawron', 'logit', 'lohse'],
                        info="Mathematical model used for route choice.",
                        cml='--route-choice-method',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('beta_gawron', kwargs.get('beta_gawron', 0.3),
                        name="Gawron's 'beta'",
                        info="Gawron's 'beta' parameter.",
                        cml='--gawron.beta',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        is_enabled=lambda self: self.method_routechoice is 'gawron',
                        )

        self.add_option('a_gawron', kwargs.get('a_gawron', 0.05),
                        name="Gawron's 'a'",
                        info="Gawron's 'a' parameter.",
                        cml='--gawron.a',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        is_enabled=lambda self: self.method_routechoice is 'gawron',
                        )

        self.add_option('beta_logit', kwargs.get('beta_logit', 0.15),
                        name="Logit's 'beta'",
                        info="C-Logit's 'beta' parameter.",
                        cml='--logit.beta',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        is_enabled=lambda self: self.method_routechoice is 'logit',
                        )

        self.add_option('gamma_logit', kwargs.get('gamma_logit', 1.0),
                        name="Logit's 'gamma'",
                        info="C-Logit's 'gamma' parameter.",
                        cml='--logit.gamma',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        is_enabled=lambda self: self.method_routechoice is 'logit',
                        )

        self.add_option('theta_logit', kwargs.get('theta_logit', 0.01),
                        name="Logit's 'theta'",
                        info="C-Logit's 'theta' parameter.",
                        cml='--logit.theta',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        is_enabled=lambda self: self.method_routechoice is 'logit',
                        )

        self.add_option('algorithm_routing', kwargs.get('algorithm_routing', 'dijkstra'),
                        name='Routing algorithm',
                        choices=['dijkstra', 'astar', 'CH', 'CHWrapper'],
                        info="Select among routing algorithms.",
                        cml='--routing-algorithm',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

        self.add_option('is_keep_all_routes', kwargs.get('is_keep_all_routes', False),
                        name='Keep all routes?',
                        info="Save even routes with near zero probability.",
                        cml='--keep-all-routes',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )
        self.add_option('is_skip_new_routes', kwargs.get('is_skip_new_routes', False),
                        name='Skip new routes?',
                        info="Only reuse routes from input, do not calculate new ones.",
                        cml='--skip-new-routes',
                        groupnames=[optiongroup, 'options', ],
                        perm='rw',
                        )

    def do(self):
        if self.is_export_net:
            # first export current net
            self.parent.export_netxml(self.netfilepath)

        if self.is_export_trips:
            self._trips.export_trips_xml(self.tripfilepaths)

        self.update_params()
        cml = self.get_cml()

        # print 'SumonetImporter.do',cml
        self.run_cml(cml)
        if self.status == 'success':
            print '  Routing done.'
            if os.path.isfile(self.outfilepath):
                # print '  outfile exists, start importing routes'
                if self.is_update_current_routes:
                    print '  update current routes'
                    self._trips.import_routes_xml(self.outfilepath,
                                                  is_clear_trips=False,
                                                  is_generate_ids=False,
                                                  is_overwrite_only=True,
                                                  is_add=False
                                                  )
                else:
                    print '  create route alternatives'
                    self._trips.import_routes_xml(self.outfilepath,
                                                  is_clear_trips=False,
                                                  is_generate_ids=True,
                                                  is_overwrite_only=False,
                                                  is_add=True)
                return True
            return False
        return False


class DuaRouter(RouterMixin):
    def __init__(self, net, trips,
                 tripfilepaths=None,
                 outfilepath=None,
                 is_export_net=True,
                 logger=None,
                 **kwargs):
        print 'DuaRouter.__init__ net, trips', net, trips
        self.init_tripsrouter('duarouter', net,  # net becomes parent
                              trips,
                              outfilepath=outfilepath,
                              logger=logger,
                              is_export_net=is_export_net,
                              )

        if tripfilepaths is None:
            if trips is not None:
                tripfilepaths = trips.get_tripfilepath()
                self.is_export_trips = True
            else:
                self.is_export_trips = False

        else:
            self.is_export_trips = False
        print '  tripfilepaths', tripfilepaths
        if tripfilepaths is not None:
            self.add_option('tripfilepaths', tripfilepaths,
                            groupnames=['_private'],
                            cml='--route-files',
                            perm='r',
                            name='Trip file(s)',
                            wildcards='Trip XML files (*.trip.xml)|*.trip.xml',
                            metatype='filepaths',
                            info='SUMO Trip files in XML format.',
                            )

        # no, -1 as default will do
        # if not kwargs.has_key('time_begin'):
        #    kwargs['time_begin'] = trips.get_time_depart_first()
        #
        # if not kwargs.has_key('time_end'):
        #    kwargs['time_end'] = trips.get_time_depart_last()

        self.init_options_time(**kwargs)
        self.init_options_methods(**kwargs)
        self.init_options_processing_common(**kwargs)
        self.init_options_processing_dua(**kwargs)
        init_random(self, **kwargs)

    def init_tripsrouter(self, ident, net,
                         trips,
                         netfilepath=None,
                         outfilepath=None,
                         name='Duarouter',
                         info='Generates routes from trips, flows or previous routes',
                         is_export_net=True,
                         logger=None, cml='duarouter'):

        self._init_common(ident, name=name,
                          parent=net,
                          logger=logger,
                          info=info,
                          )

        self.init_cml(cml)  # pass main shell command
        self.is_export_net = is_export_net
        self._trips = trips
        attrsman = self.get_attrsman()
        if netfilepath is None:
            netfilepath = net.get_filepath()

        self.add_option('netfilepath', netfilepath,
                        groupnames=['_private'],
                        cml='--net-file',
                        perm='r',
                        name='Net file',
                        wildcards='Net XML files (*.net.xml)|*.net.xml',
                        metatype='filepath',
                        info='SUMO Net file in XML format.',
                        )

        if outfilepath is None:
            outfilepath = trips.get_routefilepath()

        self.add_option('outfilepath', outfilepath,
                        groupnames=['_private'],
                        cml='--output-file',
                        perm='r',
                        name='Out routefile',
                        wildcards='Route XML files (*.rou.xml)|*.rou.xml',
                        metatype='filepath',
                        info='Output file of the routing process, which is a SUMO route file in XML format.',
                        )


class MaRouter(CmlMixin, Process):
    def __init__(self, scenario,
                 is_export_net=True,
                 is_export_rou=True,
                 is_prompt_filepaths=False,
                 flowfilepath=None,
                 netfilepath=None,
                 ptstopsfilepath=None,
                 logger=None,
                 results=None,
                 **kwargs):

        self._init_common('marouter', name='Macroscopic Router',
                          parent=scenario,
                          logger=logger,
                          info='Macroscopic assignment generating route flows.',
                          )

        self.init_cml('marouter')  # pass  no commad to generate options only

        self._results = results
        self.time_warmup = 0.0  # needed for compatibility with sumo process
        attrsman = self.get_attrsman()

        self.add_option('assignmentmethod', kwargs.get('assignmentmethod', 'incremental'),
                        groupnames=['options', 'processing'],
                        cml='--assignment-method',
                        choices={'Incremental all or nothing': 'incremental',
                                 'User equilibrium (not yet implemented)': 'UE', 'Stochastic Use equilibrium': 'SUE'},
                        name='Assignment method',
                        info='Assignment method.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        simtime_start_default = scenario.demand.get_time_depart_first()
        # estimate end of simtime
        simtime_end_default = scenario.demand.get_time_depart_last()

        self.add_option('simtime_start', kwargs.get('simtime_start', simtime_start_default),
                        groupnames=['options', 'timing'],
                        cml='--begin',
                        name='Start time',
                        perm='rw',
                        info='Start time of simulation in seconds after midnight.',
                        unit='s',
                        )

        self.add_option('simtime_end', kwargs.get('simtime_end', simtime_end_default),
                        groupnames=['options', 'timing'],
                        cml='--end',
                        name='End time',
                        perm='rw',
                        info='End time of simulation in seconds after midnight.',
                        unit='s',
                        )

        self.add_option('aggregationinterval', kwargs.get('aggregationinterval', 3600),
                        groupnames=['options', 'processing'],
                        cml='--aggregation-interval',
                        name='Aggrigation interval',
                        info='Defines the time interval when aggregating single vehicle input.',
                        untit='s',
                        )

        self.add_option('n_max_alternatives', kwargs.get('n_max_alternatives', 5),
                        groupnames=['options', 'processing'],
                        cml='--max-alternatives',
                        name='Maximum route alternatives',
                        info='Prune the number of alternatives to this integer.',
                        )

        self.add_option('is_interpolate_weights', kwargs.get('is_interpolate_weights', False),
                        groupnames=['options', 'processing'],
                        cml='--weights.interpolate',
                        name='Interpolate edge weights',
                        info='Interpolate edge weights at time interval boundaries.',
                        )

        self.add_option('is_expand_weights', kwargs.get('is_expand_weights', False),
                        groupnames=['options', 'processing'],
                        cml='--weights.expand',
                        name='Expand edge weights',
                        info='Expand edge weights at time interval boundaries.',
                        )

        self.add_option('routingalgorithm', kwargs.get('routingalgorithm', 'dijkstra'),
                        groupnames=['options', 'processing'],
                        choices=['dijkstra', 'astar', 'CH', 'CHWrapper'],
                        cml='--routing-algorithm',
                        name='Routing algorithm',
                        info='Routing algorithm.',
                        )

        self.add_option('n_routing_threads', kwargs.get('n_routing_threads', 1),
                        groupnames=['options', 'processing'],
                        cml='--routing-threads',
                        name='Number of routing threats',
                        info='The number of parallel execution threads used for routing',
                        )

        self.add_option('is_additive_traffic', kwargs.get('is_additive_traffic', False),
                        groupnames=['options', 'processing'],
                        cml='--additive-traffic',
                        name='Additive traffic',
                        info='Keep traffic flows of all time slots in the net.',
                        )

        self.add_option('tolerance', kwargs.get('tolerance', 0.001),
                        groupnames=['options', 'processing'],
                        cml='--tolerance',
                        name='SUE tolerance',
                        info='Tolerance when checking for SUE stability.',
                        )

        self.add_option('penalty_leftturn', kwargs.get('penalty_leftturn', 0.0),
                        groupnames=['options', 'processing'],
                        cml='--left-turn-penalty',
                        name='Left turn penalty',
                        info='Left-turn penalty to calculate link travel time when searching routes.',
                        )

        self.add_option('penalty_paths', kwargs.get('penalty_paths', 1.0),
                        groupnames=['options', 'processing'],
                        cml='--paths.penalty',
                        name='Paths penalty',
                        info='Penalize existing routes with  this time to find secondary routes.',
                        untit='s',
                        )

        self.add_option('c_upperbound', kwargs.get('c_upperbound', 0.5),
                        groupnames=['options', 'processing'],
                        cml='--upperbound ',
                        name='Cost upperbound',
                        info='Upper bound to determine auxiliary link cost.',
                        )

        self.add_option('c_lowerbound', kwargs.get('c_lowerbound', 0.15),
                        groupnames=['options', 'processing'],
                        cml='--lowerbound ',
                        name='Cost lowerbound',
                        info='Lower bound to determine auxiliary link cost.',
                        )

        self.add_option('n_iter_max', kwargs.get('n_iter_max', 20),
                        groupnames=['options', 'processing'],
                        cml='--max-iterations',
                        name='Max. number of iterations',
                        info='maximal number of iterations for new route searching in incremental and stochastic user assignment.',
                        )
        self.add_option('n_iter_inner_max', kwargs.get('n_iter_inner_max', 1000),
                        groupnames=['options', 'processing'],
                        cml='--max-inner-iterations',
                        name='Max. number of inner iterations',
                        info='Maximal number of inner iterations for user equilibrium calcuation in the stochastic user assignment.',
                        )

        self.add_option('routechoicemethod', kwargs.get('routechoicemethod', 'logit'),
                        groupnames=['options', 'processing'],
                        choices=['gawron', 'logit', 'lohse'],
                        cml='--route-choice-method',
                        name='Route choice method',
                        info='Route choice method. Logit recommended, no entered vehicles with Gavron.',
                        )

        self.add_option('beta_gavron', kwargs.get('beta_gavron', 0.3),
                        groupnames=['options', 'processing'],
                        cml='--gawron.beta',
                        name="Gawron's beta",
                        info="Gawron's beta parameter, only valid if Gavron's route choice method is selected.",
                        is_enabled=lambda self: self.routechoicemethod == 'gawron',
                        )

        self.add_option('a_gavron', kwargs.get('a_gavron', 0.05),
                        groupnames=['options', 'processing'],
                        cml='--gawron.a',
                        name="Gawron's a",
                        info="Gawron's a parameter. Only valid if Gavron's route choice method is selected.",
                        is_enabled=lambda self: self.routechoicemethod == 'gawron',
                        )

        self.add_option('beta_logit', kwargs.get('beta_logit', 0.15),
                        groupnames=['options', 'processing'],
                        cml='--logit.beta',
                        name="C-Logits's beta",
                        info="C-logit's beta for the commonality factor. Only valid if Logit route choice method is selected",
                        is_enabled=lambda self: self.routechoicemethod == 'logit',
                        )

        self.add_option('gamma_logit', kwargs.get('gamma_logit', 1.0),
                        groupnames=['options', 'processing'],
                        cml='--logit.gamma',
                        name="C-Logits's gamma",
                        info="C-logit's gamma for the commonality factor. Only valid if Logit route choice method is selected",
                        is_enabled=lambda self: self.routechoicemethod == 'logit',
                        )

        self.add_option('theta_logit', kwargs.get('theta_logit', 0.01),
                        groupnames=['options', 'processing'],
                        cml='--logit.theta',
                        name="C-Logits's theta",
                        info="C-logit's theta. Only valid if Logit route choice method is selected",
                        is_enabled=lambda self: self.routechoicemethod == 'logit',
                        )

        self.add_option('is_keep_all_routes', kwargs.get('is_keep_all_routes', False),
                        groupnames=['options', 'processing'],
                        cml='--keep-all-routes',
                        name="keep all routes",
                        info="Consider even routes with near zero probability.",
                        )

        self.add_option('is_skip_new_routes', kwargs.get('is_skip_new_routes', False),
                        groupnames=['options', 'processing'],
                        cml='--skip-new-routes',
                        name="Skip new routes",
                        info="Only reuse routes from input routes, do not calculate new ones.",
                        )

        self.add_option('seed', 1234,
                        groupnames=['options', 'processing'],
                        cml='--seed',
                        name='Random seed',
                        info='Initialises the random number generator with the given value.',
                        )

        self.add_option('is_ignore_errors', kwargs.get('is_ignore_errors', True),
                        groupnames=['options', 'processing'],
                        cml='--ignore-errors',
                        name="Ignore errors",
                        info="Ignore errors.",
                        )

        self.logfilepath = attrsman.add(cm.AttrConf('logfilepath', kwargs.get('logfilepath', ''),
                                                    groupnames=['options', 'misc'],
                                                    perm='rw',
                                                    name='Log file',
                                                    wildcards='Log file (*.txt)|*.txt',
                                                    metatype='filepath',
                                                    info="Writes all messages to Log filepath, implies verbous. If blank, no logfile is created",
                                                    ))

        if ptstopsfilepath is None:
            self.ptstopsfilepath = scenario.net.ptstops.get_stopfilepath()

            self.is_export_ptstops = attrsman.add(cm.AttrConf('is_export_ptstops', True,
                                                              groupnames=['input', 'options'],
                                                              perm='rw',
                                                              name='Export PT stops?',
                                                              info='Export PT stops before simulation?',
                                                              ))
        else:
            self.ptstopsfilepath = ptstopsfilepath

        self.is_export_flow = attrsman.add(cm.AttrConf('is_export_flow', kwargs.get('is_export_flow', True),
                                                       groupnames=['input', 'options'],
                                                       perm='rw',
                                                       name='Export OD flows?',
                                                       info='Export OD flows before simulation? Needs to be done only once after OD flows changed.',
                                                       ))

        if flowfilepath is None:
            self.flowfilepath = scenario.demand.odintervals.get_flowfilepath()

        else:
            self.is_export_flow = False
            self.flowfilepath = flowfilepath

        if netfilepath is None:
            self.netfilepath = scenario.net.get_filepath()

            self.is_export_net = attrsman.add(cm.AttrConf('is_export_net', is_export_net,
                                                          groupnames=['input', 'options'],
                                                          perm='rw',
                                                          name='Export net?',
                                                          info='Export current network before simulation? Needs to be done only once after network has changed.',
                                                          ))
        else:
            self.is_export_net = False
            self.netfilepath = netfilepath

        self.routeoutputfilepath = None
        self.flowoutputfilepath = None

    def do(self):
        cml = self.get_cml()
        scenario = self.parent

        # exports, if required
        if self.is_export_net:
            ptstopsfilepath = scenario.net.ptstops.export_sumoxml(self.ptstopsfilepath)
            scenario.net.export_netxml(self.netfilepath)
        else:
            ptstopsfilepath = self.ptstopsfilepath

        if self.is_export_flow & (self.flowfilepath.count(',') == 0):
            scenario.demand.odintervals.export_amitranxml(self.flowfilepath)

        # routefiles are only used to specify vtypes
        routefilepath = scenario.demand.trips.get_routefilepath()
        scenario.demand.vtypes.export_xml(routefilepath)

        zonefilepath = scenario.landuse.zones.export_sumoxml()

        rootname = scenario.get_rootfilename()
        rootdirpath = scenario.get_workdirpath()

        self.routeoutputfilepath = os.path.join(rootdirpath, rootname+'.out.mrflow.xml')
        self.flowoutputfilepath = os.path.join(rootdirpath, rootname+'.out.mrload.xml')

        cml += ' --net-file %s' % (ff(self.netfilepath)) +\
            ' --route-files %s' % (ff(routefilepath)) +\
            ' --od-amitran-files %s' % (filepathlist_to_filepathstring(self.flowfilepath)) +\
            ' --output-file %s' % (ff(self.routeoutputfilepath)) +\
            ' --netload-output %s' % (ff(self.flowoutputfilepath))

        additionalpaths = [zonefilepath, ]
        if os.path.isfile(self.ptstopsfilepath):
            additionalpaths.append(self.ptstopsfilepath)

        cml += ' --additional-files %s' % (filepathlist_to_filepathstring(additionalpaths))

        if self.logfilepath != '':
            cml += ' --log %s' % (ff(self.logfilepath))

        # print '\n Starting command:',cml
        if call(cml):
            if os.path.isfile(self.flowoutputfilepath):
                # self.parent.import_routes_xml(routefilepath)
                # os.remove(routefilepath)
                return True
            else:
                return False
        else:
            return False

    def get_results(self):
        return self._results

    def import_results(self, results=None):
        """
        Imports simulation resuts into results object.
        """
        print 'import_results of marouter'

        if results is None:
            results = self._results
            if results is None:
                results = self.parent.simulation.results

        if results is not None:
            #self.routeoutputfilepath = None

            if self.flowoutputfilepath is not None:
                # print '  self.flowoutputfilepath',self.flowoutputfilepath
                # print '  results.edgeresults',results.edgeresults,id(results.edgeresults)
                results.edgeresults.import_marouterxml(self.flowoutputfilepath, self)
