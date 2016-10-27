import sys,os, string,time
APPDIR = os.path.join(os.path.dirname(__file__),"..")
sys.path.append(os.path.join(APPDIR,"lib_base"))
IMAGEDIR =  os.path.join(os.path.dirname(__file__),"images")
 
import  wx
from wx.lib.buttons import GenBitmapTextButton, GenBitmapButton

from objpanel import ObjPanel

import classman as cm
import arrayman as am
class BaseTool(am.ArrayObjman):
    """
    This is a base tool class for Agilecanvas.
    It must handle all mouse or keyboard events,
    must create and draw helplines and finally
    modify the state of client which are grafically
    represented on the canvas.
    """ 
    
    def __init__(self,parent,mainframe=None):
        """
        To be overridden by specific tool.
        """
        self.init_common('select',parent,'Selection tool',mainframe = mainframe, info = 'Select objects in cancvas')
              
    def set_button_info(self,bsize=(32,32)):
        #print 'set_button_info select tool'
        self._bitmap = wx.Bitmap(os.path.join(IMAGEDIR,'selectIcon.bmp'),wx.BITMAP_TYPE_BMP)
        self._bitmap_sel=wx.Bitmap(os.path.join(IMAGEDIR,'selectIconSel.bmp'),wx.BITMAP_TYPE_BMP)
    
    def get_button(self, parent, bottonsize = (32,32), bottonborder=10):
        
        # simple stockbuttons
        #b=wx.Button(parent, wx.ID_DELETE)
        
        id=wx.NewId()
        bitmap = self._bitmap
        
        b=GenBitmapTextToggleButton(parent, id, bitmap,self.ident.title(),name = self.get_name())
        #b=GenBitmapToggleButton(parent, id, bitmap,(bitmap.GetWidth()+bottonborder, bitmap.GetHeight()+bottonborder), name = self.get_name())
        #b=GenBitmapToggleButton(self, wx.ID_DELETE)
        #b = GenBitmapTextToggleButton(self, id, None, tool.get('name',''), size = (200, 45))
        
        if bitmap!=None:
            #mask = wx.Mask(bitmap, wx.BLUE)
            #bitmap.SetMask(mask)
            b.SetBitmapLabel(bitmap)
            #bmp=wx.NullBitmap
        
        bitmap_sel = self._bitmap_sel    
        if bitmap_sel!=None:
            #mask = wx.Mask(bmp, wx.BLUE)
            #bmp.SetMask(mask)
            b.SetBitmapSelected(bitmap_sel)
            
        b.SetUseFocusIndicator(False)
            
        b.SetUseFocusIndicator(False)
        #b.SetSize((36,140))
        #b.SetBestSize()
        tt=wx.ToolTip(self.get_info())
        b.SetToolTip(tt)#.SetTip(tool.tooltip)
        return b
    
    def init_common(self,ident,parent,name,mainframe=None,info = None):
        #print 'Agiletool.__init__',ident,name
        #self.name = name
        self._mainframe = mainframe
        self._canvas=None
        self._init_objman(ident, parent=parent, name = name.title(), info = info)
        attrsman = self.set_attrsman(cm.Attrsman(self))
        self.drawobjs={}
        self.helpobjs={}
         
        
        #print ' call set_button',self.ident
        self.set_button_info()
        self._optionspanel = None
        
    def get_optionspanel(self, parent):
        """
        Return tool option widgets on given parent
        """
        self._optionspanel = ObjPanel(parent, obj =  self, 
                    attrconfigs=None, 
                    #tables = None,
                    #table = None, id=None, ids=None,
                    groupnames = ['options'],
                    func_change_obj=None, 
                    show_groupnames=False, show_title=True, is_modal=False,
                    mainframe=None, 
                    pos=wx.DefaultPosition, size=wx.DefaultSize, style = wx.MAXIMIZE_BOX|wx.RESIZE_BORDER,
                    immediate_apply=False, panelstyle='default',#'instrumental'
                    standartbuttons=['apply','restore'])
                    
        return self._optionspanel
        
        
                
    def activate(self,canvas = None):
        """
        This call by metacanvas signals that the tool has been
        activated and can now interact with metacanvas.
        """
        #print 'activate_metacanvas',self.ident
        self._canvas=canvas
        #self._canvas.del_handles()
        
    

    def deactivate(self):
        """
        This call by metacanvas signals that the tool has been
        deactivated and can now interact with metacanvas.
        """
        
        #self.deactivate()
        self._canvas=None
        self._mainframe = None

   
class BaseToolOld(cm.BaseObjman):
    """
    This is a base tool class for Agilecanvas.
    It must handle all mouse or keyboard events,
    must create and draw helplines and finally
    modify the state of client which are grafically
    represented on the canvas.
    """ 
    
    def __init__(self,parent,mainframe=None):
        """
        To be overridden by specific tool.
        """
        self.init_common('select',parent,'Selection tool',mainframe, info = 'Select objects in cancvas')
              
    def set_button_info(self,bsize=(32,32)):
        #print 'set_button_info select tool'
        self._bitmap = wx.Bitmap(os.path.join(IMAGEDIR,'selectIcon.bmp'),wx.BITMAP_TYPE_BMP)
        self._bitmap_sel=wx.Bitmap(os.path.join(IMAGEDIR,'selectIconSel.bmp'),wx.BITMAP_TYPE_BMP)
    
    def get_button(self, parent, bottonsize = (32,32), bottonborder=10):
        
        # simple stockbuttons
        #b=wx.Button(parent, wx.ID_DELETE)
        
        id=wx.NewId()
        bitmap = self._bitmap
        
        b=GenBitmapTextToggleButton(parent, id, bitmap,self.ident.title(),name = self.get_name())
        #b=GenBitmapToggleButton(parent, id, bitmap,(bitmap.GetWidth()+bottonborder, bitmap.GetHeight()+bottonborder), name = self.get_name())
        #b=GenBitmapToggleButton(self, wx.ID_DELETE)
        #b = GenBitmapTextToggleButton(self, id, None, tool.get('name',''), size = (200, 45))
        
        if bitmap!=None:
            #mask = wx.Mask(bitmap, wx.BLUE)
            #bitmap.SetMask(mask)
            b.SetBitmapLabel(bitmap)
            #bmp=wx.NullBitmap
        
        bitmap_sel = self._bitmap_sel    
        if bitmap_sel!=None:
            #mask = wx.Mask(bmp, wx.BLUE)
            #bmp.SetMask(mask)
            b.SetBitmapSelected(bitmap_sel)
            
        b.SetUseFocusIndicator(False)
            
        b.SetUseFocusIndicator(False)
        #b.SetSize((36,140))
        #b.SetBestSize()
        tt=wx.ToolTip(self.get_info())
        b.SetToolTip(tt)#.SetTip(tool.tooltip)
        return b
    
    def init_common(self,ident,parent,name,mainframe=None,info = None):
        #print 'Agiletool.__init__',ident,name
        #self.name = name
        self._canvas=None
        #FSMnamed.__init__(self,ident,parent,name)
        self._init_objman(ident, parent=parent, name = name.title(), info = info)
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        
        
        
        #self.access = attrsman.add(cm.AttrConf(  'access', ['bus','bike','tram'],
        #                            groupnames = ['options'], 
        #                            perm='rw', 
        #                            is_save = True,
        #                            name = 'Access list', 
        #                            info = 'List with vehicle classes that have access',
        #                            ))
        #                            
        #self.emissiontype = attrsman.add(cm.AttrConf(  'emissiontype', 'Euro 0',
        #                            groupnames = ['options'], 
        #                            perm='rw', 
        #                            is_save = True,
        #                            name = 'Emission type', 
        #                            info = 'Emission type of vehicle',
        #                            ))
                                    
        # dictionary of drawobjects that will be created during 
        # the application of the tool.
        # ident is the number in chronological order of creation,
        # starting with one. Value is the ad hoc instance of a drawing 
        # object.
        self.drawobjs={}
        self.helpobjs={}
         
        
        #print ' call set_button',self.ident
        self.set_button_info()
        self.targetsets={}
        #self.optionspanel=None
    
    def activate(self,canvas, mainframe = None):
        """
        This call by metacanvas signals that the tool has been
        activated and can now interact with metacanvas.
        """
        #print 'activate_metacanvas',self.ident
        self._canvas=canvas
        self._mainframe = mainframe
        #self._canvas.del_handles()
        
    
    #def get_metacanvas(self):
    #    return self._canvas
    
    #def get_pentable(self):
    #    if self._canvas:
    #        return self._canvas.get_pentable()
        
    def deactivate(self):
        """
        This call by metacanvas signals that the tool has been
        deactivated and can now interact with metacanvas.
        """
        
        #self.deactivate()
        self._canvas=None
        self._mainframe = None
            
    def append_drawobj(self,drawobj):
        """
        Append new drawobject
        """    
        n=len(self.drawobjs)+1
        self.drawobjs[n] = drawobj
        
    
        
        
    def pop_drawobj(self):
        """
        Returns most recent drawobject, removing it from the list.
        If there are no more drwobjects in the list None is returned.
        """
        
        n=len(self.drawobjs)
        if n > 0:
            drawobj=self.drawobjs[n]
            del self.drawobjs[n]
            return drawobj
        else:
            return None
    
    def get_last_drawobj(self):
        """
        Returns most recent drawobject, without changing the list.
        If there are no more drwobjects in the list None is returned.
        """
        n=len(self.drawobjs)
        if n > 0:
            drawobj=self.drawobjs[n]
            return drawobj
        else:
            return None
           
    def clear_drawobjs(self):
        """
        Clear list of  drawobjects, while maintaining them on metacanvas.
        """
        self.drawobjs={}
            
    def del_drawobjs(self):
        """
        Remove all drawobjects from metacanvas.
        """
        while len(self.drawobjs)>0:
            drawobj=self.pop_drawobj()
            self._canvas.del_obj(drawobj)
        
    
    
    def make_targetsets(self,setnames=None,layer=None):
        """
        Returns a dictionary with instances of targetsets as values
        and setnames as key.
        This allows the tool to select a list of sets from the a specific
        layer to which it can directly communicate.
        
        If no setnames are given then all sets of the specific layer 
        are returned.
        
        Can be used for example to change handle settings
        """
        if setnames!=None:
            objsets={}
            for name in setnames:
                objsets[name]=self._canvas.get_objset_from_layer(layer,name)
            self.targetsets = objsets  
        else:
            self.targetsets = self._canvas.get_objset_from_layer(layer)
        
    def set_handles(self):
        """
        Set handles to selected object sets which can be connected.
        """
        # put handles on all section objects
        for name_set in self.targetsets.keys():
            self._canvas.set_handles(name_set=name_set)
            
    
                    
    
        
    def get_optionspanel(self, parent):
        """
        Return tool option widgets on given parent
        """
        p= ObjPanel(parent, obj =  self, 
                    attrconfigs=None, 
                    #tables = None,
                    #table = None, id=None, ids=None,
                    groupnames = ['options'],
                    func_change_obj=None, 
                    show_groupnames=False, show_title=True, is_modal=False,
                    mainframe=None, 
                    pos=wx.DefaultPosition, size=wx.DefaultSize, style = wx.MAXIMIZE_BOX|wx.RESIZE_BORDER,
                    immediate_apply=False, panelstyle='default',#'instrumental'
                    standartbuttons=['apply','restore'])
                    
        return p
        
        
                
    def set_options(self,event):
        """
        Called from options panel.
        """
        print 'set_options',self.ident
        print '  event=',event
        pass
        
    def set_statusbar(self,key,info):
        pass
            
            
    


class DelTool(BaseTool):
    def __init__(self,parent,mainframe=None):
        """
        To be overridden by specific tool.
        """
        self.init_common('delete',parent,'Delete',mainframe, info = 'Delete objects in cancvas')
              
    def set_button_info(self,bsize=(32,32)):
        #print 'set_button_info select tool'
        self._bitmap = None
        self._bitmap_sel = None
    
    def get_button(self, parent, bottonsize = (32,32), bottonborder=10):
        
        # simple stockbuttons
        b=wx.Button(parent, wx.ID_DELETE,name = self.get_name())
        
        b.SetSize(bottonsize)
        #b.SetBestSize()
        tt=wx.ToolTip(self.get_info())
        b.SetToolTip(tt)#.SetTip(tool.tooltip)
        #print 'DelTool.get_button',dir(b)
        return b
        
                    
class ToolPalett(wx.Panel):
    """
    This is a panel where tools are represented by images and/or text.
    The tools are selected in a radio-button-fashion.
    
    Each tool has a string as key. Each time the status changes,
    a callback function is called with new and old tool key as argument.
    """
    def __init__(self,parent,tools=[],callback=None, n_buttoncolumns=3):
        """
        callback is a function that is called when a tool has been selected.
        The function is called as:
            callback(tool)
        
        """
        #the metacanvas object with which the pallet should apply th tools
        self._callback=callback
        
        #wx.Window.__init__(self,parent,wx.ID_ANY,wx.DefaultPosition,wx.DefaultSize,wx.SUNKEN_BORDER|wx.WANTS_CHARS)
        #wx.Panel.__init__(self,parent,wx.ID_ANY,wx.DefaultPosition,size,wx.RAISED_BORDER|wx.WANTS_CHARS)
        wx.Panel.__init__(self,parent,-1,wx.DefaultPosition,wx.DefaultSize) 
        #wx.Panel.__init__(self,parent,wx.ID_ANY,wx.DefaultPosition,(300,600),wx.RAISED_BORDER|wx.WANTS_CHARS)
        self.sizer = wx.GridSizer(0, n_buttoncolumns, 5, 5)
        self.SetSizer(self.sizer)
        self._id_to_tool={} 
        self._id=-1
        
        for tool in tools:
            self.add_tool(tool)
        
        self.sizer.Fit(self)
        #self.SetMaxSize((300,300))
    
    def add_tool(self,tool):
        """
        Add a tool to the pallet. 
        """
        #print 'add_tool',tool
        bottonsize = (32,32)
        bottonborder = 10
        toolbarborder = 1
        
        
        b= tool.get_button(self, bottonsize = bottonsize, bottonborder=bottonborder)
        self.Bind(wx.EVT_BUTTON, self.on_select, b)
        
        _id = b.GetId()
        self._id_to_tool[_id]=(tool,b)
        
        #self.sizer.Add(b, 0, wx.GROW)
        self.sizer.Add(b,0, wx.EXPAND, border=toolbarborder)
        #self.sizer.Add(b)
        #print ' _id =',_id
        return _id
        
    
        
    def get_tools(self):
        """
        Returns lins with all toll instances
        """
        tools=[]
        for (tool,b) in self._id_to_tool.values():
            tools.append(tool)
        return tools
            
    def refresh(self):
        """
        Reorganizes toolpallet after adding/removing tools.
        Attention is not automatically called.
        """
        self.sizer.Layout()
    
    def on_select(self,event):
        """
        Called from a pressed button
        """
        _id=event.GetEventObject().GetId()
        print '\n on_select',_id,self._id#,self._id_to_tool[_id]
        
        
        if _id!=self._id:
            if self._id_to_tool.has_key(_id):
                
                (tool,button)=self._id_to_tool[_id]
                print '  new tool',tool.get_name()
                self.unselect()
                self._id=_id
                
                # this will cause the main OGL editor to activate the
                # tool with the current canvas
                self.GetParent().set_tool(tool)
                #if self._callback!=None:
                #    self._callback(tool)
                event.Skip()
                return tool
            
        return None
    
        
            
        
    def select(self,_id):
        """
        Select explicitelt a tool with _id.
        """
        #print '\nselect',_id,self._id,self._id_to_tool
        
        if _id!=self._id:
            if self._id_to_tool.has_key(_id):
                
                (tool,button)=self._id_to_tool[_id]
                
                #print '  explicitly press button'
                if hasattr(button,'SetToggle'):
                    button.SetToggle(True)
                else:
                    button.SetFocus()
                    #print 'button.SetFocus',button.SetFocus.__doc__
                    #pass
                
                #print '  new tool',tool.get_name()
                #self.unselect()
                self._id=_id
                
            
                self.GetParent().set_tool(tool)
                #if self._callback!=None:
                #    self._callback(tool)
                return tool
            
        return None
                
                
    
    def unselect(self):
        """
        Unselect currently selected tool.
        """
        if self._id_to_tool.has_key(self._id):
            (tool,button)=self._id_to_tool[self._id]
            
            # Disactivate current tool
            tool.deactivate()
            
            if hasattr(button,'SetToggle'):
                button.SetToggle(False)
            else:
                #button.SetFocus()
                #print 'button.SetFocus',button.SetFocus.__doc__
                pass
   
    
            

        

class __ToggleMixin:
    def SetToggle(self, flag):
        self.up = not flag
        self.Refresh()
    SetValue = SetToggle

    def GetToggle(self):
        return not self.up
    GetValue = GetToggle

    def OnLeftDown(self, event):
        if not self.IsEnabled():
            return
        self.saveUp = self.up
        self.up = False #not self.up
        self.CaptureMouse()
        self.SetFocus()
        self.Refresh()

    def OnLeftUp(self, event):
        if not self.IsEnabled() or not self.HasCapture():
            return
        if self.HasCapture():
            if self.up != self.saveUp:
                self.Notify()
            self.ReleaseMouse()
            self.Refresh()

    def OnKeyDown(self, event):
        event.Skip()

    
        
class GenBitmapTextToggleButton(__ToggleMixin, GenBitmapTextButton):
    """A generic toggle bitmap button with text label"""
    pass

class GenBitmapToggleButton(__ToggleMixin, GenBitmapButton):
    """A generic toggle bitmap button with text label"""
    pass

class ToolsPanel(wx.Panel):
    """
    
    Interactively navigates through objects and displays attributes 
    on a panel.
    """
    def __init__(self, parent,**kwargs):
        wx.Panel.__init__(self,parent,wx.NewId(),wx.DefaultPosition,wx.DefaultSize) 
        
        sizer=wx.BoxSizer(wx.VERTICAL)
        
        self._toolspalett =   ToolPalett(self,**kwargs)  
        
        #self._toolspalett.add_tool(BaseTool(self))     
        
        # create initial option panel
        self._optionspanel = wx.Window(self)
        self._optionspanel.SetBackgroundColour("pink")
        wx.StaticText(self._optionspanel, -1, "Tool Options", (300,-1))
        
        
        sizer.Add(self._toolspalett,0, wx.ALL | wx.ALIGN_LEFT | wx.GROW, 4)
        sizer.Add(self._optionspanel,1,wx.GROW)# 
        
        # finish panel setup
        self.SetSizer(sizer)
        sizer.Fit(self)
        
        #self.SetSize(parent.GetSize())
    
    def get_canvas(self):
        # ask the OGL editor for the currently active canvas in focus 
        return self.GetParent().get_canvas()
    
    #def get_mainframe(self):
    #    return self.GetParent().get_mainframe()
    
    def add_tool(self, tool):
        return self._toolspalett.add_tool(tool)
    
    def set_tool_with_id(self, _id):
        """
        Explicitely set a tool from tool pallet using its id.
        Used to set initial tool.
        """
        #print 'set_tool_with_id',_id
        return self._toolspalett.select(_id)
    
    def set_tool(self, tool):
        """
        Called by toolpallet after new tool has been selected.
        """
        # Activate current tool
        # then tool wil set itself to canvas
        tool.activate(self.get_canvas())
        
        # set options of current tool
        sizer=self.GetSizer()
        sizer.Remove(1)
        self._optionspanel.Destroy()
                
        self._optionspanel = tool.get_optionspanel(self)
        
        #if id!=None:
        #    self.objpanel=ObjPanel(self,obj,id=id,func_change_obj=self.change_obj)
        #else:    
        #    self.objpanel=ObjPanel(self,obj,func_change_obj=self.change_obj)
        sizer.Add(self._optionspanel,1,wx.GROW)
        
        self.Refresh()
        #sizer.Fit(self)
        sizer.Layout()
                            
