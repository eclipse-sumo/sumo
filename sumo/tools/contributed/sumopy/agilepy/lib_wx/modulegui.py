import os
import wx

import agilepy.lib_base.classman as cm


class ModuleGui:

    """Manages all GUIs to interact between the widgets from mainframe
    and the specific functions of the module.
    """

    def __init__(self, ident):

        self._init_common(ident,
                          module=cm.BaseObjman('empty'),
                          priority=9999999,
                          icondirpath=os.path.join(
                              os.path.dirname(__file__), 'images')
                          )

    def get_initpriority(self):
        return self._initpriority

    def _init_common(self, ident, module=None, priority=9999999, icondirpath=''):
        """
        ident: any string to identify this gui
        module: the module which interacts with this gui
        priority: specifies priority with wich this gui initialized in mainframe
         Initialization of widgets in init_widgets 
        """

        self._ident = ident
        self._set_module(module)
        self._initpriority = priority
        self._icondirpath = icondirpath

    def get_icon(self, name):
        return wx.Bitmap(os.path.join(self._icondirpath, name))

    def _set_module(self, module):
        self._module = module

    def get_module(self):
        return self._module

    def get_ident(self):
        return self._ident

    def get_logger(self):
        return self._mainframe.get_logger()

    # def set_logger(self, logger):
    #    self._logger = logger

    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        self.make_menu()
        self.make_toolbar()

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        pass

    def make_toolbar(self):
        pass

    def make_menu(self):
        pass
# print 'make_menu'
##        menubar = self._mainframe.menubar
##        menubar.append_menu('plugins/traces', bitmap=wx.Bitmap(os.path.join(ICONDIR,'icon_gps.png')))
##
# menubar.append_item('plugins/traces/browse traces',
# self.on_renew_objectbrowser,
##            info='Browse traces in object browser.',
# bitmap=wx.Bitmap(os.path.join(ICONDIR_APP,'icon_control_panel16x16.png')),)
##
# menubar.append_item('plugins/traces/open matched routes...',
##                self.on_import_routes, info='Open matched routes and GPS points from xml.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
# menubar.append_item('plugins/traces/save matched routes...',
##                self.on_export_routes_to_xml, info='Save matched routes and GPS points in xml.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
##
# imports
##
# menubar.append_item('plugins/traces/import gps traces from gpx...',
##                self.on_import_gpx, info='Import tracews from gpx file, the format of Endomondo and othe GPS tracker applications.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
##
# menubar.append_item('plugins/traces/import gps traces from csv...',
##                self.on_import_traces, info='Import gps traces from a CSV file.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
##
# menubar.append_item('plugins/traces/import routes...',
# self.on_import_routes, info='Import routes from sumo xml file. Choose one or more files to import.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
# exports
##
# menubar.append_item('plugins/traces/export routes to shapefile...',
##                self.on_export_routes_to_shape, info='Export routes to shape file.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
##
# menubar.append_item('plugins/traces/export shortest routes to shapefile...',
##                self.on_export_routes_mindist_to_shape, info='Export shortest routes to shape file.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
# menubar.append_item('plugins/traces/export points to shapefile...',
##                self.on_export_points_to_shape, info='Export GPS points to shape file.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
# menubar.append_item('plugins/traces/export to demand...',
##                self.on_make_demand, info='Generate demand with matched routes for microsimulation with SUMO.',
# bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU))
##
##
# basic operations
##        menubar.append_item('plugins/traces/clean traces', self.on_clean_traces, info='Remove invalid traces and split traces, if necessary.')
# menubar.append_item('plugins/traces/calculate edge boundaries', self.on_calc_edge_boundaries, info='Calculate boundaries of edges for all traces.')
# menubar.append_item('plugins/traces/calculate edge weights', self.on_calc_edge_weights, info='Calculate weight of edges for all traces.')
##        menubar.append_item('plugins/traces/match traces', self.on_match, info='match traces to network.')
##
##        menubar.append_item('plugins/traces/filter OD traces', self.on_filter_traces_between_zones, info='Filter traces with origin and destination in predefined zones. Remove all other traces.')
##
##
##
# all data graphics
##
##
# (self._menuitem_draw_pointsets, id_item,) = menubar.append_item(
##            'plugins/traces/show all datapoints in network',
# self.on_draw_pointsets,
# info='Choose to show all GPS data points on network map', check=True)
# self._menuitem_draw_pointsets.Check(False)
##
# menubar.append_item(
##            'plugins/traces/add plot all datapoints',
# self.on_plot_pointsets,
# info='Choose to add all GPS data points in traces graphics')
##
##
##        menubar.append_item('plugins/traces/add plot all edge boundaries', self.on_plot_boundaries, info='Add plot of all edge boundaries on Trace tab.')
##
##        menubar.append_item('plugins/traces/add plot all routes in traces graphics', self.on_plot_routes, info='Add plot of all routes in Trace tab.')
##
##
# single trace graphics
# (self._menuitem_draw_pointset, id_item,) = menubar.append_item(
##            'plugins/traces/show selected datapoints in Network',
# self.on_renew_objectbrowser,
##            info='Enable visualization of datasets in network.  Double-click on trace in table',
# check=True)
# self._menuitem_draw_pointset.Check(False)
##
# (self._menuitem_plot_pointset, id_item,) = menubar.append_item(
##            'plugins/traces/add plot selected datapoints',
# self.on_renew_objectbrowser,
##            info='Enable adding of datasets to graphics in trace tab. Double-click on trace in table.',
# check=True)
# self._menuitem_plot_pointset.Check(False)
##
# (self._menuitem_plot_weight, id_item,) = menubar.append_item(
##            'plugins/traces/plot selected edge weights',
# self.on_renew_objectbrowser,
##            info='Enable adding of edge weights to graphics in trace tab. Double-click on trace in table.',
# check=True)
# self._menuitem_plot_weight.Check(False)
##
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
##
##
##
##
##
##
##
##
##        menubar.append_item('plugins/traces/clear all plots and graphics', self.on_clear_plots, info='Clear plots in trace and network tab.')
##
##        menubar.append_item('plugins/traces/delete all', self.on_delete_all, info='Delete all traces and intermediate results.')
##

    def on_browse_obj(self,  event=None):
        self._mainframe.browse_obj(self._module)
        #self._mainframe.change_obj(self._module, func_choose_id = self.on_select_trace)
        #self.write_to_statusbar('%d traces in GPS trace table.'%len(self._traces),'message')
