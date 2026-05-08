from coremodules.misc.matplottools import Netplotter
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from coremodules.misc.matplottools import * 
class MatsimNetplotter(Netplotter):
    def __init__(self, matsim, name= 'Plot MATSim network', 
                    info = "High quality MATSim network plot using matplotlib",  
                    logger = None, 
                    **kwargs):
        self._matsim = matsim
        net = self._matsim.get_scenario().net
        
        self._init_common('netplotter', parent = net, name = name, 
                            info = info, logger = logger)
        
        #print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()
        
        # change some defaults
        kwargs.update({ 'color_network':np.array([0.1,0.1,1.0,0.9], dtype=np.float32),
                        'color_nodes':np.array([0.1,1.0,1.0,1.0], dtype=np.float32),
                        'alpha_net':0.9,
                        'alpha_maps': 0.9,
                        'color_facilities': np.array([0.9,0.7,0.4,0.6], dtype=np.float32),
                        'color_borders': np.array([0.3,0.2,0.0,0.8], dtype=np.float32),
                        'color_zones': np.array([0.2,0.8,0.2,0.1], dtype=np.float32),
                        'alpha_zones': 0.2,
                        'color_zoneborders': np.array([0.0,0.5,0.0,0.9], dtype=np.float32),
                        'color_background': np.array([1,1,1,1], dtype=np.float32),
                        #'title':
                        })
        
        
        self.is_show_matsim_network = attrsman.add(cm.AttrConf(  'is_show_matsim_network', kwargs.get('is_show_matsim_network', True),
                                        groupnames = ['options','matsim'], 
                                        name = 'Show matsim network', 
                                        info = 'Shows a schematic matsim network in the background.',
                                        ))
                                        
        
                                        
        self.color_matsim_network = attrsman.add(cm.AttrConf(  'color_matsim_network', kwargs.get('color_matsim_network', np.array([0.7,0.2,0.7,0.9], dtype=np.float32)),
                                        groupnames = ['options','network'],
                                        perm='wr', 
                                        metatype = 'color',
                                        name = 'Matsim Network color', 
                                        info = 'Outline color of schematic Matsim network in the background.',
                                        ))
        
        
        self.alpha_matsim_network = attrsman.add(cm.AttrConf(  'alpha_matsim_network', kwargs.get('alpha_matsim_network', 0.9),
                                        groupnames = ['options','network'],
                                        perm='wr', 
                                        name = 'Matsim Network alpha', 
                                        info = 'Alpha color of schematic Matsim network in the background.',
                                        ))
                                        
        self.scale_width_edge_matsim =   attrsman.add(cm.AttrConf(  'scale_width_edge_matsim', kwargs.get('scale_width_edge_matsim', 0.9),
                                        groupnames = ['options','network'],
                                        perm='wr', 
                                        name = 'Matsim Edgewidth scale', 
                                        info = 'Scale factor for edge width.',
                                        ))
                                                                      
        self.add_networkoptions(**kwargs)                                    
        self.add_facilityoptions(**kwargs)  
        self.add_zoneoptions(**kwargs)  
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)
        self.add_save_options(**kwargs)
    
    def show(self):
        ax = init_plot()
        net = self._matsim.get_scenario().net
        self.plot_net(axis = ax)
        
        # plot MATSim net with direct call
        unit = self.unit_mapscale
        mapscale =self.get_attrsman().get_config('unit_mapscale').mapscales[unit]
        plot_net(       ax, self._matsim.net, 
                        color_edge = self.color_matsim_network, 
                        alpha = self.alpha_matsim_network, 
                        is_show_connections = False,
                        is_show_nodes = False,
                        mapscale = mapscale, 
                        scale_width_edge = self.scale_width_edge_matsim,
                        zorder = -200,
                        )
        
        x_min = np.min(self._matsim.net.nodes.coords.get_value()[:,0])*mapscale
        x_max = np.max(self._matsim.net.nodes.coords.get_value()[:,0])*mapscale
        y_min = np.min(self._matsim.net.nodes.coords.get_value()[:,1])*mapscale
        y_max = np.max(self._matsim.net.nodes.coords.get_value()[:,1])*mapscale
        
        ax.plot([x_min,x_max], [y_min,y_max], color = 'w', lw = 1, alpha=0.0 ,zorder = -10000)
        
        # now plot SUMO network
        self.plot_net(axis = ax)
        
        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('matsimnet')
        else:
            show_plot()
    




