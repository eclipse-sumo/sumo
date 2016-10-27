#!/usr/bin/env python;
"""OpenGL editor"""
if __name__ == '__main__': 
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
from collections import OrderedDict
from wx import glcanvas
from wx.lib.buttons import GenBitmapTextButton, GenBitmapButton
try:
    from OpenGL.GL import *
    from OpenGL.GLU import * # project , unproject , tess
    from OpenGL.GLUT import *
    from OpenGL.raw.GL.ARB.vertex_array_object import glGenVertexArrays, \
                                                  glBindVertexArray
                                                
    from OpenGL.arrays import vbo
    import numpy as np
    
except ImportError:
    raise ImportError, "Required dependency OpenGL not present"

import sys, os, types
if  __name__ == '__main__':
    APPDIR = os.path.join(os.path.dirname(__file__),"..")
    sys.path.append(os.path.join(APPDIR,"lib_base"))
    
IMAGEDIR = os.path.join(os.path.dirname(__file__),"images")

import classman as cm
import arrayman as am

from geometry import *

# wx gui stuff
from wxmisc import *  
from toolbox import *  



FLATHEAD = 0             
BEVELHEAD = 1
TRIANGLEHEAD = 2 
ARROWHEAD = 3 

LINEHEADS = {  #'flat':0,# not a style
                'bevel':BEVELHEAD,
                'triangle':TRIANGLEHEAD,
                'arrow':ARROWHEAD,
                } 
              
# to be deleted
#import  test_glcanvas as testogl
##class TestVbo(Vbo):
##    def draw(self, resolution):
##        
##        
##        
##        print 'draw',self.get_ident(),self
##        glEnableClientState(GL_VERTEX_ARRAY)
##        glEnableClientState(GL_COLOR_ARRAY)
##        
##        self._colorvbo.bind()
##        glColorPointer(4, GL_FLOAT, 0, None)
##        
##        self._vertexvbo.bind()
##        self._indexvbo.bind()
##        glVertexPointer(3, GL_FLOAT, 0, None)
##        
##        print '  self._n_vpo *(self._n_vpo-1)*self._n_drawobjs',self._n_vpo *(self._n_vpo-1)*self._n_drawobjs
##        print '  len(self._vertexvbo)',len(self._vertexvbo)
##        print '  len(self._colorvbo)',len(self._colorvbo)
##        #n_vpo = len(self._vertexvbo)/self._n_vpe/self._n_drawobjs+1
##        #glDrawElements(self._glelement, self._n_vpo *(self._n_vpo-1)*self._n_drawobjs, GL_UNSIGNED_INT, None)
##        glDrawElements(self._glelement, len(self._vertexvbo), GL_UNSIGNED_INT, None)
##        
##        glDisableClientState(GL_VERTEX_ARRAY)
##        glDisableClientState(GL_COLOR_ARRAY)
##        
##        self._vertexvbo.unbind()
##        self._indexvbo.unbind()
##        self._colorvbo.unbind()
        
    
class Vbo:
    def __init__(self, ident, glelement, n_vpe, objtype=''):
        self._ident = ident
        self._glelement = glelement
        self._n_vpe = n_vpe
        self._objtype = objtype
        
        self._vertexvbo = None
        self._indexvbo  = None
        self._colorvbo = None
        self._inds = None
        
    def get_objtype(self):
        return self._objtype
    
    def is_fill(self):
        return self._objtype == 'fill'
    
    def get_ident(self):
        return self._ident
    
    def get_vpe(self):
        return self._n_vpe
    
    def update_vertices(self, vertices, n=None, inds = None):
        """
        
        n = number of draw objects
        """
        self._inds = inds
        #print 'update_vertices',self.get_ident(),n
        self._n_drawobjs = n
        # 
        #vertices = np.zeros((self._n_elem_fill, self._n_vpe_fill * 3),dtype=np.float32)
        # len(vertices) = n_elem = (n_vpo-1)*n 
        #n_elem = len(vertices)/n_drawobjs+self._noncyclic
        
        #n_vpo = vertex per draw object
        # n_elem = (n_vpo-1)*n_drawobjs 
        #n_vpo = vertex per draw object
        #self._n_vpo = n_elem/n_drawobjs+1
        #n_vert = len(self._vertexvbo_fill)/self._n_vpe_fill/len(self)+1
        #n_elem_fill = (n_vert-1)*n 
        # len(_vertexvbo_fill) =  self._n_vpe * self._n_elem_fill =  self._n_vpe * (n_vert-1)*n                
        #print '  len(vertices),n_vpo,n_elem',len(vertices),self._n_vpo,n_elem
        
        #glDrawElements(GL_TRIANGLES, self._n_vert*self._n_elem_fill, GL_UNSIGNED_INT, None)
        #  self._n_vert*self._n_elem_fill = n_vert * (n_vert-1)*n
        #repeat for each of the n objects:  self._n_vpe_fill*(n_vert-1)
        #print '  vertices=\n',vertices
        #print '  vertices.reshape((-1,3))=\n',vertices.reshape((-1,3))
        self._vertexvbo = vbo.VBO(vertices.reshape((-1,3)))
        self._indexvbo = vbo.VBO(np.arange(len(self._vertexvbo), dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)
        #self._indexvbo = vbo.VBO(np.arange(len(vertices.reshape((-1,3)) ), dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)
        
    def update_colors(self, colors):
        if self._vertexvbo == None: return 
        if self._n_drawobjs == None:
            n_repreat = len(self._vertexvbo)/len(self._inds)# self._n_drawobjs
            self._colorvbo = vbo.VBO( np.repeat(colors[self._inds],n_repreat,0))
        else:
            n_repreat = len(self._vertexvbo)/self._n_drawobjs
            self._colorvbo = vbo.VBO( np.repeat(colors,n_repreat,0))

    
    def draw(self, resolution):
        #print 'draw',self.get_ident(),self
        #glEnableClientState(GL_VERTEX_ARRAY)
        #glEnableClientState(GL_COLOR_ARRAY)
        if self._vertexvbo == None: return 
        self._colorvbo.bind()
        glColorPointer(4, GL_FLOAT, 0, None)
        
        self._vertexvbo.bind()
        self._indexvbo.bind()
        glVertexPointer(3, GL_FLOAT, 0, None)
        
        #print '  self._n_vpo *(self._n_vpo-1)*self._n_drawobjs',self._n_vpo *(self._n_vpo-1)*self._n_drawobjs
        #print '  len(self._vertexvbo)',len(self._vertexvbo)
        #print '  len(self._indexvbo)',len(self._indexvbo)
        #print '  len(self._colorvbo)',len(self._colorvbo)
        #n_vpo = len(self._vertexvbo)/self._n_vpe/self._n_drawobjs+1
        #glDrawElements(self._glelement, self._n_vpo *(self._n_vpo-1)*self._n_drawobjs, GL_UNSIGNED_INT, None)
        glDrawElements(self._glelement, len(self._vertexvbo), GL_UNSIGNED_INT, None)
        
        #glDisableClientState(GL_VERTEX_ARRAY)
        #glDisableClientState(GL_COLOR_ARRAY)
        
        self._vertexvbo.unbind()
        self._indexvbo.unbind()
        self._colorvbo.unbind()

class DrawobjList(am.ArrayObjman):
    """
    Currently not in use (selection tool has its own list)
    """
    def __init__(self,ident,  parent=None,  **kwargs):
        self._init_objman(ident,parent=parent, **kwargs)
        self.add_col(am.TabIdsArrayConf( 'obj_ids',
                                    groupnames = ['options'], 
                                    name = 'Object[id]',
                                    info = 'Draw obj and ids',
                                    ))
        
class SelectTool(BaseTool):
    """
    Selection tool for OGL canvas.
    """ 
    
    def __init__(self,parent, mainframe=None):
        """
        To be overridden by specific tool.
        """
        self.init_common('select',parent,'Selection tool', info = 'Select objects in cancvas')
        self.add_col(am.TabIdsArrayConf(    'drawobjects',
                                            groupnames = ['options'], 
                                            name = 'Object',
                                            info = 'Selected Draw objects with id',
                                            ))
              
    def set_button_info(self,bsize=(32,32)):
        #print 'set_button_info select tool'
        self._bitmap = wx.Bitmap(os.path.join(IMAGEDIR,'selectIcon.bmp'),wx.BITMAP_TYPE_BMP)
        self._bitmap_sel=wx.Bitmap(os.path.join(IMAGEDIR,'selectIconSel.bmp'),wx.BITMAP_TYPE_BMP)
    
    def activate(self,canvas = None):
        """
        This call by metacanvas signals that the tool has been
        activated and can now interact with metacanvas.
        """
        #print 'activate_metacanvas',self.ident
        self._canvas = canvas
        #self._canvas.del_handles()
        canvas.activate_tool(self)
    

    def deactivate(self):
        """
        This call by metacanvas signals that the tool has been
        deactivated and can now interact with metacanvas.
        """
        drawing= self._canvas.get_drawing()
        
        if len(self)>0:
            # ungighlight selected objects
            is_draw = False
            for drawobj, _id in  self.drawobjects.value:# drawing.get_drawobjs():
                 is_draw |= drawobj.unhighlight([_id], is_update = True)
            if is_draw:
                self._canvas.on_draw()
                 
            # remove selected objects 
            self.del_rows(self.get_ids())
               
        self._canvas.deactivate_tool()
        self._canvas = None

    def on_wheel(self, event):
        return False


    def on_left_down(self, event):
        p = self._canvas.unproject_event(event) 
        #vetrex = np.array([p[0], p[1], 0.0, 1.0],float)
        #p_screen = self._canvas.project(vetrex)
        #print 'SelectTool.on_left_down (px,py)=',p
        #print '  (x,y)=',event.GetPosition(),p_screen
         
        is_draw = False
        is_hit = False
        drawing = self._canvas.get_drawing()
        if drawing:
            for drawobj in  drawing.get_drawobjs():
                 ids_pick = drawobj.pick(p)
                 if len(ids_pick)>0:
                    is_hit = True 
                    is_draw |= self.select(drawobj,ids_pick[0], event)
                    break
        
        if (not is_hit) & (not event.ControlDown()) & (len(self)>0):
            # clicked somewhere -> remove selection
            self.unselect_all()
            is_draw = True
            
        if is_draw:
            self.parent.refresh_optionspanel(self)
            
            
        return is_draw
    
    def unselect_all(self):
        for drawobj, _id in self.drawobjects.value:
            drawobj.unhighlight([_id])
        
        self.del_rows(self.get_ids())
        #print 'unselect_all',len(self._ids),len(self._inds),len(self._ids),len(self._inds),len(self.drawobjects.value)
        #print '  drawobjects',self.drawobjects.value
        #print '  self._ids',self._ids
            
    def select(self, drawobj, _id, event, is_draw = False):
        #print 'select',drawobj.format_ident(),_id
        #drawobjid = np.array(( drawobj, _id), self.drawobjects.get_dtype())
        
        drawobjid =  self.drawobjects.convert_type(( drawobj, _id))
        #print '  len(self._ids),len(self._inds),len(self.drawobjects.value)',len(self._ids),len(self._inds),len(self.drawobjects.value)
        #print '  ==',self.drawobjects.value == drawobjid
        ids_match = self.select_ids(self.drawobjects.value == drawobjid) 
        
        #print '  ids_match',ids_match
        if len(ids_match)>0:
            # object already selected
            if event.ControlDown():
                # remove selection
                drawobj.unhighlight([_id])
                self.del_rows(ids_match)
                is_draw = True
        else:
            # add selection and highlight
            drawobj.highlight([_id])
            self.add_rows(1,drawobjects = [( drawobj, _id)])
            is_draw = True
        
        return is_draw 
        
        
    def on_left_up(self, event):
        return False
    
    def on_left_wheel(self, event):
        return False
    
    def on_motion(self, event):
        return False # return True if something moved
    


class DrawobjMixin(am.ArrayObjman):
    def init_common(self,ident,parent=None, name = None, 
                            linewidth = 1,
                            is_parentobj = False,
                            is_outline = True, # show outline
                            is_fill = False,
                            is_fillable = True,
                            n_vert = 2, # number of vertex per draw object
                            c_highl=0.3,**kwargs):
                                
        self._init_objman(ident, parent=parent, name = name,**kwargs) 
        
        self._vbos = OrderedDict()
        
        self._n_vert = n_vert

        
        # TODO: these 2 could be eliminated and replaced by function
        #self._n_elem = 0# will be adjusted in update_vertexvbo
        #self._n_elem_fill = 0#
        
        
        
        if is_fillable > 0:
            # objects can potentially be filled
            self.add(cm.AttrConf(  '_is_fill', is_fill,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Fill', 
                                        info = 'If True, object are filled.',
                                        ))
            self.add(cm.AttrConf(  '_is_outline', is_outline,
                                                groupnames = ['options'], 
                                                perm='rw', 
                                                is_save = True,
                                                is_plugin = False,
                                                name = 'Outline', 
                                                info = 'If True, outline of object is shown.',
                                                ))
                                            
            self.add_col(am.ArrayConf( 'colors_fill',  np.zeros(4, dtype=np.float32),
                                        dtype=np.float32,
                                        groupnames = ['options'], 
                                        perm='rw', 
                                        metatype = 'color',
                                        name = 'Fill color',
                                        is_save = True,
                                        info = 'Object fill color with format [r,g,b,a]',
                                        ))
                                    
            self.add_col(am.ArrayConf( 'colors_fill_highl',  np.zeros(4, dtype=np.float32),
                                        dtype=np.float32,
                                        groupnames = ['_private'], 
                                        perm='rw', 
                                        metatype = 'color',
                                        name = 'Color highl',
                                        is_save = False,
                                        info = 'Object fill color when highlighting with format [r,g,b,a]',
                                        ))
                                    
        else:
            # no filling foreseen
            self.add(cm.AttrConf(  '_is_fill', False,
                                            groupnames = ['_private_'], 
                                            perm='r', 
                                            is_save = True,
                                            is_plugin = False,
                                            name = 'Filled', 
                                            info = 'If True, object is filled.',
                                            ))
        
            self.add(cm.AttrConf(  '_is_outline', True,
                                                groupnames = ['_private_'], 
                                                perm='r', 
                                                is_save = True,
                                                is_plugin = False,
                                                name = 'Outline', 
                                                info = 'If True, outline of object is shown.',
                                                ))
        if is_outline:
            self.add_col(am.ArrayConf( 'colors',  np.zeros(4, dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    metatype = 'color',
                                    name = 'Color',
                                    is_save = True,
                                    info = 'Object Vertex color with format [r,g,b,a]',
                                    ))
                                    
            self.add_col(am.ArrayConf( 'colors_highl',  np.zeros(4, dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    metatype = 'color',
                                    name = 'Color highl',
                                    is_save = False,
                                    info = 'Added object color when highlighting with format [r,g,b,a]',
                                    ))
                                                                            
        self.add(cm.AttrConf(  'linewidth', linewidth,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Line width', 
                                        info = 'Line width in pixel',
                                        ))
                                        
        #print 'init_common',self.format_ident(),self._is_fill.value,self._is_outline.value
        
        self.add(cm.AttrConf(  'c_highl', c_highl,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Highl. const.', 
                                        info = 'Highlight constant takes values beteewn 0 and 1 and is the amount of brightness added for highlighting',
                                        ))
                                        
        
                                    
        self.add_col(am.ArrayConf( 'are_highlighted',  False,
                                    dtype=np.bool,
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    name = 'highlighted',
                                    is_save = False,
                                    info = 'If true, object is highlighted',
                                    ))
        #self.add_col(am.ArrayConf( 'have_handles',  False,
        #                            dtype=np.bool,
        #                            groupnames = ['options'], 
        #                            perm='rw', 
        #                            name = 'has handles',
        #                            is_save = False,
        #                            info = 'If true, object has handles',
        #                            ))
                                                                
         
                                
        if is_parentobj:   
            self.add_col(am.IdsArrayConf( 'ids_parent', parent,
                                        groupnames = ['state'], 
                                        is_save = True,
                                        name = parent.format_ident()+'[ID]', 
                                        info = 'ID of '+parent.get_name()+' object.',
                                        ))
    def add_vbo(self,vbo):
        self._vbos[vbo.get_ident()] = vbo
    
    def get_vbo(self,ident):
        return self._vbos[ident]
    
    def get_vbos(self):
        return self._vbos.values()
    
    def del_vbo  (self,key):
        del self._vbos[key]
                              
    def get_n_vert(self):
        return self._n_vert
    
    
    def _get_colors_highl(self, colors):
        return np.clip(colors+self.c_highl.value*np.ones(colors.shape,float),0,1)-colors
    
    
    def set_colors(self, ids, colors, colors_highl=None, is_update = True):
        self.colors[ids] = colors
        if colors_highl==None:
            self.colors_highl[ids]=self._get_colors_highl(colors)
        else:
            self.colors_highl[ids]=colors_highl
        if is_update:
            self._update_colorvbo() 
            
    def _update_colorvbo(self):
        n=len(self)
        #n_vert = self.get_n_vert()
        #print '_update_colorvbo fancyline'
        
        if self._is_outline.value:
            colors = self.colors.value + self.are_highlighted.value.reshape(n,1)*self.colors_highl.value
            for _vbo in self.get_vbos():
                if not _vbo.is_fill():
                    _vbo.update_colors(colors)
                    
        if self._is_fill.value:
            colors = self.colors_fill.value + self.are_highlighted.value.reshape(n,1)*self.colors_fill_highl.value
            for _vbo in self.get_vbos():
                if _vbo.is_fill():
                    _vbo.update_colors(colors)
                    
    def draw(self, resolution=1.0):
        #print 'draw n=',len(self)
        
        glLineWidth(self.linewidth.value)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_BLEND)

        # main drawing
        glEnableClientState(GL_VERTEX_ARRAY)
        glEnableClientState(GL_COLOR_ARRAY)
        
        #print 'draw',self.format_ident(), self._is_fill.value, self._is_outline.value   
        
        for _vbo in self.get_vbos():
            _vbo.draw(resolution)
        
        
        # draw handles
        glDisableClientState(GL_VERTEX_ARRAY)
        glDisableClientState(GL_COLOR_ARRAY)
        
    def highlight(self, ids, is_update = True):
        #print 'highlight=',self._inds[ids]
        if len(ids)>0:
            self.are_highlighted.value[self._inds[ids]] = True
            is_draw = True
        else:
           is_draw =  False
        
        if is_update:
                self._update_colorvbo()
                
        return is_draw
      
    def unhighlight(self, ids=None, is_update = True):
        
        if ids == None:
            if np.any(self.are_highlighted.value):
                inds_highl = self._inds# np.flatnonzero(self.are_highlighted.value)
                self.are_highlighted.value[:]= False
                is_draw = True
            else:
                is_draw = False
        else:
            inds_highl = self._inds[ids]
            if np.any(self.are_highlighted.value[inds_highl]):
                self.are_highlighted.value[inds_highl] = False
                is_draw = True
            else:
                is_draw = False
                
        #print 'unhighlight' ,  inds_highl
        #print '  self.are_highlighted.value',self.are_highlighted.value
        
        if is_update:
            self._update_colorvbo()
        return is_draw
                                                  

                            
                            
class Lines(DrawobjMixin):
    def __init__(self,ident,  parent, name = 'Lines',
            is_parentobj = False,
            n_vert =2,  # 2 verts for line draw obj
            c_highl=0.3, 
            linewidth=3,
            detectwidth = 0.1,#m
            **kwargs):
        
        self.init_common(   ident,parent=parent, name = name,
                            linewidth = linewidth,
                            is_parentobj=is_parentobj, 
                            n_vert = n_vert, 
                            c_highl=c_highl,
                            is_fillable = False,
                            **kwargs)
        
        # parameter for detecting clicls beside line                    
        self.detectwidth = detectwidth #m
        
        self.add_vbo(Vbo('line', GL_LINES, 2, objtype='outline'))# 2 verts for OGL line element
                                        
        self.add_col(am.ArrayConf( 'vertices',  np.zeros((n_vert,3), dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    name = 'Vertex',
                                    unit = 'm',
                                    is_save = True,
                                    info = 'Vertex coordinate vectors of points. Example with 2 vertex: [[[x11,y11,z11],[x12,y12,z12]],[[x21,y21,z21],[x22,y22,z122]],...]',
                                    ))
                                    
        
                                 
    def get_boundingbox(self):
        vv = self.vertices.value
        # TODOD: can be done faster
        return np.array([   [np.min(vv[:,:,0]),np.min(vv[:,:,1]),np.min(vv[:,:,2])],
                            [np.max(vv[:,:,0]),np.max(vv[:,:,1]),np.max(vv[:,:,2])]
                            ],float)
                                    
    def add_drawobj(self, vertices, color, color_highl = None, is_update = True):
        if color_highl == None:
            colors_highl=self._get_colors_highl(np.array([color]))
            
        _id = self.add_row( vertices=vertices, 
                            colors=color,
                            colors_highl = colors_highl[0],
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return _id
    
        
    def add_drawobjs(self, vertices, colors, colors_highl = None, is_update = True):
        if colors_highl == None:
            colors_highl=self._get_colors_highl(colors)
            
        ids = self.add_rows(len(vertices), 
                            vertices=vertices, 
                            colors=colors,
                            colors_highl = colors_highl,
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return ids
        
    def set_vertices(self, ids, vertices, is_update = True):
        self.vertices[ids] = vertices
        if is_update:
            self._update_vertexvbo()
            
    
                    
    def _update_vertexvbo(self):
        self.get_vbo('line').update_vertices(self.vertices.value.reshape((-1,6)),len(self))
        

    def _make_handlevbo(self,x, y, resolution):
        #print '_get_handlevbo'
        #print '  x =\n',x
        #print '  y =\n',y
        dx = resolution* 5.0
        dy = resolution* 5.0
        z = np.zeros(x.shape,dtype=np.float32)
        n= len(z)
        hvertices = np.concatenate((x-dx,y-dy,z, x+dx,y-dy,z, x+dx,y+dy,z, x-dx,y+dy,z, ),1).reshape(-1,4,3)
        #print '  hvertices =\n',hvertices
        self._vertexvbo_handles = vbo.VBO(hvertices.reshape((-1,3)))
        self._indexvbo_handles = vbo.VBO(np.arange(4*n,dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)        
        
        colors = np.repeat(np.array([[0.9,0.8,0.7,0.5]],dtype=np.float32),n,0)
        self._colorvbo_handles = vbo.VBO(colors[np.array(np.arange(0,n,1.0/4),int)])
        
    
        
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
            
        
        return self._ids[get_dist_point_to_segs(p,x1,y1,x2,y2, is_ending=True) < self.detectwidth**2]

        

        
              
    def draw_old(self, resolution=1.0):
        #print 'draw n=',len(self)
        glLineWidth(self.linewidth.value)

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_BLEND)

        # main drawing
        glEnableClientState(GL_VERTEX_ARRAY)
        glEnableClientState(GL_COLOR_ARRAY)
        
        self._colorvbo.bind()
        glColorPointer(4, GL_FLOAT, 0, None)
        
        self._vertexvbo.bind()
        self._indexvbo.bind()
        glVertexPointer(3, GL_FLOAT, 0, None)
        
        glDrawElements(GL_LINES, self._n_vert*len(self), GL_UNSIGNED_INT, None)

        
        glDisableClientState(GL_VERTEX_ARRAY)
        glDisableClientState(GL_COLOR_ARRAY)
        self._vertexvbo.unbind()
        self._indexvbo.unbind()
        self._colorvbo.unbind()
        
        # draw handles
        if 1:
            vertices = self.vertices.value
            x1 = vertices[:,0,0]
            y1 = vertices[:,0,1]
            
            x2 = vertices[:,1,0]
            y2 = vertices[:,1,1]
            self._make_handlevbo(x2.reshape(-1,1),y2.reshape(-1,1),resolution)
        

            glLineWidth(2)
            glEnableClientState(GL_VERTEX_ARRAY)
            glEnableClientState(GL_COLOR_ARRAY)
            
            self._colorvbo_handles.bind()
            glColorPointer(4, GL_FLOAT, 0, None)
            
            self._vertexvbo_handles.bind()
            self._indexvbo_handles.bind()
            glVertexPointer(3, GL_FLOAT, 0, None)
            
            # GL_LINE_STRIP, GL_QUADS,  GL_LINES,  GL_LINE_LOOP,  GL_POINTS
            glDrawElements( GL_QUADS, 4*len(self), GL_UNSIGNED_INT, None)
            
            glDisableClientState(GL_VERTEX_ARRAY)
            glDisableClientState(GL_COLOR_ARRAY)
            self._vertexvbo_handles.unbind()
            self._indexvbo_handles.unbind()
            self._colorvbo_handles.unbind() 


class Fancylines(Lines):
    def __init__(self,ident,  parent, name = 'Lines',
            is_parentobj = False,
            is_fill = True,
            is_outline = False,
            arrowstretch = 2.5,
            arrowlength = 0.5,
            c_highl=0.3, 
            linewidth=3,
            **kwargs):
        
        self.init_common(   ident,parent=parent, name = name,
                            linewidth = linewidth,# for outline only
                            is_parentobj=is_parentobj, 
                            is_fill = is_fill,
                            is_outline = is_outline,
                            n_vert = 3, # vertex points for ending
                            c_highl=c_highl,
                            **kwargs)
                            
        #if is_outline:
        #    self.add_vbo(Vbo('outline',GL_LINES,2, noncyclic = 0))
     
        if is_fill:
            self.add_vbo(Vbo('line_fill', GL_QUADS,4,objtype='fill'))
            for style in LINEHEADS.keys():
                self.add_vbo(Vbo(('begin','fill',style), GL_TRIANGLES,3,objtype='fill'))
                self.add_vbo(Vbo(('end','fill',style), GL_TRIANGLES,3,objtype='fill'))
            

        
        self.add(cm.AttrConf(  'arrowstretch', arrowstretch,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Arrow stretch', 
                                        info = 'Arrow stretch factor.',
                                        ))
                                                                                                        
        self.add(cm.AttrConf(  'arrowlength', arrowlength,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Arrow length', 
                                        info = 'Arrow length.',
                                        ))
        
        self.add_col(am.ArrayConf(  'beginstyles', 0,
                                        groupnames = ['options'], 
                                        perm='r', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Begin style', 
                                        info = 'Style of the line extremity at the beginning.',
                                        ))
        
        self.add_col(am.ArrayConf(  'endstyles', 0,
                                        groupnames = ['options'], 
                                        perm='r', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'End style', 
                                        info = 'Style of the line extremity at the ending.',
                                        ))
                                                                        
        self.add_col(am.ArrayConf( 'vertices',  np.zeros((2,3), dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    name = 'Vertex',
                                    unit = 'm',
                                    is_save = True,
                                    info = 'Vertex coordinate vectors of points. Example with 2 vertex: [[[x11,y11,z11],[x12,y12,z12]],[[x21,y21,z21],[x22,y22,z122]],...]',
                                    ))
        
                                    
        self.add_col(am.ArrayConf( 'widths',  0.5, 
                                    dtype=np.float32,
                                    groupnames = ['option'], 
                                    perm='rw', 
                                    name = 'Width',
                                    unit = 'm',
                                    is_save = True,
                                    info = 'Line width',
                                    ))
    def add_drawobj(self, vertices, width, color, color_highl = None, is_update = True):
        if color_highl == None:
            color_highl=self._get_colors_highl(np.array([color]))[0]
            
        _id = self.add_row( vertices=vertices, 
                            widths = width,
                            colors=color,
                            colors_highl = color_highl,
                            colors_fill = color,
                            colors_fill_highl = color_highl,
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return _id
    
        
    def add_drawobjs(self, vertices, widths, colors, colors_highl = None, 
                    beginstyles = None, endstyles = None,
                    is_update = True):
                        
        if colors_highl == None:
            colors_highl=self._get_colors_highl(colors)
        
        n = len(vertices) 
        if  beginstyles == None:
            beginstyles = np.zeros(n)
        if  endstyles == None:
            endstyles = np.zeros(n)    
        ids = self.add_rows(len(vertices), 
                            vertices=vertices, 
                            widths = widths,
                            colors=colors,
                            colors_highl = colors_highl,
                            colors_fill=colors,
                            colors_fill_highl = colors_highl,
                            beginstyles = beginstyles,
                            endstyles = endstyles,
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return ids
        
    def set_vertices(self, ids, vertices, is_update = True):
        self.vertices[ids] = vertices
        if is_update:
            self._update_vertexvbo()
            
    
           
           
    def _update_vertexvbo(self):
        #print '_update_vertexvbo',self.format_ident()
        
        
        vertices = self.vertices.value
        x1 = vertices[:,0,0]
        y1 = vertices[:,0,1]
        z1 = vertices[:,0,2]
        
        x2 = vertices[:,1,0]
        y2 = vertices[:,1,1]
        z2 = vertices[:,1,2]
        dx = x2-x1
        dy = y2-y1
        alpha_xy=np.arctan2(dy,dx)
        length_xy = np.sqrt(dx*dx + dy*dy)
        halfwidth = 0.5*self.widths.value
        x1_new = x1.copy()
        y1_new = y1.copy()
        x2_new = x2.copy()
        y2_new = y2.copy()
        #print '  origvert'
        #print '  x1_new=',x1_new,x1_new.dtype
        #print '  x2_new=',x2_new,x2_new.dtype
        if self._is_fill.value:
            for style, id_style in LINEHEADS.iteritems():
                
                # begin
                inds_style = np.flatnonzero(self.beginstyles.value == id_style)
                
                if len(inds_style)>0:
                    
                    #print '     style',style,len(inds_style)
                    #print '         x1_new=',x1_new,x1_new.dtype
                    #print '         x2_new=',x2_new,x2_new.dtype
                    self._update_vertexvbo_begin_fill(style, inds_style, x1_new, y1_new,z1,x2,y2,z2,length_xy, alpha_xy, halfwidth)
                
                # end
                inds_style = np.flatnonzero(self.endstyles.value == id_style)
                if len(inds_style)>0:
                    self._update_vertexvbo_end_fill(style,inds_style,x1,y1,z1,x2_new,y2_new,z2,length_xy, alpha_xy,halfwidth)

                
            self._update_vertexvbo_line_fill(x1_new,y1_new,z1,x2_new,y2_new,z2,length_xy, alpha_xy,halfwidth, len(self))
        
    def _update_vertexvbo_line_fill(self,x1,y1,z1,x2,y2,z2,length_xy, alpha_xy,halfwidth, n):
        #print '_update_vertexvbo_line_fill'
        # this allows different color indexing for polyline
        # TODO n
        #if inds_colors == None:
        #n = len(self)
        #else:
        #    n = None
            
        #n_vert = self.get_n_vert()
        #dphi = np.pi/(n_vert-1)
        
        #beta = alpha_xy+np.pi/2 
        #print '  alpha_xy\n',alpha_xy/np.pi*180
        #print '  halfwidth\n',halfwidth
        #print '  x1\n',x1
        #print '  length_xy=',length_xy
        #print '  length_xy-self.widths.value=',(length_xy-self.widths.value)
        
        
        xr1 =  x1 + halfwidth * np.cos(alpha_xy+np.pi/2)
        yr1 =  y1 + halfwidth * np.sin(alpha_xy+np.pi/2)
        
        xr2 =  x2 + halfwidth * np.cos(alpha_xy+np.pi/2)
        yr2 =  y2 + halfwidth * np.sin(alpha_xy+np.pi/2)
        
        xr3 =  x2 + halfwidth * np.cos(alpha_xy-np.pi/2)
        yr3 =  y2 + halfwidth * np.sin(alpha_xy-np.pi/2)
        
        xr4 =  x1 + halfwidth * np.cos(alpha_xy-np.pi/2)
        yr4 =  y1 + halfwidth * np.sin(alpha_xy-np.pi/2)
        
        
        
        
        


        #print '_update_vertexvbo ',n,n_vert,self._is_fill.value
        #print '  x\n',x
        #print '  y\n',y
        #print '  z\n',z
        n_vpe = 4
        vertices = np.zeros((len(xr1), n_vpe * 3),dtype=np.float32)
        
        vertices[:,0]=xr1 
        vertices[:,1]=yr1
        vertices[:,2]=z1
        
        vertices[:,3]=xr2 
        vertices[:,4]=yr2 
        vertices[:,5]=z2
        
        vertices[:,6]=xr3 
        vertices[:,7]=yr3
        vertices[:,8]=z2
        
        vertices[:,9]=xr4 
        vertices[:,10]=yr4 
        vertices[:,11]=z1
        
        #print '  vertices=\n',vertices
        self.get_vbo('line_fill').update_vertices(vertices,n)
        
        #self._vertexvbo_fill = vbo.VBO(vertices.reshape((-1,3)))
        #self._indexvbo_fill = vbo.VBO(np.arange(self._n_vert*self._n_elem_fill, dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)
    
        
        
    def _update_vertexvbo_begin_fill(self, style, inds_style, x1_new, y1_new,z1,x2,y2,z2, length_xy, alpha_xy, halfwidth):
        ident_vbo = ('begin','fill',style)
        headvbo = self.get_vbo(ident_vbo)
        #print '_update_vertexvbo_begin_fill'
        
                            
        n=len(inds_style)
        alphastretch = 1.0
        n_vert = 6
        arrowstretch = 1.0
        if style =='bevel':
            n_vert = 6
            
        if style =='triangle':
            n_vert = 3
            arrowstretch = self.arrowstretch.value
            
        if style =='arrow':
            n_vert = 3
            alphastretch = 1.2
            arrowstretch = self.arrowstretch.value
        radius = arrowstretch*halfwidth[inds_style]
        if style !='bevel':
            x1_new[inds_style] = x2[inds_style] -(length_xy[inds_style]-radius)*np.cos(alpha_xy[inds_style])
            y1_new[inds_style] = y2[inds_style] -(length_xy[inds_style]-radius)*np.sin(alpha_xy[inds_style])
        
        #print '         x1_new=',x1_new,x1_new.dtype,halfwidth[inds_style]
        #print '         y1_new=',y1_new,y1_new.dtype
                
        dphi = np.pi/(n_vert-1)
        
        alphas = alpha_xy[inds_style]+(alphastretch*np.arange(-np.pi/2,np.pi/2+dphi,dphi)+np.pi).reshape(-1,1)
        #print '  alpha_xy\n',alpha_xy/np.pi*180
        #print '  alphas=alpha0+phi\n',alphas/np.pi*180
        

        x = np.cos(alphas) * radius + x1_new[inds_style]
        y = np.sin(alphas) * radius + y1_new[inds_style]
        
        z = np.ones((n_vert,1)) *z1[inds_style]
        
        #print '_update_vertexvbo ',n,n_vert,self._is_fill.value
        #print '  x\n',x
        #print '  y\n',y
        #print '  z\n',z
        
        #print '_update_vertexvbo_fill',n,len(x),n_vert,self._n_vpe_fill
        xf = x.transpose().flatten()#+0.5
        yf = y.transpose().flatten()#+0.5
        zf = z.transpose().flatten()
        xcf = (np.ones((n_vert,1)) * x1_new[inds_style]).transpose().flatten()
        ycf = (np.ones((n_vert,1)) * y1_new[inds_style]).transpose().flatten()
        #print '  xcf\n',xcf
        #print '  ycf\n',ycf
    
        
            
        # create and compose main vertices array
        n_elem_fill = (n_vert-1)*n
        
        n_vpe = headvbo.get_vpe()
        #print '  n_elem_fill ',self._n_elem_fill
        #print '  n_vpe',n_vpe
        vertices = np.zeros((n_elem_fill, n_vpe * 3),dtype=np.float32)
        #print '  vertices.reshape((-1,3)).shape',vertices.reshape((-1,3)).shape
        #zf = z.transpose().flatten()
        
        
            
        ind=np.arange(0,n*n_vert).reshape(n,n_vert)[:,:-1].flatten()
        
        vertices[:,0]=xf[ind]
        vertices[:,1]=yf[ind]
        vertices[:,2]=zf[ind]
        
        vertices[:,3]=xcf[ind]
        vertices[:,4]=ycf[ind]
        vertices[:,5]=zf[ind]
        
        ind2=np.arange(0,n*n_vert).reshape(n,n_vert)[:,1:].flatten()
        vertices[:,6]=xf[ind2]
        vertices[:,7]=yf[ind2]
        vertices[:,8]=zf[ind2]
        #print '  vertices=\n',vertices
        headvbo.update_vertices(vertices,inds = inds_style)
        
        #self._vertexvbo_fill = vbo.VBO(vertices.reshape((-1,3)))
        #self._indexvbo_fill = vbo.VBO(np.arange(self._n_vert*self._n_elem_fill, dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)
    
        
    def _update_vertexvbo_end_fill(self,style,inds_style,x1,y1,z1,x2_new,y2_new,z2,length_xy, alpha_xy,halfwidth):
        ident_vbo = ('end','fill',style)
        headvbo = self.get_vbo(ident_vbo)
        #print '_update_vertexvbo_begin_fill'
        
                            
        n=len(inds_style)
        alphastretch = 1.0
        n_vert = 6
        arrowstretch = 1.0
        if style =='bevel':
            n_vert = 6
            
        if style =='triangle':
            n_vert = 3
            arrowstretch = self.arrowstretch.value
            
        if style =='arrow':
            n_vert = 3
            alphastretch = 1.2
            arrowstretch = self.arrowstretch.value
        radius = arrowstretch*halfwidth[inds_style]
        
        if style !='bevel':
            x2_new[inds_style] = x1[inds_style] +(length_xy[inds_style]-radius)*np.cos(alpha_xy[inds_style])
            y2_new[inds_style] = y1[inds_style] +(length_xy[inds_style]-radius)*np.sin(alpha_xy[inds_style])
            

        dphi = np.pi/(n_vert-1)
        
        alphas = alpha_xy[inds_style]+alphastretch*np.arange(-np.pi/2,np.pi/2+dphi,dphi).reshape(-1,1)
        #print '  alpha_xy\n',alpha_xy/np.pi*180
        #print '  alphas=alpha0+phi\n',alphas/np.pi*180
        
        #print '  length_xy=',length_xy
        #print '  length_xy-self.widths.value=',(length_xy-self.widths.value)
        #print '  x2=',x2,x2.dtype
        
        #print '  y2_new=',y2_new,y2_new.dtype
        x = np.cos(alphas) * radius + x2_new[inds_style]
        y = np.sin(alphas) * radius + y2_new[inds_style]
        
        z = np.ones((n_vert,1)) *z2[inds_style]
        
        #print '_update_vertexvbo ',n,n_vert,self._is_fill.value
        #print '  x\n',x
        #print '  y\n',y
        #print '  z\n',z
        
        #print '_update_vertexvbo_fill',n,len(x),n_vert,self._n_vpe_fill
        xf = x.transpose().flatten()#+0.5
        yf = y.transpose().flatten()#+0.5
        zf = z.transpose().flatten()
        xcf = (np.ones((n_vert,1)) * x2_new[inds_style]).transpose().flatten()
        ycf = (np.ones((n_vert,1)) * y2_new[inds_style]).transpose().flatten()
        #print '  xcf\n',xcf
        #print '  ycf\n',ycf
    
        
            
        # create and compose main vertices array
        n_elem_fill = (n_vert-1)*n
        
        n_vpe = headvbo.get_vpe()
        #print '  n_elem_fill ',self._n_elem_fill
        #print '  n_vpe',n_vpe
        vertices = np.zeros((n_elem_fill, n_vpe * 3),dtype=np.float32)
        #print '  vertices.reshape((-1,3)).shape',vertices.reshape((-1,3)).shape
        #zf = z.transpose().flatten()
        
        
            
        ind=np.arange(0,n*n_vert).reshape(n,n_vert)[:,:-1].flatten()
        
        vertices[:,0]=xf[ind]
        vertices[:,1]=yf[ind]
        vertices[:,2]=zf[ind]
        
        vertices[:,3]=xcf[ind]
        vertices[:,4]=ycf[ind]
        vertices[:,5]=zf[ind]
        
        ind2=np.arange(0,n*n_vert).reshape(n,n_vert)[:,1:].flatten()
        vertices[:,6]=xf[ind2]
        vertices[:,7]=yf[ind2]
        vertices[:,8]=zf[ind2]
        #print '  vertices=\n',vertices
        headvbo.update_vertices(vertices,inds = inds_style)
        
        #self._vertexvbo_fill = vbo.VBO(vertices.reshape((-1,3)))
        #self._indexvbo_fill = vbo.VBO(np.arange(self._n_vert*self._n_elem_fill, dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)
    
    
                    
     
                

    def pick(self,p):
        """
        Returns a binary vector which is True values for lines that have been selected 
        by point p.
        
        In particular, an element of this vector is True if the minimum distance 
        between the respective line to point p is less than self.detectwidth
        """
        #print 'pick'
        vertices = self.vertices.value
        x1 = vertices[:,0,0]
        y1 = vertices[:,0,1]
        
        x2 = vertices[:,1,0]
        y2 = vertices[:,1,1]
        
        #print '  x1', x1   
        #print '  x2', x2   
        return self._ids[get_dist_point_to_segs(p,x1,y1,x2,y2, is_ending=True) < 0.25*self.widths.value*self.widths.value]

        
    
    
            

class Polylines(Fancylines):
    def __init__(self,ident,  parent, name = 'Polylines', joinstyle = FLATHEAD, **kwargs):
        Fancylines.__init__(self, ident,  parent, name = name,**kwargs)
        
        # FLATHEAD = 0             
        # BEVELHEAD = 1
        self.add(cm.AttrConf(  'joinstyle', joinstyle,
                                        groupnames = ['options'], 
                                        perm='wr', 
                                        is_save = True,
                                        is_plugin = False,
                                        name = 'Joinstyle', 
                                        info = 'Joinstyle of line segments.',
                                        ))
                                        
        self.delete('vertices')
        self.add_col(am.ArrayConf( 'vertices',  None,
                                    dtype=np.object,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    name = 'Vertex',
                                    unit = 'm',
                                    is_save = True,
                                    info = 'Vertex coordinate vectors of points. Example with 2 vertex: [[[x11,y11,z11],[x12,y12,z12]],[[x21,y21,z21],[x22,y22,z122]],...]',
                                    ))
    
    def _make_lines(self):
        # from   test_fancyline.py
        #print 'make_linevertices',len(self.vertices.value)
        linevertices = np.zeros((0,2,3),np.float32)
        polyinds = []
        lineinds = []
        #linecolors = []
        #linecolors_highl = []
        linebeginstyles = []
        lineendstyles = []
        
        #colors = self.colors_fill.value
        #colors_highl = self.colors_fill_highl.value
        beginstyles = self.beginstyles.value
        endstyles = self.endstyles.value
        joinstyle = self.joinstyle.value
        ind = 0
        ind_line = 0
        for polyline in self.vertices.value:
            #print '  ======='
            #print '  polyline\n',polyline
            # Important type conversion!!
            v = np.zeros(  ( 2*len(polyline)-2,3),np.float32)
            v[0]=polyline[0]
            v[-1]=polyline[-1]
            if len(v)>2:
                
                #print 'v[1:-1]',v[1:-1]
                #print 'v=\n',v
                #m = np.repeat(polyline[1:-1],2,0) 
                #print 'm\n',m,m.shape,m.dtype
                #v[1:-1] = m
                v[1:-1] = np.repeat(polyline[1:-1],2,0) 
            #vadd = v.reshape((-1,2,3))
            #print 'vadd\n',vadd
            n_lines = len(v)/2
            #print '  v\n',v
            polyinds += n_lines*[ind]
            lineinds.append(np.arange(ind_line,ind_line+n_lines))
            ind_line += n_lines
            #print '  polyinds\n',polyinds,n_lines
            #linecolors += n_lines*[colors[ind]]
            #linecolors_highl += n_lines*[colors_highl[ind]]
            linebeginstyle = n_lines*[joinstyle]
            linebeginstyle[0] = beginstyles[ind]
            linebeginstyles += linebeginstyle
            #print '  linebeginstyle',linebeginstyle,beginstyles[ind]
            
            lineendstyle = n_lines*[joinstyle]
            lineendstyle[-1] = endstyles[ind]
            lineendstyles += lineendstyle
            
            linevertices = np.concatenate((linevertices, v.reshape((-1,2,3))))
            #print '  linevertex\n',linevertices
            ind +=1
                
        self._linevertices = linevertices
        #self._linecolors = np.array(linecolors, np.float32)
        #self._linecolors_highl = np.array(linecolors_highl, np.float32)
        self._polyinds = np.array(polyinds, np.int32)
        self._lineinds = lineinds
        #print '  lineinds',lineinds
        self._linebeginstyles =  np.array(linebeginstyles, np.int32)
        self._lineendstyles =  np.array(lineendstyles, np.int32)
        #print '  self._linebeginstyles',self._linebeginstyles
        #print '  self._lineendstyles',self._lineendstyles
        
    def _update_vertexvbo(self):
        #print '_update_vertexvbo',self.format_ident()
        self._make_lines()
        vertices = self._linevertices
        x1 = vertices[:,0,0]
        y1 = vertices[:,0,1]
        z1 = vertices[:,0,2]
        
        x2 = vertices[:,1,0]
        y2 = vertices[:,1,1]
        z2 = vertices[:,1,2]
        dx = x2-x1
        dy = y2-y1
        alpha_xy=np.arctan2(dy,dx)
        length_xy = np.sqrt(dx*dx + dy*dy)
        halfwidth = 0.5*self.widths.value[self._polyinds]
        x1_new = x1.copy()
        y1_new = y1.copy()
        x2_new = x2.copy()
        y2_new = y2.copy()
        #print '  origvert'
        #print '  x1_new=',x1_new,x1_new.dtype
        #print '  x2_new=',x2_new,x2_new.dtype
        if self._is_fill.value:
            for style, id_style in LINEHEADS.iteritems():
                
                # begin
                inds_style = np.flatnonzero(self._linebeginstyles == id_style)
                
                if len(inds_style)>0:
                    
                    #print '     style',style,len(inds_style)
                    #print '         x1_new=',x1_new,x1_new.dtype
                    #print '         x2_new=',x2_new,x2_new.dtype
                    self._update_vertexvbo_begin_fill(style, inds_style, x1_new, y1_new,z1,x2,y2,z2,length_xy, alpha_xy, halfwidth)
                
                # end
                inds_style = np.flatnonzero(self._lineendstyles == id_style)
                if len(inds_style)>0:
                    self._update_vertexvbo_end_fill(style,inds_style,x1,y1,z1,x2_new,y2_new,z2,length_xy, alpha_xy,halfwidth)

                
            self._update_vertexvbo_line_fill(x1_new,y1_new,z1,x2_new,y2_new,z2,length_xy, alpha_xy,halfwidth,len(vertices))
    
    def _update_colorvbo(self):
        n = len(self._polyinds)
        #n_vert = self.get_n_vert()
        #print '_update_colorvbo fancyline'
        #self._linecolors = np.array(linecolors, np.float32)
        #self._linecolors_highl = np.array(linecolors_highl, np.float32)
        if self._is_outline.value:
            colors = self.colors.value[self._polyinds]  + self.are_highlighted.value[self._polyinds].reshape(n,1)*self.colors_highl.value[self._polyinds]
            for _vbo in self.get_vbos():
                if not _vbo.is_fill():
                    _vbo.update_colors(colors)
                    
        if self._is_fill.value:
            colors = self.colors_fill.value[self._polyinds]  + self.are_highlighted.value[self._polyinds].reshape(n,1)*self.colors_fill_highl.value[self._polyinds]
            for _vbo in self.get_vbos():
                if _vbo.is_fill():
                    _vbo.update_colors(colors)
                    
    def pick(self,p):
        """
        Returns a binary vector which is True values for lines that have been selected 
        by point p.
        
        In particular, an element of this vector is True if the minimum distance 
        between the respective line to point p is less than self.detectwidth
        """
        #print 'pick'
        vertices = self._linevertices
        x1 = vertices[:,0,0]
        y1 = vertices[:,0,1]
        
        x2 = vertices[:,1,0]
        y2 = vertices[:,1,1]
        
        #print '  x1', x1   
        #print '  x2', x2   
        halfwidths = 0.5*self.widths.value[self._polyinds]
        return self._ids[self._polyinds[get_dist_point_to_segs(p,x1,y1,x2,y2, is_ending=True) < halfwidths*halfwidths]]


    def get_boundingbox(self):
        vv = self._linevertices
        # TODOD: can be done faster
        return np.array([   [np.min(vv[:,:,0]),np.min(vv[:,:,1]),np.min(vv[:,:,2])],
                            [np.max(vv[:,:,0]),np.max(vv[:,:,1]),np.max(vv[:,:,2])]
                            ],float)
                          



                                    
class Circles(DrawobjMixin):
    
    def __init__(self,ident,  parent, name = 'Circles',
            is_parentobj = False,
            is_fill = True, # Fill objects,
            is_outline = True, # show outlines
            c_highl=0.3, 
            n_vert =7, # default number of vertex per circle
            linewidth=2, 
            **kwargs):
        
        
        

        self.init_common(   ident, parent = parent, name = name,
                            linewidth = linewidth,
                            is_parentobj = is_parentobj, 
                            n_vert = n_vert, 
                            is_fill = is_fill,
                            is_outline = is_outline, #
                            c_highl=c_highl,
                            **kwargs)
                            
        if is_outline:
            self.add_vbo(Vbo('outline',GL_LINES,2,objtype='outline'))
        if is_fill:
            self.add_vbo(Vbo('fill',GL_TRIANGLES,3,objtype='fill'))
       
        
        self.add_col(am.ArrayConf( 'centers',  np.zeros(3, dtype=np.float32),
                                    dtype=np.float32,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    name = 'Center',
                                    unit = 'm',
                                    is_save = True,
                                    info = 'Center coordinate. Example: [x,y,z]',
                                    ))
                                    
        self.add_col(am.ArrayConf( 'radii',  1.0, 
                                    dtype=np.float32,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    name = 'Radius',
                                    is_save = True,
                                    info = 'Circle radius',
                                    ))
                                    
        
                                        
    def get_boundingbox(self):
        vv = self.centers.value
        rad = self.radii.value
        # TODOD: can be done faster
        return np.array([   [np.min(vv[:,0]-rad),np.min(vv[:,1]-rad),np.min(vv[:,2])],
                            [np.max(vv[:,0]+rad),np.max(vv[:,1]+rad),np.max(vv[:,2])]
                            ],float)
                                    
    
    def add_drawobj(self, center, radius, 
                        color, color_fill = None, 
                        color_highl = None, color_fill_highl = None,
                        is_update = True):
        
        if color_fill == None:
            color_fill = color
            
        if color_highl == None:
            color_highl=self._get_colors_highl(np.array([color]))[0]
        
        if color_fill_highl == None:
            color_fill_highl=self._get_colors_highl(np.array([color_fill]))[0]
                
        _id = self.add_row( centers=center, 
                            radii = radius,
                            colors=color,
                            colors_highl = color_highl,
                            colors_fill = color_fill,
                            colors_fill_highl = color_fill_highl,
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return _id
    
        
    def add_drawobjs(   self, centers, radii, 
                        colors, colors_fill = None, 
                        colors_highl = None, colors_fill_highl = None,
                        is_update = True):
        
        if colors_fill == None:
            colors_fill = colors
            
        if colors_highl == None:
            colors_highl = self._get_colors_highl(colors)
        
        if colors_fill_highl == None:
            colors_fill_highl = self._get_colors_highl(colors_fill)
                                
        

        ids = self.add_rows(len(centers), 
                            centers=centers, 
                            radii = radii,
                            colors=colors,
                            colors_highl = colors_highl,
                            colors_fill = colors_fill,
                            colors_fill_highl = colors_fill_highl,
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return ids
        
    
    def _update_colorvbo(self):
        n=len(self)
        #n_vert = self.get_n_vert()
        #print '_update_colorvbo'
        
        
        #print '   len(colors)',self._n_vpe*n*(n_vert-1)
        #for _vbo in self.get_vbos():
        #    _vbo.update_colors()
        
        
            
        if self._is_outline.value:
            #print '  outline',self._n_vpe, n ,n_vert,len(self._vertexvbo),self._n_elem
            self.get_vbo('outline').update_colors(self.colors.value + self.are_highlighted.value.reshape(n,1)*self.colors_highl.value)

        if  self._is_fill.value:
            self.get_vbo('fill').update_colors(self.colors_fill.value + self.are_highlighted.value.reshape(n,1)*self.colors_fill_highl.value)
            #print '  filllll',self._n_vpe_fill, n ,n_vert,len(self._vertexvbo_fill),self._n_elem_fill
        
        #    n_repreat = len(self._vertexvbo_fill)/n
        #    self._colorvbo_fill = vbo.VBO( np.repeat((self.colors_fill.value + self.are_highlighted.value.reshape(n,1)*self.colors_fill_highl.value),n_repreat,0))
        #    #n_elem_fill = (n_vert-1)*n 
        #    # len(_vertexvbo_fill) =  self._n_vpe * self._n_elem_fill =  self._n_vpe * (n_vert-1)*n                
       
               
    def _update_vertexvbo(self):
        
        n=len(self)
        n_vert = self.get_n_vert()
        
        
        #print '_update_vertexvbo ',n,n_vert,self._is_fill.value

        # compute x,y,z of vertices of n objects
        dphi = 2*np.pi/(n_vert-1)
        phi = np.arange(0,2*np.pi+dphi,dphi).reshape(-1,1)
        
        x = np.cos(phi)*self.radii.value+self.centers.value[:,0]

        y = np.sin(phi)*self.radii.value+self.centers.value[:,1]
        
        z = np.ones((n_vert,1)) *self.centers.value[:,2]
        #print '  x.shape=', x.shape
        if self._is_outline.value:
            self._update_vertexvbo_lines(x,y,z)
            
        if self._is_fill.value:
            self._update_vertexvbo_fill(x,y,z)
        
        
    def _update_vertexvbo_fill(self,x,y,z):
        n=len(self)
        n_vert = self.get_n_vert()
        #print '_update_vertexvbo_fill',n,len(x),n_vert#,self._n_vpe_fill
        xf = x.transpose().flatten()#+0.5
        yf = y.transpose().flatten()#+0.5
        zf = z.transpose().flatten()
        xcf = (np.ones((n_vert,1)) *self.centers.value[:,0]).transpose().flatten()
        ycf = (np.ones((n_vert,1)) *self.centers.value[:,1]).transpose().flatten()
        
        # create and compose main vertices array
        n_elem_fill = (n_vert-1)*n
        
        #glDrawElements(GL_TRIANGLES, self._n_vert*self._n_elem_fill, GL_UNSIGNED_INT, None)
        #  self._n_vert*self._n_elem_fill = n_vert * (n_vert-1)*n
        #repeat for each of the n objects:  self._n_vpe_fill*(n_vert-1)
        #self._n_vpe * self._n_elem_fill =  self._n_vpe * (n_vert-1)*n
        
        
        n_vpe_fill = self.get_vbo('fill').get_vpe()
        #print '  n_elem_fill',n_elem_fill,n_vpe_fill
        vertices = np.zeros((n_elem_fill, n_vpe_fill * 3),dtype=np.float32)
        #print '  vertices.reshape((-1,3)).shape',vertices.reshape((-1,3)).shape
        zf = z.transpose().flatten()
        
        
            
        ind=np.arange(0,n*n_vert).reshape(n,n_vert)[:,:-1].flatten()
        
        vertices[:,0]=xf[ind]
        vertices[:,1]=yf[ind]
        vertices[:,2]=zf[ind]
        
        vertices[:,3]=xcf[ind]
        vertices[:,4]=ycf[ind]
        vertices[:,5]=zf[ind]
        
        ind2=np.arange(0,n*n_vert).reshape(n,n_vert)[:,1:].flatten()
        vertices[:,6]=xf[ind2]
        vertices[:,7]=yf[ind2]
        vertices[:,8]=zf[ind2]
        
        self.get_vbo('fill').update_vertices(vertices,n)
        #self._vertexvbo_fill = vbo.VBO(vertices.reshape((-1,3)))
        #self._indexvbo_fill = vbo.VBO(np.arange(self._n_vert*self._n_elem_fill, dtype=np.int32), target=GL_ELEMENT_ARRAY_BUFFER)
    
        
    def _update_vertexvbo_lines(self,x,y,z):
        #print '_update_vertexvbo_lines',len(x)
        n=len(self)
        n_vert = self.get_n_vert()
            
        # create and compose main vertices array
        n_elem = (n_vert-1)*n
        
        
        n_vpe = 2 # vertices per OGL element
            
        vertices = np.zeros((n_elem, n_vpe * 3),dtype=np.float32)
        
        #print '  vertices.reshape((-1,3)).shape',vertices.reshape((-1,3)).shape
        
        xf = x.transpose().flatten()
        yf = y.transpose().flatten()
        zf = z.transpose().flatten()
        
        
            
        ind=np.arange(0,n*n_vert).reshape(n,n_vert)[:,:-1].flatten()
        
        vertices[:,0]=xf[ind]
        vertices[:,1]=yf[ind]
        vertices[:,2]=zf[ind]
        
        ind2=np.arange(0,n*n_vert).reshape(n,n_vert)[:,1:].flatten()
        vertices[:,3]=xf[ind2]
        vertices[:,4]=yf[ind2]
        vertices[:,5]=zf[ind2]
        
        self.get_vbo('outline').update_vertices(vertices,n)

    
        
        
    
    def pick(self,p):
        """
        Returns a binary vector which is True values for circles that have been selected 
        by point p.
        
        In particular, an element is selected if point p is within the circle
        """
        centers = self.centers.value
        x1 = centers[:,0]
        y1 = centers[:,1]
        return self._ids[ (x1-p[0])**2+(y1-p[1])**2 < self.radii.value**2]


              
    
        
class Polygons(DrawobjMixin):
    def __init__(self,ident,  parent, name = 'Polygons', detectwidth = 0.1,**kwargs):
        self.init_common(   ident, parent = parent, name = name,
                            is_fill = False,
                            is_outline = True, #
                            **kwargs)
        self.detectwidth = detectwidth                    
        if self._is_outline:
            self.add_vbo(Vbo('outline',GL_LINES,2,objtype='outline'))
        if self._is_fill:
            self.add_vbo(Vbo('fill',GL_TRIANGLES,3,objtype='fill'))
        


                                        
        self.delete('vertices')
        self.add_col(am.ListArrayConf( 'vertices',  
                                    #None,
                                    #dtype=np.object,
                                    groupnames = ['_private'], 
                                    perm='rw', 
                                    name = 'Vertex',
                                    unit = 'm',
                                    is_save = True,
                                    info = '3D coordinate list of Polygon Points.',
                                    ))    
    
    def add_drawobj(self, vertex,
                        color, color_fill = None, 
                        color_highl = None, color_fill_highl = None,
                        is_update = True):
        
        if color_fill == None:
            color_fill = color
            
        if color_highl == None:
            color_highl=self._get_colors_highl(np.array([color]))[0]
        
        if color_fill_highl == None:
            color_fill_highl=self._get_colors_highl(np.array([color_fill]))[0]
                
        _id = self.add_row( vertices = vertex, 
                            colors=color,
                            colors_highl = color_highl,
                            colors_fill = color_fill,
                            colors_fill_highl = color_fill_highl,
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return _id
    
        
    def add_drawobjs(   self, vertices, 
                        colors, colors_fill = None, 
                        colors_highl = None, colors_fill_highl = None,
                        is_update = True):
        
        if colors_fill == None:
            colors_fill = colors
            
        if colors_highl == None:
            colors_highl = self._get_colors_highl(colors)
        
        if colors_fill_highl == None:
            colors_fill_highl = self._get_colors_highl(colors_fill)
                                
        

        ids = self.add_rows(len(vertices), 
                            vertices = vertices,
                            colors=colors,
                            colors_highl = colors_highl,
                            colors_fill = colors_fill,
                            colors_fill_highl = colors_fill_highl,
                            )
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
        return ids
        
    def _make_lines(self):
        #print 'make_linevertices',len(self.vertices.value)
        linevertices = np.zeros((0,2,3),np.float32)
        polyinds = []
        lineinds = []

        ind = 0
        ind_line = 0
        for polyline in self.vertices.value:
            #print '  ======='
            #print '  polyline\n',polyline
            # Important type conversion!!
            v = np.zeros(  ( 2*len(polyline),3),np.float32)
            v[0]=polyline[0]
            v[-2]=polyline[-1]
            v[-1]=polyline[0]
            #print '  v\n',v
            if len(v)>3:
                
                #print 'v[1:-1]',v[1:-1]
                #print 'v=\n',v
                #m = np.repeat(polyline[1:-1],2,0) 
                #print 'm\n',m,m.shape,m.dtype
                #v[1:-1] = m
                v[1:-1] = np.repeat(polyline[1:],2,0)  
            #vadd = v.reshape((-1,2,3))
            #print 'vadd\n',vadd
            n_lines = len(v)/2
            #print '  v\n',v
            polyinds += n_lines*[ind]
            lineinds.append(np.arange(ind_line,ind_line+n_lines))
            ind_line += n_lines
            #print '  polyinds\n',polyinds,n_lines

            linevertices = np.concatenate((linevertices, v.reshape((-1,2,3))))
            #print '  linevertex\n',linevertices
            ind +=1
                
        self._linevertices = linevertices
        self._polyinds = np.array(polyinds, np.int32)
        self._lineinds = lineinds
        #print '  lineinds=\n',lineinds
        
    def _update_vertexvbo(self):
        #print '_update_vertexvbo',self.format_ident()
        self._make_lines()

        if self._is_outline.value:
            #print '  linevertices.reshape((-1,6))',self._linevertices.reshape((-1,6)),len(self._linevertices)
            self.get_vbo('outline').update_vertices(self._linevertices.reshape((-1,6)),len(self._linevertices))
            #self._update_vertexvbo_line_fill(x1_new,y1_new,z1,x2_new,y2_new,z2,length_xy, alpha_xy,halfwidth,len(vertices))
    
        #if self._is_fill.value:
        #    self._update_vertexvbo_line_fill(x1_new,y1_new,z1,x2_new,y2_new,z2,length_xy, alpha_xy,halfwidth,len(vertices))
    
    def _update_colorvbo(self):
        n = len(self._polyinds)
        #n_vert = self.get_n_vert()
        #print '_update_colorvbo fancyline'
        #self._linecolors = np.array(linecolors, np.float32)
        #self._linecolors_highl = np.array(linecolors_highl, np.float32)
        if self._is_outline.value:
            colors = self.colors.value[self._polyinds]  + self.are_highlighted.value[self._polyinds].reshape(n,1)*self.colors_highl.value[self._polyinds]
            self.get_vbo('outline').update_colors(colors)
                    
        #if self._is_fill.value:
        #    colors = self.colors_fill.value[self._polyinds]  + self.are_highlighted.value[self._polyinds].reshape(n,1)*self.colors_fill_highl.value[self._polyinds]
        #    self.get_vbo('fill').update_colors(colors)
    
    def pick(self,p):
        """
        Returns a binary vector which is True values for lines that have been selected 
        by point p.
        
        In particular, an element of this vector is True if the minimum distance 
        between the respective line to point p is less than self.detectwidth
        """
        vertices = self._linevertices
        x1 = vertices[:,0,0]
        y1 = vertices[:,0,1]
        
        x2 = vertices[:,1,0]
        y2 = vertices[:,1,1]
            
        
        return self._ids[self._polyinds[get_dist_point_to_segs(p,x1,y1,x2,y2, is_ending=True) < self.detectwidth**2]]

    def get_boundingbox(self):
        vv = self._linevertices
        # TODOD: can be done faster
        return np.array([   [np.min(vv[:,:,0]),np.min(vv[:,:,1]),np.min(vv[:,:,2])],
                            [np.max(vv[:,:,0]),np.max(vv[:,:,1]),np.max(vv[:,:,2])]
                            ],float)            
    
    
   
class OGLdrawing(am.ArrayObjman):
    """
    Class holding an ordered list of all OGL draw objects.
    This class manages also the order in which the draw objects are rendered. 
    
    The basic idea is that an instance of this class can be passed to
    different OGL rendering canvas.
    """ 
    
    def __init__(self, ident = 'drawing', parent=None, name = 'OGL drawing',info = 'List of OGL draw objects'):
        """
        Holds all drawing objects and provides basic access.
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

class OGLnavcanvas(wx.Panel):
    """
    Canvas with some navigation tools.
    """
    def __init__(self,parent, 
                    mainframe = None,
                    size = wx.DefaultSize,
                    ):
        
        
        
        
        wx.Panel.__init__(self, parent, wx.ID_ANY,size = size)
        sizer=wx.BoxSizer(wx.VERTICAL)
        
        
        
        
        # initialize GL canvas
        self._canvas = OGLcanvas(self, mainframe)
        
        # navigation bar
        self._navbar =  self.make_navbar()
        
        
        
        # compose navcanvas  window
        sizer.Add(self._canvas,1,wx.GROW)# 
        sizer.Add(self._navbar,0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)# from NaviPanelTest
        # finish panel setup
        self.SetSizer(sizer)
        sizer.Fit(self)
   
    def get_canvas(self):     
        return self._canvas
    
    def make_navbar(self):
        """
        Initialize toolbar which consist of navigation buttons
        """
        bottonsize = (32,32)
        bottonborder = 10
        toolbarborder = 1
        
        # toolbar
        navbar = wx.BoxSizer(wx.HORIZONTAL)
        
        
        b=wx.Button(self, wx.ID_ZOOM_FIT)
        b.SetToolTipString("Zoom to fit")
        self.Bind(wx.EVT_BUTTON, self.on_zoom_tofit, b)
        navbar.Add(b, flag=wx.ALL, border=toolbarborder)
        
        b=wx.Button(self, wx.wx.ID_ZOOM_IN)
        b.SetToolTipString("Zoom in")
        self.Bind(wx.EVT_BUTTON, self.on_zoom_in, b)
        navbar.Add(b, flag=wx.ALL, border=toolbarborder)
        
        b=wx.Button(self, wx.wx.ID_ZOOM_OUT)
        b.SetToolTipString("Zoom out")
        self.Bind(wx.EVT_BUTTON, self.on_zoom_out, b)
        navbar.Add(b, flag=wx.ALL, border=toolbarborder)
        
        #b=wx.Button(self, wx.wx.ID_ZOOM_IN)
        #b.SetToolTipString("Zoom in")
        #self.Bind(wx.EVT_BUTTON, self.on_test, b)
        #navbar.Add(b, flag=wx.ALL, border=toolbarborder)
        
        
        #bitmap = wx.ArtProvider_GetBitmap(wx.ART_GO_FORWARD,wx.ART_TOOLBAR)
        #b = wx.BitmapButton(self, -1, bitmap, bottonsize,
        #               (bitmap.GetWidth()+bottonborder, bitmap.GetHeight()+bottonborder))
        #b.SetToolTipString("Go forward in browser history.")
        #self.Bind(wx.EVT_BUTTON, self.on_test, b)
        #navbar.Add(b,0, wx.EXPAND, border=toolbarborder)
        #bottons.append(b)
        
        
        
        #self.add_tool(
        #    'home',self.on_go_home,
        #    wx.ArtProvider.GetBitmap(wx.ART_GO_HOME, wx.ART_TOOLBAR, tsize),
        #    'show panel of root instance') 
        
        
        #self.path=wx.TextCtrl(self, -1,'Try me', style=wx.TE_RIGHT)#size=(-1, -1))#,size=(300, -1))
        #self.path.Bind(wx.EVT_CHAR, self.on_test)
        #navbar.Add(self.path,1, wx.EXPAND, border=toolbarborder)
        
        
        #self.toolbar.AddSeparator()
        
        #panel.SetAutoLayout(True)
        #panel.SetSizer(buttons)
        
        navbar.Fit(self)
        return navbar
    
    def on_zoom_tofit(self, event = None):
        self._canvas.zoom_tofit()
        if event:
            event.Skip()
            
    def on_zoom_out(self, event = None):
        self._canvas.zoom_out()
        if event:
            event.Skip()    
            
    def on_zoom_in(self, event = None):
        self._canvas.zoom_in()    
        if event:
            event.Skip()
            
    def on_test(self, event = None):
        print 'this is a test'
                 
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

        self.g_nearPlane = 10.
        self.g_farPlane = 10000.

        self.action = ""
        self.pos_start=(0.0,0.0) 
        self.trans_start=(0.0,0.0)
        
        self.resetView(is_draw = False)
        
        self._tool = None
        
        # Forcing a specific style on the window.
        #   Should this include styles passed?
        style = wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE

        attribList = (glcanvas.WX_GL_RGBA, # RGBA
                      glcanvas.WX_GL_DOUBLEBUFFER, # Double Buffered
                      glcanvas.WX_GL_DEPTH_SIZE, 24) # 24 bit


        glcanvas.GLCanvas.__init__(self, parent,-1, attribList=attribList)
        
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
        self.on_draw()
    
    def activate_tool(self, tool):
        # called from tool
        self._tool = tool
    
    def deactivate_tool(self):
        # called from tool
        self._tool = None
    
    def get_tool(self):
        return self._tool
            
    def get_drawing(self):
        return self._drawing
    
    
          
    def resetView(self, is_draw = True):
        self.zoom = 65.

        self.xRotate = 180.
        self.yRotate = 180.
        self.zRotate = -90.0
        self.trans=(0.0,0.0)
        
        
        if is_draw:
            self.on_draw()
        
    
      
    def OnWheel(self, event, is_draw = False):
        #EventType = FloatCanvas.EVT_FC_MOUSEWHEEL
        # 
        Rot = event.GetWheelRotation()
        #print 'OnWheel!!',Rot,event.ControlDown(),event.ShiftDown()
        if event.ControlDown():#event.ControlDown(): # zoom
            if Rot < 0:
                self.zoom_in(is_draw = False)
            else:
                self.zoom_out(is_draw = False)
            is_draw |= True

            
        elif self._tool != None:
            is_draw |= self._tool.on_wheel(event)
            self.on_draw()
            event.Skip()  
            
        if is_draw: 
            self.on_draw()
            event.Skip()  
             
    def get_resolution(self):
        """
        Resolution in GL unit per scren pixel
        """
        if (self.g_Width==0)&(self.g_Height==0):
            # there is no window space
            return 1.0
        
        v_top = self.unproject((0.5*self.g_Width, 0))
        v_bot = self.unproject((0.5*self.g_Width, self.g_Height)) 
        
        v_left = self.unproject((0, 0.5* self.g_Height))
        v_right = self.unproject((self.g_Width,  0.5*self.g_Height))
        
        dy = np.abs(v_bot[1]-v_top[1])
        dx = np.abs(v_right[1]-v_left[1])
        
        if dx > dy:
            return dx/self.g_Width
        else:
            return dy/self.g_Height
        
    def zoom_tofit(self, event = None, is_draw = True):
        #print 'zoom_tofit',is_draw
        #p = self._canvas.unproject_event(event) 
        #vetrex = np.array([p[0], p[1], 0.0, 1.0],float)
        #p_screen = self._canvas.project(vetrex)
        #print 'SelectTool.on_left_down (px,py)=',p
        #print '  (x,y)=',event.GetPosition(),p_screen
         
        vv_min, vv_max = self.get_boundingbox()
        dv = vv_max- vv_min    
         
        #print 'vv_min',vv_min
        #print 'vv_max',vv_max
        
        p_min = self.project(vv_min)
        p_max = self.project(vv_max)
        
        #print 'p_min',p_min
        #print 'p_max',p_max
        
        dp = np.abs(np.array(p_max-p_min, float))
        #print '  dp',dp
        window = np.array([self.g_Width, self.g_Height], float)
        zoomfactor = 0.8*np.min(window/dp[:2])
        
        #print '  zoomfactor,zoom',zoomfactor,self.zoom
        
        self.zoom /= zoomfactor
        #print '  zoomfactor',zoomfactor,self.zoom
        
        self.on_draw()
        #vv_min, vv_max = self.get_boundingbox()
        
        # lowerleft corner
        vv_target = self.unproject((0.0, self.g_Height)) 
        
        #print '  vv_min',vv_min
        #print '  vv_target',vv_target
        #print '  trans',self.trans
        dv = 0.9*vv_target-vv_min
        
        #print '  dv',dv
        newtrans = np.array(self.trans) + dv[:2]
        self.trans = tuple(newtrans)
        #print '  trans',self.trans
        self.on_draw()
            
    def get_boundingbox(self): 
        drawing = self.get_drawing()
        vv_min = np.inf * np.ones((1,3),float)
        vv_max = -np.inf * np.ones((1,3),float)
        if drawing:
            for drawobj in  drawing.get_drawobjs():
                v_min, v_max= drawobj.get_boundingbox()
                #print '    v_min',v_min
                #print '    v_max',v_max
                vv_min = np.concatenate((vv_min,v_min.reshape(1,3)),0)
                vv_max = np.concatenate((vv_max,v_max.reshape(1,3)),0)
        #print '  vv_min',vv_min
        #print '  vv_max',vv_max
        vv_min = np.min(vv_min,0)
        vv_max = np.max(vv_max,0)  
        return vv_min, vv_max
             
    def zoom_in(self, event = None, is_draw = True):
        self.zoom *= 0.9
        if is_draw:
            self.on_draw()
            
    def zoom_out(self, event = None, is_draw = True):
        self.zoom *= 1.1
        if is_draw:
            self.on_draw()
                    
    def OnLeftDown(self, event, is_draw = False):
        if (event.ControlDown()&event.ShiftDown())&(self.action == ''):
          self.action = 'drag'
          self.BeginGrap(event)
          event.Skip()
          
        elif self._tool != None:
            is_draw = self._tool.on_left_down(event)
        
        if is_draw: 
            self.on_draw()
            event.Skip()   
            
    def OnLeftUp(self, event, is_draw = False):
        
        if self.action=='drag':
          self.EndGrap(event)
          self.action==''
          event.Skip()
          
        elif self._tool != None:
            is_draw = self._tool.on_left_up(event)
        
        if is_draw: 
            self.on_draw()
            event.Skip()
        
    def OnMotion(self, event, is_draw=False):
        #print 'OnMotion',event.GetPosition() 
        if (event.ControlDown()&event.ShiftDown()&(self.action=='drag')):
          self.MoveGrap(event)
          is_draw |= True
          event.Skip()
          
        elif (self.action=='drag'):
          self.EndGrap(event)
          is_draw |= True
          self.action==''
          event.Skip()
    
        elif self._tool != None:
            is_draw |= self._tool.on_motion(event)
            
        if is_draw:
            self.on_draw()
    

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

        self.on_draw()
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

    def on_draw(self, *args, **kwargs):
        """Draw the window."""
        #print 'on_draw'
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
        
        # resolution in GL unit per scren pixel
        resolution = self.get_resolution()
        #print '  get_resolution',resolution
        
        # draw actual scene
        if self._drawing:
            for drawobj in  self._drawing.get_drawobjs():
                 drawobj.draw(resolution)
              

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
    
    def unproject_event(self, event): 
        return self.unproject(event.GetPosition())[0:2]
    
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
    
    def project(self, vertex):
        """
        http://stackoverflow.com/questions/3792481/how-to-get-screen-coordinates-from-a-3d-point-opengl
        """
        modelviewmatrix = glGetDoublev(GL_MODELVIEW_MATRIX)
        projectionmatrix = glGetDoublev(GL_PROJECTION_MATRIX)
        viewport = glGetInteger(GL_VIEWPORT)
        
        coords = np.array(gluProject(   vertex[0], vertex[1], vertex[2],
                                        modelviewmatrix, projectionmatrix,
                                        viewport))
        
        coords[1]= self.g_Height - coords[1]
        #print 'project',coords
        
        return coords    
        
class OGLcanvasTools(ToolsPanel):
    """
    Shows a toolpallet with different tools and an options panel.
    Here tools are added which 
    """
    def __init__(self, parent):
        ToolsPanel.__init__(self, parent, n_buttoncolumns=3)
        # add and set initial tool
        id_initialtool = self.add_tool(SelectTool(self))
        self.set_tool_with_id(id_initialtool)
        
        #for i in range(5):
        #    self._toolspanel.add_tool(BaseTool(self))
        self.add_tool(DelTool(self))                            
        
        # more tools can be added later...
        

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
        navcanvas = OGLnavcanvas(self, mainframe)
        #self._canvas = OGLcanvas(self)
        self._canvas = navcanvas.get_canvas() 
        
        # compose tool pallet here
        self._toolspanel =  OGLcanvasTools(self) 
        
        
        
        # compose editor window
        sizer.Add(self._toolspanel,0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)# from NaviPanelTest
        #sizer.Add(self._canvas,1,wx.GROW)# from NaviPanelTest
        sizer.Add(navcanvas,1,wx.GROW)
        
        # finish panel setup
        self.SetSizer(sizer)
        sizer.Fit(self)
        
    def get_canvas(self):
        # this is used mainly by the tools to know on which canvas to operate on
        # if multiple canvas, use the one with focus
        return self._canvas
        
    def get_mainframe(self):
        # this is used mainly by the tools to know on which mainframe to operate on
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
            print '\non_test'
            vertices = np.array([
                    [[0.0,0.0,0.0],[0.2,0.0,0.0]],# 0 green
                    [[0.0,0.0,0.0],[0.0,0.9,0.0]],# 1 red
                    ])
            
            
            colors =np.array( [
                    [0.0,0.9,0.0,0.9],    # 0
                    [0.9,0.0,0.0,0.9],    # 1
                    ])
            
            colors2 =np.array( [
                    [0.5,0.9,0.5,0.5],    # 0
                    [0.9,0.5,0.9,0.5],    # 1
                    ])
            colors2o =np.array( [
                    [0.8,0.9,0.8,0.9],    # 0
                    [0.9,0.8,0.9,0.9],    # 1
                    ])
                    
          
                
            drawing = OGLdrawing()
#-------------------------------------------------------------------------------
            
            if 0:
                lines = Lines('lines', drawing)
                lines.add_drawobjs(vertices,colors)
                drawing.add_drawobj(lines)
#-------------------------------------------------------------------------------
            if 0:
                fancylines = Fancylines('fancylines', drawing)
                vertices_fancy = np.array([
                    [[0.0,-1.0,0.0],[2,-1.0,0.0]],# 0 green
                    [[0.0,-1.0,0.0],[0.0,-5.0,0.0]],# 1 red
                    ])
                    
                widths =    [   0.5, 
                                0.3,
                                ]  
                #print '  vertices_fancy\n',vertices_fancy
                # FLATHEAD = 0
                #BEVELHEAD = 1
                #TRIANGLEHEAD = 2 
                #ARROWHEAD = 3 
                fancylines.add_drawobjs(    vertices_fancy,
                                            widths,# width
                                            colors,
                                            beginstyles = [TRIANGLEHEAD,TRIANGLEHEAD], 
                                            endstyles = [ARROWHEAD,ARROWHEAD])
                drawing.add_drawobj(fancylines)
#-------------------------------------------------------------------------------
            if 1:
                polylines = Polylines('polylines', drawing, joinstyle = BEVELHEAD)
                colors_poly =np.array( [
                    [0.0,0.8,0.5,0.9],    # 0
                    [0.8,0.0,0.5,0.9],    # 1
                    ]) 
                    
                vertices_poly = np.array([
                        [[0.0,2.0,0.0],[5.0,2.0,0.0],[5.0,7.0,0.0],[0.0,7.0,0.0]],# 0 green
                        [[0.0,-2.0,0.0],[-2.0,-2.0,0.0]],# 1 red
                        ], np.object)
                
                widths =    [   0.5, 
                                0.3,
                                ]     
                #print '  vertices_poly\n',vertices_poly
                polylines.add_drawobjs(    vertices_poly,
                                            widths,# width
                                            colors_poly,
                                            beginstyles = [ARROWHEAD,ARROWHEAD], 
                                            endstyles = [ARROWHEAD,ARROWHEAD])
                drawing.add_drawobj(polylines)
            
#-------------------------------------------------------------------------------
            if 1:
                polygons = Polygons('polygons', drawing,linewidth = 5)
                colors_poly =np.array( [
                    [0.0,0.9,0.9,0.9],    # 0
                    [0.8,0.2,0.2,0.9],    # 1
                    ]) 
                    
                vertices_poly = [
                        [[0.0,2.0,0.0],[5.0,2.0,0.0],[5.0,7.0,0.0],],# 0 green
                        [[0.0,-2.0,0.0],[-2.0,-2.0,0.0],[-2.0,0.0,0.0]],# 1 red
                        ]
                        
                print '  vertices_polygon\n',vertices_poly
                polygons.add_drawobjs(    vertices_poly,
                                            colors_poly)
                polygons.add_drawobj(    [[5.0,-2.0,0.0],[3.0,-2.0,0.0],[3.0,0.0,0.0]],
                                            [0.8,0.2,0.8,0.9])                            
                drawing.add_drawobj(polygons)
            
            
#-------------------------------------------------------------------------------
            
            if 0:
                circles = Circles('circles', drawing)
                circles.add_drawobjs([[0.0,0.0,0.0],[1.0,0.5,0.0]],[0.5, 0.25],colors2o, colors2)
                drawing.add_drawobj(circles)
                
                
                #drawing.add_drawobj(testogl.triangles)
                #drawing.add_drawobj(testogl.rectangles)
#-------------------------------------------------------------------------------
                
            canvas = self.gleditor.get_canvas()
            canvas.set_drawing(drawing)
            
            
            #lines.add_drawobj([[0.0,0.0,0.0],[-0.2,-0.8,0.0]], [0.0,0.9,0.9,0.9])
            #circles.add_drawobj([1.5,0.0,0.0],0.6,colors2o[0], colors2[0])
                
                
            #canvas.zoom_tofit()    
            wx.CallAfter(canvas.zoom_tofit)  
            
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
