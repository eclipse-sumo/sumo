import os
import wx
import numpy as np

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.ogleditor import *



class FacilityDrawings(Polygons):
    def __init__(self, facilities, parent,   **kwargs):
        

        
        Polygons.__init__(  self,'facilitydraws',  parent, 
                            name = 'Facility drawings', 
                            linewidth = 1,
                            detectwidth = 3.0,
                            **kwargs)
        
        
        
        
                                        
        self.delete('vertices')
        
        
        self.add(cm.AttrConf(  'color_facility_default', np.array([0.921875,  0.78125 ,  0.4375 , 1.0], np.float32),
                                        groupnames = ['options'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default facility color.',
                                        ))
                                        
        
                                        
        self.set_facilities(facilities)
        
        
    def get_netelement(self):
        return self._facilities
    
    def get_vertices_array(self):
        return self._facilities.shapes.value[self._inds_map]
    
    def set_facilities(self, facilities):
        #print '\nset_facilities',facilities.get_ids()
        self._facilities = facilities
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._facilities.get_ids()
        self._inds_map = self._facilities.get_inds(ids)
        self.add_rows(ids = ids)
        self.update()
        
    def update(self, is_update = True):
        # assumes that arrsy structure did not change
        #print 'FacilityDrawings.update'
        n = len(self) 
        self.colors.value[:] = np.ones((n,4),np.float32)*self.color_facility_default.value
        self.colors_highl.value[:] = self._get_colors_highl(self.colors.value)
        
        landusetypes = self._facilities.get_landusetypes()
        ids_landusetype = self._facilities.ids_landusetype.value
        for id_landusetype in landusetypes.get_ids():
            #inds = np.flatnonzero(ids_landusetype == id_landusetype)
            #color = landusetypes.colors[id_landusetype]
            #self.colors.value[self._inds_map[inds]] = color
            self.colors.value[self._inds_map[np.flatnonzero(ids_landusetype == id_landusetype)]] = landusetypes.colors[id_landusetype]
            
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()

class FacilityDrawings(Polygons):
    def __init__(self, facilities, parent,   **kwargs):
        

        
        Polygons.__init__(  self,'zonedraws',  parent, 
                            name = 'Transport assignment zone drawings', 
                            linewidth = 1,
                            detectwidth = 3.0,
                            **kwargs)
        
        
        
        
                                        
        self.delete('vertices')
        
        
        self.add(cm.AttrConf(  'color_zone_default', np.array([0.921875,  0.78125 ,  0.4375 , 1.0], np.float32),
                                        groupnames = ['options','edgecolors'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default facility color.',
                                        ))
                                        
        
                                        
        self.set_facilities(facilities)
        
        
    def get_netelement(self):
        return self._facilities
    
    def get_vertices_array(self):
        return self._facilities.shapes.value[self._inds_map]
    
    def set_facilities(self, facilities):
        #print '\nset_facilities',facilities.get_ids()
        self._facilities = facilities
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._facilities.get_ids()
        self._inds_map = self._facilities.get_inds(ids)
        self.add_rows(ids = ids)
        self.update()
        
    def update(self, is_update = True):
        # assumes that arrsy structure did not change
        #print 'FacilityDrawings.update'
        n = len(self) 
        self.colors.value[:] = np.ones((n,4),np.float32)*self.color_facility_default.value
        self.colors_highl.value[:] = self._get_colors_highl(self.colors.value)
        
        landusetypes = self._facilities.get_landusetypes()
        ids_landusetype = self._facilities.ids_landusetype.value
        for id_landusetype in landusetypes.get_ids():
            #inds = np.flatnonzero(ids_landusetype == id_landusetype)
            #color = landusetypes.colors[id_landusetype]
            #self.colors.value[self._inds_map[inds]] = color
            self.colors.value[self._inds_map[np.flatnonzero(ids_landusetype == id_landusetype)]] = landusetypes.colors[id_landusetype]
            
        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()
            
class WxGui(ModuleGui):
    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """


    def __init__(self, ident):
        self._landuse = None
        self._init_common(  ident,  priority = 100,
                            icondirpath = os.path.join(os.path.dirname(__file__), 'images'))
        
    
    def get_module(self):
        return self._landuse
        
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
        #print 'landuse refresh_widgets',scenario.net
        if self._landuse != scenario.landuse:
            del self._landuse
            self._landuse = scenario.landuse
            neteditor = self.get_neteditor()
            canvas = self.get_neteditor().get_canvas()
            drawing = canvas.get_drawing()
            drawing.del_drawobj_by_ident('facilitydraws')# TODO: could be done better
            facilitydrawings = FacilityDrawings(self._landuse.facilities, drawing)
            drawing.add_drawobj(facilitydrawings, layer = 5)
            canvas.draw()
        
        elif self._landuse.is_modified():
            #print '   is_modified',self._landuse.is_modified()
            neteditor = self.get_neteditor()
            canvas = self.get_neteditor().get_canvas()
            drawing = canvas.get_drawing()
            drawing.del_drawobj_by_ident('facilitydraws')# TODO: could be done better
            facilitydrawings = FacilityDrawings(self._landuse.facilities, drawing)
            drawing.add_drawobj(facilitydrawings, layer = 5)
            canvas.draw()
            
           
                


    def make_menu(self):
        #print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu('Landuse')
        menubar.append_item(    'Landuse/import sumo poly file...', 
                                self.on_import_poly, info='Import sumo poly xml file.',
                                #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU)
                                bitmap = self.get_icon("Home-icon_24px.png"),
                                )

##        (self._menuitem_draw_route, id_item,) = menubar.append_item(
##            'plugins/traces/draw selected route in network', 
##            self.on_renew_objectbrowser, 
##            info='Enable visualization of routes in network tab. Double-click on trace in table.',
##            check=True)
##        self._menuitem_draw_route.Check(False)
##        
##        (self._menuitem_plot_route, id_item,) = menubar.append_item(
##            'plugins/traces/add plot selected route', 
##            self.on_renew_objectbrowser, 
##            info='Enable adding of routes to graphics in trace tab. Double-click on trace in table.',
##            check=True)
##        self._menuitem_plot_route.Check(False)
##        
   

    def on_import_poly(self, event=None):
        # TODO: here we could make a nice dialog asking if
        # existing file should be overwritten.
        #self.write_to_statusbar('Import facilities', key='action')
        # Create the dialog. In this case the current directory is forced as the starting
        # directory for the dialog, and no default file name is forced. This can easilly
        # be changed in your program. This is an 'open' dialog, and allows multitple
        # file selections as well.
        #
        scenario = self.get_scenario()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_xml = "Poly xml files (*.poly.xml)|*.poly.xml|XML files (*.xml)|*.xml"
        wildcards = wildcards_xml+"|"+wildcards_all
        
        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Choose one or more poly files",
            defaultDir = scenario.get_workdirpath(), 
            defaultFile = os.path.join(scenario.get_workdirpath(), scenario.format_ident()+'poly.xml'),
            wildcard=wildcards,
            style=wx.OPEN | wx.MULTIPLE | wx.CHANGE_DIR
            )

        # Show the dialog and retrieve the user response. If it is the OK response, 
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            paths = dlg.GetPaths()
            #print 'You selected %d files:' % len(paths)
            if len(paths)>0:
                
                for path in paths:
                    #print '           %s' % path
                    #readNet(path, net = net)
                    dirname = os.path.dirname(path)
                    filename_raw = os.path.basename(path)
                    filename = filename_raw.split('.')[0]
                    self._landuse.import_xml(filename, dirname=dirname)
                    
 
                self._mainframe.refresh_moduleguis()
                #self._mainframe.browse_obj(self._landuse)
                ## inform plugins
                #for plugin in  plugins:
                #    plugin.set_scenario(self.scenario)
         

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()
        
        
        
        if event:
            event.Skip()
            

