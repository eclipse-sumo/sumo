
import os
import wx
from agilepy.lib_wx.processdialog import ProcessDialog, ProcessDialogInteractive

from . import bitjoin

class BitjoinWxGuiMixin:
    """Contains Bitjoin specific menu entries and methods."""
    def get_bitjoinicon(self, name):
        icondirpath = os.path.join(os.path.dirname(__file__), 'images_bitjoin')
        return wx.Bitmap(os.path.join(icondirpath, name))
        
    def add_menu_bitjoin(self, menubar,rootmenu):
        
        bitjoinrootmenu = rootmenu + '/bitjoin activity based demand generator'
        menubar.append_menu( bitjoinrootmenu,
                            bitmap = self.get_bitjoinicon("bitjoin.png"),
                            info = 'Virtual population and activity generation based on households, kand-use and population statistics.',
                            )  
        menubar.append_item( bitjoinrootmenu + '/household generator',
            self.on_generate_households_from_residential_facilities, 
            bitmap = self.get_bitjoinicon("icon_household_generation.jpg"),
            )
        menubar.append_item( bitjoinrootmenu + '/virtual population generator',
            self.on_generate_vp_from_households, 
            bitmap = self.get_bitjoinicon("icon_virtual_population.jpg"),
            )
        menubar.append_item( bitjoinrootmenu + '/daily activity generator',
            self.on_generate_activity_from_landuse_facilities, 
            bitmap = self.get_bitjoinicon("icon_daily_activity.jpg"),
            )
        menubar.append_item( bitjoinrootmenu + '/plan generator',
            self.on_generate_plan, 
            bitmap = self.get_icon("icon_travel_plan_generation.jpg"),
            )
        menubar.append_item( bitjoinrootmenu + '/plan selector',
            self.on_planselection, 
            bitmap = self.get_icon("icon_plan_selection.jpg"),
            )
        menubar.append_item( bitjoinrootmenu + '/clear households',
            self.on_clear_households, 
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            )
        menubar.append_item( bitjoinrootmenu + '/clear activities',
            self.on_clear_activities, 
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            )
        # ~ menubar.append_item( bitjoinrootmenu + '/bitjoin activity based demand generator/clear polulation, vehicles, activities and travel plans',
            # ~ self.on_clear_pop, 
            # ~ bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            # ~ )
        # ~ menubar.append_item( bitjoinrootmenu + '/bitjoin activity based demand generator/clear activities',
            # ~ self.on_clear_activities, 
            # ~ bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            # ~ )
        # ~ menubar.append_item( bitjoinrootmenu + '/activity-based demand generation/clear travel plans',
            # ~ self.on_clear_plans, 
            # ~ bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            # ~ )
            
    def on_generate_vp_from_households(self, event=None):
        """
        Generate households from residential facilities by statistics..
        """
        
        #self._demand.trips.clear_routes()
        
                    
        self.proc = bitjoin.VpFromHouseholdsGenerator(  'virtualpopgenerator',
                                                self._demand.virtualpop, 
                                                logger = self._mainframe.get_logger()
                                                )
                
        dlg = ProcessDialogInteractive( self._mainframe, 
                                        self.proc,
                                        title = 'Generate virtualpop from households',
                                        func_close = self.close_process,
                                        is_close_after_completion = True,
                                        )
                         
        dlg.CenterOnScreen()
        
        # this does not return until the dialog is closed.
        #val = dlg.ShowModal()
        #print('on_generate_vp_from_households')
        dlg.Show()
        #dlg.MakeModal(True) 
    
    def on_generate_activity_from_landuse_facilities(self, event=None):
        """
        Generate daily activity from landuse facilities
        """      
                    
        self.proc = bitjoin.DailyActivityFromLandUseGenerator(  'activity_from_landuse_generator',
                                                self._demand.virtualpop, 
                                                logger = self._mainframe.get_logger()
                                                )
                
        dlg = ProcessDialogInteractive( self._mainframe, 
                                        self.proc,
                                        title = 'Generate daily activity from landuse facilities',
                                        func_close = self.close_process,
                                        is_close_after_completion = True,
                                        )
                         
        dlg.CenterOnScreen()
        
        # this does not return until the dialog is closed.
        #val = dlg.ShowModal()
        print('on_generate_activity_from_landuse_facilities')
        dlg.Show()
        #dlg.MakeModal(True)
        
    def on_generate_activity_from_landuse_facilities(self, event=None):
        """
        Generate daily activity from landuse facilities
        """      
                    
        self.proc = bitjoin.DailyActivityFromLandUseGenerator(  'activitygenerator',
                                                self._demand.virtualpop, 
                                                logger = self._mainframe.get_logger()
                                                )
                
        dlg = ProcessDialogInteractive( self._mainframe, 
                                        self.proc,
                                        title = 'Generate daily activity from landuse facilities',
                                        func_close = self.close_process,
                                        is_close_after_completion = True,
                                        )
                         
        dlg.CenterOnScreen()
        
        # this does not return until the dialog is closed.
        #val = dlg.ShowModal()
        print('on_generate_activity_from_landuse_facilities')
        dlg.Show()
        #dlg.MakeModal(True)
        
        self._mainframe.browse_obj(self._demand.virtualpop.get_activities())

    def on_generate_households_from_residential_facilities(self, event=None):
        """
        Generate households from residential facilities by statistics..
        """
        
        #self._demand.trips.clear_routes()
        
                    
        self.proc = bitjoin.HouseholdsFromFacilitiesGenerator(  'householdgenerator',
                                                self._demand.virtualpop, 
                                                logger = self._mainframe.get_logger()
                                                )
                         
        
        dlg = ProcessDialogInteractive( self._mainframe, 
                                        self.proc,
                                        title = 'Generate households from residential facilities',
                                        func_close = self.close_process,
                                        )
                         
        dlg.CenterOnScreen()
        
        # this does not return until the dialog is closed.
        #val = dlg.ShowModal()
        print('on_generate_households_from_residential_facilities')
        dlg.Show()
        #dlg.MakeModal(True) 
        self._mainframe.browse_obj(self._demand.virtualpop.get_households())
        
        
    def on_clear_households(self, event=None):
        self._demand.virtualpop.clear_households()
        self._mainframe.browse_obj(self._demand.virtualpop.get_households())
    
    def on_clear_activities(self, event=None):
        self._demand.virtualpop.clear_activities()
        self._mainframe.browse_obj(self._demand.virtualpop)
            
    
