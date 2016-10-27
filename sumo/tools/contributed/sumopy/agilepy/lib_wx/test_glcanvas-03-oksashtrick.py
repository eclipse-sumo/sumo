'''
@author: Stou Sandalski (stou@icapsid.net)
@license:  Public Domain
'''

#Uncomment if you have multiple wxWidgets versions
#import wxversion
#wxversion.select('2.8')

import math, wx

from wx import glcanvas

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

# wx gui stuff
from mainframe import *  
 
import  objpanel, wxmisc

stockIDs = [
    wx.ID_ABOUT,
    wx.ID_ADD,
    wx.ID_APPLY,
    wx.ID_BOLD,
    wx.ID_CANCEL,
    wx.ID_CLEAR,
    wx.ID_CLOSE,
    wx.ID_COPY,
    wx.ID_CUT,
    wx.ID_DELETE,
    wx.ID_EDIT,
    wx.ID_FIND,
    wx.ID_FILE,
    wx.ID_REPLACE,
    wx.ID_BACKWARD,
    wx.ID_DOWN,
    wx.ID_FORWARD,
    wx.ID_UP,
    wx.ID_HELP,
    wx.ID_HOME,
    wx.ID_INDENT,
    wx.ID_INDEX,
    wx.ID_ITALIC,
    wx.ID_JUSTIFY_CENTER,
    wx.ID_JUSTIFY_FILL,
    wx.ID_JUSTIFY_LEFT,
    wx.ID_JUSTIFY_RIGHT,
    wx.ID_NEW,
    wx.ID_NO,
    wx.ID_OK,
    wx.ID_OPEN,
    wx.ID_PASTE,
    wx.ID_PREFERENCES,
    wx.ID_PRINT,
    wx.ID_PREVIEW,
    wx.ID_PROPERTIES,
    wx.ID_EXIT,
    wx.ID_REDO,
    wx.ID_REFRESH,
    wx.ID_REMOVE,
    wx.ID_REVERT_TO_SAVED,
    wx.ID_SAVE,
    wx.ID_SAVEAS,
    wx.ID_SELECTALL,
    wx.ID_STOP,
    wx.ID_UNDELETE,
    wx.ID_UNDERLINE,
    wx.ID_UNDO,
    wx.ID_UNINDENT,
    wx.ID_YES,
    wx.ID_ZOOM_100,
    wx.ID_ZOOM_FIT,
    wx.ID_ZOOM_IN,
    wx.ID_ZOOM_OUT,

    ]
def get_dist_point_to_segs(p,y1,x1,y2,x2, is_ending=True):
    """
    Minimum Distance between a Point p = (x,y) and a Line segments ,
    where vectors x1, y1 are the first  points and x2,y2 are the second points 
    of the line segments.
    Written by Paul Bourke,    October 1988
    http://astronomy.swin.edu.au/~pbourke/geometry/pointline/
    
    Rewritten in vectorial form by Joerg Schweizer
    """
    
    y3,x3 = p
    
    d = np.zeros(len(y1), dtype=np.float32)
  
    dx21 = (x2-x1)
    dy21 = (y2-y1)
    
    lensq21 = dx21*dx21 + dy21*dy21
    
    # indexvector for all zero length lines
    iz =  (lensq21 == 0) 
    
    dy = y3-y1[iz] 
    dx = x3-x1[iz]
    
    d[iz] = dx*dx + dy*dy
    

    lensq21[iz] = 1.0 # replace zeros with 1.0 to avoid div by zero error  
    
    u = (x3-x1)*dx21 + (y3-y1)*dy21
    u = u / lensq21


    x = x1+ u * dx21
    y = y1+ u * dy21    

    if is_ending:
      ie = u < 0
      x[ie] = x1[ie]
      y[ie] = y1[ie]
      ie = u > 1
      x[ie] = x2[ie]
      y[ie] = y2[ie]

    dx30 = x3-x
    dy30 = y3-y
    d[~iz] = (dx30*dx30 + dy30*dy30)[~iz]
    return d


    
def is_inside_triangles(p,x1,y1,x2,y2,x3,y3):
    """
    Returns a binary vector with True if point p is 
    inside a triangle.
    x1,y1,x2,y2,x3,y3 are vectors with the 3 coordiantes of the triangles.
    """
    alpha = ((y2 - y3)*(p[0] - x3) + (x3 - x2)*(p[1] - y3)) \
            /((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3))
    
    beta = ((y3 - y1)*(p[0] - x3) + (x1 - x3)*(p[1] - y3)) \
        /((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3))
    
    gamma = 1.0 - alpha - beta;
    return (alpha>0)&(beta>0)&(gamma>0)



class WxGLTest_orig(glcanvas.GLCanvas):
    def __init__(self, parent):
        
        glcanvas.GLCanvas.__init__(self, parent,-1, attribList=[glcanvas.WX_GL_DOUBLEBUFFER])
        wx.EVT_PAINT(self, self.OnDraw)
        wx.EVT_SIZE(self, self.OnSize)
        wx.EVT_MOTION(self, self.OnMouseMotion)
        wx.EVT_WINDOW_DESTROY(self, self.OnDestroy)
        
        self.init = True

    def OnDraw(self,event):
        self.SetCurrent()

        if not self.init:
            self.InitGL()
            self.init = False

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        
        # Draw the spiral in 'immediate mode'
        # WARNING: You should not be doing the spiral calculation inside the loop
        # even if you are using glBegin/glEnd, sin/cos are fairly expensive functions
        # I've left it here as is to make the code simpler.
        radius = 1.0
        x = radius*math.sin(0)
        y = radius*math.cos(0)
        glColor(0.0, 1.0, 0.0)
        glBegin(GL_LINE_STRIP)
        for deg in xrange(1000):
            glVertex(x, y, 0.0)
            rad = math.radians(deg)
            radius -= 0.001
            x = radius*math.sin(rad)
            y = radius*math.cos(rad)
        glEnd()
        
        glEnableClientState(GL_VERTEX_ARRAY)
        
        spiral_array = []
        
        # Second Spiral using "array immediate mode" (i.e. Vertex Arrays)
        radius = 0.8
        x = radius*math.sin(0)
        y = radius*math.cos(0)
        glColor(1.0, 0.0, 0.0)
        for deg in xrange(820):
            spiral_array.append([x,y])
            rad = math.radians(deg)
            radius -= 0.001
            x = radius*math.sin(rad)
            y = radius*math.cos(rad)

        glVertexPointerf(spiral_array)
        glDrawArrays(GL_LINE_STRIP, 0, len(spiral_array))
        glFlush()
        self.SwapBuffers()
        return
    
    def InitGL(self):
        '''
        Initialize GL
        '''

#        # set viewing projection
#        glClearColor(0.0, 0.0, 0.0, 1.0)
#        glClearDepth(1.0)
#
#        glMatrixMode(GL_PROJECTION)
#        glLoadIdentity()
#        gluPerspective(40.0, 1.0, 1.0, 30.0)
#
#        glMatrixMode(GL_MODELVIEW)
#        glLoadIdentity()
#        gluLookAt(0.0, 0.0, 10.0,
#                  0.0, 0.0, 0.0,
#                  0.0, 1.0, 0.0)

    def OnSize(self, event):

        try:
            width, height = event.GetSize()
        except:
            width = event.GetSize().width
            height = event.GetSize().height
        
        self.Refresh()
        self.Update()

    def OnMouseMotion(self, event):
        x = event.GetX()
        y = event.GetY()
    
    def OnDestroy(self, event):
        print "Destroying Window"



class Lines:
    """Lines class."""

    def __init__(self, linewidth=3, vertices = None, colors = None):
        self.name = 'Lines'
        self.n_vert_per_elem = 2
        self.linewidth = linewidth
        self.c_highl = 0.3
        self.detectwidth = 0.1 #m
        self.set_attrs(vertices, colors)
      
    def set_attrs(self, vertices, colors):
   
        self.vertices = np.array(vertices, dtype=np.float32)
        self._update_vertexvbo()
        
        self.colors = np.array(colors, dtype=np.float32)
        self.colors_highl = np.zeros((len(colors),4), dtype=np.float32)
        self._update_colorvbo()
        
    def _update_vertexvbo(self):
        self._vertexvbo = vbo.VBO(self.vertices.reshape((-1,3)))
        self._indexvbo = vbo.VBO(np.arange(self.n_vert_per_elem*len(self.vertices),dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)

    def _update_colorvbo(self):
        #self._colorvbo = vbo.VBO( np.resize( np.repeat(np.clip((self.colors+self.c_highl*self.colors_highl) ,0.0,1.0), self.n_vert_per_elem),(len(self.colors),4)) )
        self._colorvbo = vbo.VBO(np.clip((self.colors+self.colors_highl)[np.array(np.arange(0,len(self.colors),1.0/self.n_vert_per_elem),int)],0.0,1.0))
        
        
    def pick(self,p):
        """
        Returns a binary vector which is True values for lines that have been selected 
        by point p.
        
        In particular, an element of this vector is True if the minimum distance 
        between the respective line to point p is less than self.detectwidth
        """
        x1 = self.vertices[:,0,0]
        y1 = self.vertices[:,0,1]
        
        x2 = self.vertices[:,1,0]
        y2 = self.vertices[:,1,1]
            
        
        return get_dist_point_to_segs(p,x1,y1,x2,y2, is_ending=True) < self.detectwidth**2

        
    def highlight(self, inds_highl):
        self.colors_highl = np.repeat(self.c_highl*np.array(inds_highl,dtype = np.float32),4).reshape(-1,4)
        self._update_colorvbo()
      
          
    def draw(self):
        glLineWidth(self.linewidth)

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_BLEND)

        glEnableClientState(GL_VERTEX_ARRAY)
        glEnableClientState(GL_COLOR_ARRAY)
        
        
        self._colorvbo.bind()
        glColorPointer(4, GL_FLOAT, 0, None)
        
        self._vertexvbo.bind()
        self._indexvbo.bind()
        glVertexPointer(3, GL_FLOAT, 0, None)
        
        glDrawElements(GL_LINES, self.n_vert_per_elem*len(self.vertices), GL_UNSIGNED_INT, None)

        
        glDisableClientState(GL_VERTEX_ARRAY)
        glDisableClientState(GL_COLOR_ARRAY)
        self._vertexvbo.unbind()
        self._indexvbo.unbind()
        self._colorvbo.unbind()


class Rectangles(Lines):
    
    def __init__(self, linewidth=3, vertices = None, colors = None):
        self.name = 'Rectangles'
        self.n_vert_per_elem = 4
        self.c_highl= 0.3
        self.detectwidth = 0.1 #m
        self.linewidth = linewidth
        
        self.set_attrs(vertices, colors)
      
    
        
    def pick(self,p):
        
        x1 = self.vertices[:,0,0]
        y1 = self.vertices[:,0,1]
        
        x2 = self.vertices[:,1,0]
        y2 = self.vertices[:,1,1]
            
        x3 = self.vertices[:,2,0]
        y3 = self.vertices[:,2,1]
        
        x4 = self.vertices[:,3,0]
        y4 = self.vertices[:,3,1]
        
        return is_inside_triangles(p,x1,y1,x2,y2,x3,y3) | is_inside_triangles(p,x1,y1,x3,y3,x4,y4)  
        

    
    def draw(self):
        glLineWidth(self.linewidth)

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_BLEND)

        glEnableClientState(GL_VERTEX_ARRAY)
        glEnableClientState(GL_COLOR_ARRAY)
        
        
        self._colorvbo.bind()
        glColorPointer(4, GL_FLOAT, 0, None)
        
        self._vertexvbo.bind()
        self._indexvbo.bind()
        glVertexPointer(3, GL_FLOAT, 0, None)
        
        glDrawElements(GL_QUADS, self.n_vert_per_elem*len(self.vertices), GL_UNSIGNED_INT, None)
    
        glDisableClientState(GL_VERTEX_ARRAY)
        glDisableClientState(GL_COLOR_ARRAY)
        self._vertexvbo.unbind()
        self._indexvbo.unbind()
        self._colorvbo.unbind()

class Triangles(Lines):
    """Triangles class."""

    def __init__(self, linewidth=3, vertices = None, colors = None):
        self.name = 'Triangles'#,self.__name__
        self.n_vert_per_elem = 3
        self.c_highl= 0.3
        self.detectwidth = 0.1 #m
        self.linewidth = linewidth
        
        self.set_attrs(vertices, colors)
      
    

       
    def pick(self,p):
        return is_inside_triangles(p,self.vertices[:,0,0],self.vertices[:,0,1], self.vertices[:,1,0],self.vertices[:,1,1],self.vertices[:,2,0],self.vertices[:,2,1])          

    
    
    
    def draw(self):
        glLineWidth(self.linewidth)

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_BLEND)

        glEnableClientState(GL_VERTEX_ARRAY)
        glEnableClientState(GL_COLOR_ARRAY)
        
        
        self._colorvbo.bind()
        glColorPointer(4, GL_FLOAT, 0, None)
        
        self._vertexvbo.bind()
        self._indexvbo.bind()
        glVertexPointer(3, GL_FLOAT, 0, None)
        
        glDrawElements(GL_TRIANGLES, self.n_vert_per_elem*len(self.vertices), GL_UNSIGNED_INT, None)

        glDisableClientState(GL_VERTEX_ARRAY)
        glDisableClientState(GL_COLOR_ARRAY)
        self._vertexvbo.unbind()
        self._indexvbo.unbind()
        self._colorvbo.unbind()

  






  







class GLFrame(wx.Frame):
    """A simple class for using OpenGL with wxPython."""

    def __init__(self, parent, id=-1, title='', pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE,
                 name='frame',mainframe = None):
        
        print '\n\nGLFrame!!'
        if mainframe == None:
            self._mainframe = parent
        else:
            self._mainframe = mainframe
            
        self._elements = []
        self.elements_selected = []
        
        self.eyex = 0.0
        self.eyey = 0.0
        self.eyez = -9.0
        
        
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
        self.xStart = self.yStart = 0.
        self.xStart
        self.zoom = 65.

        self.xRotate = 0.
        self.yRotate = 0.
        self.zRotate = 0.

        self.xTrans = 0.
        self.yTrans = 0.

        #
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE

        super(GLFrame, self).__init__(parent, id, title, pos, size, style, name)
        #wx.Frame.__init__(self, parent, id, title, pos, size, style, name)
        
        self.GLinitialized = False
        attribList = (glcanvas.WX_GL_RGBA, # RGBA
                      glcanvas.WX_GL_DOUBLEBUFFER, # Double Buffered
                      glcanvas.WX_GL_DEPTH_SIZE, 24) # 24 bit

        #
        # Create the canvas
        self.canvas = glcanvas.GLCanvas(self, attribList=attribList)

        #
        # Set the event handlers.
        self.canvas.Bind(wx.EVT_ERASE_BACKGROUND, self.processEraseBackgroundEvent)
        self.canvas.Bind(wx.EVT_SIZE, self.processSizeEvent)
        self.canvas.Bind(wx.EVT_PAINT, self.processPaintEvent)
        
        self.canvas.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.canvas.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.canvas.Bind(wx.EVT_MOTION, self.OnMotion)
        self.canvas.Bind(wx.EVT_MOUSEWHEEL, self.OnWheel)
        
        
        # this is needed to initialize GL projections for unproject
        wx.CallAfter(self.processSizeEvent)



       
    def add_element(self,element):
        self._elements.append(element)
        self.OnDraw()
      
    def resetView():
        self.zoom = 65.
        self.xRotate = 0.
        self.yRotate = 0.
        self.zRotate = 0.
        self.xTrans = 0.
        self.yTrans = 0.
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
        ##
        if (event.ControlDown()&event.ShiftDown())&(self.action == ''):
          self.action = 'drag'
          self.BeginGrap(event)
          event.Skip()
          
    def OnLeftUp(self, event):
        if self.action=='drag':
          self.EndGrap(event)
          self.action==''
          event.Skip()
    
    def get_intersection(self, v_near, v_far):
        # 150918 
        # idea from http://www.bfilipek.com/2012/06/select-mouse-opengl.html
        # https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
        d = -v_near + v_far
        
        t = -v_near[2]/d[2]
        v_inter = v_near+t*d
        
        return v_inter
    
    
      
    def OnMotion(self, event):

        p = self.unproject(event)[0:2]
        for element in self._elements:
            inds_pick = element.pick(p)
            element.highlight(inds_pick)

        self.OnDraw()
        
        if (event.ControlDown()&event.ShiftDown()&(self.action=='drag')):
          self.MoveGrap(event)
          self.OnDraw()
          event.Skip()
          
        elif (self.action=='drag'):
          self.EndGrap(event)
          self.action==''
          event.Skip()
    
    
    
    def unproject(self, event):
        """Get the world coordinates for viewCoordinate for the event
        """
        mousex,mousey = event.GetPosition() 
        x = mousex
        y = self.g_Height-mousey
       
        modelviewmatrix = glGetDoublev(GL_MODELVIEW_MATRIX)
        projectionmatrix = glGetDoublev(GL_PROJECTION_MATRIX)
        viewport = glGetInteger(GL_VIEWPORT)
        z=0.0
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
   
        

    
    
    def BeginGrap(self,event):
        
        self.xStart,self.yStart = event.GetPosition() 
        #print 'BeginGrap',self.xStart,self.yStart 
        
    def MoveGrap(self,event):
        x,y = event.GetPosition() 
        
        self.xTrans += x-self.xStart
        self.yTrans += y-self.yStart
        #print 'MoveGrap',self.xTrans,self.yTrans 
        self.xStart,self.yStart = x,y
                
                
                
    def EndGrap(self, event):
        #print 'EndGrap'
        self.canvas.SetCursor(wx.NullCursor)
        self.action=''
            
    #
    # Canvas Proxy Methods

    def GetGLExtents(self):
        """Get the extents of the OpenGL canvas."""
        return self.canvas.GetClientSize()

    def SwapBuffers(self):
        """Swap the OpenGL buffers."""
        self.canvas.SwapBuffers()

    #
    # wxPython Window Handlers

    def processEraseBackgroundEvent(self, event):
        """Process the erase background event."""
        pass # Do nothing, to avoid flashing on MSWin

    def processSizeEvent(self, event=None):
        """Process the resize event."""
        if self.canvas.GetContext():
            # Make sure the frame is shown before calling SetCurrent.
            self.Show()
            self.canvas.SetCurrent()

            size = self.GetGLExtents()
            self.OnReshape(size.width, size.height)
            self.canvas.Refresh(False)
        if event:event.Skip()

    def processPaintEvent(self, event):
        """Process the drawing event."""
        self.canvas.SetCurrent()

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
        for element in self._elements:
          element.draw()


        self.SwapBuffers()
     
    def polarView(self):
        glTranslatef( self.yTrans/100., 0.0, 0.0 )
        glTranslatef(  0.0, -self.xTrans/100., 0.0)
        glRotatef( -self.zRotate, 0.0, 0.0, 1.0)
        glRotatef( -self.xRotate, 1.0, 0.0, 0.0)
        glRotatef( -self.yRotate, .0, 1.0, 0.0)


class WxGLTest2(glcanvas.GLCanvas):
    def __init__(self, parent, mainframe = None):
        if mainframe == None:
            self._mainframe = parent
        else:
            self._mainframe = mainframe
            
        self._elements = []
        self.elements_selected = []
        
        self.eyex = 0.0
        self.eyey = 0.0
        self.eyez = -9.0
        
        
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
        self.xStart = self.yStart = 0.
        self.xStart
        self.zoom = 65.

        self.xRotate = 0.
        self.yRotate = 0.
        self.zRotate = 0.

        self.xTrans = 0.
        self.yTrans = 0.

        #
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE

        attribList = (glcanvas.WX_GL_RGBA, # RGBA
                      glcanvas.WX_GL_DOUBLEBUFFER, # Double Buffered
                      glcanvas.WX_GL_DEPTH_SIZE, 24) # 24 bit


        glcanvas.GLCanvas.__init__(self, parent,-1, attribList=attribList)
        #super(WxGLTest2, self).__init__(parent,-1, attribList=attribList)
        
        self.GLinitialized = False
        
        
        
        ###
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        #style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE

        #super(GLFrame, self).__init__(parent, id, title, pos, size, style, name)
        #wx.Frame.__init__(self, parent, id, title, pos, size, style, name)
        
        
        #
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

                 
    def add_element(self,element):
        self._elements.append(element)
        self.OnDraw()
      
    def resetView():
        self.zoom = 65.
        self.xRotate = 0.
        self.yRotate = 0.
        self.zRotate = 0.
        self.xTrans = 0.
        self.yTrans = 0.
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
        ##
        if (event.ControlDown()&event.ShiftDown())&(self.action == ''):
          self.action = 'drag'
          self.BeginGrap(event)
          event.Skip()
          
    def OnLeftUp(self, event):
        if self.action=='drag':
          self.EndGrap(event)
          self.action==''
          event.Skip()
    
    def get_intersection(self, v_near, v_far):
        # 150918 
        # idea from http://www.bfilipek.com/2012/06/select-mouse-opengl.html
        # https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
        d = -v_near + v_far
        
        t = -v_near[2]/d[2]
        v_inter = v_near+t*d
        
        return v_inter
    
    
      
    def OnMotion(self, event):

        p = self.unproject(event)[0:2]
        for element in self._elements:
            inds_pick = element.pick(p)
            element.highlight(inds_pick)

        self.OnDraw()
        
        if (event.ControlDown()&event.ShiftDown()&(self.action=='drag')):
          self.MoveGrap(event)
          self.OnDraw()
          event.Skip()
          
        elif (self.action=='drag'):
          self.EndGrap(event)
          self.action==''
          event.Skip()
    
    
    
    def unproject(self, event):
        """Get the world coordinates for viewCoordinate for the event
        """
        mousex,mousey = event.GetPosition() 
        x = mousex
        y = self.g_Height-mousey
       
        modelviewmatrix = glGetDoublev(GL_MODELVIEW_MATRIX)
        projectionmatrix = glGetDoublev(GL_PROJECTION_MATRIX)
        viewport = glGetInteger(GL_VIEWPORT)
        z=0.0
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
   
        

    
    
    def BeginGrap(self,event):
        
        self.xStart,self.yStart = event.GetPosition() 
        #print 'BeginGrap',self.xStart,self.yStart 
        
    def MoveGrap(self,event):
        x,y = event.GetPosition() 
        
        self.xTrans += x-self.xStart
        self.yTrans += y-self.yStart
        #print 'MoveGrap',self.xTrans,self.yTrans 
        self.xStart,self.yStart = x,y
                
                
                
    def EndGrap(self, event):
        #print 'EndGrap'
        self.SetCursor(wx.NullCursor)
        self.action=''
            
    #
    # Canvas Proxy Methods

    def GetGLExtents(self):
        """Get the extents of the OpenGL canvas."""
        return self.GetClientSize()

    #def SwapBuffers(self):
    #    """Swap the OpenGL buffers."""
    #    self.SwapBuffers()

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

            size = self.GetGLExtents()
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
        for element in self._elements:
          element.draw()
          # causes bad things :AttributeError: 'Implementation' object has no attribute 'glGenBuffers'



        self.SwapBuffers()
     
    def polarView(self):
        glTranslatef( self.yTrans/100., 0.0, 0.0 )
        glTranslatef(  0.0, -self.xTrans/100., 0.0)
        glRotatef( -self.zRotate, 0.0, 0.0, 1.0)
        glRotatef( -self.xRotate, 1.0, 0.0, 0.0)
        glRotatef( -self.yRotate, .0, 1.0, 0.0)
        
        
class WxGLTest(glcanvas.GLCanvas):
    def __init__(self, parent, mainframe = None):
        if mainframe == None:
            self._mainframe = parent
        else:
            self._mainframe = mainframe
            
        self._elements = []
        self.elements_selected = []
        
        self.eyex = 0.0
        self.eyey = 0.0
        self.eyez = -9.0
        
        
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
        self.xStart = self.yStart = 0.
        self.xStart
        self.zoom = 65.

        self.xRotate = 0.
        self.yRotate = 0.
        self.zRotate = 0.

        self.xTrans = 0.
        self.yTrans = 0.

        #
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE

        attribList = (glcanvas.WX_GL_RGBA, # RGBA
                      glcanvas.WX_GL_DOUBLEBUFFER, # Double Buffered
                      glcanvas.WX_GL_DEPTH_SIZE, 24) # 24 bit


        glcanvas.GLCanvas.__init__(self, parent,-1, attribList=attribList)
        wx.EVT_PAINT(self, self.OnDraw)
        #wx.EVT_SIZE(self, self.OnSize)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        wx.EVT_MOTION(self, self.OnMouseMotion)
        wx.EVT_WINDOW_DESTROY(self, self.OnDestroy)
        
        self.init = True
        
        
        
        ###
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        #style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE

        #super(GLFrame, self).__init__(parent, id, title, pos, size, style, name)
        #wx.Frame.__init__(self, parent, id, title, pos, size, style, name)
        
        #self.GLinitialized = False
        #attribList = (glcanvas.WX_GL_RGBA, # RGBA
        #              glcanvas.WX_GL_DOUBLEBUFFER, # Double Buffered
        #              glcanvas.WX_GL_DEPTH_SIZE, 24) # 24 bit

        #
        # Create the canvas
        #self.canvas = glcanvas.GLCanvas(self, attribList=attribList)

        #
        # Set the event handlers.
        #self.canvas.Bind(wx.EVT_ERASE_BACKGROUND, self.processEraseBackgroundEvent)
        #self.canvas.Bind(wx.EVT_SIZE, self.processSizeEvent)
        #self.canvas.Bind(wx.EVT_PAINT, self.processPaintEvent)
        
        #self.canvas.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        #self.canvas.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        #self.canvas.Bind(wx.EVT_MOTION, self.OnMotion)
        #self.canvas.Bind(wx.EVT_MOUSEWHEEL, self.OnWheel)
        
        
        # this is needed to initialize GL projections for unproject
        wx.CallAfter(self.OnSize)


    def OnDraw(self,event):
        self.SetCurrent()

        if not self.init:
            self.InitGL()
            self.init = False

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        
        # Draw the spiral in 'immediate mode'
        # WARNING: You should not be doing the spiral calculation inside the loop
        # even if you are using glBegin/glEnd, sin/cos are fairly expensive functions
        # I've left it here as is to make the code simpler.
        radius = 1.0
        x = radius*math.sin(0)
        y = radius*math.cos(0)
        glColor(0.0, 1.0, 0.0)
        glBegin(GL_LINE_STRIP)
        for deg in xrange(1000):
            glVertex(x, y, 0.0)
            rad = math.radians(deg)
            radius -= 0.001
            x = radius*math.sin(rad)
            y = radius*math.cos(rad)
        glEnd()
        
        glEnableClientState(GL_VERTEX_ARRAY)
        
        spiral_array = []
        
        # Second Spiral using "array immediate mode" (i.e. Vertex Arrays)
        radius = 0.8
        x = radius*math.sin(0)
        y = radius*math.cos(0)
        glColor(1.0, 0.0, 0.0)
        for deg in xrange(820):
            spiral_array.append([x,y])
            rad = math.radians(deg)
            radius -= 0.001
            x = radius*math.sin(rad)
            y = radius*math.cos(rad)

        glVertexPointerf(spiral_array)
        glDrawArrays(GL_LINE_STRIP, 0, len(spiral_array))
        glFlush()
        self.SwapBuffers()
        return
    
    def InitGL(self):
        '''
        Initialize GL
        '''
        glClearColor(0, 0, 0, 1)
#        # set viewing projection
#        glClearColor(0.0, 0.0, 0.0, 1.0)
#        glClearDepth(1.0)
#
#        glMatrixMode(GL_PROJECTION)
#        glLoadIdentity()
#        gluPerspective(40.0, 1.0, 1.0, 30.0)
#
#        glMatrixMode(GL_MODELVIEW)
#        glLoadIdentity()
#        gluLookAt(0.0, 0.0, 10.0,
#                  0.0, 0.0, 0.0,
#                  0.0, 1.0, 0.0)

    def OnSize(self, event=None):
        if event:
            try:
                width, height = event.GetSize()
            except:
                width = event.GetSize().width
                height = event.GetSize().height
            print 'OnSize',width,width
            self.Refresh()
            self.Update()
        else:
            self.processSizeEvent( event=event)

    def processSizeEvent(self, event=None):
        """Process the resize event."""
        print 'processSizeEvent'
        if self.GetContext():
            # Make sure the frame is shown before calling SetCurrent.
            #self.Show()
            self.SetCurrent()
            
            #size = self.GetGLExtents()
            #self.OnReshape(size.width, size.height)
            
            self.Refresh(False)
        if event:event.Skip()
    
    def OnMouseMotion(self, event):
        x = event.GetX()
        y = event.GetY()
    
    def OnDestroy(self, event):
        print "Destroying Window"

    
#-------------------------------------------------------------------------------
    def OnReshape(self, width, height):
        """Reshape the OpenGL viewport based on the dimensions of the window."""
        #global g_Width, g_Height
        self.g_Width = width
        self.g_Height = height
        glViewport(0, 0, self.g_Width, self.g_Height)
    
##############################




        
class MySplitter(wx.SplitterWindow):
    def __init__(self, parent, ID=-1):
        wx.SplitterWindow.__init__(self, parent, ID,
                                   style = wx.SP_LIVE_UPDATE
                                   )
        #self.log = log
        
        #sty = wx.BORDER_NONE
        #sty = wx.BORDER_SIMPLE
        sty = wx.BORDER_SUNKEN
        
        p1 = wx.Window(self, style=sty)
        p1.SetBackgroundColour("pink")
        wx.StaticText(p1, -1, "Panel One", (5,5))
    
        p2 = wx.Window(self, style=sty)
        self.canvas = WxGLTest_orig(p2)
        #canvas = GLFrame(p2)
        
        #p2.SetBackgroundColour("sky blue")
        #wx.StaticText(p2, -1, "Panel Two", (5,5))
    
        self.SetMinimumPaneSize(20)
        self.SplitVertically(p1, p2, -100)
    
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, self.OnSashChanged)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGING, self.OnSashChanging)

    def OnSashChanged(self, evt):
        print("sash changed to %s\n" % str(evt.GetSashPosition()))
        
        
    def OnSashChanging(self, evt):
        print("sash changing to %s\n" % str(evt.GetSashPosition()))
        # uncomment this to not allow the change
        #evt.SetSashPosition(-1)
        self.canvas.OnSize()
        
class Splitter(wx.SplitterWindow):
    def __init__(self, parent, ID=-1):
        wx.SplitterWindow.__init__(self, parent, ID,
                                   style = wx.SP_LIVE_UPDATE
                                   )
        
        
        
        self.SetMinimumPaneSize(20)
        
    
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, self.OnSashChanged)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGING, self.OnSashChanging)

    def OnSashChanged(self, evt):
        print("sash changed to %s\n" % str(evt.GetSashPosition()))
        
        
    def OnSashChanging(self, evt):
        print("sash changing to %s\n" % str(evt.GetSashPosition()))
        # uncomment this to not allow the change
        #evt.SetSashPosition(-1)
        #self.canvas.OnSize()

class TestMainframe(wx.Frame):
    """
    Simple wx frame with some special features.
    """
    

    def __init__(self, parent=None,  id=-1, title ='mainframe', pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE,
                 name='frame'):
                     
        
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE
        wx.Frame.__init__(self, parent, id, title, pos, size, style, name)
        #super(GLFrame, self).__init__(parent, id, title, pos, size, style, name)
        splitter = Splitter(self)
        sty = wx.BORDER_SUNKEN
        
        emptyobj = cm.BaseObjman('empty1')
        self._objbrowser =  objpanel.NaviPanel(splitter, 
                                                emptyobj,
                                                #show_title = False
                                                #size = w.DefaultSize,
                                                #style = wx.DEFAULT_DIALOG_STYLE|wx.MAXIMIZE_BOX|wx.RESIZE_BORDER,
                                                #choose_id=False,choose_attr=False,
                                                #func_choose_id=None,
                                                #func_change_obj=None,
                                                #panelstyle = 'default', 
                                                immediate_apply = False,
                                                buttons = [],
                                                standartbuttons = [],
                                                #defaultbutton = defaultbutton,
                                                )
                                                
        #p1 = wx.Window(splitter, style=sty)
        #p1.SetBackgroundColour("pink")
        #wx.StaticText(p1, -1, "Object", (50,50))
        
        #self.canvas = wx.Window(splitter, style=sty)
        #self.canvas.SetBackgroundColour("green")
        #wx.StaticText(self.canvas, -1, "Panel two", (50,50))
        #self.canvas = WxGLTest2(splitter)
        #self.canvas = WxGLTest_orig(splitter)
        self._views = {}
        self._viewtabs = wx.Notebook(splitter,wx.ID_ANY, style=wx.CLIP_CHILDREN)
        
        
        #nbpanel = wx.Panel(splitter)
        #self._viewtabs = wx.Notebook(nbpanel,wx.ID_ANY, style=wx.CLIP_CHILDREN)
        #sizer = wx.BoxSizer(wx.VERTICAL)
        #sizer.Add(self._viewtabs, 1, wx.ALL|wx.EXPAND, 5)
        #nbpanel.SetSizer(sizer)
        #self.Layout()
        
        # finally, put the notebook in a sizer for the panel to manage
        # the layout
        #sizer = wx.BoxSizer()
        #sizer.Add(self._viewtabs, 1, wx.EXPAND)
        #self.SetSizer(sizer)
        
        
        #splitter.SplitVertically(self._objbrowser,self.canvas , -100)
        splitter.SplitVertically(self._objbrowser, self._viewtabs, -100)
        wx.EVT_SIZE  (self, self.on_size)
        
        #sizer=wx.BoxSizer(wx.VERTICAL)
        #sizer.Add(p1,0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)# from NaviPanelTest
        #sizer.Add(self.canvas,1,wx.GROW)# from NaviPanelTest
        
        # finish panel setup
        #self.SetSizer(sizer)
        #sizer.Fit(self)
        self.Show()
        
        # this is needed to initialize GL projections for unproject
        wx.CallAfter(self.on_size)
        splitter.SetSashPosition(500, True)
    
    def add_view(self, name, ViewClass, **args):
        """
        Add a new view to the notebook.
        """ 
        #print 'context.add_view',ViewClass
        #print '  args',args
        view=ViewClass(self._viewtabs, 
                        mainframe = self,
                        **args
                        )
                        
        # Add network tab with editor                           
        p=self._viewtabs.AddPage( view, name.title() )
        self._views[name] = view
        #self._viewtabs.SetSelection(p)
        self._viewtabs.Show(True)
        return view
        
    
                    
    
    def on_size(self,event=None):
        #self.tc.SetSize(self.GetSize())
        #self.tc.SetSize(self.GetSize())     
        #self._viewtabs.SetSize(self.GetSize()) 
        #pass    
        #wx.LayoutAlgorithm().LayoutWindow(self, self.p1)
        #wx.LayoutAlgorithm().LayoutWindow(self, self.p1)
        wx.LayoutAlgorithm().LayoutWindow(self, self._viewtabs)
        #self.canvas.Refresh()
        if event:
            event.Skip()
    
linewidth = 3
vertices = [
        [[0.0,0.0,0.0],[0.2,0.0,0.0]],# 0
        [[0.3,0.0,0.0],[0.9,0.0,0.0]],# 1
        ]


colors = [
        [0.0,0.9,0.0,0.9],    # 0
        [0.9,0.0,0.0,0.9],    # 1
        ]
lines = Lines(linewidth=linewidth, vertices = vertices, colors = colors)

linewidth2 = 3
vertices2 = [
        [[0.5,0.5,0.0],[0.7,0.5,0.0],[0.7,1.0,0.0]],# 0
        [[0.8,0.5,0.0],[0.9,0.8,0.0],[0.8,0.2,0.0]],# 1
        ]
colors2 = [
        [0.0,0.9,0.3,0.9],    # 0
        [0.9,0.3,0.0,0.9],    # 1
        ]
triangles = Triangles(linewidth=linewidth2, vertices = vertices2, colors = colors2)

linewidth3 = 3
vertices3 = [
        [[0.5,0.0,0.0],[0.7,0.0,0.0],[0.7,0.3,0.0],[0.5,0.3,0.0],],# 0
        [[0.1,0.0,0.0],[0.3,0.0,0.0],[0.3,0.2,0.0],[0.1,0.2,0.0],],# 1
        ]
colors3 = [
        [0.8,0.0,0.8,0.9],    # 0
        [0.0,0.6,0.6,0.9],    # 1
        ]
rectangles = Rectangles(linewidth=linewidth3, vertices = vertices3, colors = colors3)
     
if __name__ == '__main__':
    
    

    app = wx.PySimpleApp()
    if 1:
        frame = TestMainframe()
        glcanvas = frame.add_view('GL Editor',WxGLTest2)
        glcanvas.add_element(lines)
        glcanvas.add_element(triangles)
        glcanvas.add_element(rectangles)
        
    elif 0:
        
        frame = TestMainframe()
        frame.canvas.add_element(lines)
        frame.canvas.add_element(triangles)
        frame.canvas.add_element(rectangles)
        
    elif 0:
        frame = GLFrame(None, -1, 'GL Window')
        
        frame.Show()
        
        frame.add_element(lines)
        frame.add_element(triangles)
        frame.add_element(rectangles)
    app.SetTopWindow(frame)
    app.MainLoop()
    
    app.Destroy()