import os 
import numpy as np    
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import  matplotlib.pyplot as plt
from coremodules import demand
from coremodules.demand import detectorflows
#from coremodules.demand.origin_to_destination import OdIntervals
from coremodules.misc.matplottools import *
import agilepy.lib_base.xmlman as xm

from agilepy.lib_base.processes import Process


 
class DetectorsPlots(PlotoptionsMixin, Process):
    def __init__(self, ident, demand, logger = None, **kwargs):
        print('DetectorsPlots.__init__')
        self._init_common(  ident, 
                            parent = demand,
                            name = 'Detectors plots', 
                            logger = logger,
                            info ='Plot of detectors data.',
                            )      
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        
        self.is_plot_measured_flows = attrsman.add(cm.AttrConf( 'is_plot_measured_flows', kwargs.get('is_plot_measured_flows', True),
                                        groupnames = ['options'], 
                                        name = 'Plot detected flows', 
                                        info = 'Plot measured flows from detector data.',
                                        ))
        self.color_measured_flows = attrsman.add(cm.AttrConf(  'color_measured_flows', kwargs.get('color_measured_flows',np.array([0,0,1,1], dtype=np.float32) ),
                                            groupnames = ['options'],
                                            perm='wr', 
                                            metatype = 'color',
                                            name = 'Color of detected flows', 
                                            info = 'Color of detected flows.',
                                            ))
                                            
        self.is_plot_estimated_trip_flows = attrsman.add(cm.AttrConf( 'is_plot_estimated_trip_flows', kwargs.get('is_plot_estimated_trip_flows', True),
                                        groupnames = ['options'], 
                                        name = 'Plot estimated trip flows', 
                                        info = 'Plot estimated trip flows from trips and stages data.',
                                        ))
                                        
        self.color_estimated_trip_flows = attrsman.add(cm.AttrConf(  'color_estimated_trip_flows', kwargs.get('color_estimated_trip_flows',np.array([0.8,0.0,0.8,1], dtype=np.float32) ),
                                            groupnames = ['options'],
                                            perm='wr', 
                                            metatype = 'color',
                                            name = 'Color of  estimated trip flows', 
                                            info = 'Color of  estimated trip flows.',
                                            ))
                                            
        self.is_plot_estimated_vp_flows = attrsman.add(cm.AttrConf( 'is_plot_estimated_vp_flows', kwargs.get('is_plot_estimated_vp_flows', True),
                                        groupnames = ['options'], 
                                        name = 'Plot estimated vp flows', 
                                        info = 'Plot estimated trip flows from trips and stages data.',
                                        ))
        
        self.color_estimated_vp_flows = attrsman.add(cm.AttrConf(  'color_estimated_vp_flows', kwargs.get('color_estimated_vp_flows',np.array([0,1,0,1], dtype=np.float32) ),
                                            groupnames = ['options'],
                                            perm='wr', 
                                            metatype = 'color',
                                            name = 'Color of  estimated VP flows', 
                                            info = 'Color of  estimated virtual population flows.',
                                            ))
                                            
        self.is_plot_simulated_flows = attrsman.add(cm.AttrConf( 'is_plot_simulated_flows', kwargs.get('is_plot_simulated_flows', True),
                                        groupnames = ['options'], 
                                        name = 'Plot simulated flows', 
                                        info = 'Plot simulated flows from edgeresults.',
                                        ))
                                        
        self.color_simulated_flows = attrsman.add(cm.AttrConf(  'color_simulated_flows', kwargs.get('color_simulated_flows',np.array([0.5,0.8,0.8,1], dtype=np.float32) ),
                                            groupnames = ['options'],
                                            perm='wr', 
                                            metatype = 'color',
                                            name = 'Color of  simulated flows', 
                                            info = 'Color of  simulated flows from edgeresults.',
                                            ))
                                                                            
        self.av_dtd_time_trips = attrsman.add(cm.AttrConf( 'av_dtd_time_trips',kwargs.get('av_dtd_time_trips',1800.),
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    unit = 's',
                                    name = 'Av travel time trips', 
                                    info = 'Average door-to-door travel time for demand.trips used while updating departure time from detector flows, indipendently by the mean of transportation.',
                                    ))
        self.av_dtd_time_vp = attrsman.add(cm.AttrConf( 'av_dtd_time_vp',kwargs.get('av_dtd_time_vp',1800.),
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    unit = 's',
                                    name = 'Av travel time VP', 
                                    info = 'Average door-to-door travel time for vp used while updating departure time from detector flows, indipendently by the mean of transportation.',
                                    ))
        
        kwargs['is_grid'] =  True
        self.add_plotoptions_lineplot(is_no_markers = True, **kwargs)
        self.add_save_options(**kwargs)
        
    def do(self):
        return self.show()
        
    def show(self):
        print('DetectorsPlots.show')
                
        ax1,ind_fig = next_plot(ind_fig = 0)
        ax2,ind_fig = next_plot(ind_fig)
               
        demand = self.parent
        simulation = demand.parent.simulation
        trips = demand.trips
        ids_trip = trips.get_ids()
        virtualpop = demand.virtualpop
        ids_vp = virtualpop.get_ids()
        detectorflows = demand.detectorflows
        detector_flows = detectorflows.flowmeasurements
        ids_detflow = detector_flows.get_ids()
        det_hours = detector_flows.timeperiods[ids_detflow]
        det_flows = detector_flows.flows_passenger[ids_detflow] + detector_flows.flows_heavyduty[ids_detflow]
        step_hours = np.sort(np.unique(det_hours))
        interval_time = step_hours[1]-step_hours[0]
        step_hours+=interval_time/2.
        det_hours+=interval_time/2.
        
        bins = (step_hours/3600.-interval_time/7200.).tolist()
        bins.append(step_hours[-1]/3600.+interval_time/7200.)
        
        #............Estimated VP
        
        if self.is_plot_estimated_vp_flows:
            plans = virtualpop.get_plans()
            ids_current_plan = virtualpop.ids_plan[ids_vp]
            # ~ times_begin = plans.times_begin[ids_current_plan]
            #~ dep_vps = plans.times_begin[ids_current_plan]/3600.+self.av_dtd_time/7200.
            dep_vps = []
            for id_current_plan in ids_current_plan:
                stages = plans.get_stages(id_current_plan)
                is_act = False
                for stage in stages:
                    if is_act == True:
                        dep_vps.append(stage[0].times_start[stage[1]]/3600.+self.av_dtd_time_vp/7200.)
                    is_act = False
                    if str(stage[0]) == 'activities':
                        is_act = True
            values, bins = np.histogram(dep_vps, bins=bins, density = True)
            ax1.step(step_hours/3600.,values,
                label = 'Estimated VP',
                color = self.color_estimated_vp_flows, 
                linestyle='-', linewidth = self.width_line, 
                #marker = 'o', markersize = self.size_marker, 
                alpha = self.alpha_results
                )
            
            values, bins = np.histogram(dep_vps, bins=bins, density = False)

            ax2.step(step_hours/3600.,values,
                label = 'Estimated VP',
                color = self.color_estimated_vp_flows, 
                linestyle='-', linewidth = self.width_line, 
                #marker = 'o', markersize = self.size_marker, 
                alpha = self.alpha_results
                )
        
        
       
                    
        
                    
                    
        
        #............Estimated Trips
        
        if self.is_plot_estimated_trip_flows:
            dep_trips = trips.times_depart[ids_trip]/3600.+self.av_dtd_time_trips/7200.
            values, bins = np.histogram(dep_trips, bins=bins, density = True)
            ax1.step(step_hours/3600.,values,
                label = 'Estimated trips',
                color = self.color_estimated_trip_flows, 
                linestyle='-', linewidth = self.width_line, 
                #marker = 'o', markersize = self.size_marker, 
                alpha = self.alpha_results
                )
            
            values, bins = np.histogram(dep_trips, bins=bins, density = False)

            ax2.step(step_hours/3600.,values,
                label = 'Estimated trips',
                color = self.color_estimated_trip_flows, 
                linestyle='-', linewidth = self.width_line, 
                #marker = 'o', markersize = self.size_marker, 
                alpha = self.alpha_results
                )
        #............Measured
        if self.is_plot_measured_flows:
            step_flows = []
            for step_hour in step_hours:
                step_flow = np.sum(det_flows[(det_hours == step_hour)])
                step_flows.append(step_flow)

            ax1.step(step_hours/3600.,step_flows/np.sum(step_flows),
                label = 'Detected flows',
                color = self.color_measured_flows, 
                linestyle='-', linewidth = self.width_line, 
                #marker = 'o', markersize = self.size_marker, 
                alpha = self.alpha_results
                )
            
       
            ax2.step(step_hours/3600.,step_flows,
                label = 'Detected flows',
                color = self.color_measured_flows, 
                linestyle='-', linewidth = self.width_line, 
                #marker = 'o', markersize = self.size_marker, 
                alpha = self.alpha_results
                )
        
        #............Simulated
        if self.is_plot_simulated_flows:
            detectors = detectorflows.detectors
            ids_det = detectors.get_ids()
            edgeresultsevolution = simulation.results.edgeresultsevolution
            if len(edgeresultsevolution) > 0:
                    dep_times_sim = np.array(list(range(edgeresultsevolution.n_interval.get_value()+1)))*edgeresultsevolution.time_interval.get_value()+edgeresultsevolution.time_start_recording.get_value()
                    print ('  dep_times_sim=',dep_times_sim)
                    dep_sim = []
                    for id_det in ids_det:
                        ids_lane_det = detectors.ids_lanes[id_det]
                        if ids_lane_det is not None:
                            id_edge_det = demand.parent.net.lanes.ids_edge[ids_lane_det[0]]
                            id_edge_res = edgeresultsevolution.ids_edge.get_id_from_index(id_edge_det)
                            list_flows = edgeresultsevolution.entered[id_edge_res]
                            for flow, i in zip(list_flows, list(range(len(list_flows)))):
                                for j in range(flow):
                                    dep_sim.append(np.random.randint(dep_times_sim[i], dep_times_sim[i]+edgeresultsevolution.time_interval.get_value(),1)/3600.)
                    print('Total flow =', len(dep_sim))
                    
                    values, bins = np.histogram(dep_sim, bins=bins, density = True)
                    ax1.step(step_hours/3600.,values,
                        label = 'Simulated flows',
                        color = self.color_simulated_flows, 
                        linestyle='-', linewidth = self.width_line, 
                        #marker = 'o', markersize = self.size_marker, 
                        alpha = self.alpha_results
                        )
                        
                    values, bins = np.histogram(dep_sim, bins=bins, density = False)
               
                    ax2.step(step_hours/3600.,step_flows,
                        label = 'Simulated flows',
                        color = self.color_simulated_flows, 
                        linestyle='-', linewidth = self.width_line, 
                        #marker = 'o', markersize = self.size_marker, 
                        alpha = self.alpha_results
                        )                
                        
                        
                        # ~ demand.parent.simulation.edgesresults

        
        
        ax1.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
        ax1.grid(self.is_grid)
        if self.is_show_title:
            if self.title != "":
                ax1.set_title(self.title, fontsize=self.size_titlefont)
            else:
                ax1.set_title('Trip distribution over time', fontsize=self.size_titlefont)
        ax1.set_xlabel('Time [h]', fontsize=self.size_labelfont)
        ax1.set_ylabel('Flow density', fontsize=self.size_labelfont)
        ax1.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax1.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('tripdensity_over_time')
        
        ax2.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
        ax2.grid(self.is_grid)
        if self.is_show_title:
            if self.title != "":
                ax2.set_title(self.title, fontsize=self.size_titlefont)
            else:
                ax2.set_title('Trip number over time', fontsize=self.size_titlefont)
        ax2.set_xlabel('Time [h]', fontsize=self.size_labelfont)
        ax2.set_ylabel('Number of Trips', fontsize=self.size_labelfont)
        ax2.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax2.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('trips_over_time')
            
        # self.set_axisborder(ax)
        
       
        if not self.is_save:
            show_plot()
        
        return True
        
 
    
