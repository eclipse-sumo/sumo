import os
import wx

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.processdialog import ProcessDialog

import network
import routing
from network_editor import *


class WxGui(ModuleGui):

    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """

    def __init__(self, ident):
        self._net = None
        self._init_common(ident,  priority=10,
                          icondirpath=os.path.join(os.path.dirname(__file__), 'images'))

    def get_module(self):
        return self._net

    def get_scenario(self):
        return self._mainframe.get_modulegui('coremodules.scenario').get_module()

    def get_neteditor(self):
        return self._neteditor

    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        self._neteditor = mainframe.add_view("Network", Neteditor)

        # mainframe.browse_obj(self._net)
        self.make_menu()
        self.make_toolbar()

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        is_refresh = False
        scenario = self.get_scenario()
        # print '\n\nnetwork refresh_widgets',self._net !=
        # scenario.net,scenario.net.is_modified(),id(self._net)
        if self._net != scenario.net:
            # print '  scenario has a new network'
            # print '  self._net != scenario.net',self._net,scenario.net
            del self._net
            self._net = scenario.net
            is_refresh = True

        elif self._net.is_modified():
            # print '   network is_modified',self._net.is_modified()
            is_refresh = True

        if is_refresh:
            drawing = self._neteditor.set_net(self._net)
            #canvas = self._neteditor.get_canvas()

    def make_menu(self):
        # print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu('network', bitmap=self.get_icon('icon_graph.png'))

        menubar.append_menu('network/import',
                            bitmap=self.get_icon("Document_Import_24px.png"),
                            )

        menubar.append_item('network/import/from sumo net.xml ...',
                            self.on_import_sumonet,
                            info='Import network from sumo net.xml file.',
                            bitmap=self.get_icon('icon_sumo_24px.png'),  # ,
                            )

        menubar.append_item('network/import/from osm.xml ...',
                            self.on_import_osm,
                            info='Import network from osm files.',
                            bitmap=self.get_icon('Files-Osm-icon_24.png'),
                            )

        menubar.append_menu('network/export',
                            bitmap=self.get_icon("Document_Export_24px.png"),
                            )

        menubar.append_item('network/export/sumo net.xml',
                            self.on_export_sumonet,
                            info='Export network to sumo net.xml file.',
                            bitmap=self.get_icon('icon_sumo_24px.png'),  # ,
                            )

        menubar.append_item('network/edit with SUMO netedit...',
                            self.on_netedit,
                            info="Edit network with SUMO's netedit.",
                            bitmap=self.get_icon('netedit.png'),
                            )

        menubar.append_item('network/edit with SUMO netedit on map...',
                            self.on_netedit_on_map,
                            info="Edit network with SUMO's netedit. In addition to the network, backround maps are shown. You need to download the maps before with landuse/import maps.",
                            bitmap=self.get_icon('netedit.png'),
                            )

        menubar.append_item('network/show with SUMO GUI...',
                            self.on_sumogui,
                            info="Show network with sumo-gui.",
                            bitmap=self.get_icon('icon_sumo_24px.png'),  # ,
                            )

        # menubar.append_item( 'network/refresh',
        #    self.on_refresh,
        #    info='Refresh graph.',
        #    #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
        #    )

        menubar.append_item('network/clean nodes',
                            self.on_clean_codes,
                            info='Cuts back edges and lanes to the border of the node, so that connections become clearer.',
                            #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
                            )

        # menubar.append_item( 'network/correct spread',
        #    self.on_correct_spread,
        #    info='Corrects spread type for older versions.',
        #    #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
        #    )

        # menubar.append_item( 'network/correct endpoint',
        #    self.on_correct_endpoint,
        #    info='Corrects send points for older versions.',
        #    #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
        #    )
        menubar.append_item('network/clear network',
                            self.on_clear_net,
                            info='This will clear the entire network.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_DELETE, wx.ART_MENU),
                            )
        # menubar.append_item( 'network/test routing',
        #    self.on_test_routing,
        #    info='test routing.',
        #    #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
        #    )
        # menubar.append_item( 'network/test',
        #    self.on_test,
        #    info='Test graph.',
        #    #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
        #    )

    def on_clear_net(self, event=None):
        # print 'on_clear_net',id(self._net)
        self._net.clear()
        self._mainframe.refresh_moduleguis()

        if event:
            event.Skip()

    def on_netedit(self, event=None):
        if self._net.call_netedit():
            self._mainframe.refresh_moduleguis()

        if event:
            event.Skip()

    def on_netedit_on_map(self, event=None):
        if self._net.call_netedit(is_maps=True):
            self._mainframe.refresh_moduleguis()

        if event:
            event.Skip()

    def on_sumogui(self, event=None):
        self._net.call_sumogui(is_maps=True)

        if event:
            event.Skip()

    def on_test_routing(self, event=None):
        D, P = routing.dijkstra(54, self._net.nodes,
                                self._net.edges, set([42, 82]))
        cost, route = routing.get_mincostroute_node2node(
            54, 42, D, P, self._net.edges)
        print ' route:', route
        print ' cost', cost
        print '  firstnode, lastnode', self._net.edges.ids_fromnode[route[0]], self._net.edges.ids_tonode[route[-1]]

        D, P = routing.edgedijkstra(
            29, self._net.nodes, self._net.edges, set([106, 82]))
        cost, route = routing.get_mincostroute_edge2edge(29, 82, D, P)
        print ' route:', route
        print ' cost', cost
        # print  '  firstnode,
        # lastnode',self._net.edges.ids_fromnode[route[0]],self._net.edges.ids_tonode[route[-1]]

        if event:
            event.Skip()

    def on_clean_codes(self, event=None):
        self._net.nodes.clean(is_reshape_edgelanes=True)
        self._mainframe.browse_obj(self._net.nodes)
        # this will also replace the drawing!!
        self._mainframe.refresh_moduleguis()
        if event:
            event.Skip()

    def on_refresh(self, event=None):
        # print 'on_refresh neteditor',id(self._neteditor.get_drawing())

        canvas = self._neteditor.get_canvas()
        wx.CallAfter(canvas.zoom_tofit)
        self._mainframe.browse_obj(self._net)
        if event:
            event.Skip()

    def on_correct_spread(self, event=None):
        self._net.edges.correct_spread()
        self._mainframe.refresh_moduleguis()
        if event:
            event.Skip()

    def on_correct_endpoint(self, event=None):
        self._net.edges.correct_endpoint()
        self._mainframe.refresh_moduleguis()
        if event:
            event.Skip()

    def on_import_sumonet(self, event=None):
        # TODO: here we should actually replace the current network
        # so we would need a clear net method in scenario
        # alternatively we could merge properly
        sumonetimporter = network.SumonetImporter(
            self._net, logger=self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, sumonetimporter)

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

            #del self._scenario
            #self._scenario = scenariocreator.get_scenario()

            self._mainframe.browse_obj(self._net)

            # this should update all widgets for the new scenario!!
            # print 'call self._mainframe.refresh_moduleguis()'
            self._mainframe.refresh_moduleguis()

    def on_import_osm(self, event=None):
        # TODO: here we should actually replace the current network
        # so we would need a clear net method in scenario
        # alternatively we could merge properly
        importer = network.OsmImporter(
            self._net, logger=self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, importer)

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

            self._mainframe.browse_obj(self._net.nodes)

            # this should update all widgets for the new scenario!!
            # print 'call self._mainframe.refresh_moduleguis()'
            self._mainframe.refresh_moduleguis()

    def on_export_sumonet(self, event=None):
        print 'on_export_sumonet'
        if self._net.parent != None:
            rootname = self._net.parent.get_rootfilename()
            rootdirpath = self._net.parent.get_workdirpath()
        else:
            rootname = self._net.get_ident()
            rootdirpath = os.getcwd()

        netfilepath = os.path.join(rootdirpath, rootname + '.net.xml')
        wildcards_all = 'All files (*.*)|*.*|XML files (*net.xml)|*net.xml'

        dlg = wx.FileDialog(None, message='Write trips to SUMO xml',
                            defaultDir=rootdirpath,
                            defaultFile=netfilepath,
                            wildcard=wildcards_all, style=wx.SAVE | wx.CHANGE_DIR)
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()

        else:
            return

        self._net.export_netxml(filepath)
