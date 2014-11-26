
class StarNode():
    def __init__(self, xx, yy, tt, dd, lastNode,
                 sector=None, reached_by_angle=None):
        self.x_id             = xx         # int
        self.y_id             = yy         # int
        self.sector           = sector
        self.reached_by_angle = reached_by_angle
        self.is_blocked       = False
        self.tt               = tt            # true distance traveled until here
        self.dd               = dd            # estmated distance to destination
        self.full             = dd + tt

        self.previousNode = lastNode

        
        self.id = self.get_id()

        
    def get_id(self):
        if self.sector is None:
            return "%s_%s" % (self.x_id, self.y_id)
        else:
            return "%s_%s_%s" % (self.x_id, self.y_id, self.sector)

    def get_coords(self):
        if self.sector is None:
            return (self.x_id,self.y_id)
        else:
            return (self.x_id,self.y_id,  self.sector)

class StarNodeC(StarNode):
    def __init__(self, node_data, tt, dd, lastNode,
                 sector=None, reached_by_angle=None):

        self.node_data_handler = NodeDataHandler(node_data)  
        
        self.x_id                     = self.node_data_handler.x_id      
        self.y_id                     = self.node_data_handler.y_id
        self.x_coord, self.y_coord    = self.node_data_handler.get_center()
        self.sector                   = sector
        self.reached_by_angle         = reached_by_angle
        self.is_blocked               = False
        self.tt                       = tt            # true distance traveled until here
        self.dd                       = dd            # estmated distance to destination
        self.full                     = dd + tt

        self.previousNode = lastNode
        self.id = self.get_id()


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
                        self.data[self.index].sector)
            else:
                return self.data[self.index]
    def get_by_id(self, id):
            for nn in self.data:
                if nn.id == id:
                    return nn            
    def get_by_tuple(self, tuple):
            for nn in self.data:
                if (nn.x_id == tuple[0] and nn.y_id == tuple[1]
                    and nn.sector == tuple[2]):
                    return nn
    def get_min_node(self, pop=False):
            if len(self.data) == 0:
                return None
            else:
                ret = self.data[0]
                for nn in self.data:
                    if nn.full < ret.full:
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
                    if nn.full < ret.full:
                        ret = nn
                if pop:
                    self.data.remove(ret)
                return ret
    def get_tuples(self):
            return [nn.get_coords() for nn in self.data]

            
    
#class AdAStarNode(StarNode):
#        def __init__(self, xx, yy, sec, tt, dd, lastNode):
#            StarNode.__init__(self, xx, yy, tt, dd, lastNode)
#            self.sector = sec
#            print self.sector
#            self.id = "%s_%s_%s" % self.get_coords()
#            
#        def get_coords(self):
#            return (self.x_id,self.y_id, self.sector)



class NodeDataHandler():
    flaeche    = 0 
    x_id       = 1
    y_id       = 2
    is_blocked = 3

    def __init__(self, nodeDataTuple):
        flaeche_in, x_id_in, y_id_in, is_blocked_in = nodeDataTuple
        
        self.x_id       = x_id_in
        self.y_id       = y_id_in
        self.flaeche    = flaeche_in 
        self.is_blocked = is_blocked_in
        
        self.center_x, self.center_y = self.get_center()
        
    def get_center(self):
        return self.flaeche.get_cell_center((self.x_id, self.y_id))
        
