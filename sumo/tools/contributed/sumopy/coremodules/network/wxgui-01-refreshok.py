import os
import wx

from agilepy.lib_wx.modulegui import ModuleGui
from agilepy.lib_wx.processdialog import ProcessDialog
import network
from network_editor import *


class WxGui(ModuleGui):
    """Contains functions that communicate between the widgets of the main wx gui
    and the functions of the plugin.
    """


    def __init__(self, ident):
        self._net = None
        self._init_common(  ident,  priority = 10,
                            icondirpath = os.path.join(os.path.dirname(__file__), 'images'))
        
    
    def get_module(self):
        return self._net
        
    def get_scenario(self):
        return self._mainframe.get_modulegui('coremodules.scenario').get_module()
    
    
    def get_neteditor(self):
        return self._neteditor
        
    def init_widgets(self, mainframe):
        """
        Set mainframe and initialize widgets to various places.
        """
        self._mainframe = mainframe
        self._neteditor = mainframe.add_view("Network", Neteditor)
        
        #mainframe.browse_obj(self._net)
        self.make_menu()
        self.make_toolbar()
        

    def refresh_widgets(self):
        """
        Check through mainframe what the state of the application is
        and reset widgets. For exampe enable/disable widgets
        dependent on the availability of data. 
        """
        
        scenario = self.get_scenario()
        print 'network refresh_widgets',self._net,scenario.net.is_modified()
        if self._net != scenario.net:
            print '  self._net != scenario.net',self._net,scenario.net
            del self._net
            self._net = scenario.net
            drawing = self._neteditor.set_net(self._net)
            if 1:
                canvas = self._neteditor.get_canvas()
                #wx.CallAfter(canvas.zoom_tofit)
            else:
                self.on_test(drawing = drawing)
            
            
            
        elif self._net.is_modified():
            print '   is_modified',self._net.is_modified()
            drawing = self._neteditor.set_net(self._net)
            
            
            if 1:
                canvas = self._neteditor.get_canvas()
                #wx.CallAfter(canvas.zoom_tofit)
            else:
                self.on_test(drawing = drawing)
        
        
            #drawing = OGLdrawing()
            #
            #draws = EdgeDrawings(self._net.edges, drawing)
            #drawing.add_drawobj(draws, layer = 10)
            #
            #draws = NodeDrawings(self._net.nodes, drawing)
            #drawing.add_drawobj(draws, layer = 20)
            # 
            #draws = LaneDrawings(self._net.lanes, drawing)
            #drawing.add_drawobj(draws, layer = 15)
            #
            #draws = ConnectionDrawings(self._net.connections, drawing)
            #drawing.add_drawobj(draws, layer = 25)
            
            
            #self._neteditor.set_drawing(drawing)
            #print '  drawing',self._neteditor.get_drawing()

    def make_menu(self):
        #print 'make_menu'
        menubar = self._mainframe.menubar
        menubar.append_menu('network', bitmap = self.get_icon('icon_graph.png'))
        menubar.append_menu( 'network/import',
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
            )
        menubar.append_item( 'network/import/sumo net.xml',
            self.on_import_sumonet, 
            info='Import scenario from sumo net xml files.',
            bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
            )
            
        menubar.append_item( 'network/refresh',
            self.on_refresh, 
            info='Refresh graph.',
            #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
            )
            
        menubar.append_item( 'network/test',
            self.on_test, 
            info='Test graph.',
            #bitmap = wx.ArtProvider.GetBitmap(wx.ART_NORMAL_FILE,wx.ART_MENU),
            )
    
    def on_refresh(self,event = None):
        print 'on_refresh neteditor',id(self._neteditor.get_drawing())
        
        #self._neteditor.set_net(self._net)
        
        canvas = self._neteditor.get_canvas()
        wx.CallAfter(canvas.zoom_tofit)
        
        if event:
            event.Skip()
        
    def on_import_sumonet(self, event = None):
        sumonetimporter = network.SumonetImporter(self._net, logger = self._mainframe.get_logger())
        dlg = ProcessDialog(self._mainframe, sumonetimporter)
                         
        dlg.CenterOnScreen()
    
        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
        #print '  val,val == wx.ID_OK',val,wx.ID_OK,wx.ID_CANCEL,val == wx.ID_CANCEL
        #print '  status =',dlg.get_status()
        if dlg.get_status() != 'success':#val == wx.ID_CANCEL:
            #print ">>>>>>>>>Unsuccessful\n"
            dlg.Destroy()
            
        if dlg.get_status() == 'success':
            #print ">>>>>>>>>successful\n"
            # apply current widget values to scenario instance
            dlg.apply()
            dlg.Destroy()
            
            #del self._scenario
            #self._scenario = scenariocreator.get_scenario()
            
            # this should update all widgets for the new scenario!!
            #print 'call self._mainframe.refresh_moduleguis()'
            self._mainframe.refresh_moduleguis()
        
        if event:
            event.Skip()
    
    def on_test2(self,drawing):
        canvas = self._neteditor.get_canvas()
        #if is_setdrawing:
        #    canvas.set_drawing(drawing)
        #    #canvas.zoom_tofit()    
        #    wx.CallAfter(canvas.zoom_tofit)
        
        #if event:
        #    event.Skip()
        
        
    def on_test(self,event=None, drawing = None):
            if drawing == None:
                is_setdrawing = True
                drawing = OGLdrawing()
            else:
                is_setdrawing = False
            
            #drawing = self._neteditor.get_drawing()
            print '\non_test',drawing,id(drawing)
            
            
            vertices = np.array([
                    [[0.0,0.0,0.0],[0.2,0.0,0.0]],# 0 green
                    [[0.0,0.0,0.0],[0.0,0.9,0.0]],# 1 red
                    ])
            
            
            colors =np.array( [
                    [0.0,0.9,0.0,0.9],    # 0
                    [0.9,0.0,0.0,0.9],    # 1
                    ])
            
            colors2 =np.array( [
                    [0.5,0.9,0.5,0.5],    # 0
                    [0.9,0.5,0.9,0.5],    # 1
                    ])
            colors2o =np.array( [
                    [0.8,0.9,0.8,0.9],    # 0
                    [0.9,0.8,0.9,0.9],    # 1
                    ])
                    
          
                
            
#-------------------------------------------------------------------------------
            
            if 0:
                lines = Lines('lines', drawing)
                lines.add_drawobjs(vertices,colors)
                drawing.add_drawobj(lines)
#-------------------------------------------------------------------------------
            if 1:
                fancylines = Fancylines('fancylines', drawing)
                vertices_fancy = np.array([
                    [[0.0,-1.0,0.0],[2,-1.0,0.0]],# 0 green
                    [[0.0,-1.0,0.0],[0.0,-5.0,0.0]],# 1 red
                    ])
                    
                widths =    [   0.5, 
                                0.3,
                                ]  
                #print '  vertices_fancy\n',vertices_fancy
                # FLATHEAD = 0
                #BEVELHEAD = 1
                #TRIANGLEHEAD = 2 
                #ARROWHEAD = 3 
                fancylines.add_drawobjs(    vertices_fancy,
                                            widths,# width
                                            colors,
                                            beginstyles = [TRIANGLEHEAD,TRIANGLEHEAD], 
                                            endstyles = [ARROWHEAD,ARROWHEAD])
                drawing.add_drawobj(fancylines)
#-------------------------------------------------------------------------------
            if 0:
                polylines = Polylines('polylines', drawing, joinstyle = BEVELHEAD)
                colors_poly =np.array( [
                    [0.0,0.8,0.5,0.9],    # 0
                    [0.8,0.0,0.5,0.9],    # 1
                    ]) 
                    
                vertices_poly = np.array([
                        [[0.0,2.0,0.0],[5.0,2.0,0.0],[5.0,7.0,0.0],[0.0,7.0,0.0]],# 0 green
                        [[0.0,-2.0,0.0],[-2.0,-2.0,0.0]],# 1 red
                        ], np.object)
                
                widths =    [   0.5, 
                                0.3,
                                ]     
                #print '  vertices_poly\n',vertices_poly
                polylines.add_drawobjs(    vertices_poly,
                                            widths,# width
                                            colors_poly,
                                            beginstyles = [ARROWHEAD,ARROWHEAD], 
                                            endstyles = [ARROWHEAD,ARROWHEAD])
                drawing.add_drawobj(polylines)
            
#-------------------------------------------------------------------------------
            if 1:
                polygons = Polygons('polygons', drawing,linewidth = 5)
                colors_poly =np.array( [
                        [0.0,0.9,0.9,0.9],    # 0
                        [0.8,0.2,0.2,0.9],    # 1
                        ]) 
                    
                vertices_poly = np.array([
                        [[0.0,2.0,0.0],[5.0,2.0,0.0],[5.0,7.0,0.0],[0.0,7.0,0.0]],# 0 green
                        [[0.0,-2.0,0.0],[-2.0,-2.0,0.0],[-2.0,0.0,0.0]],# 1 red
                        ], np.object) 
                        
                print '  vertices_polygon\n',vertices_poly
                polygons.add_drawobjs(      vertices_poly,
                                            colors_poly)
                drawing.add_drawobj(polygons)
            
            canvas = self._neteditor.get_canvas()
            if is_setdrawing:
                canvas.set_drawing(drawing)
                #canvas.zoom_tofit()    
                wx.CallAfter(canvas.zoom_tofit)
            
            if event:
                event.Skip()
        
        
        
    
    


