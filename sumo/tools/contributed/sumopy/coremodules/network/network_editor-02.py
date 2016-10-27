 
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
                            
        self.add_col(am.TabIdsArrayConf(    'drawobjects',
                                            groupnames = ['options'], 
                                            name = 'Object',
                                            info = 'Selected Draw objects with id',
                                            ))
              

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

class NeteditorTools(ToolsPanel):
    """
    Shows a toolpallet with different tools and an options panel.
    Here tools are added which 
    """
    def __init__(self, parent):
        ToolsPanel.__init__(self, parent, n_buttoncolumns = 3)
        # add and set initial tool
        
        self.add_initial_tool(NetSelectTool(self))
        self.add_tool(ConfigureTool(self))     
        # more tools can be added later...
         
class  Neteditor(OGleditor):
    def __init__(   self,
                    parent, 
                    mainframe = None,
                    size = wx.DefaultSize,
                    is_menu=False, # create menu items
                    Debug = 0,
                    ):
        
        
        
        
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
        #self.delete('radii')
        
        self.add(cm.AttrConf(  'radius_default', 5.0, 
                                        groupnames = ['options'],
                                        perm='wr', 
                                        unit = 'm',
                                        name = 'Default radius', 
                                        info = 'Default node radius.',
                                        ))
                                        
        self.add(cm.AttrConf(  'color_node_default', np.array([0.0,0.8,0.8,1.0], np.float32),
                                        groupnames = ['options','nodecolors'],
                                        metatype = 'color',
                                        perm='wr', 
                                        name = 'Default color', 
                                        info = 'Default node color.',
                                        ))
        
                                        
        
                                        
        self.set_nodes(nodes)
        
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
        return self._nodes
    
    def get_centers_array(self):
        return self._nodes.coords[self.get_ids()]
    
    
    def set_nodes(self, nodes):
        self._nodes = nodes
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._nodes.get_ids()
        n = len(ids) 
        #self.centers = self._nodes.coords
        
        #print 'color_node_default',self.color_node_default.value
        #print 'colors\n',  np.ones((n,1),np.int32)*self.color_node_default.value    
        self.add_rows(      ids = ids, 
                            radii = np.ones(n, np.float32)*self.radius_default.value,
                            colors = np.ones((n,1),np.int32)*self.color_node_default.value,
                            colors_highl = self._get_colors_highl(np.ones((n,1),np.int32)*self.color_node_default.value),
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
            'serviceroad':('highway.service',[0.9,0.9,0.3,0.9]),
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
        return self._edges.shapes[self.get_ids()]
    
    def get_widths_array(self):
        return self._edges.widths[self.get_ids()]
    
    def set_edges(self, edges):
        self._edges = edges
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._edges.get_ids()
        n = len(ids) 
        #self.vertices = self._edges.shapes
        #self.widths = self._edges.widths
        
              
        self.add_rows(      ids = ids, 
                            beginstyles = np.ones(n)*FLATHEAD,
                            endstyles = np.ones(n)*TRIANGLEHEAD,
                            )
        self.update()
        
    def update(self, is_update = True):
        # assumes that edges have been set in set_edges
        #print 'Edgedrawing.update'
        edgeinds = self._edges.get_inds()
        n = len(edgeinds)
        
        #self.widths.value[:] = self._edges.widths_lanes.value[inds_edges]#np.ones(n, np.float)*3.2
        #self.colors.value = np.ones((n,1),np.float32)*np.array([0.9,0.9,0.9,0.9]) 
        #self.colors_highl.value = self._get_colors_highl(self.colors.value)   
        self.colors_fill.value[:] = np.ones((n,1),np.float32)*self.color_edge_default.value
        for edgeclass, cdata in self.edgeclasses.iteritems():
            edgetype, color = cdata
            #print '  ',edgeclass, np.sum(self._edges.types.value==edgetype)
            #print '  color',getattr(self,'color_'+edgeclass).value
            self.colors_fill.value[edgeinds[self._edges.types.value==edgetype]] = getattr(self,'color_'+edgeclass).value
            
        self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)
        
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
    net.read_sumonet('facsp2', 'testnet')
    app = NeteditorApp(net, output = False)
    
    app.MainLoop()
