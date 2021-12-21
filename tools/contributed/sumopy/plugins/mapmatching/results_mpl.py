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

from numpy.linalg import inv
import os
##import math
import numpy as np
from collections import OrderedDict
import matplotlib.pyplot as plt
from agilepy.lib_base.geometry import *
from matplotlib.path import Path
import matplotlib.patches as patche
from coremodules.demand.origin_to_destination import OdIntervals
from agilepy.lib_base.misc import get_inversemap
from coremodules.misc.matplottools import *
from coremodules.network import network
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.geometry import *
from agilepy.lib_base.processes import Process
from mapmatching import COLOR_MATCHED_ROUTE, COLOR_SHORTEST_ROUTE, COLOR_FASTEST_ROUTE
import time
try:
    from scipy import interpolate
    is_scipy = True
except:
    is_scipy = False


def is_sublist(l, s):
    sub_set = False
    if s == []:
        sub_set = True
    elif s == l:
        sub_set = True
    elif len(s) > len(l):
        sub_set = False

    else:
        for i in range(len(l)):
            if l[i] == s[0]:
                n = 1
                while (n < len(s)) and (l[i+n] == s[n]):
                    n += 1

                if n == len(s):
                    sub_set = True

    return sub_set


def kf_update(X, P, Y, H, R):
    IM = dot(H, X)
    IS = R + dot(H, dot(P, H.T))
    K = dot(P, dot(H.T, inv(IS)))
    X = X + dot(K, (Y-IM))
    P = P - dot(K, dot(IS, K.T))
    LH = gauss_pdf(Y, IM, IS)
    return (X, P, K, IM, IS, LH)


def gauss_pdf(X, M, S):
    if M.shape()[1] == 1:
        DX = X - tile(M, X.shape()[1])
        E = 0.5 * sum(DX * (dot(inv(S), DX)), axis=0)
        E = E + 0.5 * M.shape()[0] * log(2 * pi) + 0.5 * log(det(S))
        P = exp(-E)
    elif X.shape()[1] == 1:
        DX = tile(X, M.shape()[1]) - M
        E = 0.5 * sum(DX * (dot(inv(S), DX)), axis=0)
        E = E + 0.5 * M.shape()[0] * log(2 * pi) + 0.5 * log(det(S))
        P = exp(-E)
    else:
        DX = X-M
        E = 0.5 * dot(DX.T, dot(inv(S), DX))
        E = E + 0.5 * M.shape()[0] * log(2 * pi) + 0.5 * log(det(S))
        P = exp(-E)
    return (P[0], E[0])


SELECTPOINTS = {'FromOrigin': 1,
                'ToDestination': 2,
                'FromOriginToDestination': 3,
                'All': 4,
                }
PLOTTYPE = {'Isochrone': 1,
            'Heatmap': 2,
            }
POINTSTYPE = {'OnlyOriginPoints': 1,
              'OnlyDestinationPoints': 2,
              'OnlyOriginAndDestinationPoints': 3,
              'All': 4,
              }


class PointresultPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Plot point results with Matplotlib',
                 info="Creates plots of different point results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('pointresultplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()
        mapmatching = self.parent.parent
        scenario = mapmatching.get_scenario()
        self.zones = scenario.landuse.zones

        self.select_points = attrsman.add(cm.AttrConf('select_points', kwargs.get('select_points', SELECTPOINTS['FromOrigin']),
                                                      groupnames=['options'],
                                                      choices=SELECTPOINTS,
                                                      name='Select points',
                                                      info='Select trip as indicated',
                                                      ))
        self.plot_type = attrsman.add(cm.AttrConf('plot_type', kwargs.get('plot_type', PLOTTYPE['Heatmap']),
                                                  groupnames=['options'],
                                                  choices=PLOTTYPE,
                                                  name='Plot type',
                                                  info='Choice the type of plot',
                                                  ))
        self.points_type = attrsman.add(cm.AttrConf('points_type', kwargs.get('points_type', POINTSTYPE['All']),
                                                    groupnames=['options'],
                                                    choices=POINTSTYPE,
                                                    name='Points type',
                                                    info='Plot only trip points of the indicated type',
                                                    ))

# self.is_plot_isochrone_generated = attrsman.add(cm.AttrConf(  'is_plot_isochrone_generated', kwargs.get('is_plot_isochrone_generated', True),
##                                        groupnames = ['options'],
##                                        name = 'Plot isochrone map from the origin zone',
##                                        info = 'Plot isochrone map on edge-network for trips starting from the origin zone.',
# ))
# self.is_plot_isochrone_attracted = attrsman.add(cm.AttrConf(  'is_plot_isochrone_attracted', kwargs.get('is_plot_isochrone_attracted', True),
##                                        groupnames = ['options'],
##                                        name = 'Plot isochrone map to the destination zone',
##                                        info = 'Plot isochrone map on edge-network for trips ending to the origin zone.',
# ))
        self.is_isochrone = attrsman.add(cm.AttrConf('is_isochrone', kwargs.get('is_isochrone', False),
                                                     groupnames=['options'],
                                                     name='Plot isochrones in the map',
                                                     info='Plot isochrones lines in the map.',
                                                     ))
        self.max_time = attrsman.add(cm.AttrConf('max_time', kwargs.get('max_time', 40),
                                                 groupnames=['options'],
                                                 perm='wr',
                                                 name='Max time',
                                                 info='Max time to be considered for the isochrone plot.',
                                                 ))
        self.min_n_values = attrsman.add(cm.AttrConf('min_n_values', kwargs.get('min_n_values', 2),
                                                     groupnames=['options'],
                                                     perm='wr',
                                                     name='Minumun number of values',
                                                     info='Minumun number of values to assign a time to each edge.',
                                                     ))
# self.is_plot_points_gen_heatmap = attrsman.add(cm.AttrConf(  'is_plot_points_gen_heatmap', kwargs.get('is_plot_points_gen_heatmap', False),
##                                        groupnames = ['options'],
##                                        name = 'Plot generated trip points from the origin zone',
##                                        info = 'Plot a density heatmap of generated trip points from a zone.',
# ))
# self.is_plot_points_attr_heatmap = attrsman.add(cm.AttrConf(  'is_plot_points_attr_heatmap', kwargs.get('is_plot_points_attr_heatmap', False),
##                                        groupnames = ['options'],
##                                        name = 'Plot attracted trip points from the destination zone',
##                                        info = 'Plot a density heatmap of attracted trip points from a zone.',
# ))
# self.is_plot_points_od_heatmap = attrsman.add(cm.AttrConf(  'is_plot_points_od_heatmap', kwargs.get('is_plot_points_od_heatmap', False),
##                                        groupnames = ['options'],
##                                        name = 'Plot od trip points  heatmap',
##                                        info = 'Plot a density heatmap of trip points from the origin zone to the destination zone.',
# ))
# self.is_plot_points_heatmap = attrsman.add(cm.AttrConf(  'is_plot_points_heatmap', kwargs.get('is_plot_points_heatmap', False),
##                                        groupnames = ['options'],
##                                        name = 'Plot density points heatmap',
##                                        info = 'Plot density heatmap of the selected GPS points.',
# ))

        self.origin_zone_name = attrsman.add(cm.AttrConf('origin_zone_name', kwargs.get('origin_zone_name', self.zones.ids_sumo[0]),
                                                         groupnames=['options'],
                                                         choices=self.zones.ids_sumo,
                                                         name='Origin zone name',
                                                         info='Generating zone name.',
                                                         ))

        self.dest_zone_name = attrsman.add(cm.AttrConf('dest_zone_name', kwargs.get('dest_zone_name', self.zones.ids_sumo[0]),
                                                       groupnames=['options'],
                                                       choices=self.zones.ids_sumo,
                                                       name='Destination zone name',
                                                       info='Attracting zone name.',
                                                       ))

        self.bins = attrsman.add(cm.AttrConf('bins', kwargs.get('bins', 500),
                                             groupnames=['options'],
                                             perm='wr',
                                             name='Bins',
                                             info='Bins for the heatmap of points.',
                                             ))

        self.vmin = attrsman.add(cm.AttrConf('vmin', kwargs.get('vmin', 0),
                                             groupnames=['options'],
                                             perm='wr',
                                             name='Min value',
                                             info='Min value of points in a bin for the heatmap of points.',
                                             ))

        self.is_net = attrsman.add(cm.AttrConf('is_net', kwargs.get('is_net', False),
                                               groupnames=['options'],
                                               name='Show net',
                                               info='Show the network in the heatmap of points.',
                                               ))

        self.titletext = attrsman.add(cm.AttrConf('titletext', kwargs.get('titletext', ''),
                                                  groupnames=['options'],
                                                  name='Title text',
                                                  info='Title text. Empty text means no title.',
                                                  ))
        self.size_titlefont = attrsman.add(cm.AttrConf('size_titlefont', kwargs.get('size_titlefont', 32),
                                                       groupnames=['options'],
                                                       name='Title fontsize',
                                                       info='Title fontsize.',
                                                       ))

        self.add_plotoptions(**kwargs)

        self.add_save_options(**kwargs)

    def show(self):
        # init
        self.init_figures()
        fig = self.create_figure()
        results = self.parent
        ax = fig.add_subplot(111)
        mapmatching = self.parent.parent
        scenario = mapmatching.get_scenario()
        points = scenario.demand.mapmatching.points
        trips = scenario.demand.mapmatching.trips
        zones = scenario.landuse.zones
        net = scenario.net
        ids_edge = net.edges.get_ids()
        ids_trip = trips.get_ids()
        zone_shape_origin = zones.shapes[zones.ids_sumo.get_id_from_index(self.origin_zone_name)]
        zone_shape_dest = zones.shapes[zones.ids_sumo.get_id_from_index(self.dest_zone_name)]

        # Plot net
        if self.is_net:
            print 'plot net'
            plot_net(ax, net, color_edge="gray", width_edge=2, color_node=None,
                     alpha=0.5)
        # Plot zones
        if self.select_points == 1 or self.select_points == 3:
            # plot origin zone
            plot_zone(ax, zone_shape_origin, color_zone="coral",
                      color_outline='black', width_line=2,
                      alpha=0.4, zorder=+201)
        if self.select_points == 2 or self.select_points == 3:
            # plot destination zone
            plot_zone(ax, zone_shape_dest, color_zone="coral",
                      color_outline='black', width_line=2,
                      alpha=0.4, zorder=+201)
        # deselect points
        points.are_selected[points.get_ids()] = False
        # Take into account only selected traces with at least two point
        ids_trip = ids_trip[(trips.are_selected[ids_trip] == True) & (trips.ids_points[ids_trip] != int)]
        ids_points = trips.ids_points[ids_trip]

        # select points
        ids_final_point = np.zeros((len(ids_trip)), dtype=np.int32)
        ids_initial_point = np.zeros((len(ids_trip)), dtype=np.int32)
        for id_trip, ids_point, i in zip(trips.get_ids(), ids_points, range(len(ids_trip))):
            ids_final_point[i] = ids_point[-1]
            ids_initial_point[i] = ids_point[0]
            if self.select_points == 1:
                points.are_selected[ids_point] = is_point_in_polygon(
                    points.coords[ids_initial_point[i]], zone_shape_origin)
            if self.select_points == 2:
                points.are_selected[ids_point] = is_point_in_polygon(points.coords[ids_final_point[i]], zone_shape_dest)
            if self.select_points == 3:
                points.are_selected[ids_point] = is_point_in_polygon(
                    points.coords[ids_initial_point[i]], zone_shape_origin)*is_point_in_polygon(points.coords[ids_final_point[i]], zone_shape_dest)
            if self.select_points == 4:
                points.are_selected[ids_point] = True

        # consider only selected points
        if self.points_type == 1:
            ids_point = ids_initial_point[(points.are_selected[ids_initial_point] == True)]
        if self.points_type == 2:
            ids_point = ids_final_point[(points.are_selected[ids_final_point] == True)]
        if self.points_type == 3:
            ids_initial_point = ids_initial_point[(points.are_selected[ids_initial_point] == True)]
            ids_initial_point.tolist()
            ids_final_point = ids_final_point[(points.are_selected[ids_final_point] == True)]
# ids_final_point.tolist()
            ids_point = ids_initial_point.tolist()
            ids_point.extend(ids_final_point)
            print ids_initial_point, ids_final_point, ids_points
        if self.points_type == 4:
            ids_point = points.get_ids(points.are_selected.get_value() == True)
        coords = points.coords[ids_point]

        if self.plot_type == 1:
            # Match time points with edges and call isochrone plot, if selected
            min_n_values = self.min_n_values
            times_edge = np.zeros(np.max(ids_edge))
            values = np.zeros(np.max(ids_edge))
            valuess = []
            for i in range(np.max(ids_edge)-1):
                valuess.append([])
            count = np.zeros(np.max(ids_edge))
# if self.is_isochrone:
##                times_point = np.zeros(np.max(ids_point)+1)
            for coord, id_point in zip(coords, ids_point):
                coord = np.array([coord[0], coord[1]])
                id_closest_edge = net.edges.get_closest_edge(coord)
                print 'point', id_point
                if id_closest_edge > 0:
                    if self.select_points == 1:
                        time = (points.timestamps[id_point] - trips.timestamps[points.ids_trip[id_point]])/60.0
                    if self.select_points == 2:
                        time = -(points.timestamps[id_point] -
                                 points.timestamps[trips.ids_points[points.ids_trip[id_point]][-1]])/60.0
                    if self.select_points == 3:
                        time = (points.timestamps[id_point] - trips.timestamps[points.ids_trip[id_point]])/60.0
                    if self.select_points == 4:
                        time = (points.timestamps[id_point] - trips.timestamps[points.ids_trip[id_point]])/60.0

                    if 0 < time < self.max_time:
                        count[id_closest_edge-1] += 1
                        times_edge[id_closest_edge-1] += time
                        if count[id_closest_edge-1] > min_n_values:
                            values[id_closest_edge-1] = times_edge[id_closest_edge-1]/count[id_closest_edge-1]

                        # For the isochrone plot
# if self.is_isochrone:
##                    times_point[id_point] = time
            print 'number of points:', np.sum(count)

            # Plot result on map
            print 'plot results on map'
            if self.select_points == 1:
                title = 'Isochrone from the origin zone'
            if self.select_points == 2:
                title = 'Isochrone to the destination zone'
            if self.select_points == 3:
                title = 'Isochrone from the origin to the destination zone'
            if self.select_points == 4:
                title = 'Isochrone for all trips'

            self.plot_results_on_map(ax, ids_edge=ids_edge,
                                     values=values[ids_edge-1],
                                     title=title,
                                     valuelabel='Minutes',
                                     )

            if self.is_save:
                if self.select_points == 1:
                    self.save_fig('routeana_isochrone_fromO')
                if self.select_points == 2:
                    self.save_fig('routeana_isochrone_toD')
                if self.select_points == 3:
                    self.save_fig('routeana_isochrone_FromOtoD')
                if self.select_points == 4:
                    self.save_fig('routeana_isochrone')

        if self.plot_type == 2:

            # Plot heatmap
            x_points = coords[:, 0]
            y_points = coords[:, 1]
            xedges = np.linspace(0., 14000., num=self.bins)
            yedges = np.linspace(0., 11000., num=self.bins)
            extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
            H, xedges, yedges = np.histogram2d(x_points, y_points, bins=(xedges, yedges))
            X, Y = np.meshgrid(xedges, yedges)
            if self.titletext != '':
                ax.set_title(self.titletext, fontsize=self.size_titlefont)
            else:
                if self.select_points == 1:
                    ax.set_title('Density heatmap of trip points from the origin zone', fontsize=self.size_titlefont)
                if self.select_points == 2:
                    ax.set_title('Density heatmap of trip points to the destination zone', fontsize=self.size_titlefont)
                if self.select_points == 3:
                    ax.set_title('Density heatmap of trip points from the origin zone to the destination zone',
                                 fontsize=self.size_titlefont)
                if self.select_points == 4:
                    ax.set_title('Density heatmap of all trip points', fontsize=self.size_titlefont)

            plt.imshow(list(zip(*reversed(list(zip(*reversed(list(zip(*reversed(H))))))))),
                       vmin=self.vmin, cmap='Reds', extent=extent, interpolation='bicubic', alpha=0.9)
            plt.colorbar()
            if self.is_save:
                if self.select_points == 1:
                    self.save_fig('routeana_heatmappoints_fromO')
                if self.select_points == 2:
                    self.save_fig('routeana_heatmappoints_toD')
                if self.select_points == 3:
                    self.save_fig('routeana_heatmappoints_FromOtoD')
                if self.select_points == 4:
                    self.save_fig('routeana_heatmappoints')

        # print isochrone
        if self.is_isochrone:
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

        if not self.is_save:
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


class NoderesultPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Node results plotter',
                 info="Plots nodes related results of GPS trips using matplotlib",
                 logger=None, **kwargs):

        self._init_common('noderesultplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()

        self.plotthemefuncs = OrderedDict([
            ('times wait', self.plot_times_wait),
        ])
        self.plottheme = attrsman.add(cm.AttrConf('plottheme', kwargs.get('plottheme', 'times wait'),
                                                  groupnames=['options'],
                                                  choices=self.plotthemefuncs.keys(),
                                                  name='Plot theme',
                                                  info='Theme  or edge attribute to be plottet.',
                                                  ))

        self.n_min_matched = attrsman.add(cm.AttrConf('n_min_matched', 3,
                                                      groupnames=['options'],
                                                      name='Minum number of matched for speed analysis',
                                                      info='Only nodes contained in almost this number of matched routes\
                                        will be considered for plotting the dynamic\
                                        characteristics of edges (speeds and times).',
                                                      ))
        self.add_plotoptions(**kwargs)
        self.add_save_options(**kwargs)

        attrsman.delete('plottype')
        attrsman.delete('resultwidth')
        attrsman.delete('length_arrowhead')
        attrsman.delete('is_widthvalue')

    def plot_all_themes(self):
        for plottheme in self.plotthemefuncs.keys():
            self.plottheme = plottheme
            self.show()
##
# self.is_grid = attrsman.add(cm.AttrConf(  'is_grid', kwargs.get('is_grid', True),
##                                        groupnames = ['options'],
##                                        name = 'Show grid?',
##                                        info = 'If True, shows a grid on the graphical representation.',
# ))
##
# self.titletext = attrsman.add(cm.AttrConf(  'titletext', kwargs.get('titletext', ''),
##                                        groupnames = ['options'],
##                                        name = 'Title text',
##                                        info = 'Title text. Empty text means no title.',
# ))
##
# self.size_titlefont = attrsman.add(cm.AttrConf('size_titlefont', kwargs.get('size_titlefont',32),
##                                        groupnames = ['options'],
##                                        name = 'Title fontsize',
##                                        info = 'Title fontsize.',
# ))
##
# self.size_labelfont = attrsman.add(cm.AttrConf('size_labelfont', kwargs.get('size_labelfont',24),
##                                        groupnames = ['options'],
##                                        name = 'Label fontsize',
##                                        info = 'Label fontsize.',
# ))
##
# self.width_line = attrsman.add(cm.AttrConf(  'width_line', kwargs.get('width_line', 1.0),
##                                        groupnames = ['options'],
##                                        name = 'Line width',
##                                        info = 'Line width of plot.',
# ))
        self.add_save_options()

    def get_noderesults(self):
        return self.parent.nodesresults

    def show(self):
        print 'NoderesultPlotter.show', self.plottheme
        # if self.axis  is None:
        #axis = init_plot()
        self.init_figures()
        fig = self.create_figure()
        axis = fig.add_subplot(111)
        self.plotthemefuncs[self.plottheme](axis)

        print '  self.is_save', self.is_save
        if not self.is_save:
            print '  show_plot'
            show_plot()
        else:
            figname = 'nodeplot_'+self.plottheme
            # print '  savefig',figname

            # self.save_fig('edgeplot_'+self.plottheme)

            rootfilepath = self.get_scenario().get_rootfilepath()

            fig.savefig("%s_%s.%s" % (rootfilepath, figname, self.figformat),
                        format=self.figformat,
                        dpi=self.resolution,
                        # orientation='landscape',
                        orientation='portrait',
                        transparent=True)
            plt.close(fig)

    def plot_times_wait(self, ax):
        print 'show noderesults', len(self.parent.nodesresults)
        # if self.axis  is None:

        nodesresults = self.parent.nodesresults

        if len(nodesresults) == 0:
            return False

        mapmatching = self.parent.parent
        trips = mapmatching.trips
        #points = mapmatching.points
        routes = trips.get_routes()
        scenario = mapmatching.get_scenario()
        edges = scenario.net.edges
        nodes = scenario.net.nodes
        ids_noderes = nodesresults.select_ids(
            nodesresults.numbers_matched_for_speed_analysis.get_value() > self.n_min_matched)
        nodetypes = nodes.types[nodesresults.ids_node[ids_noderes]]
        times_wait = nodesresults.times_wait[ids_noderes]
        nodetypeset = set(nodetypes)

##        map_type_to_typename = get_inversemap(nodes.types.choices)
##        map_typename_to_times_wait = {}
# for thistype in nodetypeset:
##            map_typename_to_times_wait[map_type_to_typename[thistype]] = np.mean(times_wait[nodetypes == thistype])

# self.init_figures()
##        fig = self.create_figure()
##        ax = fig.add_subplot(111)
        self.plot_node_results_on_map(ax, ids_noderes,
                                      # edgesresults.differences_dist_tot_shortest[ids_result]/edgesresults.numbers_tot_shortest[ids_result],
                                      times_wait,
                                      title='Average waiting time at intersections',
                                      valuelabel='Average waiting time [s]',
                                      is_node_results=True,)


# colors = np.array(COLORS,dtype = np.object)
##        inds_plot = np.arange(len(map_typename_to_times_wait), dtype = np.int32)
##        bar_width = 0.45
##        opacity = 0.5
# error_config = {'ecolor': '0.3'}
# print '  colors',get_colors(inds_plot)
# rects = ax.barh( inds_plot,
# map_typename_to_times_wait.values(),
# align='center',
# alpha=opacity,
# height=bar_width,
# color=get_colors(inds_plot),
# yerr=std_women, error_kw=error_config,
##                         linewidth = self.width_line,
# facecolor=colors[inds][inds_nz],
# )
##
##
##        ax.set_yticks( inds_plot + bar_width / 2)
# ax.set_yticklabels(map_typename_to_times_wait.keys())
# ax.legend()
##
# ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
# ax.grid(self.is_grid)
# if self.titletext != '':
##            ax.set_title(self.titletext, fontsize=self.size_titlefont)
##        ax.set_xlabel('Average wait times [s]', fontsize=self.size_labelfont)
##        ax.set_ylabel('Intersection type', fontsize=self.size_labelfont)
##        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
##        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
# fig.tight_layout()
# if self.is_save:
# self.save_fig('virtualpop_strategy_share_current')
##
##
##
# plt.show()
        # show_plot()

    def get_scenario(self):
        return self.parent.get_scenario()


class SpeedprofilePlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Speedprofile plotter with Matplotlib',
                 info="Plots the speed profile of a selected GPS trip using matplotlib",
                 logger=None, **kwargs):

        self._init_common('speedprofileplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()

        self.id_trip = attrsman.add(cm.AttrConf('id_trip', kwargs.get('id_trip', -1),
                                                groupnames=['options'],
                                                name='Trip ID',
                                                info='ID of GPS trip to be plotted.',
                                                ))

        self.is_plot_similar_trips = attrsman.add(cm.AttrConf('is_plot_similar_trips', kwargs.get('is_plot_similar_trips', True),
                                                              groupnames=['options'],
                                                              name='Plot similar trips',
                                                              info='If True, plot all trips which contain the same route as the given trip.',
                                                              ))

        self.method_interp = attrsman.add(cm.AttrConf('method_interp', kwargs.get('method_interp', 'slinear'),
                                                      groupnames=['options'],
                                                      choices=['linear', 'nearest', 'zero',
                                                               'slinear', 'quadratic', 'cubic'],
                                                      name='Interpolation method',
                                                      info='GPS point interpolation method.',
                                                      ))

        self.color_point = attrsman.add(cm.AttrConf('color_point', kwargs.get('color_point', np.array([0.0, 0.4, 0.6, 0.6], np.float32)),
                                                    groupnames=['options'],
                                                    perm='wr',
                                                    metatype='color',
                                                    name='Point color',
                                                    info='Color of GPS-points.',
                                                    ))

        self.size_point = attrsman.add(cm.AttrConf('size_point', kwargs.get('size_point', 10.0),
                                                   groupnames=['options'],
                                                   name='Point size',
                                                   info='Point size of GPS points.',
                                                   ))

        self.is_pointlabel = attrsman.add(cm.AttrConf('is_pointlabel', kwargs.get('is_pointlabel', True),
                                                      groupnames=['options'],
                                                      name='Label points?',
                                                      info='If True, shows point ID nex to each point.',
                                                      ))

        self.is_waitslabel = attrsman.add(cm.AttrConf('is_waitslabel', kwargs.get('is_waitslabel', True),
                                                      groupnames=['options'],
                                                      name='Show wait times?',
                                                      info='If True, shows wait times for each edge.',
                                                      ))

        self.is_waitslabel_junction = attrsman.add(cm.AttrConf('is_waitslabel_junction', kwargs.get('is_waitslabel_junction', True),
                                                               groupnames=['options'],
                                                               name='Show wait times at junctions?',
                                                               info='If True, shows wait times at junctions for each edge entering a junction.',
                                                               ))

        self.is_waitslabel_tls = attrsman.add(cm.AttrConf('is_waitslabel_tls', kwargs.get('is_waitslabel_tls', True),
                                                          groupnames=['options'],
                                                          name='Show wait times at TLS?',
                                                          info='If True, shows wait times at traffic light systems (TLS) for each edge entering a traffic light.',
                                                          ))

        self.color_line = attrsman.add(cm.AttrConf('color_line', kwargs.get('color_line', np.array([1.0, 0.6, 0.0, 1.0], np.float32)),
                                                   groupnames=['options'],
                                                   perm='wr',
                                                   metatype='color',
                                                   name='Line color',
                                                   info='Color of plotted line in diagram.',
                                                   ))

        self.width_line = attrsman.add(cm.AttrConf('width_line', kwargs.get('width_line', 3.0),
                                                   groupnames=['options'],
                                                   name='Line width',
                                                   info='Line width of plot.',
                                                   ))

        self.alpha_line = attrsman.add(cm.AttrConf('alpha_line', kwargs.get('alpha_line', 0.8),
                                                   groupnames=['options'],
                                                   name='Line transp.',
                                                   info='Line transparency of plot.',
                                                   ))

        self.is_grid = attrsman.add(cm.AttrConf('is_grid', kwargs.get('is_grid', True),
                                                groupnames=['options'],
                                                name='Show grid?',
                                                info='If True, shows a grid on the graphical representation.',
                                                ))

        self.titletext = attrsman.add(cm.AttrConf('titletext', kwargs.get('titletext', ''),
                                                  groupnames=['options'],
                                                  name='Title text',
                                                  info='Title text. Empty text means no title.',
                                                  ))

        self.size_titlefont = attrsman.add(cm.AttrConf('size_titlefont', kwargs.get('size_titlefont', 32),
                                                       groupnames=['options'],
                                                       name='Title fontsize',
                                                       info='Title fontsize.',
                                                       ))

        self.size_labelfont = attrsman.add(cm.AttrConf('size_labelfont', kwargs.get('size_labelfont', 24),
                                                       groupnames=['options'],
                                                       name='Label fontsize',
                                                       info='Label fontsize.',
                                                       ))
        self.add_save_options()

    def plot_speed_over_time(self, ax, id_trip, id_route, edges, i_min=None, i_max=None,
                             is_pointlabel=True, alpha=1.0):
        print 'plot_speed_over_time', id_trip, type(id_trip), self.parent.parent

        #mapmatching = self.parent.parent
        #trips = mapmatching.trips

        routeresults = self.get_routeresults()

        #id_route = trips.ids_route_matched[id_trip]
        id_routeres = routeresults.ids_route.get_id_from_index(id_route)

        mapmatching = self.parent.parent
        trips = mapmatching.trips
        ids_point = routeresults.ids_valid_point_speedana[id_routeres]

        # tripresults.pointsposition[id_routeres],\
        # tripresults.pointsspeed[id_routeres],
        # tripresults.pointstime[id_routeres],
        # ids_pointedges,

        if i_min is None:
            #ids_pointedge = routeresults.ids_pointedges[id_routeres]
            #pointspositions = routeresults.pointspositions[id_routeres]
            speeds = routeresults.speedana_point_speeds[id_routeres]
            times = routeresults.speedana_point_times[id_routeres]
        else:
            #ids_pointedge = routeresults.ids_pointedges[id_routeres][i_min:i_max]
            #pointspositions = routeresults.pointspositions[id_routeres][i_min:i_max]
            speeds = routeresults.speedana_point_speeds[id_routeres][i_min:i_max]
            times = routeresults.speedana_point_times[id_routeres][i_min:i_max]
        # print '  id_route,id_routeres',id_route,id_routeres
        # print '  ids_pointedge',ids_pointedge
        n_point = len(times)

        x = np.array(times, dtype=np.float32)
        y = np.array(speeds, dtype=np.float32)*3.6  # in km/h
        #ax = init_plot()
        # print '  x',x
        # print '  y',y
        #ax.plot(locations, speeds, color = self.color_line[:2], lw = self.width_line ,alpha=0.9 ,zorder = 0)

        alpha = min(alpha, self.alpha_line)

        if is_scipy & (not (self.method_interp == 'linear')):

            print 'use scipy to interpolate'
            #tck = interpolate.splrep(x, y, s=0)
            #xnew = np.linspace(np.min(x), np.max(x), 200)
            #ynew = interpolate.splev(xnew, tck, der=0)
            # if 1:
            f_inter = interpolate.interp1d(x, y, kind=self.method_interp)
            xnew = np.linspace(np.min(x), np.max(x), 200)
            ynew = f_inter(xnew)

            ax.plot(xnew, ynew, color=self.color_line, lw=self.width_line, alpha=alpha)
            ax.plot(x, y, 'o', markersize=self.size_point, color=self.color_point, alpha=alpha)
        else:
            ax.plot(x, y, 'o-', markersize=self.size_point, color=self.color_line,
                    lw=self.width_line, markerfacecolor=self.color_point, alpha=alpha)

        # label points
        if self.is_pointlabel & is_pointlabel:
            for id_point,  xi, yi in zip(ids_point, x, y):

                ax.text(xi+2, yi, ' %s ' % (str(id_point)),
                        verticalalignment='top',
                        horizontalalignment='left',
                        #rotation = 'vertical',
                        fontsize=int(0.8*self.size_labelfont))  # baseline

    def plot_speed_over_way(self, ax, id_trip, id_route, edges, i_min=None, i_max=None,
                            is_pointlabel=True, alpha=1.0):
        print 'plot_speed_over_way', id_trip, type(id_trip), self.parent.parent

        #mapmatching = self.parent.parent
        #trips = mapmatching.trips

        routeresults = self.get_routeresults()

        #id_route = trips.ids_route_matched[id_trip]
        id_routeres = routeresults.ids_route.get_id_from_index(id_route)

        mapmatching = self.parent.parent
        trips = mapmatching.trips
        ids_point = routeresults.ids_valid_point_speedana[id_routeres]

        # tripresults.pointsposition[id_routeres],\
        # tripresults.pointsspeed[id_routeres],
        # tripresults.pointstime[id_routeres],
        # ids_pointedges,

        if i_min is None:
            ids_pointedge = routeresults.ids_pointedges[id_routeres]
            pointspositions = routeresults.speedana_point_pos[id_routeres]
            speeds = routeresults.speedana_point_speeds[id_routeres]
            times = routeresults.speedana_point_times[id_routeres]
        else:
            ids_pointedge = routeresults.ids_pointedges[id_routeres][i_min:i_max]
            pointspositions = routeresults.speedana_point_pos[id_routeres][i_min:i_max]
            speeds = routeresults.speedana_point_speeds[id_routeres][i_min:i_max]
            times = routeresults.speedana_point_times[id_routeres][i_min:i_max]
        # print '  id_route,id_routeres',id_route,id_routeres
        # print '  ids_pointedge',ids_pointedge
        n_point = len(ids_pointedge)

        alpha = min(alpha, self.alpha_line)
        x = pointspositions
        y = np.array(speeds, dtype=np.float32)*3.6  # in km/h

        #ax = init_plot()
        print '  ids_point', routeresults.ids_valid_point_speedana[id_routeres]
        print '  position', pointspositions
        print '  x', x
        print '  y', y
        #ax.plot(locations, speeds, color = self.color_line[:2], lw = self.width_line ,alpha=0.9 ,zorder = 0)

        if is_scipy & (not (self.method_interp == 'linear')):
            #tck = interpolate.splrep(x, y, s=0)
            #xnew = np.linspace(np.min(x), np.max(x), 200)
            #ynew = interpolate.splev(xnew, tck, der=0)
            # if 1:
            f_inter = interpolate.interp1d(x, y, kind=self.method_interp)
            xnew = np.linspace(np.min(x), np.max(x), 200)
            ynew = f_inter(xnew)

            ax.plot(xnew, ynew, color=self.color_line, lw=self.width_line, alpha=alpha)
            ax.plot(x, y, 'o', markersize=self.size_point, color=self.color_point, alpha=alpha)
        else:
            ax.plot(x, y, 'o-', markersize=self.size_point, color=self.color_line,
                    lw=self.width_line, markerfacecolor=self.color_point, alpha=alpha)
        if self.is_pointlabel & is_pointlabel:
            # label points
            for id_point,  xi, yi in zip(ids_point, x, y):

                ax.text(xi+2, yi, ' %s ' % (str(id_point)),
                        verticalalignment='top',
                        horizontalalignment='left',
                        #rotation = 'vertical',
                        fontsize=int(0.8*self.size_labelfont))  # baseline

    def show(self):
        print 'show', self.id_trip, type(self.id_trip), self.parent.parent
        # if self.axis  is None:

        if self.id_trip >= 0:
            id_trip = self.id_trip
            mapmatching = self.parent.parent
            trips = mapmatching.trips
            #points = mapmatching.points
            routes = trips.get_routes()
            scenario = mapmatching.get_scenario()
            edges = scenario.net.edges
            nodes = scenario.net.nodes

            routeresults = self.get_routeresults()

            if id_trip in trips:
                id_route = trips.ids_route_matched[id_trip]
            else:
                return False

            route = routes.ids_edges[id_route]

            if routeresults.ids_route.has_index(id_route):
                id_routeres = routeresults.ids_route.get_id_from_index(id_route)
            else:
                return False

            edgesresults = self.parent.edgesresults
            connectionsresults = self.parent.connectionsresults

            # tripresults.pointsposition[id_routeres],\
            # tripresults.pointsspeed[id_routeres],
            # tripresults.pointstime[id_routeres],
            # ids_pointedges,
            self.init_figures()
            fig = self.create_figure()
            ax = fig.add_subplot(111)

            fig2 = self.create_figure()
            ax2 = fig2.add_subplot(111)

            self.plot_speed_over_way(ax, id_trip, id_route, edges)
            self.plot_speed_over_time(ax2, id_trip, id_route, edges)

            # get_color()
            # is_sublist
            #id_route = trips.ids_route_matched[id_trip]
            #route = routes.ids_edge[id_route]
            ids_pointedge = routeresults.ids_pointedges[id_routeres]
            if self.is_plot_similar_trips:
                #id_routeres = routeresults.ids_route.get_ids_from_indices(ids_route)

                id_pointedge_first = ids_pointedge[0]
                id_pointedge_last = ids_pointedge[-1]

                ids_routeres_speed = routeresults.get_ids()
                ids_route_speed = routeresults.ids_route[ids_routeres_speed]
                # print '  route',route
                for id_trip_speed, id_route_speed, route_speed, ids_pointedge_speed in zip(
                        routes.ids_trip[ids_route_speed],
                        ids_route_speed, routes.ids_edges[ids_route_speed],
                        routeresults.ids_pointedges[ids_routeres_speed]):
                    # print '    ids_pointedge_speed',ids_pointedge_speed
                    # print '    route[0],is_inlist',route[0],ids_pointedge_speed.count(route[0]),type(ids_pointedge_speed)
                    # print '    route_speed',route_speed

                    # is_sublist(route,route_speed):# |  is_sublist(route_speed,route):
                    if is_sublist(route_speed, route):
                        i = ids_pointedge_speed.index(id_pointedge_first)
                        j = ids_pointedge_speed.index(id_pointedge_last)
                        n_pointedge = len(ids_pointedge_speed)
                        while (ids_pointedge_speed[j] == id_pointedge_last) & (j < n_pointedge-1):
                            j += 1
                        self.plot_speed_over_way(ax, id_trip_speed, id_route_speed, edges,
                                                 i, j, is_pointlabel=False, alpha=0.5)
                        self.plot_speed_over_time(ax2, id_trip_speed, id_route_speed, edges,
                                                  i, j, is_pointlabel=False, alpha=0.5)

            # plot edge info to speed over time

            colors = [(0.2, 0.2, 0.2, 0.7), (0.8, 0.8, 0.8, 0.7)]
            ymin, ymax = ax.get_ylim()
            x = 0
            x_last = routeresults.speedana_point_pos[id_routeres][0]
            t_last = 0
            i = 0
##            id_arc_last = routeresults.ids_arc_point[id_routeres][0]
            id_arc_last = 0
            i_point = 0
            pointstime = routeresults.speedana_point_times[id_routeres]
            print '  len(ids_pointedge)', len(ids_pointedge)
            print '  len(pointstime)', len(pointstime)

            # if len(pointstime)>1:
            #    t_last = pointstime[1]
            # else:
            #t_last_edge = pointstime[0]
            #t_last = pointstime[0]
            # print '  ids_pointedge\n',ids_pointedge
            # print '  pointstime\n',pointstime
            # print '  pointsspeeds\n',routeresults.pointsspeeds[id_routeres]

            linestyle = '--'

            for id_arc_point, is_connection_point, t, v, pos in zip(
                routeresults.ids_arc_point[id_routeres],
                routeresults.is_connection_point[id_routeres],
                pointstime,
                routeresults.speedana_point_speeds[id_routeres],
                routeresults.speedana_point_pos[id_routeres],
            ):

                print '  id_arc_point', id_arc_point, 'is_connection_point', is_connection_point, 'id_arc_last', id_arc_last, id_arc_point != id_arc_last, 'pos=%.2fm, t=%.2fs, v=%.2fkm/h' % (pos, t, v*3.6),
                color = 'k'
                if (id_arc_point != id_arc_last):

                    t_last = t  # memorize t where edge ids changed
                    if not is_connection_point:
                        # edges have changed
                        # if id_edge != -1:
                        #    x += edges.lengths[id_edge] # add length of old id_edge

                        ax2.text(t_last+2, ymax, ' ID Edge = %s ' % (str(id_arc_point)),
                                 verticalalignment='top',
                                 horizontalalignment='left',
                                 rotation='vertical',
                                 fontsize=int(0.8*self.size_labelfont))  # baseline

                        # put wait times, if any available
                        if edgesresults.ids_edge.has_index(id_arc_point):
                            id_edgeres = edgesresults.ids_edge.get_id_from_index(id_arc_point)
                            time_wait = edgesresults.times_wait[id_edgeres]
                            time_wait_junc = edgesresults.times_wait_junc[id_edgeres]
                            time_wait_tls = edgesresults.times_wait_tls[id_edgeres]
                            label = r'  '
                            # is_waitslabel,is_waitslabel_junction,is_waitslabel_tls
                            if self.is_waitslabel & (time_wait > 0):
                                label += ' $T_W=%ds$' % time_wait

                            if self.is_waitslabel_junction & (time_wait_junc > 0):
                                label += ' $T_J=%ds$' % time_wait_junc

                            if self.is_waitslabel_tls & (time_wait_tls > 0):
                                label += ' $T_{\mathrm{TL}}=%ds$' % time_wait_tls

                            ax2.text(t_last+2, ymin, label,
                                     verticalalignment='bottom',
                                     horizontalalignment='left',
                                     rotation='vertical',
                                     color=color,
                                     fontsize=int(0.8*self.size_labelfont))

                        ax2.plot([t_last, t_last], [ymin, ymax], color=color, linestyle=linestyle)
                        t_last = t  # memorize t where edge ids changed

                    else:
                        # edges have changed
                        # if id_edge != -1:
                        #    x += edges.lengths[id_edge] # add length of old id_edge

                        ax2.text(t_last+2, ymax, ' ID Connection = %s ' % (str(id_arc_point)),
                                 verticalalignment='top',
                                 horizontalalignment='left',
                                 rotation='vertical',
                                 fontsize=int(0.8*self.size_labelfont))  # baseline

                        # put wait times, if any available
                        if connectionsresults.ids_connection.has_index(id_arc_point):
                            id_connectionres = connectionsresults.ids_connection.get_id_from_index(id_arc_point)
                            time_wait = connectionsresults.times_wait[id_connectionres]
##                            time_wait_junc = connectionsresults.times_wait_junc[id_edgeres]
                            time_wait_tls = connectionsresults.times_wait_tls[id_connectionres]
                            label = r'  '
                            # is_waitslabel,is_waitslabel_junction,is_waitslabel_tls
                            if self.is_waitslabel & (time_wait > 0):
                                label += ' $T_W=%ds$' % time_wait

##
# if self.is_waitslabel_junction & (time_wait_junc>0):
##                                label += ' $T_J=%ds$'%time_wait_junc
##

                            if self.is_waitslabel_tls & (time_wait_tls > 0):
                                label += ' $T_{\mathrm{TL}}=%ds$' % time_wait_tls

                            ax2.text(t_last+2, ymin, label,
                                     verticalalignment='bottom',
                                     horizontalalignment='left',
                                     rotation='vertical',
                                     color=color,
                                     fontsize=int(0.8*self.size_labelfont))

                        ax2.plot([t_last, t_last], [ymin, ymax], color=color, linestyle=linestyle)

                id_arc_last = id_arc_point  # memorize edge of last point

                i_point += 1

            #ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
            ax2.grid(self.is_grid)
            if self.titletext != '':
                ax2.set_title(self.titletext, fontsize=self.size_titlefont)

            ax2.set_xlabel('Time [s]', fontsize=self.size_labelfont)
            ax2.set_ylabel('Speed [km/h]', fontsize=self.size_labelfont)
            ax2.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
            ax2.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

            # plot edge info to speed over way graph
            # this will put correct edge borders and labels
            linestyle = '--'
            color = 'k'
            cumulative_dists = routeresults.cumulative_dists[id_routeres]
            cumulative_dists = np.delete(cumulative_dists, -1)
            cumulative_dists = np.insert(cumulative_dists, 0, 0.).tolist()
            ids_arc = routeresults.ids_arc[id_routeres]
            are_connections = routeresults.is_connection[id_routeres]
            print cumulative_dists, ids_arc

            for cumulative_dist, id_arc, is_connection in zip(cumulative_dists, ids_arc, are_connections):

                #ax.plot([x,x+length],[ymin,ymin],color = colors[i%2],lw = 3*self.width_line)

                # ax.text( dist, ymax, ' ID= %s '%(str(id_arc)),
                ##                                        verticalalignment = 'top',
                ##                                        horizontalalignment = 'left',
                ##                                        rotation = 'vertical',
                # fontsize = int(0.8*self.size_labelfont))#baseline

                if not is_connection:
                    if edgesresults.ids_edge.has_index(id_arc):
                        id_edgeres = edgesresults.ids_edge.get_id_from_index(id_arc)
                        time_wait = edgesresults.times_wait[id_edgeres]
                        time_wait_junc = edgesresults.times_wait_junc[id_edgeres]
                        time_wait_tls = edgesresults.times_wait_tls[id_edgeres]
                        label = r'  '

                        if self.is_waitslabel & (time_wait > 0):
                            label += ' $T_W=%ds$' % time_wait

                        if self.is_waitslabel_junction & (time_wait_junc > 0):
                            label += ' $T_J=%ds$' % time_wait_junc

                        if self.is_waitslabel_tls & (time_wait_tls > 0):
                            label += ' $T_{\mathrm{TL}}=%ds$' % time_wait_tls

                        print '  id_edge', id_arc, 'pos=%df' % cumulative_dist, 'time_wait', time_wait, 'time_wait_junc', time_wait_junc, 'time_wait_tls', time_wait_tls

                        ax.text(cumulative_dist, ymin, label,
                                verticalalignment='bottom',
                                horizontalalignment='left',
                                rotation='vertical',
                                color=color,
                                fontsize=int(0.8*self.size_labelfont))
                    else:
                        print 'the edge', id_arc, 'is not in the edgeresult database'
                else:
                    if connectionsresults.ids_connection.has_index(id_arc):
                        id_connectionres = connectionsresults.ids_connection.get_id_from_index(id_arc)
                        time_wait = connectionsresults.times_wait[id_connectionres]
    ##                        time_wait_junc = connectionsresults.times_wait_junc[id_connectionres]
                        time_wait_tls = connectionsresults.times_wait_tls[id_connectionres]
                        label = r'  '

                        if self.is_waitslabel & (time_wait > 0):
                            label += ' $T_W=%ds$' % time_wait

    # if self.is_waitslabel_junction & (time_wait_junc>0):
    ##                            label += ' $T_J=%ds$'%time_wait_junc

                        if self.is_waitslabel_tls & (time_wait_tls > 0):
                            label += ' $T_{\mathrm{TL}}=%ds$' % time_wait_tls

                        print '  id_connection', id_arc, 'pos=%df' % x, 'time_wait', time_wait, 'time_wait_junc', time_wait_junc, 'time_wait_tls', time_wait_tls

                        ax.text(cumulative_dist, ymin, label,
                                verticalalignment='bottom',
                                horizontalalignment='left',
                                rotation='vertical',
                                color=color,
                                fontsize=int(0.8*self.size_labelfont))
                    else:
                        print 'the connection', id_arc, 'is not in the connectionresult database'

                ax.plot([cumulative_dist, cumulative_dist], [ymin, ymax], color=color, linestyle=linestyle)

                if not is_connection:
                    ax.text(cumulative_dist, ymax, ' ID Edge= %s ' % (str(id_arc)),
                            verticalalignment='top',
                            horizontalalignment='left',
                            rotation='vertical',
                            fontsize=int(0.8*self.size_labelfont))  # baseline
                else:
                    ax.text(cumulative_dist, ymax, ' ID Connection= %s ' % (str(id_arc)),
                            verticalalignment='top',
                            horizontalalignment='left',
                            rotation='vertical',
                            fontsize=int(0.8*self.size_labelfont))  # baseline
##                    x += edges.lengths[id_edge]

            #ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
            ax.grid(self.is_grid)
            if self.titletext != '':
                ax.set_title(self.titletext, fontsize=self.size_titlefont)

            ax.set_xlabel('Distance [m]', fontsize=self.size_labelfont)
            ax.set_ylabel('Speed [km/h]', fontsize=self.size_labelfont)
            ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
            ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

            # if self.is_save:
            #    self.save_fig('routeana_speedprofile')

            plt.show()
            # show_plot()

    def get_routeresults(self):
        return self.parent.routesresults_matched

    def get_scenario(self):
        return self.parent.get_scenario()


class EdgeresultPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Plot edge results with Matplotlib',
                 info="Creates plots of different edge results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('routeresultplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()

        self.plotthemefuncs = OrderedDict([
            ('average slopes', self.plot_average_slopes),
            ('positive climbs', self.plot_positive_climbs),
            ('negative climbs', self.plot_negative_climbs),
            ('average speeds', self.plot_speeds_average),
            ('inmove speeds', self.plot_speeds_inmotion),
            ('times wait', self.plot_times_wait),
            ('times wait_tls', self.plot_times_wait_tls),
            ('number matched_routes', self.plot_numbers_tot_matched),
            ('number shortest', self.plot_numbers_tot_shortest),
            ('total deviation', self.plot_differences_dist_tot_shortest),
            ('relative deviation', self.plot_differences_dist_rel_shortest),
            ('usage probabilities by matched routes', self.plot_probabilities_tot_matched),
            ('estimated flows from matched routes', self.plot_flows_est_matched_routes),
        ])
        self.plottheme = attrsman.add(cm.AttrConf('plottheme', kwargs.get('plottheme', 'average speeds'),
                                                  groupnames=['options'],
                                                  choices=self.plotthemefuncs.keys(),
                                                  name='Plot theme',
                                                  info='Theme  or edge attribute to be plottet.',
                                                  ))
        self.n_min_matched = attrsman.add(cm.AttrConf('n_min_matched', 3,
                                                      groupnames=['options'],
                                                      name='Minum number of matched for speed analysis',
                                                      info='Only edge contained in almost this number of matched routes\
                                        will be considered for plotting the dynamic\
                                        characteristics of edges (speeds and times).',
                                                      ))

        self.add_plotoptions(**kwargs)
        self.add_save_options(**kwargs)

    def plot_all_themes(self):
        for plottheme in self.plotthemefuncs.keys():
            self.plottheme = plottheme
            self.show()

    def show(self):
        print 'EdgeresultPlotter.show', self.plottheme
        # if self.axis  is None:
        #axis = init_plot()
        self.init_figures()
        fig = self.create_figure()
        axis = fig.add_subplot(111)
        self.plotthemefuncs[self.plottheme](axis)

        print '  self.is_save', self.is_save
        if not self.is_save:
            print '  show_plot'
            show_plot()
        else:
            figname = 'edgeplot_'+self.plottheme
            # print '  savefig',figname

            # self.save_fig('edgeplot_'+self.plottheme)

            rootfilepath = self.get_scenario().get_rootfilepath()

            fig.savefig("%s_%s.%s" % (rootfilepath, figname, self.figformat),
                        format=self.figformat,
                        dpi=self.resolution,
                        # orientation='landscape',
                        orientation='portrait',
                        transparent=True)
            plt.close(fig)

    def get_edgeresults(self):
        return self.parent.edgesresults  # must have attribute 'ids_edge'

    def plot_average_slopes(self, ax):

        edges = self.parent.parent.parent.parent.net.edges
        ids_edge = edges.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        average_slopes = edges.average_slopes
        print ids_edge, average_slopes[ids_edge]
        self.plot_results_on_map(ax,
                                 values=average_slopes[ids_edge],
                                 ids_edge=ids_edge,
                                 title='Average edge slopes',
                                 valuelabel='Slope',
                                 )

    def plot_positive_climbs(self, ax):

        edges = self.parent.parent.parent.parent.net.edges
        ids_edge = edges.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        positive_climbs = edges.positive_climbs
        print ids_edge, positive_climbs[ids_edge]
        self.plot_results_on_map(ax,
                                 values=positive_climbs[ids_edge],
                                 ids_edge=ids_edge,
                                 title='Average edge slopes',
                                 valuelabel='Slope',
                                 )

    def plot_negative_climbs(self, ax):

        edges = self.parent.parent.parent.parent.net.edges
        ids_edge = edges.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        negative_climbs = edges.negative_climbs
        print ids_edge, negative_climbs[ids_edge]
        self.plot_results_on_map(ax,
                                 values=negative_climbs[ids_edge],
                                 ids_edge=ids_edge,
                                 title='Average edge slopes',
                                 valuelabel='Slope',
                                 )

    def plot_differences_dist_rel_shortest(self, ax):
        edgesresults = self.get_edgeresults()
        ids_result = edgesresults.select_ids(edgesresults.differences_dist_rel_shortest.get_value() > 0)

        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)

        self.plot_results_on_map(ax, ids_result,
                                 # edgesresults.differences_dist_tot_shortest[ids_result]/edgesresults.numbers_tot_shortest[ids_result],
                                 edgesresults.differences_dist_rel_shortest[ids_result],
                                 title='Deviation generated per trip',
                                 valuelabel='Generated deviation per trip [m]',
                                 )

    def plot_differences_dist_tot_shortest(self, ax):
        edgesresults = self.get_edgeresults()
        ids_result = edgesresults.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        #deviation_tot = edgesresults.differences_dist_tot_shortest
        self.plot_results_on_map(ax, ids_result,
                                 edgesresults.differences_dist_tot_shortest[ids_result]/1000,
                                 # deviation_tot[ids_result]/1000,
                                 title='Total deviation generated per edge',
                                 valuelabel='Generated total deviation [km]',
                                 )

    def plot_numbers_tot_shortest(self, ax):
        edgesresults = self.get_edgeresults()
        ids_result = edgesresults.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        numbers_tot_shortest = edgesresults.numbers_tot_shortest
        self.plot_results_on_map(ax, ids_result,
                                 numbers_tot_shortest[ids_result],
                                 title='Edge usage from shortest routes',
                                 valuelabel='Usage in number of persons',
                                 )

    def plot_numbers_tot_matched(self, ax):
        edgesresults = self.get_edgeresults()
        ids_result = edgesresults.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        numbers_tot_matched = edgesresults.numbers_tot_matched
        self.plot_results_on_map(ax, ids_result,
                                 numbers_tot_matched[ids_result],
                                 title='Edge usage from matched routes',
                                 valuelabel='Usage in number of persons',
                                 )

    def plot_speeds_average(self, ax):
        edgesresults = self.parent.edgesresults

        print 'plot_speeds_average'

        #ids_result = edgesresults.get_ids()
        ids_result = edgesresults.select_ids(
            edgesresults.numbers_matched_for_speed_analysis.get_value() > self.n_min_matched)

        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        speeds_average = self.parent.edgesresults.speed_average
        # print '  speeds_average',speeds_average[ids_result]
        # print '  ids_result',ids_result

        self.plot_results_on_map(ax, ids_result,
                                 speeds_average[ids_result]*3.6,
                                 title='Average edge speeds',
                                 valuelabel='Average edge speeds [km/h]',
                                 )

    def plot_speeds_inmotion(self, ax):
        edgesresults = self.parent.edgesresults
        #ids_result = edgesresults.get_ids()
        ids_result = edgesresults.select_ids(
            edgesresults.numbers_matched_for_speed_analysis.get_value() > self.n_min_matched)

        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        speeds = self.parent.edgesresults.speed_average_in_motion

        # print '  speeds_average',speeds[ids_result]
        # print '  ids_result',ids_result

        self.plot_results_on_map(ax, ids_result,
                                 speeds[ids_result]*3.6,
                                 title='Average edge speeds in motion',
                                 valuelabel='Average edge speeds in motion [km/h]',
                                 )

    def plot_times_wait(self, ax):
        edgesresults = self.parent.edgesresults
        #ids_result = edgesresults.get_ids()
        ids_result = edgesresults.select_ids(
            edgesresults.numbers_matched_for_speed_analysis.get_value() > self.n_min_matched)

        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        times = edgesresults.times_wait
        self.plot_results_on_map(ax, ids_result,
                                 times[ids_result],
                                 title='Average wait times',
                                 valuelabel='Average wait times [s]',
                                 )

    def plot_times_wait_tls(self, ax):
        #ids_result = self.parent.edgesresults.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        edgesresults = self.parent.edgesresults
        ids_result = edgesresults.select_ids(
            edgesresults.numbers_matched_for_speed_analysis.get_value() > self.n_min_matched)
        times = edgesresults.times_wait_tls
        self.plot_results_on_map(ax, ids_result,
                                 times[ids_result],
                                 title='Average wait times at Traffic Lights',
                                 valuelabel='Average wait times at TLS [s]',
                                 )

    def plot_probabilities_tot_matched(self, ax):
        ids_result = self.parent.edgesresults.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        self.plot_results_on_map(ax, ids_result,
                                 self.parent.edgesresults.probabilities_tot_matched[ids_result],
                                 title='Probabilities',
                                 valuelabel=r'Enter probabilities [\%]',
                                 )

    def plot_flows_est_matched_routes(self, ax):
        ids_result = self.parent.edgesresults.get_ids()
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        self.plot_results_on_map(ax, ids_result,
                                 self.parent.edgesresults.flows_est[ids_result],
                                 title='Flows',
                                 valuelabel=r'Estimated flows [1/h]',
                                 )

    def do(self):
        # print 'do',self.edgeattrname
        self.show()
        return True


class ConnectionresultPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Plot connection results with Matplotlib',
                 info="Creates plots of different connection results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('routeresultplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()

        self.plotthemefuncs = OrderedDict([
            ('times wait', self.plot_times_wait),
            ('times wait_tls', self.plot_times_wait_tls),
        ])
        self.plottheme = attrsman.add(cm.AttrConf('plottheme', kwargs.get('plottheme', 'times wait'),
                                                  groupnames=['options'],
                                                  choices=self.plotthemefuncs.keys(),
                                                  name='Plot theme',
                                                  info='Theme  or edge attribute to be plottet.',
                                                  ))

        self.n_min_matched = attrsman.add(cm.AttrConf('n_min_matched', 3,
                                                      groupnames=['options'],
                                                      name='Minum number of matched for speed analysis',
                                                      info='Only connectors contained in almost this number of matched routes\
                                        will be considered for plotting the dynamic\
                                        characteristics of edges (speeds and times).'))

        self.add_plotoptions(**kwargs)
        self.add_save_options(**kwargs)

    def plot_all_themes(self):
        for plottheme in self.plotthemefuncs.keys():
            self.plottheme = plottheme
            self.show()

    def show(self):
        print 'connectionresultPlotter.show', self.plottheme
        # if self.axis  is None:
        #axis = init_plot()
        self.init_figures()
        fig = self.create_figure()
        axis = fig.add_subplot(111)
        self.plotthemefuncs[self.plottheme](axis)

        print '  self.is_save', self.is_save
        if not self.is_save:
            print '  show_plot'
            show_plot()
        else:
            figname = 'connectionplot_'+self.plottheme
            # print '  savefig',figname

            # self.save_fig('edgeplot_'+self.plottheme)

            rootfilepath = self.get_scenario().get_rootfilepath()

            fig.savefig("%s_%s.%s" % (rootfilepath, figname, self.figformat),
                        format=self.figformat,
                        dpi=self.resolution,
                        # orientation='landscape',
                        orientation='portrait',
                        transparent=True)
            plt.close(fig)

    def get_connectionresults(self):
        return self.parent.connectionsresults

    def plot_times_wait(self, ax):

        connectionsresults = self.parent.connectionsresults
        #ids_result = edgesresults.get_ids()

        ids_result = connectionsresults.select_ids(
            connectionsresults.numbers_matched_for_speed_analysis.get_value() > self.n_min_matched)
        #resultattrconf = getattr(self.parent.edgesresults, self.edgeattrname)
        times = connectionsresults.times_wait
# print 'TIMES VALUES', times[ids_result]
        self.plot_results_on_map(ax,
                                 values=times[ids_result],
                                 title='Average wait times at connectors',
                                 valuelabel='Average wait times [s]',
                                 is_connection_results=True,
                                 ids_connectionres=ids_result)

    def plot_times_wait_tls(self, ax):

        connectionsresults = self.parent.connectionsresults
        ids_result = connectionsresults.select_ids(
            connectionsresults.numbers_matched_for_speed_analysis.get_value() > self.n_min_matched)
        times = connectionsresults.times_wait_tls
# print 'TIMES VALUES', times[ids_result]
        self.plot_results_on_map(ax,
                                 values=times[ids_result],
                                 title='Average wait times at tls connectors',
                                 valuelabel='Average wait times [s]',
                                 is_connection_results=True,
                                 ids_connectionres=ids_result)

    def do(self):
        # print 'do',self.edgeattrname
        self.show()
        return True


class AlternativeRoutesPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Plot edge of alternative routes with Matplotlib',
                 info="Creates plots of different edge results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('routeresultplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()

        self.color_chosen = attrsman.add(cm.AttrConf('color_chosen', kwargs.get('color_chosen', np.array([0.9, 0.2, 0.2, 0.99], dtype=np.float32)),
                                                     groupnames=['options'],
                                                     perm='wr',
                                                     metatype='color',
                                                     name='Color Alt. chosen',
                                                     info='Color of chosen alternative.',
                                                     ))

        self.color2 = attrsman.add(cm.AttrConf('color2', kwargs.get('color2', np.array([0.2, 0.9, 0.2, 0.99], dtype=np.float32)),
                                               groupnames=['options'],
                                               perm='wr',
                                               metatype='color',
                                               name='Color Alt. 2',
                                               info='Color of second alternative.',
                                               ))

        kwargs_fixed = {'plottype': 'polygons',
                        'is_widthvalue': True,
                        'is_colorvalue': False,  # colors assigned explicitely
                        'is_value_range': True,
                        'value_range_min': 0,
                        'value_range_max': 2,
                        'color_fill': np.array([0.9, 0.2, 0.2, 0.99], dtype=np.float32),
                        'printformat': '',
                        }
        kwargs.update(kwargs_fixed)

        self.add_plotoptions(**kwargs)
        self.add_save_options(**kwargs)

        # configure fixed options as privat (non visible for gui)
        attrsman = self.get_attrsman()
        for attrname in kwargs_fixed.keys():
            attrconf = attrsman.get_config(attrname)
            attrconf.add_groupnames(['_private'])

    def show(self):
        print 'AlternativeRoutesPlotter.show'
        # if self.axis  is None:
        #axis = init_plot()
        title = 'Route alternatives'
        valuelabel = ''
        self.init_figures()
        fig = self.create_figure()
        axis = fig.add_subplot(111)

        altroutesresults = self.parent.altroutesresults
        ids_res = altroutesresults.get_ids()
        net = self.parent.get_scenario().net
        edges = net.edges

        axis.set_axis_bgcolor(self.color_background)
        if self.is_show_network:
            plot_net(axis, net, color_edge=self.color_network, width_edge=2,
                     color_node=self.color_nodes, alpha=self.alpha_net)

        if self.is_show_facilities:
            facilities = self.parent.get_scenario().landuse.facilities
            plot_facilities(axis, facilities, color_building=self.color_facilities,
                            color_outline=self.color_borders,
                            width_line=2, alpha=self.alpha_facilities,
                            )
        if self.is_show_maps:
            plot_maps(axis, self.parent.get_scenario().landuse.maps, alpha=self.alpha_maps)

        # if self.is_value_range:
        #    value_range = (self.value_range_min, self.value_range_max)
        # else:
        value_range = (0, 2)

        for id_trip, id_choice, chosen, ids_edge_all in zip(
                altroutesresults.ids_trip[ids_res],
                altroutesresults.ids_alt[ids_res],
                altroutesresults.choices[ids_res],
                altroutesresults.ids_edges[ids_res]):
            # print '  id_trip',id_trip,'ids_edge',ids_edge_all
            if ids_edge_all is not None:
                if id_choice == 1:
                    ids_edge = ids_edge_all

                else:
                    ids_edge = ids_edge_all[1:-1]

                if chosen:
                    color = self.color_chosen  # use predefined color sor selected
                elif id_choice == 2:
                    color = self.color2
                else:
                    color = get_color(id_choice)

                plot_edgevalues_lines(axis,
                                      ids_edge=ids_edge,
                                      values=np.ones(len(ids_edge), dtype=np.float32)*(int(chosen)+1),
                                      edges=net.edges,
                                      width_max=self.resultwidth,
                                      alpha=self.alpha_results,
                                      printformat=self.printformat,
                                      color_outline=self.color_outline,
                                      color_fill=color,
                                      color_label=self.color_label,
                                      is_antialiased=True,
                                      is_fill=self.is_colorvalue,
                                      is_widthvalue=self.is_widthvalue,
                                      length_arrowhead=self.length_arrowhead,
                                      fontsize=self.size_labelfont,
                                      valuelabel=False,
                                      value_range=value_range,
                                      is_colorbar=False,
                                      )
            else:
                print 'WARNING in AlternativeRoutesPlotter.show ids_edge=', ids_edge_all, 'of id_trip', id_trip

        if self.is_show_title:
            axis.set_title(title, fontsize=self.size_titlefont)

        axis.axis('equal')
        # ax.legend(loc='best',shadow=True)

        axis.grid(self.is_grid)
        axis.set_xlabel('West-East [m]', fontsize=self.size_labelfont)
        axis.set_ylabel('South-North [m]', fontsize=self.size_labelfont)
        axis.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        axis.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        ## save or show
        print '  self.is_save', self.is_save
        if not self.is_save:
            print '  show_plot'
            show_plot()
        else:
            self.save_fig('altroutesplot')
            plt.close(fig)


class RouteresultPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Plot route results with Matplotlib',
                 info="Creates plots of different route results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('routeresultplotter', parent=results, name=name,
                          info=info, logger=logger)

        print 'Resultplotter.__init__', results, self.parent
        attrsman = self.get_attrsman()

        mapmatching = self.parent.parent
        scenario = mapmatching.get_scenario()
        self.zones = scenario.landuse.zones
        self.is_plot_bike_availability_ffbss_zone = attrsman.add(cm.AttrConf('is_plot_bike_availability_ffbss_zone', kwargs.get('is_plot_bike_availability_ffbss_zone', True),
                                                                             groupnames=['options'],
                                                                             name='Plot bike relative availability for FFBSS in a zone',
                                                                             info='Plot the variation of the bike availability in a day for a zone. Either 24 or 96 bins are recommended',
                                                                             ))

        if len(self.zones) > 0:

            self.zone_ffbss = attrsman.add(cm.AttrConf('zone_ffbss', kwargs.get('zone_ffbss', self.zones.ids_sumo[1]),
                                                       groupnames=['options'],
                                                       choices=self.zones.ids_sumo,
                                                       name='Zone for the FFBSS availability analysis',
                                                       info='Select the zone for the FFBSS availability analysis.',
                                                       ))

        self.is_analyze_bike_availability_ffbss_zones = attrsman.add(cm.AttrConf('is_analyze_bike_availability_ffbss_zones', kwargs.get('is_analyze_bike_availability_ffbss_zones', True),
                                                                                 groupnames=['options'],
                                                                                 name='Analize bike relative availability for FFBSS in all zones',
                                                                                 info='Plot the absolute and relative (for zones with almost n_bins trips) variation of the bike availability in a day for all zones. Either 24 or 96 bins are recommended',
                                                                                 ))

        self.is_plot_bike_availability_ffbss = attrsman.add(cm.AttrConf('is_plot_bike_availability_ffbss', kwargs.get('is_plot_bike_availability_ffbss', False),
                                                                        groupnames=['options'],
                                                                        name='Plot bike usage distribution',
                                                                        info='Plot the number of bikes that are doing a trip simultaneously in the day period. It require an high number of bins',
                                                                        ))

        self.is_plot_deptimedistrib = attrsman.add(cm.AttrConf('is_plot_deptimedistrib', kwargs.get('is_plot_deptimedistrib', False),
                                                               groupnames=['options'],
                                                               name='Plot departure time distribution',
                                                               info='Plot cumulative distribution on trip departure time of matched route.',
                                                               ))

        # comprison matched shortest
        self.is_plot_lengthdistrib = attrsman.add(cm.AttrConf('is_plot_lengthdistrib', kwargs.get('is_plot_lengthdistrib', True),
                                                              groupnames=['options'],
                                                              name='Plot length distribution',
                                                              info='Plot cumulative distribution on length of matched route and shortest route.',
                                                              ))

        self.is_plot_lengthprob = attrsman.add(cm.AttrConf('is_plot_lengthprob', kwargs.get('is_plot_lengthprob', False),
                                                           groupnames=['options'],
                                                           name='Plot length probabilities',
                                                           info='Plot probabilities length of matched route and shortest route.',
                                                           ))

        self.is_plot_lengthdistrib_by_class = attrsman.add(cm.AttrConf('is_plot_lengthdistrib_by_class', kwargs.get('is_plot_lengthdistrib_by_class', False),
                                                                       groupnames=['options'],
                                                                       name='Plot class length distribution',
                                                                       info='Plot mean values of length of matched route and shortest route for different trip length classes.',
                                                                       ))

        self.distance_class = attrsman.add(cm.AttrConf('distance_class', kwargs.get('distance_class', 2000),
                                                       groupnames=['options'],
                                                       name='Class distance',
                                                       info='Distance to generate trip length classes.',
                                                       ))

        self.is_plot_lengthratio = attrsman.add(cm.AttrConf('is_plot_lengthratio', kwargs.get('is_plot_lengthratio', False),
                                                            groupnames=['options'],
                                                            name='Plot length ratio',
                                                            info='Plot  cumulative distribution  on length ratio between shortest route and matched route.',
                                                            ))

        self.is_plot_lengthoverlap = attrsman.add(cm.AttrConf('is_plot_lengthoverlap', kwargs.get('is_plot_lengthoverlap', False),
                                                              groupnames=['options'],
                                                              name='Plot overlap with shortest',
                                                              info='Plot  cumulative distribution on the realtive length overlap between shortest route and matched route.',
                                                              ))

        self.is_plot_lengthoverlap_fastest = attrsman.add(cm.AttrConf('is_plot_lengthoverlap_fastest', kwargs.get('is_plot_lengthoverlap_fastest', False),
                                                                      groupnames=['options'],
                                                                      name='Plot overlap with fastest',
                                                                      info='Plot  cumulative distribution  on on the realtive length  between fastest route and matched route.',
                                                                      ))

        self.is_plot_mixshare = attrsman.add(cm.AttrConf('is_plot_mixshare', kwargs.get('is_plot_mixshare', False),
                                                         groupnames=['options'],
                                                         name='Plot mixed share',
                                                         info='Plot  cumulative distribution of share of mixed access roads of shortest route and matched route.',
                                                         ))

        self.is_plot_exclusiveshare = attrsman.add(cm.AttrConf('is_plot_exclusiveshare', kwargs.get('is_plot_exclusiveshare', False),
                                                               groupnames=['options'],
                                                               name='Plot exclusive share',
                                                               info='Plot  cumulative distribution of share of exclusive access roads of shortest route and matched route.',
                                                               ))

        self.is_plot_lowpriorityshare = attrsman.add(cm.AttrConf('is_plot_lowpriorityshare', kwargs.get('is_plot_lowpriorityshare', False),
                                                                 groupnames=['options'],
                                                                 name='Plot low priority share',
                                                                 info='Plot  cumulative distribution of share of low priority roads of shortest route and matched route.',
                                                                 ))

        self.is_plot_nodesdensity = attrsman.add(cm.AttrConf('is_plot_nodesdensity', kwargs.get('is_plot_nodesdensity', False),
                                                             groupnames=['options'],
                                                             name='Plot node ratio',
                                                             info='Plot  cumulative distribution of node ratio between shortest route and matched route.',
                                                             ))

        self.is_plot_tldensity = attrsman.add(cm.AttrConf('is_plot_tldensity', kwargs.get('is_plot_tldensity', False),
                                                          groupnames=['options'],
                                                          name='Plot TL ratio',
                                                          info='Plot  cumulative distribution of traffic light ratio between shortest route and matched route.',
                                                          ))
        self.is_prioritychangedensity = attrsman.add(cm.AttrConf('is_prioritychangedensity', kwargs.get('is_prioritychangedensity', False),
                                                                 groupnames=['options'],
                                                                 name='Plot prio. change dens.',
                                                                 info='Plot  cumulative distribution of priority change denities between shortest route and matched route.',
                                                                 ))

        # comprison  non-overlapping matched and shortest
        self.is_plot_lengthratio_nonoverlap = attrsman.add(cm.AttrConf('is_plot_lengthratio_nonoverlap', kwargs.get('is_plot_lengthratio_nonoverlap', False),
                                                                       groupnames=['options'],
                                                                       name='Plot length ratio non-overlap',
                                                                       info='Plot cumulative distribution  on length ratio between non-overlapping parts of  shortest route and matched route.',
                                                                       ))

        self.is_plot_mixshare_nonoverlap = attrsman.add(cm.AttrConf('is_plot_mixshare_nonoverlap', kwargs.get('is_plot_mixshare_nonoverlap', False),
                                                                    groupnames=['options'],
                                                                    name='Plot mixed share non-overlap',
                                                                    info='Plot  cumulative distribution of share of mixed access roads of non-overlapping parts of shortest route and matched route.',
                                                                    ))

        self.is_plot_exclusiveshare_nonoverlap = attrsman.add(cm.AttrConf('is_plot_exclusiveshare_nonoverlap', kwargs.get('is_plot_exclusiveshare_nonoverlap', False),
                                                                          groupnames=['options'],
                                                                          name='Plot exclusive share non-overlap',
                                                                          info='Plot  cumulative distribution of share of exclusive access roads of non-overlapping parts of shortest route and matched route.',
                                                                          ))

        self.is_plot_lowpriorityshare_nonoverlap = attrsman.add(cm.AttrConf('is_plot_lowpriorityshare_nonoverlap', kwargs.get('is_plot_lowpriorityshare_nonoverlap', False),
                                                                            groupnames=['options'],
                                                                            name='Plot low priority share non-overlap',
                                                                            info='Plot  cumulative distribution of share of low priority roads of non-overlapping parts of shortest route and matched route.',
                                                                            ))

        self.is_plot_nodesdensity_nonoverlap = attrsman.add(cm.AttrConf('is_plot_nodesdensity_nonoverlap', kwargs.get('is_plot_nodesdensity_nonoverlap', False),
                                                                        groupnames=['options'],
                                                                        name='Plot node ratio non-overlap',
                                                                        info='Plot  cumulative distribution of node ratio between non-overlapping parts of shortest route and matched route.',
                                                                        ))

        self.is_plot_tldensity_nonoverlap = attrsman.add(cm.AttrConf('is_plot_tldensity_nonoverlap', kwargs.get('is_plot_tldensity_nonoverlap', False),
                                                                     groupnames=['options'],
                                                                     name='Plot TL ratio non-overlap',
                                                                     info='Plot  cumulative distribution of traffic light ratio between non-overlapping parts of shortest route and matched route.',
                                                                     ))

        self.is_prioritychangedensity_nonoverlap = attrsman.add(cm.AttrConf('is_prioritychangedensity_nonoverlap', kwargs.get('is_prioritychangedensity_nonoverlap', False),
                                                                            groupnames=['options'],
                                                                            name='Plot prio. change dens. non-overlap',
                                                                            info='Plot  cumulative distribution of priority change denities between non-overlapping parts of shortest route and matched route.',
                                                                            ))

        # other
        self.n_bins = attrsman.add(cm.AttrConf('n_bins', kwargs.get('n_bins', 10),
                                               groupnames=['options'],
                                               name='Bin number',
                                               info='Number of bins for histograms.',
                                               ))

        # self.add_plotoptions(**kwargs)
        self.is_title = attrsman.add(cm.AttrConf('is_title', kwargs.get('is_title', False),
                                                 groupnames=['options'],
                                                 name='Show title',
                                                 info='Show title of diagrams.',
                                                 ))

        self.title = attrsman.add(cm.AttrConf('title', kwargs.get('title', ''),
                                              groupnames=['options'],
                                              name='Title',
                                              info='Title text, if blank then default values are used. Only in combination with show title option.',
                                              ))

        self.size_titlefont = attrsman.add(cm.AttrConf('size_titlefont', kwargs.get('size_titlefont', 32),
                                                       groupnames=['options'],
                                                       name='Title fontsize',
                                                       info='Title fontsize.',
                                                       ))

        self.size_labelfont = attrsman.add(cm.AttrConf('size_labelfont', kwargs.get('size_labelfont', 24),
                                                       groupnames=['options'],
                                                       name='Label fontsize',
                                                       info='Label fontsize.',
                                                       ))

        self.width_line = attrsman.add(cm.AttrConf('width_line', kwargs.get('width_line', 2),
                                                   groupnames=['options'],
                                                   perm='wr',
                                                   name='Line width',
                                                   info='Width of plotted lines.',
                                                   ))

        self.color_line = attrsman.add(cm.AttrConf('color_line', kwargs.get('color_line', np.array([0, 0, 0, 1], dtype=np.float32)),
                                                   groupnames=['options'],
                                                   perm='wr',
                                                   metatype='color',
                                                   name='Line color',
                                                   info='Color of line in various diagrams.',
                                                   ))

        # COLOR_MATCHED_ROUTE,COLOR_SHORTEST_ROUTE,COLOR_FASTEST_ROUTE
        self.color_matched = attrsman.add(cm.AttrConf('color_matched', kwargs.get('color_matched', COLOR_MATCHED_ROUTE.copy()),
                                                      groupnames=['options'],
                                                      perm='wr',
                                                      metatype='color',
                                                      name='Color matched data',
                                                      info='Color of matched data in various diagrams.',
                                                      ))

        self.color_shortest = attrsman.add(cm.AttrConf('color_shortest', kwargs.get('color_shortest', COLOR_SHORTEST_ROUTE.copy()),
                                                       groupnames=['options'],
                                                       perm='wr',
                                                       metatype='color',
                                                       name='Color shortest route data',
                                                       info='Color of shortest route data in various diagrams.',
                                                       ))

        self.color_fastest = attrsman.add(cm.AttrConf('color_fastest', kwargs.get('color_fastest', COLOR_FASTEST_ROUTE.copy()),
                                                      groupnames=['options'],
                                                      perm='wr',
                                                      metatype='color',
                                                      name='Color fastest route data',
                                                      info='Color of fastest route data in various diagrams.',
                                                      ))

        self.printformat = attrsman.add(cm.AttrConf('printformat', kwargs.get('printformat', '%.1f'),
                                                    choices=OrderedDict([
                                                        ('Show no values', ''),
                                                        ('x', '%.d'),
                                                        ('x.x', '%.1f'),
                                                        ('x.xx', '%.2f'),
                                                        ('x.xxx', '%.3f'),
                                                        ('x.xxxx', '%.4f'),
                                                    ]),
                                                    groupnames=['options'],
                                                    name='Label formatting',
                                                    info='Print formatting of value label in graphical representation.',
                                                    ))

        self.color_label = attrsman.add(cm.AttrConf('color_label', kwargs.get('color_label', np.array([0, 0, 0, 1], dtype=np.float32)),
                                                    groupnames=['options'],
                                                    perm='wr',
                                                    metatype='color',
                                                    name='Label color',
                                                    info='Color of value label in graphical representation.',
                                                    ))

        self.is_grid = attrsman.add(cm.AttrConf('is_grid', kwargs.get('is_grid', True),
                                                groupnames=['options'],
                                                name='Show grid?',
                                                info='If True, shows a grid on the graphical representation.',
                                                ))
        self.color_background = attrsman.add(cm.AttrConf('color_background', kwargs.get('color_background', np.array([1, 1, 1, 1], dtype=np.float32)),
                                                         groupnames=['options'],
                                                         perm='wr',
                                                         metatype='color',
                                                         name='Background color',
                                                         info='Background color of schematic network in the background.',
                                                         ))

        self.add_save_options(**kwargs)

    def show(self):
        # print 'show',self.edgeattrname
        # if self.axis  is None:
        self.init_figures()
        plt.rc('lines', linewidth=self.width_line)
        # plt.rc('axes', prop_cycle=(cycler('color', ['r', 'g', 'b', 'y']) +
        #                    cycler('linestyle', ['-', '--', ':', '-.'])))
        if self.is_plot_lengthratio:
            self.plot_lengthratio()

        if self.is_plot_lengthoverlap:
            self.plot_lengthoverlap()

        if self.is_plot_lengthoverlap_fastest:
            self.plot_lengthoverlap_fastest()

        if self.is_plot_lengthdistrib:
            self.plot_lengthdistrib()

        if self.is_plot_lengthprob:
            self.plot_lengthprob()

        if self.is_plot_lengthdistrib_by_class:
            self.plot_lengthdistrib_by_class()

        # --
        if self.is_plot_lengthdistrib:
            self.plot_lengthdistrib()

        if self.is_plot_mixshare:
            self.plot_mixshare()

        if self.is_plot_exclusiveshare:
            self.plot_exclusiveshare()

        if self.is_plot_lowpriorityshare:
            self.plot_lowpriorityshare()

        if self.is_plot_nodesdensity:
            self.plot_nodesdensity()

        if self.is_plot_tldensity:
            self.plot_tldensity()

        if self.is_prioritychangedensity:
            self.plot_prioritychangedensity()

        # non overlapping
        if self.is_plot_lengthratio_nonoverlap:
            self.plot_lengthratio_nonoverlap()

        if self.is_plot_mixshare_nonoverlap:
            self.plot_mixshare_nonoverlap()

        if self.is_plot_exclusiveshare_nonoverlap:
            self.plot_exclusiveshare_nonoverlap()

        if self.is_plot_lowpriorityshare_nonoverlap:
            self.plot_lowpriorityshare_nonoverlap()

        if self.is_plot_nodesdensity_nonoverlap:
            self.plot_nodesdensity_nonoverlap()

        if self.is_plot_tldensity_nonoverlap:
            self.plot_tldensity_nonoverlap()

        if self.is_prioritychangedensity_nonoverlap:
            self.plot_prioritychangedensity_nonoverlap()

        if self.is_plot_deptimedistrib:
            self.plot_deptimedistrib()

        if self.is_plot_bike_availability_ffbss:
            self.plot_bike_availability_ffbss()

        if self.is_plot_bike_availability_ffbss_zone:
            self.plot_bike_availability_ffbss_zone()

        if self.is_analyze_bike_availability_ffbss_zones:
            self.analyze_bike_availability_ffbss_zones()

        if not self.is_save:
            show_plot()

    def plot_bike_availability_ffbss(self):
        print 'plot_bike_availability_ffbss'
        # Print the number of bikes simultaneously used every 24h/n_bins hours by selecting traces of a particular day
        fig = self.create_figure()
        mapmatching = self.parent.parent
        trips = mapmatching.trips
        ids_trip = trips.get_ids()[(trips.are_selected[trips.get_ids()] == True)]
        dep_arr_times = np.zeros((len(ids_trip), 2))
        for i, id_trip in zip(range(len(ids_trip)), ids_trip):
            t = time.localtime(trips.timestamps[id_trip])
            dep_arr_times[i, 0] = t.tm_hour + t.tm_min/60.0 + t.tm_sec/3600.0
            dep_arr_times[i, 1] = dep_arr_times[i, 0] + trips.durations_gps[id_trip]/3600.0
        self.n_bins
##        dep_arr_times = np.sort(dep_arr_times.sort, axis = 0)
        print dep_arr_times[:1000]
        ax = fig.add_subplot(111)
        x_min = min(dep_arr_times[:, 0])
        x_max = max(dep_arr_times[:, 0])
        bins = np.linspace(x_min, x_max, self.n_bins)
        bike_usage = np.zeros(self.n_bins)

        for id_trip, dep_arr_time in zip(ids_trip, dep_arr_times):
            for bin, i in zip(bins, range(self.n_bins)):
                if dep_arr_time[0] < bin and dep_arr_time[1] > bin:
                    bike_usage[i-1] += 1
        print bike_usage
        bincenters = plt.plot(bins, bike_usage,  color=self.color_matched,
                              label='Bike usage distribution of GPS Mobike traces')
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Bike usage distribution of GPS Mobike traces', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Bike usage', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('bike_usage')

    def plot_bike_availability_ffbss_zone(self):
        print 'plot_bike_availability_ffbss_zone'
        # Plot the difference between attracted and generated trips froma zone in a day every 24h/n_bins hours
        fig = self.create_figure()
        mapmatching = self.parent.parent
        trips = mapmatching.trips
        points = mapmatching.points
        ids_trip = trips.get_ids()
        ids_trip = ids_trip[(trips.are_selected[ids_trip] == True) & (trips.ids_points[ids_trip] != int)]
        ids_points = trips.ids_points[ids_trip]
        zone = self.zone_ffbss
        scenario = mapmatching.get_scenario()
        zones = scenario.landuse.zones
        zone_shape = zones.shapes[zones.ids_sumo.get_id_from_index(self.zone_ffbss)]
        n_bins = self.n_bins
        generated_trips = np.zeros((n_bins))
        attracted_trips = np.zeros((n_bins))

        for id_trip, ids_point in zip(ids_trip, ids_points):
            id_initial_point = ids_point[0]
            id_final_point = ids_point[-1]
            print id_trip
            starting_time = np.int(points.timestamps[id_initial_point] % 86400/86400.*np.float(n_bins))
            arriving_time = np.int(points.timestamps[id_final_point] % 86400/86400.*np.float(n_bins))

            if is_point_in_polygon(mapmatching.points.coords[id_initial_point], zone_shape):
                generated_trips[starting_time] += 1

            if is_point_in_polygon(mapmatching.points.coords[id_final_point], zone_shape):
                attracted_trips[arriving_time] += 1

        availability = np.cumsum(attracted_trips) - np.cumsum(generated_trips)
        bins = np.linspace(0, 24, num=n_bins)
        np.savetxt('generated_trips_%s.txt' % (zone), generated_trips)
        np.savetxt('attracted_trips_%s.txt' % (zone), attracted_trips)

        ax = fig.add_subplot(131)
        bincenters = plt.plot(bins, availability,  color=self.color_matched,
                              label='Bikes availability')
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Bikes relativeavailability during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Bike Relative Availability', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        ax = fig.add_subplot(132)
        bincenters = plt.plot(bins, generated_trips,  color=self.color_matched,
                              label='Generated bikes')
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Generated bikes during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Generated bikes', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        ax = fig.add_subplot(133)
        bincenters = plt.plot(bins, attracted_trips,  color=self.color_matched,
                              label='Attracted bikes')
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Attracted bikes during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Attracted bikes', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('bike_availability')

    def analyze_bike_availability_ffbss_zones(self):
        print 'analyze_bike_availability_ffbss_zones'
        # Save a .txt file with generated trips and attracted trips every 24h/n_bins hours for each zone with selected trips
        fig = self.create_figure()
        mapmatching = self.parent.parent
        trips = mapmatching.trips
        points = mapmatching.points
        ids_trip = trips.get_ids()
        ids_trip = ids_trip[(trips.are_selected[ids_trip] == True) & (trips.ids_points[ids_trip] != int)]
        ids_points = trips.ids_points[ids_trip]
        zone = self.zone_ffbss
        scenario = mapmatching.get_scenario()
        zones = scenario.landuse.zones
        n_zone = len(zones.get_ids())
        n_bins = self.n_bins
        generated_trips = np.zeros((n_zone, n_bins))
        attracted_trips = np.zeros((n_zone, n_bins))
        for id_trip, ids_point in zip(ids_trip, ids_points):
            id_initial_point = ids_point[0]
            id_final_point = ids_point[-1]
            print id_trip
            starting_time = np.int(points.timestamps[id_initial_point] % 86400/86400.*np.float(n_bins))
            arriving_time = np.int(points.timestamps[id_final_point] % 86400/86400.*np.float(n_bins))
            i = 0
            for zone in zones.get_ids():
                zone_shape = zones.shapes[zone]
                if is_point_in_polygon(mapmatching.points.coords[id_initial_point], zone_shape):
                    generated_trips[i, starting_time] += 1
                    break
                i += 1
            i = 0
            for zone in zones.get_ids():
                zone_shape = zones.shapes[zone]
                if is_point_in_polygon(mapmatching.points.coords[id_final_point], zone_shape):
                    attracted_trips[i, arriving_time] += 1
                    break
                i += 1
        np.savetxt('generated_trips.txt', generated_trips)
        np.savetxt('attracted_trips.txt', attracted_trips)

        availability = attracted_trips - generated_trips
        for row in range(len(availability[:, 0])):
            availability[row, :] = np.cumsum(availability[row, :])

        bins = np.linspace(0, 24, num=n_bins)
        np.savetxt('generated_trips_%s.txt' % (zone), generated_trips)
        np.savetxt('attracted_trips_%s.txt' % (zone), attracted_trips)

        ax = fig.add_subplot(231)
        for row in range(len(availability[:, 0])):
            bincenters = plt.plot(bins, availability[row, :],  color=self.color_matched)
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Bikes relativeavailability during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Bike Relative Availability', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        ax = fig.add_subplot(232)
        for row in range(len(generated_trips[:, 0])):
            bincenters = plt.plot(bins, generated_trips[row, :],  color=self.color_matched)
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Generated bikes during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Generated bikes', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        ax = fig.add_subplot(233)
        for row in range(len(attracted_trips[:, 0])):
            bincenters = plt.plot(bins, attracted_trips[row, :],  color=self.color_matched)
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Attracted bikes during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Attracted bikes', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('bike_availability')

        # plot relative values
        availability2 = np.zeros((len(availability[:, 0]), len(availability[0, :])))
        generated_trips2 = np.zeros((len(availability[:, 0]), len(availability[0, :])))
        attracted_trips2 = np.zeros((len(availability[:, 0]), len(availability[0, :])))

        for row in range(len(availability[:, 0])):
            availability2[row, :] = availability[row, :]/np.sum(np.absolute(availability[row, :]))
            generated_trips2[row, :] = generated_trips[row, :]/np.sum(generated_trips[row, :])
            attracted_trips2[row, :] = attracted_trips[row, :]/np.sum(attracted_trips[row, :])

        ax = fig.add_subplot(234)
        for row in range(len(availability[:, 0])):
            if np.sum(generated_trips[row, :]) > n_bins and np.sum(attracted_trips[row, :]) > n_bins:
                bincenters = plt.plot(bins, availability2[row, :],  color=self.color_matched)
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Bikes relativeavailability during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Bike Relative Availability', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        ax = fig.add_subplot(235)
        for row in range(len(generated_trips[:, 0])):
            if np.sum(generated_trips[row, :]) > n_bins and np.sum(attracted_trips[row, :]) > n_bins:
                bincenters = plt.plot(bins, generated_trips2[row, :],  color=self.color_matched)
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Generated bikes during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Generated bikes', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        ax = fig.add_subplot(236)
        for row in range(len(attracted_trips[:, 0])):
            if np.sum(generated_trips[row, :]) > n_bins and np.sum(attracted_trips[row, :]) > n_bins:
                bincenters = plt.plot(bins, attracted_trips2[row, :],  color=self.color_matched)
        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Attracted bikes during time', fontsize=self.size_titlefont)
        ax.set_xlabel('time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Attracted bikes', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('bike_availability')

    def plot_deptimedistrib(self):
        print 'plot_deptimedistrib'
        fig = self.create_figure()
        mapmatching = self.parent.parent
        trips = mapmatching.trips
        ids_trip = trips.get_ids()
        hour_departure = np.zeros(len(ids_trip))
        for i, id_trip in zip(range(len(ids_trip)), ids_trip):
            t = time.localtime(trips.timestamps[id_trip])
            hour_departure[i] = t.tm_hour + t.tm_min/60.0 + t.tm_sec/3600.0
        # print hour_departure[:1000]

        ax = fig.add_subplot(111)
        x_min = min(hour_departure)
        x_max = max(hour_departure)
        if self.n_bins < 0:
            bins = np.arange(x_min, x_max + 1, 1)

        else:
            n_bins = self.n_bins
            bins = np.linspace(x_min, x_max, n_bins)

        # print '  bins',bins
        bincenters = plt.hist(hour_departure, bins=bins, color=self.color_line,
                              density=True,
                              histtype='stepfilled',
                              label='Departure time (hour) distribution')

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            if self.title == "":
                title = 'Departure time distribution of GPS traces'
            else:
                title = self.title
            ax.set_title(title, fontsize=self.size_titlefont)
        ax.set_xlabel('Departure time [h]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('plot_deptimedistrib')

    def plot_tldensity(self):
        print 'plot_tldensity'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        matched = routesresults_matched.numbers_nodes_tls[ids_valid]/dists_match*1000
        shortest = routesresults_shortest.numbers_nodes_tls[ids_valid]/dists_shortest*1000

        x_min = min(np.min(matched), np.min(shortest))
        x_max = 10.0  # max(np.max(matched),np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Node densities of matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Traffic light density [1/km]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_tldensity')

    def plot_nodesdensity(self):
        print 'plot_nodesdensity'
        fig = self.create_figure()
        results = self.parent

        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        matched = routesresults_matched.numbers_nodes[ids_valid]/dists_match*1000
        shortest = routesresults_shortest.numbers_nodes[ids_valid]/dists_shortest*1000

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Node densities of matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Node density [1/km]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_nodesdensity')

    def plot_prioritychangedensity(self):
        print 'plot_prioritychangedensity'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        matched = routesresults_matched.numbers_prioritychange[ids_valid]/dists_match*1000
        shortest = routesresults_shortest.numbers_prioritychange[ids_valid]/dists_shortest*1000

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Priority change dens. of matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Priority change density [1/km]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_nodesdensity')

    def plot_lowpriorityshare(self):
        print 'plot_lowpriorityshare'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        matched = routesresults_matched.lengths_low_priority[ids_valid]/dists_match*100
        shortest = routesresults_shortest.lengths_low_priority[ids_valid]/dists_shortest*100

        x_min = min(np.min(matched), np.min(shortest))
        x_max = 15.0  # max(np.max(matched),np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of low priority roads of matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Low priority road share [%]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_lowpriorityshare')

    def plot_exclusiveshare(self):
        print 'plot_exclusiveshare'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        matched = routesresults_matched.lengths_exclusive[ids_valid]/dists_match*100
        shortest = routesresults_shortest.lengths_exclusive[ids_valid]/dists_shortest*100

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of exclusive access roads of matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Exclusive access road share [%]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_exclusiveshare')

    def plot_mixshare(self):
        print 'plot_mixshare'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        matched = routesresults_matched.lengths_mixed[ids_valid]/dists_match*100
        shortest = routesresults_shortest.lengths_mixed[ids_valid]/dists_shortest*100

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of mixed reserved access roads of matched and shortest route',
                         fontsize=self.size_titlefont)
        ax.set_xlabel('Mixed reserved access road share [%]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_mixshare')

    def plot_lengthdistrib_by_class(self):
        print 'plot_lengthdistrib_by_class'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]
        x_min = min(np.min(dists_match), np.min(dists_shortest))
        x_max = max(np.max(dists_match), np.max(dists_shortest))

        dists_class = np.arange(0, int(x_max), self.distance_class)
        dists_class_center = (dists_class+0.5*self.distance_class)[1:]
        n_class = len(dists_class)-1
        means_match = np.zeros(n_class, dtype=np.float32)
        stds_match = np.zeros(n_class, dtype=np.float32)
        means_shortest = np.zeros(n_class, dtype=np.float32)
        stds_shortest = np.zeros(n_class, dtype=np.float32)
        xticklabels = []
        ratiolabels = []
        for dist_lower, dist_upper, i in zip(dists_class[:-1], dists_class[1:], range(n_class)):
            xticklabels.append('%d - %d' % (float(dist_lower)/1000, float(dist_upper)/1000))
            inds = np.logical_and(dists_match > dist_lower, dists_match < dist_upper)
            means_match[i] = np.mean(dists_match[inds])
            stds_match[i] = np.std(dists_match[inds])

            #inds = np.logical_and(dists_shortest>dist_lower,dists_shortest<dist_upper)
            means_shortest[i] = np.mean(dists_shortest[inds])
            stds_shortest[i] = np.std(dists_shortest[inds])

            ratiolabel = ''
            if (not np.isnan(means_shortest[i])) & (not np.isnan(means_match[i])):
                if means_match[i] > 0:
                    ratiolabel = '%d%%' % (means_shortest[i]/means_match[i]*100)
            ratiolabels.append(ratiolabel)

        print '  dists_class_center', dists_class_center
        print '  means_match', means_match
        print '  stds_match', stds_match
        print '  means_shortest', means_shortest
        print '  stds_shortest', stds_shortest

        x = np.arange(n_class, dtype=np.float32)  # the x locations for the groups
        width = 0.35       # the width of the bars

        #        ax.bar(ind + width, women_means, width, color='y', yerr=women_std)
        bars1 = ax.bar(x-width, means_match, width, color=self.color_matched, yerr=stds_match)
        bars2 = ax.bar(x+0*width, means_shortest, width, color=self.color_shortest, yerr=stds_shortest)
        #bars1 = ax.bar(dists_class_center+0.35*self.distance_class, means_match, 0.25*self.distance_class, color=self.color_matched, yerr=stds_match)
        #bars2 = ax.bar(dists_class_center-0.35*self.distance_class, means_shortest, 0.25*self.distance_class, color=self.color_shortest, yerr=stds_shortest)

        #ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
        ax.legend((bars1[0], bars2[0]), ('matched', 'shortest'),
                  shadow=True, fontsize=self.size_labelfont, loc='best')

        # if self.is_grid:
        ax.yaxis.grid(self.is_grid)

        if self.is_title:
            ax.set_title('Mean length by trip length class', fontsize=self.size_titlefont)
        ax.set_xlabel('Length classes [km]', fontsize=self.size_labelfont)
        ax.set_ylabel('Mean length [m]', fontsize=self.size_labelfont)

        ax.set_xticks(x)
        ax.set_xticklabels(xticklabels)
        # self._autolabel_bars(ax,bars1,means_match)
        self._autolabel_bars(ax, bars2, ratiolabels)

        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_lengthdistrib_by_class')

    def _autolabel_bars(self, ax, bars, labels):
        """
        Attach a text label above each bar displaying its height
        """
        for rect, label in zip(bars, labels):
            height = rect.get_height()
            if not np.isnan(height):
                ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                        '%s' % label,
                        ha='center', va='bottom',
                        fontsize=int(0.8*self.size_labelfont),
                        )

    def plot_lengthdistrib(self):

        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(routesresults_shortest.distances.get_value() > 0,
                                                                    time.                                          routesresults_matched.distances.get_value() > 0,
                                                                    # routesresults_matched.distances.get_value()<20000)
                                                                    ))
        print 'plot_lengthdistrib', len(ids_valid)
        # print '  ids_valid',ids_valid
        if len(ids_valid) == 0:
            return False

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        x_min = min(np.min(dists_match), np.min(dists_shortest))
        x_max = max(np.max(dists_match), np.max(dists_shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, dists_match, bins=bins, color=self.color_matched,
                                    label='matched:'+'$\mu = %dm$, $\sigma=%dm$' % (np.mean(dists_match), np.std(dists_match)))
        bincenters = self.plot_hist(ax, dists_shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %dm$, $\sigma=%dm$' % (np.mean(dists_shortest), np.std(dists_shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Length distribution of matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Length [m]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_lengthdistrib')
        return True

    def plot_timedistrib(self):
        print 'plot_timedistrib'
        fig = self.create_figure()
        results = self.parent
        mapmatching = results.parent
        trips = mapmatching.trips
        #routesresults_fastest = results.routesresults_fastest
        #routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        #ids_overlength = routesresults_matched.select_ids(np.logical_and(routesresults_shortest.distances.get_value()>0, routesresults_matched.distances.get_value()>20000))
        # print '  len(ids_overlength)',len(ids_overlength)
        # print '  ids_overlength',ids_overlength

        ids_valid = trips.select_ids(
            np.logical_and(trips.lengths_route_matched.get_value() > 0,
                           trips.durations_route_fastest.get_value() > 0,
                           # routesresults_matched.distances.get_value()<20000)
                           ))

        if len(ids_valid) == 0:
            return False

        times_fastest = trips.durations_route_fastest[ids_valid]
        times_match = times_fastest+trips.timelosses_route_fastest[ids_valid]

        x_min = min(np.min(times_fastest), np.min(times_match))
        x_max = max(np.max(times_fastest), np.max(times_match))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, times_match, bins=bins, color=self.color_matched,
                                    label='matched:'+'$\mu = %dm$, $\sigma=%dm$' % (np.mean(times_match), np.std(times_match)))
        bincenters = self.plot_hist(ax, times_fastest, bins=bins, color=self.color_fastest, label='fastest:' +
                                    '$\mu = %dm$, $\sigma=%dm$' % (np.mean(times_fastest), np.std(times_fastest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Triptime distribution of matched and fastest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Time [s]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_timedistrib')

        return True

    def plot_lengthprob(self):
        print 'plot_lengthprob'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))

        dists_match = routesresults_matched.distances[ids_valid]
        dists_shortest = routesresults_shortest.distances[ids_valid]

        x_min = min(np.min(dists_match), np.min(dists_shortest))
        x_max = max(np.max(dists_match), np.max(dists_shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        w_bin = bins[1]-bins[0]
        bincenters = self.plot_hist(ax, dists_match, bins=bins,
                                    color=self.color_matched,
                                    label='matched:' +
                                    '$\mu = %dm$, $\sigma=%dm$' % (np.mean(dists_match), np.std(dists_match)),
                                    is_rel_frequ=True,
                                    is_percent=True,
                                    )
        bincenters = self.plot_hist(ax, dists_shortest, bins=bins,
                                    color=self.color_shortest,
                                    label='shortest:' +
                                    '$\mu = %dm$, $\sigma=%dm$' % (np.mean(dists_shortest), np.std(dists_shortest)),
                                    is_rel_frequ=True,
                                    is_percent=True,
                                    )

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Relative frequency of matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Length [m]', fontsize=self.size_labelfont)
        ax.set_ylabel('Relative frequency [%]', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_lengthprob')

    def plot_lengthoverlap(self):
        print 'plot_lengthoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        bins = np.linspace(0.0, 1.0, self.n_bins)

        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))
        values = routesresults_shortest.lengths_overlap_matched[ids_valid]/routesresults_matched.distances[ids_valid]
        bincenters = self.plot_hist(ax, values,
                                    bins=bins, histtype='bar',
                                    label=r'$\mu = %.2f$, $\sigma=%.2f$' % (np.mean(values), np.std(values))
                                    )

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of overlap between shortest path and matched path', fontsize=self.size_titlefont)
        ax.set_xlabel('Overlap share between shortest and matched path', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        self.set_figmargins()
        if self.is_save:
            self.save_fig('routeana_lengthoverlap')

    def plot_lengthoverlap_fastest(self):
        print 'plot_lengthoverlap_fastest'
        fig = self.create_figure()
        results = self.parent
        print 'dir(results)', dir(results)
        routesresults_fastest = results.routesresults_fastest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        bins = np.linspace(0.0, 1.0, self.n_bins)

        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_fastest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))
        values = routesresults_fastest.lengths_overlap_matched[ids_valid]/routesresults_matched.distances[ids_valid]
        bincenters = self.plot_hist(ax, values,
                                    bins=bins, histtype='bar',
                                    label=r'$\mu = %.2f$, $\sigma=%.2f$' % (np.mean(values), np.std(values))
                                    )

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of overlap between fastest path and matched path', fontsize=self.size_titlefont)
        ax.set_xlabel('Overlap share between fastest and matched path', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        self.set_figmargins()
        if self.is_save:
            self.save_fig('routeana_lengthoverlap_fastest')

    def plot_lengthratio(self):
        print 'plot_lengthratio'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        bins = np.linspace(0.0, 1.0, self.n_bins)

        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest.distances.get_value() > 0, routesresults_matched.distances.get_value() > 0))
        values = routesresults_shortest.distances[ids_valid]/routesresults_matched.distances[ids_valid]
        bincenters = self.plot_hist(ax, values,
                                    bins=bins, histtype='bar',
                                    label=r'$\mu = %.2f$, $\sigma=%.2f$' % (np.mean(values), np.std(values))
                                    )

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Ratio distance shortest path over matched path', fontsize=self.size_titlefont)
        ax.set_xlabel('Ratio shortest path length/matched path length', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        self.set_figmargins()
        if self.is_save:
            self.save_fig('routeana_lengthratio')


# -------------------------------------------------------------------------------
    # non-overlap

    def plot_lengthratio_nonoverlap(self):
        print 'plot_lengthratio_nonoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        routesresults_shortest_nonoverlap = results.routesresults_shortest_nonoverlap
        routesresults_matched_nonoverlap = results.routesresults_matched_nonoverlap

        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        bins = np.linspace(0.0, 1.0, self.n_bins)

        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest_nonoverlap.distances.get_value() > 0,
            routesresults_matched_nonoverlap.distances.get_value() > 0)
        )

        values = routesresults_shortest_nonoverlap.distances[ids_valid] / \
            routesresults_matched_nonoverlap.distances[ids_valid]
        bincenters = self.plot_hist(ax, values,
                                    bins=bins, histtype='bar',
                                    label=r'$\mu = %.2f$, $\sigma=%.2f$' % (np.mean(values), np.std(values))
                                    )

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Ratio distance of non-overlapping shortest over matched path', fontsize=self.size_titlefont)
        ax.set_xlabel('Ratio shortest n.o. path length/matched path length', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        self.set_figmargins()
        if self.is_save:
            self.save_fig('routeana_lengthratio_nonoverlap')

    def plot_tldensity_nonoverlap(self):
        print 'plot_tldensity_nonoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        routesresults_shortest_nonoverlap = results.routesresults_shortest_nonoverlap
        routesresults_matched_nonoverlap = results.routesresults_matched_nonoverlap
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest_nonoverlap.distances.get_value() > 0, routesresults_matched_nonoverlap.distances.get_value() > 0))

        dists_match = routesresults_matched_nonoverlap.distances[ids_valid]
        dists_shortest = routesresults_shortest_nonoverlap.distances[ids_valid]

        matched = routesresults_matched_nonoverlap.numbers_nodes_tls[ids_valid]/dists_match*1000
        shortest = routesresults_shortest_nonoverlap.numbers_nodes_tls[ids_valid]/dists_shortest*1000

        x_min = min(np.min(matched), np.min(shortest))
        x_max = 10.0  # max(np.max(matched),np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Node densities of non-overlapping matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Traffic light density  n.o. [1/km]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_tldensity_nonoverlap')

    def plot_nodesdensity_nonoverlap(self):
        print 'plot_nodesdensity_nonoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        routesresults_shortest_nonoverlap = results.routesresults_shortest_nonoverlap
        routesresults_matched_nonoverlap = results.routesresults_matched_nonoverlap
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest_nonoverlap.distances.get_value() > 0, routesresults_matched_nonoverlap.distances.get_value() > 0))

        dists_match = routesresults_matched_nonoverlap.distances[ids_valid]
        dists_shortest = routesresults_shortest_nonoverlap.distances[ids_valid]

        matched = routesresults_matched_nonoverlap.numbers_nodes[ids_valid]/dists_match*1000
        shortest = routesresults_shortest_nonoverlap.numbers_nodes[ids_valid]/dists_shortest*1000

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Node densities of non-overlapping matched and shortest route', fontsize=self.size_titlefont)
        ax.set_xlabel('Node density  n.o. [1/km]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_nodesdensity_nonoverlap')

    def plot_prioritychangedensity_nonoverlap(self):
        print 'plot_prioritychangedensity_nonoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        routesresults_shortest_nonoverlap = results.routesresults_shortest_nonoverlap
        routesresults_matched_nonoverlap = results.routesresults_matched_nonoverlap
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest_nonoverlap.distances.get_value() > 0, routesresults_matched_nonoverlap.distances.get_value() > 0))

        dists_match = routesresults_matched_nonoverlap.distances[ids_valid]
        dists_shortest = routesresults_shortest_nonoverlap.distances[ids_valid]

        matched = routesresults_matched_nonoverlap.numbers_prioritychange[ids_valid]/dists_match*1000
        shortest = routesresults_shortest_nonoverlap.numbers_prioritychange[ids_valid]/dists_shortest*1000

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f/km$, $\sigma=%.2f/km$' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Priority change dens. of non-overlapping matched and shortest route',
                         fontsize=self.size_titlefont)
        ax.set_xlabel('Priority change density  n.o. [1/km]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_nodesdensity_nonoverlap')

    def plot_lowpriorityshare_nonoverlap(self):
        print 'plot_lowpriorityshare_nonoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        routesresults_shortest_nonoverlap = results.routesresults_shortest_nonoverlap
        routesresults_matched_nonoverlap = results.routesresults_matched_nonoverlap
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest_nonoverlap.distances.get_value() > 0, routesresults_matched_nonoverlap.distances.get_value() > 0))

        dists_match = routesresults_matched_nonoverlap.distances[ids_valid]
        dists_shortest = routesresults_shortest_nonoverlap.distances[ids_valid]

        matched = routesresults_matched_nonoverlap.lengths_low_priority[ids_valid]/dists_match*100
        shortest = routesresults_shortest_nonoverlap.lengths_low_priority[ids_valid]/dists_shortest*100

        x_min = min(np.min(matched), np.min(shortest))
        x_max = 15.0  # max(np.max(matched),np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of low priority roads of non-overlapping matched and shortest route',
                         fontsize=self.size_titlefont)
        ax.set_xlabel('Low priority road share  n.o. [%]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_lowpriorityshare_nonoverlap')

    def plot_exclusiveshare_nonoverlap(self):
        print 'plot_exclusiveshare_nonoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        routesresults_shortest_nonoverlap = results.routesresults_shortest_nonoverlap
        routesresults_matched_nonoverlap = results.routesresults_matched_nonoverlap
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest_nonoverlap.distances.get_value() > 0, routesresults_matched_nonoverlap.distances.get_value() > 0))

        dists_match = routesresults_matched_nonoverlap.distances[ids_valid]
        dists_shortest = routesresults_shortest_nonoverlap.distances[ids_valid]

        matched = routesresults_matched_nonoverlap.lengths_exclusive[ids_valid]/dists_match*100
        shortest = routesresults_shortest_nonoverlap.lengths_exclusive[ids_valid]/dists_shortest*100

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of exclusive access roads of non-overlapping matched and shortest route',
                         fontsize=self.size_titlefont)
        ax.set_xlabel('Exclusive access road share  n.o. [%]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_exclusiveshare_nonoverlap')

    def plot_mixshare_nonoverlap(self):
        print 'plot_mixshare_nonoverlap'
        fig = self.create_figure()
        results = self.parent
        routesresults_shortest = results.routesresults_shortest
        routesresults_matched = results.routesresults_matched
        routesresults_shortest_nonoverlap = results.routesresults_shortest_nonoverlap
        routesresults_matched_nonoverlap = results.routesresults_matched_nonoverlap
        edgesresults = results.edgesresults

        ax = fig.add_subplot(111)
        ids_valid = routesresults_matched.select_ids(np.logical_and(
            routesresults_shortest_nonoverlap.distances.get_value() > 0, routesresults_matched_nonoverlap.distances.get_value() > 0))

        dists_match = routesresults_matched_nonoverlap.distances[ids_valid]
        dists_shortest = routesresults_shortest_nonoverlap.distances[ids_valid]

        matched = routesresults_matched_nonoverlap.lengths_mixed[ids_valid]/dists_match*100
        shortest = routesresults_shortest_nonoverlap.lengths_mixed[ids_valid]/dists_shortest*100

        x_min = min(np.min(matched), np.min(shortest))
        x_max = max(np.max(matched), np.max(shortest))
        bins = np.linspace(x_min, x_max, self.n_bins)
        bincenters = self.plot_hist(ax, matched, bins=bins, color=self.color_matched, label='matched:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(matched), np.std(matched)))
        bincenters = self.plot_hist(ax, shortest, bins=bins, color=self.color_shortest, label='shortest:' +
                                    '$\mu = %.2f$%%, $\sigma=%.2f$%%' % (np.mean(shortest), np.std(shortest)))

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Share of mixed reserved access roads of non-overlapping matched and shortest route',
                         fontsize=self.size_titlefont)
        ax.set_xlabel('Mixed reserved access road share n.o. [%]', fontsize=self.size_labelfont)
        ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            self.save_fig('routeana_mixshare_nonoverlap')
# -------------------------------------------------------------------------------

    def do(self):
        # print 'do',self.edgeattrname
        self.show()

    def get_scenario(self):
        return self.parent.get_scenario()


class PtFlowdigramPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='PT flowdiagram',
                 info="Plots the flow diagram of PT lines using matplotlib",
                 logger=None, **kwargs):

        self._init_common('ptrouteresultplotter', parent=results, name=name,
                          info=info, logger=logger)

        print 'PtFlowdigramPlotter.__init__', results, self.parent
        attrsman = self.get_attrsman()

        self.id_line = attrsman.add(cm.AttrConf('id_line', kwargs.get('id_line', -1),
                                                groupnames=['options'],
                                                name='Line ID',
                                                info='ID of public transport line to be plotted.',
                                                ))

        self.is_add_similar = attrsman.add(cm.AttrConf('is_add_similar', kwargs.get('is_add_similar', True),
                                                       groupnames=['options'],
                                                       name='include similar lines',
                                                       info='Add also trips on similar PT lines.',
                                                       ))

        self.is_title = attrsman.add(cm.AttrConf('is_title', kwargs.get('is_title', False),
                                                 groupnames=['options'],
                                                 name='Show title',
                                                 info='Show title of diagrams.',
                                                 ))

        self.size_titlefont = attrsman.add(cm.AttrConf('size_titlefont', kwargs.get('size_titlefont', 32),
                                                       groupnames=['options'],
                                                       name='Title fontsize',
                                                       info='Title fontsize.',
                                                       ))

        self.size_labelfont = attrsman.add(cm.AttrConf('size_labelfont', kwargs.get('size_labelfont', 24),
                                                       groupnames=['options'],
                                                       name='Label fontsize',
                                                       info='Label fontsize.',
                                                       ))

        self.width_line = attrsman.add(cm.AttrConf('width_line', kwargs.get('width_line', 2),
                                                   groupnames=['options'],
                                                   perm='wr',
                                                   name='Line width',
                                                   info='Width of plotted lines.',
                                                   ))

        # self.color_outline = attrsman.add(cm.AttrConf( 'color_outline', kwargs.get('color_outline', np.array([0.0,0.0,0.0,0.95], dtype=np.float32)),
        #                                groupnames = ['options'],
        #                                perm='wr',
        #                                metatype = 'color',
        #                                name = 'Outline color',
        #                                info = 'Outline color of result arrows in graphical representation. Only valid if no color-fill is chosen.',
        #                                ))

        self.color_fill = attrsman.add(cm.AttrConf('color_fill', kwargs.get('color_fill', np.array([0.3, 0.3, 1.0, 0.95], dtype=np.float32)),
                                                   groupnames=['options'],
                                                   perm='wr',
                                                   metatype='color',
                                                   name='Fill color',
                                                   info='Fill color of result arrows in graphical representation.',
                                                   ))

        self.is_grid = attrsman.add(cm.AttrConf('is_grid', kwargs.get('is_grid', False),
                                                groupnames=['options'],
                                                name='Show grid?',
                                                info='If True, shows a grid on the graphical representation.',
                                                ))

        self.csvfilepath = attrsman.add(cm.AttrConf('csvfilepath', kwargs.get('csvfilepath', ''),
                                                    groupnames=['options'],
                                                    perm='rw',
                                                    name='CSV filename',
                                                    wildcards='CSV file (*.csv)|*.csv|All file (*.*)|*.*',
                                                    metatype='filepaths',
                                                    info="""Filename to export flow results.""",
                                                    ))

        self.add_save_options(**kwargs)

    def show(self):
        print 'show'
        # if self.axis  is None:
        self.init_figures()
        plt.rc('lines', linewidth=self.width_line)
        fig = self.create_figure()
        ax = fig.add_subplot(111)

        results = self.parent

        ptlinesresults = results.ptlinesresults

        # link object
        mapmatching = results.parent
        #trips = mapmatching.trips
        #routes = trips.get_routes()

        scenario = mapmatching.get_scenario()
        ptstops = scenario.net.ptstops
        demand = scenario.demand
        ptlines = demand.ptlines
        ptlinks = ptlines.get_ptlinks()

        if not self.id_line in ptlines:
            print 'WARNING: line with ID', self.id_line, 'not found.'
            return False

        #id_line = ptlines.linenames.get_id_from_index(self.linename)
        if self.is_add_similar:
            linename = ptlines.linenames[self.id_line].split('_')[0]
        else:
            linename = ptlines.linenames[self.id_line]
        ids_stoptuple_line, tripnumbers_line = ptlinesresults.get_flowdiagramm(
            self.id_line, is_add_similar=self.is_add_similar)

        n_stops = len(ids_stoptuple_line)
        ax.bar(xrange(n_stops), tripnumbers_line,
               width=1.0, bottom=0, align='edge',
               color=self.color_fill,
               #linecolor = self.color_outline,
               linewidth=self.width_line,)

        stopnames = []
        for id_fromstop, id_tostop in ids_stoptuple_line:
            stopnames.append(ptstops.stopnames_human[id_fromstop])
        # print  '  stopnames',stopnames
        ax.set_xticks(xrange(n_stops))
        ax.set_xticklabels(stopnames)

        #ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_title:
            ax.set_title('Flow diagram of PT line '+linename, fontsize=self.size_titlefont)
        ax.set_xlabel('Stops', fontsize=self.size_labelfont)
        ax.set_ylabel('Number of Passengers', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        plt.setp(ax.get_xticklabels(), rotation=90, ha="right", rotation_mode="anchor")

        if len(self.csvfilepath) > 0:
            # export to csv
            f = open(self.csvfilepath, 'w')
            f.write('Flow diagram of PT line '+linename+'\n\n')
            sep = ','
            f.write('Stopname from'+sep+'Stopname from'+sep+'Passengers\n')
            for ids_stoptuple, number in zip(ids_stoptuple_line, tripnumbers_line):
                f.write(ptstops.stopnames_human[ids_stoptuple[0]]+sep +
                        ptstops.stopnames_human[ids_stoptuple[1]]+sep+str(number)+'\n')

            f.close()

        if self.is_save:
            self.save_fig('_ptrouteana_flowdiagram')
        else:
            show_plot()
