 
import os, sys, wx
if  __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    #AGILEDIR = os.path.join(APPDIR,'..','..','agilepy')
    
    #sys.path.append(AGILEDIR)
    #sys.path.append(os.path.join(AGILEDIR,"lib_base"))
    #sys.path.append(os.path.join(AGILEDIR,"lib_wx"))
    
    SUMOPYDIR = os.path.join(APPDIR,'..','..')
    sys.path.append(os.path.join(SUMOPYDIR))
            
import numpy as np  

from agilepy.lib_wx.ogleditor import *
from agilepy.lib_wx.mainframe import AgileMainframe   


class NetSelectTool(SelectTool):
    """
    Selection tool for OGL canvas.
    """ 
    
    def __init__(self,parent, mainframe=None):
        """
        To be overridden by specific tool.
        """
        self.init_common(   'select',parent,'Net Selection', 
                            info = 'Select objects in networks',
                            is_textbutton = False,
                            )
                            

        self._init_select(is_show_selected = True) 
              

    def on_left_down_trans(self, event):
        
        #vetrex = np.array([p[0], p[1], 0.0, 1.0],float)
        #p_screen = self._canvas.project(vetrex)
        #print 'SelectTool.on_left_down (px,py)=',self._canvas.unproject_event(event)
        #print '  (x,y)=',event.GetPosition(),p_screen
        is_draw = False 
        
        #if drawing:
        if len(self)>0:
            if event.ShiftDown():
                self.unhighlight_current()
                self._idcounter +=1
                if self._idcounter == len(self):
                    self._idcounter = 0
                self.highlight_current()
                
                self.parent.refresh_optionspanel(self)
                is_draw = True
            else:
                is_draw = self.unselect_all()
                self._idcounter = 0
                if is_draw:
                    self.parent.refresh_optionspanel(self)
        else:
            is_draw = self.pick_all(event)
            self.highlight_current()
            self.parent.refresh_optionspanel(self)
        

        return is_draw
 
    
    def set_objbrowser(self):
        mainframe = self.parent.get_mainframe()
        if mainframe != None:
            drawobj, _id = self.get_current_selection()
            if drawobj != None:
                obj = drawobj.get_netelement()
                mainframe.browse_obj(obj, id = _id)
                
    
    #def get_optionspanel(self, parent):
    #    """
    #    Return tool option widgets on given parent
    #    """
    #    drawobj, _id = self.get_current_selection()
    #    if drawobj != None:
    #        obj = drawobj.get_netelement()
    #        
    #    else:
    #        obj=cm.BaseObjman('empty')
    #        _id = None
    #        
    #    #print 'get_optionspanel',drawobj, _id
    #    self._optionspanel = NaviPanel(parent, obj =  obj, 
    #                attrconfigs=None, id = _id,
    #                #tables = None,
    #                #table = None, id=None, ids=None,
    #                #groupnames = ['options'],
    #                mainframe=self.parent.get_mainframe(), 
    #                immediate_apply=False, panelstyle='default',#'instrumental'
    #                standartbuttons=['apply','restore'])
    #                
    #    return self._optionspanel 

class NetDeleteTool(DeleteTool):
    """
    Delete tool for OGL canvas.
    """ 
    
    def __init__(self,parent, mainframe=None):
        """
        To be overridden by specific tool.
        """
        self.init_common(   'delete',parent,'Delete tool', 
                            info = 'Select and delete objects in network',
                            is_textbutton = False,
                            )
        self._init_select(is_show_selected = True)                    
        
              

    
    def on_execute_selection(self,event):
        """
        Definively execute operation on currently selected drawobjects.
        """
        drawobj, _id =  self.get_current_selection()
        if drawobj!= None:
            drawobj.del_elem(_id)
            self.unselect_all()
            is_draw = True
        else:
            is_draw = False
        return is_draw
        
    
    
class NeteditorTools(ToolsPanel):
    """
    Shows a toolpallet with different tools and an options panel.
    Here tools are added which 
    """
    def __init__(self, parent):
        ToolsPanel.__init__(self, parent, n_buttoncolumns = 3)
        # add and set initial tool
        
        self.add_initial_tool(NetSelectTool(self))
        self.add_tool(StretchTool(self)) 
        self.add_tool(MoveTool(self))
        self.add_tool(ConfigureTool(self))  
        self.add_tool(NetDeleteTool(self))
           
        # more tools can be added later...
         
class  Neteditor(OGleditor):
    def __init__(   self,
                    parent, 
                    mainframe = None,
                    size = wx.DefaultSize,
                    is_menu=False, # create menu items
                    Debug = 0,
                    ):
        
        
        self.prefix_anim = 'anim_' 
        self.layer_anim = 1000.0
        
        wx.Panel.__init__(self, parent, wx.ID_ANY,size = size)
        sizer=wx.BoxSizer(wx.HORIZONTAL)
        
        
        self._mainframe=mainframe
        
        # initialize GL canvas
        navcanvas = OGLnavcanvas(self, mainframe)
        #self._canvas = OGLcanvas(self)
        self._canvas = navcanvas.get_canvas() 
        
        # compose tool pallet here
        self._toolspanel =  NeteditorTools(self) 
        
        
        
        # compose editor window
        sizer.Add(self._toolspanel,0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)# from NaviPanelTest
        #sizer.Add(self._canvas,1,wx.GROW)# from NaviPanelTest
        sizer.Add(navcanvas,1,wx.GROW)
        
        # finish panel setup
        self.SetSizer(sizer)
        sizer.Fit(self)
    
    
        
    def add_netdrawings(self, net, drawing):
        draws = EdgeDrawings(net.edges, drawing)
        drawing.add_drawobj(draws, layer = 10)
        
        draws = NodeDrawings(net.nodes, drawing)
        drawing.add_drawobj(draws, layer = 20)
        
        draws = LaneDrawings(net.lanes, drawing)
        drawing.add_drawobj(draws, layer = 15)
        
        draws = ConnectionDrawings(net.connections, drawing)
        drawing.add_drawobj(draws, layer = 25)
        
        draws = CrossingDrawings(net.crossings, drawing)
        drawing.add_drawobj(draws, layer = 30)
        #print '   len(drawing.get_drawobjs(is_anim = True))',len(drawing.get_drawobjs(is_anim = True))
        
        
    def set_net(self, net):
        
        drawing = OGLdrawing()
        self.add_netdrawings(net, drawing)
        #print 'set_net',net,id(net),drawing,id(drawing)
        
        
        self.set_drawing(drawing)
        #self._canvas.set_drawing(drawing)
        return drawing# returned for test purposes
        
    
    
class NodeDrawings(Circles):
    def __init__(self, nodes, parent,   **kwargs):
        
        Circles.__init__(self, 'nodedraws',  parent, name = 'Node drawings',
                            is_parentobj = False,
                            is_fill = False, # Fill objects,
                            is_outline = True, # show outlines
                            n_vert =7, # default number of vertex per circle
                            linewidth=2, 
                            **kwargs)
        

        
        
                                        
        self.delete('centers')
        self.delete('radii')
        
        
                                        
        self.add(cm.AttrConf(  'color_node_default', np.array([0.0,0.8,0.8,1.0], np.float32),
                                        groupnames = ['options','nodecolors'],
                                        metatype = 'color',
                                        perm='wr', 
                                        name = 'Default color', 
                                        info = 'Default node color.',
                                        ))
        
                                        
        
                                        
        self.set_nodes(nodes)
        

    def get_netelement(self):
        return self._nodes
    
    def get_centers_array(self):
        return self._nodes.coords.value[self._inds_map]
    
    def get_radii_array(self):
        return self._nodes.radii.value[self._inds_map]
    
    def set_nodes(self, nodes):
        #print 'set_nodes'
        self._nodes = nodes
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._nodes.get_ids()
        n = len(ids) 

        self._inds_map = self._nodes.get_inds(ids)
        
        #print 'color_node_default',self.color_node_default.value
        #print 'colors\n',  np.ones((n,1),np.int32)*self.color_node_default.value    
        self.add_rows(      ids = ids, 
                            colors = np.ones((n,1),np.int32)*self.color_node_default.value,
                            colors_highl = self._get_colors_highl(np.ones((n,1),np.int32)*self.color_node_default.value),
                            #centers = self._nodes.coords[ids],
                            #radii = self._nodes.radii[ids],
                            )
                            
        self.update()
        
    def update(self, is_update = True):
        
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
            
class EdgeDrawings(Polylines):
    def __init__(self, edges, parent,   **kwargs):
        
        # joinstyle
        # FLATHEAD = 0             
        # BEVELHEAD = 1
        Polylines.__init__( self, 'edgedraws',  parent, name = 'Edge drawings',
                            is_lefthalf = False,
                            is_righthalf = True, # reverse for english roads
                            arrowstretch = 1.0,
                            joinstyle = FLATHEAD,#BEVELHEAD,
                            **kwargs)
        
        
        
                                        
        self.delete('vertices')
        self.delete('widths')
        
        self.add(cm.AttrConf(  'color_edge_default', np.array([0.2,0.2,1.0,1.0], np.float32),
                                        groupnames = ['options','edgecolors'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default edge color.',
                                        ))
                                        
        self.edgeclasses = {
            'bikeway':('highway.cycleway',[0.9,0.2,0.2,0.9]),
            'serviceroad':('highway.service',[0.4,0.2,0.8,0.9]),
            }
        
        for edgeclass, cdata in self.edgeclasses.iteritems():
            edgetype, color = cdata
            self.add(cm.AttrConf(  'color_'+edgeclass, np.array(color, np.float32),
                                            groupnames = ['options','edgecolors'],
                                            edgetype = edgetype,
                                            metatype = 'color',
                                            perm='wr', 
                                            name = edgeclass+' color', 
                                            info = 'Color of '+edgeclass+' edge class.',
                                            ))
                                        
        self.set_edges(edges)
        
        #self.add_col(am.ArrayConf( 'vertices',  None,
        #                            dtype=np.object,
        #                            groupnames = ['_private'], 
        #                            perm='rw', 
        #                            name = 'Vertex',
        #                            unit = 'm',
        #                            is_save = True,
        #                            info = 'Vertex coordinate vectors of points. Example with 2 vertex: [[[x11,y11,z11],[x12,y12,z12]],[[x21,y21,z21],[x22,y22,z122]],...]',
        #                            ))
    def get_netelement(self):
        return self._edges
    
    def get_vertices_array(self):
        return self._edges.shapes.value[self._inds_map]#[self.get_ids()]
    
    def get_widths_array(self):
        # double because only the right half is shown
        # add a little bit to the width to make it a little wider than the lanes contained  
        return 2.2*self._edges.widths.value[self._inds_map]#[self.get_ids()]
    
    def set_edges(self, edges):
        
        self._edges = edges
        self._inds_edges = self._edges.get_inds()
        
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._edges.get_ids()
        self._inds_map = self._edges.get_inds(ids)
        n = len(ids) 
        #self.vertices = self._edges.shapes
        #self.widths = self._edges.widths
        
              
        self.add_rows(      ids = ids, 
                            beginstyles = np.ones(n)*FLATHEAD,
                            endstyles = np.ones(n)*TRIANGLEHEAD,
                            )
        self.update()
        
    def update(self, is_update = True):
        """
        Update color, assume that there have not been structural changes of the arrays
        """
        # assumes that edges have been set in set_edges
        #print 'Edgedrawing.update'
        #edgeinds = self._edges.get_inds()
        n = len(self)
        
        #self.widths.value[:] = self._edges.widths_lanes.value[inds_edges]#np.ones(n, np.float)*3.2
        #self.colors.value = np.ones((n,1),np.float32)*np.array([0.9,0.9,0.9,0.9]) 
        #self.colors_highl.value = self._get_colors_highl(self.colors.value)   
        self.colors_fill.value[:] = np.ones((n,1),np.float32)*self.color_edge_default.value
        for edgeclass, cdata in self.edgeclasses.iteritems():
            edgetype, color = cdata
            #print '  ',edgeclass, np.sum(self._edges.types.value==edgetype)
            #print '  color',getattr(self,'color_'+edgeclass).value
            self.colors_fill.value[self._inds_map[self._edges.types.value==edgetype]] = getattr(self,'color_'+edgeclass).value
            
        self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)
        
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        

class LaneDrawings(EdgeDrawings):
    def __init__(self, lanes, parent,   **kwargs):
        
        # joinstyle
        # FLATHEAD = 0             
        # BEVELHEAD = 1
        Polylines.__init__( self, 'lanedraws',  parent, name = 'Lane drawings',
                            is_lefthalf = True,
                            is_righthalf = True, # reverse for english roads
                            arrowstretch = 1.0,
                            joinstyle = FLATHEAD,#BEVELHEAD,
                            **kwargs)
        
        
        
                                        
        self.delete('vertices')
        self.delete('widths')
        
        
                                        
        self.add(cm.AttrConf(  'color_lane_default', np.array([0.2, 0.6, 1.0, 0.7], np.float32),
                                        groupnames = ['options','lanecolors'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default lane color.',
                                        ))
                                        
        typecolors = {
            'bicycle':[0.8,0.4,0.4,0.8],
            'pedestrian':[0.1,0.8,0.5,0.8],
            'delivery':[0.5,0.5,0.8,0.8],
            }
        
        net = lanes.parent
        for typename, color in typecolors.iteritems():
            id_mode = net.get_id_mode(typename)
            self.add(cm.AttrConf(  'color_'+typename, np.array(color, np.float32),
                                            groupnames = ['options','typecolors'],
                                            id_mode = id_mode,
                                            metatype = 'color',
                                            perm='wr', 
                                            name = typename+' color', 
                                            info = 'Color of '+typename+' lane type.',
                                            ))
                                                                            
        self.set_lanes(lanes)
        
        
    def get_netelement(self):
        return self._lanes
    
    def get_vertices_array(self):
        return self._lanes.shapes.value[self._inds_map]#[self.get_ids()]
    
    def get_widths_array(self):
        # return here only 0.9 so that there remains a little gap between the lanes
        return 0.8*self._lanes.widths.value[self._inds_map]#[self.get_ids()]
    
    def set_lanes(self, lanes):
        #print 'set_lanes',len(lanes)
        self._lanes = lanes
        #self._inds_lanes = self._lanes.get_inds()
        
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._lanes.get_ids()
        self._inds_map = self._lanes.get_inds(ids)
        n = len(ids) 
        #self.vertices = self._edges.shapes
        #self.widths = self._edges.widths
        
              
        self.add_rows(      ids = ids, 
                            beginstyles = np.ones(n)*FLATHEAD,
                            endstyles = np.ones(n)*TRIANGLEHEAD,
                            )
        self.update()
        
    def update(self, is_update = True):
        """
        Update color, assume that there have not been structural changes of the arrays
        """
        #print 'Lanedraws.update'
        # assumes that edges have been set in set_edges
        #print 'Edgedrawing.update'
        #edgeinds = self._edges.get_inds()
        n = len(self)
        
        #self.widths.value[:] = self._edges.widths_lanes.value[inds_edges]#np.ones(n, np.float)*3.2
        #self.colors.value = np.ones((n,1),np.float32)*np.array([0.9,0.9,0.9,0.9]) 
        #self.colors_highl.value = self._get_colors_highl(self.colors.value)   
        self.colors_fill.value[:] = np.ones((n,1),np.float32)*self.color_lane_default.value
        
        #lanetype = np.array([21],np.int32)
        #lanetype = [21]
        #inds_ped = self._lanes.modes_allow.value == lanetype
        #for modes_allow in self._lanes.modes_allow.value:
        #     print '   modes_allow = ',modes_allow,type(modes_allow),modes_allow==lanetype
        #print ' inds_ped = ',np.flatnonzero(inds_ped)
        #self.colors_fill.value[self._inds_map[inds_ped]] = np.array([0.9,0.0,0.0,0.8],np.float32)
        for attr in self.get_attrsman().get_group('typecolors'):# better use get group
            #print '  color',attr.attrname,attr.value
            self.colors_fill.value[self._inds_map[self._lanes.ids_mode.value==attr.id_mode]] = attr.value
        
        self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)
        
        
        #self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)
        
        
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()


class ConnectionDrawings(Fancylines):
    def __init__(self, connections, parent,   **kwargs):
        
        # joinstyle
        # FLATHEAD = 0             
        # BEVELHEAD = 1
        
        Fancylines.__init__(self, 'connectiondraws',  parent, name = 'Connection drawings',
            is_fill = True,
            is_outline = False,# currently only fill implemented
            arrowstretch = 2.5,
            is_lefthalf = True,
            is_righthalf = True,
            c_highl=0.3, 
            linewidth=1,
            **kwargs)
        
        
        
        #self.delete('vertices')
        #self.delete('widths')
        
        
                                        
        self.add(cm.AttrConf(  'color_con_default', np.array([0.4, 0.8, 1.0, 0.9], np.float32),
                                        groupnames = ['options','colors'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default connection color.',
                                        ))
                                        
        
                                                                            
        self.set_connections(connections)
        
        
    def get_netelement(self):
        return self._connections
    
    #def get_vertices_array(self):
    #    ids_lane_from = self._connections.ids_fromlane[self._inds_map]
    #    ids_lane_to = self._connections.ids_tolane[self._inds_map]
    #    return self._lanes.shapes.value[self._inds_map]#[self.get_ids()]
    
    #def get_widths_array(self):
    #    # return here only 0.9 so that there remains a little gap between the lanes
    #    return 0.8*self._lanes.widths.value[self._inds_map]#[self.get_ids()]
    
    def set_connections(self, connections):
        
        self._connections = connections
        self._lanes = connections.parent.lanes
        #self._inds_lanes = self._lanes.get_inds()
        
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._connections.get_ids()
        self._inds_map = self._connections.get_inds(ids)
        n = len(ids) 
        #self.vertices = self._edges.shapes
        #self.widths = self._edges.widths
        
              
        self.add_rows(      ids = ids, 
                            beginstyles = np.ones(n)*FLATHEAD,
                            endstyles = np.ones(n)*TRIANGLEHEAD,
                            colors_fill = np.ones((n,1),np.float32)*self.color_con_default.value,#get()
                            colors_fill_highl = self._get_colors_highl(np.ones((n,1),np.float32)*self.color_con_default.value)
                            )
        ids_lane_from = self._connections.ids_fromlane[self._inds_map]
        ids_lane_to = self._connections.ids_tolane[self._inds_map]
        
        shapes_lane_from = self._lanes.shapes[ids_lane_from]
        shapes_lane_to = self._lanes.shapes[ids_lane_to]
        i=0
        for shape_lane_from, shape_lane_to in zip(shapes_lane_from,shapes_lane_to):
            self.vertices.value[i] = [shape_lane_from[-1],shape_lane_to[0]]
            i +=1
        self.update()
        
    def update(self, is_update = True):
        """
        Update color, assume that there have not been structural changes of the arrays
        """
        #print 'Lanedraws.update'
        # assumes that edges have been set in set_edges
        #print 'Edgedrawing.update'
        #edgeinds = self._edges.get_inds()
        n = len(self)
        
        #self.widths.value[:] = self._edges.widths_lanes.value[inds_edges]#np.ones(n, np.float)*3.2
        #self.colors.value = np.ones((n,1),np.float32)*np.array([0.9,0.9,0.9,0.9]) 
        #self.colors_highl.value = self._get_colors_highl(self.colors.value)   
        #self.colors_fill.value[:] = np.ones((n,1),np.float32)*self.color_lane_default.value
        #self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)
        
        
        
        #return self._lanes.shapes.value[self._inds_map]#[self.get_ids()]
        
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
 
class CrossingDrawings(Fancylines):
    def __init__(self, crossings, parent,   **kwargs):
        
        # joinstyle
        # FLATHEAD = 0             
        # BEVELHEAD = 1
        
        Fancylines.__init__(self, 'crossingdraws',  parent, name = 'Crossing drawings',
            is_fill = True,
            is_outline = False,# currently only fill implemented
            arrowstretch = 2.5,
            is_lefthalf = True,
            is_righthalf = True,
            c_highl=0.3, 
            **kwargs)
        
        
        
        #self.delete('vertices')
        self.delete('widths')
        
        
                                        
        self.add(cm.AttrConf(  'color_cross_default', np.array([0.8, 0.8, 0.9, 0.8], np.float32),
                                        groupnames = ['options','colors'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default crossing color.',
                                        ))
                                        
        
                                                                            
        self.set_netelement(crossings)
        
        
    def get_netelement(self):
        return self._crossings
    
    #def get_vertices_array(self):
    #    ids_lane_from = self._connections.ids_fromlane[self._inds_map]
    #    ids_lane_to = self._connections.ids_tolane[self._inds_map]
    #    return self._lanes.shapes.value[self._inds_map]#[self.get_ids()]
    
    def get_widths_array(self):
        # return here only 0.9 so that there remains a little gap between the lanes
        return self._crossings.widths.value[self._inds_map]#[self.get_ids()]
    
    def set_netelement(self, crossings):
        
        self._crossings = crossings
        #self._lanes = crossings.parent.lanes
        edges = crossings.parent.edges
        #self._inds_lanes = self._lanes.get_inds()
        
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._crossings.get_ids()
        self._inds_map = self._crossings.get_inds(ids)
        n = len(ids) 
        #self.vertices = self._edges.shapes
        
              
        self.add_rows(      ids = ids, 
                            beginstyles = np.ones(n)*FLATHEAD,
                            endstyles = np.ones(n)*FLATHEAD,
                            colors_fill = np.ones((n,1),np.float32)*self.color_cross_default.value,#get()
                            colors_fill_highl = self._get_colors_highl(np.ones((n,1),np.float32)*self.color_cross_default.value),
                            #widths = crossings.widths[ids],
                            )
        
        
        ids_edge = self._crossings.ids_edge.value[self._inds_map]
        widths_edge = edges.widths[ids_edge]
        widths_crossing = self.get_widths_array()
        
        inds_tonode = np.flatnonzero(edges.ids_tonode[ids_edge] == self._crossings.ids_node.value[self._inds_map])
        inds_fromnode = np.flatnonzero(edges.ids_fromnode[ids_edge] == self._crossings.ids_node.value[self._inds_map])
        
        
        vertices = np.zeros((n,2,3),np.float32 )
        
        for ind in inds_tonode:
            vertices[ind,:] = edges.shapes[ids_edge[ind]][-2:]
        
        for ind in inds_fromnode:
            vertices[ind,:] = edges.shapes[ids_edge[ind]][:2]
        
        vertices_cross = np.zeros((len(self),2,3),np.float32 )
        # copy z-coord vertices_cross
        vertices_cross[:,:,2] = vertices[:,:,2]
        
        vertices_delta = vertices[:,1] -  vertices[:,0]  
        
        #angles = np.arctan2(vertices_delta[:,1],vertices_delta[:,0]) + np.pi/2
        #vertices_cross[:,0] = np.array(vertices[:,1])
        #vertices_cross[:,1,0] = vertices[:,1,0] + np.cos(angles )*10
        #vertices_cross[:,1,1] = vertices[:,1,1] + np.sin(angles )*10
            
        angles = np.arctan2(vertices_delta[:,1], vertices_delta[:,0]) - np.pi/2
        #angles = np.arctan2(vertices_delta[inds_tonode,1],vertices_delta[inds_tonode,0]) - np.pi/2
        vertices_cross[inds_tonode,0] = vertices[inds_tonode,1]
        vertices_cross[inds_tonode,1,0] = vertices[inds_tonode,1,0] +  np.cos(angles[inds_tonode] )*widths_edge[inds_tonode]
        vertices_cross[inds_tonode,1,1] = vertices[inds_tonode,1,1]+ np.sin(angles[inds_tonode] )*widths_edge[inds_tonode]
        
        #angles = np.arctan2(vertices_delta[inds_fromnode,1],vertices_delta[inds_fromnode,0]) - np.pi/2
        vertices_cross[inds_fromnode,0] = vertices[inds_fromnode,0]
        vertices_cross[inds_fromnode,1,0] = vertices[inds_fromnode,0,0]+ np.cos(angles[inds_fromnode] )*widths_edge[inds_fromnode]
        vertices_cross[inds_fromnode,1,1] = vertices[inds_fromnode,0,1]+ np.sin(angles[inds_fromnode] )*widths_edge[inds_fromnode]
        

        #vertices_cross[inds_fromnode,0,0] = vertices[inds_fromnode,0,0]+ np.cos(angles[inds_fromnode] +np.pi/2)*widths_crossing[inds_fromnode] 
        #vertices_cross[inds_fromnode,0,1] = vertices[inds_fromnode,0,1]+ np.sin(angles[inds_fromnode] +np.pi/2)*widths_crossing[inds_fromnode]
        #vertices_cross[inds_fromnode,1,0] = vertices[inds_fromnode,0,0]+ np.cos(angles[inds_fromnode] +np.pi/2)*widths_crossing[inds_fromnode] + np.cos(angles[inds_fromnode] )*widths_edge[inds_fromnode]
        #vertices_cross[inds_fromnode,1,1] = vertices[inds_fromnode,0,1]+ np.sin(angles[inds_fromnode] +np.pi/2)*widths_crossing[inds_fromnode]   + np.sin(angles[inds_fromnode] )*widths_edge[inds_fromnode]
        
        
        
        #vertices_delta = vertices[:,1] -  vertices[:,0]  
        #norm  = get_norm_2d(vertices_delta ).reshape(n,1)# np.sqrt( np.sum((vertices_delta)**2,0))
        #print '  norm.shape  vertices_delta.shape=', norm.shape,vertices_delta.shape
        #vertices_perp_norm = np.zeros((n,2), np.float32) 
        #vertices_perp_norm[:,:] = vertices_delta[:,[1,0]]/norm
        #vertices_perp_norm[:,:] = vertices_delta[:,:2]/norm
        #vertices[:,1,:2] =  vertices[:,0,:2] + 20.0*vertices_perp_norm
        
        
        self.vertices.value[:]= vertices_cross   
        self.update()
        
    def update(self, is_update = True):
        """
        Update color, assume that there have not been structural changes of the arrays
        """
        #print 'Lanedraws.update'
        # assumes that edges have been set in set_edges
        #print 'Edgedrawing.update'
        #edgeinds = self._edges.get_inds()
        #n = len(self)
        
        #self.widths.value[:] = self._edges.widths_lanes.value[inds_edges]#np.ones(n, np.float)*3.2
        #self.colors.value = np.ones((n,1),np.float32)*np.array([0.9,0.9,0.9,0.9]) 
        #self.colors_highl.value = self._get_colors_highl(self.colors.value)   
        #self.colors_fill.value[:] = np.ones((n,1),np.float32)*self.color_lane_default.value
        #self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)
        
        
        
        #return self._lanes.shapes.value[self._inds_map]#[self.get_ids()]
        
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
                                               
class NeteditorMainframe(AgileToolbarFrameMixin, wx.Frame):
    """
    Simple wx frame with some special features.
    """
    

    def __init__(self, net, title='Neteditor', pos=wx.DefaultPosition,
                 size=(1000,500), style=wx.DEFAULT_FRAME_STYLE,
                 name='frame'):
                     
        self._net = net
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE
        wx.Frame.__init__(self, None, wx.NewId(), title, pos, size = size, style = style, name = name)
        self.gleditor = Neteditor(self)
        
        self.Show()# must be here , before putting stuff on canvas
        
        self.init_drawing()
        
    
    def init_drawing(self):
        drawing = OGLdrawing()
        
        edgesdrawings = EdgeDrawings(self._net.edges, drawing)
        drawing.add_drawobj(edgesdrawings)
        
        nodesdrawings = NodeDrawings(self._net.nodes, drawing)
        drawing.add_drawobj(nodesdrawings)
        
        
        
        canvas = self.gleditor.get_canvas()
        canvas.set_drawing(drawing)
        wx.CallAfter(canvas.zoom_tofit)
            
              
             

class NeteditorApp(wx.App):
    def __init__(self, net = None, output=False,  **kwargs):
        self._net = net
        #print 'ViewerApp.__init__',self._net
        wx.App.__init__(self, output, **kwargs)
        #wx.App.__init__(self,  **kwargs)
        
    def OnInit(self):
        # wx.InitAllImageHandlers()
        self.mainframe = NeteditorMainframe(self._net)
        #sumopyicon = wx.Icon(os.path.join(IMAGEDIR,'icon_sumopy.png'),wx.BITMAP_TYPE_PNG, 16,16)
        #wx.Frame.SetIcon(self.mainframe,sumopyicon)
        #self.mainframe.SetIcon(sumopyicon)
        

        #if True: #len(sys.argv)>=2:
        #    from  lib.net import readNet
        #    filepath = '/home/joerg/projects/sumopy/bologna/bologna4.net.xml'#sys.argv[1]
        #    _net = readNet(filepath)
        #else:
        #    _net = None

        
        self.SetTopWindow(self.mainframe)
        return True
        
def netediting(net):
    app = NeteditorApp(net, output = False)# put in True if you want output to go to it's own window.
    #print 'call MainLoop'
    app.MainLoop() 
           
if __name__ == '__main__':          
    ###########################################################################
    # MAINLOOP
    import network
    from agilepy.lib_base.logger import Logger
    net = network.Network(logger = Logger())
    net.import_xml('facsp2', 'testnet')
    app = NeteditorApp(net, output = False)
    
    app.MainLoop()
