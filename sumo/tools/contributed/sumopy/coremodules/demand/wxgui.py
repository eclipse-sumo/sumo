import os
import wx
import numpy as np

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.ogleditor import *
from agilepy.lib_base.processes import Process
from agilepy.lib_wx.processdialog import ProcessDialog
import demand
import turnflows
import origin_to_destination_wxgui as odgui


class TripDrawings(Polygons):

    def __init__(self, facilities, parent,   **kwargs):

        Polygons.__init__(self, 'facilitydraws',  parent,
                          name='Facility drawings',
                          linewidth=1,
                          detectwidth=3.0,
                          **kwargs)

        self.delete('vertices')

        self.add(cm.AttrConf('color_facility_default', np.array([0.921875,  0.78125,  0.4375, 1.0], np.float32),
                             groupnames=['options'],
                             perm='wr',
                             metatype='color',
                             name='Default color',
                             info='Default facility color.',
                             ))

        self.set_facilities(facilities)

    def get_netelement(self):
        return self._facilities

    def get_vertices_array(self):
        return self._facilities.shapes.value[self._inds_map]

    def set_facilities(self, facilities):
        # print '\nset_facilities',facilities.get_ids()
        self._facilities = facilities
        if len(self) > 0:
            self.del_rows(self.get_ids())

        ids = self._facilities.get_ids()
        self._inds_map = self._facilities.get_inds(ids)
        self.add_rows(ids=ids)
        self.update()

    def update(self, is_update=True):
        # assumes that arrsy structure did not change
        # print 'FacilityDrawings.update'
        n = len(self)
        self.colors.value[:] = np.ones(
            (n, 4), np.float32) * self.color_facility_default.value
        self.colors_highl.value[:] = self._get_colors_highl(self.colors.value)

        landusetypes = self._facilities.get_landusetypes()
        ids_landusetype = self._facilities.ids_landusetype.value
        for id_landusetype in landusetypes.get_ids():
            #inds = np.flatnonzero(ids_landusetype == id_landusetype)
            #color = landusetypes.colors[id_landusetype]
            #self.colors.value[self._inds_map[inds]] = color
            self.colors.value[self._inds_map[np.flatnonzero(
                ids_landusetype == id_landusetype)]] = landusetypes.colors[id_landusetype]

        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()


class RouteDrawings(Polygons):

    def __init__(self, zones, parent,   **kwargs):

        Polygons.__init__(self, 'zonedraws',  parent,
                          name='Transport assignment zone drawings',
                          linewidth=4,
                          detectwidth=5.0,
                          **kwargs)

        self.delete('vertices')

        self.add(cm.AttrConf('color_default', np.array([0.3, 0.9, 0.5, 0.8], dtype=np.float32),
                             groupnames=['options'],
                             perm='wr',
                             metatype='color',
                             name='Default color',
                             info='Default facility color.',
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
        # print '\nset_facilities',facilities.get_ids()
        self._zones = zones
        if len(self) > 0:
            self.del_rows(self.get_ids())

        ids = self._zones.get_ids()
        #self._inds_map = self._zones.get_inds(ids)
        self.add_rows(ids=ids)

        # plugins to keep grapgics syncronized with netelements
        zones.shapes.plugin.add_event(cm.EVTADDITEM, self.on_add_element)
        zones.plugin.add_event(cm.EVTDELITEM, self.on_del_element)

        self._id_target = -1
        self.update()

    def update(self, is_update=True):
        # assumes that arrsy structure did not change
        # print 'FacilityDrawings.update'
        n = len(self)
        self.colors.value[:] = np.ones(
            (n, 4), np.float32) * self.color_default.value
        self.colors_highl.value[:] = self._get_colors_highl(self.colors.value)

        if is_update:
            self.update_internal()

    def update_internal(self):
        # print 'update_internal'
        # print '    len(self),len(self._zones)',len(self),len(self._zones)
        self._is_not_synched = True
        self._update_vertexvbo()
        self._update_colorvbo()

    def make_zone(self, shape, color):
        # print 'make_zone shape',shape,type(shape)
        return self._zones.make(shape=shape)

    def on_add_element(self, shapes, ids):
        # print 'on_add_element',shapes.attrname,ids
        if shapes == self._zones.shapes:
            self._id_target = ids[0]
            self.add_row(_id=self._id_target,
                         colors=self.color_default.value,
                         colors_highl=self._get_colors_highl(
                             self.color_default.value)
                         )
            #self._inds_map = self._zones.get_inds(self._zones.get_ids())
            # self._update_vertexvbo()
            # self._update_colorvbo()

    def begin_animation(self, id_target):
        # print 'ZoneDrawings.begin_animation
        # zones.shapes=\n',id_target,self._id_target,
        # self._zones.shapes[id_target]
        if self._id_target == -1:
            self._id_target = id_target
        self._drawobj_anim = self.parent.get_drawobj_by_ident(
            self._ident_drawobj_anim)
        self.id_anim = self._drawobj_anim.add_drawobj(self._zones.shapes[self._id_target],
                                                      self.color_anim.value,
                                                      )
        # print 'begin_animation',self.ident,_id,self._drawobj_anim
        return True

    def end_animation(self, is_del_last_vert=False):
        # print
        # 'ZoneDrawings.end_animation',self.ident,self._id_target,self.id_anim

        # print '  verices =',self._drawobj_anim.vertices[self.id_anim]
        # print '
        # self._drawobj_anim.vertices[self.id_anim]=',self._drawobj_anim.vertices[self.id_anim]
        shape = self._drawobj_anim.vertices[self.id_anim]
        self._zones.shapes[self._id_target] = shape
        self._zones.coords[
            self._id_target] = self._zones.get_coords_from_shape(shape)

        self.del_animation()
        # print '  self.get_vertices_array()=\n',self.get_vertices_array()
        # self._drawobj_anim.del_drawobj(self.id_anim)
        self.update_internal()
        #self._inds_map = self._zones.get_inds(self._zones.get_ids())
        # self._update_vertexvbo()
        # self._update_colorvbo()
        return True

    def del_elem(self, id_zone):
        """
        Deletes an element from network and then in on canvas
        through callback on_del_element 
        """
        # print 'del_elem'
        # print '    len(self),len(self._zones)',len(self),len(self._zones)

        self._zones.del_element(id_zone)

    def on_del_element(self, shapes, ids):
        """
        callback from netelement
        """
        # print 'on_del_element',shapes.attrname,ids,self._id_target
        # print '    len(self),len(self._zones)',len(self),len(self._zones)
        # print '  verices =',self._drawobj_anim.vertices[self.id_anim]
        self._is_not_synched = True
        self.del_drawobj(ids[0])
        # print '    len(self),len(self._zones)',len(self),len(self._zones)
        # wx.CallAfter(self.update_internal)
        # self.update_internal()
        # print '  after CallAfter'
        # print '    len(self),len(self._zones)',len(self),len(self._zones)
        return True

    def del_animation(self, is_del_main=False):
        # print 'end_animation',self.ident,_id,self._drawobj_anim
        self._drawobj_anim.del_drawobj(self.id_anim)
        self._drawobj_anim = None

        if is_del_main:
            # self.del_drawobj(self._id_target)
            # delete first element from net, which will
            # call back on_del_netelement where the main drawobj is deleted
            self.del_elem(self._id_target)

        self._id_target = -1
        self.id_anim = -1
        return True


class AddTripTool(AddPolygonTool):

    """
    Mixin for Selection tools for OGL canvas.
    """

    def __init__(self, parent, mainframe=None):
        self.init_common('add_zone', parent, 'Add zone tool',
                         info='Click on canvas to add a zone. Add a vertex with a single click, double click to finish, right click to aboard.',
                         is_textbutton=False,
                         )
        self.init_options()

    def init_options(self):
        pass
        # self.add(cm.AttrConf( 'color',  np.array([0.3,0.9,0.5,0.8], dtype=np.float32),
        #                                groupnames = ['options'],
        #                                perm='rw',
        #                                metatype = 'color',
        #                                name = 'Zone color',
        #                                is_save = True,
        #                                info = 'Zone outline color with format [r,g,b,a]',
        #                                ))

        # self.add(cm.AttrConf( 'color_fill',  np.array([1.0,0.0,0.0,0.8], dtype=np.float32),
        #                                groupnames = ['options'],
        #                                perm='rw',
        #                                metatype = 'color',
        #                                name = 'Fill color',
        #                                is_save = True,
        #                                info = 'Object fill color with format [r,g,b,a]',
        #                                ))

    def set_button_info(self, bsize=(32, 32)):
        # print 'set_button_info select tool'
        self._bitmap = wx.Bitmap(os.path.join(
            IMAGEDIR, 'polygon2_24px.png'), wx.BITMAP_TYPE_PNG)
        self._bitmap_sel = wx.Bitmap(os.path.join(
            IMAGEDIR, 'polygon2_24px.png'), wx.BITMAP_TYPE_PNG)

    def begin_animation(self, event):
        # print 'AddLineTool.begin_animation'
        #self.drawobj_anim, _id, self.ind_vert =  self.get_current_vertexselection()
        self.drawobj_anim = self._canvas.get_drawing().get_drawobj_by_ident(
            'zonedraws')
        self.coord_last = self._canvas.unproject(event.GetPosition())
        #vertices = [list(self.coord_last),list(self.coord_last) ]
        # attention, we need copies here!!
        vertices = [1.0 * self.coord_last, 1.0 * self.coord_last, ]
        # print '  vertices
        # ',vertices#,self.width.get_value(),self.color.get_value(),

        _id = self.drawobj_anim.make_zone(vertices,
                                          color=self.drawobj_anim.color_default.get_value(),
                                          )
        self.ind_vert = 1
        self.drawobj_anim.begin_animation(_id)

        # http://www.wxpython.org/docs/api/wx.Cursor-class.html
        self._canvas.SetCursor(wx.StockCursor(wx.CURSOR_CROSS))
        self.is_animated = True

        self.parent.refresh_optionspanel(self)
        return True  # True for redrawing


class WxGui(ModuleGui):

    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """

    def __init__(self, ident):
        self._demand = None
        self._init_common(ident,  priority=1000,
                          icondirpath=os.path.join(os.path.dirname(__file__), 'images'))

    def get_module(self):
        return self._demand

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

        # mainframe.browse_obj(self._module)
        self.make_menu()
        self.make_toolbar()

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        scenario = self.get_scenario()
        # print 'demand refresh_widgets',scenario.net
        is_refresh = False
        if self._demand != scenario.demand:
            del self._demand
            self._demand = scenario.demand
            is_refresh = True
            # self.get_neteditor().get_toolbox().add_toolclass(AddZoneTool)
            # print '  odintervals',self._demand.odintervals#,self.odintervals.times_start
            # print ' ',dir(self._demand.odintervals)

        elif self._demand.is_modified():
            is_refresh = True

        if is_refresh:
            neteditor = self.get_neteditor()
            canvas = self.get_neteditor().get_canvas()
            drawing = canvas.get_drawing()
            # drawing.del_drawobj_by_ident('facilitydraws')# TODO: could be done better
            # drawing.del_drawobj_by_ident('zonedraws')
            #facilitydrawings = FacilityDrawings(self._landuse.facilities, drawing)
            #zonedrawings = ZoneDrawings(self._landuse.zones, drawing)
            #drawing.add_drawobj(facilitydrawings, layer = 5)
            #drawing.add_drawobj(zonedrawings, layer = 100)
            # canvas.draw()

    def make_menu(self):
        # print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu('demand')
#-------------------------------------------------------------------------
        menubar.append_menu('demand/vehicles',
                            bitmap=self.get_icon("vehicle_24px.png"),
                            )

        menubar.append_item('demand/vehicles/clear vehicle types',
                            self.on_clear_vtypes,
                            info='Remove all vehicle types .',
                            #bitmap = self.get_icon("route3_24px.png"),
                            )

        menubar.append_item('demand/vehicles/load defaults',
                            self.on_load_vtypes_defaults,
                            info='Load default vehicle types, removing all existing vehicle types.',
                            #bitmap = self.get_icon("route3_24px.png"),
                            )

#-------------------------------------------------------------------------

        menubar.append_menu('demand/Zone-to-zone demand',
                            bitmap=self.get_icon("icon_odm.png"),
                            )
        menubar.append_item('demand/Zone-to-zone demand/add zone-to-zone flows...',
                            self.on_add_odtrips,
                            info='Add or import trips between origin and destination zones, with a certain mode during a certain time interval.',
                            bitmap=self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('demand/Zone-to-zone demand/generate trips from flows',
                            self.on_generate_odtrips,
                            info=self.on_generate_odtrips.__doc__.strip(),
                            #bitmap = wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE_AS,wx.ART_MENU),
                            )

        menubar.append_item('demand/Zone-to-zone demand/clear zone-to-zone flows',
                            self.on_clear_odtrips,
                            info='Clear all zone to zone trips.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_DELETE, wx.ART_MENU),
                            )
#-------------------------------------------------------------------------
        menubar.append_menu('demand/turnflows',
                            bitmap=self.get_icon("icon_turnratio.png"),
                            )
        menubar.append_item('demand/turnflows/import turnflows...',
                            self.on_import_turnflows,
                            info='Import turnflows from file.',
                            bitmap=self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('demand/turnflows/normalize turn-probabilities',
                            self.on_normalize_turnprobabilities,
                            #info='Makes sure that sum of turn probabilities from an edge equals 1.',
                            #bitmap = self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('demand/turnflows/turnflows to routes',
                            self.on_turnflows_to_routes,
                            #info='Makes sure that sum of turn probabilities from an edge equals 1.',
                            #bitmap = self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('demand/turnflows/clear all turnflows',
                            self.on_clear_turnflows,
                            info='Clear all turnflows.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_DELETE, wx.ART_MENU),
                            )


#-------------------------------------------------------------------------

        menubar.append_menu('demand/trips and routes',
                            bitmap=self.get_icon("trip3_24px.png"),
                            )

        menubar.append_item('demand/trips and routes/route',
                            self.on_route,
                            info='Route current trips and create SUMO route xml.',
                            bitmap=self.get_icon("route3_24px.png"),
                            )

        menubar.append_item('demand/trips and routes/export trips to SUMO xml...',
                            self.on_export_sumotrips,
                            info='Export all trips to SUMO XML format.',
                            bitmap=self.get_icon("Document_Export_24px.png"),
                            )

        menubar.append_item('demand/trips and routes/export routes to SUMO xml...',
                            self.on_export_sumoroutes,
                            info='Export all trips to SUMO XML format.',
                            bitmap=self.get_icon("Document_Export_24px.png"),
                            )

        menubar.append_item('demand/trips and routes/import trips from SUMO xml...',
                            self.on_import_trips_xml,
                            info=self.on_import_trips_xml.__doc__.strip(),
                            bitmap=self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('demand/trips and routes/import trips with routes from SUMO xml...',
                            self.on_import_triproutes_xml,
                            info=self.on_import_triproutes_xml.__doc__.strip(),
                            bitmap=self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('demand/trips and routes/import routes to existing trips from SUMO xml...',
                            self.on_import_routes_alternative_xml,
                            info=self.on_import_routes_alternative_xml.__doc__.strip(),
                            bitmap=self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('demand/trips and routes/clear all trips and routes',
                            self.on_clear_trips,
                            info='Clear all trips with respective routes.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_DELETE, wx.ART_MENU),
                            )

        menubar.append_item('demand/trips and routes/clear all routes',
                            self.on_clear_routes,
                            info='Clear all routes, trips will remain.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_DELETE, wx.ART_MENU),
                            )


#-------------------------------------------------------------------------
        if 0:
            menubar.append_menu('demand/virtual population',
                                bitmap=self.get_icon(
                                    "icon_virtualpopulation.jpg")
                                )
            menubar.append_item('demand/virtual population/create from O-D matrix',
                                self.on_create_pop_from_ods,
                                info='Generates population from O-D matrix data.',
                                #bitmap = self.get_icon("Document_Import_24px.png"),
                                )
            menubar.append_item('demand/virtual population/create plans for privates',
                                self.on_make_plans_private,
                                info='create plans for private transport.',
                                #bitmap = self.get_icon("Document_Import_24px.png"),
                                )

            menubar.append_item('demand/virtual population/Export plans to SUMO xml...',
                                self.on_write_plans_to_sumoxml,
                                info='Export all plans to SUMO routes XML format.',
                                #bitmap = self.get_icon("Document_Import_24px.png"),
                                )


# (self._menuitem_draw_route, id_item,) = menubar.append_item(
##            'plugins/traces/draw selected route in network',
# self.on_renew_objectbrowser,
##            info='Enable visualization of routes in network tab. Double-click on trace in table.',
# check=True)
# self._menuitem_draw_route.Check(False)
##
# (self._menuitem_plot_route, id_item,) = menubar.append_item(
##            'plugins/traces/add plot selected route',
# self.on_renew_objectbrowser,
##            info='Enable adding of routes to graphics in trace tab. Double-click on trace in table.',
# check=True)
# self._menuitem_plot_route.Check(False)
##
    def on_clear_vtypes(self, event=None):
        self._demand.vtypes.clear_vtypes()
        self._mainframe.browse_obj(self._demand.vtypes)
        if event:
            event.Skip()

    def on_load_vtypes_defaults(self, event=None):
        self._demand.vtypes.clear_vtypes()
        self._demand.vtypes.add_vtypes_default()
        self._mainframe.browse_obj(self._demand.vtypes)
        if event:
            event.Skip()

    def on_clear_trips(self, event=None):
        self._demand.trips.clear_trips()
        self._mainframe.browse_obj(self._demand.trips)
        if event:
            event.Skip()

    def on_clear_routes(self, event=None):
        self._demand.trips.clear_routes()
        self._mainframe.browse_obj(self._demand.trips)
        if event:
            event.Skip()

    def on_create_pop_from_ods(self, event=None):
        """
        Generates virtual population from ODM matrix.
        """
        self._demand.virtualpop.create_pop_from_ods()
        self._mainframe.browse_obj(self._demand.virtualpop)
        event.Skip()

    def on_make_plans_private(self, event=None):
        """
        Generates virtual population from ODM matrix.
        """
        self._demand.virtualpop.make_plans_private()
        self._mainframe.browse_obj(self._demand.virtualpop)
        event.Skip()

    def on_write_plans_to_sumoxml(self, event=None):
        # print 'on_write_plans_to_sumoxml'
        dirpath = self.get_scenario().get_workdirpath()
        defaultFile = self.get_scenario().get_rootfilename() + '.rou.xml'
        wildcards_all = 'All files (*.*)|*.*|XML files (*.xml)|*.xml'
        dlg = wx.FileDialog(None, message='Write plans to SUMO rou.xml',
                            defaultDir=dirpath,
                            defaultFile=defaultFile,
                            wildcard=wildcards_all, style=wx.SAVE | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return

        self._demand.virtualpop.export_trips_xml(filepath)

        event.Skip()

    def on_add_odtrips(self, event=None):
        """
        Opend odm wizzard
        """
        dlg = odgui.AddOdDialog(self._mainframe, self._demand.odintervals)
        dlg.Show()
        dlg.MakeModal(True)
        self._mainframe.browse_obj(self._demand.odintervals)
        # self.scenariopanel.refresh_panel(self.scenario)

    def on_generate_odtrips(self, event=None):
        """
        Generates trips from origin to destination zone from current OD matrices.
        """
        self._demand.odintervals.generate_trips()
        self._mainframe.browse_obj(self._demand.trips)
        event.Skip()

    def on_clear_odtrips(self, event=None):
        """
        Generates trips from origin to destination zone from current OD matrices.
        """
        self._demand.odintervals.clear_od_trips()
        self._mainframe.browse_obj(self._demand.odintervals)
        event.Skip()

    def on_import_turnflows(self, event=None):
        tfimporter = turnflows.TurnflowImporter(self._demand.turnflows,
                                                logger=self._mainframe.get_logger()
                                                )
        dlg = ProcessDialog(self._mainframe, tfimporter)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        # print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
        # print '  status =',dlg.get_status()
        if dlg.get_status() != 'success':  # val == wx.ID_CANCEL:
            # print ">>>>>>>>>Unsuccessful\n"
            dlg.Destroy()

        if dlg.get_status() == 'success':
            # print ">>>>>>>>>successful\n"
            # apply current widget values to scenario instance
            dlg.apply()
            dlg.Destroy()
            self._mainframe.browse_obj(self._demand.turnflows)
            #del self._scenario
            #self._scenario = scenariocreator.get_scenario()
            # self._scenario.import_xml()
            # this should update all widgets for the new scenario!!
            # print 'call self._mainframe.refresh_moduleguis()'
            # self._mainframe.refresh_moduleguis()

        if event:
            event.Skip()

    def on_normalize_turnprobabilities(self, event=None):
        """
        Makes sure that sum of turn probabilities from an edge equals 1.
        """
        self._demand.turnflows.normalize_turnprobabilities()
        self._mainframe.browse_obj(self._demand.turnflows)
        if event:
            event.Skip()

    def on_clear_turnflows(self, event=None):
        """Generates routes, based on flow information and turnflow probabilities.
        This function will apply the JTROUTER for each transport mode separately.
        """
        self._demand.turnflows.clear_turnflows()
        self._mainframe.browse_obj(self._demand.turnflows)
        if event:
            event.Skip()

    def on_turnflows_to_routes(self, event=None):
        """Generates routes, based on flow information and turnflow probabilities.
        This function will apply the JTROUTER for each transport mode separately.
        """
        self._demand.turnflows.turnflows_to_routes(is_clear_trips=False)
        self._mainframe.browse_obj(self._demand.trips)
        if event:
            event.Skip()

    def on_route(self, event=None):
        """Generates routes from current trip info.
        """
        # self._demand.trips.clear_routes()
        self._demand.trips.route(is_export_net=True, is_export_trips=True)
        self._mainframe.browse_obj(self._demand.trips)
        #filepath = self._demand.trips.get_routefilepath()
        #defaultFile = os.path.basename(filepath)
        #dirpath = os.path.dirname(filepath)
        #wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
        # dlg = wx.FileDialog(None, message='Write routes to SUMO xml',
        #                    defaultDir=dirpath,
        #                    defaultFile=defaultFile,
        #                    wildcard=wildcards_all, style=wx.SAVE | wx.CHANGE_DIR)
        # if dlg.ShowModal() == wx.ID_OK:
        #    filepath = dlg.GetPath()
        #
        # else:
        #    return
        #
        #filepath = self._demand.trips.route(routefilepath = None,is_export_net = True, is_export_trips = True)

        # if filepath is not None:
        #     print 'Successfully routed to %s.'%filepath
        #    self._demand.import_routes_xml( filepath,  is_clear_trips = False, is_add = True)

    def on_import_trips_xml(self, event=None):
        """Select xml file and import new trips. 
        New trips with associated routes will generated. 
        """
        filepath = self._demand.trips.get_routefilepath()
        defaultFile = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML trip files (*.trip.xml)|*.trip.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import trips and routes from SUMO xml',
                            defaultDir=dirpath,
                            defaultFile=defaultFile,
                            wildcard=wildcards_all, style=wx.SAVE | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return

        self._demand.trips.import_trips_xml(filepath, is_clear_trips=False,
                                            is_generate_ids=True)
        self._mainframe.browse_obj(self._demand.trips)

    def on_import_triproutes_xml(self, event=None):
        """Select xml file and import trips and routes. 
        New trips with associated routes will generated. 
        """
        filepath = self._demand.trips.get_routefilepath()
        defaultFile = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*.rou.xml)|*.rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import trips and routes from SUMO xml',
                            defaultDir=dirpath,
                            defaultFile=defaultFile,
                            wildcard=wildcards_all, style=wx.SAVE | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return

        self._demand.trips.import_routes_xml(filepath, is_clear_trips=False,
                                             is_generate_ids=True,
                                             is_add=False)
        self._mainframe.browse_obj(self._demand.trips)

    def on_import_routes_alternative_xml(self, event=None):
        """Select xml file and import routes. 
        Routes will added as route alternative to existing trips. 
        """
        filepath = self._demand.trips.get_routefilepath()
        defaultFile = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*.rou.xml)|*.rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Import trips and routes from SUMO xml',
                            defaultDir=dirpath,
                            defaultFile=defaultFile,
                            wildcard=wildcards_all, style=wx.OPEN | wx.CHANGE_DIR
                            )

        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return

        self._demand.trips.import_routes_xml(filepath, is_clear_trips=False,
                                             is_generate_ids=False,
                                             is_add=True)
        self._mainframe.browse_obj(self._demand.trips)

    def on_export_sumotrips(self, event=None):
        # print 'on_export_sumotrips'
        filepath = self._demand.trips.get_tripfilepath()
        defaultFile = os.path.dirname(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = '*.*|XML trip files (*.trip.xml)|*.trip.xml|All files (*.*)'
        dlg = wx.FileDialog(None, message='Write trips to SUMO xml',
                            defaultDir=dirpath,
                            defaultFile=defaultFile,
                            wildcard=wildcards_all, style=wx.SAVE | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return

        self._demand.trips.export_trips_xml(filepath)

    def on_export_sumoroutes(self, event=None):
        # print 'on_export_sumotrips'
        filepath = self._demand.trips.get_routefilepath()
        defaultFile = os.path.basename(filepath)
        dirpath = os.path.dirname(filepath)
        wildcards_all = 'XML route files (*rou.xml)|*rou.xml|All files (*.*)|*.*'
        dlg = wx.FileDialog(None, message='Write routes to SUMO xml',
                            defaultDir=dirpath,
                            defaultFile=defaultFile,
                            wildcard=wildcards_all, style=wx.SAVE | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return

        self._demand.trips.export_routes_xml(filepath)
