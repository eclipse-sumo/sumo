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

# @file    objbrowser.py
# @author  Joerg Schweizer
# @date    2012

import wx
from objpanel import *
from mainframe import AgileMainframe, AgileStatusbar, AgileMenubar


class ObjBrowserMainframe(AgileMainframe):
    def __init__(self, obj=None, table=None, _id=None, parent=None,
                 title='Object browser', appname='Object browser app',
                 moduledirs=[], args=[], appdir='',
                 is_maximize=False, is_centerscreen=True,
                 pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=wx.DEFAULT_FRAME_STYLE, logger=None,
                 name='Object browser frame', size_toolbaricons=(24, 24)):

        # print 'AgileMainframe.__init__',title,appdir

        # Forcing a specific style on the window.
        #   Should this include styles passed?

        if appname is not None:
            self.appname = appname
        else:
            self.appname = title
        wx.Frame.__init__(self, parent, wx.ID_ANY, self.appname,
                          pos, size=size, style=style, name=name)

        if obj is not None:
            # print '  init ObjPanel'
            #self.browser = ObjPanel(self, obj)
            self.browser = NaviPanel(self, obj, _id)
        elif table is not None:
            # print '  init TablePanel'
            self.browser = TablePanel(self, table)
        else:
            obj = cm.BaseObjman('empty')
            self.browser = NaviPanel(self, obj, _id)

        if logger is not None:
            self._logger = logger
        else:
            self._logger = Logger()

        if is_maximize:
            self.Maximize()
        if is_centerscreen:
            self.CenterOnScreen()

        #################################################################
        # create statusbar
        self.statusbar = AgileStatusbar(self)
        self.SetStatusBar(self.statusbar)
        # self.count=0.0

        #################################################################
        # create toolbar

        self.init_toolbar(size=size_toolbaricons)
        #
        #new_bmp =  wx.ArtProvider.GetBitmap(wx.ART_NEW, wx.ART_TOOLBAR, tsize)
        #open_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_TOOLBAR, tsize)
        #save_bmp= wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE, wx.ART_TOOLBAR, tsize)
        #cut_bmp = wx.ArtProvider.GetBitmap(wx.ART_CUT, wx.ART_TOOLBAR, tsize)
        #copy_bmp = wx.ArtProvider.GetBitmap(wx.ART_COPY, wx.ART_TOOLBAR, tsize)
        #paste_bmp= wx.ArtProvider.GetBitmap(wx.ART_PASTE, wx.ART_TOOLBAR, tsize)

        #self.add_tool('new',self.on_open,new_bmp,'create new doc')
        #self.add_tool('open',self.on_open,open_bmp,'Open doc')
        #self.add_tool('save',self.on_save,save_bmp,'Save doc')
        # self.toolbar.AddSeparator()
        # self.add_tool('cut',self.on_open,cut_bmp,'Cut')
        # self.add_tool('copy',self.on_open,copy_bmp,'Copy')
        # self.add_tool('paste',self.on_open,paste_bmp,'Paste')

        # self.SetToolBar(self.toolbar)

        #################################################################
        # create the menu bar

        self.menubar = AgileMenubar(self)
        self.make_menu()
        # self.menubar.append_menu('tools')
        self.SetMenuBar(self.menubar)

        #################################################################
        # init logger
        #self._logger = Logger()
        #self._logger.add_callback(self.write_message, 'message')
        #self._logger.add_callback(self.write_action, 'action')
        #self._logger.add_callback(self.show_progress, 'progress')
        #################################################################

        #################################################################
        # event section: specify in App

        self.MsgWindow = py.shell.Shell(self)
        # Create a sizer to manage the Canvas and message window
        MainSizer = wx.BoxSizer(wx.VERTICAL)
        MainSizer.Add(self.browser, 4, wx.EXPAND)
        MainSizer.Add(self.MsgWindow, 1, wx.EXPAND | wx.ALL, 5)

        self.SetSizer(MainSizer)
        #self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        self.EventsAreBound = False

        #wx.EVT_BUTTON(self, 1003, self.on_close)
        # wx.EVT_CLOSE(self, self.on_close)
        #wx.EVT_IDLE(self, self.on_idle)
    def show_progress(self, percent, **kwargs):
        pass

    def get_args(self):
        pass

    def browse_obj(self, obj, **kwargs):
        self.browser.change_obj(obj, **kwargs)

    def get_obj(self):
        return self.browser.get_obj()

    def make_menu(self):
        # event section
        #wx.EVT_BUTTON(self._mainframe, 1003, self.on_close)
        wx.EVT_CLOSE(self, self.on_close)
        #wx.EVT_IDLE(self._mainframe, self.on_idle)

        # print 'make_menu'
        menubar = self.menubar
        menubar.append_menu('Object')

        menubar.append_item('Object/open...',
                            self.on_open,
                            info='Open a object from a Python binary file.',
                            bitmap=wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_MENU),
                            )

        menubar.append_item('Object/browse',
                            self.on_browse_obj,  # common function in modulegui
                            info='View and browse object in object panel.',
                            bitmap=wx.ArtProvider.GetBitmap(wx.ART_INFORMATION, wx.ART_MENU),
                            )

        menubar.append_item('Object/safe as',
                            self.on_save_as, shortkey='Ctrl+S',
                            info='Save current scenario in a Python binary file.',
                            bitmap=wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE, wx.ART_MENU),
                            )

        self.make_menu_specific()

    def make_menu_specific(self):
        """Here we can add App specific main menu items"""
        pass

    def on_close(self, event):
        self.destroy()

    def on_exit(self, event):
        """Close browser application"""
        dlg = wx.MessageDialog(self,
                               'Browser is about to close.\nDo you want to SAVE the current object before closing?',
                               'Closing Browser',
                               wx.YES_NO | wx.CANCEL | wx.ICON_QUESTION)
        ans = dlg.ShowModal()
        dlg.Destroy()
        # print '  ans,wx.ID_CANCEL,wx.ID_YES,wx.ID_NO',ans,wx.ID_CANCEL,wx.ID_YES,wx.ID_NO
        if ans == wx.ID_CANCEL:
            # print ' do not quit!'
            pass

        elif ans == wx.ID_YES:
            # print ' save and quit'
            self.on_save_as(event)
            self.destroy()

        elif ans == wx.ID_NO:
            # print 'quit immediately'
            self._mainframe.destroy()

    def on_open(self, event=None):
        """
        Opens object from a binary file.
        """
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "Python binary files (*.obj)|*.obj"
        wildcards = wildcards_obj+"|"+wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path chosen.
        dlg = wx.FileDialog(self, message="Open object file",
                            #defaultDir = scenario.get_workdirpath(),
                            #defaultFile = os.path.join(scenario.get_workdirpath(), scenario.format_ident()+'.obj'),
                            wildcard=wildcards,
                            style=wx.OPEN | wx.CHANGE_DIR
                            )

        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            filepath = dlg.GetPath()
            if len(filepath) > 0:
                obj = cm.load_obj(filepath)
                self.browse_obj(obj)

        # Destroy the dialog. Don't do this until you are done with it!
        dlg.Destroy()

    def on_browse_obj(self, event=None):
        """
        Browse main object.
        """
        self.browse_obj(self.get_obj())

    def on_save_as(self, event=None):
        """
        Saves current object into a binary file.
        """
        obj = self.get_obj()
        wildcards_all = "All files (*.*)|*.*"
        wildcards_obj = "Python binary files (*.obj)|*.obj"
        wildcards = wildcards_obj+"|"+wildcards_all

        # Finally, if the directory is changed in the process of getting files, this
        # dialog is set up to change the current working directory to the path chosen.
        dlg = wx.FileDialog(
            self, message="Save Object to file",
            #defaultDir = scenario.get_workdirpath(),
            #defaultFile = scenario.get_rootfilepath()+'.obj',
            wildcard=wildcards,
            style=wx.SAVE | wx.CHANGE_DIR
        )

        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            filepath = dlg.GetPath()
            if len(filepath) > 0:
                # now set new filename and workdir

                cm.save_obj(obj, filepath)

        # Destroy the dialog. Don't do this until you are done with it!
        # BAD things can happen otherwise!
        dlg.Destroy()

    def add_view(self, name, ViewClass, **args):
        pass

    def select_view(self, ind=0, name=None):
        pass

    def on_size(self, event=None):
        pass


class ObjBrowserApp(wx.App):
    """

    """

    def OnInit(self):
        wx.InitAllImageHandlers()
        #DrawFrame = BuildDrawFrame()
        #frame = ObjBrowserMainframe(None, -1, "Object browser",wx.DefaultPosition,(700,700),obj=self._obj, _id = self._id)
        frame = ObjBrowserMainframe(obj=self._obj, table=None, parent=None,
                                    title='Object browser', appname='Object browser app',
                                    logger=self._logger,
                                    appdir='', is_maximize=False, is_centerscreen=True,
                                    pos=wx.DefaultPosition, size=wx.DefaultSize,
                                    style=wx.DEFAULT_FRAME_STYLE,
                                    name='Object browser frame', size_toolbaricons=(24, 24))
        self.SetTopWindow(frame)
        frame.Show()

        return True
