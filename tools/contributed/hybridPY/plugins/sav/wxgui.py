import os
import wx
import numpy as np

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.ogleditor import *
from agilepy.lib_base.processes import Process
from agilepy.lib_wx.processdialog import ProcessDialog,ProcessDialogInteractive

from coremodules.network import routing 
from coremodules.demand import demand
from coremodules.simulation import sumo,results
from . import taxi


            
class WxGui(    ModuleGui):
    """Contains functions that communicate between SAV plugin and the widgets of the main wx gui
    and the functions of the plugin.
    """


    def __init__(self, ident):
        self._taxiservice = None
        self._demand = None
        self._simulation = None
        self._init_common(  ident,  priority = 100002,
                            icondirpath = os.path.join(os.path.dirname(__file__), 'images'))
        
        
    def get_module(self):
        return self._taxiservice
        
    def get_scenario(self):
        return self._mainframe.get_modulegui('coremodules.scenario').get_scenario()
    
    def get_neteditor(self):
        return self._mainframe.get_modulegui('coremodules.network').get_neteditor()

    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        #self._neteditor = mainframe.add_view("Network", Neteditor)
        
        #mainframe.browse_obj(self._module)
        self.make_menu()
        self.make_toolbar()
        

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        scenario = self.get_scenario()
        #print 'prtgui.refresh_widgets',self._simulation != scenario.simulation
        
            
        is_refresh = False
        if self._simulation != scenario.simulation:
            del self._demand
            del self._taxiservice
            del self._simulation
            self._demand = scenario.demand
            self._simulation = scenario.simulation
            self._taxiservice = self._simulation.add_simobject(ident = 'taxiservice', SimClass = taxi.TaxiService)
            is_refresh = True

    def make_menu(self):
        menubar = self._mainframe.menubar
        menubar.append_menu('plugins/Sav and Taxi', bitmap = self.get_icon('sav.png'),)
        if sumo.traci is not None:
            menubar.append_item( 'plugins/Sav and Taxi/generate taxi trips...',
                self.on_generate_taxitrips, 
                )

            menubar.append_item( 'plugins/Sav and Taxi/generate SAV trips...',
                self.on_generate_SAVtrips, 
                )
       
    def on_generate_taxitrips(self, event=None):
        """Stochastic traffic assignment with duarouter.
        """
 
                    
        obj = demand.TaxiGenerator(    self._demand, 
                                logger = self._mainframe.get_logger())
        
        
        dlg = ProcessDialog(self._mainframe, obj)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        #print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
        #print '  status =',dlg.get_status()

        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()
            
            self._mainframe.browse_obj(self._demand.trips)
    
    
    def on_generate_SAVtrips(self, event=None):
        """Stochastic traffic assignment with duarouter.
        """
 
                    
        obj = demand.SAVGenerator(    self._demand, 
                                logger = self._mainframe.get_logger())
        
        
        dlg = ProcessDialog(self._mainframe, obj)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        #print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
        #print '  status =',dlg.get_status()

        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()
            
            self._mainframe.browse_obj(self._demand.trips)
        
   

