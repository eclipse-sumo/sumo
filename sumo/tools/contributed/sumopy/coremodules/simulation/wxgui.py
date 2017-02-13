import os
import wx

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.processdialog import ProcessDialog
import sumo
import results
from result_oglviewer import Resultviewer

try:
    import results_mpl as results_mpl
    is_mpl = True  # we have matplotlib support
except:
    print "WARNING: python matplotlib package not installed, no matplotlib plots."
    is_mpl = False


class ResultDialog(ProcessDialog):

    def _get_buttons(self):
        buttons = [('Plot and close',   self.on_run,      'Plot  selected quantity in matplotlib window and close this window thereafter.'),
                   ('Plot',   self.on_show,
                    'Plot selected quantity  in matplotlib window.'),
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
        self._results = None

    def get_module(self):
        return self.get_scenario()

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

        # mainframe.browse_obj(self._net)
        self.make_menu()
        self.make_toolbar()
        self._resultviewer = mainframe.add_view("Result viewer", Resultviewer)

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        is_refresh = False

        #scenario = self.get_scenario()
        # print '\n\nResults refresh_widgets'

        if self._results is not None:
            print '   results is_modified', self._results.is_modified()
            if self._results.is_modified():
                #
                is_refresh = True

        if is_refresh:
            drawing = self._resultviewer.set_results(self._results)
        #    canvas = self._neteditor.get_canvas()

    def make_menu(self):
        # print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu(
            'simulation', bitmap=self.get_icon('icon_sumo.png'))

        menubar.append_menu('simulation/SUMO',
                            bitmap=self.get_icon('icon_sumo.png'),
                            )

        menubar.append_item('simulation/SUMO/export routes and simulate...',
                            self.on_sumo_routes,
                            info='Export current routes, network and poly and SUMO micro-simulate.',
                            bitmap=self.get_icon('icon_sumo.png'),  # ,
                            )

        menubar.append_item('simulation/SUMO/choose files and simulate...',
                            self.on_sumo,
                            info='SUMO micro-simulate current network. Route file(s) can be chosen.',
                            bitmap=self.get_icon('icon_sumo.png'),  # ,
                            )

        menubar.append_menu('simulation/results')
        menubar.append_item('simulation/results/browse',
                            self.on_show_results,
                            info='Browse simulation result table and graphics.',
                            # bitmap = self.get_icon('icon_sumo.png'),#,
                            )

        menubar.append_item('simulation/results/safe',
                            self.on_save,
                            info='Save current results in a Python binary file.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_FILE_SAVE_AS, wx.ART_MENU),
                            )

        menubar.append_item('simulation/results/safe as...',
                            self.on_save_as,
                            info='Save results in a new Python binary file.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_FILE_SAVE_AS, wx.ART_MENU),
                            )

        menubar.append_item('simulation/results/export edge results in csv...',
                            self.on_export_edgeresults_csv,
                            info='Save edge related results in a CSV file.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_FILE_SAVE_AS, wx.ART_MENU),
                            )
        menubar.append_item('simulation/results/export trip results in csv...',
                            self.on_export_tripresults_csv,
                            info='Save trip related results in a CSV file.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_FILE_SAVE_AS, wx.ART_MENU),
                            )

        if is_mpl:
            menubar.append_item('simulation/results/plot with matplotlib',
                                self.on_plot_results,
                                info='Plot results in Matplotlib plotting envitonment.',
                                # bitmap = self.get_icon('icon_sumo.png'),#,
                                )

        menubar.append_item('simulation/results/open...',
                            self.on_open,
                            info='Open previousely saved simulation results from a Python binary file.',
                            bitmap=wx.ArtProvider.GetBitmap(
                                wx.ART_FILE_OPEN, wx.ART_MENU),
                            )

        # menubar.append_item( 'simulation/results/refresh results',
        #    self.on_refresh,
        #    info='refresh results.',
        #    #bitmap = self.get_icon('icon_sumo.png'),#,
        #    )

    def on_show_results(self, event=None):
        if self._results == None:
            self._results = results.Simresults(
                ident='simresults', scenario=self.get_scenario())

        self._mainframe.browse_obj(self._results)
        self._mainframe.select_view(1)  # !!!!!!!!tricky, crashes without

        if event:
            event.Skip()

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
        wildcards = wildcards_obj + "|" + wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path
        # chosen.
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
                if self._results is not None:
                    # browse away from results
                    # self._mainframe.browse_obj(self._results.get_scenario())
                    del self._results

                self._results = results.load_results(filepath,
                                                     parent=self.get_scenario(),
                                                     logger=self._mainframe.get_logger()
                                                     )
                is_newresults = True

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

        if is_newresults:
            # this should update all widgets for the new scenario!!
            # print 'call self._mainframe.refresh_moduleguis()'
            self._mainframe.browse_obj(self._results)
            self._mainframe.select_view(1)  # !!!!!!!!tricky, crashes without
            self.refresh_widgets()
            # wx.CallAfter(self.refresh_widgets)
            # self._mainframe.refresh_moduleguis()
            #if event: event.Skip()

    def on_save(self, event=None):
        if self._results is None:
            return
        self._results.save()
        if event:
            event.Skip()

    def on_save_as(self, event=None):
        if self._results is None:
            return
        scenario = self._results.get_scenario()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "Python binary result files (*.res.obj)|*.res.obj|Python binary files (*.obj)|*.obj"
        wildcards = wildcards_obj + "|" + wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path
        # chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Save results to file",
            defaultDir=scenario.get_workdirpath(),
            defaultFile=scenario.get_rootfilepath() + '.res.obj',
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
                self._results.save(filepath)

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

    def on_export_edgeresults_csv(self, event=None):
        if self._results is None:
            return
        scenario = self._results.get_scenario()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "CSV files (*.csv)|*.csv|Text file (*.txt)|*.txt"
        wildcards = wildcards_obj + "|" + wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path
        # chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Export edge results to CSV file",
            defaultDir=scenario.get_workdirpath(),
            defaultFile=scenario.get_rootfilepath() + '.edgeres.csv',
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
                self._results.edgeresults.export_csv(filepath)

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

    def on_export_tripresults_csv(self, event=None):
        if self._results is None:
            return
        scenario = self._results.get_scenario()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "CSV files (*.csv)|*.csv|Text file (*.txt)|*.txt"
        wildcards = wildcards_obj + "|" + wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path
        # chosen.
        dlg = wx.FileDialog(
            self._mainframe, message="Export trip results to CSV file",
            defaultDir=scenario.get_workdirpath(),
            defaultFile=scenario.get_rootfilepath() + '.tripres.csv',
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
                self._results.tripresults.export_csv(filepath)

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

    def on_plot_results(self, event=None):
        if self._results == None:
            self._results = results.Simresults(
                ident='simresults', scenario=self.get_scenario())

        if is_mpl:
            resultplotter = results_mpl.Resultplotter(self._results,
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
        else:
            if event:
                event.Skip()

    def on_sumo_routes(self, event=None):
        self.prepare_results()
        self.simulator = sumo.Sumo(scenario=self.get_scenario(),
                                   results=self._results,
                                   logger=self._mainframe.get_logger(),
                                   is_gui=True,
                                   is_export_net=True,
                                   is_export_poly=True,
                                   is_export_rou=True,
                                   method_routechoice=None,
                                   )
        self.open_sumodialog()

    def on_sumo(self, event=None):
        self.prepare_results()
        self.simulator = sumo.Sumo(scenario=self.get_scenario(),
                                   results=self._results,
                                   logger=self._mainframe.get_logger(),
                                   is_gui=True,
                                   )
        self.open_sumodialog()

    def prepare_results(self, ident_results='simresults'):

        # TODO: here we should actually replace the current network
        # so we would need a clear net method in scenario
        # alternatively we could merge properly
        scenario = self.get_scenario()
        if self._results == None:
            self._results = results.Simresults(ident=ident_results,
                                               scenario=scenario)
        else:
            if self._results.parent != scenario:
                # uups scenario changed
                del self._results
                self._results = results.Simresults(
                    ident=ident_results, scenario=scenario)

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

            self._mainframe.browse_obj(self._results)
            self._mainframe.select_view(1)  # !!!!!!!!tricky, crashes without
            self.refresh_widgets()
            # print 'call self._mainframe.refresh_moduleguis()'
            # self._mainframe.refresh_moduleguis()
