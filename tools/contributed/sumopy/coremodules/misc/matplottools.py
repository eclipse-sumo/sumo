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

# @file    matplottools.py
# @author  Joerg Schweizer
# @date   2012

from agilepy.lib_wx.processdialog import ProcessDialog, ProcessDialogInteractive
from agilepy.lib_base.processes import Process
from agilepy.lib_base.geometry import *
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.classman as cm
import os
import numpy as np
from collections import OrderedDict
import matplotlib as mpl
from matplotlib.patches import Arrow, Circle, Wedge, Polygon, FancyArrow
from matplotlib.collections import PatchCollection
import matplotlib.colors as colors
import matplotlib.cm as cmx
import matplotlib.pyplot as plt
import matplotlib.image as image
from matplotlib.ticker import MaxNLocator
import matplotlib.patches as mpatches

if 0:  # change backend...makes problems with plt
    gui_env = ['WXAgg', 'GTKAgg', 'Qt4Agg', 'TKAgg', ]
    for gui in gui_env:
        try:
            print "Try Matplotlib backend:", gui
            mpl.use(gui, warn=False, force=True)
            from mpl.patches import Arrow, Circle, Wedge, Polygon, FancyArrow
            from mpl.mpl import PatchCollection
            import matplotlib.colors as colors
            import mpl.cm as cmx
            import mpl.pyplot as plt
            import mpl.image as image
            from mpl.ticker import MaxNLocator
            import mpl.patches as mpatches
            break
        except:
            continue
    print "Using Matplotlib backend", mpl.get_backend()


try:
    import wx
except:
    print 'WARNING: no wxwindows support'


COLORS = ['#1f77b4', '#aec7e8', '#ff7f0e', '#ffbb78', '#2ca02c',
          '#98df8a', '#d62728', '#ff9896', '#9467bd', '#c5b0d5',
          '#8c564b', '#c49c94', '#e377c2', '#f7b6d2', '#7f7f7f',
          '#c7c7c7', '#bcbd22', '#dbdb8d', '#17becf', '#9edae5']

LINESTYLES = ['-', '--', '-.', ':', 'None']


MARKERSTYLES = ['None', 'o', 'v', 's', '*', '^', 'h', 'D', 'p', '<', '>']

global ind_fig
ind_fig = 0


def get_color(i):
    return COLORS[i % len(COLORS)]


def get_colors(inds):
    return np.array(COLORS, dtype=np.str)[np.mod(inds, len(COLORS))]


def get_marker(i):
    return MARKERSTYLES[i % len(MARKERSTYLES)]


def get_markers(inds):
    return np.array(MARKERSTYLES, dtype=np.str)[np.mod(inds, len(MARKERSTYLES))]


class ResultDialog(ProcessDialog):
    def _get_buttons(self):
        buttons = [('Plot and close',   self.on_run, 'Plot  selected quantity in matplotlib window and close this window thereafter.'),
                   ('Plot',   self.on_show, 'Plot selected quantity  in matplotlib window.'),
                   ('Save Options...', self.on_save_options, self.on_save_options.__doc__),
                   ('Load Options...', self.on_load_options, self.on_load_options.__doc__),
                   ]
        defaultbutton = 'Plot and close'
        standartbuttons = ['cancel', ]

        return buttons, defaultbutton, standartbuttons

    def on_show(self, event):
        self.process.show()


def get_mplicon():
    icondir = os.path.join(os.path.dirname(__file__), 'images')
    return wx.Bitmap(os.path.join(icondir, 'icon_mpl.png'))


def init_plot(tight_layout=True):
    plt.close("all")
    fig = plt.figure()
    ax = fig.add_subplot(111)
    if tight_layout:
        fig.tight_layout()
    return ax


def init_plot_fig_ax():
    plt.close("all")
    fig = plt.figure()
    ax = fig.add_subplot(111)
    return fig, ax


def new_plot(tight_layout=True):
    global ind_fig
    fig = plt.figure(ind_fig)
    ind_fig += 1
    ax = fig.add_subplot(111)
    if tight_layout:
        fig.tight_layout()
    return ax


def new_doubkeplot(tight_layout=True):
    global ind_fig
    fig = plt.figure(ind_fig)
    ind_fig += 1
    if tight_layout:
        fig.tight_layout()
    return plt.subplot(2, 1, 1), plt.subplot(2, 1, 2)


def plot_net(ax, net, color_edge="gray", width_edge=2, color_node=None,
             alpha=0.5, is_show_connections=True, mapscale=1.0):
    for shape in net.edges.shapes.get_value():
        x_vec = np.array(shape)[:, 0]
        y_vec = np.array(shape)[:, 1]
        ax.plot(x_vec*mapscale, y_vec*mapscale, color=color_edge, lw=width_edge, alpha=alpha, zorder=-100)
    if is_show_connections:
        for id_connection in net.connections.get_ids():
            x_vec = np.array([net.edges.shapes[net.lanes.ids_edge[net.connections.ids_fromlane[id_connection]]]
                              [-1, 0], net.edges.shapes[net.lanes.ids_edge[net.connections.ids_tolane[id_connection]]][0, 0]])
            y_vec = np.array([net.edges.shapes[net.lanes.ids_edge[net.connections.ids_fromlane[id_connection]]]
                              [-1, 1], net.edges.shapes[net.lanes.ids_edge[net.connections.ids_tolane[id_connection]]][0, 1]])
            ax.plot(x_vec*mapscale, y_vec*mapscale, color=color_edge, lw=width_edge*0.8, alpha=alpha, zorder=-100)

    # do nodes
    if color_node is None:
        is_nodefill = False
        color_node = 'none'
    else:
        is_nodefill = True

    #ax.scatter(x_vec[0], y_vec[0], s=np.pi * (10.0)**2, c=colors, alpha=0.5)
    coords = net.nodes.coords.get_value()
    radii = net.nodes.radii.get_value()
    #ax.scatter(coords[:,0], coords[:,1], s=np.pi * (radii)**2, alpha=0.5)
    #patches = []
    for coord, radius in zip(coords, radii):
        ax.add_patch(Circle(coord*mapscale, radius=radius*mapscale,
                            linewidth=width_edge,
                            edgecolor=color_edge,
                            facecolor=color_node,
                            fill=is_nodefill,
                            alpha=alpha,
                            zorder=-50))


def plot_maps(ax, maps, alpha=0.5, mapscale=1.0):
    # print 'plot_maps'
    net = maps.parent.get_net()
    rootdir = os.path.dirname(net.parent.get_rootfilepath())

    for filename, bbox in zip(maps.filenames.get_value(), maps.bboxes.get_value()):
        # print '  ',filename,
        if filename == os.path.basename(filename):
            # filename does not contain path info
            filepath = os.path.join(rootdir, filename)
        else:
            # filename contains path info (can happen if interactively inserted)
            filepath = filename
        # print '  filepath',filepath
        im = image.imread(filepath)
        myaximage = ax.imshow(im, aspect='auto', extent=(
            bbox[0, 0]*mapscale, bbox[1, 0]*mapscale, bbox[0, 1]*mapscale, bbox[1, 1]*mapscale), alpha=alpha, zorder=-1000)

# def plot_facilities(ax, facilities, color_building = "gray",
        # color_outline = 'white', width_line = 2,
        # alpha = 0.5, mapscale=1.0):

    # if color_building is None:
        # is_fill = False
        # color_building = 'none'
    # else:
        # is_fill = True
    # print mpl.patches.Polygon.__doc__
    # for shape in facilities.shapes.get_value():
        # #x_vec = np.array(shape)[:,0]
        # #y_vec = np.array(shape)[:,1]
        # #ax.plot(x_vec, y_vec, color = color_building, lw = width_line,
        # #        alpha=alpha )
        # print '    add shape',shape
        # ax.add_patch(mpl.patches.Polygon(  np.array(shape)[:,:2]*mapscale,
        # linewidth = width_line,
        # edgecolor = color_outline,
        # facecolor = color_building,
        # fill = is_fill,
        # alpha = alpha,
        # zorder = -200))


def plot_facilities(ax, facilities, color_building="gray",
                    color_outline='white', width_line=2,
                    alpha=0.5, mapscale=1.0):

    if color_building is None:
        is_fill = False
        color_building = 'none'
    else:
        is_fill = True
    for shape in facilities.shapes.get_value():
        #x_vec = np.array(shape)[:,0]
        #y_vec = np.array(shape)[:,1]
        # ax.plot(x_vec, y_vec, color = color_building, lw = width_line,
        #        alpha=alpha )
        ax.add_patch(mpl.patches.Polygon(np.array(shape)[:, :2]*mapscale,
                                         linewidth=width_line,
                                         edgecolor=color_outline,
                                         facecolor=color_building,
                                         fill=is_fill,
                                         alpha=alpha,
                                         zorder=-200))


def plot_zones(ax, zones, color_zone=None,
               color_outline='green', width_line=2,
               alpha=0.5, mapscale=1.0):

    if color_zone is None:
        is_fill = False
        color_zone = 'none'
    else:
        is_fill = True
    for shape in zones.shapes.get_value():
        #x_vec = np.array(shape)[:,0]
        #y_vec = np.array(shape)[:,1]
        # ax.plot(x_vec, y_vec, color = color_building, lw = width_line,
        #        alpha=alpha )
        ax.add_patch(mpl.patches.Polygon(np.array(shape)[:, :2]*mapscale,
                                         linewidth=width_line,
                                         edgecolor=color_outline,
                                         facecolor=color_zone,
                                         fill=is_fill,
                                         alpha=alpha,
                                         zorder=200))


def plot_zone(ax, zone_shape, color_zone="gray",
              color_outline='white', width_line=2,
              alpha=0.5, zorder=-200, mapscale=1.0):

    if color_zone is None:
        is_fill = False
        color_building = 'none'
    else:
        is_fill = True

    shape = (np.array(zone_shape)[:, :2]*mapscale).tolist()
    print shape
    ax.add_patch(Polygon(shape,
                         linewidth=width_line,
                         edgecolor=color_outline,
                         facecolor=color_zone,
                         fill=is_fill,
                         alpha=alpha,
                         zorder=-200))


def plot_edgevalues_lines(ax, ids_result=None, config_ids_edge=None, values=[],
                          ids_edge=None, edges=None,
                          width_max=10.0, alpha=0.8, printformat='%.2f',
                          color_outline=None, color_fill=None,
                          color_label='black', is_antialiased=True,
                          is_fill=True, is_widthvalue=True,
                          arrowshape='left', length_arrowhead=10.0,
                          headwidthstretch=1.3,
                          fontsize=32,
                          valuelabel='',
                          value_range=None,
                          is_colorbar=True,
                          mapscale=1.0):
    # ATTENTION: do not change order of args for backwards compatibility

    head_width = headwidthstretch*width_max
    fontsize_ticks = int(0.8*fontsize)

    if ids_edge is None:
        edges = config_ids_edge.get_linktab()
        ids_edge = config_ids_edge[ids_result]
    #values = config_values[ids_result]
    values_norm = np.array(values, dtype=np.float32)/np.max(values)

    patches = []
    displacement = float(width_max)/2.0
    if is_widthvalue:
        linewidths = width_max*values_norm
    else:
        linewidths = width_max * np.ones(len(values_norm), np.float32)

    deltaangle_text = -np.pi/2.0
    displacement_text = displacement+width_max

    jet = cm_jet = plt.get_cmap('jet')
    if value_range is None:
        c_norm = colors.Normalize(vmin=0, vmax=np.max(values))
    else:
        c_norm = colors.Normalize(vmin=value_range[0], vmax=value_range[1])

    # http://stackoverflow.com/questions/8342549/matplotlib-add-colorbar-to-a-sequence-of-line-plots/11558629#11558629
    sm = cmx.ScalarMappable(norm=c_norm, cmap=jet)
    sm.set_array(values_norm)
    for id_edge, value, value_norm, linewidth in zip(ids_edge, values, values_norm, linewidths):
        if not np.isnan(value):
            shape, angles_perb = get_resultshape(edges, id_edge, displacement)
            x_vec = np.array(shape)[:, 0]
            y_vec = np.array(shape)[:, 1]
            deltax = x_vec[-1]-x_vec[0]
            deltay = y_vec[-1]-y_vec[0]
            if is_fill:  # means that fill with value proportional color
                color = sm.to_rgba(value)
            else:
                color = color_fill

            # http://matplotlib.org/users/pyplot_tutorial.html
            line = mpl.lines.Line2D(x_vec*mapscale, y_vec*mapscale, color=color,
                                    linewidth=linewidth,
                                    antialiased=is_antialiased,
                                    alpha=alpha,
                                    solid_capstyle='round',
                                    zorder=0,
                                    )
            ax.add_line(line)

            if printformat is not '':
                angles_text = np.arctan2(deltay, deltax)+deltaangle_text
                # print '  angles_text',printformat%value,angles_text/(np.pi)*180,(angles_text>np.pi/2),(angles_text<3*np.pi/2)
                if (angles_text > np.pi/2) | (angles_text < -np.pi/2):
                    angles_text += np.pi
                x_label = x_vec[0]+0.66*deltax+displacement_text*np.cos(angles_text)
                y_label = y_vec[0]+0.66*deltay+displacement_text*np.sin(angles_text)

                ax.text(x_label*mapscale, y_label*mapscale, printformat % value,
                        rotation=angles_text/(np.pi)*180,
                        color=color_label,
                        fontsize=fontsize_ticks,
                        zorder=10,
                        )

    if is_fill & is_colorbar:
        cbar = plt.colorbar(sm)
        # mpl.setp(cbar.ax.yaxis.get_ticklabels(),  fontsize=fontsize)#weight='bold',
        # cb.ax.tick_params(labelsize=font_size)
        if valuelabel != '':
            cbar.ax.set_ylabel(valuelabel, fontsize=fontsize)  # , weight="bold")
        for l in cbar.ax.yaxis.get_ticklabels():
            # l.set_weight("bold")
            l.set_fontsize(fontsize_ticks)


def plot_connectionvalues_lines(ax, ids_result=None, config_ids_connection=None, values=[],
                                ids_connection=None, connections=None, net=None,
                                width_max=10.0, alpha=0.8, printformat='%.2f',
                                color_outline=None, color_fill=None,
                                color_label='black', is_antialiased=True,
                                is_fill=True, is_widthvalue=True,
                                arrowshape='left', length_arrowhead=10.0,
                                headwidthstretch=1.3,
                                fontsize=32,
                                valuelabel='',
                                value_range=None,
                                is_colorbar=True,
                                mapscale=1.0):
    # ATTENTION: do not change order of args for backwards compatibility

    head_width = headwidthstretch*width_max
    fontsize_ticks = int(0.8*fontsize)

    #values = config_values[ids_result]
    values_norm = np.array(values, dtype=np.float32)/np.max(values)

    patches = []
    displacement = float(width_max)/2.0
    if is_widthvalue:
        linewidths = width_max*values_norm
    else:
        linewidths = width_max * np.ones(len(values_norm), np.float32)

    deltaangle_text = -np.pi/2.0
    displacement_text = displacement+width_max

    jet = cm_jet = plt.get_cmap('jet')
    if value_range is None:
        c_norm = colors.Normalize(vmin=0, vmax=np.max(values))
    else:
        c_norm = colors.Normalize(vmin=value_range[0], vmax=value_range[1])

    # http://stackoverflow.com/questions/8342549/matplotlib-add-colorbar-to-a-sequence-of-line-plots/11558629#11558629
    sm = cmx.ScalarMappable(norm=c_norm, cmap=jet)
    sm.set_array(values_norm)
    for id_connection, value, value_norm, linewidth in zip(ids_connection, values, values_norm, linewidths):
        if not np.isnan(value):
            shape, angles_perb = get_resultshape_connections(net, id_connection, displacement)
            x_vec = np.array(shape)[:, 0]
            y_vec = np.array(shape)[:, 1]
            deltax = x_vec[-1]-x_vec[0]
            deltay = y_vec[-1]-y_vec[0]
            if is_fill:  # means that fill with value proportional color
                color = sm.to_rgba(value)
            else:
                color = color_fill

            # http://matplotlib.org/users/pyplot_tutorial.html
            line = mpl.lines.Line2D(x_vec*mapscale, y_vec*mapscale, color=color,
                                    linewidth=linewidth,
                                    antialiased=is_antialiased,
                                    alpha=alpha,
                                    solid_capstyle='round',
                                    zorder=0,
                                    )
            ax.add_line(line)

            if printformat is not '':
                angles_text = np.arctan2(deltay, deltax)+deltaangle_text
                # print '  angles_text',printformat%value,angles_text/(np.pi)*180,(angles_text>np.pi/2),(angles_text<3*np.pi/2)
                if (angles_text > np.pi/2) | (angles_text < -np.pi/2):
                    angles_text += np.pi
                x_label = x_vec[0]+0.66*deltax+displacement_text*np.cos(angles_text)
                y_label = y_vec[0]+0.66*deltay+displacement_text*np.sin(angles_text)

                ax.text(x_label*mapscale, y_label*mapscale, printformat % value,
                        rotation=angles_text/(np.pi)*180,
                        color=color_label,
                        fontsize=fontsize_ticks,
                        zorder=10,
                        )

    if is_fill & is_colorbar:
        cbar = plt.colorbar(sm)
        # mpl.setp(cbar.ax.yaxis.get_ticklabels(),  fontsize=fontsize)#weight='bold',
        # cb.ax.tick_params(labelsize=font_size)
        if valuelabel != '':
            cbar.ax.set_ylabel(valuelabel, fontsize=fontsize)  # , weight="bold")
        for l in cbar.ax.yaxis.get_ticklabels():
            # l.set_weight("bold")
            l.set_fontsize(fontsize_ticks)


def plot_edgevalues_arrows(ax, ids_result=None, config_ids_edge=None, values=[],
                           ids_edge=None, edges=None,
                           width_max=10.0, alpha=0.8, printformat='%.2f',
                           color_outline=None, color_fill=None,
                           color_label='black', is_antialiased=True,
                           is_fill=True, is_widthvalue=True,
                           arrowshape='left', length_arrowhead=10.0,
                           headwidthstretch=1.3,
                           fontsize=32,
                           valuelabel='',
                           value_range=None,
                           mapscale=1.0):

    # ATTENTION: do not change order of args for backwards compatibility
    print 'plot_edgevalues_arrows ids_result width_max', width_max
    # print '  ids_result',ids_result
    # print '  ids_edge',ids_edge
    if ids_edge is None:
        edges = config_ids_edge.get_linktab()
        ids_edge = config_ids_edge[ids_result]

    head_width = headwidthstretch*width_max
    fontsize_ticks = int(0.8*fontsize)

    #values = config_values[ids_result]
    values_norm = np.array(values, dtype=np.float32)/np.max(values)

    patches = []
    displacement = float(width_max)/4.0
    if is_widthvalue:
        linewidths = width_max*values_norm
    else:
        linewidths = width_max * np.ones(len(values_norm), np.float32)

    deltaangle_text = -np.pi/2.0
    displacement_text = displacement+width_max
    for id_edge, value, value_norm, linewidth in zip(ids_edge, values, values_norm, linewidths):
        if not np.isnan(value):
            shape, angles_perb = get_resultshape(edges, id_edge, displacement)
            x_vec = np.array(shape)[:, 0]
            y_vec = np.array(shape)[:, 1]
            deltax = x_vec[-1]-x_vec[0]
            deltay = y_vec[-1]-y_vec[0]

            if printformat is not '':
                angles_text = np.arctan2(deltay, deltax)+deltaangle_text
                if (angles_text > np.pi/2) | (angles_text < -np.pi/2):
                    angles_text += np.pi
                x_label = x_vec[0]+0.66*deltax+displacement_text*np.cos(angles_text)
                y_label = y_vec[0]+0.66*deltay+displacement_text*np.sin(angles_text)

                ax.text(x_label*mapscale, y_label*mapscale, printformat % value,
                        rotation=angles_text/(np.pi)*180,
                        color=color_label,
                        fontsize=fontsize_ticks,
                        zorder=10,
                        )

            if is_widthvalue:
                head_width = headwidthstretch*linewidth
            arrow = FancyArrow(x_vec[0]*mapscale, y_vec[0]*mapscale, deltax*mapscale, deltay*mapscale, width=linewidth*mapscale,
                               antialiased=is_antialiased,
                               edgecolor=color_outline, facecolor=color_fill,
                               head_width=head_width, head_length=length_arrowhead,
                               length_includes_head=True,
                               fill=True, shape=arrowshape, zorder=0)
            patches.append(arrow)

    if is_fill:
        alpha_patch = alpha
        patchcollection = PatchCollection(patches, cmap=mpl.cm.jet, alpha=alpha_patch)
        patchcollection.set_array(values)

        # custom value range
        if value_range is not None:
            patchcollection.set_clim(value_range)

        ax.add_collection(patchcollection)
        cbar = plt.colorbar(patchcollection)
        if valuelabel != '':
            cbar.ax.set_ylabel(valuelabel, fontsize=fontsize)  # , weight="bold")
        for l in cbar.ax.yaxis.get_ticklabels():
            # l.set_weight("bold")
            l.set_fontsize(fontsize_ticks)
    else:
        for patch in patches:
            ax.add_patch(patch)


def plot_connectionvalues_arrows(ax, ids_result=None, config_ids_edge=None, values=[],
                                 ids_connection=None, connections=None, net=None,
                                 width_max=10.0, alpha=0.8, printformat='%.2f',
                                 color_outline=None, color_fill=None,
                                 color_label='black', is_antialiased=True,
                                 is_fill=True, is_widthvalue=True,
                                 arrowshape='left', length_arrowhead=10.0,
                                 headwidthstretch=1.3,
                                 fontsize=32,
                                 valuelabel='',
                                 value_range=None,
                                 mapscale=1.0):

    # ATTENTION: do not change order of args for backwards compatibility
    print 'plot_connectionvalues_arrows ids_result'

    head_width = headwidthstretch*width_max
    fontsize_ticks = int(0.8*fontsize)

    #values = config_values[ids_result]
    values_norm = np.array(values, dtype=np.float32)/np.max(values)

    patches = []
    displacement = float(width_max)/4.0
    if is_widthvalue:
        linewidths = width_max*values_norm
    else:
        linewidths = width_max * np.ones(len(values_norm), np.float32)

    deltaangle_text = -np.pi/2.0
    displacement_text = displacement+width_max
    for id_connection, value, value_norm, linewidth in zip(ids_connection, values, values_norm, linewidths):
        if not np.isnan(value):
            shape, angles_perb = get_resultshape_connections(net, id_connection, displacement)
            x_vec = np.array(shape)[:, 0]
            y_vec = np.array(shape)[:, 1]
            deltax = x_vec[-1]-x_vec[0]
            deltay = y_vec[-1]-y_vec[0]

            if printformat is not '':
                angles_text = np.arctan2(deltay, deltax)+deltaangle_text
                if (angles_text > np.pi/2) | (angles_text < -np.pi/2):
                    angles_text += np.pi
                x_label = x_vec[0]+0.66*deltax+displacement_text*np.cos(angles_text)
                y_label = y_vec[0]+0.66*deltay+displacement_text*np.sin(angles_text)

                ax.text(x_label*mapscale, y_label*mapscale, printformat % value,
                        rotation=angles_text/(np.pi)*180,
                        color=color_label,
                        fontsize=fontsize_ticks,
                        zorder=10,
                        )

            if is_widthvalue:
                head_width = headwidthstretch*linewidth
            arrow = FancyArrow(x_vec[0]*mapscale, y_vec[0]*mapscale, deltax*mapscale, deltay*mapscale, width=linewidth*mapscale,
                               antialiased=is_antialiased,
                               edgecolor=color_outline, facecolor=color_fill,
                               head_width=head_width, head_length=length_arrowhead,
                               length_includes_head=True,
                               fill=True, shape=arrowshape, zorder=0)
            patches.append(arrow)

    if is_fill:
        alpha_patch = alpha
        patchcollection = PatchCollection(patches, cmap=mpl.cm.jet, alpha=alpha_patch)
        patchcollection.set_array(values)

        # custom value range
        if value_range is not None:
            patchcollection.set_clim(value_range)

        ax.add_collection(patchcollection)
        cbar = plt.colorbar(patchcollection)
        if valuelabel != '':
            cbar.ax.set_ylabel(valuelabel, fontsize=fontsize)  # , weight="bold")
        for l in cbar.ax.yaxis.get_ticklabels():
            # l.set_weight("bold")
            l.set_fontsize(fontsize_ticks)
    else:
        for patch in patches:
            ax.add_patch(patch)


def plot_nodevalues(ax, ids_result=None, config_ids_edge=None, values=[],
                    ids_node=None, nodes=None, net=None,
                    ##                            width_max = 10.0,
                    alpha=0.8,
                    printformat='%.2f',
                    ##                            color_outline = None,
                    ##                            color_fill = None,
                    ##                            color_label = 'black',
                    ##                            is_antialiased = True,
                    is_fill=True,
                    ##                            is_widthvalue = True,
                    ##                            arrowshape = 'left',
                    ##                            length_arrowhead = 10.0,
                    ##                            headwidthstretch = 1.3,
                    fontsize=32,
                    valuelabel='',
                    value_range=None,
                    mapscale=1.0):

    # ATTENTION: do not change order of args for backwards compatibility
    print 'plot_nodevalues ids_result'
    fontsize_ticks = int(0.8*fontsize)

    patches = []
    for id_node, value, in zip(ids_node, values):

        if not np.isnan(value):
            poly = mpl.patches.Polygon(np.transpose(np.stack((np.array(net.nodes.shapes[id_node])[
                                       :][:, 0]*mapscale, np.array(net.nodes.shapes[id_node])[:][:, 1]*mapscale))))
            patches.append(poly)

    if is_fill:
        alpha_patch = alpha
        patchcollection = PatchCollection(patches, cmap=mpl.cm.jet, alpha=alpha_patch)
        patchcollection.set_array(values)

        # custom value range
        if value_range is not None:
            patchcollection.set_clim(value_range)

        ax.add_collection(patchcollection)
        cbar = plt.colorbar(patchcollection)
        if valuelabel != '':
            cbar.ax.set_ylabel(valuelabel, fontsize=fontsize)  # , weight="bold")
        for l in cbar.ax.yaxis.get_ticklabels():
            # l.set_weight("bold")
            l.set_fontsize(fontsize_ticks)
    else:
        for patch in patches:
            ax.add_patch(patch)


def plot_pointvalues(ax, x_coords, y_coords,  values,
                     net=None,
                     width_max=10.0,
                     alpha=0.8,
                     printformat='%.2f',
                     ##                            color_outline = None,
                     ##                            color_fill = None,
                     ##                            color_label = 'black',
                     ##                            is_antialiased = True,
                     is_fill=True,
                     is_widthvalue=True,
                     ##                            arrowshape = 'left',
                     ##                            length_arrowhead = 10.0,
                     ##                            headwidthstretch = 1.3,
                     fontsize=32,
                     valuelabel='',
                     value_range=None,
                     mapscale=1.0
                     ):

    # ATTENTION: do not change order of args for backwards compatibility
    print 'plot_pointvalues ids_result'
    fontsize_ticks = int(0.8*fontsize)
##    i = 0
    patches = []
    for x, y, value in zip(x_coords*mapscale, y_coords*mapscale, values):

        if not np.isnan(value):
            if is_widthvalue:
                poly = mpl.patches.Circle([x, y], radius=value*width_max/np.max(values)*mapscale, )
            else:
                poly = mpl.patches.Circle([x, y], radius=width_max*mapscale)
            patches.append(poly)
# print i
# i+=1

    if is_fill:
        alpha_patch = alpha
        patchcollection = PatchCollection(patches, cmap=mpl.cm.jet, alpha=alpha_patch)
        patchcollection.set_array(values)

        # custom value range
        if value_range is not None:
            patchcollection.set_clim(value_range)

        ax.add_collection(patchcollection)
        cbar = plt.colorbar(patchcollection)
        if valuelabel != '':
            cbar.ax.set_ylabel(valuelabel, fontsize=fontsize)  # , weight="bold")
        for l in cbar.ax.yaxis.get_ticklabels():
            # l.set_weight("bold")
            l.set_fontsize(fontsize_ticks)
    else:
        for patch in patches:
            ax.add_patch(patch)


def get_resultshape(edges, id_edge, dispacement):
    """
    Return resultshape coords for this edge.
    """
    # print 'get_resultshape',edges,'id_edge',id_edge,'dispacement', dispacement
    shape = np.array(edges.shapes[id_edge], np.float32)
    n_vert = len(shape)
    resultshape = np.zeros(shape.shape, np.float32)
    #laneshapes = np.zeros((n_lanes,n_vert,3), np.float32)
    angles_perb = get_angles_perpendicular(shape)
    if edges.types_spread[id_edge] == 0:  # right spread

        resultshape[:, 0] = shape[:, 0] + np.cos(angles_perb) * dispacement
        resultshape[:, 1] = shape[:, 1] + np.sin(angles_perb) * dispacement
        return resultshape, angles_perb
    else:
        return shape.copy(), angles_perb


def get_resultshape_connections(net, id_connection, dispacement):
    """
    Return resultshape coords for this edge.
    """
    print 'get_resultshape', 'connections', 'id_connection', id_connection, 'dispacement', dispacement
    x0 = net.edges.shapes[net.lanes.ids_edge[net.connections.ids_fromlane[id_connection]]][-1, 0]
    y0 = net.edges.shapes[net.lanes.ids_edge[net.connections.ids_fromlane[id_connection]]][-1, 1]
    x1 = net.edges.shapes[net.lanes.ids_edge[net.connections.ids_tolane[id_connection]]][0, 0]
    y1 = net.edges.shapes[net.lanes.ids_edge[net.connections.ids_tolane[id_connection]]][0, 1]
    if x0 != x1 or y0 != y1:
        shape = np.array([[x0, y0, 0.], [x1, y1, 0.]])
    else:
        shape = np.array([[x0, y0, 0.], [x1 + 1., y1 + 1., 0.]])
        print 'connection', id_connection, 'has no shape'
    n_vert = len(shape)
    resultshape = np.zeros(shape.shape, np.float32)
    #laneshapes = np.zeros((n_lanes,n_vert,3), np.float32)
    angles_perb = get_angles_perpendicular(shape)

    return shape.copy(), angles_perb


def show_plot():
    plt.show()


class PlotoptionsMixin:

    def add_plotoptions(self, **kwargs):
        """
        [depricated]
        These are plotoptions for edge results shown on a map. 
        """
        attrsman = self.get_attrsman()
        self.add_edgeresultoptions(**kwargs)
        self.add_networkoptions(**kwargs)
        self.add_facilityoptions(**kwargs)
        self.add_zoneoptions(**kwargs)
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)

    def add_edgeresultoptions(self, **kwargs):
        attrsman = self.get_attrsman()
        plottypes = ['arrows', 'polygons']
        self.plottype = attrsman.add(cm.AttrConf('plottype', kwargs.get('plottype', 'polygons'),
                                                 choices=plottypes,
                                                 groupnames=['options'],
                                                 name='Plot types',
                                                 info='Plot type representing the results.',
                                                 ))

        self.resultwidth = attrsman.add(cm.AttrConf('resultwidth', kwargs.get('resultwidth', 4.0),
                                                    groupnames=['options'],
                                                    name='Result width',
                                                    unit='m',
                                                    info='Maximum width of graphical resuls on map.',
                                                    ))

        self.length_arrowhead = attrsman.add(cm.AttrConf('length_arrowhead', kwargs.get('length_arrowhead', 10.0),
                                                         groupnames=['options'],
                                                         name='Arrow length',
                                                         unit='m',
                                                         info='Length of arrowhead on result map.',
                                                         ))

        self.is_widthvalue = attrsman.add(cm.AttrConf('is_widthvalue', kwargs.get('is_widthvalue', True),
                                                      groupnames=['options'],
                                                      name='Value width?',
                                                      info='If True, the arrow width of the graphical representation is proportional to the result value.',
                                                      ))

        self.is_colorvalue = attrsman.add(cm.AttrConf('is_colorvalue', kwargs.get('is_colorvalue', True),
                                                      groupnames=['options'],
                                                      name='Value color?',
                                                      info='If True, the arrows of the graphical representation are filled with a colour representing the result value.',
                                                      ))

        self.is_value_range = attrsman.add(cm.AttrConf('is_value_range', kwargs.get('is_value_range', False),
                                                       groupnames=['options'],
                                                       name='Manual value range?',
                                                       info='If True, value range is set manually, otherwise value range is adjusted according to values.',
                                                       ))

        self.value_range_min = attrsman.add(cm.AttrConf('value_range_min', kwargs.get('value_range_min', 1.0),
                                                        groupnames=['options'],
                                                        name='Min. value range',
                                                        info='Minimum value range, if manual value range is selected.',
                                                        ))

        self.value_range_max = attrsman.add(cm.AttrConf('value_range_max', kwargs.get('value_range_max', 40.0),
                                                        groupnames=['options'],
                                                        name='Max. value range',
                                                        info='Maximum value range, if manual value range is selected.',
                                                        ))
        self.color_outline = attrsman.add(cm.AttrConf('color_outline', kwargs.get('color_outline', np.array([0.0, 0.0, 0.0, 0.95], dtype=np.float32)),
                                                      groupnames=['options'],
                                                      perm='wr',
                                                      metatype='color',
                                                      name='Outline color',
                                                      info='Outline color of result arrows in graphical representation. Only valid if no color-fill is chosen.',
                                                      ))

        self.color_fill = attrsman.add(cm.AttrConf('color_fill', kwargs.get('color_fill', np.array([0.3, 0.3, 1.0, 0.95], dtype=np.float32)),
                                                   groupnames=['options'],
                                                   perm='wr',
                                                   metatype='color',
                                                   name='Fill color',
                                                   info='Fill color of result arrows in graphical representation.',
                                                   ))

        self.valuelabel = attrsman.add(cm.AttrConf('valuelabel', kwargs.get('valuelabel',  ''),
                                                   groupnames=['options'],
                                                   name='Value label',
                                                   info='Label of results, as shown in the plot. Leave blank for default values.',
                                                   ))

        self.alpha_results = attrsman.add(cm.AttrConf('alpha_results', kwargs.get('alpha_results',  0.5),
                                                      groupnames=['options'],
                                                      name='Result transparency',
                                                      info='Transparency of result objects in graphical representation.',
                                                      ))

    def add_networkoptions(self, **kwargs):
        """
        Network plot options
        """
        attrsman = self.get_attrsman()
        self.is_show_network = attrsman.add(cm.AttrConf('is_show_network', kwargs.get('is_show_network', True),
                                                        groupnames=['options', 'network'],
                                                        name='Show network',
                                                        info='Shows a schematic network in the background.',
                                                        ))

        self.is_show_connections = attrsman.add(cm.AttrConf('is_show_connections', kwargs.get('is_show_connections', False),
                                                            groupnames=['options', 'network'],
                                                            name='Show network-connections',
                                                            info='Shows connections between edges in the network.',
                                                            ))

        self.color_network = attrsman.add(cm.AttrConf('color_network', kwargs.get('color_network', np.array([0.8, 0.8, 0.8, 0.8], dtype=np.float32)),
                                                      groupnames=['options', 'network'],
                                                      perm='wr',
                                                      metatype='color',
                                                      name='Network color',
                                                      info='Outline color of schematic network in the background.',
                                                      ))

        self.color_nodes = attrsman.add(cm.AttrConf('color_nodes', kwargs.get('color_nodes', np.array([1, 1, 1, 1], dtype=np.float32)),
                                                    groupnames=['options', 'network'],
                                                    perm='wr',
                                                    metatype='color',
                                                    name='Nodes color',
                                                    info='Color of simplified nodes (or juctions in the background.',
                                                    ))
        self.alpha_net = attrsman.add(cm.AttrConf('alpha_net', kwargs.get('alpha_net', 0.5),
                                                  groupnames=['options', 'network'],
                                                  name='Net transparency',
                                                  info='Transparency of network (edges and nodes) in graphical representation.',
                                                  ))
        self.is_show_maps = attrsman.add(cm.AttrConf('is_show_maps', kwargs.get('is_show_maps', False),
                                                     groupnames=['options', 'network'],
                                                     name='Show map?',
                                                     info='If True, shows map as background in graphical representation. This feature requires that maps have been previously downloaded.',
                                                     ))

        self.alpha_maps = attrsman.add(cm.AttrConf('alpha_maps', kwargs.get('alpha_maps', 0.5),
                                                   groupnames=['options', 'network'],
                                                   name='Map transparency',
                                                   info='Transparency of background maps in graphical representation.',
                                                   ))

    def add_facilityoptions(self, **kwargs):
        attrsman = self.get_attrsman()
        self.is_show_facilities = attrsman.add(cm.AttrConf('is_show_facilities', kwargs.get('is_show_facilities', False),
                                                           groupnames=['options', 'facilities'],
                                                           name='Show facilities',
                                                           info='Shows a schematic facilities (buildings, parks, etc.) in the background.',
                                                           ))

        self.color_facilities = attrsman.add(cm.AttrConf('color_facilities',  kwargs.get('color_facilities', np.array([1, 1, 1, 1], dtype=np.float32)),
                                                         groupnames=['options', 'facilities'],
                                                         perm='wr',
                                                         metatype='color',
                                                         name='Facilities color',
                                                         info='Color of schematic facilities in the background.',
                                                         ))

        self.alpha_facilities = attrsman.add(cm.AttrConf('alpha_facilities', kwargs.get('alpha_facilities', 0.5),
                                                         groupnames=['options', 'facilities'],
                                                         name='Facility transparency',
                                                         info='Transparency of facilities in graphical representation.',
                                                         ))

        self.color_borders = attrsman.add(cm.AttrConf('color_borders', kwargs.get('color_borders', np.array([0.7, 0.7, 0.7, 0.8], dtype=np.float32)),
                                                      groupnames=['options', 'facilities'],
                                                      perm='wr',
                                                      metatype='color',
                                                      name='Facility border color',
                                                      info='Facility border (or building walls) color of schematic facilities in the background.',
                                                      ))

    def add_zoneoptions(self, **kwargs):
        attrsman = self.get_attrsman()
        self.is_show_zones = attrsman.add(cm.AttrConf('is_show_zones', kwargs.get('is_show_zones', False),
                                                      groupnames=['options', 'zones'],
                                                      name='Show Zones',
                                                      info='Show zone polygons.',
                                                      ))

        self.color_zones = attrsman.add(cm.AttrConf('color_zones',  kwargs.get('color_zones', np.array([0.2, 0.8, 0.2, 1.0], dtype=np.float32)),
                                                    groupnames=['options', 'zones'],
                                                    perm='wr',
                                                    metatype='color',
                                                    name='Zone color',
                                                    info='Fill color of Zone.',
                                                    ))

        self.alpha_zones = attrsman.add(cm.AttrConf('alpha_zones', kwargs.get('alpha_zones', 0.1),
                                                    groupnames=['options', 'zones'],
                                                    name='Zone transparency',
                                                    info='Transparency of color of zones.',
                                                    ))

        self.color_zoneborders = attrsman.add(cm.AttrConf('color_zoneborders', kwargs.get('color_zoneborders', np.array([0.0, 0.7, 0.0, 1.0], dtype=np.float32)),
                                                          groupnames=['options', 'zones'],
                                                          perm='wr',
                                                          metatype='color',
                                                          name='Zone border color',
                                                          info='Color of zone border.',
                                                          ))

    def add_plotoptions_lineplot(self, is_marker_only=False, **kwargs):
        """
        These are plotoptions for y over x oriented graphs. 
        """
        attrsman = self.get_attrsman()

        if not is_marker_only:
            self.color_line = attrsman.add(cm.AttrConf('color_line', kwargs.get('color_line', np.array([1, 0, 0, 1], dtype=np.float32)),
                                                       groupnames=['options'],
                                                       perm='wr',
                                                       metatype='color',
                                                       name='Line color',
                                                       info='Color of line in various diagrams.',
                                                       ))

            self.width_line = attrsman.add(cm.AttrConf('width_line', kwargs.get('width_line', 2),
                                                       groupnames=['options'],
                                                       perm='wr',
                                                       name='Line width',
                                                       info='Width of plotted lines.',
                                                       ))

            self.style_line = attrsman.add(cm.AttrConf('style_line', kwargs.get('style_line', '-'),
                                                       choices=LINESTYLES,
                                                       groupnames=['options'],
                                                       name='Line Style',
                                                       info='Line styles used for plots.',
                                                       ))

            self.alpha_line = attrsman.add(cm.AttrConf('alpha_line', kwargs.get('alpha_line', 0.9),
                                                       groupnames=['options'],
                                                       name='Line transparency',
                                                       info='Line transparency used for plots.',
                                                       ))

        if is_marker_only:
            style_marker_default = 'o'
        else:
            style_marker_default = 'o'

        self.style_marker = attrsman.add(cm.AttrConf('style_marker', kwargs.get('style_marker', style_marker_default),
                                                     choices=MARKERSTYLES,
                                                     groupnames=['options'],
                                                     name='Marker Style',
                                                     info='Marker style used for plots.',
                                                     ))

        self.color_marker = attrsman.add(cm.AttrConf('color_marker', kwargs.get('color_marker', np.array([0, 0, 1, 1], dtype=np.float32)),
                                                     groupnames=['options'],
                                                     perm='wr',
                                                     metatype='color',
                                                     name='Marker color',
                                                     info='Color of marker in plot.',
                                                     ))

        self.size_marker = attrsman.add(cm.AttrConf('size_marker', kwargs.get('size_marker', 16),
                                                    groupnames=['options'],
                                                    perm='wr',
                                                    name='Marker size',
                                                    info='Size of marker in plot.',
                                                    ))

        self.xlabel = attrsman.add(cm.AttrConf('xlabel', kwargs.get('xlabel', ''),
                                               groupnames=['options'],
                                               name='X label',
                                               info='X-axis label text, if blank then default values are used.',
                                               ))

        self.ylabel = attrsman.add(cm.AttrConf('ylabel', kwargs.get('ylabel', ''),
                                               groupnames=['options'],
                                               name='Y label',
                                               info='Y-axis label text, if blank then default values are used.',
                                               ))
        self.add_plotoptions_base(**kwargs)

    def add_plotoptions_mapbase(self, **kwargs):
        """
        These are all necessary option to plot whatever on a map.
        """
        attrsman = self.get_attrsman()
        self.unit_mapscale = attrsman.add(cm.AttrConf('unit_mapscale', kwargs.get('unit_mapscale', 'm'),
                                                      groupnames=['options', 'network'],
                                                      choices=['m', 'km'],
                                                      mapscales={'m': 1.0, 'km': 0.001},
                                                      name='Unit',
                                                      info='Unit used for map.',
                                                      ))

    def add_plotoptions_base(self, **kwargs):
        attrsman = self.get_attrsman()
        self.axis = None
        self.color_background = attrsman.add(cm.AttrConf('color_background', kwargs.get('color_background', np.array([1, 1, 1, 1], dtype=np.float32)),
                                                         groupnames=['options'],
                                                         perm='wr',
                                                         metatype='color',
                                                         name='Background color',
                                                         info='Background color of schematic network in the background.',
                                                         ))

        self.is_grid = attrsman.add(cm.AttrConf('is_grid', kwargs.get('is_grid', False),
                                                groupnames=['options'],
                                                name='Show grid?',
                                                info='If True, shows a grid on the graphical representation.',
                                                ))

        self.is_show_title = attrsman.add(cm.AttrConf('is_show_title', kwargs.get('is_show_title', True),
                                                      groupnames=['options'],
                                                      name='Show title',
                                                      info='Shows title on top of figure.',
                                                      ))

        self.title = attrsman.add(cm.AttrConf('title', kwargs.get('title', ''),
                                              groupnames=['options'],
                                              name='Title',
                                              info='Title text, if blank then default values are used.',
                                              ))

        self.size_titlefont = attrsman.add(cm.AttrConf('size_titlefont', kwargs.get('size_titlefont', 16),
                                                       groupnames=['options'],
                                                       name='Title fontsize',
                                                       info='Title fontsize.',
                                                       ))

        self.size_labelfont = attrsman.add(cm.AttrConf('size_labelfont', kwargs.get('size_labelfont', 14),
                                                       groupnames=['options'],
                                                       name='Label fontsize',
                                                       info='Label fontsize.',
                                                       ))

        self.printformat = attrsman.add(cm.AttrConf('printformat', kwargs.get('printformat', ''),
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

    def add_save_options(self, **kwargs):

        attrsman = self.get_attrsman()
        self.is_save = attrsman.add(cm.AttrConf('is_save', kwargs.get('is_save', False),
                                                groupnames=['options'],
                                                name='Save plots',
                                                info='If true, save plots instead of showing on screen.',
                                                ))

        self.figformat = attrsman.add(cm.AttrConf('figformat', kwargs.get('figformat', 'png'),
                                                  groupnames=['options'],
                                                  choices=['png', 'pdf', 'jpg', 'svg'],
                                                  name='Graphics format',
                                                  info='Format of saved graphic.',
                                                  ))
        self.resolution = attrsman.add(cm.AttrConf('resolution', kwargs.get('resolution', 300),
                                                   groupnames=['options'],
                                                   name='Resolution',
                                                   unit='dpi',
                                                   info='Resolution of saved graphic in dots per inch (dpi).',
                                                   ))
        self.figwidth = attrsman.add(cm.AttrConf('figwidth', kwargs.get('figwidth', 40.0),
                                                 groupnames=['options'],
                                                 name='Figure width',
                                                 unit='cm',
                                                 info='Figure width.',
                                                 ))
        self.figheight = attrsman.add(cm.AttrConf('figheight', kwargs.get('figheight', 30.0),
                                                  groupnames=['options'],
                                                  name='Figure height',
                                                  unit='cm',
                                                  info='Figure height.',
                                                  ))

    def get_scenario(self):
        return self.parent.get_scenario()

    def init_figures(self):
        self._i_fig = 0
        plt.close("all")

    def create_figure(self):
        self._i_fig += 1
        fig = plt.figure(self._i_fig,
                         # dpi=self.resolution,#this may cause huge windows in interactive mode
                         tight_layout=True,
                         figsize=(self.figwidth*0.393701, self.figheight*0.393701)
                         )
        return fig

    def _autolabel_bars(self, ax, bars, labels):
        """
        Helperfunction: Attach a text label above each bar displaying its height
        """
        for rect, label in zip(bars, labels):
            height = rect.get_height()
            if not np.isnan(height):
                ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                        '%s' % label,
                        ha='center', va='bottom',
                        fontsize=int(0.8*self.size_labelfont),
                        )

    def do(self):
        """"This method calls the show method which should be overriden
        by to execute class  specific plot actions."""
        # print 'do',self.edgeattrname
        self.show()
        return True

    def set_figmargins(self):
        plt.subplots_adjust(left=0.1, right=0.9, top=0.9, bottom=0.1)

    def save_fig(self, figname):

        rootfilepath = self.get_scenario().get_rootfilepath()
        filepath = "%s_%s.%s" % (rootfilepath, figname, self.figformat)
        print 'save_fig', filepath
        plt.savefig(filepath, format=self.figformat,
                    dpi=self.resolution,
                    # orientation='landscape',
                    orientation='portrait',
                    transparent=True)

    def get_edgeresults(self):
        """
        Returns table with results which must have the attribute 'ids_edge'
        """
        return self.parent.edgeresults

    def set_axisborder(self, ax):
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.spines['bottom'].set_linewidth(0.5)
        ax.spines['left'].set_linewidth(0.5)

    def plot_hist(self, ax, x, bins=None, range=None,
                  color='blue', facecolor=None,
                  alpha=0.75, label=None,
                  histtype='step',  # {'bar', 'barstacked', 'step', 'stepfilled'}
                  is_normed=True,
                  is_cumulative=True,
                  is_show_mean=True,
                  is_show_std=False,
                  is_rel_frequ=False,
                  is_percent=False,
                  ):

        # the histogram of the data
        # print 'plot_hist',bins,range
        if bins is None:
            bins = self.n_bins

        if color is None:
            color = self.color_line

        if is_rel_frequ:
            is_cumulative = False
            is_normed = False
            weights = np.zeros_like(x) + 1. / x.size
        else:
            weights = np.zeros_like(x) + 1.

        if is_percent:
            weights *= 100

        n, bins0, patches = ax.hist(x, bins, range=range,
                                    normed=is_normed,
                                    cumulative=is_cumulative,
                                    histtype=histtype,
                                    linewidth=self.width_line,
                                    color=color, facecolor=facecolor,
                                    alpha=alpha, label=label,
                                    weights=weights,
                                    )

        mean = np.mean(x)
        std = np.std(x)
        y_min, y_max = ax.get_ylim()
        x_min = np.min(x)
        x_max = np.max(x)
        dy = 0.05*(y_max-y_min)
        dx = 0.05*(x_max-x_min)
        if is_show_mean:
            ax.plot([mean, mean], [y_min, y_max], color=color, linestyle='-.', linewidth=2*self.width_line)
            # very good: https://matplotlib.org/users/text_intro.html
            # ax.text(3, 8, 'boxed italics text in data coords', style='italic',
            #        bbox={'facecolor':'red', 'alpha':0.5, 'pad':10})
            # ax.annotate(r'$\mu=%.2f$'%mean, xy=(mean, y_max-dy),
            #                xytext=(mean-dx, y_max-2*dy),
            #                fontsize=int(0.8*self.size_labelfont),
            #                arrowprops=dict(facecolor='black', shrink=0.05)
            #                )

        if is_show_std:
            ax.plot([mean-std, mean-std], [y_min, y_max], color=color, linestyle='--', linewidth=self.width_line)
            ax.plot([mean+std, mean+std], [y_min, y_max], color=color, linestyle='--', linewidth=self.width_line)

        # hist uses np.histogram under the hood to create 'n' and 'bins'.
        # np.histogram returns the bin edges, so there will be 50 probability
        # density values in n, 51 bin edges in bins and 50 patches.  To get
        # everything lined up, we'll compute the bin centers
        bincenters = 0.5*(bins0[1:]+bins0[:-1])
        return bincenters

    def plot_results_on_map(self, axis, ids_edgeres=None, values=[],
                            ids_edge=None,
                            title='', valuelabel='',
                            is_connection_results=False, ids_connectionres=None, ids_connection=None):

        # axis, ids, resultattrconf[ids] , title, valuelabel = resultattrconf.format_symbol()

        if self.valuelabel != "":
            valuelabel = self.valuelabel

        if is_connection_results:
            if ids_connection is None:
                ids_connection = self.get_connectionresults().ids_connection[ids_connectionres]
        else:
            if ids_edge is None:
                ids_edge = self.get_edgeresults().ids_edge[ids_edgeres]

        net = self.get_scenario().net

        unit = self.unit_mapscale
        mapscale = self.get_attrsman().get_config('unit_mapscale').mapscales[unit]
        # print '  unit',unit
        # print '  mapscale',mapscale
        #xticks =axis.get_xticks()
        #yticks =axis.get_yticks()
        # print 'xticks',xticks
        # print 'yticks',yticks
        self.plot_net(axis, mapscale=mapscale,  unit=unit, is_configure=False)

        if self.is_value_range:
            value_range = (self.value_range_min, self.value_range_max)
        else:
            value_range = None

        if is_connection_results:
            if self.plottype == 'arrows':  # = ['Arrows','Polygons']
                plot_connectionvalues_arrows(axis,
                                             # ids,
                                             # edgesresults.ids_edge,
                                             ids_connection=ids_connection,
                                             values=values,
                                             connections=net.connections,
                                             net=net,
                                             width_max=self.resultwidth,
                                             alpha=self.alpha_results,
                                             printformat=self.printformat,
                                             color_outline=self.color_outline,
                                             color_fill=self.color_fill,
                                             color_label=self.color_label,
                                             is_antialiased=True,
                                             is_fill=self.is_colorvalue,
                                             is_widthvalue=self.is_widthvalue,
                                             length_arrowhead=self.length_arrowhead,
                                             fontsize=self.size_labelfont,
                                             valuelabel=valuelabel,
                                             value_range=value_range,
                                             mapscale=mapscale)
            elif self.plottype == 'polygons':
                plot_connectionvalues_lines(axis,
                                            # ids,
                                            # edgesresults.ids_edge,
                                            ids_connection=ids_connection,
                                            values=values,
                                            connections=net.connections,
                                            net=net,
                                            width_max=self.resultwidth,
                                            alpha=self.alpha_results,
                                            printformat=self.printformat,
                                            color_outline=self.color_outline,
                                            color_fill=self.color_fill,
                                            color_label=self.color_label,
                                            is_antialiased=True,
                                            is_fill=self.is_colorvalue,
                                            is_widthvalue=self.is_widthvalue,
                                            length_arrowhead=self.length_arrowhead,
                                            fontsize=self.size_labelfont,
                                            valuelabel=valuelabel,
                                            value_range=value_range,
                                            mapscale=mapscale)
        else:
            if len(ids_edge) > 0:
                if self.plottype == 'arrows':  # = ['Arrows','Polygons']
                    plot_edgevalues_arrows(axis,
                                           # ids,
                                           # edgesresults.ids_edge,
                                           ids_edge=ids_edge,
                                           values=values,
                                           edges=net.edges,
                                           width_max=self.resultwidth,
                                           alpha=self.alpha_results,
                                           printformat=self.printformat,
                                           color_outline=self.color_outline,
                                           color_fill=self.color_fill,
                                           color_label=self.color_label,
                                           is_antialiased=True,
                                           is_fill=self.is_colorvalue,
                                           is_widthvalue=self.is_widthvalue,
                                           length_arrowhead=self.length_arrowhead,
                                           fontsize=self.size_labelfont,
                                           valuelabel=valuelabel,
                                           value_range=value_range,
                                           mapscale=mapscale)
                elif self.plottype == 'polygons':
                    plot_edgevalues_lines(axis,
                                          # ids,
                                          # edgesresults.ids_edge,
                                          ids_edge=ids_edge,
                                          values=values,
                                          edges=net.edges,
                                          width_max=self.resultwidth,
                                          alpha=self.alpha_results,
                                          printformat=self.printformat,
                                          color_outline=self.color_outline,
                                          color_fill=self.color_fill,
                                          color_label=self.color_label,
                                          is_antialiased=True,
                                          is_fill=self.is_colorvalue,
                                          is_widthvalue=self.is_widthvalue,
                                          length_arrowhead=self.length_arrowhead,
                                          fontsize=self.size_labelfont,
                                          valuelabel=valuelabel,
                                          value_range=value_range,
                                          mapscale=mapscale)

        self.configure_map(axis, title, unit=unit)

    def configure_map(self, axis, title=None, unit='m'):

        if self.is_show_title:
            if self.title != "":
                title = self.title

            axis.set_title(title, fontsize=self.size_titlefont)

        axis.axis('equal')
        # ax.legend(loc='best',shadow=True)

        axis.grid(self.is_grid)
        axis.set_xlabel('West-East ['+unit+']', fontsize=self.size_labelfont)
        axis.set_ylabel('South-North ['+unit+']', fontsize=self.size_labelfont)
        axis.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        axis.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

        # get borders right, this seems to work
        plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)

        # show_plot()

        #x_min,y_min,x_max,y_max = net.get_boundaries(is_netboundaries = True)
        # axis.set_autoscaley_on(False)
        # axis.set_xlim([x_min,x_max])
        # axis.set_ylim([y_min,y_max])

    def plot_net(self, axis=None, title="", unit='', mapscale=None, is_configure=True):
        print 'plot_net mapscale', mapscale
        if mapscale is None:
            unit = self.unit_mapscale
            mapscale = self.get_attrsman().get_config('unit_mapscale').mapscales[unit]

        if axis is None:
            axis = init_plot()

        if hasattr(self, 'is_show_network'):
            if self.is_show_network:
                net = self.get_scenario().net
                plot_net(axis, net, color_edge=self.color_network, width_edge=2,
                         color_node=self.color_nodes, alpha=self.alpha_net, is_show_connections=self.is_show_connections, mapscale=mapscale)

        if hasattr(self, 'is_show_facilities'):
            if self.is_show_facilities:
                facilities = self.get_scenario().landuse.facilities
                plot_facilities(axis, facilities, color_building=self.color_facilities,
                                color_outline=self.color_borders,
                                width_line=2, alpha=self.alpha_facilities,
                                mapscale=mapscale)
        if hasattr(self, 'is_show_maps'):
            if self.is_show_maps:
                plot_maps(axis, self.get_scenario().landuse.maps, alpha=self.alpha_maps, mapscale=mapscale)

        if hasattr(self, 'is_show_zones'):
            if self.is_show_zones:
                zones = self.get_scenario().landuse.zones
                plot_zones(axis, zones, color_zone=self.color_zones,
                           color_outline=self.color_zoneborders,
                           width_line=2, alpha=self.alpha_zones,
                           mapscale=mapscale)
        if is_configure:
            self.configure_map(axis, title=title, unit=unit)

    def plot_node_results_on_map(self, axis, ids_noderes=None, values=[],
                                 ids_node=None,
                                 title='', valuelabel='',
                                 is_node_results=False, ):

        if ids_node is None:
            ids_node = self.get_noderesults().ids_node[ids_noderes]

        net = self.get_scenario().net

        unit = self.unit_mapscale
        mapscale = self.get_attrsman().get_config('unit_mapscale').mapscales[unit]
        self.plot_net(axis, mapscale=mapscale,  unit=unit, is_configure=False)

        if self.is_value_range:
            value_range = (self.value_range_min, self.value_range_max)
        else:
            value_range = None

        if is_node_results:
            plot_nodevalues(axis,
                            # ids,
                            # edgesresults.ids_edge,
                            ids_node=ids_node,
                            values=values,
                            nodes=net.nodes,
                            ##                                            width_max = self.resultwidth,
                            alpha=self.alpha_results,
                            ##                                            printformat = self.printformat,
                            ##                                            color_outline = self.color_outline,
                            ##                                            color_fill = self.color_fill,
                            ##                                            color_label = self.color_label,
                            ##                                            is_antialiased = True,
                            is_fill=self.is_colorvalue,
                            ##                                            is_widthvalue = self.is_widthvalue,
                            ##                                            length_arrowhead = self.length_arrowhead,
                            fontsize=self.size_labelfont,
                            valuelabel=valuelabel,
                            value_range=value_range,
                            net=net
                            )

        self.configure_map(axis, title, unit=unit)

    def plot_point_results_on_map(self, net, axis, x_coords, y_coords, values=[],
                                  title='', valuelabel='',
                                  ):

        unit = self.unit_mapscale
        mapscale = self.get_attrsman().get_config('unit_mapscale').mapscales[unit]
        self.plot_net(axis, mapscale=mapscale,  unit=unit, is_configure=False)

        if self.is_value_range:
            value_range = (self.value_range_min, self.value_range_max)
        else:
            value_range = None

        plot_pointvalues(axis,
                         # ids,
                         # edgesresults.ids_edge,
                         x_coords=x_coords,
                         y_coords=y_coords,
                         values=values,
                         width_max=self.resultwidth,
                         alpha=self.alpha_results,
                         ##                                            printformat = self.printformat,
                         ##                                            color_outline = self.color_outline,
                         ##                                            color_fill = self.color_fill,
                         ##                                            color_label = self.color_label,
                         ##                                            is_antialiased = True,
                         is_fill=self.is_colorvalue,
                         is_widthvalue=self.is_widthvalue,
                         ##                                            length_arrowhead = self.length_arrowhead,
                         fontsize=self.size_labelfont,
                         valuelabel=valuelabel,
                         value_range=value_range,
                         net=net
                         )

        self.configure_map(axis, title, unit=unit)
