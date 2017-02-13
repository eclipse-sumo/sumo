import wx
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am


from agilepy.lib_wx.objpanel import ObjPanel
from coremodules.network.network import SumoIdsConf, MODES

# def make_menu(menubar):


class TurnflowImporter(cm.BaseObjman):

    """Contains information and methods to import.
    """

    def __init__(self, turnflowintervals):
        # print 'AddOdWizzard',odintervals#,odintervals.times_start
        # print ' ',dir(odintervals)
        edges = turnflowintervals.get_edges()
        self._init_objman('turnflowimporter', parent=turnflowintervals,
                          name='Turnflow Wizzard',
                          info='Wizzard to add origin zone to destination zone demand informations.',
                          )
        attrsman = self.set_attrsman(cm.Attrsman(self))

        attrsman.add(am.AttrConf('t_start', 0,
                                 groupnames=['state'],
                                 perm='rw',
                                 name='Start time',
                                 unit='s',
                                 info='Start time of interval',
                                 ))

        attrsman.add(am.AttrConf('t_end', 3600,
                                 groupnames=['state'],
                                 perm='rw',
                                 name='End time',
                                 unit='s',
                                 info='End time of interval',
                                 ))

        # here we ged classes not vehicle type
        # specific vehicle type within a class will be generated later
        attrsman.add(am.AttrConf('id_mode',   MODES['private'],
                                 groupnames=['state'],
                                 choices=MODES,
                                 name='ID mode',
                                 info='ID of transport mode.',
                                 ))

    def add_demand(self):
        """
        Add demand to scenario.
        """
        print 'AddOdm.add_demand'
        odintervals = self.parent
        #demand = self._scenario.demand
        # odm={} # create a temporary dict with (o,d) as key and trips as value
        ids = self.get_ids()
        self.parent.add_od_trips(self.t_start.value, self.t_end.value,
                                 self.id_mode.value, self.scale.value,
                                 self.names_orig[ids], self.names_dest[
                                     ids], self.tripnumbers[ids]
                                 )


class AddTurnflowDialog(wx.Frame):

    """
    A frame used for the ObjBrowser Demo

    """

    def __init__(self, parent, odintervals):
        wx.Frame.__init__(self, parent, -1, title='Add OD Wizzard',
                          pos=wx.DefaultPosition, size=wx.DefaultSize)
        self.wizzard = AddOdWizzard(odintervals)
        self.parent = parent
        # Set up the MenuBar
        MenuBar = wx.MenuBar()

        file_menu = wx.Menu()
        item = file_menu.Append(-1, "&Import CSV...",
                                "Import OD data from a CSV text file with format <ID origin>, <ID destination>,<number of trips>")
        self.Bind(wx.EVT_MENU, self.on_import_csv, item)
        #item = file_menu.Append(-1, "&Import Exel...","Import OD data from an Exel file.")
        #self.Bind(wx.EVT_MENU, self.on_import_exel, item)

        item = file_menu.Append(-1, "&Add and Close",
                                "Add demand to scenario and close wizzard")
        self.Bind(wx.EVT_MENU, self.on_add, item)

        item = file_menu.Append(-1, "&Close", "Close wizzard")
        self.Bind(wx.EVT_MENU, self.on_close, item)

        MenuBar.Append(file_menu, "&File")

        if odintervals.get_net().parent != None:
            self.dirpath = odintervals.get_net().parent.workdirpath
        else:
            self.dirpath = os.getcwd()

        #help_menu = wx.Menu()
        # item = help_menu.Append(-1, "&About",
        #                        "More information About this program")
        #self.Bind(wx.EVT_MENU, self.on_menu, item)
        #MenuBar.Append(help_menu, "&Help")

        self.SetMenuBar(MenuBar)

        self.CreateStatusBar()

        self.browser = self.make_browser()

        # Create a sizer to manage the Canvas and message window
        MainSizer = wx.BoxSizer(wx.VERTICAL)
        MainSizer.Add(self.browser, 4, wx.EXPAND)

        self.SetSizer(MainSizer)
        self.Bind(wx.EVT_CLOSE, self.on_close)

        self.EventsAreBound = False

        # getting all the colors for random objects
        # wxlib.colourdb.updateColourDB()
        #self.colors = wxlib.colourdb.getColourList()

        return None

    def make_browser(self):
        # Create Browser widget here
        buttons = [('Add ODM', self.on_add, 'Add ODM data to current scenario.'),
                   ('Cancel', self.on_close,
                    'Close wizzard without adding ODM.'),
                   #('OK', self.on_add_close, 'Add ODM data to current scenario and close wizzard.'),
                   ]
        defaultbuttontext = 'Add ODM'
        # standartbuttons=['cancel','apply','ok']# apply does not show
        standartbuttons = ['apply']

        browser = ObjPanel(self, self.wizzard,
                           attrconfigs=None,
                           id=None, ids=None,
                           groupnames=None,
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
                           defaultbutton=defaultbuttontext,
                           )
        return browser

    def on_import_csv(self, event=None):
        # print 'on_import_csv'
        wizzard = self.browser.obj

        # +"|"+otherwildcards
        wildcards_all = "All files (*.*)|*.*|CSV files (*.csv)|*.csv"
        dlg = wx.FileDialog(self.parent, message="Import CSV file",
                            defaultDir=self.dirpath,
                            defaultFile="",
                            wildcard=wildcards_all,
                            style=wx.OPEN | wx.MULTIPLE | wx.CHANGE_DIR
                            )

        # Show the dialog and retrieve the user response. If it is the OK response,
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            paths = dlg.GetPaths()
            # print 'You selected %d files:' % len(paths)
            if len(paths) > 0:
                filepath = paths[0]
            else:
                filepath = ''
            dlg.Destroy()
        else:
            return
        ###
        wizzard.import_csv(filepath)
        ##

        self.refresh_browser()
        event.Skip()
        self.Raise()
        # self.SetFocus()
        # self.MakeModal(False)
        # self.MakeModal(True)
        # self.browser.restore()

    def on_add(self, event=None):
        # print 'on_add,AddOdm',self.browser.obj
        self.browser.apply()  # important to transfer widget values to obj!
        wizzard = self.browser.obj
        wizzard.add_demand()

        self.on_close(event)

    def on_close(self, event=None):
        self.MakeModal(False)
        self.Destroy()
        pass

    def on_add_close(self, event=None):
        self.on_add()
        self.on_close()

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
