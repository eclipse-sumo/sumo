# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
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

# @file    results_mpl.py
# @author  Joerg Schweizer
# @date   2012

import os
import numpy as np
from collections import OrderedDict
#import  matplotlib as mpl
#from matplotlib.patches import Arrow,Circle, Wedge, Polygon,FancyArrow
#from matplotlib.collections import PatchCollection
#import matplotlib.colors as colors
#import matplotlib.cm as cmx
#import matplotlib.pyplot as plt
#import matplotlib.image as image

from coremodules.misc.matplottools import *
from coremodules.network import routing

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.geometry import *
from agilepy.lib_base.processes import Process


class Resultplotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Plot results with Matplotlib',
                 info="Creates plots of different results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('resultplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()

        # edgeresultes....
        attrnames_edgeresults = OrderedDict()
        edgeresultattrconfigs = self.parent.edgeresults.get_group_attrs('results')
        edgeresultattrnames = edgeresultattrconfigs.keys()
        # edgeresultattrnames.sort()
        for attrname in edgeresultattrnames:
            attrconfig = edgeresultattrconfigs[attrname]

            attrnames_edgeresults[attrconfig.format_symbol()] = attrconfig.attrname

        #attrnames_edgeresults = {'Entered':'entered'}
        self.edgeattrname = attrsman.add(cm.AttrConf('edgeattrname', kwargs.get('edgeattrname', 'entered'),
                                                     choices=attrnames_edgeresults,
                                                     groupnames=['options'],
                                                     name='Edge Quantity',
                                                     info='The edge related quantity to be plotted.',
                                                     ))
        self.valuelabel = attrsman.add(cm.AttrConf('valuelabel', kwargs.get('valuelabel', ''),
                                                   groupnames=['options'],
                                                   name='value label',
                                                   info='Value label text to label the color bar, if blank then default values are used.',
                                                   ))
        self.add_edgeresultoptions(**kwargs)
        self.add_networkoptions(**kwargs)
        self.add_facilityoptions(**kwargs)
        self.add_zoneoptions(**kwargs)
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)
        self.add_save_options(**kwargs)

    def show(self):
        # print 'show',self.edgeattrname
        # if self.axis  is None:
        axis = init_plot()

        if (self.edgeattrname is not ""):
            resultattrconf = getattr(self.parent.edgeresults, self.edgeattrname)
            ids = self.parent.edgeresults.get_ids()
            if self.is_show_title:
                if self.title != "":
                    title = self.title
                else:
                    title = resultattrconf.get_info()

            self.plot_results_on_map(axis, ids, resultattrconf[ids],
                                     title=title,
                                     valuelabel=resultattrconf.format_symbol())

            if self.is_save:
                plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
                self.save_fig(resultattrconf.attrname)
            else:
                show_plot()

    def do(self):
        # print 'do',self.edgeattrname
        self.show()

    def get_scenario(self):
        return self.parent.get_scenario()


class ElectricalEnergyResultsPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Electrical energy plotter',
                 info="Plot electrical energy results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('electricalenergyresultsplotter', parent=results, name=name,
                          info=info, logger=logger)

        print 'ElectricalEnergyResultsPlotter.__init__', results, self.parent, len(self.get_eneryresults())
        attrsman = self.get_attrsman()

        self.add_plotoptions_lineplot(**kwargs)
        self.add_save_options(**kwargs)

    def get_eneryresults(self):
        return self.parent.electricenergy_vehicleresults

    def show(self):
        eneryresults = self.get_eneryresults()
        print 'show', eneryresults
        # print '  dir(vehicleman)',dir(vehicleman)

        print '  len(eneryresults)', len(eneryresults)
        if len(eneryresults) > 0:
            plt.close("all")
            self.plot_power()

    def plot_power(self):
        print 'plot_power'
        eneryresults = self.get_eneryresults()

        times = eneryresults.times.get_value()
        if len(times) < 2:
            return
        ax = init_plot()

        dt = times[1]-times[0]
        times_scaled = (times-times[0])/60.0
        energies = eneryresults.energies.get_value()
        powers = energies/dt*3600.0  # W energy is in Wh
        powers_av = np.mean(powers)
        ax.plot(times_scaled, powers/1000.0,
                label='Power over time',
                color=self.color_line,
                linestyle='-', linewidth=self.width_line,
                # marker = 'o', markersize = self.size_marker,
                alpha=self.alpha_results
                )
        ax.plot([times_scaled[0], times_scaled[-1]], [powers_av/1000.0, powers_av/1000.0],
                label='Average power = %.2fKW' % (powers_av/1000),
                color='r',
                linestyle='-', linewidth=self.width_line+1.0,
                # marker = 'o', markersize = self.size_marker,
                )

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_show_title:
            if self.title != "":
                ax.set_title(self.title, fontsize=self.size_titlefont)
            else:
                ax.set_title('Vehicle power over time', fontsize=self.size_titlefont)
        ax.set_xlabel('Time [min]', fontsize=self.size_labelfont)
        ax.set_ylabel('Power [KW]', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        # self.set_axisborder(ax)

        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('power')
        else:
            show_plot()


class SpeedprofilePlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Speed profile  plotter',
                 info="Plots speed profile of vehicles using matplotlib",
                 logger=None, **kwargs):

        self._init_common('speedprofileplotter', parent=results, name=name,
                          info=info, logger=logger)

        print 'SpeedprofilePlotter.__init__', results, self.parent, len(self.get_results())
        attrsman = self.get_attrsman()

        self.id_veh_sumo = attrsman.add(cm.AttrConf('id_veh_sumo', kwargs.get('id_veh_sumo', ''),
                                                    groupnames=['options'],
                                                    name='SUMO ID plot',
                                                    info='Plot speed profile for vehicle with this SUMO ID.',
                                                    ))

        self.add_plotoptions_lineplot(color_line=[0, 0, 1, 0.5])

        self.add_save_options(**kwargs)

    def get_results(self):
        return self.parent.trajectory_results

    def show(self):
        results = self.get_results()
        print 'show', results
        id_res = results.ids_sumo.get_id_from_index(self.id_veh_sumo)

        print '  len(results)', len(results), results.ids_sumo.has_index(self.id_veh_sumo)
        if results.ids_sumo.has_index(self.id_veh_sumo):
            id_res = results.ids_sumo.get_id_from_index(self.id_veh_sumo)
            plt.close("all")
            self.plot_speedprofile(np.array(results.times.get_value()), np.array(results.speeds[id_res]))

        else:
            return False

    def plot_speedprofile(self, times, speeds):
        print 'plot_speedprofile'

        if len(times) < 2:
            return False
        ax = init_plot()

        dt = times[1]-times[0]
        inds = np.flatnonzero(np.logical_not(np.isnan(speeds)))
        times_scaled = (times[inds]-times[inds[0]])

        speed_av = np.mean(speeds[inds])
        ax.plot(times_scaled, speeds[inds],
                label='Speed over time',
                color=self.color_line,
                linestyle=self.style_line, linewidth=self.width_line,
                # marker = 'o', markersize = self.size_marker,
                alpha=self.alpha_line
                )
        ax.plot([times_scaled[0], times_scaled[-1]], [speed_av, speed_av],
                label='Average speed = %.2fm/s' % (speed_av),
                color='r',
                linestyle='-', linewidth=self.width_line+1.0,
                # marker = 'o', markersize = self.size_marker,
                )

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_show_title:
            if self.title != "":
                ax.set_title(self.title, fontsize=self.size_titlefont)
            else:
                ax.set_title('Vehicle speed over time', fontsize=self.size_titlefont)
        ax.set_xlabel('Time [min]', fontsize=self.size_labelfont)
        ax.set_ylabel('Speed [m/s]', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        # self.set_axisborder(ax)

        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('speedprofile_'+self.id_veh_sumo)
        else:
            show_plot()


class XYEdgeresultsPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='XY edge result plotter',
                 info="Plot one edge result attribute versus another result attribute using matplotlib",
                 logger=None, **kwargs):

        self._init_common('xyedgeresultsplotter', parent=results, name=name,
                          info=info, logger=logger)

        print 'XYResultsPlotter.__init__', results
        attrsman = self.get_attrsman()

        # edgeresultes....
        attrnames_edgeresults = OrderedDict()
        edgeresultattrconfigs = self.parent.edgeresults.get_group_attrs('results')
        edgeresultattrnames = edgeresultattrconfigs.keys()
        # edgeresultattrnames.sort()
        for attrname in edgeresultattrnames:
            attrconfig = edgeresultattrconfigs[attrname]

            attrnames_edgeresults[attrconfig.format_symbol()] = attrconfig.attrname

        #attrnames_edgeresults = {'Entered':'entered'}
        self.edgeattrname_x = attrsman.add(cm.AttrConf('edgeattrname_x', kwargs.get('edgeattrname_x', 'entered'),
                                                       choices=attrnames_edgeresults,
                                                       groupnames=['options'],
                                                       name='Edge attribute X',
                                                       info='The edge related attribute to be plotted on x-axis.',
                                                       ))

        self.edgeattrname_y = attrsman.add(cm.AttrConf('edgeattrname_y', kwargs.get('edgeattrname_y', 'left'),
                                                       choices=attrnames_edgeresults,
                                                       groupnames=['options'],
                                                       name='Edge attribute Y',
                                                       info='The edge related attribute to be plotted on y-axis.',
                                                       ))

        self.is_linreg = attrsman.add(cm.AttrConf('is_linreg', kwargs.get('is_linreg', True),
                                                  groupnames=['options'],
                                                  name='Plot linear regression',
                                                  info='Plot linear regression.',
                                                  ))
        self.add_plotoptions_lineplot(is_marker_only=False, **kwargs)
        self.add_save_options(**kwargs)

    def show(self):
        print 'show'

        plt.close("all")
        self.plot_xy()

    def plot_xy(self):
        print 'plot_xy', self.edgeattrname_y, 'vs', self.edgeattrname_x

        if (self.edgeattrname_x is not "") & (self.edgeattrname_x is not ""):
            resultattrconf_x = getattr(self.parent.edgeresults, self.edgeattrname_x)
            resultattrconf_y = getattr(self.parent.edgeresults, self.edgeattrname_y)

        else:
            return False

        eps = 10.0**-10
        #ids_res = self.get_ids()
        x = np.array(resultattrconf_x.get_value(), dtype=np.float)
        y = np.array(resultattrconf_y.get_value(), dtype=np.float)
        inds = np.flatnonzero((x > eps) & (y > eps))

        if (len(inds) < 2):
            return False

        fig, ax = init_plot_fig_ax()

        # print '  x',len(x),'v',x[inds]
        # print '  y',len(y),'v',y[inds]
        ax.plot(x[inds], y[inds],
                #label = resultattrconf_x.attrname+'',
                linestyle='None',
                marker=self.style_marker,
                markersize=self.size_marker,
                markerfacecolor=self.color_marker,
                #alpha = self.alpha_line
                )
        if self.is_linreg:
            mx = np.mean(x[inds])
            my = np.mean(y[inds])
            sxx = np.sum((x[inds]-mx)**2)
            syy = np.sum((y[inds]-my)**2)
            sxy = np.sum((x[inds]-mx)*(y[inds]-my))
            R2 = sxy**2/(sxx*syy)
            m = (len(inds) * np.sum(x[inds]*y[inds]) - np.sum(x[inds]) * np.sum(y[inds])) / \
                (len(x[inds])*np.sum(x[inds]*x[inds]) - np.sum(x[inds]) ** 2)
            offset = my - (m*mx)

            x_linreg = np.array([np.min(x[inds]), np.max(x[inds])], dtype=np.float)
            print ' m', m, 'offset', offset
            # print ' x_linreg',offset * x_linreg
            ax.plot(x_linreg, offset + m*x_linreg,
                    label=r'$%.3f+%.3fx$, $R^2=%.3f$' % (offset, m, R2),
                    linewidth=self.width_line,
                    linestyle=self.style_line,
                    color=self.color_line,
                    alpha=self.alpha_line
                    )

            ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)

        ax.grid(self.is_grid)
        if self.is_show_title:
            if self.title != '':
                title = self.title
            else:
                title = resultattrconf_y.get_name()+' vs. '+resultattrconf_x.get_name()
            ax.set_title(title, fontsize=self.size_titlefont)

        if self.xlabel != '':
            xlabel = self.xlabel
        else:
            xlabel = resultattrconf_x.format_symbol(show_parentesis=True)

        if self.ylabel != '':
            ylabel = self.ylabel
        else:
            ylabel = resultattrconf_y.format_symbol(show_parentesis=True)

        ax.set_xlabel(xlabel, fontsize=self.size_labelfont)
        ax.set_ylabel(ylabel, fontsize=self.size_labelfont)

        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        # self.set_axisborder(ax)
        fig.tight_layout()
        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig(resultattrconf_y.attrname+'_vs_'+resultattrconf_x.attrname)
        else:
            show_plot()


SELECTPOINTS = {'FromOrigin': 1,
                'ToDestination': 2,
                'FromOriginToDestination': 3,
                'All': 4,
                }
# PLOTTYPE = {'Isochrone':1,
#                'Heatmap':2,
#                }
# POINTSTYPE = {'OnlyOriginPoints':1,
#                'OnlyDestinationPoints':2,
#                'OnlyOriginAndDestinationPoints':3,
#                'All':4,
#                }


class TraveltimePlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Travel time plotter',
                 info="Plots travel times to or from a specific edge on map using matplotlib",
                 logger=None, **kwargs):

        self._init_common('isochoneplotter', parent=results, name=name,
                          info=info, logger=logger)

        print 'TraveltimePlotter.__init__', results, self.parent
        attrsman = self.get_attrsman()
        scenario = self.get_scenario()

        self.id_edge_reference = attrsman.add(cm.AttrConf('id_edge_reference', kwargs.get('id_edge_reference', -1),
                                                          groupnames=['options'],
                                                          name='ID reference edge',
                                                          info='This is the edge of origin or destination to which travel times refer to.',
                                                          ))

        self.id_mode = attrsman.add(cm.AttrConf('id_mode',    kwargs.get('id_mode', -1),
                                                groupnames=['options'],
                                                choices=scenario.net.modes.names.get_indexmap(),
                                                name='Mode',
                                                info='Transport mode for which travel times are plotted.',
                                                ))

        self.direction = attrsman.add(cm.AttrConf('direction', kwargs.get('direction', 'destination'),
                                                  groupnames=['options'],
                                                  choices=['origin', 'destination'],
                                                  name='Direction',
                                                  info='Routing direction, to determine weather the reference edge is origin or destination of trips.',
                                                  ))

        # self.plot_type = attrsman.add(cm.AttrConf(  'plot_type', kwargs.get('plot_type', 'edge values'),
        #                                groupnames = ['options'],
        #                                choices = ['edge values','heatmap'],
        #                                name = 'Plot type',
        #                                info = 'Choice the type of plot',
        #                                ))
        self.is_resulttraveltimes = attrsman.add(cm.AttrConf('is_resulttraveltimes', kwargs.get('is_resulttraveltimes', True),
                                                             groupnames=['options'],
                                                             name='Use result travel times',
                                                             info='Use edge travel times from simulation results where non zero results were obtained. If not set, free flow travel times are used.',
                                                             ))

        self.is_isochrone = attrsman.add(cm.AttrConf('is_isochrone', kwargs.get('is_isochrone', False),
                                                     groupnames=['options'],
                                                     name='Plot isochrones in the map',
                                                     info='Plot isochrones lines in the map.',
                                                     ))
        self.time_max = attrsman.add(cm.AttrConf('time_max', kwargs.get('time_max', 3600.0),
                                                 groupnames=['options'],
                                                 perm='wr',
                                                 name='Max time',
                                                 info='Max cumulative travel time from/to target edge to be considered for plotting.',
                                                 ))
        self.min_n_values = attrsman.add(cm.AttrConf('min_n_values', kwargs.get('min_n_values', 2),
                                                     groupnames=['options'],
                                                     perm='wr',
                                                     name='Minumun number of values',
                                                     info='Minumun number of values to assign a time to each edge.',
                                                     ))

        # self.vmin = attrsman.add(cm.AttrConf(  'vmin', kwargs.get('vmin',0 ),
        #                                groupnames = ['options'],
        #                                perm='wr',
        #                                name = 'Min value',
        #                                info = 'Min value of points in a bin for the heatmap of points.',
        #                                ))

        self.add_edgeresultoptions(**kwargs)
        self.add_networkoptions(**kwargs)
        self.add_facilityoptions(**kwargs)
        self.add_zoneoptions(**kwargs)
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)
        self.add_save_options(**kwargs)

    def show(self):
        # init

        edgeresults = self.parent.edgeresults
        scenario = self.get_scenario()

        net = scenario.net
        edges = net.edges
        unit = self.unit_mapscale
        mapscale = self.get_attrsman().get_config('unit_mapscale').mapscales[unit]
        ax = init_plot(tight_layout=True)

        self.plot_net(ax, mapscale=mapscale,  unit=unit, is_configure=False)

        edgetimes = edges.get_times(id_mode=self.id_mode,
                                    speed_max=net.modes.speeds_max[self.id_mode],
                                    is_check_lanes=True
                                    )

        if self.is_resulttraveltimes:
            # update edge travel times with those from simulation
            ids_res = edgeresults.get_ids()
            inds_valid = np.flatnonzero(edgeresults.traveltime[ids_res] > 0)
            ids_edge_res = edgeresults.ids_edge[ids_res[inds_valid]]
            edgetimes[ids_edge_res] = edgeresults.traveltime[ids_res[inds_valid]]

        bstar = edges.get_bstar()
        ids_edge_from, edgetimestree, tree = routing.edgedijkstra_backwards(self.id_edge_reference,
                                                                            self.time_max,
                                                                            weights=edgetimes,
                                                                            bstar=bstar,
                                                                            )

        # print '  ids_edge_from',ids_edge_from
        ids_from_all = routing.get_edges_orig_from_tree(self.id_edge_reference, tree)
        # for id_edge, val in tree.iteritems():
        #    print '    id_edge',id_edge,'val',val
        # print '  ids_from_all',ids_from_all

        if self.direction == 'origin':
            title = 'Accumulated travel times from edge ID %d' % (self.id_edge_reference)
            fstar = edges.get_fstar()
            edgetimestree, tree = routing.edgedijkstra(self.id_edge_reference,
                                                       ids_edge_target=ids_from_all,
                                                       weights=edgetimes,
                                                       fstar=fstar,
                                                       )
        elif self.direction == 'destination':
            title = 'Accumulated travel times to edge ID %d' % (self.id_edge_reference)
        ids_edge = np.array(edgetimestree.keys(), dtype=np.int32)
        edgetimes = np.array(edgetimestree.values(), dtype=np.float32)

        self.plot_results_on_map(ax, ids_edge=ids_edge,
                                 values=edgetimes,
                                 valuelabel='Travel time [s]',
                                 )

        if 0:  # self.is_isochrone:

            print 'isochrone plot not yet implemented'

            times_point = np.zeros(np.max(ids_point)+1)
            for id_point in ids_point:
                if self.select_points == 1:
                    time = (points.timestamps[id_point] - trips.timestamps[points.ids_trip[id_point]])/60.0
                if self.select_points == 2:
                    time = -(points.timestamps[id_point] -
                             points.timestamps[trips.ids_points[points.ids_trip[id_point]][-1]])/60.0
                if self.select_points == 3:
                    time = (points.timestamps[id_point] - trips.timestamps[points.ids_trip[id_point]])/60.0
                if self.select_points == 4:
                    time = (points.timestamps[id_point] - trips.timestamps[points.ids_trip[id_point]])/60.0

                times_point[id_point] = time
            self.plot_isochrone(ax, ids_point, zone_shape_origin, times_point)

        self.configure_map(ax, title=title, unit=unit)
        if self.is_save:
            self.save_fig('accumulated_traveltimes')
        show_plot()

    def plot_isochrone(self, ax, ids_point, zone_shape, times_point):

        mapmatching = self.parent.parent
        scenario = mapmatching.get_scenario()
        points = scenario.demand.mapmatching.points
        zone_g = np.array([np.mean(np.array(zone_shape)[:, 0]), np.mean(np.array(zone_shape)[:, 1])])
        coords = points.coords[ids_point]
        coords = coords[:][:, :2]
        isochrone = []
        for coord, id_point, time_point in zip(coords,  ids_point, times_point[ids_point]):
            if zone_g[0] < coord[0] and zone_g[1] < coord[1]:
                angle = np.arctan((coord[0]-zone_g[0])/(coord[1]-zone_g[1]))
            if zone_g[0] < coord[0] and zone_g[1] > coord[1]:
                angle = np.arctan((zone_g[1]-coord[1])/(coord[0]-zone_g[0])) + np.pi/2
            if zone_g[0] > coord[0] and zone_g[1] > coord[1]:
                angle = np.arctan((zone_g[0]-coord[0])/(zone_g[1]-coord[1])) + np.pi
            if zone_g[0] > coord[0] and zone_g[1] < coord[1]:
                angle = np.arctan((coord[1]-zone_g[1])/(zone_g[0]-coord[0])) + np.pi*3/2
            dist = np.sqrt(np.sum((coord-zone_g)**2))
            isochrone.append([angle, id_point, time_point, dist])
        isochrone = np.asarray(isochrone)
        isochrone = isochrone[isochrone[:, 0].argsort()]
        isochrone = isochrone[(isochrone[:, 2] > 0)]

        n_bins = 200.0
##        max_perc_var = 20.
        isochrone_times = [5., 10., 15., 20., 25., 30., 35.,  40.]
        isochrone_shapes = [[]]*len(isochrone_times)
        for bin in range(int(n_bins)):
            for i in range(len(isochrone_times)):
                iso_points = isochrone[(bin*2.0*np.pi/n_bins <= isochrone[:, 0]) & (isochrone[:, 0] < (bin+1)
                                                                                    * 2.0*np.pi/n_bins) & (0 < isochrone[:, 2]) & (isochrone_times[i] > isochrone[:, 2])]
                if len(iso_points) > 10:
                    if len(isochrone_shapes[i]) == 0:
                        isochrone_shapes[i] = [[(points.coords[iso_points[np.argmax(iso_points[:, 3]), 1]][0]),
                                                (points.coords[iso_points[np.argmax(iso_points[:, 3]), 1]][1])]]
                    else:
                        isochrone_shapes[i].append([(points.coords[iso_points[np.argmax(iso_points[:, 3]), 1]][0]),
                                                    (points.coords[iso_points[np.argmax(iso_points[:, 3]), 1]][1])])
        print isochrone_shapes
        for isochrone_shape in isochrone_shapes:
            verts = np.array(isochrone_shape)[:, :2].tolist()
            verts.append([0, 0])
            codes = [Path.MOVETO]
            for i in range(len(verts)-2):
                codes.append(Path.LINETO)
            codes.append(Path.CLOSEPOLY)
            path = Path(verts, codes)
            facecolor = 'none'
            patch = patche.PathPatch(path, facecolor=facecolor, edgecolor='r', lw=5, alpha=0.8)
            ax.add_patch(patch)

        for isochrone_shape in isochrone_shapes:

            if len(isochrone_shape) > 4:
                zone_shape = isochrone_shape
                print zone_shape
                for zone_shape_coords, i in zip(isochrone_shape, range(len(isochrone_shape))):
                    print i, len(isochrone_shape)
                    if i == 0:
                        zone_shape[i] = ((np.array(isochrone_shape[i])+np.array(isochrone_shape[i+1])+np.array(
                            isochrone_shape[-1])+np.array(isochrone_shape[i+2])+np.array(isochrone_shape[-2]))/5.).tolist()
                    elif i == (len(isochrone_shape)-1):
                        zone_shape[i] = ((np.array(isochrone_shape[i])+np.array(isochrone_shape[0])+np.array(
                            isochrone_shape[i-1])+np.array(isochrone_shape[i-2])+np.array(isochrone_shape[1]))/5.).tolist()
                    elif i == 1:
                        zone_shape[i] = ((np.array(isochrone_shape[i])+np.array(isochrone_shape[i+1])+np.array(
                            isochrone_shape[i-1])+np.array(isochrone_shape[i+2])+np.array(isochrone_shape[-1]))/5.).tolist()
                    elif i == (len(isochrone_shape)-2):
                        zone_shape[i] = ((np.array(isochrone_shape[i])+np.array(isochrone_shape[i+1])+np.array(
                            isochrone_shape[i-1])+np.array(isochrone_shape[i-2])+np.array(isochrone_shape[0]))/5.).tolist()
                    else:
                        zone_shape[i] = ((np.array(isochrone_shape[i])+np.array(isochrone_shape[i+1])+np.array(
                            isochrone_shape[i-1])+np.array(isochrone_shape[i+2])+np.array(isochrone_shape[i-2]))/5.).tolist()

                verts = np.array(zone_shape)[:, :2].tolist()
                verts.append([0, 0])
                codes = [Path.MOVETO]
                for i in range(len(verts)-2):
                    codes.append(Path.LINETO)
                codes.append(Path.CLOSEPOLY)
                path = Path(verts, codes)
                facecolor = 'none'
                patch = patche.PathPatch(path, facecolor=facecolor, edgecolor='b', lw=5, alpha=0.8)
                ax.add_patch(patch)

        return True

    def get_scenario(self):
        return self.parent.get_scenario()
