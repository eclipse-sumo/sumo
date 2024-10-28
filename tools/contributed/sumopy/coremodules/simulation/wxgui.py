# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2024 German Aerospace Center (DLR) and others.
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

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.processdialog import ProcessDialog, ProcessDialogInteractive
from coremodules.scenario.scenario import Scenario
from coremodules.network import routing
from coremodules.misc.matplottools import ResultDialog
from . import sumo
from . import results

from .result_oglviewer import Resultviewer

try:
    from . import results_mpl as results_mpl
    is_mpl = True  # we have matplotlib support
except:
    print("WARNING: python matplotlib package not installed, no matplotlib plots.")
    is_mpl = False


class EnergyResultDialog(ProcessDialog):
    def _get_buttons(self):
        buttons = [('Plot and close',   self.on_run,      'Plot  energy results in matplotlib window and close this window thereafter.'),
                   ('Plot',   self.on_show,      'Plot energy results in matplotlib window.'),
                   ('Save Options...', self.on_save_options, self.on_save_options.__doc__),
                   ('Load Options...', self.on_load_options, self.on_load_options.__doc__),
                   ]
        defaultbutton = 'Plot and close'
        standartbuttons = ['cancel', ]

        return buttons, defaultbutton, standartbuttons

    def on_show(self, event):
        self.process.show()


class WxGui(ModuleGui):
    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """

    def __init__(self, ident):
        self._net = None
        self._init_common(ident,  priority=10000,
                          icondirpath=os.path.join(os.path.dirname(__file__), 'images'))
        self._simulation = None
        self.simulator = None

    def get_module(self):
        return self._simulation

    def get_scenario(self):
        return self._mainframe.get_modulegui('coremodules.scenario').get_module()

    def get_neteditor(self):
        return self._mainframe.get_modulegui('coremodules.network').get_neteditor()

    def get_resultviewer(self):
        return self._resultviewer

    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        print('SimulationGui.init_widgets')
        # mainframe.browse_obj(self._net)
        self.make_menu()
        self.make_toolbar()
        self._resultviewer = mainframe.add_view("Result viewer", Resultviewer)
        print('  self._resultviewer', self._resultviewer, self._resultviewer.get_drawing())

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        scenario = self.get_scenario()
        print('simulation.WxGui.refresh_widgets', self._simulation != scenario.simulation)
        is_refresh = False

        if self._simulation != scenario.simulation:
            print('  id(self._simulation)', id(self._simulation), 'id(scenario.simulation)', id(scenario.simulation), scenario.rootname)
            del self._simulation
            self._simulation = scenario.simulation
            is_refresh = True

        # if self._simulation.results is not None:
        # print '   results is_modified',self._simulation.results.is_modified()
        # if is_refresh
        if self._simulation.results.is_modified():
            #
            print('  refresh of _resultviewer')
            drawing = self._resultviewer.set_results(self._simulation.results)
        #    canvas = self._neteditor.get_canvas()
        else:
            print('  no refresh of _resultviewer :(')

    def make_menu(self):
        # print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu('simulation', bitmap=self.get_icon('icon_sumo.png'))

        menubar.append_item('simulation/browse',
                            self.on_browse_obj,  # common function in modulegui
                            info='View and browse simulation panel.',
                            bitmap=self.get_agileicon('icon_browse_24px.png'),  # ,
                            )

        menubar.append_menu('simulation/microscopic simulation',
                            bitmap=self.get_icon('icon_sumo.png'),
                            )

        menubar.append_item('simulation/microscopic simulation/SUMO...',
                            self.on_sumo,
                            bitmap=self.get_icon('icon_sumo.png'),  # ,
                            )

        # menubar.append_item( 'simulation/micro-simulation/Low memory SUMO...',
        #    self.on_sumo_lowmem,
        #    info='Define simulation parameters and simulate with SUMO micro-simulator. Low memory is used because the SUMOPy scenario will be deleted prior to run the simulation.',
        #    bitmap = self.get_icon('icon_sumo.png'),#,
        #    )

        if sumo.traci is not None:
            menubar.append_item('simulation/microscopic simulation/SUMO traci...',
                                self.on_sumo_traci,
                                info='Define simulation parameters and simulate with SUMO with interactive control via TRACI.',
                                bitmap=self.get_icon('icon_sumo.png'),  # ,
                                )

        menubar.append_item('simulation/microscopic simulation/Dynamic User Equilibrium...',
                            self.on_duaiterate,
                            bitmap=self.get_icon('icon_sumo.png'),  # ,
                            )

        menubar.append_menu('simulation/mesoscopic simulation',
                            bitmap=self.get_icon('icon_sumo.png'),
                            )

        menubar.append_item('simulation/mesoscopic simulation/MESO...',
                            self.on_meso,
                            )

        menubar.append_menu('simulation/macroscopic simulation',
                            #bitmap = self.get_icon('icon_sumo.png'),
                            )

        menubar.append_item('simulation/macroscopic simulation/macroscopic router...',
                            self.on_maroute,
                            # bitmap = self.get_icon('icon_sumo.png'),#,
                            )

        menubar.append_item('simulation/macroscopic simulation/estimate entered from routes',
                            self.on_estimate_entered_demand,
                            info='Use routes from demand to compute how many vehicle entered each edge.',
                            # bitmap = self.get_icon('icon_sumo.png'),#,
                            )

        menubar.append_item('simulation/macroscopic simulation/estimate entered from turnflows',
                            self.on_estimate_entered_turnflows,
                            info='Use turnflows from demand to compute how many vehicle entered each edge.',
                            # bitmap = self.get_icon('icon_sumo.png'),#,
                            )

        menubar.append_menu('simulation/results',
                            bitmap=self.get_icon('icon_results_24px.png'),  # ,
                            )

        menubar.append_item('simulation/results/browse',
                            self.on_show_results,
                            info='Browse simulation result table and graphics.',
                            bitmap=self.get_agileicon('icon_browse_24px.png'),
                            )

        # TODO: Error: Simresults instance has no attribute 'process'
        # migh have been implemented in an older version!!
        menubar.append_item('simulation/results/process',
                            self.on_process_results,
                            info='Process results. Update demand models with results from last simulation run.',
                            #bitmap = self.get_agileicon('icon_browse_24px.png'),#
                            )

        menubar.append_item('simulation/results/add edge length to edgeresults',
                            self.on_add_edgelength,
                            )

        menubar.append_item('simulation/results/add detector flow measurements to edgeresults',
                            self.on_add_detectorflows_to_edgeresults,
                            )

        # menubar.append_item( 'simulation/results/safe',
        #    self.on_save,
        #    info='Save current results in a Python binary file.',
        #    bitmap = wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE,wx.ART_MENU),
        #    )

        menubar.append_item('simulation/results/safe as...',
                            self.on_save_as,
                            info='Save results in a new Python binary file.',
                            bitmap=wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE_AS, wx.ART_MENU),
                            )

        menubar.append_item('simulation/results/export edge results in csv...',
                            self.on_export_edgeresults_csv,
                            info='Save edge related results in a CSV file.',
                            bitmap=self.get_agileicon("Document_Export_24px.png"),
                            )
        menubar.append_item('simulation/results/export trip results in csv...',
                            self.on_export_tripresults_csv,
                            info='Save trip related results in a CSV file.',
                            bitmap=self.get_agileicon("Document_Export_24px.png"),
                            )

        menubar.append_item('simulation/results/filter edgresults...',
                            self.on_filter_edgeresults,
                            )

        if is_mpl:
            menubar.append_item('simulation/results/plot edge results on map',
                                self.on_plot_results,
                                bitmap=self.get_icon('icon_mpl.png'),  # ,
                                )

            menubar.append_item('simulation/results/plot 2 edge results on XY plot...',
                                self.on_plot_xy_edgeresults,
                                bitmap=self.get_icon('icon_mpl.png'),  # ,
                                )
            menubar.append_item('simulation/results/plot speedprofiles...',
                                self.on_plot_speedprofiles,
                                bitmap=self.get_icon('icon_mpl.png'),  # ,
                                )

            menubar.append_item('simulation/results/plot travel times...',
                                self.on_plot_traveltimes,
                                bitmap=self.get_icon('icon_mpl.png'),  # ,
                                )

            menubar.append_item('simulation/results/plot electrical energy...',
                                self.on_plot_electrical_energy_results,
                                bitmap=self.get_icon('icon_mpl.png'),  # ,
                                )
            # menubar.append_item( 'simulation/results/Flowcompare with matplotlib',
            #    self.on_mpl_flowcompare,
            #    info='Compare simulated and estimated flows in Matplotlib plotting envitonment.',
            #    bitmap = self.get_icon('icon_mpl.png'),#,
            #    )

        menubar.append_item('simulation/results/open...',
                            self.on_open,
                            info='Open previousely saved simulation results from a Python binary file.',
                            bitmap=wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_MENU),
                            )

        menubar.append_item('simulation/results/clear all',
                            self.on_clear_results,
                            bitmap=wx.ArtProvider.GetBitmap(wx.ART_DELETE, wx.ART_MENU),
                            )

        # menubar.append_item( 'simulation/results/refresh results',
        #    self.on_refresh,
        #    info='refresh results.',
        #    #bitmap = self.get_icon('icon_sumo.png'),#,
        #    )

    def on_process_results(self, event=None):
        """
        Deletes all results.
        """
        if (self._simulation.results is not None):
            self._simulation.results.process(self.simulator)
            self._mainframe.browse_obj(self._simulation.results)
            self._mainframe.select_view(name="Result viewer")  # !!!!!!!! tricky, crashes without

    def on_clear_results(self, event=None):
        if (self._simulation.results is not None):
            self._simulation.results.clear_results()
            self._mainframe.browse_obj(self._simulation.results)
            self._mainframe.select_view(name="Result viewer")  # !!!!!!!! tricky, crashes without
            self.refresh_widgets()

    def on_show_results(self, event=None):
        if self._simulation.results is None:
            self._simulation.results = results.Simresults(ident='simresults', simulation=self._simulation)

        self._mainframe.browse_obj(self._simulation.results)
        self._mainframe.select_view(name="Result viewer")  # !!!!!!!! tricky, crashes without

    # def on_refresh(self,event = None):
    #    #print 'on_refresh neteditor',id(self._neteditor.get_drawing())
    #
    #
    #    wx.CallAfter(self.refresh_widgets)
    #
    #    if event:
    #        event.Skip()

    def on_open(self, event=None):

        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "Python binary result files (*.res.obj)|*.res.obj|Python binary files (*.obj)|*.obj"
        wildcards = wildcards_obj+"|"+wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Open results file",
            defaultDir=self.get_scenario().get_workdirpath(),
            #defaultFile = os.path.join(scenario.get_workdirpath(), scenario.format_ident()+'.obj'),
            wildcard=wildcards,
            style=wx.OPEN | wx.CHANGE_DIR
        )

        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        is_newresults = False
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            filepath = dlg.GetPath()
            if len(filepath) > 0:
                if self._simulation.results is not None:
                    # browse away from results
                    # self._mainframe.browse_obj(self._simulation.results.get_scenario())
                    del self._simulation.results

                self._simulation.results = results.load_results(filepath,
                                                                parent=self._simulation,
                                                                logger=self._mainframe.get_logger()
                                                                )
                is_newresults = True

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

        if is_newresults:
            # this should update all widgets for the new scenario!!
            # print 'call self._mainframe.refresh_moduleguis()'
            self._mainframe.browse_obj(self._simulation.results)
            self._mainframe.select_view(name="Result viewer")  # !!!!!!!!tricky, crashes without
            self.refresh_widgets()
            # wx.CallAfter(self.refresh_widgets)
            # self._mainframe.refresh_moduleguis()
            #if event: event.Skip()

    # def on_save(self, event=None):
    #    if self._simulation.results is None: return
    #    self._simulation.results.save()
    #    #if event:
    #    #    event.Skip()

    def on_save_as(self, event=None):
        if self._simulation.results is None:
            return
        scenario = self._simulation.results.get_scenario()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "Python binary result files (*.res.obj)|*.res.obj|Python binary files (*.obj)|*.obj"
        wildcards = wildcards_obj+"|"+wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Save results to file",
            defaultDir=scenario.get_workdirpath(),
            #defaultFile = scenario.get_rootfilepath()+'.res.obj',
            wildcard=wildcards,
            style=wx.SAVE | wx.CHANGE_DIR
        )
        val = dlg.ShowModal()
        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if val == wx.ID_OK:
            # This returns a Python list of files that were selected.
            filepath = dlg.GetPath()
            if len(filepath) > 0:
                # now set new filename and workdir
                self._simulation.results.save(filepath)

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

    def on_export_edgeresults_csv(self, event=None):
        if self._simulation.results is None:
            return
        scenario = self._simulation.results.get_scenario()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "CSV files (*.csv)|*.csv|Text file (*.txt)|*.txt"
        wildcards = wildcards_obj+"|"+wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Export edge results to CSV file",
            defaultDir=scenario.get_workdirpath(),
            # defaultFile = scenario.get_rootfilepath()+'.edgeres.csv',
            wildcard=wildcards,
            style=wx.SAVE | wx.CHANGE_DIR
        )
        val = dlg.ShowModal()
        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if val == wx.ID_OK:
            # This returns a Python list of files that were selected.
            filepath = dlg.GetPath()
            if len(filepath) > 0:
                # now set new filename and workdir
                self._simulation.results.edgeresults.export_csv(filepath)

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

    def on_export_tripresults_csv(self, event=None):
        if self._simulation.results is None:
            return
        scenario = self._simulation.results.get_scenario()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "CSV files (*.csv)|*.csv|Text file (*.txt)|*.txt"
        wildcards = wildcards_obj+"|"+wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Export trip results to CSV file",
            defaultDir=scenario.get_workdirpath(),
            # defaultFile = scenario.get_rootfilepath()+'.tripres.csv',
            wildcard=wildcards,
            style=wx.SAVE | wx.CHANGE_DIR
        )
        val = dlg.ShowModal()
        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if val == wx.ID_OK:
            # This returns a Python list of files that were selected.
            filepath = dlg.GetPath()
            if len(filepath) > 0:
                # now set new filename and workdir
                self._simulation.results.tripresults.export_csv(filepath)

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

    def on_filter_edgeresults(self, event=None):
        """Filter edgeresults by zone, etc."""
        if self._simulation.results is None:
            self._simulation.results = results.Simresults(ident='simresults', simulation=self._simulation)

        edgeresultfilter = results.EdgeresultFilter(self._simulation.results.edgeresults,
                                                    logger=self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, edgeresultfilter)

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
            self._mainframe.browse_obj(self._simulation.results.edgeresults)

    def on_plot_results(self, event=None):
        """Plot edge results on map using the Matplotlib plotting envitonment."""
        if self._simulation.results is None:
            self._simulation.results = results.Simresults(ident='simresults', simulation=self._simulation)

        if is_mpl:
            resultplotter = results_mpl.Resultplotter(self._simulation.results,
                                                      logger=self._mainframe.get_logger())
            dlg = ResultDialog(self._mainframe, resultplotter)

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

    def on_plot_xy_edgeresults(self, event=None):
        """Plot one edge result attribute versus another result attribute using matplotlib."""
        if self._simulation.results is None:
            self._simulation.results = results.Simresults(ident='simresults', simulation=self._simulation)

        if is_mpl:
            resultplotter = results_mpl.XYEdgeresultsPlotter(self._simulation.results,
                                                             logger=self._mainframe.get_logger())
            dlg = ResultDialog(self._mainframe, resultplotter)

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

    def on_plot_speedprofiles(self, event=None):
        """Plot speed profiles using the Matplotlib plotting envitonment."""
        if self._simulation.results is None:
            self._simulation.results = results.Simresults(ident='simresults', simulation=self._simulation)

        if is_mpl:
            resultplotter = results_mpl.SpeedprofilePlotter(self._simulation.results,
                                                            logger=self._mainframe.get_logger())
            dlg = ResultDialog(self._mainframe, resultplotter)

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

    def on_plot_traveltimes(self, event=None):
        """Plot travel times to/from specific edges using the Matplotlib plotting envitonment."""
        if self._simulation.results is None:
            self._simulation.results = results.Simresults(ident='simresults', simulation=self._simulation)

        if is_mpl:
            resultplotter = results_mpl.TraveltimePlotter(self._simulation.results,
                                                          logger=self._mainframe.get_logger())
            dlg = ResultDialog(self._mainframe, resultplotter)

            dlg.CenterOnScreen()

            # this does not return until the dialog is closed.
            val = dlg.ShowModal()
            if dlg.get_status() != 'success':  # val == wx.ID_CANCEL:
                dlg.Destroy()

            if dlg.get_status() == 'success':
                dlg.apply()
                dlg.Destroy()

    def on_plot_electrical_energy_results(self, event=None):
        """Plot energy results using the Matplotlib plotting envitonment."""
        if self._simulation.results is None:
            self._simulation.results = results.Simresults(ident='simresults', simulation=self._simulation)

        if is_mpl:
            resultplotter = results_mpl.ElectricalEnergyResultsPlotter(self._simulation.results,
                                                                       logger=self._mainframe.get_logger())
            dlg = EnergyResultDialog(self._mainframe, resultplotter)

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

    # def on_mpl_flowcompare(self, event = None):
    #    if self._simulation.results is None:
    #        self._simulation.results = results.Simresults(ident= 'simresults', simulation=self._simulation)
    #
    #    if is_mpl:
    #        resultplotter = results_mpl.Flowcomparison(  self._simulation.results,
    #                                                    logger = self._mainframe.get_logger())
    #        dlg = ResultDialog(self._mainframe, resultplotter)
    #
    #        dlg.CenterOnScreen()
    #
    #        # this does not return until the dialog is closed.
    #        val = dlg.ShowModal()
    #        #print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
    #        #print '  status =',dlg.get_status()
    #        if dlg.get_status() != 'success':#val == wx.ID_CANCEL:
    #            #print ">>>>>>>>>Unsuccessful\n"
    #            dlg.Destroy()
    #
    #        if dlg.get_status() == 'success':
    #            #print ">>>>>>>>>successful\n"
    #            # apply current widget values to scenario instance
    #            dlg.apply()
    #            dlg.Destroy()

    def on_add_detectorflows_to_edgeresults(self, event=None):
        """Add detector flow measurements to edge results.
        """
        # self._demand.detectorflows.clear()
        # self._mainframe.browse_obj(self._demand.detectorflows)
        detectorflows = self.get_scenario().demand.detectorflows
        detectorflows.add_flows_to_edgeresults(self._simulation.results.edgeresults)
        self._mainframe.browse_obj(self._simulation.results.edgeresults)

    def on_sumo(self, event=None):
        """
        Set simulation parameters and simulate with SUMO micro-simulator
        """
        # self.prepare_results()
        self.simulator = sumo.Sumo(scenario=self.get_scenario(),
                                   results=self._simulation.results,
                                   logger=self._mainframe.get_logger(),
                                   is_gui=True,
                                   is_export_net=True,
                                   is_export_poly=True,
                                   is_export_rou=True,
                                   is_prompt_filepaths=False,
                                   )
        self.open_sumodialog()

    # def on_sumo_lowmem(self, event = None):
    #    #self.prepare_results()
    #    # delete current scenario
    #    #del self._scenario
    #    scenario = self.get_scenario()
    #
    #    # set dummy scenario
    #    dummyscenario = Scenario('empty', name_scenario = 'Empty')
    #    self._mainframe.get_modulegui('coremodules.scenario').set_module(dummyscenario)
    #    #self._scenario = scenariocreator.get_scenario()
    #
    #    # this should update all widgets for the new scenario!!
    #    #print 'call self._mainframe.refresh_moduleguis()'
    #    self._mainframe.refresh_moduleguis()
    #    self._mainframe.browse_obj(dummyscenario)
    #
    #    del scenario

    def on_sumo_prompt(self, event=None):
        # self.prepare_results()
        self.simulator = sumo.Sumo(scenario=self.get_scenario(),
                                   results=self._simulation.results,
                                   logger=self._mainframe.get_logger(),
                                   is_gui=True,
                                   is_export_net=False,
                                   is_export_poly=False,
                                   is_export_rou=False,
                                   is_prompt_filepaths=True,
                                   )
        self.open_sumodialog()

    def on_sumo_traci(self, event=None):
        # self.prepare_results()
        self.simulator = sumo.SumoTraci(
            scenario=self.get_scenario(),
            results=self._simulation.results,
            logger=self._mainframe.get_logger(),
            is_gui=True,
            is_export_net=True,
            is_export_poly=True,
            is_export_rou=True,
            is_prompt_filepaths=False,
            is_quit_on_end=True,
            is_start=True,
        )
        self.open_sumodialog_interactive()

    def open_sumodialog(self):
        dlg = ProcessDialog(self._mainframe, self.simulator)

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
            if self.simulator.status == 'success':
                self.simulator.import_results()

            self._mainframe.browse_obj(self._simulation.results)
            # print '  select_view'
            # self._mainframe.select_view(name = "Result viewer") #!!!!!!!!tricky, crashes without
            # print '  call refresh_widgets'
            self.refresh_widgets()
            # print 'call self._mainframe.refresh_moduleguis()'
            # self._mainframe.refresh_moduleguis()

    def open_sumodialog_interactive(self):
        dlg = ProcessDialogInteractive(self._mainframe,
                                       self.simulator,
                                       title='SUMO-Traci Dialog',
                                       func_close=self.close_sumodialog_interactive,
                                       )

        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        #val = dlg.ShowModal()
        print('open_sumodialog_interactive')
        dlg.Show()
        dlg.MakeModal(True)
        # print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
        # print '  status =',dlg.get_status()
        # print 'returned to main window self.simulator.status',self.simulator.status

    def close_sumodialog_interactive(self, dlg):
        # called before destroying the dialog
        if self.simulator.status == 'success':
            self.simulator.import_results()

            self._mainframe.browse_obj(self._simulation.results)
            self._mainframe.select_view(name="Result viewer")  # !!!!!!!!tricky, crashes without
            self.refresh_widgets()
            # print 'call self._mainframe.refresh_moduleguis()'
            # self._mainframe.refresh_moduleguis()

    def on_estimate_entered_demand(self, event=None):
        results = self._simulation.results
        trips = self.get_scenario().demand.trips
        results.edgeresults.add_entered_est(*trips.estimate_entered())
        self._mainframe.browse_obj(results.edgeresults)
        self._mainframe.select_view(name="Result viewer")  # !!!!!!!!tricky, crashes without
        self.refresh_widgets()

    def on_add_edgelength(self, event=None):
        """
        Add edgelength to edgeresults.
        """
        results = self._simulation.results
        results.edgeresults.add_edgelength()
        self._mainframe.browse_obj(results.edgeresults)
        self.refresh_widgets()

    def on_estimate_entered_turnflows(self, event=None):
        results = self._simulation.results
        turnflows = self.get_scenario().demand.turnflows
        ids_edge, flows = turnflows.estimate_entered()
        print('on_estimate_entered_turnflows')
        print('ids_edge', ids_edge)
        print('flows', flows)
        results.edgeresults.add_entered_est(*turnflows.estimate_entered())
        self._mainframe.browse_obj(results.edgeresults)
        self._mainframe.select_view(name="Result viewer")  # !!!!!!!!tricky, crashes without
        self.refresh_widgets()

    def on_duaiterate(self, event=None):
        """Dynamic User equilibrium Assignment (DuaIterate).
        """

        p = sumo.Duaiterate(self.get_scenario(),
                            results=self._simulation.results,
                            logger=self._mainframe.get_logger(),
                            )
        dlg = ProcessDialog(self._mainframe, p)

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
            self._mainframe.browse_obj(self.get_scenario().demand.trips)

            # p.import_results()
            # self._mainframe.browse_obj(p.get_results())
            # self._mainframe.select_view(name = "Result viewer") #!!!!!!!!tricky, crashes without
            # self.refresh_widgets()

    def on_meso(self, event=None):
        """Simulate scenario with Mesoscopic, queue based simulator.
        """

        p = sumo.Meso(
            scenario=self.get_scenario(),
            results=self._simulation.results,
            logger=self._mainframe.get_logger(),
            is_gui=True,
            is_export_net=True,
            is_export_poly=True,
            is_export_rou=True,
            is_prompt_filepaths=False,
            is_quit_on_end=False,
            is_start=False,
        )

        dlg = ProcessDialog(self._mainframe, p)

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
            # self._mainframe.browse_obj(self._simulation.results)
            p.import_results()
            self._mainframe.browse_obj(p.get_results())
            self._mainframe.select_view(name="Result viewer")  # !!!!!!!!tricky, crashes without
            self.refresh_widgets()

    def on_maroute(self, event=None):
        """Simulate scenario with Macrosopic router (marouter).
        """

        p = routing.MaRouter(self.get_scenario(),
                             results=self._simulation.results,
                             logger=self._mainframe.get_logger(),
                             )
        dlg = ProcessDialog(self._mainframe, p)

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
            # self._mainframe.browse_obj(self._simulation.results)
            p.import_results()
            self._mainframe.browse_obj(p.get_results())
            self._mainframe.select_view(name="Result viewer")  # !!!!!!!!tricky, crashes without
            self.refresh_widgets()
