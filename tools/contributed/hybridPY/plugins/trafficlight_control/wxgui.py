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
import wx
import numpy as np

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.ogleditor import *
from agilepy.lib_base.processes import Process
from agilepy.lib_wx.processdialog import ProcessDialog, ProcessDialogInteractive


from coremodules.demand import demand
from coremodules.simulation import sumo, results
from . import tlcontrol

try:
    from . import results_mpl as results_mpl
    is_mpl = True  # we have matplotlib support
except:
    print("WARNING: python matplotlib package not installed, no matplotlib plots.")
    is_mpl = False


class WxGui(ModuleGui):
    """Contains functions that communicate between PRT plugin and the widgets of the main wx gui
    and the functions of the plugin.
    """

    def __init__(self, ident):
        self._tlclusters = None
        self._demand = None
        self._simulation = None
        self._init_common(ident,  priority=100015,
                          icondirpath=os.path.join(os.path.dirname(__file__), 'images'))

    def get_module(self):
        return self._tlclusters

    def get_scenario(self):
        return self._mainframe.get_modulegui('coremodules.scenario').get_scenario()

    def get_neteditor(self):
        return self._mainframe.get_modulegui('coremodules.network').get_neteditor()

    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        # self._neteditor = mainframe.add_view("Network", Neteditor)

        # mainframe.browse_obj(self._module)
        self.make_menu()
        # self.make_toolbar()

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        scenario = self.get_scenario()
        # print ('tlclusters.refresh_widgets',scenario.simulation,self._simulation != scenario.simulation)

        is_refresh = False
        if self._simulation != scenario.simulation:
            del self._demand
            del self._tlclusters
            del self._simulation
            self._demand = scenario.demand
            self._simulation = scenario.simulation
            self._tlclusters = self._simulation.add_simobject(ident='tlclusters', SimClass=tlcontrol.TlClusters)
            is_refresh = True
            # if (self._prtservice is not None)&(self._simulation is not None):
            # print ' self._simulation.results,self._prtservice._results:', self._simulation.results,self._prtservice.get_results(),id(self._simulation.results), id(self._prtservice.get_results())
            # print '  self._simulation.results',id(self._simulation.results)

    def make_menu(self):
        menubar = self._mainframe.menubar
        menubar.append_menu('plugins/trafficlight control', bitmap=self.get_icon('icon_tls_24px.png'),)
        # if sumo.traci is not None:
        menubar.append_item('plugins/trafficlight control/browse',
                            self.on_browse_obj,  # common function in modulegui
                            info='View and browse traffic light control in object panel.',
                            bitmap=self.get_agileicon('icon_browse_24px.png'),  # ,
                            )
        menubar.append_item('plugins/trafficlight control/import traffic light cluster...',
                            self.on_import_tlcluster,  # common function in modulegui
                            # bitmap = self.get_agileicon('icon_browse_24px.png'),#,
                            )

        if is_mpl:
            menubar.append_item('plugins/trafficlight control/plot legresults...',
                                self.on_plot_legresults,
                                bitmap=self.get_icon('icon_mpl.png'),  # ,
                                )

    def on_import_tlcluster(self, event=None):
        """Import traffic light cluster from text file."""
        p = tlcontrol.TlClusterImporter(self._tlclusters, logger=self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, p, immediate_apply=True)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() != 'success':  # val == wx.ID_CANCEL:
            dlg.Destroy()
        self._mainframe.browse_obj(self._tlclusters)

    def on_plot_legresults(self, event=None):
        """Plot different metrices of legresults over time."""
        p = results_mpl.LegresultsPlotter(self._simulation.results, logger=self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, p, immediate_apply=True)

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        if dlg.get_status() != 'success':  # val == wx.ID_CANCEL:
            dlg.Destroy()
        self._mainframe.browse_obj(self._tlclusters)
