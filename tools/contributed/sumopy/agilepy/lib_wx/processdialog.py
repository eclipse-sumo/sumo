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

# @file    processdialog.py
# @author  Joerg Schweizer
# @date    2012


import wx
import os
import objpanel
from wxmisc import AgileStatusbar


class ProcessDialogMixin:

    def get_status(self):
        return self.process.status

    def clear_log(self):
        self.get_logger().w('', 'message')
        self.get_logger().w('', 'action')

    def copy_options_to_results(self, results):
        # TODO
        pass

    def on_save_options(self, event):
        """
        Save current options to file.
        """
        dlg = wx.FileDialog(
            self, message="Save Options as ...",
            # defaultDir=os.getcwd(),
            # defaultFile="",
            wildcard='Python obj files (*.obj)|*.obj|All files (*.*)|*.*',
            style=wx.SAVE | wx.CHANGE_DIR
        )

        # This sets the default filter that the user will initially see. Otherwise,
        # the first filter in the list will be used by default.
        dlg.SetFilterIndex(0)

        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            filepath = dlg.GetPath()
            self.process.save_options(filepath)

    def on_load_options(self, event):
        """
        Load previously saved options from file. 
        """

        dlg = wx.FileDialog(self, message="Open object",
                            # defaultDir=os.getcwd(),
                            # defaultFile="",
                            wildcard='Python obj files (*.obj)|*.obj|All files (*.*)|*.*',
                            style=wx.OPEN | wx.CHANGE_DIR
                            )

        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            filepath = dlg.GetPath()
            self.process.load_options(filepath)

        self.restore()


class ProcessDialog(ProcessDialogMixin, objpanel.ObjPanelDialog):
    def __init__(self, parent,
                 process,
                 attrconfigs=None,
                 tables=None,
                 table=None, id=None, ids=None,
                 groupnames=None, show_groupnames=False,
                 title=None, size=wx.DefaultSize, pos=wx.DefaultPosition,
                 style=wx.DEFAULT_DIALOG_STYLE | wx.MAXIMIZE_BOX | wx.RESIZE_BORDER,
                 choose_id=False, choose_attr=False,
                 func_choose_id=None, func_change_obj=None, panelstyle='default',
                 immediate_apply=True):

        self.process = process
        if title is None:
            title = process.get_name()

        buttons, defaultbutton, standartbuttons = self._get_buttons()

        objpanel.ObjPanelDialog.__init__(self, parent, process,
                                         attrconfigs=None,
                                         id=None, ids=None,
                                         groupnames=['options', 'inputparameters'],
                                         show_groupnames=False,
                                         title=title, size=(800, 400), pos=wx.DefaultPosition,
                                         style=wx.DEFAULT_DIALOG_STYLE | wx.MAXIMIZE_BOX | wx.RESIZE_BORDER,
                                         choose_id=False, choose_attr=False,
                                         func_choose_id=None,
                                         func_change_obj=None,
                                         panelstyle='default',
                                         immediate_apply=immediate_apply,
                                         buttons=buttons,
                                         standartbuttons=standartbuttons,
                                         defaultbutton=defaultbutton,
                                         )

    def _get_buttons(self):
        buttons = [('Run',   self.on_run,      'Start the process!'),
                   #('Kill',   self.on_kill,      'Kill process in background.'),
                   ('Apply',   self.on_apply,      'Apply current options, complete them and make them consistent, but do not run.'),
                   ('Save Options...', self.on_save_options, self.on_save_options.__doc__),
                   ('Load Options...', self.on_load_options, self.on_load_options.__doc__),
                   ]
        defaultbutton = 'Run'
        standartbuttons = ['cancel', ]

        return buttons, defaultbutton, standartbuttons

    def get_process(self):
        return self.process

    def on_run(self, event=None):
        self.on_apply()
        self.process.run()
        if self.process.status == 'success':
            self.Destroy()

    def on_kill(self, event=None):
        self.process.kill()

    def on_apply(self, event=None):
        self.process.update_params()
        self.restore()

    def recreate_panel_proc(self, obj=None,
                            attrconfigs=None,
                            tables=None,
                            table=None, id=None, ids=None,
                            groupnames=None, show_groupnames=False,
                            show_title=True,
                            is_modal=False,
                            immediate_apply=False,
                            panelstyle='default',
                            func_change_obj=None,
                            func_choose_id=None,
                            func_choose_attr=None,
                            **buttonargs):
        """
        Recreates panel and destroys previous contents:
            attr = a list ith attribute names to be shown
            groups = list with group names to be shown
            show_groupnames = the group names are shown 
                              together with the respective attributes
            is_modal = if true the panel will be optimized for dialog window
            immediate_apply = changes to widgets will be immediately applied to 
                                obj attribute without pressing apply
            buttonargs = a dictionary with arguments for botton creation
                    obj = the object to be displayed
            id = used if there is only one single id to be displayed
            ids = used if a selection of ids need to be displayed
            func_change_obj = function to be called if user clicks on on object
                                with obj as argument
            func_choose_id = function to be called when user clicks on id(table row)
                                with id as argument
            func_choose_attr = function to be called when user clicks on attribute 
                                with attr as argument. In this case the attribute
                                names of scalars turn into buttons. Array attributes
                                are selected by clicking on the column name. 
        """
        # print 'ProcessDialog.recreate_panel',groupnames
        # for attrconfig in obj.get_attrman().get_configs():
        #   print '  attrconfig.attrname',attrconfig.attrname
        #self.id = id
        #self.ids = ids
        self.obj = obj
        self.func_change_obj = func_change_obj
        self.func_choose_id = func_choose_id
        self.func_choose_attr = func_choose_attr
        self._show_title = show_title

        if (attrconfigs is None) & (tables in (None, [])) & (table is None):
            attrconfigs = obj.get_attrman().get_configs()
            if not self._show_title:
                # titels of groupname are not shown so just
                # select attributes at this stage
                attrconfig_new = []
                for attrconfig in attrconfigs:
                    # print '  attrconfig.attrname',attrconfig.attrname
                    is_shown = False
                    for groupname in attrconfig.groupnames:
                        # print ' comp:',attrconfig.attrname,groupname,groupname in groupnames
                        if groupname in groupnames:
                            is_shown = True
                            break

                    if is_shown:
                        attrconfig_new.append(attrconfig)
                attrconfigs = attrconfig_new

        tables = None  # obj.get_tablemans()

        # print 'is_scalar_panel & is_multitab'

        sizer = self.init_notebook()
        self.add_scalarpage(attrconfigs=attrconfigs, groupnames=groupnames,
                            id=id, immediate_apply=immediate_apply, panelstyle=panelstyle,
                            is_modal=is_modal, show_title=show_title,
                            show_buttons=False, **buttonargs)

        for table in tables:
            self.add_tablepage(table, groupnames=groupnames)

        self.show_notebook()
        sizer = self.GetSizer()
        self.add_buttons(self, sizer,
                         is_modal=is_modal,
                         **buttonargs)
        # some widgets like color need this to expand into their maximum space
        self.restore()
        return True


class ProcessDialogInteractive(ProcessDialogMixin, wx.Frame):

    """
    A frame used for the ObjBrowser Demo

    """

    def __init__(self, parent,
                 process,
                 title=None,
                 func_close=None,
                 is_close_after_completion=True,
                 ):

        if title is None:
            title = process.get_name()
        wx.Frame.__init__(self, parent, -1, title=title, pos=wx.DefaultPosition, size=wx.DefaultSize)

        self.parent = parent
        self.process = process

        self.func_close = func_close
        # if self.func_close is None:
        self.is_close_after_completion = is_close_after_completion
        # else:
        #    self.is_close_after_completion = False
        #self._is_run = False

        self.Bind(wx.EVT_TIMER, self.on_step)
        self.timer = wx.Timer(self)
        # Set up the MenuBar
        #MenuBar = wx.MenuBar()
        #file_menu = wx.Menu()
        #item = file_menu.Append(-1, "&Import CSV...","Import OD data from a CSV text file with format <zonename orig>, <zonename dest>,<number of trips>")
        #self.Bind(wx.EVT_MENU, self.on_import_csv, item)
        #item = file_menu.Append(-1, "&Import Exel...","Import OD data from an Exel file.")
        #self.Bind(wx.EVT_MENU, self.on_import_exel, item)
        #help_menu = wx.Menu()
        # item = help_menu.Append(-1, "&About",
        #                        "More information About this program")
        #self.Bind(wx.EVT_MENU, self.on_menu, item)
        #MenuBar.Append(help_menu, "&Help")
        # self.SetMenuBar(MenuBar)

        #################################################################
        # create statusbar
        #self.statusbar = AgileStatusbar(self)
        self.statusbar = AgileStatusbar(self, fields=[  # ('action',-4),
                                        ('message', -10),
                                        # ('coords',-1),
                                        # ('zoom',-1),
                                        ('progress', -3),
                                        ('status', -1),
                                        # ('coords',-1),
                                        ])
        self.SetStatusBar(self.statusbar)
        # self.count=0.0

        logger = process.get_logger()
        self.oldprogressfunc = logger.get_clallbackfunc('progress')
        logger.add_callback(self.show_progress, 'progress')

        self.oldmessagefunc = logger.get_clallbackfunc('message')
        logger.add_callback(self.write_message, 'message')

        #################################################################
        # create toolbar

        # self.init_toolbar(size=size_toolbaricons)

        self.browser = self.make_browser()

        # Create a sizer to manage the Canvas and message window
        MainSizer = wx.BoxSizer(wx.VERTICAL)
        MainSizer.Add(self.browser, 4, wx.EXPAND)

        self.SetSizer(MainSizer)
        self.Bind(wx.EVT_CLOSE, self.on_close)
        #self.Bind(wx.EVT_IDLE, self.on_idle)

        self.EventsAreBound = False

        # getting all the colors for random objects
        # wxlib.colourdb.updateColourDB()
        #self.colors = wxlib.colourdb.getColourList()

        return None

    def _get_buttons(self):
        if hasattr(self.process, 'step'):
            buttons = [('Start',   self.on_start,      'Start the process!'),
                       ('Stop',   self.on_stop,        'Stop process.'),
                       ('Step',   self.on_singlestep,      'Make a single step.'),
                       ('Done',   self.on_done,        'Stop process and close window.'),
                       ]
            defaultbutton = 'Start'

        else:
            if self.is_close_after_completion:
                buttons = [('Run',   self.on_start,      'Start the process!'),
                           ]
            else:
                buttons = [('Run',   self.on_start,      'Start the process!'),
                           ('Done',   self.on_done,      'Finish with process and close window.'),
                           ]
            defaultbutton = 'Run'

        buttons.append(('Cancel',   self.on_close,        'Close window, without running the process.'))
        # standartbuttons=['cancel'] # uses also standard on_cancel :(
        standartbuttons = []
        return buttons, defaultbutton, standartbuttons

    # def on_idle(self,event):
    #    print 'on_idle'
    #
    #    if self._is_run:
    #        self.process.step()
    #        if self.process.status=='success':
    #            self._is_run = False
    #            self.Destroy()

    def on_start(self, event=None):
        self.browser.apply()
        if self.process.status != 'running':
            self.process.update_params()
            # self.refresh_browser()

            # TODO: check whether to use ALWAYS run to put process in running mode
            if hasattr(self.process, 'step'):
                self.process.do()
            else:
                self.process.run()

                # no closing function defined
                if self.is_close_after_completion:
                    self.on_close(event)

            # print 'on_start: after preparation self.process.status=',self.process.status

        if hasattr(self.process, 'step'):
            self.timer.Start(1)

        # while  self.process.status=='running':
        #    print 'call step'
        #    self.process.step()

        # if self.process.status=='running':
        #    # this will call step method in on_idle
        #    self._is_run = True
        #
        # elif self.process.status=='preparation':
        #    self.process.update_params()
        #    self.refresh_browser()
        #    self.process.run()

    def on_step(self, event=None):
        if self.process.status == 'running':
            # print 'call step'
            self.process.step()
        else:
            print '  Simulation finished'
            self.timer.Stop()

    def on_stop(self, event=None):
        self.timer.Stop()

    def on_singlestep(self, event=None):
        if self.process.status == 'running':
            if hasattr(self.process, 'step'):
                wx.FutureCall(1, self.process.step)

    def on_close(self, event=None):
        # print 'on_close',self.process.status
        if self.process.status == 'running':
            self.process.aboard()
            # print '  aboarded.'

        self.MakeModal(False)
        # print '  call func_close',self.func_close
        if self.func_close is not None:
            self.func_close(self)

        # print '  reput callbacks to main'
        if self.oldprogressfunc is not None:
            self.process.get_logger().add_callback(self.oldprogressfunc, 'progress')

        if self.oldmessagefunc is not None:
            self.process.get_logger().add_callback(self.oldmessagefunc, 'message')

        # print '  call destroy'
        self.Destroy()
        # self.Close() # no effect

    def on_done(self, event=None):
        # print 'on_done',self.process.status
        self.on_close(event)

    def on_cancel(self, event):
        """
        Apply values, destroy itself and parent
        """
        # print 'on_cancel'
        self.on_close(event)

    def on_kill(self, event=None):
        # self.process.kill()
        pass

    def show_progress(self, percent, **kwargs):
        """
        Shows progress on progress bar. Plug-in funcion for logger.
        """
        # print 'show_progress',percent
        self.statusbar.set_progress(percent)
        # self.Refresh()
        # wx.Yield()
        # self.Update()

    def write_message(self, text, **kwargs):
        print 'write_message', text
        self.statusbar.write_message(text)

    def make_browser(self):

        buttons, defaultbutton, standartbuttons = self._get_buttons()
        browser = objpanel.ObjPanel(self, self.process,
                                    attrconfigs=None,
                                    id=None, ids=None,
                                    groupnames=['options', 'inputparameters'],
                                    func_change_obj=None,
                                    show_groupnames=False, show_title=False,
                                    is_modal=True,
                                    mainframe=None,
                                    pos=wx.DefaultPosition,
                                    size=wx.DefaultSize,
                                    style=wx.MAXIMIZE_BOX | wx.RESIZE_BORDER,
                                    immediate_apply=False,  # True,
                                    panelstyle='default',
                                    buttons=buttons,
                                    standartbuttons=standartbuttons,
                                    defaultbutton=defaultbutton,
                                    )
        return browser

    def refresh_browser(self):
        """
        Deletes previous conents 
        Builds panel for obj
        Updates path window and history
        """
        # print 'Wizzard.refresh_panel with',obj.ident
        # remove previous obj panel
        sizer = self.GetSizer()
        sizer.Remove(0)
        self.browser.Destroy()
        #del self.browser
        self.browser = self.make_browser()

        sizer.Add(self.browser, 1, wx.GROW)

        self.Refresh()
        # sizer.Fit(self)
        sizer.Layout()
        # add to history
