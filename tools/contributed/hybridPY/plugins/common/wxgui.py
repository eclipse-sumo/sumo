import os

from agilepy.lib_wx.modulegui import ModuleGui


       
            
class WxGui(    ModuleGui):
    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """


    def __init__(self, ident):
        self._init_common(  ident,  priority = 100000,
                            icondirpath = os.path.join(os.path.dirname(__file__), 'images'))
        
    def get_module(self):
        return None
        
    
    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        #self._neteditor = mainframe.add_view("Network", Neteditor)
        
        #mainframe.browse_obj(self._module)
        self.make_menu()
        self.make_toolbar()
        

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        pass

    def make_menu(self):
        menubar = self._mainframe.menubar
        menubar.append_menu('plugins')
        
        #menubar.append_item( 'demand/browse',
        #    self.on_browse_obj, # common function in modulegui
        #    info='View and browse demand in object panel.',
        #    bitmap = self.get_agileicon('icon_browse_24px.png'),#,
        #    )
        
