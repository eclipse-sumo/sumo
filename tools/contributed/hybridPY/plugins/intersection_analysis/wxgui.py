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
from . import interana


            
class WxGui(    ModuleGui):
    """Contains functions that communicate between PRT plugin and the widgets of the main wx gui
    and the functions of the plugin.
    """


    def __init__(self, ident):
        self._interana = None
        self._demand = None
        self._simulation = None
        self._init_common(  ident,  priority = 100005,
                            icondirpath = os.path.join(os.path.dirname(__file__), 'images'))
        
        
    def get_module(self):
        return self._interana
        
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
        #self.make_toolbar()
        

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
            del self._interana
            del self._simulation
            self._demand = scenario.demand
            self._simulation = scenario.simulation
            self._interana = self._simulation.add_simobject(ident = 'interana', SimClass = interana.IntersectionAnalysis)
            is_refresh = True
            #if (self._prtservice is not None)&(self._simulation is not None):
            #print ' self._simulation.results,self._prtservice._results:', self._simulation.results,self._prtservice.get_results(),id(self._simulation.results), id(self._prtservice.get_results())
            #print '  self._simulation.results',id(self._simulation.results)
    def make_menu(self):
        menubar = self._mainframe.menubar
        menubar.append_menu('plugins/intersection analysis', bitmap = self.get_icon('icon_intersection2.png'),)
        if sumo.traci is not None:
            menubar.append_item( 'plugins/intersection analysis/browse',
                self.on_browse_obj, # common function in modulegui
                info='View and browse intersection analysis in object panel.',
                bitmap = self.get_agileicon('icon_browse_24px.png'),#,
                )
            menubar.append_item( 'plugins/intersection analysis/add intersection ...',
                self.on_add_intersection, # common function in modulegui
                #bitmap = self.get_agileicon('icon_browse_24px.png'),#,
                )
            
            menubar.append_item( 'plugins/intersection analysis/import intersection descriptions...',
                self.on_import_intersections, # common function in modulegui
                #bitmap = self.get_agileicon('icon_browse_24px.png'),#,
                )   
                
                

        
    def on_add_intersection(self, event = None):
        """Add intersection with data to analyzer."""
        p = interana.IntersectionAdder(parent = self._interana, logger = self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, p, immediate_apply=True)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() != 'success':#val == wx.ID_CANCEL:
            dlg.Destroy()
        self._mainframe.browse_obj(self._interana)
    
    def on_import_intersections(self, event = None):
        """Import intersection description from text file."""
        p = interana.IntersectionImporter(parent = self._interana, logger = self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, p, immediate_apply=True)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() != 'success':#val == wx.ID_CANCEL:
            dlg.Destroy()
        self._mainframe.browse_obj(self._interana)
    
            

        
   

