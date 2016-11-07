import numpy as np


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
                smallChild = 2 * insertionPoint + 1
                if smallChild + 1 < len(heap) and \
                        heap[smallChild][0] > heap[smallChild + 1][0]:
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
            while insertionPoint > 0 and val < heap[(insertionPoint - 1) // 2][0]:
                heap[insertionPoint] = heap[(insertionPoint - 1) // 2]
                insertionPoint = (insertionPoint - 1) // 2
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
    Calculates minimum cost tree and minimum route costs from 
    id_node_start to all nodes of the network or to 
    target nodes given in set ids_node_target.
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

        if ids_node_target != None:
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


def edgedijkstra(id_edge_start, nodes, edges, ids_edge_target=None, weights={}):
    """
    Calculates minimum cost tree and minimum route costs from 
    id_edge_start to all edges of the network or to 
    target edges given in set ids_edge_target.
    """
    # dictionary of final distances
    D = {}
    # dictionary of predecessors
    P = {}
    # est.dist. of non-final vert.
    Q = priorityDictionary()
    Q[id_edge_start] = weights.get(id_edge_start, edges.lengths[id_edge_start])
    for e in Q:
        D[e] = Q[e]

        if ids_edge_target != None:
            ids_edge_target.discard(e)
            # if ids_edge_target.discard(e):
            if len(ids_edge_target) == 0:
                return (D, P)

        for id_edge in edges.get_outgoing(e):
            #w = edge._to
            #vwLength = D[v] + weights.get(edge,edge._cost)
            vwLength = D[e] + weights.get(id_edge, edges.lengths[id_edge])
            if id_edge not in D and (id_edge not in Q or vwLength < Q[id_edge]):
                Q[id_edge] = vwLength
                P[id_edge] = e
    return (D, P)


def get_mincostroute_edge2edge(id_rootedge, id_targetedge, D, P):
    """
    Returns cost and shortest path from rootedge to a specific targetedge.
    D, P must be precalculated for rootnode with function dijkstraPlainEdge

    """

    route = [id_targetedge]
    if not P.has_key(id_targetedge):
        return 0.0, []

    e = id_targetedge
    while e != id_rootedge:
        id_edge = P[e]
        route.append(id_edge)
        e = id_edge
    route.reverse()
    return D[id_targetedge], route


def get_mincostroute_node2node(id_rootnode, id_targetnode, D, P, edges):
    """
    Returns cost and shortest path from rootnode to a specific targetnode.
    D, P must be precalculated for rootnode with function dijkstraPlain
    """
    # print 'getMinCostRoute node_start=%s, edge_end =%s
    # node_end=%s'%(rootnode.getID(),P[targetnode].getID(),targetnode.getID())
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
