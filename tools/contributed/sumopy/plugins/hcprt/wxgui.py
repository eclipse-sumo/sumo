# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
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
# @date   2012

import os
import wx
import numpy as np

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.ogleditor import *
from agilepy.lib_base.processes import Process
from agilepy.lib_wx.processdialog import ProcessDialog, ProcessDialogInteractive

from coremodules.network import routing
from coremodules.demand import demand
from coremodules.simulation import sumo, results
import hcprt

try:
    import results_mpl as results_mpl
    is_mpl = True  # we have matplotlib support
except:
    print "WARNING: python matplotlib package not installed, no matplotlib plots."
    is_mpl = False


class ResultDialog(ProcessDialog):
    def _get_buttons(self):
        buttons = [('Plot and close',   self.on_run,      'Plot in matplotlib window and close this window thereafter.'),
                   ('Plot',   self.on_show,      'Plot in matplotlib window.'),
                   ]
        defaultbutton = 'Plot and close'
        standartbuttons = ['cancel', ]

        return buttons, defaultbutton, standartbuttons

    def on_show(self, event):
        self.process.show()


class WxGui(ModuleGui):
    """Contains functions that communicate between PRT plugin and the widgets of the main wx gui
    and the functions of the plugin.
    """

    def __init__(self, ident):
        self._prtservice = None
        self._demand = None
        self._simulation = None
        self._init_common(ident,  priority=100003,
                          icondirpath=os.path.join(os.path.dirname(__file__), 'images'))

    def get_module(self):
        return self._prtservice

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
        print 'prtgui.refresh_widgets', self._simulation != scenario.simulation

        is_refresh = False
        if self._simulation != scenario.simulation:
            del self._demand
            del self._prtservice
            del self._simulation
            self._demand = scenario.demand
            self._simulation = scenario.simulation
            self._prtservice = self._simulation.add_simobject(ident='hcprtservice', SimClass=hcprt.HcPrtService)
            is_refresh = True
            #neteditor = self.get_neteditor()
            # neteditor.get_toolbox().add_toolclass(AddPrtCompressorTool)
            # if (self._prtservice is not None)&(self._simulation is not None):
            # print ' self._simulation.results,self._prtservice._results:', self._simulation.results,self._prtservice.get_results(),id(self._simulation.results), id(self._prtservice.get_results())
            # print '  self._simulation.results',id(self._simulation.results)

    def make_menu(self):
        menubar = self._mainframe.menubar
        menubar.append_menu('plugins/hcprt', bitmap=self.get_icon('icon_hcprt.png'),)
        if sumo.traci is not None:
            menubar.append_item('plugins/hcprt/browse',
                                self.on_browse_obj,  # common function in modulegui
                                info='View and browse High Capacity PRT in object panel.',
                                bitmap=self.get_agileicon('icon_browse_24px.png'),  # ,
                                )

            menubar.append_item('plugins/hcprt/make stops',
                                self.on_make_stops,
                                info='Make HCPRT stops from PT stops with PRT access ("custom1").',
                                # bitmap = self.get_icon('icon_sumo.png'),#,
                                )
            # menubar.append_item( 'plugins/prt/update compressors',
            #    self.on_update_compressors,
            #    #bitmap = self.get_icon('icon_sumo.png'),#,
            #    )

            # menubar.append_item( 'plugins/prt/clear compressors',
            #    self.on_clear_compressors,
            #    bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            #    )
            # menubar.append_item( 'plugins/prt/update decompressors',
            #    self.on_update_decompressors,
            #    #bitmap = self.get_icon('icon_sumo.png'),#,
            #    )
            # menubar.append_item( 'plugins/prt/clear decompressors',
            #    self.on_clear_decompressors,
            #    bitmap = wx.ArtProvider.GetBitmap(wx.ART_DELETE,wx.ART_MENU),
            #    )

            menubar.append_item('plugins/hcprt/make merge nodes',
                                self.on_make_merges,
                                #info='Make PRT merge nodes.',
                                # bitmap = self.get_icon('icon_sumo.png'),#,
                                )
            menubar.append_item('plugins/hcprt/calculate stop to stop times',
                                self.on_make_times_stop_to_stop,
                                # bitmap = self.get_icon('icon_sumo.png'),#,
                                )

            menubar.append_item('plugins/hcprt/add vehicles...',
                                self.on_add_vehicles,
                                info='Add PRT vehicles to network.',
                                # bitmap = self.get_icon('icon_sumo.png'),#,
                                )

            menubar.append_item('plugins/hcprt/clear vehicles.',
                                self.on_clear_vehicles,
                                info='Clear all PRT vehicles from network.',
                                bitmap=wx.ArtProvider.GetBitmap(wx.ART_DELETE, wx.ART_MENU),
                                )

            menubar.append_menu('plugins/hcprt/results',
                                bitmap=self.get_icon('icon_results_24px.png'),  # ,
                                )
            if is_mpl:
                menubar.append_item('plugins/hcprt/results/stopresults with matplotlib',
                                    self.on_mpl_stopresults,
                                    bitmap=self.get_icon('icon_mpl.png'),  # ,
                                    )

    def on_make_stops(self, event=None):
        self._prtservice.prtstops.make_from_net()
        self._mainframe.browse_obj(self._prtservice.prtstops)

    def on_make_merges(self, event=None):

        self._prtservice.mergenodes.make_from_net()
        self._mainframe.browse_obj(self._prtservice.mergenodes)

    def on_make_times_stop_to_stop(self, event=None):
        """Determine stop to sto time matrix"""

        self._prtservice.make_times_stop_to_stop()
        self._mainframe.browse_obj(self._prtservice)

    def on_add_vehicles(self, event=None):
        p = hcprt.VehicleAdder(self._prtservice.prtvehicles, logger=self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, p, immediate_apply=True)

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
            self._mainframe.browse_obj(self._prtservice.prtvehicles)

    def on_clear_vehicles(self, event=None):
        self._prtservice.prtvehicles.clear()
        self._prtservice.prtvehicles.make_vtype(is_reset_vtype=True)
        self._mainframe.browse_obj(self._prtservice.prtvehicles)

    def on_mpl_stopresults(self, event=None):
        print 'on_mpl_stopresults', id(self._simulation.results)  # ,id(self._prtservice.get_results())
        if self._prtservice is not None:
            if self._simulation is not None:
                resultplotter = results_mpl.StopresultsPlotter(self._simulation.results,  # self._prtservice.get_results(),
                                                               logger=self._mainframe.get_logger())
                dlg = ResultDialog(self._mainframe, resultplotter)

                dlg.CenterOnScreen()

                # this does not return until the dialog is closed.
                val = dlg.ShowModal()
                if dlg.get_status() != 'success':  # val == wx.ID_CANCEL:
                    dlg.Destroy()

                elif dlg.get_status() == 'success':
                    # apply current widget values to scenario instance
                    dlg.apply()
                    dlg.Destroy()
