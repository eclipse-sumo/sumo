import wx
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am

from agilepy.lib_wx.objpanel import ObjPanel
from coremodules.network.network import SumoIdsConf, MODES

#def make_menu(menubar):
    

class AddOdWizzard(am.ArrayObjman):
    """Contains information and methods to add an od matrix for 
    a certain mode and for a certain time interval to the scenario.
    """
    # j.s
    def __init__(self, odintervals):
        #print 'AddOdWizzard',odintervals#,odintervals.times_start
        #print ' ',dir(odintervals)
        zones = odintervals.get_zones()
        self._init_objman(  'odm_adder', parent =odintervals,
                                name = 'ODM Wizzard',
                                info = 'Wizzard to add origin zone to destination zone demand informations.',
                                )
                                
        self.add(am.AttrConf(  't_start', 0,
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    name = 'Start time', 
                                    unit = 's',
                                    info = 'Start time of interval',
                                    ))
            
        self.add(am.AttrConf('t_end', 3600,
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    name = 'End time', 
                                    unit = 's',
                                    info = 'End time of interval',
                                    ))
        
        # here we ged classes not vehicle type
        # specific vehicle type within a class will be generated later 
        self.add(am.AttrConf('id_mode',   MODES['private'], 
                                            groupnames = ['state'], 
                                            choices = MODES,
                                            name = 'ID mode', 
                                            info = 'ID of transport mode.',
                                            ))

        self.add(cm.AttrConf('scale', 1.0,
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    name = 'Scale',
                                    info = 'Scale demand by this factor before adding. Value od 1.0 means no scaling.' 
                                    ))
        
        
                                                                
        
        self.add_col(am.ArrayConf( 'names_orig', '', 
                                    dtype = 'object',
                                    groupnames = ['state'], 
                                    choices = list(zones.ids_sumo.get_value()),
                                    name = 'Orig zone', 
                                    info = 'Name of traffic assignment zone of origin of trip.',
                                    ))


        self.add_col(am.ArrayConf( 'names_dest', '',
                                dtype = 'object',
                                groupnames = ['state'], 
                                choices = list(zones.ids_sumo.get_value()),
                                name = 'Dest zone', 
                                info = 'Name of traffic assignment zone of destination of trip.',
                                ))
                                
        self.add_col(am.ArrayConf( 'tripnumbers', 0,
                                groupnames = ['state'], 
                                perm = 'rw', 
                                name = 'Trips', 
                                info = 'Number of trips from zone with ID Orig to zone with ID Dest.',
                                xmltag = 'tripnumber',
                                ))
        
        self.add(cm.FuncConf(  'func_make_row','on_add_row', None,
                                groupnames = ['rowfunctions','_private'], 
                                name = 'New row', 
                                info = 'Add a new row.',
                                ))
                                
        self.add(cm.FuncConf(  'func_delete_row','on_del_row', None,
                                groupnames = ['rowfunctions','_private'], 
                                name = 'Del row', 
                                #info = 'Delete a row.',
                                )) 
                                
                                
        #self.attrs.print_attrs()
    
    def on_del_row(self, id_row):
            """
            True if position greater than thhreshold.
            """
            #print 'on_del_row', id_row
            if id_row!=None:
                self.del_row(id_row) 
    
    def on_add_row(self, id=None):
        if len(self)>0:
            # copy previous
            od_last = self.get_row(self.get_ids()[-1])
            #id_orig = self.odtab.ids_orig.get(id_last)
            #id_dest = self.odtab.ids_dest.get(id_last)
            #id = self.suggest_id()
            self.add_row(**od_last)
        else:
            # create empty
            self.add_row()
    
        
    
    def add_demand(self):
        """
        Add demand to scenario.
        """
        print 'AddOdm.add_demand'
        odintervals = self.parent
        #demand = self._scenario.demand
        #odm={} # create a temporary dict with (o,d) as key and trips as value
        ids = self.get_ids()
        self.parent.add_od_trips(   self.t_start.value, self.t_end.value, 
                                    self.id_mode.value, self.scale.value, 
                                    self.names_orig[ids],self.names_dest[ids],self.tripnumbers[ids]
                                    )
        

    
    def import_csv(self, filepath, sep = ",", n_firstline = 1):
        f=open(filepath,'r')
        self.attrs.print_attrs()
        #print '  open',filepath
        i_line = n_firstline
        for line in f.readlines():
            #print '    ',line,
            cols = line.split(sep)
            if len(cols)==3:
                names_orig,names_dest,tripnumbers_str=cols
                id_new = self.suggest_id()
                
                self.odtab.set_row( id_new,
                                    names_orig = names_orig,
                                    names_dest = name_dest,
                                    tripnumbers = int(tripnumbers_str)
                                    )
                
                
            else:
                print 'WARNING: inconsistent o,d,trips info in line %d, file %s'%(i_line,filepath)
            i_line +=1
        #self.odtab.print_rows()  
        f.close()




class AddOdDialog(wx.Frame):

    """
    A frame used for the ObjBrowser Demo

    """


    def __init__(self,parent, odintervals):
        wx.Frame.__init__(self,parent, -1,title='Add OD Wizzard',pos=wx.DefaultPosition, size=wx.DefaultSize)
        self.wizzard = AddOdWizzard(odintervals)
        self.parent = parent
        ## Set up the MenuBar
        MenuBar = wx.MenuBar()

        file_menu = wx.Menu()
        item = file_menu.Append(-1, "&Import CSV...","Import OD data from a CSV text file with format <ID origin>, <ID destination>,<number of trips>")
        self.Bind(wx.EVT_MENU, self.on_import_csv, item)
        #item = file_menu.Append(-1, "&Import Exel...","Import OD data from an Exel file.")
        #self.Bind(wx.EVT_MENU, self.on_import_exel, item)
        
        item = file_menu.Append(-1, "&Add and Close","Add demand to scenario and close wizzard")
        self.Bind(wx.EVT_MENU, self.on_add, item)
        
        item = file_menu.Append(-1, "&Close","Close wizzard")
        self.Bind(wx.EVT_MENU, self.on_close, item)

        MenuBar.Append(file_menu, "&File")
        
        if odintervals.get_net().parent != None:
            self.dirpath = odintervals.get_net().parent.workdirpath
        else:
            self.dirpath = os.getcwd()

        #help_menu = wx.Menu()
        #item = help_menu.Append(-1, "&About",
        #                        "More information About this program")
        #self.Bind(wx.EVT_MENU, self.on_menu, item)
        #MenuBar.Append(help_menu, "&Help")

        self.SetMenuBar(MenuBar)

        self.CreateStatusBar()

        
        
        
        self.browser = self.make_browser()
           
        ##Create a sizer to manage the Canvas and message window
        MainSizer = wx.BoxSizer(wx.VERTICAL)
        MainSizer.Add(self.browser, 4, wx.EXPAND)
        

        self.SetSizer(MainSizer)
        self.Bind(wx.EVT_CLOSE, self.on_close)

        self.EventsAreBound = False

        ## getting all the colors for random objects
        #wxlib.colourdb.updateColourDB()
        #self.colors = wxlib.colourdb.getColourList()


        return None
    
    def make_browser(self):
        # Create Browser widget here
        buttons=[('Add ODM', self.on_add, 'Add ODM data to current scenario.'),
                 ('Cancel', self.on_close, 'Close wizzard without adding ODM.'),
                 #('OK', self.on_add_close, 'Add ODM data to current scenario and close wizzard.'),
                 ]
        defaultbuttontext='Add ODM'
        #standartbuttons=['cancel','apply','ok']# apply does not show
        standartbuttons=['apply']
        
        browser = ObjPanel(self, self.wizzard,
                                            attrconfigs=None, 
                                            id=None, ids=None,
                                            groupnames = None,
                                            func_change_obj=None, 
                                            show_groupnames=False, show_title=False, 
                                            is_modal=True,
                                            mainframe=None, 
                                            pos=wx.DefaultPosition, 
                                            size=wx.DefaultSize, 
                                            style = wx.MAXIMIZE_BOX|wx.RESIZE_BORDER,
                                            immediate_apply=False,#True, 
                                            panelstyle='default', 
                                            buttons = buttons,
                                            standartbuttons = standartbuttons,
                                            defaultbutton = defaultbuttontext,
                                            )
        return browser
    
    def on_import_csv(self, event=None):
        #print 'on_import_csv'
        wizzard = self.browser.obj 
        
        wildcards_all = "All files (*.*)|*.*|CSV files (*.csv)|*.csv"#+"|"+otherwildcards
        dlg = wx.FileDialog(self.parent, message="Import CSV file",
                            defaultDir = self.dirpath, 
                            defaultFile="",
                            wildcard = wildcards_all,
                            style=wx.OPEN | wx.MULTIPLE | wx.CHANGE_DIR
                            )
        
        # Show the dialog and retrieve the user response. If it is the OK response, 
        # process the data.
        if dlg.ShowModal() == wx.ID_OK:
            # This returns a Python list of files that were selected.
            paths = dlg.GetPaths()
            #print 'You selected %d files:' % len(paths)
            if len(paths)>0:
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
        #self.SetFocus()
        #self.MakeModal(False)
        #self.MakeModal(True)
        #self.browser.restore()
    
    def on_add(self,event = None ):
        #print 'on_add,AddOdm',self.browser.obj 
        self.browser.apply()# important to transfer widget values to obj!
        wizzard = self.browser.obj 
        wizzard.add_demand()
        
        self.on_close(event)
        
    def on_close(self, event=None):
        self.MakeModal(False)
        self.Destroy()
        pass
        
    def on_add_close(self,event = None ):
        self.on_add()
        self.on_close()
    
    
                                                
    def refresh_browser(self):
        """
        Deletes previous conents 
        Builds panel for obj
        Updates path window and history
        """
        #print 'Wizzard.refresh_panel with',obj.ident
        # remove previous obj panel
        sizer=self.GetSizer()
        sizer.Remove(0)
        self.browser.Destroy()
        #del self.browser
        self.browser = self.make_browser()
        
        sizer.Add(self.browser,1,wx.GROW)
        
        self.Refresh()
        #sizer.Fit(self)
        sizer.Layout()
        # add to history
    