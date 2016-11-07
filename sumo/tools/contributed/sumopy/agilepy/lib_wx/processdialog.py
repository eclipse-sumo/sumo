
import wx
import objpanel


class ProcessDialog(objpanel.ObjPanelDialog):

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
        if title == None:
            title = process.get_name()

        buttons, defaultbutton, standartbuttons = self._get_buttons()

        objpanel.ObjPanelDialog.__init__(self, parent, process,
                                         attrconfigs=None,
                                         id=None, ids=None,
                                         groupnames=['options',
                                                     'inputparameters'],
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
                   ('Apply',   self.on_apply,
                       'Apply current values, complete them and make them consistent, but do not run.'),
                   ]
        defaultbutton = 'Run'
        standartbuttons = ['cancel', ]

        return buttons, defaultbutton, standartbuttons

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

    def get_status(self):
        return self.process.status

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

        if (attrconfigs == None) & (tables in (None, [])) & (table == None):
            attrconfigs = obj.get_attrman().get_configs()
            if not self._show_title:
                # titels of groupname are not shown so just
                # select attributes at this stage
                attrconfig_new = []
                for attrconfig in attrconfigs:
                    # print '  attrconfig.attrname',attrconfig.attrname
                    is_shown = False
                    for groupname in attrconfig.groupnames:
                        # print '
                        # comp:',attrconfig.attrname,groupname,groupname in
                        # groupnames
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
