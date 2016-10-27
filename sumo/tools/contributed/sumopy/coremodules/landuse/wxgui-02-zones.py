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


class ZoneDrawings(Polygons):
    def __init__(self, zones, parent,   **kwargs):
        

        
        Polygons.__init__(  self,'zonedraws',  parent, 
                            name = 'Transport assignment zone drawings', 
                            linewidth = 1,
                            detectwidth = 3.0,
                            **kwargs)
        
        
        
        
                                        
        self.delete('vertices')
        
        
        self.add(cm.AttrConf(  'color_default', np.array([0.3,0.9,0.5,0.8], dtype=np.float32),
                                        groupnames = ['options'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default facility color.',
                                        ))
                                        
        
                                        
        self.set_netelement(zones)
        
        
    def get_netelement(self):
        return self._zones
    
    def get_vertices_array(self):
        if self._is_not_synched:
            self._inds_map = self._zones.get_inds(self._zones.get_ids())
            self._is_not_synched = False
        return self._zones.shapes.value[self._inds_map]
    
    def set_netelement(self, zones):
        #print '\nset_facilities',facilities.get_ids()
        self._zones = zones
        if len(self)>0:
            self.del_rows(self.get_ids())
        
        ids = self._zones.get_ids()
        #self._inds_map = self._zones.get_inds(ids)
        self.add_rows(ids = ids)
        
        # plugins to keep grapgics syncronized with netelements
        zones.shapes.plugin.add_event(cm.EVTADDITEM,self.on_add_element)
        zones.plugin.add_event(cm.EVTDELITEM,self.on_del_element)
        self._id_target = -1
        self.update()
    
    def update(self, is_update = True):
        # assumes that arrsy structure did not change
        #print 'FacilityDrawings.update'
        n = len(self) 
        self.colors.value[:] = np.ones((n,4),np.float32)*self.color_default.value
        self.colors_highl.value[:] = self._get_colors_highl(self.colors.value)
        
        if is_update:
            self.update_internal()

    def update_internal(self):
        print 'update_internal'
        print '    len(self),len(self._zones)',len(self),len(self._zones)
        self._is_not_synched = True
        self._update_vertexvbo()
        self._update_colorvbo()
        
    def make_zone(self, shape, color ):
        print 'add_zone shape',shape,type(shape)
        return self._zones.make(shape = shape)
        
        
    def on_add_element(self, shapes, ids):
        print 'on_add_element',shapes.attrname,ids
        if shapes == self._zones.shapes:
            self._id_target = ids[0]
            self.add_row(   _id = self._id_target,
                            colors = self.color_default.value,
                            colors_highl = self._get_colors_highl(self.color_default.value)
                            )
            #self._inds_map = self._zones.get_inds(self._zones.get_ids())
            #self._update_vertexvbo()
            #self._update_colorvbo()
            
    
    
    
    def begin_animation(self, id_target):
        print 'ZoneDrawings.begin_animation zones.shapes=\n',id_target,self._id_target, self._zones.shapes[id_target]
        if self._id_target ==-1:
            self._id_target = id_target
        self._drawobj_anim = self.parent.get_drawobj_by_ident(self._ident_drawobj_anim)
        self.id_anim = self._drawobj_anim.add_drawobj(  self._zones.shapes[self._id_target], 
                                                        self.color_anim.value,
                                                        )
        #print 'begin_animation',self.ident,_id,self._drawobj_anim
        return True
            
    def end_animation(self, is_del_last_vert = False):
        print 'ZoneDrawings.end_animation',self.ident,self._id_target,self.id_anim
        

        #print '  verices =',self._drawobj_anim.vertices[self.id_anim]
        print '  self._drawobj_anim.vertices[self.id_anim]=',self._drawobj_anim.vertices[self.id_anim]
        self._zones.shapes[self._id_target] = self._drawobj_anim.vertices[self.id_anim]#.copy()
        
        self.del_animation()
        print '  self.get_vertices_array()=\n',self.get_vertices_array()
        #self._drawobj_anim.del_drawobj(self.id_anim)
        self.update_internal()
        #self._inds_map = self._zones.get_inds(self._zones.get_ids())
        #self._update_vertexvbo()
        #self._update_colorvbo()
        return True
    
    def del_elem(self, id_zone):
        """
        Deletes an element from network and then in on canvas
        through callback on_del_element 
        """
        print 'del_elem'
        print '    len(self),len(self._zones)',len(self),len(self._zones)
        
        self._zones.del_element(id_zone)
        
    def on_del_element(self,shapes,ids):
        """
        callback from netelement
        """
        print 'on_del_element',shapes.attrname,ids,self._id_target
        print '    len(self),len(self._zones)',len(self),len(self._zones)
        #print '  verices =',self._drawobj_anim.vertices[self.id_anim]
        self._is_not_synched = True
        self.del_drawobj(ids[0])
        print '    len(self),len(self._zones)',len(self),len(self._zones)
        #wx.CallAfter(self.update_internal)
        #self.update_internal()
        print '  after CallAfter'
        print '    len(self),len(self._zones)',len(self),len(self._zones)
        return True
    
     
        
    def del_animation(self, is_del_main = False):
        #print 'end_animation',self.ident,_id,self._drawobj_anim
        self._drawobj_anim.del_drawobj(self.id_anim)
        self._drawobj_anim = None
        
        if is_del_main:
            #self.del_drawobj(self._id_target)
            # delete first element from net, which will
            # call back on_del_netelement where the main drawobj is deleted
            self.del_elem(self._id_target)
        
        self._id_target = -1
        self.id_anim = -1
        return True
            
class AddZoneTool(AddPolygonTool):
    """
    Mixin for Selection tools for OGL canvas.
    """ 
    def __init__(self,parent, mainframe=None):
        self.init_common(   'add_zone',parent,'Add zone tool', 
                            info = 'Click on canvas to add a zone. Add a vertex with a single click, double click to finish, right click to aboard.',
                            is_textbutton = False,
                            )
        self.init_options()
    
    def init_options(self):
        pass
        #self.add(cm.AttrConf( 'color',  np.array([0.3,0.9,0.5,0.8], dtype=np.float32),
        #                                groupnames = ['options'], 
        #                                perm='rw', 
        #                                metatype = 'color',
        #                                name = 'Zone color',
        #                                is_save = True,
        #                                info = 'Zone outline color with format [r,g,b,a]',
        #                                ))
                                        
        #self.add(cm.AttrConf( 'color_fill',  np.array([1.0,0.0,0.0,0.8], dtype=np.float32),
        #                                groupnames = ['options'], 
        #                                perm='rw', 
        #                                metatype = 'color',
        #                                name = 'Fill color',
        #                                is_save = True,
        #                                info = 'Object fill color with format [r,g,b,a]',
        #                                ))   
            
    def set_button_info(self,bsize=(32,32)):
        #print 'set_button_info select tool'  
        self._bitmap = wx.Bitmap(os.path.join(IMAGEDIR,'polygon2_24px.png'),wx.BITMAP_TYPE_PNG)
        self._bitmap_sel=wx.Bitmap(os.path.join(IMAGEDIR,'polygon2_24px.png'),wx.BITMAP_TYPE_PNG)
        



    def begin_animation(self, event):
        #print 'AddLineTool.begin_animation'
        #self.drawobj_anim, _id, self.ind_vert =  self.get_current_vertexselection()
        self.drawobj_anim = self._canvas.get_drawing().get_drawobj_by_ident('zonedraws')
        self.coord_last = self._canvas.unproject(event.GetPosition())
        #vertices = [list(self.coord_last),list(self.coord_last) ]
        vertices = [ 1.0*self.coord_last, 1.0*self.coord_last,  ]# attention, we need copies here!!
        #print '  vertices ',vertices#,self.width.get_value(),self.color.get_value(),
        
        _id = self.drawobj_anim.make_zone(vertices,
                                            color = self.drawobj_anim.color_default.get_value(),
                                            )
        self.ind_vert = 1                                   
        self.drawobj_anim.begin_animation(_id)
        
        
        # http://www.wxpython.org/docs/api/wx.Cursor-class.html
        self._canvas.SetCursor(wx.StockCursor(wx.CURSOR_CROSS))
        self.is_animated = True
        
        self.parent.refresh_optionspanel(self)
        return True # True for redrawing
    
       
            
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
        is_draw = False
        if self._landuse != scenario.landuse:
            del self._landuse
            self._landuse = scenario.landuse
            neteditor = self.get_neteditor()
            canvas = self.get_neteditor().get_canvas()
            drawing = canvas.get_drawing()
            drawing.del_drawobj_by_ident('facilitydraws')# TODO: could be done better
            drawing.del_drawobj_by_ident('zonedraws')
            facilitydrawings = FacilityDrawings(self._landuse.facilities, drawing)
            zonedrawings = ZoneDrawings(self._landuse.zones, drawing)
            drawing.add_drawobj(facilitydrawings, layer = 5)
            drawing.add_drawobj(zonedrawings, layer = 100)
            
            neteditor.get_toolbox().add_toolclass(AddZoneTool)
            is_draw = True
        
        elif self._landuse.is_modified():
            #print '   is_modified',self._landuse.is_modified()
            neteditor = self.get_neteditor()
            canvas = self.get_neteditor().get_canvas()
            drawing = canvas.get_drawing()
            drawing.del_drawobj_by_ident('facilitydraws')# TODO: could be done better
            drawing.del_drawobj_by_ident('zonedraws')
            facilitydrawings = FacilityDrawings(self._landuse.facilities, drawing)
            zonedrawings = ZoneDrawings(self._landuse.zones, drawing)
            drawing.add_drawobj(facilitydrawings, layer = 5)
            drawing.add_drawobj(zonedrawings, layer = 100)
            neteditor.get_toolbox().add_toolclass(AddZoneTool)
            is_draw = True
        
        
        
        if is_draw:
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
            

