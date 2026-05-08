import os,sys, string
from xml.sax import saxutils, parse, handler
import numpy as np  
from numpy import random          
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from  agilepy.lib_base.geometry import *
#from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from coremodules.network.routing import  get_mincostroute_edge2edge
from agilepy.lib_base.processes import Process,P,call,CmlMixin
from . import demandbase as db
 
     
FLOWTYPES = {'from external in':'i', 'to external out':'o', 'internal transit':'t', 'no route':'x'}
       
                     
                        
class Flows(am.ArrayObjman):
    def __init__(self,ident, parent, edges, **kwargs):
            self._init_objman(  ident, parent=parent, 
                                name = 'Flows',
                                info = 'Contains the number of vehicles which start on the given edge during a certain time interval.',
                                version = 0.2,
                                xmltag = ('flows','flow',None), **kwargs)
            
            self._init_attributes(edges)
        
        
    def _init_attributes(self, edges = None):
            if edges is None:
                # recover edges from already initialized
                edges = self.ids_edge.get_linktab()
            
            
            if self.get_version() < 0.1:
                # update attrs from previous
                # IdsArrayConf not yet modifiable interactively, despite perm = 'rw',!!!
                self.ids_edge.set_perm('rw')
            if hasattr(self,'func_delete_row'):
                self.func_make_row._is_returnval = False
                self.func_delete_row._is_returnval = False    
                    
            self.add_col(am.IdsArrayConf( 'ids_edge', edges, 
                                    groupnames = ['state'], 
                                    name = 'Edge ID',
                                    perm = 'rw',
                                    info = 'Edge ID of flow.',
                                    xmltag = 'from',
                                    ))

        
            self.add_col(am.ArrayConf( 'flows', 0,
                                    dtype = np.int32,
                                    groupnames = ['state'], 
                                    perm = 'rw', 
                                    name = 'Flow', 
                                    info = 'Absolute number of vehicles which start on the given edge during a certain time interval.',
                                    xmltag = 'number',
                                    ))
                                    
            
            self.add_col(am.ArrayConf( 'flowtypes', 't',
                                    dtype = np.object_,
                                    groupnames = ['state'], 
                                    choices = FLOWTYPES,
                                    perm = 'rw', 
                                    name = 'Type', 
                                    info = 'Flow types:'+str(list(FLOWTYPES.keys()))
                                    ))
            
            self.add(cm.FuncConf(  'func_make_row','on_add_row', None,
                                    groupnames = ['rowfunctions','_private'], 
                                    name = 'New flow.', 
                                    info = 'Add a new flow.',
                                    is_returnval = False,
                                    ))
                                
            self.add(cm.FuncConf(  'func_delete_row','on_del_row', None,
                                    groupnames = ['rowfunctions','_private'], 
                                    name = 'Del flow', 
                                    info = 'Delete flow.',
                                    is_returnval = False,
                                    ))
                                                        
            
    
    
    
    def on_del_row(self, id_row=None):
        if id_row is not None:
            #print 'on_del_row', id_row
            self.del_row(id_row) 
    
    def on_add_row(self, id_row=None):
        if len(self)>0:
            # copy previous
            flow_last = self.get_row(self.get_ids()[-1])
            self.add_row(**flow_last)
        else:
            self.add_row(self.suggest_id())
                                    

    def _init_constants(self):
        #self.edgeweights_orig = None
        #self.edgeweights_dest = None
        pass
    
        
    def add_flow(self, id_edge, flow, flowtype):
        print('Flows.add_flows',id_edge,flow, flowtype)
        return self.add_row(   ids_edge = id_edge,
                                flows = flow,
                                flowtypes = flowtype
                                )
                                
            
    def get_edges(self):
        return  self.ids_edge.get_linktab()
    
    def get_demand(self):
        return self.parent.parent.parent
    
    def count_left(self,counter):
        """
        Counts the number of vehicles leaving the edge.
        The counter is an array where the index equals the edge ID
        and the value represens the number of entered vehicles per edge.
        """
        ids_flow = self.get_ids()
        inds_valid = self.flows[ids_flow] >= 0
        counter[self.ids_edge[ids_flow[inds_valid]]] += self.flows[ids_flow[inds_valid]]
        # retruns valid egde IDs with flows
        return self.ids_edge[ids_flow[inds_valid]]
        
    def export_xml(self, fd, vtype, id_flow, share = 1.0, indent=2):
    
        """
        Generates a line for each edge with a flow.
        
        id_flow is the flow count and is used to generate a unique flow id
        
            
        """
        # TODO: better handling of mode and vtypes, distributions 
        # DONE with share
        # <flow id="0" from="edge0" to="edge1" type= "vType" number="100"/>
        ids_eges =[] 
        ids_sumoeges = self.get_edges().ids_sumo
        i = 0
        #print 'Flows.export_xml vtypes,id_flow,len(self)',vtype,id_flow,len(self)
        for id_edge,flow in zip(self.ids_edge.get_value(), share*self.flows.get_value()):
            #print '    id_edge,flow',id_edge,flow
            if flow> -1:
                ids_eges.append(id_edge)
                id_flow += 1
                fd.write(xm.start('flow'+xm.num('id',id_flow), indent))
                fd.write(xm.num('from',ids_sumoeges[id_edge]))
                fd.write(xm.num('type',vtype))
                fd.write(xm.num('number',int(flow)) ) 
                fd.write(xm.stopit())
            i += 1
        #print '  return ids_eges, id_flow',ids_eges, id_flow    
        return ids_eges, id_flow

class Turns(am.ArrayObjman):
    def __init__(self,ident, parent, edges, **kwargs):
            self._init_objman(  ident, parent=parent, 
                                name = 'Turn flows',
                                info = 'The table contains turn probabilities between two edges during a given time interval.',
                                version = 0.1,
                                xmltag = ('odtrips','odtrip',None), **kwargs)
            
            self._init_attributes(edges)
        
        
    def _init_attributes(self, edges = None):
            if edges is None:
                # recover edges from already initialized
                edges = self.ids_fromedge.get_linktab()
            
            if self.get_version() < 0.1:
                # update attrs from previous
                # IdsArrayConf not yet modifiable interactively, despite perm = 'rw',!!!
                self.ids_fromedge.set_perm('rw')
                self.ids_toedge.set_perm('rw')
            if hasattr(self,'func_delete_row'):
                self.func_make_row._is_returnval = False
                self.func_delete_row._is_returnval = False 
            
            self.add_col(am.IdsArrayConf( 'ids_fromedge', edges, 
                                    groupnames = ['state'], 
                                    name = 'Edge ID from',
                                    info = 'Edge ID where turn starts.',
                                    xmltag = 'fromEdge',
                                    ))

            self.add_col(am.IdsArrayConf( 'ids_toedge', edges, 
                                    groupnames = ['state'], 
                                    name = 'Edge ID to',
                                    info = 'Edge ID where turn ends.',
                                    xmltag = 'toEdge',
                                    ))
            
            self.add_col(am.ArrayConf( 'flows', 0,
                                    dtype = np.int32,
                                    groupnames = ['state'], 
                                    perm = 'rw', 
                                    name = 'Flow', 
                                    info = 'Absolute number of vehicles which pass from "fromedge" to "toedge" during a certain time interval.',
                                    xmltag = 'number',
                                    ))
                                                            
            self.add_col(am.ArrayConf( 'probabilities', 0.0,
                                    dtype = np.float32,
                                    groupnames = ['state'], 
                                    perm = 'rw', 
                                    name = 'Probab.', 
                                    info = 'Probability to make a turn between "Edge ID from" and "Edge ID to" and .',
                                    xmltag = 'probability',
                                    ))
            
            self.add_col(am.ArrayConf( 'flowtypes', 't',
                                    dtype = np.object_,
                                    groupnames = ['state'], 
                                    choices = FLOWTYPES,
                                    perm = 'rw', 
                                    name = 'Type', 
                                    info = 'Flow types:'+str(list(FLOWTYPES.keys()))
                                    ))    
                                                        
            self.add(cm.FuncConf(  'func_make_row','on_add_row', None,
                                groupnames = ['rowfunctions','_private'], 
                                name = 'New turns', 
                                info = 'Add a new turnflow.',
                                is_returnval = False,
                                ))
                                
            self.add(cm.FuncConf(  'func_delete_row','on_del_row', None,
                                groupnames = ['rowfunctions','_private'], 
                                name = 'Del turns', 
                                info = 'Delete turns.',
                                is_returnval = False,
                                ))
                                                        
            
    
    
    
    def on_del_row(self, id_row=None):
        if id_row is not None:
            #print 'on_del_row', id_row
            self.del_row(id_row) 
    
    def on_add_row(self, id_row=None):
        if len(self)>0:
            # copy previous
            flow_last = self.get_row(self.get_ids()[-1])
            self.add_row(**flow_last)
        else:
            self.add_row(self.suggest_id())
    def _init_constants(self):
        #self.edgeweights_orig = None
        #self.edgeweights_dest = None
        pass
    
    
    def normalize_turnprobabilities(self):
        """
        Makes sure that sum of turn probabilities from an edge equals 1.
        """
        # TODO: there is a quicker way with picking a set and then select 
        # and sum with vectors
        #ids_source = set(self.ids_fromedge.get_values())
        
        flows_total = {}
        for _id in self.get_ids():
            id_fromedge = self.ids_fromedge[_id]
            if id_fromedge not in flows_total:
                flows_total[id_fromedge] = 0.0
            flows_total[id_fromedge] += self.flows[_id]
            
        for _id in self.get_ids():
            if flows_total[self.ids_fromedge[_id]]>0:
                self.probabilities[_id] = self.flows[_id]/ flows_total[self.ids_fromedge[_id]]
            
                
    def add_turn(self, id_fromedge,id_toedge, flow, flowtype):
        #print 'Turns.add_turn', id_fromedge,id_toedge,flow, flowtype
        return self.add_row(    ids_fromedge = id_fromedge,
                                ids_toedge = id_toedge,
                                flows = flow,
                                flowtypes = flowtype,
                                )
                                
            
    def get_edges(self):
        return  self.ids_fromedge.get_linktab()
    
    def count_entered(self,counter, id_edge_flows):
        """
        Counts the number of vehicles entered in an edge.
        The counter is an array where the index equals the edge ID
        and the value represens the number of entered vehicles per edge.
        """
        ids_turns = self.get_ids()
        ids_toedge = self.ids_toedge[ids_turns]
        
        inds_valid = self.flows[ids_turns] >= 0
        
        for id_edge, flow in zip(self.ids_toedge[ids_turns[inds_valid]], self.flows[ids_turns[inds_valid]]):
            if id_edge not in id_edge_flows:
                counter[id_edge] += flow
        
    
    
    def export_xml(self,fd,indent=0):
        # <edgeRelations>
           # <interval begin="0" end="3600">
              # <edgeRelation from="myEdge0" to="myEdge1" probability="0.2"/>
              # <edgeRelation from="myEdge0" to="myEdge2" probability="0.7"/>
              # <edgeRelation from="myEdge0" to="myEdge3" probability="0.1"/>
        
              # ... any other edges ...
        
           # </interval>
        
           # ... some further intervals ...
        
        # </edgeRelations>

        fromedge_to_turnprobs = {}
        for _id in self.get_ids():
            id_fromedge = self.ids_fromedge[_id]
            if id_fromedge not in fromedge_to_turnprobs:
                fromedge_to_turnprobs[id_fromedge] = []
            fromedge_to_turnprobs[id_fromedge].append((self.ids_toedge[_id], self.probabilities[_id]))
            
        ids_sumoeges = self.get_edges().ids_sumo
        
        fd.write(xm.begin('edgeRelations',indent))
        for id_fromedge in list(fromedge_to_turnprobs.keys()):
            
            for id_toedge, turnprob in fromedge_to_turnprobs[id_fromedge]:
                if turnprob >= 0:
                    fd.write(xm.start('edgeRelation',indent+2))
                    fd.write(xm.num('from',ids_sumoeges[id_fromedge]))
                    fd.write(xm.num('to',ids_sumoeges[id_toedge]))
                    fd.write(xm.num('probability',turnprob))
                    fd.write(xm.stopit())
                
        fd.write(xm.end('edgeRelations',indent))
                                       
class TurnflowModes(am.ArrayObjman):
    def __init__(self,ident,parent, modes, edges, **kwargs):
            self._init_objman(  ident,parent=parent,
                                name = 'Mode OD tables',
                                info = 'Contains for each transport mode an OD trip table.',
                                xmltag = ('modesods','modeods','ids_mode'), **kwargs)
            
            print('TurnflowModes.__init__',modes)
            self.add_col(am.IdsArrayConf( 'ids_mode', modes, 
                                            groupnames = ['state'], 
                                            choices = MODES,
                                            name = 'ID mode', 
                                            is_index = True,
                                            #xmltag = 'vClass',
                                            info = 'ID of transport mode.',
                                            ))
            print('  self.ids_mode.is_index',self.ids_mode.is_index())
                                                                                          
            self.add_col(cm.ObjsConf( 'flowtables', 
                                            groupnames = ['state'], 
                                            name = 'Flows', 
                                            info = 'Flow generation per edge for a specific mode.',
                                            ))
            
            self.add_col(cm.ObjsConf( 'turntables', 
                                            groupnames = ['state'], 
                                            name = 'Turns', 
                                            info = 'Turn probabilities between edges for a specific mode.',
                                            ))
                                                                            
            self.add( cm.ObjConf( edges, is_child = False,groups = ['_private']))
                                    
    #def generate_trips(self, demand, time_start, time_end,**kwargs):
    #    for id_od_mode in self.get_ids():
    #        self.odtrips[id_od_mode].generate_trips( demand, time_start, time_end, self.ids_mode[id_od_mode],**kwargs)
 
    
    
    #def generate_odflows(self, odflowtab, time_start, time_end,**kwargs):
    #    for id_od_mode in self.get_ids():
    #        self.odtrips[id_od_mode].generate_odflows( odflowtab, time_start, time_end, self.ids_mode[id_od_mode],**kwargs)
    
    def get_demand(self):
        return self.parent.parent
    
    def normalize_turnprobabilities(self):
        """
        Makes sure that sum of turn probabilities from an edge equals 1.
        """
        for _id in self.get_ids():
            self.turntables[_id].normalize_turnprobabilities()
            
             
    def add_mode(self, id_mode):
        id_tf_modes = self.add_row(ids_mode = id_mode)
        print('  add_mode',id_mode,id_tf_modes)
        
        
        flows = Flows( (self.flowtables.attrname, id_tf_modes), self, self.edges.get_value() )
        self.flowtables[id_tf_modes] = flows
        
        turns = Turns( (self.turntables.attrname, id_tf_modes), self, self.edges.get_value() )
        self.turntables[id_tf_modes] = turns
        
        return id_tf_modes
    
    def add_flow(self, id_mode, id_edge, flow, flowtype = 't'):
        """
        Sets a demand flows between from-Edge and toEdge pairs for mode where flows is a dictionary
        with (fromEdgeID,toEdgeID) pair as key and number of trips as values.
        """
        print('TurnflowModes.add_turnflows',id_mode,flow,flowtype)
        #print '  self.ids_mode.is_index()',self.ids_mode.is_index()
        if self.ids_mode.has_index(id_mode):
            id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
        else:
            id_tf_modes = self.add_mode(id_mode)
        self.flowtables[id_tf_modes].add_flow(id_edge, flow, flowtype)
        return self.flowtables[id_tf_modes]   
            
        
        
                  
    def add_turn(self,id_mode, id_fromedge, id_toedge, turnflow, flowtype = 't'):
        """
        Sets turn probability between from-edge and to-edge.
        """
        print('TurnflowModes.add_turn',id_mode,turnflow,flowtype)
        #if scale!=1.0:
        #    for od in odm.iterkeys():
        #        odm[od] *= scale
        #if not self.contains_key(mode):
        #    self._initTurnflowsMode(mode)
        if self.ids_mode.has_index(id_mode):
            id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
        else:
            id_tf_modes = self.add_mode(id_mode)
        
        self.turntables[id_tf_modes].add_turn(id_fromedge, id_toedge, turnflow, flowtype)
        return self.turntables[id_tf_modes]
    
    def export_flows_xml(self, fd, id_mode, id_flow = 0, indent=0):
        """
        Export flow data of desired mode to xml file.
        Returns list with edge IDs with non zero flows and a flow ID counter.
        """
        print('TurnflowModes.export_flows_xml id_mode, id_flow',id_mode, id_flow, self.ids_mode.has_index(id_mode))
        ids_sourceedge=[]
        
        if not self.ids_mode.has_index(id_mode):
            return ids_sourceedge,id_flow
        
        
        
        # get vtypes for specified mode
        vtypes, shares = self.get_demand().vtypes.select_by_mode(\
                            id_mode, is_sumoid = True, is_share = True)
        
        #for vtype in vtypes:
        #    print '  test vtype',vtype
        #    print '            _index_to_id', self.get_demand().vtypes.ids_sumo._index_to_id
        #    print '       id_vtype',self.get_demand().vtypes.ids_sumo.get_id_from_index(vtype)
            
            
            
        #if len(vtypes) > 0:# any vehicles found for this mode?
        #    # TODO: can we put some distributen here?
        #    vtype = vtypes[0]
        for  vtype, share in zip(vtypes, shares):
            print('  write flows for vtype',vtype,share)
            if self.ids_mode.has_index(id_mode):
                id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
                ids_sourceedge, id_flow = self.flowtables[id_tf_modes].export_xml(fd, vtype, id_flow, share = share, indent = indent)
        
        #print '  return ids_sourceedge, id_flow',ids_sourceedge, id_flow     
        return ids_sourceedge, id_flow
        
    def export_turns_xml(self, fd, id_mode, indent=0):
        """
        Export flow data of desired mode to xml file.
        Returns list with edge IDs with non zero flows and a flow ID counter.
        """
        print('TurnflowModes.export_turns_xml')

        if self.ids_mode.has_index(id_mode):
            id_tf_modes = self.ids_mode.get_id_from_index(id_mode)
            self.turntables[id_tf_modes].export_xml(fd, indent = indent)
         
    def count_entered(self,counter):
        """
        Counts the number of vehicles entered in an edge.
        The counter is an array where the index equals the edge ID
        and the value represens the number of entered vehicles per edge.
        """
        for id_tf_modes in self.get_ids():
            ids_edge_with_flows = self.flowtables[id_tf_modes].count_left(counter)
            self.turntables[id_tf_modes].count_entered(counter, ids_edge_with_flows)
        
          
    
                   
                                
class Turnflows(am.ArrayObjman):
    def __init__(self, ident, demand, net, **kwargs):
            self._init_objman(  ident,parent = demand,# = demand
                                name = 'Turnflow Demand',
                                info = 'Contains flows and turn probailities for different modes and time intervals. Here demand data is ordered by time intervals.',
                                xmltag = ('turnflows','interval',None), **kwargs)
            
            
            
            self.add_col(am.ArrayConf(  'times_start', 0,
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    name = 'Start time', 
                                    unit = 's',
                                    info = 'Start time of interval in seconds (no fractional seconds).',
                                    xmltag = 't_start',
                                    ))
            
            self.add_col(am.ArrayConf('times_end', 3600,
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    name = 'End time', 
                                    unit = 's',
                                    info = 'End time of interval in seconds (no fractional seconds).',
                                    xmltag = 't_end',
                                    ))
                                    
            self.add_col(cm.ObjsConf( 'turnflowmodes', 
                                    groupnames = ['state'], 
                                    is_save = True,
                                    name = 'Turnflows by modes', 
                                    info = 'Turnflow transport demand for all transport modes within the respective time interval.',
                                    )) 
            #self.add( cm.ObjConf( Sinkzones('sinkzones', self, demand.get_scenario().net.edges) ))
            
    def get_demand(self):
        return self.parent
    
    def normalize_turnprobabilities(self):
        """
        Makes sure that sum of turn probabilities from an edge equals 1.
        """
        print('Turnflows.normalize_turnprobabilities')
        #for turnflowmode in self.turnflowmodes.get_value():
        #    turnflowmode.normalize_turnprobabilities() # no! it's a dict!!
        #print '  ',self.turnflowmodes.get_value()
        for _id in self.get_ids():
            self.turnflowmodes[_id].normalize_turnprobabilities()
        
    
    def clear_turnflows(self):
        self.clear()
        
    
    def add_flow(self, t_start, t_end, id_mode, id_edge, flow, flowtype = 't'):
        
        #print 'turnflows.add_flow', t_start, t_end, id_mode, id_edge, flow
        ids_inter = self.select_ids((self.times_start.get_value()==t_start)&(self.times_end.get_value()==t_end))
        if len(ids_inter)==0:
            
            id_inter = self.add_row(times_start = t_start, times_end=t_end,  )
            #print '  create new',id_inter
            tfmodes = TurnflowModes( (self.turnflowmodes.attrname, id_inter), 
                                self, self.get_net().modes, self.get_net().edges)
            
            self.turnflowmodes[id_inter] = tfmodes
            
            flows = tfmodes.add_flow(id_mode, id_edge, flow, flowtype)
        
        else:
            
            # there should be only one demand table found for a certain interval
            id_inter = ids_inter[0]
            #print '  use',id_inter
            flows = self.turnflowmodes[id_inter].add_flow(id_mode,id_edge, flow, flowtype)
        return flows
                 
    def add_turn(self, t_start, t_end,id_mode,id_fromedge, id_toedge, turnflow, flowtype = 't'):
        
        print('turnflows.add_turnflows',id_fromedge, id_toedge, turnflow,flowtype)
        ids_inter = self.select_ids((self.times_start.get_value()==t_start)&(self.times_end.get_value()==t_end))
        if len(ids_inter)==0:
            
            id_inter = self.add_row(times_start = t_start, times_end=t_end,  )
            #print '  create new',id_inter
            tfmodes = TurnflowModes( (  self.turnflowmodes.attrname, id_inter), 
                                        self, self.get_net().modes, self.get_net().edges)
            
            #NO!! odmodes = OdModes( ('ODMs for modes', id_inter), parent = self, modes = self.get_net().modes, zones = self.get_zones())
            self.turnflowmodes[id_inter] = tfmodes
            
            turns = tfmodes.add_turn(id_mode,id_fromedge, id_toedge, turnflow, flowtype)
        
        else:
            
            # there should be only one demand table found for a certain interval
            id_inter = ids_inter[0]
            #print '  use',id_inter
            turns = self.turnflowmodes[id_inter].add_turn(id_mode,id_fromedge, id_toedge, turnflow, flowtype)
        return turns   
            
            
    
    def get_net(self):
        return self.parent.get_scenario().net
    
    def get_edges(self):
        return self.get_net().edges
    
    def get_modes(self):
        ids_mode = set()
        for id_inter in self.get_ids():
            ids_mode.update(self.turnflowmodes[id_inter].ids_mode.value)
        return list(ids_mode)# self.get_net().modes
    
    
    def get_sinkedges(self):
        zones = self.parent.get_scenario().landuse.zones
        ids_sinkedges = set()
        ids_sinkzone = zones.select_ids(zones.ids_landusetype.get_value() == 7)
        for ids_edge in zones.ids_edges_inside[ids_sinkzone]:
            ids_sinkedges.update(ids_edge)
        #sinkedges = zones.ids_edges_orig.get_value().tolist()
        #print 'get_sinkedges',sinkedges
        #print '  sinkedges',np.array(sinkedges,np.object_)
        return ids_sinkedges
    
    def export_flows_and_turns(self, flowfilepath, turnsfilepath, id_mode, indent=0):
        """
        Create the flow file and turn ratios file for a specific mode.
        In the hybridPY tunflow data structure, each mode has its own 
        flow and turnratio data.
        """
        print('\n\n'+79*'_')
        print('export_flows_and_turns id_mode=',id_mode,'ids_vtype=',self.parent.vtypes.select_by_mode(id_mode))
        print('  write flows',flowfilepath)
        fd = open(flowfilepath, 'w', encoding="utf-8")
        fd.write(xm.begin('flows',indent))
        
        # write all possible vtypes for this mode
        self.parent.vtypes.write_xml(   fd, indent=indent, 
                                        ids = self.parent.vtypes.select_by_mode(id_mode), 
                                        is_print_begin_end = False)
        
        id_flow = 0# ?????
        ids_allsourceedges = []
        time_start_min = +np.inf
        time_end_max =  -np.inf
        for id_inter in self.get_ids():
            time_start = self.times_start[id_inter]
            time_end = self.times_end[id_inter]
            fd.write(xm.begin('interval'+xm.num('begin',time_start)+xm.num('end',time_end),indent+2))
            ids_sourceedge, id_flow = self.turnflowmodes[id_inter].export_flows_xml(fd, id_mode, id_flow,  indent+4)
            #print '  got ids_sourceedge, id_flow',ids_sourceedge, id_flow
            ids_allsourceedges += ids_sourceedge
            
            if len(ids_sourceedge)>0:
                #print '  extend total time interval only for intervals with flow' 
                if time_start<time_start_min:
                    time_start_min = time_start
                    
                if time_end>time_end_max:
                    time_end_max = time_end
                
            fd.write(xm.end('interval',indent+2))
           
        fd.write(xm.end('flows',indent))
        fd.close()
        
        #print '  write turndefs', turnsfilepath
        fd = open(turnsfilepath, 'w', encoding="utf-8")
        fd.write(xm.begin('turns',indent))
        
        
        
        for id_inter in self.get_ids():
            time_start = self.times_start[id_inter]
            time_end = self.times_end[id_inter]
            fd.write(xm.begin('interval'+xm.num('begin',time_start)+xm.num('end',time_end),indent+2))
            self.turnflowmodes[id_inter].export_turns_xml(fd, id_mode, indent+4)
            fd.write(xm.end('interval',indent+2))
            
 
        #  take sink edges from sink zones 
        ids_sinkedge = self.get_sinkedges()# it's a set
        # ...and remove source edges, otherwise vehicle will be inserted and
        # immediately removed
        #print '  ids_sinkedge',ids_sinkedge
        #print '  ids_allsourceedges',ids_allsourceedges
        ids_sinkedge = ids_sinkedge.difference(ids_allsourceedges)
        
        ids_sumoedge = self.get_edges().ids_sumo
        #print '  determined sink edges',list(ids_sinkedge)
        if len(ids_sinkedge)>0:   
            fd.write(xm.start('sink'))
            fd.write(xm.arr('edges',ids_sumoedge[list(ids_sinkedge)]))
            fd.write(xm.stopit())
        
        fd.write(xm.end('turns',indent))
        fd.close()
        if len(ids_allsourceedges)==0:
            time_start_min = 0
            time_end_max = 0
            
        return time_start_min, time_end_max
  
    
    
            
    def estimate_entered(self):
        """
        Estimates the entered number of vehicles for each edge
        generated by turnflow definitions. Bases are the only the 
        turnflows, not the generated flows (which are not entering an edge).
        
        returns ids_edge and entered_vec
        """
            
        counter = np.zeros(np.max(self.get_edges().get_ids())+1,int)
        
        for id_inter in self.get_ids():
            self.turnflowmodes[id_inter].count_entered(counter)
            
        
        ids_edge = np.flatnonzero(counter)
        entered_vec = counter[ids_edge].copy()
        return ids_edge, entered_vec
        
    
    def turnflows_to_routes(self, is_clear_trips = True, is_export_network = True, 
                                is_make_probabilities = True, cmloptions = None,):
        #  jtrrouter --flow-files=<FLOW_DEFS> 
        # --turn-ratio-files=<TURN_DEFINITIONS> --net-file=<SUMO_NET> \
        # --output-file=MySUMORoutes.rou.xml --begin <UINT> --end <UINT>
        
        if is_make_probabilities:
            self.normalize_turnprobabilities()
        
        scenario = self.parent.get_scenario()
        if cmloptions is None:
            cmloptions = '-v --max-edges-factor 1  --seed 23423 --repair --ignore-vclasses false --ignore-errors --turn-defaults 5,90,5'
        
        trips = scenario.demand.trips
        if is_clear_trips:
            # clear all current trips = routes
            trips.clear_trips()
        
    
        
        rootfilepath = scenario.get_rootfilepath()
        netfilepath = scenario.net.get_filepath()
        flowfilepath = rootfilepath+'.flow.xml'
        turnfilepath = rootfilepath+'.turn.xml'
        
        routefilepath = trips.get_routefilepath()
        
        # first generate xml for net
        if is_export_network:
            scenario.net.export_netxml()
        
        ids_mode = self.get_modes()
        print('turnflows_to_routes', ids_mode) #scenario.net.modes.get_ids()
        print('  cmloptions',cmloptions)
        
        # route for all modes and read in routes 
        for id_mode in  ids_mode:
            # write flow and turns xml file for this mode
            time_start, time_end = self.export_flows_and_turns(flowfilepath, turnfilepath, id_mode)
            print('  time_start, time_end =',time_start, time_end)
            
            if time_end>time_start:# means there exist some flows for this mode
                cmd = 'jtrrouter --route-files=%s --turn-ratio-files=%s --net-file=%s --output-file=%s --begin %s --end %s %s'\
                    %(  P+flowfilepath+P, 
                        P+turnfilepath+P, 
                        P+netfilepath+P,
                        P+routefilepath+P,
                        time_start,
                        time_end,
                        cmloptions,
                        )
                #print '\n Starting command:',cmd
                if call(cmd):
                    if os.path.isfile(routefilepath): 
                        trips.import_routes_xml(routefilepath, is_generate_ids = True)
                        os.remove(routefilepath)
                
            else:
                print('jtrroute: no flows generated for id_mode',id_mode) 
        
        
        #self.simfiles.set_modified_data('rou',True)
        #self.simfiles.set_modified_data('trip',True)
        # trips and routes are not yet saved!!

class TurnflowRouter(db.TripoptionMixin,CmlMixin,Process):
    def __init__(self, turnflows, logger = None,**kwargs):
        
        self._init_common(  'turnflowrouter', name = 'Turnflow Router', 
                            parent = turnflows,
                            logger = logger,
                            info ='Generates routes from turnflow database using the JTR router.',
                            )
         
        self.init_cml('')# pass  no commad to generate options only
        
        
        attrsman = self.get_attrsman()
        
        
                                    
        
        self.add_option('turnratio_defaults', kwargs.get('turnratio_defaults','30,50,20'),
                        groupnames = ['options'],# 
                        cml = '--turn-defaults',
                        name = 'Default turn ratios', 
                        info = 'Default turn definition. Comma separated string means: "percent right, percent straight, percent left".',
                        )
                        
        self.add_option('max-edges-factor', kwargs.get('max-edges-factor',2.0),
                        groupnames = ['options'],# 
                        cml = '--max-edges-factor',
                        name = 'Maximum edge factor', 
                        info = 'Routes are cut off when the route edges to net edges ratio is larger than this factor.',
                        )
                        
        self.add_option('is_internal_links', kwargs.get('is_internal_links',False),
                        groupnames = ['options'],# 
                        cml = '--no-internal-links',
                        name = 'Disable internal links', 
                        info = 'Disable (junction) internal links.',
                        )
                        
        self.add_option('is_randomize_flows',  kwargs.get('is_randomize_flows',True),
                        groupnames = ['options'],# 
                        cml = '--randomize-flows',
                        name = 'Randomize flows', 
                        info = 'generate random departure times for flow input.',
                        )

                        
        self.add_option('is_ignore_vclasses',  kwargs.get('is_ignore_vclasses',False),
                        groupnames = ['options'],# 
                        cml = '--ignore-vclasses',
                        name = 'Ignore mode restrictions', 
                        info = 'Ignore mode restrictions of network edges.',
                        )
                        
        self.add_option('is_remove_loops',  kwargs.get('is_remove_loops',True),
                        groupnames = ['options'],# 
                        cml = '--remove-loops',
                        name = 'Remove loops', 
                        info = 'emove loops within the route; Remove turnarounds at start and end of the route.',
                        ) 
        
        self.add_option('is_remove_loops',  kwargs.get('is_remove_loops',True),
                        groupnames = ['options'],# 
                        cml = '--remove-loops',
                        name = 'Remove loops', 
                        info = 'Remove loops within the route; Remove turnarounds at start and end of the route.',
                        ) 
        # no longer supported
        #self.add_option('is_weights_interpolate', True,
        #                groupnames = ['options'],# 
        #                cml = '--weights.interpolate',
        #                name = 'Interpolate edge weights', 
        #                info = 'Interpolate edge weights at interval boundaries.',
        #                )
        
        self.add_option('weights_minor_penalty',  kwargs.get('weights_minor_penalty',1.5),
                        groupnames = ['options'],# 
                        cml = '--weights.minor-penalty',
                        name = 'Minor link panelty', 
                        info = 'Apply the given time penalty when computing routing costs for minor-link internal lanes.',
                        ) 
                        
        self.add_option('n_routing_threads',  kwargs.get('n_routing_threads',0),
                        groupnames = ['options'],# 
                        cml = '--routing-threads',
                        name = 'Number of parallel threads', 
                        info = 'ATTENTION: Numbers greater than 0 may cause errors!. The number of parallel execution threads used for routing.',
                        )
        self.add_option('seed',  kwargs.get('seed',1),
                        groupnames = ['options'],# 
                        cml = '--seed',
                        name = 'Random seed', 
                        info = 'Initialises the random number generator with the given value.',
                        )
                                        
                        
                                                                        
        self.add_option('is_repair',  kwargs.get('is_repair',True),
                        groupnames = ['options'],# 
                        cml = '--repair',
                        name = 'Repair routes', 
                        info = 'Tries to correct a false route.',
                        ) 
        
        
        
        
        self.add_option('is_ignore_errors',  kwargs.get('is_ignore_errors',True),
                        groupnames = ['options'],# 
                        cml = '--ignore-errors',
                        name = 'Ignore errors', 
                        info = """Continue routing, even if errors occur. 
                        This option is recommended to avoid abortion if no sink zones are.""",
                        ) 
                        
        
        self.is_export_network = attrsman.add(am.AttrConf(  'is_export_network',  kwargs.get('is_export_network',True),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Export network', 
                                    info = 'Export network before routing.',
                                    ))
                                                                                   
        self.is_clear_trips = attrsman.add(am.AttrConf(  'is_clear_trips',  kwargs.get('is_clear_trips',True),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Clear trips', 
                                    info = 'Clear all trips in current trips database before routing.',
                                    ))
                              
                              
        self.add_posoptions()#kwargs!!!
        self.add_laneoptions()
        self.add_speedoptions()
        #self.add_option('turnratiofilepath', turnratiofilepath,
        #                groupnames = ['_private'],# 
        #                cml = '--turn-ratio-files',
        #                perm='r', 
        #                name = 'Net file', 
        #                wildcards = 'Net XML files (*.net.xml)|*.net.xml',
        #                metatype = 'filepath',
        #                info = 'SUMO Net file in XML format.',
        #                )
        
    def do(self):
        print('do')
        cml = self.get_cml(is_without_command = True)# only options, not the command #
        print('  cml=',cml)
        self.parent.turnflows_to_routes(is_clear_trips = self.is_clear_trips, 
                                        is_export_network = self.is_export_network, 
                                        is_make_probabilities = True, 
                                        cmloptions = cml)
        return True
   

class RouteSampler(db.TripoptionMixin,CmlMixin,Process):
    def __init__(self, turnflows, logger = None,**kwargs):
        
        self._init_common(  'routesamper', name = 'Route Sampler', 
                            parent = turnflows,
                            logger = logger,
                            info ="""The Route sampler algorithm generates traffic (routed vehicles) from any combination of turn-flow data (generator flows and turn flows).  It requires a set of routes that will be produced in different ways. Routes are sampled (heuristically) from this set so that the resulting traffic fulfills the turnflow data.
                            """,
                            )
         
        self.init_cml('')# pass  no commad to generate options only
        
        
        attrsman = self.get_attrsman()
        
        
                                    
        
        self.dist_min = attrsman.add(am.AttrConf('dist_min', kwargs.get('dist_min',400.0),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    unit = 'm',
                                    name = 'Min. distance', 
                                    info = 'Minimum trip distance, valid for all generated trips.',
                                    ))
                        
        self.factor_routelength = attrsman.add(am.AttrConf('factor_routelength', kwargs.get('factor_routelength',1.75),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Routelength factor', 
                                    info = 'The route length must be less than the Routelength factor times the line of sight distance between origin and destination edge.',
                                    ))
        
        self.factor_routelength_io = attrsman.add(am.AttrConf('factor_routelength_io', kwargs.get('factor_routelength_io',2.625),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Routelength factor in/out', 
                                    info = """The route length must be less than the Routelength factor times the line of sight distance between origin and destination edge, 
                                    in case origin or destination is an ingoing or outgoing edge.
                                    This factor can be relatively high as it is only to prevent turnaround routes at the same leg. 
                                    """,
                                    ))
        
        self.factor_min_veh = attrsman.add(am.AttrConf('factor_min_veh', kwargs.get('factor_min_veh',0.5),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Vehicle minimization factor', 
                                    info = """Set Vehicle minimization factor from [0, 1[ for reducing the
                        number of vehicles(prefer routes that pass multiple
                        counting locations over routes that pass fewer)""",
                                    ))
                                    
        #self.is_export_network = attrsman.add(am.AttrConf(  'is_export_network', True,
        #                            groupnames = ['options'],
        #                            perm='rw', 
        #                            name = 'Export network', 
        #                            info = 'Export network before routing.',
        #                            ))
        
        self.is_allow_internal_routes = attrsman.add(am.AttrConf(  'is_allow_internal_routes', kwargs.get('is_allow_internal_routes',True),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Allow internal routes', 
                                    info = 'Allow routes to begin or end inside the zone.',
                                    ))
                                                                                                               
        self.optlevel = attrsman.add(am.AttrConf(  'optlevel', kwargs.get('optlevel',0),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Optimizarion level', 
                                    info = 'Optimization method level (full, INT boundary).',
                                    ))
        self.is_clear_trips = attrsman.add(am.AttrConf(  'is_clear_trips', kwargs.get('is_clear_trips',True),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Clear trips', 
                                    info = 'Clear all trips in current trips database before routing.',
                                    ))
                              
                              
        self.add_posoptions()
        self.add_laneoptions()
        self.add_speedoptions()
        
        self._timesmap = {}
        self._fstarmap = {}
        self.demand = self.parent.parent
        self.edges = self.demand.parent.net.edges

    def do(self):
        print('RouteSampler.do')
        
        scenario = self.parent.parent.parent
        
        trips = scenario.demand.trips
        if self.is_clear_trips:
            # clear all current trips = routes
            trips.clear_trips()
        
    
        
        rootfilepath = scenario.get_rootfilepath()
        self.netfilepath = scenario.net.get_filepath()
        self.flowfilepath = rootfilepath+'.flow.xml'
        self.turnfilepath = rootfilepath+'.turn.xml'
        self.sampleroutefilepath = rootfilepath+'.rousample.xml'
        self.routefilepath = rootfilepath+'.roucalib.xml'
        #'  --total-count 10000'+' --geh-ok %d'%5
        options_aux = ' --optimize %s'%self.optlevel+' --minimize-vehicles %.1f'%self.factor_min_veh+' --threads 1'
        
        routesamplercommand = 'python3 '+ P + os.environ['SUMO_HOME'] + os.path.sep+'tools'+os.path.sep + 'routeSampler.py' + P
        self.cmd_turns =    routesamplercommand+""" -r %s --edgedata-files %s --turn-files %s -o %s"""%\
                                        (   P+self.sampleroutefilepath+P,
                                            P+self.flowfilepath+P, 
                                            P+self.turnfilepath+P, 
                                            P+self.routefilepath+P,
                                        )+options_aux
        self.cmd_noturns = routesamplercommand + """ -r %s --edgedata-files %s -o %s"""%\
                                        (   P+self.sampleroutefilepath+P,
                                            P+self.flowfilepath+P, 
                                            P+self.routefilepath+P,
                                        )+options_aux
        
        
        # first generate xml for net -> NOT NEEDEd
        #if self.is_export_network:
        #    scenario.net.export_netxml()
        
        # create routes between counting points and calibrate with counts
        return self.route_between_counters()
        
        # TODO:
        # other route creation methods...
        
        # optimization base on current routes (probably not possible at ones for different modes)
        self.cmloption_str = '' #'-v --max-edges-factor 1  --seed 23423 --repair --ignore-vclasses false --ignore-errors --turn-defaults 5,90,5'
        
    def route_randomly(self):
        print('Method: route_randomly')
        netfilepath = self.parent.get_net().get_filepath()
        #python tools/randomTrips.py -n <input-net-file> -r sampleRoutes.rou.xml
        options_aux = ""
        randomtripcommand = 'python3 '+ P + os.environ['SUMO_HOME'] + os.path.sep+'tools'+os.path.sep + 'randomTrips.py' + P
        cmd =  randomtripcommand + """ -n %s -r %s"""%\
                                        (   P+netfilepath+P,
                                            P+self.sampleroutefilepath+P,
                                        )+options_aux
        
        call(cmd)
        
        self.sample_routes(flowtable,turntable, time_start, time_end)
    
    def sample_routes(self,flowtable, turntable, time_start, time_end):
        # export sample routes
        #_trips.print_attrs()
        #print '  write to sampleroutefilepath',self.sampleroutefilepath
        #self.demand.export_routes_xml(self.sampleroutefilepath, demandobjects = [_trips])
        
        print('  write to flowfilepath',self.flowfilepath)
        self.export_flows_xml(flowtable, time_start, time_end)
        self.export_turns_xml(turntable, time_start, time_end)
        
        if len(turntable)>0:
            cmd = self.cmd_turns
        else:
            cmd = self.cmd_noturns
                                
        
        #print '\n Starting command:',cmd
        if call(cmd):
            if os.path.isfile(self.routefilepath): 
                #self.demand.trips.import_routes_xml(self.routefilepath, is_generate_ids = True)
                routes, ids_edge_depart, ids_edge_arrival = self.read_routes(self.routefilepath)
                
                n_routes = len(routes)
                times_depart = np.random.uniform(low=time_start, high=time_end, size=n_routes)
                ids_routes,ids_trip = self.demand.trips.make_routes(\
                                        ids_vtype[0]*np.ones(n_routes, dtype = np.int32), # TODO currently only one type of the mode, 
                                        is_generate_ids = True, ids_trip=None, is_add = False, 
                                        routes = routes,
                                        times_depart = times_depart*np.ones(n_routes, dtype = np.int32),
                                        ids_edge_depart = ids_edge_depart*np.ones(n_routes, dtype = np.int32),
                                        ids_edge_arrival = ids_edge_arrival*np.ones(n_routes, dtype = np.int32),
                                        inds_lane_depart = self.ind_lane_depart*np.ones(n_routes, dtype = np.int32),
                                        positions_depart = self.pos_depart*np.ones(n_routes, dtype = np.int32),
                                        speeds_depart = self.speed_depart*np.ones(n_routes, dtype = np.int32),
                                        inds_lane_arrival = self.ind_lane_arrival*np.ones(n_routes, dtype = np.int32),
                                        positions_arrival = self.pos_arrival*np.ones(n_routes, dtype = np.int32),
                                        speeds_arrival = self.speed_arrival*np.ones(n_routes, dtype = np.int32),
                                        )
                os.remove(self.routefilepath)
                
                
            else:
                print('routeSampler: no routes generated for id_mode',id_mode) 
                return False
                
                
    def read_routes(self, filepath):
        reader = RouteReaderRoutesampler(self.edges)
        try:
            parse(filepath, reader)
            return reader.get_routes()
            
        except KeyError:
            print("Error: Problems with reading routes!", file=sys.stderr)
            raise    
        
        
    def route_between_counters(self):
        print('Method: route_between_counters')
        turnflows = self.parent
        
        if self.is_allow_internal_routes:
            flowtypes_in = ('t','i')
            flowtypes_out = ('t','o')
        else:
            flowtypes_in = ('i',)
            flowtypes_out = ('o',)
        
        ids_inter = turnflows.get_ids()
        for id_inter,time_start, time_end, turnflowmode  in zip(\
                    ids_inter,
                    turnflows.times_start[ids_inter], 
                    turnflows.times_end[ids_inter], 
                    turnflows.turnflowmodes[ids_inter]
                    ):
            
            ids_tfm = turnflowmode.get_ids()
            for id_mode, flowtable, turntable in zip(\
                        turnflowmode.ids_mode[ids_tfm], 
                        turnflowmode.flowtables[ids_tfm], 
                        turnflowmode.turntables[ids_tfm]):
                print('\n  id_mode',id_mode,'time_start',time_start,'time_end',time_end)
                
                # get vtypes for specified mode
                ids_vtype, shares = self.demand.vtypes.select_by_mode(\
                            id_mode, is_sumoid = False, is_share = True)
                            
                _trips = db.TripsBase(self.demand,)
                ids_flows = flowtable.get_ids()
                ids_edge_flows = flowtable.ids_edge[ids_flows]
                
                ids_turns = turntable.get_ids()
                ids_edge_from = turntable.ids_fromedge[ids_turns]
                ids_edge_to = turntable.ids_fromedge[ids_turns]
                
                ids_edge_blacklist_in = ids_edge_flows[flowtable.flowtypes[ids_flows]=='i']
                ids_edge_blacklist_out = ids_edge_flows[flowtable.flowtypes[ids_flows]=='o']
                #print '  ids_edge_blacklist_in',ids_edge_blacklist_in
                #print '  ids_edge_blacklist_out',ids_edge_blacklist_out
                _edgecouples_routed = []
                
                print ('  make routes from each edge with detectors to each other')
                # but only ithe from edge has no tornflows
                #print '  ids_edge flows',flowtable.ids_edge[ids_flows]
                #print '  flowtypes',flowtable.flowtypes[ids_flows]
                for id_edge_from, flowtype_from in zip(flowtable.ids_edge[ids_flows],flowtable.flowtypes[ids_flows]):
                    print('  *id_edge_from',id_edge_from,flowtype_from,((id_edge_from not in ids_edge_from)|(flowtype_from =='i')),(flowtype_from in ['t','i']))
                    # go ahead only if no turnflow definition for this edge
                    if ((id_edge_from not in ids_edge_from)|(flowtype_from =='i')) & (flowtype_from in ['t','i']):
                        # route to other edges with flow measurements
                        for id_edge_to, flowtype_to in zip(flowtable.ids_edge[ids_flows],flowtable.flowtypes[ids_flows]):
                            self._route_between_counter_pair(id_edge_from, id_edge_to, flowtype_from,  flowtype_to, id_mode, _trips, _edgecouples_routed, ids_edge_blacklist_in,ids_edge_blacklist_out,flowtypes_in, flowtypes_out)
                    
                        # route to from-edges of tornflow measurements
                        for id_edge_to, flowtype_to in zip(turntable.ids_fromedge[ids_turns],turntable.flowtypes[ids_turns]):
                            self._route_between_counter_pair(id_edge_from, id_edge_to, flowtype_from,  flowtype_to, id_mode, _trips, _edgecouples_routed, ids_edge_blacklist_in,ids_edge_blacklist_out,flowtypes_in, flowtypes_out)
                        
                
                
                print ('  make routes from each to-edge to each from edge or detector edge')
                # set(ids_edge_from.tolist()+ids_edge_flows.tolist())
                for id_edge_from, flowtype_from  in zip(turntable.ids_toedge[ids_turns],turntable.flowtypes[ids_turns]):
                    if  (flowtype_from in ['t','i']):
                        # route from to-edges of turnflows to from-edges of turnflows 
                        for id_edge_to, flowtype_to in zip(turntable.ids_fromedge[ids_turns], turntable.flowtypes[ids_turns]):
                            self._route_between_counter_pair(id_edge_from, id_edge_to, flowtype_from,  flowtype_to, id_mode, _trips, _edgecouples_routed, ids_edge_blacklist_in,ids_edge_blacklist_out,flowtypes_in, flowtypes_out)
            
                        # route from to-edges of turnflows  to edges with flow measurements
                        for id_edge_to, flowtype_to in zip(flowtable.ids_edge[ids_flows],flowtable.flowtypes[ids_flows]):
                            if (id_edge_to not in ids_edge_from): # but not edges involved in turnflows
                                self._route_between_counter_pair(id_edge_from, id_edge_to, flowtype_from,  flowtype_to, id_mode, _trips, _edgecouples_routed, ids_edge_blacklist_in,ids_edge_blacklist_out,flowtypes_in, flowtypes_out)
                
                # export sample routes
                #_trips.print_attrs()
                #print '  write to sampleroutefilepath',self.sampleroutefilepath
                self.demand.export_routes_xml(self.sampleroutefilepath, demandobjects = [_trips])
                
                #print '  write to flowfilepath',self.flowfilepath
                self.export_flows_xml(flowtable, time_start, time_end)
                self.export_turns_xml(turntable, time_start, time_end)
                
                if len(turntable)>0:
                    cmd = self.cmd_turns
                else:
                    cmd = self.cmd_noturns
                                        
                
                #print '\n Starting command:',cmd
                if call(cmd):
                    if os.path.isfile(self.routefilepath): 
                        #self.demand.trips.import_routes_xml(self.routefilepath, is_generate_ids = True)
                        routes, ids_edge_depart, ids_edge_arrival = self.read_routes(self.routefilepath)
                        
                        n_routes = len(routes)
                        times_depart = np.random.uniform(low=time_start, high=time_end, size=n_routes)
                        ids_routes,ids_trip = self.demand.trips.make_routes(\
                                                ids_vtype[0]*np.ones(n_routes, dtype = np.int32), # TODO currently only one type of the mode, 
                                                is_generate_ids = True, ids_trip=None, is_add = False, 
                                                routes = routes,
                                                times_depart = times_depart*np.ones(n_routes, dtype = np.int32),
                                                ids_edge_depart = ids_edge_depart*np.ones(n_routes, dtype = np.int32),
                                                ids_edge_arrival = ids_edge_arrival*np.ones(n_routes, dtype = np.int32),
                                                inds_lane_depart = self.ind_lane_depart*np.ones(n_routes, dtype = np.int32),
                                                positions_depart = self.pos_depart*np.ones(n_routes, dtype = np.int32),
                                                speeds_depart = self.speed_depart*np.ones(n_routes, dtype = np.int32),
                                                inds_lane_arrival = self.ind_lane_arrival*np.ones(n_routes, dtype = np.int32),
                                                positions_arrival = self.pos_arrival*np.ones(n_routes, dtype = np.int32),
                                                speeds_arrival = self.speed_arrival*np.ones(n_routes, dtype = np.int32),
                                                )
                        #os.remove(self.routefilepath)
                        
                        
                    else:
                        print('routeSampler: no routes generated for id_mode',id_mode) 
                        return False
        
        return True        
                
    
                            
    def _route_between_counter_pair(self, id_edge_from, id_edge_to, flowtype_from,flowtype_to, id_mode,
                                     _trips, _edgecouples_routed, ids_edge_blacklist_in,ids_edge_blacklist_out,
                                     flowtypes_in, flowtypes_out):
        ids_sumoeges = self.edges.ids_sumo
        print('\n_route_between_counter_pair',ids_sumoeges[id_edge_from], ids_sumoeges[id_edge_to],'flowtypes',flowtype_from,flowtype_to)
        print('  ',(id_edge_to != id_edge_from), (flowtype_from in flowtypes_in),(flowtype_to in flowtypes_out),(id_edge_from, id_edge_to) not in _edgecouples_routed)
        
        
        
        if (id_edge_to != id_edge_from) & (flowtype_from in flowtypes_in) & (flowtype_to in flowtypes_out):
            
            
            if (id_edge_from, id_edge_to) not in _edgecouples_routed:
                edges = self.edges
                # check if..
                # not yet existant
                
                # dist too short ?
                coord_to = edges.shapes[id_edge_to][0]
                coord_from = edges.shapes[id_edge_from][1]
                #print '   coord_to',coord_to,'coord_from',coord_from
                dist = np.linalg.norm(edges.shapes[id_edge_to][0]-edges.shapes[id_edge_from][-1])
                
                if (dist > self.dist_min) | ((flowtype_from == 'i' ) & (flowtype_to == 'o')):
                
                    #print('    OK dist',dist,'greater',self.dist_min,'factor',dist/self.dist_min)
                    cost, ids_edge = get_mincostroute_edge2edge(id_edge_from, id_edge_to,
                                                                weights = self.get_weighs(id_mode), 
                                                                fstar = self.get_fstar(id_mode),
                                                                )
                    
                        
                    print('   found route cost',cost,ids_sumoeges[ids_edge])
                    n_edge = len(ids_edge) 
                    if n_edge > 0:   
                        is_pass = True
                        
                        if 1:
                            ids_edge_crit = set(ids_edge).intersection(ids_edge_blacklist_in)
                            print('   overlapping with in edges:',ids_sumoeges[list(ids_edge_crit)])
                            if len(ids_edge_crit)>1:
                                is_pass = False
                            elif len(ids_edge_crit)==1:
                                id_edge_crit = ids_edge_crit.pop()
                                print('  is critical edge equal this in edge',ids_edge[-1] != id_edge_crit)
                                if ids_edge[0] != id_edge_crit:
                                    #is_pass = False
                                    # take the piece of route that follows after the in edge
                                    ind = ids_edge.index(id_edge_crit)
                                    ids_edge=ids_edge[ind:]
                                    is_pass = len(ids_edge) >= 3
                            
                            if is_pass:
                                ids_edge_crit = set(ids_edge).intersection(ids_edge_blacklist_out)
                                print('   overlapping with out edges:',ids_sumoeges[list(ids_edge_crit)])
                                if len(ids_edge_crit)>1:
                                    is_pass = False
                                elif len(ids_edge_crit)==1:
                                    id_edge_crit = ids_edge_crit.pop()
                                    print('  is critical edge equal this out edge',ids_edge[-1] != id_edge_crit)
                                    if ids_edge[-1] != id_edge_crit:
                                        #is_pass = False
                                        # take the piece of route that is before the out edge
                                        ind = ids_edge.index(id_edge_crit)
                                        ids_edge=ids_edge[:ind]
                                        is_pass = len(ids_edge) >= 3
                                    
                         
                        #print '  checked ids_edge',is_pass,ids_sumoeges[ids_edge]
                        if is_pass:
                            # cut of endings if not explicitly marked as endings to/from external
                            # this has the net-effect that route does not contribute to the 
                            # counts where it starts or ends, but starts/finishes between counting edges  
                            n_edge = len(ids_edge)   
                            if (n_edge>=2) & (flowtype_from != 'i'):
                                ids_edge.pop(0)
                            
                            if (n_edge>=3) & (flowtype_to != 'o'):
                                ids_edge.pop(-1)
                            
                            id_edge_from_new = ids_edge[0]
                            id_edge_to_new = ids_edge[-1]
                            
                            #coord_to = edges.shapes[id_edge_to_new][0]
                            #coord_from = edges.shapes[id_edge_from_new][1]
                            #print '   coord_to',coord_to,'coord_from',coord_from
                            dist = np.linalg.norm(edges.shapes[id_edge_to_new][0]-edges.shapes[id_edge_from_new][-1])
                            
                            print('    found rout',cost, ids_sumoeges[ids_edge])
                            length_route = np.sum(edges.lengths[ids_edge])
                            print('    factor_routelength',length_route/dist,'<?',self.factor_routelength,'dist',dist,'io',(flowtype_from == 'i' ) & (flowtype_to == 'o'))
                            
                            
                            if (length_route < self.factor_routelength * dist) | ((length_route < self.factor_routelength_io * dist)&(flowtype_from == 'i' ) & (flowtype_to == 'o')):
                                # TODO: here we could do routing via close by detectors
                                id_trip = _trips.make_trip(   
                                                    id_vtype = self.demand.vtypes.select_by_mode(id_mode)[0],
                                                    id_edge_depart = ids_edge[0],
                                                    id_edge_arrival = ids_edge[-1],
                                                    route =ids_edge
                                                    )
                                print('  >>> made id_trip',id_trip,'vtypes',self.demand.vtypes.select_by_mode(id_mode))
                                _edgecouples_routed.append(((id_edge_from, id_edge_to)))
                            else:
                                print('  failed: too much deviation')
                        else:
                            print('  failed: crossig in or out edges')
                    else:
                        print('   failed:zero edges, routing failed ids_edge=',ids_edge)
                else:
                    print ('    failed: on absolute minimum distance of route')
            else:
                print ('    route already existant')
        else:            
            print ('    failed: identical from to edge or flow type incorrect')
                    
    def export_flows_xml(self, flowtable, time_begin, time_end):
        print('export_flows_xml',flowtable)
        ids_sumoeges = self.edges.ids_sumo
        indent = 0
        fd = open(self.flowfilepath, 'w', encoding="utf-8")
        fd.write(xm.begin('data',indent))
        #turnflowmode.export_turns_xml(fd, id_mode, indent+4)
        
         #<interval id="arbitrary" begin="0.0" end="300">        
        id_int = 0
        fd.write(xm.start('interval', indent+2))
        fd.write(xm.num('id',id_int))
        fd.write(xm.num('begin',time_begin))
        fd.write(xm.num('end',time_end))
        fd.write(xm.stop())
        
        
        ids_flow = flowtable.get_ids()
        for id_edge_sumo, flow in zip(ids_sumoeges[flowtable.ids_edge[ids_flow]], flowtable.flows[ids_flow]):
            #print '    write id_edge_sumo,flow',id_edge_sumo,flow
            
            if flow> -1: # negative flows means no flow measurements availabe
                # <edge id="-58" entered="4"/>
                fd.write(xm.start('edge',indent+4))
                fd.write(xm.num('id',id_edge_sumo))
                fd.write(xm.num('entered',int(flow)) ) 
                fd.write(xm.stopit())
       
        
        fd.write(xm.end('interval',indent))
        fd.write(xm.end('data',indent))
        fd.close()
        
    def export_turns_xml(self, turntable, time_begin, time_end):
        print('export_turns_xml',turntable)
        ids_sumoeges = self.edges.ids_sumo
        indent = 0
        fd = open(self.turnfilepath, 'w', encoding="utf-8")
        fd.write(xm.begin('data',indent))
        #turnflowmode.export_turns_xml(fd, id_mode, indent+4)
        
         #<interval id="arbitrary" begin="0.0" end="300">        
        id_int = 0
        fd.write(xm.start('interval', indent+2))
        fd.write(xm.num('id',id_int))
        fd.write(xm.num('begin',time_begin))
        fd.write(xm.num('end',time_end))
        fd.write(xm.stop())
        
        
        ids_turn = turntable.get_ids()
        for id_fromedge_sumo, id_toedge_sumo, flow in zip(ids_sumoeges[turntable.ids_fromedge[ids_turn]], ids_sumoeges[turntable.ids_toedge[ids_turn]], turntable.flows[ids_turn]):
            #print '    write id_fromedge_sumo, id_toedge_sumo, flow',id_fromedge_sumo, id_toedge_sumo, flow
            if flow> -1: # negative flows means no flow measurements availabe
                # <edgeRelation from="-58.121.42" to="64" count="1"/>
                fd.write(xm.start('edgeRelation',indent+4))
                fd.write(xm.num('from',id_fromedge_sumo))
                fd.write(xm.num('to',id_toedge_sumo) )
                fd.write(xm.num('count',int(flow)) )
                fd.write(xm.stopit())
       
        
        fd.write(xm.end('interval',indent))
        fd.write(xm.end('data',indent))
        fd.close()
        
    def get_weighs(self, id_mode, is_check_lanes = True):
            """
            Returns edge weighs for id_mode 
            """
            #print 'get_weighs_and_fstar'
            
            
            if id_mode not in self._timesmap:
                edges = self.edges
                self._timesmap[id_mode] = edges.get_times(  id_mode = id_mode,
                                                                is_check_lanes = is_check_lanes,
                                                                )   
       
            
            return self._timesmap[id_mode]
        

        
    def get_fstar(self, id_mode,  is_return_arrays = True, is_ignor_connections = False):
            """
            Returns edge  fstar for id_mode 
            """
            #print 'get_fstar'
            
            
            if id_mode not in self._fstarmap:   
                edges = self.edges   
                self._fstarmap[id_mode] = edges.get_fstar(  id_mode = id_mode,
                                                                is_ignor_connections = is_ignor_connections,
                                                                is_return_arrays = is_return_arrays,
                                                            )
            
            return self._fstarmap[id_mode]

class TurnflowImporter(Process):
    def __init__(self, turnflows, rootname = None, rootdirpath = None, tffilepath = None, 
                    logger = None,**kwargs):
        
        self._init_common(  'turnflowimporter', name = 'Turnflow Importer', 
                            parent = turnflows,
                            logger = logger,
                            info ='Reads and imports turnflow data from different file formates.',
                            )
        
        self._edges = turnflows.get_edges()
        self._net = self._edges.get_parent()
        
        
        if rootname  is None:
            rootname = self._net.parent.get_rootfilename()
            
        
        if rootdirpath  is None:
            if self._net.parent  is not None:
                rootdirpath = self._net.parent.get_workdirpath()
            else:
                rootdirpath = os.getcwd()
        
        if tffilepath  is None:
            tffilepath =os.path.join(rootdirpath,rootname+'.net.xml')
            
                            
        attrsman = self.get_attrsman()
        
        self.t_start = attrsman.add(am.AttrConf(  't_start', kwargs.get('t_start',0),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Start time', 
                                    unit = 's',
                                    info = 'Start time of interval',
                                    ))
            
        self.t_end = attrsman.add(am.AttrConf('t_end', kwargs.get('t_end',3600),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'End time', 
                                    unit = 's',
                                    info = 'End time of interval',
                                    ))
        
        # here we get currently available vehicle classes not vehicle type
        # specific vehicle type within a class will be generated later 
        self.id_mode = attrsman.add(am.AttrConf('id_mode', kwargs.get('id_mode',MODES['passenger']), 
                                            groupnames = ['options'],
                                            choices = turnflows.parent.vtypes.get_modechoices(),
                                            name = 'ID mode', 
                                            info = 'ID of transport mode.',
                                            ))
                                           
                                           
        self.tffilepath = attrsman.add(am.AttrConf('tffilepath',tffilepath,
                                                    groupnames = ['options'],# this will make it show up in the dialog
                                                    perm='rw', 
                                                    name = 'Turnflow file', 
                                                    wildcards = "Turnflows CSV files (*.csv)|*.csv|CSV files (*.txt)|*.txt|All files (*.*)|*.*",
                                                    metatype = 'filepath',
                                                    info = 'CSV file with turnflow information for the specific mode and time interval.',
                                                    ))
        
        
        self.scale = attrsman.add(am.AttrConf('scale', kwargs.get('scale',1.0), 
                                            groupnames = ['options'],
                                            name = 'Scale', 
                                            info = 'Scale factor with which the counts are multiplied.',
                                            )) 
    
    def update_params(self):
        """
        Make all parameters consistent.
        example: used by import OSM to calculate/update number of tiles
        from process dialog
        """
        pass
        #self.workdirpath = os.path.dirname(self.netfilepath)
        #bn =  os.path.basename(self.netfilepath).split('.')
        #if len(bn)>0:
        #    self.rootname = bn[0]
    
    def do(self):
        #self.update_params()
        ids_sumoedge_notexist=[None]
        pairs_sumoedge_unconnected = [None]
        if os.path.isfile(self.tffilepath):
            ids_sumoedge_notexist,pairs_sumoedge_unconnected = self.import_pat_csv()
        return (len(ids_sumoedge_notexist)==0) & (len(pairs_sumoedge_unconnected) == 0)
    
    
            
    def _get_flow_type(self, flowstring_raw):
        flowstring = flowstring_raw.strip()
        
        n = len(flowstring)
        #print '_get_flow_type flowstring*%s* n=%d'%(flowstring,n)
        if n>0:
            #print '  type',flowstring[-1],flowstring[-1] in FLOWTYPES.values()
            if flowstring[-1] in list(FLOWTYPES.values()):
                if n > 1:
                    return int(0.5+self.scale*eval(flowstring[:-1])),flowstring[-1]
                else:
                    # no flow def
                    return -1,flowstring[-1]
                
            else:
                # no flowtype def
                return int(0.5+self.scale*eval(flowstring)),'t'
        else:
            # no flow def, no flowtype def
            return -1, 't'
     
    
    def import_pat_csv(self, sep = ","):
        flowtypekeys = list(FLOWTYPES.keys())
        f=open(self.tffilepath,'r', encoding="utf-8")
        #self.attrs.print_attrs()
        turnflows = self.parent
        edges = turnflows.get_edges()
        ids_edge_sumo = edges.ids_sumo
        
        ids_sumoedge_notexist = []
        pairs_sumoedge_unconnected = []
        
        print('import_pat_csv',self.tffilepath)
        i_line = 1
        for line in f.readlines():
            cols = line.split(sep)
            print('    scanning line',i_line,'cols=',cols)
            if len(cols)>=2:
                id_fromedge_sumo = cols[0].strip()
                if not ids_edge_sumo.has_index(id_fromedge_sumo):
                    ids_sumoedge_notexist.append(id_fromedge_sumo)
                else:
                    id_fromedge = ids_edge_sumo.get_id_from_index(id_fromedge_sumo)
                    #print '    cols[1]',cols[1]
                    flow, flowtype =self._get_flow_type(cols[1])
                    
                    #print '   id_fromedge,flow',id_fromedge,flow
                    turnflows.add_flow(self.t_start, self.t_end, self.id_mode, id_fromedge, flow, flowtype)
                    print('    import flow id_edge=%d,flow=*%d* flowtype=*%s*'%(id_fromedge, flow, flowtype))
                    
                    if len(cols)>=4:
                        for i in range(2,len(cols),2):
                            id_toedge_sumo = cols[i].strip()
                            if not ids_edge_sumo.has_index(id_toedge_sumo):
                                ids_sumoedge_notexist.append(id_toedge_sumo)
                                print('WARNING: inexistent',id_toedge_sumo,'on line',i_line)
                            else:
                                id_toedge = ids_edge_sumo.get_id_from_index(id_toedge_sumo)
                                if not (id_toedge in edges.get_outgoing(id_fromedge)):
                                    pairs_sumoedge_unconnected.append((id_fromedge_sumo,id_toedge_sumo))
                                    print('WARNING: unconnected',id_fromedge_sumo,id_toedge_sumo,'on line',i_line)
                                else:
                                    if i+1<len(cols):
                                        #print '    cols[i+1]',cols[i+1]
                                        flow, flowtype =self._get_flow_type(cols[i+1])
                                        turnflows.add_turn(self.t_start, self.t_end, self.id_mode,id_fromedge, id_toedge, flow, flowtype)
                                        print('    import turn id_fromedge=%d,id_toedge=%d,flow=*%d* flowtype=*%s*'%(id_fromedge,id_toedge, flow, flowtype))
                                    else:
                                        print('WARNING: inconsistent row in line %d, file %s'%(i_line,self.tffilepath))  
            
                
            else:
                print('  Skipping line %d'%(i_line))
            i_line +=1
        f.close()
        
        if len(ids_sumoedge_notexist)>0:
            print('WARNING: inexistant edge IDs:',ids_sumoedge_notexist)
        if len(pairs_sumoedge_unconnected)>0:
            print('WARNING: unconnected edge pairs:',pairs_sumoedge_unconnected)    
        
        return ids_sumoedge_notexist,pairs_sumoedge_unconnected

class RouteReaderRoutesampler(handler.ContentHandler):
    """Reads routes into a list"""

    def __init__(self, edges):
        #print 'RouteReaderRoutesampler.__init__'
        self.id_edge_sumo_to_id = edges.ids_sumo.get_ids_from_indices
        self._routes = []
        self._ids_edge_depart = []
        self._ids_edge_arrival = []

    def startElement(self, name, attrs):
        if name == 'route':
            self._routes.append(self.id_edge_sumo_to_id(attrs['edges'].split(' ')))
            if len(self._routes[-1]) > 0:
                self._ids_edge_depart.append(self._routes[-1][0])
                self._ids_edge_arrival.append(self._routes[-1][-1])
            else:
                print('WARNING in RouteReaderRoutesampler: empty route')
                self._ids_edge_depart.append(-1)
                self._ids_edge_arrival.append(-1)
                
    def get_routes(self):
        return self._routes, self._ids_edge_depart,self._ids_edge_arrival
        
