# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    wxgui.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

import os
import shutil
import wx
import numpy as np

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.ogleditor import *
from agilepy.lib_base.processes import Process
from agilepy.lib_wx.processdialog import ProcessDialog, ProcessDialogInteractive

from coremodules.network import routing
from coremodules.demand import demand
from coremodules.simulation import sumo, results
from . import matsim, matsim_demand, matsim_network
from agilepy.lib_wx.ogleditor import Polylines
try:
    import matsim_mpl
    is_mpl = True  # we have matplotlib support
except:
    print("WARNING: python matplotlib package not installed, no matplotlib plots.")
    is_mpl = False


class MatsimEdgeDrawings(Polylines):
    def __init__(self, ident, edges, parent,   **kwargs):

        # joinstyle
        # FLATHEAD = 0
        # BEVELHEAD = 1
        Polylines.__init__(self, ident,  parent, name='MATSIM edge drawings',
                           is_lefthalf=False,
                           is_righthalf=True,  # reverse for english roads
                           arrowstretch=1.5,
                           joinstyle=FLATHEAD,  # BEVELHEAD is good for both halfs,
                           **kwargs)

        self.delete('vertices')
        self.delete('widths')

        self.add(cm.AttrConf('color_edge_default', np.array([0.4, 0.1, 0.8, 1.0], np.float32),
                             groupnames=['options', 'edgecolors'],
                             perm='wr',
                             metatype='color',
                             name='Default color',
                             info='Default edge color.',
                             ))

        self.add(cm.AttrConf('width_lane_default', 3.5,
                             groupnames=['options',],
                             perm='wr',
                             name='Default lane width',
                             unit='m',
                             info='Default lane width.',
                             ))

        self.edgeclasses = {
            # 'bikeway':('highway.cycleway',[0.6,0.2,0.2,0.9]),
            #
            # 'footpath':('highway.footpath',[0.1,0.6,0.5,0.9]),
            'train': ('train', [1.0, 0.0, 0.0, 1.0]),
            'tram': ('tram', [1.0, 0.0, 0.0, 1.0]),
            #
            'car': ('car', [1.0, 1.0, 1.0, 1.0]),
            'bus': ('bus', [0.0, 0.0, 1.0, 1.0]),
        }

        for edgeclass, cdata in self.edgeclasses.items():
            edgetype, color = cdata
            self.add(cm.AttrConf('color_'+edgeclass, np.array(color, np.float32),
                                 groupnames=['options', 'edgecolors'],
                                 edgetype=edgetype,
                                 metatype='color',
                                 perm='wr',
                                 name=edgeclass+' color',
                                 info='Color of '+edgeclass+' edge class.',
                                 ))

        self.set_netelement(edges)

    def get_netelement(self):
        return self._edges

    def get_vertices_array(self):
        return self._edges.shapes[self.get_ids()]  # .value[self._inds_map]#[self.get_ids()]

    def get_widths_array(self):
        # double because only the right half is shown
        # add a little bit to the width to make it a little wider than the lanes contained
        # return 2.2*self._edges.widths.value[self._inds_map]
        return self.width_lane_default.get_value() * self._edges.nums_lane[self.get_ids()]

    def get_vertices(self, ids):
        return self._edges.shapes[ids]

    def set_vertices(self, ids, vertices, is_update=True):
        self._edges.set_shapes(ids, vertices)
        if is_update:
            self._update_vertexvbo()

    def get_widths(self, ids):
        return self.width_lane_default.get_value() * self._edges.nums_lane[ids]

    def set_widths(self, ids, values):
        pass

    def is_tool_allowed(self, tool, id_drawobj=-1):
        """
        Returns True if this tool can be applied to this drawobj.
        Optionally a particular drawobj can be specified with id_drawobj.
        """
        # basic tools:
        # return tool.ident not in   ['configure','select_handles','delete','move','stretch']
        return tool.ident not in ['delete',]

    def set_netelement(self, edges):

        self._edges = edges
        # self._inds_edges = self._edges.get_inds()
        self.clear_rows()
        ids = self._edges.get_ids()
        n = len(ids)

        self.add_rows(ids=ids,
                      beginstyles=np.ones(n)*FLATHEAD,
                      endstyles=np.ones(n)*TRIANGLEHEAD,
                      )
        self.update()

    def update(self, is_update=True):
        """
        Update color, assume that there have not been structural changes of the arrays
        """
        # print ('Matsimedge.update')
        # assumes that edges have been set in set_edges
        n = len(self)

        self.colors_fill.value[:] = np.ones((n, 1), np.float32)*self.color_edge_default.value

        for edgeclass, cdata in self.edgeclasses.items():
            edgetype, color = cdata
            # print ('  ',edgeclass, np.sum(self._edges.types.value==edgetype))
            # print ('  color',getattr(self,'color_'+edgeclass).value)
            self.colors_fill[self._edges.select_ids(self._edges.types.value == edgetype)] = getattr(
                self, 'color_'+edgeclass).value
            # self.colors_fill.value[self._inds_map[self._edges.types.value==edgetype]] = getattr(self,'color_'+edgeclass).value

        self.colors_fill_highl.value[:] = self._get_colors_highl(self.colors_fill.value)

        if is_update:
            self._update_vertexvbo()
            self._update_colorvbo()


class WxGui(ModuleGui):
    """Contains functions that communicate between MATSIM plugin and the widgets of the main wx gui
    and the functions of the plugin.
    """

    def __init__(self, ident):
        self._matsim = None
        self._demand = None
        self._simulation = None
        self._init_common(ident,  priority=100010,
                          icondirpath=os.path.join(os.path.dirname(__file__), 'images'))

    def get_module(self):
        return self._matsim

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
        self._mainframe = mainframe
        self.make_menu()

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        scenario = self.get_scenario()
        # print 'matsim.refresh_widgets',self._simulation != scenario.simulation

        is_refresh = False
        if self._simulation != scenario.simulation:
            ident = 'matsim'
            del self._demand
            del self._matsim
            self._demand = scenario.demand
            self._matsim = self._demand.add_demandobject(ident=ident, DemandClass=matsim.Matsim)
            neteditor = self.get_neteditor()
            drawing = self.get_drawing()
            drawing.set_element('matsimedgedrawings', MatsimEdgeDrawings,
                                self._matsim.net.edges, layer=500)

            neteditor.draw()
            is_refresh = True

    def make_menu(self):
        menubar = self._mainframe.menubar
        menubar.append_menu('plugins/matsim', bitmap=self.get_icon('icon_matsim.png'),)
        # menubar.append_menu('plugins/matsim/analyse', )
        menubar.append_menu('plugins/matsim/network', )
        menubar.append_menu('plugins/matsim/demand', )
        # menubar.append_menu('plugins/matsim/config', )
        # menubar.append_menu('plugins/matsim/public-transport', )
        # menubar.append_menu('plugins/matsim/mapmatching', )

        # menubar.append_item( 'plugins/matsim/public-transport/Import',
        #    self.on_import_pt, # common function in modulegui
        #    info='Import Public Transport Data',
        #    )
        menubar.append_item('plugins/matsim/browse',
                            self.on_browse_obj,  # common function in modulegui
                            info='View and browse matsim in object panel.',
                            bitmap=self.get_agileicon('icon_browse_24px.png'),  # ,
                            )
        # menubar.append_item( 'plugins/matsim/Run MATSim Simulation',
        #    self.on_run_sim, # common function in modulegui
        #    info='Configure & Run a MATSim Simulation',
        #    )
        # menubar.append_item( 'plugins/matsim/analysis/Display analysis',
        #    self.on_run_sim, # common function in modulegui
        #    info='Analysis of MATSim',
        #    )
        # menubar.append_item( 'plugins/matsim/analysis/Run analysis',
        #    self.on_run_sim, # common function in modulegui
        #    info='Analysis of MATSim',
        #    )
        # menubar.append_item( 'plugins/matsim/config/Import Configuration',
        #    self.on_import_config, # common function in modulegui
        #    info='Import config',
        #    )
        menubar.append_item('plugins/matsim/network/import network ...',
                            self.on_import_network,  # common function in modulegui
                            bitmap=self.get_agileicon("Document_Import_24px.png"),
                            )

        menubar.append_item('plugins/matsim/network/import BEAM network ...',
                            self.on_import_beamnetwork,  # common function in modulegui
                            bitmap=self.get_agileicon("Document_Import_24px.png"),
                            )

        if is_mpl:

            menubar.append_item('plugins/matsim/network/plot network with matplotlib',
                                self.on_plot_matsimnet,
                                bitmap=self.get_icon('icon_mpl.png'),  # ,
                                )

        menubar.append_item('plugins/matsim/demand/import trips from plans file...',
                            self.on_import_trips_from_plans,  # common function in modulegui
                            bitmap=self.get_agileicon("Document_Import_24px.png"),
                            )
        # menubar.append_item( 'plugins/matsim/demand/import trips from extracted plans file...',
        #    self.on_import_trips_with_routes, # common function in modulegui
        #    bitmap = self.get_agileicon("Document_Import_24px.png"),
        #    )
        menubar.append_item('plugins/matsim/demand/import population from plans file...',
                            self.on_import_population,  # common function in modulegui
                            bitmap=self.get_agileicon("Document_Import_24px.png"),
                            )
        # menubar.append_item( 'plugins/matsim/mapmatching/match simulation maps',
        #    self.on_matchmaps, # common function in modulegui
        #    bitmap = self.get_agileicon("Document_Import_24px.png"),
        #    )

        # if is_mpl:
        #    menubar.append_item( 'plugins/prt/results/stopresults with matplotlib',
        #        self.on_mpl_stopresults,
        #        bitmap = self.get_icon('icon_mpl.png'),#,
        #        )

    def on_import_network(self, event=None):
        """import MATSIM network from xml file"""

        proc = matsim_network.NetImporter(self._matsim,
                                          logger=self._mainframe.get_logger()
                                          )

        dlg = ProcessDialog(self._mainframe, proc)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()

            self._mainframe.browse_obj(self._matsim.net)
            self.refresh_widgets()

    def on_import_beamnetwork(self, event=None):
        """import BEAM network from CSV file"""

        proc = matsim_network.BeamNetImporter(self._matsim,
                                              logger=self._mainframe.get_logger()
                                              )

        dlg = ProcessDialog(self._mainframe, proc)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()

            self._mainframe.browse_obj(self._matsim.net)

    def on_plot_matsimnet(self, event=None):
        """Plot MATSim and SUMO network with matplotlib"""
        proc = matsim_mpl.MatsimNetplotter(self._matsim,
                                           logger=self._mainframe.get_logger()
                                           )

        dlg = ProcessDialog(self._mainframe, proc)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()

    def on_import_trips_from_plans(self, event=None):
        """import trips from MATSIM xml plan file."""

        proc = matsim_demand.TripImporter(self._matsim,
                                          logger=self._mainframe.get_logger()
                                          )

        dlg = ProcessDialog(self._mainframe, proc)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()

            self._mainframe.browse_obj(self._demand.trips)

    def on_import_population(self, event=None):
        """import population with plans from MATSIM xml plan file."""

        proc = matsim_demand.PopulationImporter(self._matsim,
                                                logger=self._mainframe.get_logger())

        dlg = ProcessDialog(self._mainframe, proc)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()

            self._mainframe.browse_obj(self._demand.virtualpop)

    def on_run_sim(self, event=None):

        if not os.path.exists(self.get_scenario().workdirpath + '\\docker-matsim-input\\'):
            os.makedirs(self.get_scenario().workdirpath + '\\docker-matsim-input\\')

        networkexporter = matsim_network.MATSimNetExport(self._matsim, self.get_scenario(
        ).workdirpath + '\\docker-matsim-input\\', logger=self._mainframe.get_logger())

        networkexporter.do()

        if os.path.exists(self._matsim.config.population_config):
            shutil.copy(self._matsim.config.population_config,
                        self.get_scenario().workdirpath + '\\docker-matsim-input\\')

        self._matsim.config.write_config(self.get_scenario().workdirpath + '\\docker-matsim-input\\config.xml')

    def on_matchmaps(self, event=None):

        result = matsim_mapmatch.match_maps(self._matsim.mapmatch, self.get_scenario(
        ).workdirpath, self.get_scenario().net, self._matsim.net, tolerance=10)

    def on_import_config(self, event=None):

        proc = matsim_config.MATSimConfigImport(self._matsim, logger=self._mainframe.get_logger())

        dlg = ProcessDialog(self._mainframe, proc)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() == 'success':
            dlg.apply()
            dlg.Destroy()

            self._mainframe.browse_obj(self._matsim.config)
            self.refresh_widgets()
