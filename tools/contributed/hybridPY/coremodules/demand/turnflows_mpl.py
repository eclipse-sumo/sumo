import os 
from collections import OrderedDict
import numpy as np    
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import  matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patche
from coremodules import demand
#from coremodules.misc.matplottools import *
from coremodules.simulation.results_mpl import XYEdgeresultsPlotter
import agilepy.lib_base.xmlman as xm

from agilepy.lib_base.processes import Process

class TurnflowComparisonPlotter(XYEdgeresultsPlotter):
    def _init_edgeattrs(self, edgeresults, **kwargs):
        print('TurnflowComparisonPlotter._init_edgeattrs')
        # edgeresultes....
        attrsman = self.get_attrsman()
        attrnames_edgeresults = OrderedDict()
        edgeresultattrconfigs = edgeresults.get_group_attrs('results')
        edgeresultattrnames = list(edgeresultattrconfigs.keys())
        #edgeresultattrnames.sort()
        for attrname in edgeresultattrnames:
            attrconfig = edgeresultattrconfigs[attrname]
            
            attrnames_edgeresults[attrconfig.format_symbol()] = attrconfig.attrname
        
        #attrnames_edgeresults = {'Entered':'entered'}
        self.edgeattrname_x = attrsman.add(cm.AttrConf(  'edgeattrname_x', 'entered_est',
                                        choices = attrnames_edgeresults,
                                        groupnames = ['options','_private'], 
                                        name = 'Edge attribute X', 
                                        info = 'The edge related attribute to be plotted on x-axis.',
                                        ))
        
        self.edgeattrname_y = attrsman.add(cm.AttrConf(  'edgeattrname_y', 'entered',
                                        groupnames = ['options','_private'], 
                                        name = 'Edge attribute Y', 
                                        info = 'The edge related attribute to be plotted on y-axis.',
                                        ))
 
        self.is_show_ids_edge = attrsman.add(cm.AttrConf(  'is_show_ids_edge', kwargs.get('is_show_ids_edge',True),
                                        groupnames = ['options',], 
                                        name = 'Show edge IDs', 
                                        info = 'Show edge IDs.',
                                        ))
        
        self.counts_diff_err = attrsman.add(cm.AttrConf(  'counts_diff_err', kwargs.get('counts_diff_err',100),
                                        groupnames = ['options',], 
                                        name = 'Count difference outlayer', 
                                        info = 'Differente in counts to identify an outlayer. Outlayers will be displayed.',
                                        ))
        
        
    
    
        #print '  is_show_ids_edge',self.is_show_ids_edge


    def plot_xy(self, ax, x_all, y_enter, resultattrconf_x, resultattrconf_y, title = None):
        print('plot_xy',resultattrconf_y.attrname,'vs',resultattrconf_x.attrname,'len=', len(x_all),len(y_enter),self.is_show_ids_edge)
        y_left = np.array(getattr(self.parent.edgeresults, 'left').get_value(), dtype = np.float64)
        
        eps = 10.0**-10
        #ids_res = self.get_ids()
        n = len(x_all)
        y_enter.resize(1,n)
        y_left.resize(1,n)
        y_all = np.max(np.concatenate((y_enter, y_left), axis = 0), axis = 0)
        
        inds = np.flatnonzero((x_all > eps) & (y_all > eps) )
        
        if (len(inds)<2): return False
        
        x = x_all[inds]
        y = y_all[inds] 
        #print '  x',len(x),'v',x
        #print '  y',len(y),'v',y
        ax.plot(x, y,
                #label = resultattrconf_x.attrname+'',
                linestyle='None', 
                marker = self.style_marker, 
                markersize = self.size_marker,
                markerfacecolor =  self.color_marker,
                #alpha = self.alpha_line
                )
        
        val_max = max(np.max(x),np.max(y))   
        print('  val_max',val_max)   
        points_unity = np.array([0.0, val_max], dtype = np.float64)
        ax.plot(points_unity, points_unity,
                    '-.',
                    linewidth = self.width_line,
                    color = 'black', 
                    alpha = self.alpha_line
                    )
                    
        
                    
        if self.is_show_ids_edge:
            inds_diff = np.flatnonzero((np.abs(x_all-y_all) > self.counts_diff_err) & ((x_all > eps) & (y_all > eps)))
            edges = scenario = self.get_scenario().net.edges
            #print '  inds_diff',inds_diff
            #print '  inds',inds
            ids_edge_err = self.parent.edgeresults.ids_edge.get_value()[inds_diff]
            #print '  ids_edge_err',self.parent.edgeresults.ids_edge.get_value()[inds_diff]
            
            ax.plot([   self.counts_diff_err,val_max], 
                    [   0.0                 ,val_max-self.counts_diff_err],
                    '--',
                    linewidth = self.width_line,
                    color = 'gray', 
                    alpha = 0.5*self.alpha_line
                    )
                    
            ax.plot([   0.0                 ,val_max], 
                    [   self.counts_diff_err,val_max+self.counts_diff_err],
                    '--',
                    linewidth = self.width_line,
                    color = 'gray', 
                    alpha = 0.5*self.alpha_line
                    )
                    
            for id_edge,id_edge_sumo, x_edge, y_edge , dist_err in zip(ids_edge_err,edges.ids_sumo[ids_edge_err], x_all[inds_diff], y_all[inds_diff], np.abs(x_all[inds_diff]-y_all[inds_diff])):
                ax.text(x_edge, y_edge, str('  '+id_edge_sumo),horizontalalignment='left',fontsize = self.size_labelfont)
                print('  err=%03d, id_edge=%05d, id_edge_sumo=%s'%(dist_err, id_edge, id_edge_sumo))
        
        if self.is_linreg:
            mx = np.mean(x)
            my = np.mean(y)
            sxx = np.sum((x-mx)**2)
            syy = np.sum((y-my)**2)
            sxy = np.sum((x-mx)*(y-my))
            R2 = sxy**2/(sxx*syy)         
            m = (len(inds) * np.sum(x*y) - np.sum(x) * np.sum(y)) / (len(x)*np.sum(x*x) - np.sum(x) ** 2) 
            offset = my - (m*mx)
            
            x_linreg = np.array([np.min(x), np.max(x)], dtype = np.float64)
            print(' m',m,'offset',offset)
            #print ' x_linreg',offset * x_linreg
            if self.is_linreg_label:
                label = r'$%.3f+%.3fx$, $R^2=%.3f$'%(offset,m,R2)
            else:
                label = r'$R^2=%.3f$'%R2
            ax.plot(x_linreg, offset + m*x_linreg,
                    label = label,
                    linewidth = self.width_line,
                    linestyle= self.style_line, 
                    color = self.color_line, 
                    alpha = self.alpha_line
                    )
            #if self.is_linreg_label:            
            ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
        
        ax.grid(self.is_grid)
        if self.is_show_title:
            if title is None:
                if self.title != '':
                    title = self.title
                else:
                    title = 'Simulated counts'+' vs. '+'Measured counts'
            ax.set_title(title, fontsize=self.size_titlefont)
        
            #if subtitle is not None:
            #    NO! ax.set_subtitle(subtitle, fontsize=0.8*self.size_titlefont)
        
        if self.xlabel != '':
            xlabel = self.xlabel
        else:
            xlabel = 'Measured counts'
        
        if self.ylabel != '':
            ylabel = self.ylabel
        else:
            ylabel = 'Simulated counts'
            
                
        ax.set_xlabel(xlabel, fontsize=self.size_labelfont)
        ax.set_ylabel(ylabel, fontsize=self.size_labelfont)
        
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

    
