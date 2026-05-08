
import  wx
import  wx.py   as  py # pyshell
import sys,os, string,time
from collections import OrderedDict 

from .wxmisc import KEYMAP,AgileMenuMixin,AgileToolbarMixin,AgileStatusbar,AgileMenubar


from os.path import *
from os import getcwd

from . import objpanel
from agilepy.lib_base.logger import Logger

# We first have to set an application-wide help provider.  Normally you
# would do this in your app's OnInit or in other startup code...
provider = wx.SimpleHelpProvider()
wx.HelpProvider.Set(provider)



def make_moduleguis(appdir, dirlist):
    moduleguilist = []
    for modulesdir in dirlist:
        make_moduleguilist(appdir, moduleguilist, modulesdir)
        #print 'make_moduleguis: moduleguilist=\n',moduleguilist
        
    moduleguilist.sort(key = lambda x: x.get_initpriority())
    moduleguis = OrderedDict()
    #for initpriority, wxgui in moduleguilist:
    for wxgui in moduleguilist:
        moduleguis[wxgui.get_ident()] = wxgui
    return moduleguis

def take_identity(elem):
    return elem[1]

def make_moduleguilist(appdir, moduleguilist, modulesdir):
    #print 'make_moduleguilist appdir,modulesdir',appdir,modulesdir
    #print '  check dir',os.path.join(appdir, modulesdir)
  
    for modulename in os.listdir(os.path.join(appdir, modulesdir)):# use walk module to get recursive
        #if os.path.isdir(os.path.join(os.getcwd(), pluginname)): # is never a dir???
        
        is_noimport = (modulename in ['__init__.py',]) | (modulename.split('.')[-1] == 'pyc')
        is_dir = os.path.isdir(os.path.join(appdir,modulesdir, modulename))
        
        #print '  GUI modulename',modulename,is_noimport,is_dir
        if (not is_noimport)& is_dir:
            
            lib = __import__(modulesdir+'.'+modulename)
            module = getattr(lib,modulename)
            #print '   imported modulename',modulename,module,hasattr(module,'get_wxgui')
            # has  module gui support specified in __init__.py
            if hasattr(module,'get_wxgui'):
                wxgui = module.get_wxgui()
                if wxgui is not None: 
                    #print '  append',(wxgui.get_initpriority(), wxgui.get_ident())
                    
                    #moduleguilist.append((wxgui.get_initpriority(), wxgui))
                    moduleguilist.append(wxgui)

class MainSplitter(wx.SplitterWindow):
    def __init__(self, parent, ID=wx.ID_ANY):
        wx.SplitterWindow.__init__(self, parent, ID,
                                   style = wx.SP_LIVE_UPDATE
                                   )
        
        
        
        self.SetMinimumPaneSize(20)
        
        #sty = wx.BORDER_SUNKEN
        
        #emptyobj = cm.BaseObjman('empty')
        self._objbrowser =  objpanel.NaviPanel(self, 
                                                None,
                                                #show_title = False
                                                #size = w.DefaultSize,
                                                #style = wx.DEFAULT_DIALOG_STYLE|wx.MAXIMIZE_BOX|wx.RESIZE_BORDER,
                                                #choose_id=False,choose_attr=False,
                                                #func_choose_id=None,
                                                #func_change_obj=None,
                                                #panelstyle = 'default', 
                                                immediate_apply = True,
                                                buttons = [],
                                                standartbuttons = ['apply','restore'],
                                                #defaultbutton = defaultbutton,
                                                )
                                             

        
        #self._viewtabs = wx.Notebook(self,wx.ID_ANY, style=wx.CLIP_CHILDREN)
        self._viewtabs = wx.Notebook(self, -1, size=(21,21), style=wx.BK_DEFAULT
                                 #wx.BK_TOP 
                                 #wx.BK_BOTTOM
                                 #wx.BK_LEFT
                                 #wx.BK_RIGHT
                                 # | wx.NB_MULTILINE
                                 )
        
        self._n_views = 0
        self._viewnames = []

        #splitter.SplitVertically(self._objbrowser,self.canvas , -100)
        self.SplitVertically(self._objbrowser, self._viewtabs, -100)
        #self.SetSashGravity(0.2) # notebook too small
        self.SetSashPosition(400, True)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, self.OnSashChanged)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGING, self.OnSashChanging)
    
    def add_view(self, name, ViewClass, **args):
        """
        Add a new view to the notebook.
        """ 
        #print 'context.add_view',ViewClass
        #print '  args',args
        view=ViewClass( self._viewtabs, 
                        mainframe = self.GetParent(),
                        **args
                        )
                        
        # Add network tab with editor                           
        p=self._viewtabs.AddPage( view, name.title() )
        self._viewnames.append(name)
        return view
    
    def select_view(self, ind = 0, name = None):
        if name is not None:
            if name in self._viewnames:
                ind = self._viewnames.index(name)
                self._viewtabs.ChangeSelection(ind)
            else:
                return False
        else:
            self._viewtabs.ChangeSelection(ind)
        
    def browse_obj(self, obj, **kwargs):
        self._objbrowser.change_obj(obj, **kwargs)
        
    def OnSashChanged(self, evt):
        #print("sash changed to %s\n" % str(evt.GetSashPosition()))
        pass
        
    def OnSashChanging(self, evt):
        #print("sash changing to %s\n" % str(evt.GetSashPosition()))
        # uncomment this to not allow the change
        #evt.SetSashPosition(-1)
        #self.canvas.OnSize()
        pass

class AgileMainframe(AgileToolbarMixin, wx.Frame):
    """
    Simple wx frame with some special features.
    """
    

    def __init__(self, parent=None,   title ='mainframe', appname = None,
                    moduledirs = [], args = [], appdir = '',
                    is_maximize = False, is_centerscreen = True,
                    pos=wx.DefaultPosition, size=wx.DefaultSize, 
                    style=wx.DEFAULT_FRAME_STYLE,
                    name='theframe', size_toolbaricons = (24,24)):
                     
        self._args = args
        
        print ('AgileMainframe.__init__')
        if appname is not None:
            self.appname = appname
        else:
            self.appname = title
            
        #wx.Frame.__init__(self, parent, -1, 'Test ToolBar', size=(600, 400))
        super().__init__( parent, -1, 'Test ToolBar', size=(600, 400))
        #super(GLFrame, self).__init__(parent, id, title, pos, size, style, name)
        self._splitter = MainSplitter(self)
        self._views = {}

        if is_maximize:
            self.Maximize()
        if is_centerscreen:
            self.CenterOnScreen()
            
        #################################################################
        # create statusbar
        self.statusbar = AgileStatusbar(self)
        self.SetStatusBar(self.statusbar)

        #################################################################
        # create toolbar
        
        self.init_toolbar(size=size_toolbaricons)

        
        
        self._mainframe = self
        
        if 0:    
            bmp = wx.ArtProvider.GetBitmap(wx.ART_NEW, wx.ART_TOOLBAR, (16, 16))
            self._mainframe.add_tool(   'tool',
                                        name = 'tool time',
                                        func=self.on_timedisplay,
                                        bitmap=bmp,
                                        info='Current tool time.',
                                        widget=None)
            
            self.timedisplay = wx.TextCtrl(self._mainframe.toolbar, value="0", size=(60, -1))
            #self.timedisplay.SetValue("000000.00")
            
            self._mainframe.add_tool(   'timedisplay',
                                        name = 'Simulation time',
                                        func=self.on_timedisplay,
                                        bitmap=None,
                                        info='Current simulation time.',
                                        widget=self.timedisplay)
                                        
                                        
            self.timecontrol = wx.Slider(self._mainframe.toolbar, value=10, minValue=0, maxValue=3600, size=(200, 24),
                                        style=wx.SL_HORIZONTAL  | wx.SL_LABELS #| wx.SL_AUTOTICKS
                                        )
            
            self._mainframe.Bind(wx.EVT_SCROLL_CHANGED, self.on_change_timecontrol)
            self.simtime_break = 0.0
            self._mainframe.add_tool(   'timecontrol',
                                        name = 'Time control',
                                        func=None,
                                        bitmap=None,
                                        info='Time control bar.',
                                        widget=self.timecontrol)
            if 1:
                # --- Timer ---
                self.timer = wx.Timer(self)
                self.Bind(wx.EVT_TIMER, self.onTimer2, self.timer)
                self.timer.Start(500)  # every ... milli seconds
                                    

                self.enabled = True
                self.counter = 0
                
        #################################################################
        #create the menu bar 
        
        self.menubar=AgileMenubar(self)
        self.SetMenuBar(self.menubar)
        #self.Show(True) #NO!!
        
        #################################################################
        # init logger
        self._logger = Logger()
        self._logger.add_callback(self.write_message, 'message')
        self._logger.add_callback(self.write_action, 'action')
        self._logger.add_callback(self.show_progress, 'progress')
        #################################################################
        self._moduleguis = make_moduleguis(appdir, moduledirs)
        
        for modulename, modulegui in self._moduleguis.items():
            #print ("    start init modulegui'%s',"%modulename)
            self._moduleguis[modulename] = modulegui
            modulegui.init_widgets(self)
            #print ("----finish init modulegui'%s',"%modulename)
                    
        #print ("----finish inizializing moduleguis")
        #################################################################
        # define idle method
        self.Bind(wx.EVT_IDLE, self.on_idle)
        
  
    #################################################################
    def set_title(self, titlename):
        self.SetTitle(self.appname+' - '+titlename)
        
    def refresh_moduleguis(self):
        #print 'refresh_moduleguis',len(self._moduleguis)
        self.browse_obj(None)
        for modulename, modulegui in self._moduleguis.items():
            #print '  refresh gui of module',modulename
            modulegui.refresh_widgets()
                    
    def get_modulegui(self, modulename):
        return self._moduleguis[modulename]  
          
    def write_message(self, text, **kwargs):
        self.statusbar.write_message(text)
        
    def write_action(self, text, **kwargs):
        self.statusbar.write_action(text)
        self.statusbar.write_message('')
        
    def show_progress(self,percent, **kwargs):
        #print ('show_progress',percent,int(percent))
        self.statusbar.set_progress(percent)
            
    def get_logger(self):
        return  self._logger
    
    def set_logger(self, logger):
        self._logger = logger
     
    def get_args(self):
        return self._args
    
    def browse_obj(self,obj,**kwargs):
        self._splitter.browse_obj(obj, **kwargs)
        
    def make_menu(self):
        """
        Creates manu. To be overridden.
        """
        self.menubar.append_menu('file')
        self.menubar.append_menu('file/doc')

        
        self.menubar.append_item('file/doc/open',self.on_open,\
                            shortkey='Ctrl+o',info='open it out')
        
        self.menubar.append_item('file/doc/save',self.on_save,\
                            shortkey='Ctrl+s',info='save it out')


        
    def add_view(self, name, ViewClass, **args):
        """
        Add a new view to the notebook.
        """ 
        #print 'context.add_view',ViewClass

        
        view=self._splitter.add_view(name, ViewClass, **args)
        self._views[name] = view
        return view
    
    
        
    
    def select_view(self, ind = 0, name = None):
        self._splitter.select_view(ind = ind, name = name)        
    
    def start_simulation(self, widget):
        """
        Initializes simulation and sets timer for callbacks.
        widget must have a step(event) function where it updates 
        all animated stuff
        """
        self._widget = widget
        self.Bind(wx.EVT_TIMER, self.on_step)
        self.timer = wx.Timer(self)
        self.continue_simulation()
        
   
        
    def on_step(self,event=None):
        #print('Mainframe.on_step')
        self._widget.step()
            
    
    def continue_simulation(self):
        print('continue_simulation') 
        self.timer.Start(int(self._widget.get_frametime()))
        self.write_action('Simulation running')
        
    def restart_simulation(self):
        print('restart_simulation') 
        self.timer.Stop()
        self._widget.restart()
        # self._widget will call start_simulation to reinitialize the timer
        
    def stop_simulation(self):
        print('stop_simulation')  
        self.timer.Stop()
        self.write_action('Simulation stopped')
    
    def on_size(self,event=None):
        #print 'Mainframe.on_size'
        wx.LayoutAlgorithm().LayoutWindow(self, self._splitter)
        #if event:
        #    event.Skip()
            
    def on_save(self,event):
        print('save it!!')  
    
  

    def on_open(self, event):
        """Open a document"""
        #wildcards = CreateWildCards() + "All files (*.*)|*.*"
        print('open it!!')  
  

    def destroy(self):
        """Destroy this object"""
        ##self.theDocManager.theDestructor()
        #imgPreferences.saveXml(self.GetStartDirectory() + "/" + imgINI_FILE_NAME)
        ##del self.thePrint
        self.Destroy()
    
        
    def on_close(self, event):
        #self.Close(True)
        print('Mainframe.on_close')
        #pass
        self.destroy()
    
    def on_exit(self, event):
        """Called when the application is to be finished"""
        self.destroy()
        


    def on_idle(self,event):  
        for modulename, modulegui in self._moduleguis.items():
            #print ('  on_idle gui of module',modulename)
            modulegui.on_idle(event)
            
        #self.count = self.count + 1
        #if self.count >= 100:
        #    self.count = 0
        
        #self.statusbar.set_progress(self.count)
	
    def on_about(self, event):
        """Display the information about this application"""
        #dlg = imgDlgAbout(self, -1, "")
        #dlg.ShowModal()
        #dlg.Destroy()
        pass  
    
    def write_to_statusbar(self, data, key='message'):
        self.statusbar[key] = str(data)
        
