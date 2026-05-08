import os
import wx
import numpy as np

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.ogleditor import *
from agilepy.lib_base.processes import Process
from agilepy.lib_wx.processdialog import ProcessDialog

from coremodules.network import routing 
from coremodules.scenario.scenario import load_scenario

from . import demand,turnflows
from . import origin_to_destination_wxgui as odgui
from . import turnflows_wxgui as turnflowsgui
from . import virtualpop_wxgui as virtualpopgui
from . import detectorflows_wxgui as detectorflowsgui
from . import publictransportservices_wxgui as pt
    
    

class TripDrawings(Polygons):
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


class RoutesDrawings(Polylines):
    def __init__(self,ident, edges, parent,   **kwargs):
        
        # joinstyle
        # FLATHEAD = 0             
        # BEVELHEAD = 1
        Polylines.__init__( self, ident,  parent, name = 'Routes drawings',
                            is_lefthalf = True,
                            is_righthalf = True, 
                            arrowstretch = 1.5,
                            joinstyle = BEVELHEAD,#FLATHEAD,#BEVELHEAD is good for both halfs,
                            **kwargs)
        
        
        
                                        
    

        
        
        self.add(cm.AttrConf(  'width_default', 4.0,
                                        groupnames = ['options'],
                                        perm='wr', 
                                        name = 'Default width', 
                                        info = 'Default route width of drawing.',
                                        ))
                                        
        self.add(cm.AttrConf(  'color_default', np.array([1.0,0.4,0.0,0.6], np.float32),
                                        groupnames = ['options'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Default color', 
                                        info = 'Default route color.',
                                        ))
                                        
        
                                        
        #self.set_netelement(edges)
        

    
    def get_netelement(self):
        return self._routes
    
    #def get_vertices_array(self):
    #    return self._routes.shapes[self.get_ids()]#.value[self._inds_map]#[self.get_ids()]
    
    #def get_widths_array(self):
    #    # double because only the right half is shown
    #    # add a little bit to the width to make it a little wider than the lanes contained  
    #    #return 2.2*self._edges.widths.value[self._inds_map]
    #    return 1.1*self._edges.widths[self.get_ids()]#.value[self._inds_map]
    
    #def get_vertices(self, ids):
    #    return self._edges.shapes[ids]
    
    #def set_vertices(self, ids, vertices, is_update = True):
    #    self._edges.set_shapes(ids, vertices)
    #    if is_update:
    #        self._update_vertexvbo()
    #        self.parent.get_drawobj_by_ident('lanedraws').update()
    #        self.parent.get_drawobj_by_ident('crossingsdraws').update()
    #        self.parent.get_drawobj_by_ident('connectiondraws').update()
            
    #def get_widths(self, ids):
    #    return 1.1*self._edges.widths[ids]
    
    #def set_widths(self, ids, values):
    #    #self._edges.widths[ids] = values/1.1       
    #    pass
    
    def is_tool_allowed(self, tool, id_drawobj = -1):
        """
        Returns True if this tool can be applied to this drawobj.
        Optionally a particular drawobj can be specified with id_drawobj.
        """
        # basic tools:
        return tool.ident not in   ['configure','select_handles','delete','move','stretch']
        #return tool.ident not in   ['delete',]
    
    def set_netelement(self, routes):
        
        self._routes = routes
        #self._inds_edges = self._edges.get_inds()
        self.clear_rows()
        #if len(self)>0:
        #    self.del_rows(self.get_ids())
        
        ids = self._routes.get_ids()
        #self._inds_map = self._edges.get_inds(ids)
        n = len(ids) 
        #self.vertices = self._edges.shapes
        #self.widths = self._edges.widths
        #print ('\n\nRoutesDrawings.set_netelement',n)
        #print ('  ids.dtype',ids.dtype)
        #print ( '  self._ids.dtype',self._ids.dtype)
        #print ('  self._inds.dtype',self._inds.dtype)  
        #print ('  ids',ids)
        self.add_rows(      ids = ids, 
                            beginstyles = np.ones(n,dtype = np.float32)*FLATHEAD,
                            endstyles = np.ones(n,dtype = np.float32)*TRIANGLEHEAD,
                            widths = np.ones(n,dtype = np.float32)*self.width_default.get_value()
                            )
        self.vertices[ids] = self._routes.get_shapes(ids)
        self.update()
        
    def update(self, is_update = True):
        """
        Update color, assume that there have not been structural changes of the arrays
        """
        # assumes that edges have been set in set_edges
        print ('RoutesDrawings.update')
        #edgeinds = self._edges.get_inds()
        n = len(self)
        ids = self.get_ids()
        
        self.colors_fill.value[:] = self._routes.colors[ids]
        #self.colors_fill.value[:] = np.ones((n,1),np.float32)*self.color_default.value
        self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)
        
        if is_update:
            print ('  now update vbos')
            self._update_vertexvbo()
            self._update_colorvbo()

       
            
class WxGui(    turnflowsgui.TurnflowWxGuiMixin,
                odgui.OdFlowsWxGuiMixin, 
                virtualpopgui.VirtualpopWxGuiMixin,
                detectorflowsgui.DetectorflowsWxGuiMixin,
                pt.PublicTransportWxGuiMixin,
                ModuleGui):
                    
    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """


    def __init__(self, ident):
        self._demand = None
        self._init_common(  ident,  priority = 1000,
                            icondirpath = os.path.join(os.path.dirname(__file__), 'images'))
        
    
    def get_module(self):
        return self._demand
        
    def get_scenario(self):
        return self._mainframe.get_modulegui('coremodules.scenario').get_scenario()
    
    def get_neteditor(self):
        return self._mainframe.get_modulegui('coremodules.network').get_neteditor()

    def get_canvas(self):
        return self.get_neteditor().get_canvas()
    
    def get_drawing(self):
        return self.get_canvas().get_drawing()

    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        #print ('demand init_widgets')
        self._mainframe = mainframe
        #self._neteditor = mainframe.add_view("Network", Neteditor)
        
        #mainframe.browse_obj(self._module)
        self.make_menu()
        self.make_toolbar()
        self._is_needs_refresh = True

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        scenario = self.get_scenario()
        #print ('demand refresh_widgets',scenario.net,'_is_needs_refresh',self._is_needs_refresh)
        is_refresh = False
        if self._demand != scenario.demand:
            del self._demand
            self._demand = scenario.demand
            is_refresh = True
            #self.get_neteditor().get_toolbox().add_toolclass(AddZoneTool)
            #print '  odintervals',self._demand.odintervals#,self.odintervals.times_start
            #print ' ',dir(self._demand.odintervals)
            
        
        elif self._demand.is_modified():
            is_refresh = True
        
        
                
        self.refresh_odflow(is_refresh)
        self.refresh_turnflow(is_refresh)        
        self.refresh_pt(is_refresh)  
        self.refresh_detectorflow(is_refresh)
        
        if is_refresh | self._is_needs_refresh:
            self._is_needs_refresh = False
            #print '  is_refresh',is_refresh,self._is_needs_refresh
            neteditor = self.get_neteditor()
            #canvas = self.get_canvas()
            drawing = self.get_drawing()#canvas.get_drawing()
            
            # add or refresh routes drawing
            # avoid plotting routes in this way, it may jam the memory
            #drawing.set_element('routesdraws', RoutesDrawings, 
            #                        self._demand.trips.get_routes(), layer = 145, is_visible = False)

                            
            #neteditor.get_toolbox().add_toolclass(AddZoneTool)# will check if tool is already there
            #neteditor.draw()

    def make_menu(self):
        #print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu('demand')
        
        menubar.append_item( 'demand/browse',
            self.on_browse_obj, # common function in modulegui
            info='View and browse demand in object panel.',
            bitmap = self.get_agileicon('icon_browse_24px.png'),#,
            )
        #----------------------------------------------------------------------
        menubar.append_menu( 'demand/vehicles',
            bitmap = self.get_icon("vehicle_24px.png"),
            ) 
        
        menubar.append_item( 'demand/vehicles/clear vehicle types',
            self.on_clear_vtypes, 
            info='Remove all vehicle types .',
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            )
        
        menubar.append_item( 'demand/vehicles/import vehicle types from xml...',
            self.on_import_vtypes_xml, 
            info='Import vehicle types from xml file',
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            )
        menubar.append_item( 'demand/vehicles/export vehicle types from xml...',
            self.on_export_vtypes, 
            bitmap = self.get_agileicon("Document_Export_24px.png"),
            )
        menubar.append_item( 'demand/vehicles/load defaults',
            self.on_load_vtypes_defaults, 
            info='Load default vehicle types, removing all existing vehicle types.',
            #bitmap = self.get_icon("route3_24px.png"),
            )
        
        
        #----------------------------------------------------------------------
        self.add_menu_odflows(menubar)
        #----------------------------------------------------------------------
        self.add_menu_turnflow(menubar)
        #----------------------------------------------------------------------
        self.add_menu_detectorflow(menubar)
        #----------------------------------------------------------------------
        self.add_menu_pt(menubar) 
        #----------------------------------------------------------------------
        

        menubar.append_menu( 'demand/trips and routes',
            bitmap = self.get_icon("trip3_24px.png"),
            )  
        
        menubar.append_item( 'demand/trips and routes/quickroute with fastest path',
            self.on_route, 
            bitmap = self.get_icon("route3_24px.png"),
            )
        #menubar.append_item( 'demand/trips and routes/trips to routes with fastest path, del. disconnected',
        #    self.on_route_del_disconnected, 
        #    bitmap = self.get_icon("route3_24px.png"),
        #    )
            
        menubar.append_item( 'demand/trips and routes/duarouter fastest route',
            self.on_duaroute, 
            bitmap = self.get_icon("route3_24px.png"),
            )
                
        menubar.append_item( 'demand/trips and routes/duarouter stochastic assignment...',
            self.on_duaroute_stochastic, 
            bitmap = self.get_icon("route3_24px.png"),
            )
        
        menubar.append_item( 'demand/trips and routes/scale trip number...',
            self.on_scale_trips, 
            )
            
        #menubar.append_item( 'demand/trips and routes/trips to routes with congested net ...',
        #    self.on_route_congested, 
        #    bitmap = self.get_icon("route3_24px.png"),
        #    )
        
        # menubar.append_item( 'demand/trips and routes/generate taxi trips...',
        #     self.on_generate_taxitrips, 
        #     )

        # menubar.append_item( 'demand/trips and routes/generate SAV trips...',
        #     self.on_generate_SAVtrips, 
        #     )
                        
        menubar.append_item( 'demand/trips and routes/export trips to SUMO trip.xml...',
            self.on_export_sumotrips, 
            info='Export all trips to SUMO XML format.',
            bitmap = self.get_agileicon("Document_Export_24px.png"),
            ) 
        
        #menubar.append_item( 'demand/trips and routes/export routes to SUMO xml...',
        #    self.on_export_sumoroutes, 
        #    info='Export all trips to SUMO XML format.',
        #    bitmap = self.get_agileicon("Document_Export_24px.png"),
        #    ) 
        menubar.append_menu( 'demand/trips and routes/import',
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            )
            
        menubar.append_item( 'demand/trips and routes/import/import trips from SUMO trip.xml...',
            self.on_import_trips_xml, 
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            ) 
        menubar.append_item( 'demand/trips and routes/import/import routes with trips from SUMO rou.xml...',
            self.on_import_routes_xml, 
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            )
        menubar.append_item( 'demand/trips and routes/import/import trips with routes from SUMO trip.xml...',
            self.on_import_triproutes_xml, 
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            ) 
        
        
        menubar.append_item( 'demand/trips and routes/import/import and overwrite routes of existing trips from SUMO xml...',
            self.on_import_routes_overwrite_xml, 
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            )
            
        menubar.append_item( 'demand/trips and routes/import/import and add routes to existing trips from SUMO xml...',
            self.on_import_routes_alternative_xml, 
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            )
            
        
        menubar.append_item( 'demand/trips and routes/import/import trips from scenario...',
            self.on_import_trips_from_scenario, 
            info='Import trips from another scenario',
##          bitmap = self.get_agileicon("Document_Import_24px.png"),
            )
        
        
            
        menubar.append_item( 'demand/trips and routes/clear all trips and routes',
            self.on_clear_trips, 
            info='Clear all trips with respective routes.',
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            )
            
        menubar.append_item( 'demand/trips and routes/clear all routes',
            self.on_clear_routes, 
            info='Clear all routes, trips will remain.',
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            )
            
        menubar.append_item( 'demand/trips and routes/clear alternative routes',
            self.on_clear_route_alternatves, 
            info='Clear alternative routes, only current route will remain.',
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            )
        #----------------------------------------------------------------------
        
        self.add_menu_virtualpop(menubar)      
        
        #----------------------------------------------------------------------
        menubar.append_item( 'demand/export all routes to SUMO xml...',
            self.on_export_all_sumoroutes, 
            bitmap = self.get_agileicon("Document_Export_24px.png"),
            )  
        
        menubar.append_item( 'demand/export all routes to SUMO xml special...',
            self.on_export_all_sumoroutes_flat, 
            bitmap = self.get_agileicon("Document_Export_24px.png"),
            ) 
            
        menubar.append_item( 'demand/update all routes from SUMO xml...',
            self.on_update_all_sumoroutes,
            info='Update all routes available in the demand database from SUMO XML format. Note that no new routes are going to be imported.',
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            ) 
        
        menubar.append_item( 'demand/update with route alternatives from Macrorouter...',
            self.on_update_all_sumoroutes_alternatives,
            bitmap = self.get_agileicon("Document_Import_24px.png"),
            )
        menubar.append_item( 'demand/Redraw Canvas.',
            self.on_redraw,
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
    
    def on_redraw (self, event=None):   
        self._mainframe.browse_obj(self._demand)     
        self._is_needs_refresh = True
        self.refresh_widgets() 
        
    def on_import_routes_xml(self, event = None):
        """
        Import trips with routes from a sumo route xml file. Trips will be generated, not substituted or updated.
        """
        print('on_import_routes_xml**')
        filepath = self._demand.get_routefilepath()
        # defaultFile =  = os.path.basename(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Update all routes from SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_OPEN | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.trips.import_routes_xml( filepath,  is_clear_trips = False,
                            is_generate_ids = True, is_add = False,
                            is_overwrite_only = False)
          
    def on_clear_vtypes(self, event=None):
        self._demand.vtypes.clear_vtypes()
        self._mainframe.browse_obj(self._demand.vtypes)
        #if event:  event.Skip()
        
    def on_load_vtypes_defaults(self, event=None):
        self._demand.vtypes.clear_vtypes()
        self._demand.vtypes.eprofiles.get_value().add_profiles_default()
        self._demand.vtypes.add_vtypes_default()
        self._mainframe.browse_obj(self._demand.vtypes)
        #if event:  event.Skip()
        
    def on_clear_trips(self, event=None):
        self._demand.trips.clear_trips()
        self._mainframe.browse_obj(self._demand.trips)
    
    
    def on_clear_routes(self, event=None):
        self._demand.trips.clear_routes()
        self._mainframe.browse_obj(self._demand.trips)
 
    
    def on_clear_route_alternatves(self, event=None):
        self._demand.trips.clear_route_alternatves()
        self._mainframe.browse_obj(self._demand.trips)    
    
    
        
    
                      
    def on_route(self, event=None):
        """Generates routes from current trip info. Uses a python implementation of a fastest path router.
        """
        #self._demand.trips.clear_routes()
        self._demand.trips.route(is_set_current = True)
        self._mainframe.browse_obj(self._demand.trips)
        
    #def on_route_del_disconnected(self, event=None):
    #    """Generates routes from current trip info and deletes disconnected trips. Uses a python implementation of a fastest path router.
    #    """
    #    #self._demand.trips.clear_routes()
    #    self._demand.trips.route(is_del_disconnected = True)
    #    self._mainframe.browse_obj(self._demand.trips)
        
    def on_duaroute(self, event=None):
        """Fastest route traffic assignment with duarouter.
        """
        #self._demand.trips.clear_routes()
        self._demand.trips.duaroute(is_export_net = True, is_export_trips = True)
        self._mainframe.browse_obj(self._demand.trips)
    
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
                
    def on_duaroute_stochastic(self, event=None):
        """Stochastic traffic assignment with duarouter.
        """
        #self._demand.trips.clear_routes()
        
                    
        obj = routing.DuaRouter(    self.get_scenario().net, 
                                    trips = self._demand.trips, 
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
            
            # this should update all widgets for the new scenario!!
            #print 'call self._mainframe.refresh_moduleguis()'
            #self._mainframe.refresh_moduleguis()
            
    
    def on_scale_trips(self, event=None):
        """Scale number of trips.
        """
        obj = demand.Tripscaler(    self._demand, 
                                logger = self._mainframe.get_logger())
        
        
        dlg = ProcessDialog(self._mainframe, obj)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()

        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()
            
            self._mainframe.browse_obj(self._demand.trips)
    
    
            
                    
    def on_import_vtypes_xml(self, event = None):
        """Select xml file and import new vehicle types. 
        """
        filepath = self._demand.trips.get_routefilepath()
        # defaultFile =  = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML files (*.xml)|*.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import vtypes from SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.vtypes.import_xml( filepath)
        self._mainframe.browse_obj(self._demand.vtypes)
        
    def on_import_trips_xml(self, event = None):
        """Select xml file and import new trips. 
        New trips with associated routes will generated. 
        """
        filepath = self._demand.trips.get_routefilepath()
        # defaultFile =  = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML trip files (*.trip.xml)|*.trip.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import trips and routes from SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.trips.import_trips_xml( filepath, is_clear_trips = False, 
                                              is_generate_ids = True)
        self._mainframe.browse_obj(self._demand.trips)
        
        
    def on_import_triproutes_xml(self, event = None):
        """Select xml file and import trips and routes. 
        New trips with associated routes will generated. 
        """
        filepath = self._demand.trips.get_routefilepath()
        # defaultFile =  = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*.rou.xml)|*.rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import trips and routes from SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.trips.import_routes_xml( filepath, is_clear_trips = False, 
                                              is_generate_ids = True,
                                              is_add = False)
        self._mainframe.browse_obj(self._demand.trips)
    
        
    
    def on_import_routes_overwrite_xml(self, event = None):
        """Select xml file and import routes. 
        Overwrite current routes of existing trips. 
        """
        filepath = self._demand.trips.get_routefilepath()
        # defaultFile =  = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*.rou.xml)|*.rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import trips and routes from SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_OPEN | wx.FD_CHANGE_DIR
                            )
                            
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.trips.import_routes_xml(filepath,is_clear_trips = False, 
                                                is_overwrite_only = True,
                                                is_generate_ids = False,
                                                is_add = False)
        self._mainframe.browse_obj(self._demand.trips)
                                      
    def on_import_routes_alternative_xml(self, event = None):
        """Select xml file and import routes. 
        Add routes as alternative of existing trips. 
        """
        filepath = self._demand.trips.get_routefilepath()
        # defaultFile =  = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*.rou.xml)|*.rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import trips and routes from SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_OPEN | wx.FD_CHANGE_DIR
                            )
                            
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.trips.import_routes_xml(filepath,is_clear_trips = False, 
                                                is_generate_ids = False,
                                                is_add = True)
        self._mainframe.browse_obj(self._demand.trips)
    
    def on_import_trips_from_scenario(self, event = None):
        """Select xml file and import new vehicle types. 
        """
        filepath = self.get_scenario().get_rootfilepath()
        ## defaultFile =  = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "Python binary files (*.obj)|*.obj"
        wildcards = wildcards_obj+"|"+wildcards_all
        dlg = wx.FileDialog(
            self._mainframe, message="Open scenario file",
            defaultDir = dirpath, 
            # # defaultFile =  = filepath+'.obj',
            wildcard=wildcards,
            style=wx.FD_OPEN | wx.FD_CHANGE_DIR
            )
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        scenario2 = load_scenario(filepath, logger = None)
        self._demand.trips.import_trips_from_scenario(scenario2)
        self._mainframe.browse_obj(self._demand.trips)
        
        
    def on_export_sumotrips(self, event = None):
        
        filepath = self._demand.trips.get_tripfilepath()
        dirpath = os.path.dirname(filepath)
        print ('on_export_sumotrips default dirpath',dirpath,'filepath',filepath)
        # defaultFile =  = os.path.basename(filepath)
        
        wildcards_all = 'XML trip files (*trip.xml)|*trip.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Write trips to SUMO xml', 
                            #defaultDir=dirpath, 
                            defaultFile = filepath, 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
                            
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.trips.export_trips_xml(filepath)
    
    
            
    def on_export_vtypes(self, event = None):
        """
        Write vtypes to SUMO xml
        """
        #print 'on_export_vtypes'
        filepath = self._demand.trips.get_tripfilepath()
        # defaultFile =  = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = '*.*|XML vtype files (*.vtype.xml)|*.vtype.xml|All files (*.*)'
        dlg = wx.FileDialog(None, message='Write vtypes to SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.vtypes.export_xml(filepath)    
    
    
    #def on_export_sumoroutes(self, event = None):
    #    #print 'on_export_sumotrips'
    #    filepath = self._demand.trips.get_routefilepath()
    #    # defaultFile =  = os.path.basename(filepath)
    #    dirpath = os.path.dirname(filepath)
    #    wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
    #    dlg = wx.FileDialog(None, message='Write routes to SUMO xml', 
    #                        defaultDir=dirpath, 
    #                        # defaultFile = =# defaultFile = , 
    #                        wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
    #    if dlg.ShowModal() == wx.ID_OK:
    #        filepath = dlg.GetPath()
    #
    #    else:
    #        return 
    #    
    #    self._demand.trips.export_routes_xml(filepath)
        
        
   
    def on_export_all_sumoroutes(self, event = None):
        """
        Export all routes available in the demand database to SUMO XML format..
        """
           
        #print 'on_export_sumotrips'
        filepath = self._demand.get_routefilepath()
        # defaultFile =  = os.path.basename(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Write all routes to SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.export_routes_xml(filepath)
    
    def on_export_all_sumoroutes_flat(self, event = None):
        """
        Generates the routes for the entire demand database as trips.
        No plans are generated, only empty vehicle trips.
        """
        #print 'on_export_sumotrips'
        filepath = self._demand.get_routefilepath()
        # defaultFile =  = os.path.basename(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Write all routes to SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.export_routes_xml(filepath,encoding = 'UTF-8',
                                is_route = True, # allow True if route export is implemened in virtual population self.is_skip_first_routing,# produce routes only if first dua routing is skipped
                                #vtypeattrs_excluded = ['times_boarding','times_loading'],# bug of duaiterate!!
                                is_plain = True, # this will prevent exporting stops and plans 
                                is_exclude_pedestrians = True, # no pedestriann trips, but plans are OK
                                )
            
    def on_export_all_sumoroutes(self, event = None):
        #print 'on_export_sumotrips'
        filepath = self._demand.get_routefilepath()
        # defaultFile =  = os.path.basename(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Write all routes to SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_SAVE | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.export_routes_xml(filepath)
        
            
    def on_update_all_sumoroutes_alternatives(self, event = None):
        """
        Update routes from SUMO route alternative file
        """
        #print 'on_export_sumotrips'
        filepath = self._demand.get_routefilepath()
        # defaultFile =  = os.path.basename(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML files (*.mrflow.xml)|*mrflow.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Update all routes from route dists SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_OPEN | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.import_routealternatives_xml(filepath, is_fastest = True)# is_fastest = False means modet probable instead of fastest



    def on_update_all_sumoroutes(self, event = None):
        #print 'on_export_sumotrips'
        filepath = self._demand.get_routefilepath()
        # defaultFile =  = os.path.basename(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Update all routes from SUMO xml', 
                            defaultDir=dirpath, 
                            ## defaultFile = =# defaultFile = , 
                            wildcard=wildcards_all, style=wx.FD_OPEN | wx.FD_CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return 
        
        self._demand.import_routes_xml(filepath)
