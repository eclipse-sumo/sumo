 
try:
    import wx
    from wx import glcanvas
except ImportError:
    raise ImportError, "Required dependency wx.glcanvas not present"

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
        glDrawElements(GL_TRIANGLES, len(self._vertexvbo), GL_UNSIGNED_INT, None)
        
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



##############################


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



###############################################################################
if __name__ == '__main__':
    
    app = wx.PySimpleApp()
    frame = GLFrame(None, -1, 'GL Window')
    
    frame.Show()
    
    frame.add_element(lines)
    frame.add_element(triangles)
    frame.add_element(rectangles)
    
    app.MainLoop()
    
    app.Destroy()