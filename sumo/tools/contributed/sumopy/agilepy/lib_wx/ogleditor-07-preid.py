#!/usr/bin/env python;
"""OpenGL editor"""
__version__="0.1a"
__licence__ = """licensed under the GPL."""
__copyright__ = "(c) 2012-2016 University of Bologna - DICAM"
__author__ = "Joerg Schweizer"

__usage__="""USAGE:
from command line:
python ogleditor.py
python ogleditor.py <scenariobasename> <scenariodir>

use for debugging
python ogleditor.py --debug > debug.txt 2>&1
"""
print __copyright__
###############################################################################
## IMPORTS

#import wxversion
#wxversion.select("2.8")

import wx

from wx import glcanvas
from wx.lib.buttons import GenBitmapTextButton, GenBitmapButton
try:
    from OpenGL.GL import *
    from OpenGL.GLU import *
    from OpenGL.GLUT import *
    from OpenGL.raw.GL.ARB.vertex_array_object import glGenVertexArrays, \
                                                  glBindVertexArray
                                                
    from OpenGL.arrays import vbo
    import numpy as np
    
except ImportError:
    raise ImportError, "Required dependency OpenGL not present"

import sys, os, types
APPDIR = os.path.join(os.path.dirname(__file__),"..")
sys.path.append(os.path.join(APPDIR,"lib_base"))
IMAGEDIR =  os.path.join(os.path.dirname(__file__),"images")

import classman as cm
import arrayman as am

from geometry import *

# wx gui stuff
from wxmisc import *  
from toolbox import *  

# to be deleted
#import  test_glcanvas as testogl



        





class Lines(am.ArrayObjman):
    def __init__(self,ident,  parent, name = 'Lines',
            n_vert =2, linewidth=3, 
            c_highl=0.3, 
            detectwidth = 0.1,#m
            **kwargs):
    
        self.n_vert = n_vert
        self.detectwidth = 0.1 #m
        
        
        self._init_objman(ident,parent=parent, name = name, **kwargs)
        self.add(cm.AttrConf(  'linewidth', linewidth,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Line width', 
                                        info = 'Line width in pixel',
                                        ))
                                        
        
        self.add(cm.AttrConf(  'c_highl', c_highl,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Highl. const.', 
                                        info = 'Highlight constant takes values beteewn 0 and 1 and is the amount of brightness added for highlighting',
                                        ))
                                        
        self.add_col(am.ArrayConf( 'vertices',  np.zeros((n_vert,3), dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    name = 'Vertex',
                                    is_save = True,
                                    info = 'Vertex coordinate vectors of points. Example with 2 vertex: [[[x11,y11,z11],[x12,y12,z12]],[[x21,y21,z21],[x22,y22,z122]],...]',
                                    ))
                                    
        self.add_col(am.ArrayConf( 'colors',  np.zeros(4, dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    metatype = 'color',
                                    name = 'Color',
                                    is_save = True,
                                    info = 'Object Vertex color with format [[r1,g1,b1,a1],[r2,g2,b2,a2],...]',
                                    ))
                                    
        self.add_col(am.ArrayConf( 'are_highlighted',  False,
                                    dtype=np.bool,
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    name = 'highlighted',
                                    is_save = False,
                                    info = 'If true, object is highlighted',
                                    ))
                                    
        self.add_col(am.ArrayConf( 'colors_highl',  np.zeros(4, dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    metatype = 'color',
                                    name = 'Color highl',
                                    is_save = False,
                                    info = 'Added object color when highlighting with format [[r1,g1,b1,a1],[r2,g2,b2,a2],...]',
                                    )) 
                                
        
                                    
    def _get_colors_highl(self, colors):
        return np.clip(colors+self.c_highl.value*np.ones(colors.shape,float),0,1)-colors
    
    def add_drawobjs(self, vertices, colors, colors_highl = None):
        if colors_highl == None:
            colors_highl=self._get_colors_highl(colors)
            
        ids = self.add_rows(len(vertices), 
                            vertices=vertices, 
                            colors=colors,
                            colors_highl = colors_highl,
                            )
        self._update_vertexvbo()
        self._update_colorvbo()
        return ids
        
    def set_vertices(self, ids, vertices, is_update = True):
        self.vertices[ids] = vertices
        if is_update:
            self._update_vertexvbo()
            
    def set_colors(self, ids, colors, colors_highl=None, is_update = True):
        self.colors[ids] = colors
        if colors_highl==None:
            self.colors_highl[ids]=self._get_colors_highl(colors)
        else:
            self.colors_highl[ids]=colors_highl
        if is_update:
            self._update_colorvbo()
                    
    def _update_vertexvbo(self):
        #print '_update_vertexvbo vertices=\n',self.vertices.value
        self._vertexvbo = vbo.VBO(self.vertices.value.reshape((-1,3)))
        self._indexvbo = vbo.VBO(np.arange(self.n_vert*len(self),dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)

    def _update_colorvbo(self):
        n=len(self)
        #print '_update_colorvbo'
        #print '   colors=\n',self.colors.value
        #print '   colors_highl=\n',self.colors_highl.value
        #print '   are_highlighted=\n',self.are_highlighted.value.reshape(n,1)
        #print '    =\n',self.are_highlighted.value.reshape(n,1) * self.colors_highl.value
        
        #self._colorvbo = vbo.VBO( np.resize( np.repeat(np.clip((self.colors+self.c_highl*self.colors_highl) ,0.0,1.0), self.n_vert),(len(self.colors),4)) )
        #self._colorvbo = vbo.VBO(np.clip((self.colors.value+self.colors_highl.value)[np.array(np.arange(0,len(self),1.0/self.n_vert),int)],0.0,1.0))
        self._colorvbo = vbo.VBO((self.colors.value + self.are_highlighted.value.reshape(n,1)*self.colors_highl.value)[np.array(np.arange(0,n,1.0/self.n_vert),int)])
        #self._colorvbo = vbo.VBO(self.colors.value)
        
    def pick(self,p):
        """
        Returns a binary vector which is True values for lines that have been selected 
        by point p.
        
        In particular, an element of this vector is True if the minimum distance 
        between the respective line to point p is less than self.detectwidth
        """
        vertices = self.vertices.value
        x1 = vertices[:,0,0]
        y1 = vertices[:,0,1]
        
        x2 = vertices[:,1,0]
        y2 = vertices[:,1,1]
            
        
        return get_dist_point_to_segs(p,x1,y1,x2,y2, is_ending=True) < self.detectwidth**2

        
    def highlight(self, inds_highl, is_update = True):
        #print 'highlight'
        #print '   inds_highl=\n',inds_highl
        self.are_highlighted.value[inds_highl] = True#inds_highl
        #self.colors_highl.value = np.repeat(self.c_highl*np.array(inds_highl,dtype = np.float32),4).reshape(-1,4)
        if is_update:
            self._update_colorvbo()
      
    def unhighlight(self, inds_highl=None, is_update = True):
        if inds_highl == None:
            inds_highl = np.flatnonzero(self.are_highlighted.value)
        self.are_highlighted.value[inds_highl] = False
        if is_update:
            self._update_colorvbo()
              
    def draw(self):
        #print 'draw n=',len(self)
        glLineWidth(self.linewidth.value)

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_BLEND)

        glEnableClientState(GL_VERTEX_ARRAY)
        glEnableClientState(GL_COLOR_ARRAY)
        
        
        self._colorvbo.bind()
        glColorPointer(4, GL_FLOAT, 0, None)
        
        self._vertexvbo.bind()
        self._indexvbo.bind()
        glVertexPointer(3, GL_FLOAT, 0, None)
        
        glDrawElements(GL_LINES, self.n_vert*len(self), GL_UNSIGNED_INT, None)

        
        glDisableClientState(GL_VERTEX_ARRAY)
        glDisableClientState(GL_COLOR_ARRAY)
        self._vertexvbo.unbind()
        self._indexvbo.unbind()
        self._colorvbo.unbind()
        
class OGLdrawing(cm.TableObjman):
    """
    Class holding an ordered list of all OGL draw objects.
    This class manages also the order in which the draw objects are rendered. 
    
    The basic idea is that an instance of this class can be passed to
    different OGL rendering canvas.
    """ 
    
    def __init__(self, ident = 'drawing', parent=None, name = 'OGL drawing',info = 'List of OGL draw objects'):
        """
        To be overridden by specific tool.
        """
        self._init_objman(ident, parent=parent, name = name, info = info)
        self.add_col(cm.ObjsConf( 'drawobjects', 
                                    groupnames = ['state'], 
                                    is_save = True,
                                    name = 'Draw object', 
                                    info = 'Object, containing data and rendering methods.',
                                    ))
    def get_drawobjs(self):
        return self.drawobjects.value.values()
        
    def add_drawobj(self, drawobj):
        id_drawobj = self.add_rows(1, drawobjects = [drawobj])
        return  id_drawobj  
        
class OGLcanvas(glcanvas.GLCanvas):
    def __init__(self, parent, mainframe = None):
        if mainframe == None:
            self._mainframe = parent
        else:
            self._mainframe = mainframe
        
        self._drawing = None     
        self.eyex = 0.0
        self.eyey = 0.0
        self.eyez = -10.0#-9.0
        
        
        self.centerx = 0.0
        self.centery = 0.0
        self.centerz = 0.0
        
        self.upx = -1.0
        self.upy = 0.0
        self.upz = 0.0
        
        self.g_Width = 600
        self.g_Height = 600

        self.g_nearPlane = 1.
        self.g_farPlane = 1000.

        self.action = ""
        self.pos_start=(0.0,0.0) 
        self.trans_start=(0.0,0.0)
        
        self.resetView(is_draw = False)
        

        
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE

        attribList = (glcanvas.WX_GL_RGBA, # RGBA
                      glcanvas.WX_GL_DOUBLEBUFFER, # Double Buffered
                      glcanvas.WX_GL_DEPTH_SIZE, 24) # 24 bit


        glcanvas.GLCanvas.__init__(self, parent,-1, attribList=attribList)
        #super(WxGLTest2, self).__init__(parent,-1, attribList=attribList)
        
        self.GLinitialized = False
        

        # Set the event handlers.
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.processEraseBackgroundEvent)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_PAINT, self.processPaintEvent)
        
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_MOTION, self.OnMotion)
        self.Bind(wx.EVT_MOUSEWHEEL, self.OnWheel)
        
        
        # this is needed to initialize GL projections for unproject
        wx.CallAfter(self.OnSize)

    
    def set_drawing(self, drawing):
        self._drawing = drawing     
        self.OnDraw()
    
    def get_drawing(self, drawing):
        return self._drawing
          
    def resetView(self, is_draw = True):
        self.zoom = 65.

        self.xRotate = 180.
        self.yRotate = 180.
        self.zRotate = -90.0
        self.trans=(0.0,0.0)
        
        
        if is_draw:
            self.OnDraw()
        
    
      
    def OnWheel(self, event):
        #EventType = FloatCanvas.EVT_FC_MOUSEWHEEL
        # 
        Rot = event.GetWheelRotation()
        #print 'OnWheel!!',Rot,event.ControlDown(),event.ShiftDown()
        if event.ControlDown():#event.ControlDown(): # zoom
            if Rot < 0:
                self.zoom *= 0.9
            else:
                self.zoom *= 1.1
            self.OnDraw()
            event.Skip()
            
    def OnLeftDown(self, event):
        if (event.ControlDown()&event.ShiftDown())&(self.action == ''):
          self.action = 'drag'
          self.BeginGrap(event)
          event.Skip()
          
    def OnLeftUp(self, event):
        if self.action=='drag':
          self.EndGrap(event)
          self.action==''
          event.Skip()
    
    

    def OnMotion(self, event, is_draw=False):

        p = self.unproject(event.GetPosition())[0:2]
        #print 'OnMotion (px,py)=',p,'(x,y)=',event.GetPosition() 
        if self._drawing:
            for drawobj in  self._drawing.get_drawobjs():
                 inds_pick = drawobj.pick(p)
                 drawobj.highlight(inds_pick, is_update = False)
                 drawobj.unhighlight(~inds_pick, is_update = True)
                 if len(inds_pick)>0:
                     is_draw |= True

            
        
        if (event.ControlDown()&event.ShiftDown()&(self.action=='drag')):
          self.MoveGrap(event)
          is_draw |= True
          event.Skip()
          
        elif (self.action=='drag'):
          self.EndGrap(event)
          is_draw |= True
          self.action==''
          event.Skip()
    
        if is_draw:
            self.OnDraw()
    

    def BeginGrap(self,event):
        # http://www.wxpython.org/docs/api/wx.Cursor-class.html
        self.SetCursor(wx.StockCursor(wx.CURSOR_HAND))
        self.pos_start = event.GetPosition() 
        self.trans_start = self.trans
        #print 'BeginGrap',self.trans 
        
    def MoveGrap(self,event):
        
        x, y = self.unproject(event.GetPosition())[0:2]
        x0,y0 = self.unproject(self.pos_start)[0:2]
        
        self.trans = (self.trans_start[0] + (x-x0), self.trans_start[1] + (y-y0))
        #print 'MoveGrap',self.trans, x,y

                
                
    def EndGrap(self, event):
        #print 'EndGrap'
        self.SetCursor(wx.NullCursor)
        self.action=''
            



    #
    # wxPython Window Handlers

    def processEraseBackgroundEvent(self, event):
        """Process the erase background event."""
        pass # Do nothing, to avoid flashing on MSWin

    def OnSize(self, event = None, win = None):
        """Process the resize event."""
        if self.GetContext():
            # Make sure the frame is shown before calling SetCurrent.
            self.Show()
            self.SetCurrent()

            size = self.GetClientSize()
            self.OnReshape(size.width, size.height)
            self.Refresh(False)
        if event:event.Skip()

    def processPaintEvent(self, event):
        """Process the drawing event."""
        self.SetCurrent()

        # This is a 'perfect' time to initialize OpenGL ... only if we need to
        if not self.GLinitialized:
            self.OnInitGL()
            self.GLinitialized = True

        self.OnDraw()
        event.Skip()

    #
    # GLFrame OpenGL Event Handlers

    def OnInitGL(self):
        """Initialize OpenGL for use in the window."""
        glClearColor(0, 0, 0, 1)
        

    def OnReshape(self, width, height):
        """Reshape the OpenGL viewport based on the dimensions of the window."""
        #global g_Width, g_Height
        self.g_Width = width
        self.g_Height = height
        glViewport(0, 0, self.g_Width, self.g_Height)

    def OnDraw(self, *args, **kwargs):
        """Draw the window."""
        # Clear frame buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        # Set up viewing transformation, looking down -Z axis
        glLoadIdentity()
        gluLookAt(self.eyex, self.eyey, self.eyez, self.centerx,self.centery, self.centerz, self.upx, self.upy, self.upz)   #-.1,0,0
        

        # Set perspective (also zoom)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        # the window corner OpenGL coordinates are (-+1, -+1)
        glOrtho(-1, 1, 1, -1, -1, 1)
        
        aspect = float(self.g_Width)/float(self.g_Height)

        gluPerspective(self.zoom, aspect, self.g_nearPlane, self.g_farPlane)
        glMatrixMode(GL_MODELVIEW)
        self.polarView()

        # draw actual scene
        if self._drawing:
            for drawobj in  self._drawing.get_drawobjs():
                 drawobj.draw()
              

        self.SwapBuffers()
     
    def polarView(self):
        glTranslatef( self.trans[1], 0.0, 0.0 )
        glTranslatef(  0.0, -self.trans[0], 0.0)
        
        glRotatef( -self.zRotate, 0.0, 0.0, 1.0)
        glRotatef( -self.xRotate, 1.0, 0.0, 0.0)
        glRotatef( -self.yRotate, 0.0, 1.0, 0.0)
    
    def get_intersection(self, v_near, v_far):
        # 150918 
        # idea from http://www.bfilipek.com/2012/06/select-mouse-opengl.html
        # https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
        d = -v_near + v_far
        
        t = -v_near[2]/d[2]
        v_inter = v_near+t*d
        
        return v_inter
    
    def unproject(self, pos_display):
        """Get the world coordinates for viewCoordinate for the event
        """
         
        x = pos_display[0]
        y = self.g_Height-pos_display[1]
        
        modelviewmatrix = glGetDoublev(GL_MODELVIEW_MATRIX)
        projectionmatrix = glGetDoublev(GL_PROJECTION_MATRIX)
        viewport = glGetInteger(GL_VIEWPORT)
        
        z = 0.0
        worldCoordinate_near = np.array(gluUnProject(
            x,y,z,
            modelviewmatrix,
            projectionmatrix,
            viewport,),dtype=np.float32)
        
        z=1.0
        worldCoordinate_far = np.array(gluUnProject(
            x,y,z,
            modelviewmatrix,
            projectionmatrix,
            viewport,),dtype=np.float32)
        
        v_inter = self.get_intersection(worldCoordinate_near,worldCoordinate_far)
        return v_inter
    
class OGleditor(wx.Panel):
    
    def __init__(self,parent, 
                    mainframe = None,
                    size = wx.DefaultSize,
                    is_menu=False, # create menu items
                    Debug = 0,
                    ):
        
        
        
        
        wx.Panel.__init__(self, parent, wx.ID_ANY,size = size)
        sizer=wx.BoxSizer(wx.HORIZONTAL)
        
        
        self._mainframe=mainframe
        
        # initialize GL canvas
        self._canvas = OGLcanvas(self)
        
        # compose tool pallet here
        self._toolspanel =   ToolsPanel(self, callback=None, n_buttoncolumns=3)  
        id_initialtool = self._toolspanel.add_tool(BaseTool(self))
        #for i in range(5):
        #    self._toolspanel.add_tool(BaseTool(self))
        self._toolspanel.add_tool(DelTool(self))                            
        self._toolspanel.set_tool_with_id(id_initialtool)
        
        
        # compose editor window
        sizer.Add(self._toolspanel,0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)# from NaviPanelTest
        sizer.Add(self._canvas,1,wx.GROW)# from NaviPanelTest
        
        # finish panel setup
        self.SetSizer(sizer)
        sizer.Fit(self)
        
    def get_canvas(self):
        # this is used mainly by the tools to know on which canvas to operate
        return self._canvas
        
    def get_mainframe(self):
        # this is used mainly by the tools to know on which mainframe to operate
        return self._mainframe
    
    def on_size(self,event=None):
        #self.tc.SetSize(self.GetSize())
        #self.tc.SetSize(self.GetSize())     
        #self._viewtabs.SetSize(self.GetSize()) 
        #pass    
        #wx.LayoutAlgorithm().LayoutWindow(self, self.p1)
        #wx.LayoutAlgorithm().LayoutWindow(self, self.p1)
        
        # important:
        #wx.LayoutAlgorithm().LayoutWindow(self, self._viewtabs)
        
        if event:
            event.Skip()
                    
if __name__ == '__main__':          
    ###############################################################################
    ## MAIN FRAME

    from mainframe import AgileToolbarFrameMixin                
    class OGLeditorMainframe(AgileToolbarFrameMixin, wx.Frame):
        """
        Simple wx frame with some special features.
        """
        
    
        def __init__(self, title, pos=wx.DefaultPosition,
                     size=(1000,500), style=wx.DEFAULT_FRAME_STYLE,
                     name='frame'):
                         
            
            # Forcing a specific style on the window.
            #   Should this include styles passed?
            style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE
            wx.Frame.__init__(self, None, wx.NewId(), title, pos, size = size, style = style, name = name)
            self.gleditor = OGleditor(self)
            
            self.Show()# must be here , before putting stuff on canvas
            self.on_test()
            
                                     
        
            
        def on_test(self,event=None):
            vertices = np.array([
                    [[0.0,0.0,0.0],[0.2,0.0,0.0]],# 0 green
                    [[0.0,0.0,0.0],[0.0,0.9,0.0]],# 1 red
                    ])
            
            
            colors =np.array( [
                    [0.0,0.9,0.0,0.9],    # 0
                    [0.9,0.0,0.0,0.9],    # 1
                    ])
            if 1:
                
                drawing = OGLdrawing()
                lines = Lines('lines', drawing)
                lines.add_drawobjs(vertices,colors)
                drawing.add_drawobj(lines)
                #drawing.add_drawobj(testogl.triangles)
                #drawing.add_drawobj(testogl.rectangles)
                canvas = self.gleditor.get_canvas()
                canvas.set_drawing(drawing)
            
            if 0:
                
                drawing = OGLdrawing()
                lines = Lines('lines', drawing, linewidth=3, vertices = vertices, colors = colors)
                drawing.add_drawobj(lines)
                #drawing.add_drawobj(testogl.triangles)
                #drawing.add_drawobj(testogl.rectangles)
                canvas = self.gleditor.get_canvas()
                canvas.set_drawing(drawing)
                    
            
            if event:
                event.Skip()    
        
        def get_objbrowser(self):
            return self._objbrowser
           
    class OGLeditorApp(wx.App):
        def OnInit(self):
            # wx.InitAllImageHandlers()
            self.mainframe = OGLeditorMainframe("OGLeditor")
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
    
    ###########################################################################
    # MAINLOOP
    
    if len(sys.argv)==3:
        ident = sys.argv[1]
        dirpath = sys.argv[2]
    else:
        ident =  None
        dirpath = None
        app = OGLeditorApp(0)
    
    if 0:#len(noplugins)>0:
        _msg = """The following plugins could not be plugged in:\n\n  """+ noplugins[:-2]+\
        """\n\nIf you like to use these plugins, please check messages on console for Import errors and install the required packages.
        """ 
        dlg = wx.MessageDialog(None, _msg, "Warning from Plugins", wx.OK | wx.ICON_WARNING)
        dlg.ShowModal()
        dlg.Destroy()
    
    app.MainLoop()
