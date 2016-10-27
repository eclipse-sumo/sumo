import os
import wx

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.processdialog import ProcessDialog
import sumo
import results

class ResultDialog(ProcessDialog):
    def _get_buttons(self):
        buttons=[   ('Show and close',   self.on_run,      'Show selected quantity and close this window thereafter.'),
                    ('Show',   self.on_show,      'Show selected quantity.'),
                    ]
        defaultbutton='Show and close'
        standartbuttons=['cancel',]
        
        return buttons, defaultbutton, standartbuttons
    
    def on_show(self, event):
        self.process.show()
        
class WxGui(ModuleGui):
    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """


    def __init__(self, ident):
        self._net = None
        self._init_common(  ident,  priority = 10000,
                            icondirpath = os.path.join(os.path.dirname(__file__), 'images'))
        self._results = None 
        
    
    def get_module(self):
        return self.get_scenario()
        
    def get_scenario(self):
        return self._mainframe.get_modulegui('coremodules.scenario').get_module()
    
    
    #def get_neteditor(self):
    #    return self._neteditor
        
    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        #self._neteditor = mainframe.add_view("Network", Neteditor)
        
        #mainframe.browse_obj(self._net)
        self.make_menu()
        self.make_toolbar()
        #self._resultviewer = mainframe.add_view("Results", Neteditor)

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        is_refresh = False 
        
        scenario = self.get_scenario()
        #print '\n\nnetwork refresh_widgets',self._net,scenario.net.is_modified()
        #if self._net != scenario.net:
        #    #print '  scenario has a new network'
        #    #print '  self._net != scenario.net',self._net,scenario.net
        #    del self._net
        #    self._net = scenario.net
        #    is_refresh = True 

            
        #elif self._net.is_modified():
        #    #print '   network is_modified',self._net.is_modified()
        #    is_refresh = True 
     
        #if is_refresh:    
        #    drawing = self._neteditor.set_net(self._net)
        #    canvas = self._neteditor.get_canvas()
           

    def make_menu(self):
        #print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu('simulation', bitmap = self.get_icon('icon_sumo.png'))
        
        
        menubar.append_menu('simulation/SUMO',
                            bitmap = self.get_icon('icon_sumo.png'),
                            )
            
        menubar.append_item( 'simulation/SUMO/export routes and simulate...',
            self.on_sumo_routes, 
            info='Export current routes, network and poly and SUMO micro-simulate.',
            bitmap = self.get_icon('icon_sumo.png'),#,
            )
            
        menubar.append_item( 'simulation/SUMO/choose files and simulate...',
            self.on_sumo, 
            info='SUMO micro-simulate current network. Route file(s) can be chosen.',
            bitmap = self.get_icon('icon_sumo.png'),#,
            )
            
            
        menubar.append_menu('simulation/results')
        menubar.append_item( 'simulation/results/show',
            self.on_show_results, 
            info='Show simulation result table and graphics.',
            #bitmap = self.get_icon('icon_sumo.png'),#,
            )
        menubar.append_item( 'simulation/results/plot',
            self.on_plot_results, 
            info='Plot results in Matplotlib plotting envitonment.',
            #bitmap = self.get_icon('icon_sumo.png'),#,
            )    
                
        #menubar.append_item( 'network/test',
        #    self.on_test, 
        #    info='Test graph.',
        #    #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
        #    )
    
    
            
    
    def on_show_results(self, event = None):
        if self._results == None:
            self._results = results.Simresults(ident= 'simresults', scenario=self.get_scenario())
            
        self._mainframe.browse_obj(self._results)
        if event:
            event.Skip()
        
    def on_plot_results(self, event = None):
        if self._results == None:
            self._results = results.Simresults(ident= 'simresults', scenario=self.get_scenario())
        resultplotter = results.Resultplotter(  self._results, 
                                                logger = self._mainframe.get_logger())
        dlg = ResultDialog(self._mainframe, resultplotter)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        #print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
        #print '  status =',dlg.get_status()
        if dlg.get_status() != 'success':#val == wx.ID_CANCEL:
            #print ">>>>>>>>>Unsuccessful\n"
            dlg.Destroy()
            
        if dlg.get_status() == 'success':
            #print ">>>>>>>>>successful\n"
            # apply current widget values to scenario instance
            dlg.apply()
            dlg.Destroy()
            
        
    def on_sumo_routes(self, event = None):
        self.prepare_results()
        self.simulator = sumo.Sumo( scenario = self.get_scenario(), 
                                    results = self._results,
                                    logger = self._mainframe.get_logger(),
                                    is_gui = True,
                                    is_export_net = True,
                                    is_export_poly = True,
                                    is_export_rou = True,
                                    method_routechoice = None,
                                   )
        self.open_sumodialog()
        
    def on_sumo(self, event = None):
        self.prepare_results()
        self.simulator = sumo.Sumo( scenario = self.get_scenario(), 
                                    results = self._results,
                                    logger = self._mainframe.get_logger(),
                                    is_gui = True,
                                   )
        self.open_sumodialog()
        
    def prepare_results(self, ident_results = 'simresults'):
        
        # TODO: here we should actually replace the current network
        # so we would need a clear net method in scenario
        # alternatively we could merge properly
        scenario = self.get_scenario()
        if self._results == None:
            self._results = results.Simresults( ident= ident_results, 
                                                scenario = scenario)
        else:
            if self._results.parent != scenario:
                # uups scenario changed
                del self._results
                self._results = results.Simresults(ident= ident_results, scenario=scenario)
            
        
                
    
                                
    def open_sumodialog(self):
        dlg = ProcessDialog(self._mainframe, self.simulator)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        #print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
        #print '  status =',dlg.get_status()
        if dlg.get_status() != 'success':#val == wx.ID_CANCEL:
            #print ">>>>>>>>>Unsuccessful\n"
            dlg.Destroy()
            
        if dlg.get_status() == 'success':
            #print ">>>>>>>>>successful\n"
            # apply current widget values to scenario instance
            dlg.apply()
            dlg.Destroy()
            
            #print 'call self._mainframe.refresh_moduleguis()'
            self._mainframe.refresh_moduleguis()
        
    
    
        
        
        
    
    


