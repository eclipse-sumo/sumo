import os 
##import math
import numpy as np            

import  matplotlib.pyplot as plt


import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.processes import Process

from coremodules.misc.matplottools import *

def detect_green_intervals(green_array):
    """Returns a list of (start_time, end_time) in seconds for each green interval."""
    intervals = []
    in_green = False
    start = None
    for i, val in enumerate(green_array):
        if val and not in_green:
            in_green = True
            start = i
        elif not val and in_green:
            in_green = False
            intervals.append((start, i - 1))
    if in_green:
        intervals.append((start, len(green_array) - 1))
    return intervals
    
class LegresultsPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name= 'Legresults Plotter', 
                    info = "Plots leg results over time using matplotlib",  
                    logger = None, **kwargs):
        
        self._init_common('legresultsplotter', parent = results, name = name, 
                            info = info, logger = logger)
        
        #print 'LegresultsPlotter.__init__',results,self.parent
        attrsman = self.get_attrsman()
        
        self.ids_leg = attrsman.add(cm.ListConf('ids_leg',kwargs.get('ids_leg',[1,]), 
                                            groupnames = ['options'], 
                                            name = 'Leg IDs', 
                                            info = """Plot these leg IDs, all leg IDs are plotted if left empty.""",
                                            ))
        
        self.add_plotoptions_lineplot(is_no_markers=False,**kwargs)
        self.add_save_options(**kwargs)
    
    def get_scenario(self):
        return self.parent.get_scenario()                              
                    
    def show(self):
        print('show legresultsplotter')
        #self.init_figures()
        #fig = self.create_figure()
        #ax1 = fig.add_subplot(111)
        legres = self.parent.legresults
        window_size = 60
        print('  self.ids_leg',self.ids_leg)
        if len(self.ids_leg) > 0:
            ids_leg = self.ids_leg
        else:
            ids_leg = legres.get_ids()
            
        for leg_id in ids_leg:
            queue_list = np.array(legres.lengths_queue[leg_id])
            green_list = np.array(legres.are_greenphase[leg_id])  
            flow_list = np.array(legres.detecteds_out[leg_id])     
            n = min(len(queue_list), len(green_list), len(flow_list))
            n_trimmed = n - (n % window_size)
        
            queue_list = queue_list[:n_trimmed]
            green_list = green_list[:n_trimmed]
            flow_list = flow_list[:n_trimmed]
        
            # Aggregate per minute
            queue_matrix = queue_list.reshape(-1, window_size)
            flow_matrix = flow_list.reshape(-1, window_size)
            avg_queue_per_minute = np.mean(queue_matrix, axis=1)
            flow_sum_per_minute = np.sum(flow_matrix, axis=1)
        
            # Detect green intervals from full green array
            green_intervals = detect_green_intervals(green_list)
        
            # Plot
            minutes = np.arange(len(avg_queue_per_minute))
            fig, axs = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
        
            axs[0].plot(minutes, avg_queue_per_minute,
                        color=self.color_line, 
                        linestyle = self.style_line,
                        linewidth = self.width_line,
                        alpha = self.alpha_line,
                        marker = self.style_marker,
                        markerfacecolor =self.color_marker,
                        markersize = self.size_marker,)
                        
            axs[0].set_ylabel("Avg Queue Length", fontsize=self.size_labelfont)
            axs[0].tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
            axs[0].grid(self.is_grid)
            if self.is_show_title:
                if self.title != '':
                    title = self.title
                else:
                    title = f"Leg {leg_id} Traffic Metrics"
                axs[0].set_title(title,fontsize=self.size_titlefont)
            
            axs[1].set_ylabel('Vehicles per min', fontsize=self.size_labelfont)
            axs[1].tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
            axs[1].plot(minutes, flow_sum_per_minute, 
                        color=self.color_line, 
                        linestyle = self.style_line,
                        linewidth = self.width_line,
                        alpha = self.alpha_line,
                        marker = self.style_marker,
                        markerfacecolor =self.color_marker,
                        markersize = self.size_marker,
                        )
            axs[1].grid(self.is_grid)
            
            axs[2].plot(minutes, avg_queue_per_minute, label="Queue %d"%leg_id, 
                        color=self.color_line, 
                        linestyle = self.style_line,
                        linewidth = self.width_line,
                        alpha = self.alpha_line,
                        marker = self.style_marker,
                        markerfacecolor =self.color_marker,
                        markersize = self.size_marker,
                        )
            axs[2].set_ylabel("Queue & Green", fontsize=self.size_labelfont)
            axs[2].tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
            axs[2].grid(self.is_grid)
            
            axs[2].set_xlabel("Time [min]", fontsize=self.size_labelfont)
            axs[2].tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
            
            for start_sec, end_sec in green_intervals:
                axs[2].axvspan(start_sec / 60, end_sec / 60, color='green', alpha=0.3)
        
            axs[2].legend()
            plt.tight_layout()
            
            
        
            if self.is_save:
                self.save_fig(figname = '_leg%03d'%leg_id)
            
        if self.is_show:
            print('  show_plot')
            show_plot()   
        else:
            plt.close('all') 
            
                    
            
        
      
        
