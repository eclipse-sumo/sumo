# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
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
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

import os
import subprocess
import numpy as np
from collections import OrderedDict
# import  matplotlib as mpl
# from matplotlib.patches import Arrow,Circle, Wedge, Polygon,FancyArrow
# from matplotlib.collections import PatchCollection
# import matplotlib.colors as colors
# import matplotlib.cm as cmx
# import matplotlib.pyplot as plt
# import matplotlib.image as image

from coremodules.misc.matplottools import *
from coremodules.network import routing

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.geometry import *
from agilepy.lib_base.processes import Process


class Resultplotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Plot edge results with Matplotlib',
                 info="Creates plots of different edge results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('resultplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        edgeresultattrconfigs = self.parent.edgeresults.get_group_attrs('results')
        self.add_edgeresultplotteroptions(edgeresultattrconfigs, **kwargs)
        self.add_edgeresultoptions(**kwargs)
        self.add_networkoptions(**kwargs)
        self.add_facilityoptions(**kwargs)
        self.add_zoneoptions(**kwargs)
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)
        self.add_save_options(**kwargs)

    def add_edgeresultplotteroptions(self, edgeresultattrconfigs, **kwargs):
        attrsman = self.get_attrsman()
        # edgeresultes....
        attrnames_edgeresults = OrderedDict()

        edgeresultattrnames = list(edgeresultattrconfigs.keys())
        # edgeresultattrnames.sort()
        for attrname in edgeresultattrnames:
            attrconfig = edgeresultattrconfigs[attrname]

            attrnames_edgeresults[attrconfig.format_symbol()] = attrconfig.attrname

        # attrnames_edgeresults = {'Entered':'entered'}
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

    def show(self):
        # print 'show',self.edgeattrname
        # if self.axis  is None:
        axis = init_plot()

        if (self.edgeattrname != ""):
            resultattrconf = getattr(self.get_edgeresults(), self.edgeattrname)
            ids = self.get_edgeresults().get_ids()
            if self.is_show_title:
                if self.title != "":
                    title = self.title
                else:
                    title = resultattrconf.get_info()

            self.plot_results_on_map(axis, ids, resultattrconf[ids],
                                     title=title,
                                     valuelabel=resultattrconf.format_symbol())

            self.show_or_save(resultattrconf.attrname)

    def get_edgeresults(self):
        return self.parent.edgeresults

    def do(self):
        # print 'do',self.edgeattrname
        self.show()

    def get_scenario(self):
        return self.parent.get_scenario()


class XYEdgeresultsPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='XY edge result plotter',
                 info="Plot one edge result attribute versus another result attribute using matplotlib",
                 logger=None, **kwargs):

        self._init_common('xyedgeresultsplotter', parent=results, name=name,
                          info=info, logger=logger)

        print('XYResultsPlotter.__init__', results)

        self._init_edgeattrs(self.get_edgeresults(), **kwargs)
        self._init_plotoptions(self.get_edgeresults(), **kwargs)

    def get_edgeresults(self):
        return self.parent.edgeresults

    def _init_plotoptions(self, edgeresults, **kwargs):
        attrsman = self.get_attrsman()
        self.is_linreg = attrsman.add(cm.AttrConf('is_linreg', kwargs.get('is_linreg', True),
                                                  groupnames=['options'],
                                                  name='Plot linear regression',
                                                  info='Plot linear regression.',
                                                  ))
        self.is_linreg_label = attrsman.add(cm.AttrConf('is_linreg_label', kwargs.get('is_linreg_label', True),
                                                        groupnames=['options'],
                                                        name='Show linear regression label',
                                                        info='Plot linear regression label with linear equation and R^2.',
                                                        ))
        self.add_plotoptions_lineplot(is_marker_only=False, is_no_markers=False, **kwargs)
        self.add_save_options(**kwargs)

    def _init_edgeattrs(self, edgeresults, **kwargs):
        # edgeresultes....
        attrsman = self.get_attrsman()
        attrnames_edgeresults = OrderedDict()
        edgeresultattrconfigs = edgeresults.get_group_attrs('results')
        edgeresultattrnames = list(edgeresultattrconfigs.keys())
        # edgeresultattrnames.sort()
        for attrname in edgeresultattrnames:
            attrconfig = edgeresultattrconfigs[attrname]

            attrnames_edgeresults[attrconfig.format_symbol()] = attrconfig.attrname

        # attrnames_edgeresults = {'Entered':'entered'}
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

        self.is_show_unitline = attrsman.add(cm.AttrConf('is_show_unitline', kwargs.get('is_show_unitline', False),
                                                         groupnames=['options',],
                                                         name='Show unit line',
                                                         info='Show unit line.',
                                                         ))

        self.is_show_ids_edge = attrsman.add(cm.AttrConf('is_show_ids_edge', kwargs.get('is_show_ids_edge', False),
                                                         groupnames=['options',],
                                                         name='Show edge IDs',
                                                         info='Show edge IDs of outlayers.',
                                                         ))

        self.counts_diff_err = attrsman.add(cm.AttrConf('counts_diff_err', kwargs.get('counts_diff_err', 100),
                                                        groupnames=['options',],
                                                        name='Outlayer boundaries',
                                                        info='Define the minimum error difference from which on a y-value is considered an outlayer. Outlayer edge IDs will be displayed.',
                                                        ))

    def show(self):
        print('show')

        plt.close("all")
        fig, ax = init_plot_fig_ax()
        if (self.edgeattrname_x != "") & (self.edgeattrname_x != ""):
            resultattrconf_x = getattr(self.parent.edgeresults, self.edgeattrname_x)
            resultattrconf_y = getattr(self.parent.edgeresults, self.edgeattrname_y)
        else:
            return False

        self.plot_xy(ax,
                     np.array(resultattrconf_x.get_value(), dtype=np.float32),
                     np.array(resultattrconf_y.get_value(), dtype=np.float32),
                     resultattrconf_x,
                     resultattrconf_y,
                     )
        self.show_or_save(resultattrconf_y.attrname, '_vs_', resultattrconf_x.attrname)

    def plot_xy(self, ax, x, y, resultattrconf_x, resultattrconf_y, title=None):
        print('plot_xy', resultattrconf_y.attrname, 'vs', resultattrconf_x.attrname)

        eps = 10.0**-10
        # ids_res = self.get_ids()

        inds = np.flatnonzero((x > eps) & (y > eps))

        val_max = max(np.max(x), np.max(y))
        # print('  val_max',val_max)

        if (len(inds) < 2):
            return False

        # print '  x',len(x),'v',x[inds]
        # print '  y',len(y),'v',y[inds]
        ax.plot(x[inds], y[inds],
                # label = resultattrconf_x.attrname+'',
                linestyle='None',
                marker=self.style_marker,
                markersize=self.size_marker,
                markerfacecolor=self.color_marker,
                # alpha = self.alpha_line
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

            x_linreg = np.array([np.min(x[inds]), np.max(x[inds])], dtype=np.float32)
            print(' m', m, 'offset', offset)
            # print ' x_linreg',offset * x_linreg
            if self.is_linreg_label:
                label = r'$%.3f+%.3fx$, $R^2=%.3f$' % (offset, m, R2)
            else:
                label = r'$R^2=%.3f$' % R2
            ax.plot(x_linreg, offset + m*x_linreg,
                    label=label,
                    linewidth=self.width_line,
                    linestyle=self.style_line,
                    color=self.color_line,
                    alpha=self.alpha_line
                    )
            # if self.is_linreg_label:
            ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)

        if self.is_show_unitline:

            points_unity = np.array([0.0, val_max], dtype=np.float32)
            ax.plot(points_unity, points_unity,
                    '-.',
                    linewidth=self.width_line,
                    color='black',
                    alpha=self.alpha_line
                    )

        if self.is_show_ids_edge:
            inds_diff = np.flatnonzero((np.abs(x-y) > self.counts_diff_err) & ((x > eps) & (y > eps)))
            edges = self.get_scenario().net.edges
            # print '  inds_diff',inds_diff
            # print '  inds',inds
            ids_edge_err = self.parent.edgeresults.ids_edge.get_value()[inds_diff]
            # print '  ids_edge_err',self.parent.edgeresults.ids_edge.get_value()[inds_diff]

            ax.plot([self.counts_diff_err, val_max],
                    [0.0, val_max-self.counts_diff_err],
                    '--',
                    linewidth=self.width_line,
                    color='gray',
                    alpha=0.5*self.alpha_line
                    )

            ax.plot([0.0, val_max],
                    [self.counts_diff_err, val_max+self.counts_diff_err],
                    '--',
                    linewidth=self.width_line,
                    color='gray',
                    alpha=0.5*self.alpha_line
                    )

            for id_edge, id_edge_sumo, x_edge, y_edge, dist_err in zip(ids_edge_err, edges.ids_sumo[ids_edge_err], x[inds_diff], y[inds_diff], np.abs(x[inds_diff]-y[inds_diff])):
                ax.text(x_edge, y_edge, str('  '+id_edge_sumo), horizontalalignment='left', fontsize=self.size_labelfont)
                print('  err=%03d, id_edge=%05d, id_edge_sumo=%s' % (dist_err, id_edge, id_edge_sumo))

        ax.grid(self.is_grid)

        if self.is_show_title:
            if title is None:
                if self.title != '':
                    title = self.title
                else:
                    title = resultattrconf_y.get_name()+' vs. '+resultattrconf_x.get_name()
            ax.set_title(title, fontsize=self.size_titlefont)

            # if subtitle is not None:
            #    NO! ax.set_subtitle(subtitle, fontsize=0.8*self.size_titlefont)

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


class EvolutionplotterMixin(Resultplotter, Process):
    def _init_attributes_common(self, **kwargs):
        attrsman = self.get_attrsman()
        edgeresultsevolution = self.parent.edgeresultsevolution
        framechoices = {}
        t_begin = edgeresultsevolution.time_start_recording.get_value()
        t_interval = edgeresultsevolution.time_interval.get_value()
        for framenumber in range(edgeresultsevolution.n_interval.get_value()):
            framechoices["%ds-%ds" % (t_begin + framenumber*t_interval, t_begin +
                                      (framenumber+1)*t_interval)] = framenumber
        self.framenumber_begin = attrsman.add(cm.AttrConf('framenumber_begin', kwargs.get('framenumber_begin', 0),
                                                          choices=framechoices,
                                                          groupnames=['options'],
                                                          name='Frame begin',
                                                          info='First frame to be plotted. Frame corrisponds to observed time interval.',
                                                          ))
        self.framenumber_end = attrsman.add(cm.AttrConf('framenumber_end', kwargs.get('framenumber_end', 0),
                                                        choices=framechoices,
                                                        groupnames=['options'],
                                                        name='Frame end',
                                                        info='Last frame to be plotted. Frame corrisponds to observed time interval.',
                                                        ))
        self.is_plot_in_one_fig = attrsman.add(cm.AttrConf('is_plot_in_one_fig', kwargs.get('is_plot_in_one_fig', False),
                                                           groupnames=['options'],
                                                           name='Plot in one figure?',
                                                           info='Plots all time intervals in one figure, instead of individual plots. This option excludes animation.',
                                                           ))

        self.n_plots_per_row = attrsman.add(cm.AttrConf('n_plots_per_row', kwargs.get('n_plots_per_row', 3),
                                                        groupnames=['options'],
                                                        name='Number of plots per row',
                                                        info='Number of plots per row, only in combination with plot all intervals in one figure.',
                                                        ))

    def add_anim_options(self, **kwargs):
        attrsman = self.get_attrsman()
        self.is_create_anim = attrsman.add(cm.AttrConf('is_create_anim', kwargs.get('is_create_anim', False),
                                                       groupnames=['options'],
                                                       name='Create animation?',
                                                       info='Creates animation file from single images per interval. This option requires imagemegic to be installed.',
                                                       ))

        self.format_anim = attrsman.add(cm.AttrConf('format_anim', kwargs.get('format_anim', 'gif'),
                                        choices=['mp4', 'gif'],
                                        groupnames=['options'],
                                        name='Animation file format',
                                        info='File format of animation. This option requires imagemegic to be installed.',
                                        ))
        self.delay_anim = attrsman.add(cm.AttrConf('delay_anim', kwargs.get('delay_anim', 10),
                                                   groupnames=['options'],
                                                   name='Animation delay',
                                                   unit='s',
                                                   info='Time delay for one interval to show in animation.',
                                                   ))
        self.n_loops_anim = attrsman.add(cm.AttrConf('n_loops_anim', kwargs.get('n_loops_anim', 0),
                                                     groupnames=['options'],
                                                     name='Number of animation loops',
                                                     info='Number of animation loops.',
                                                     ))
        self.is_create_anim_only = attrsman.add(cm.AttrConf('is_create_anim_only', kwargs.get('is_create_anim_only', False),
                                                            groupnames=['options'],
                                                            name='Create animation only?',
                                                            info='Creates only animation file using already existing image files. This is to play with animation oprions.',
                                                            ))

    def get_edgeresults(self):
        return self.parent.edgeresultsevolution

    def create_anim(self):
        if self.is_create_anim | self.is_create_anim_only:
            rootfilepath = os.path.join(self.figdirpath, self.parent.get_scenario().get_rootfilename())

            filepattern = """%s_%s_*.%s""" % (rootfilepath, self.get_figname(), self.figformat)
            animfilepath = """%s_%s.%s""" % (rootfilepath, self.get_figname(), self.format_anim)
            print('   create animation from pattern', filepattern)

            cml = 'convert -delay %d -loop %d %s %s' % (
                self.delay_anim, self.n_loops_anim, filepattern, animfilepath
            )
            print('cml=', cml)
            subprocess.Popen(cml, shell=True)


class XYEdgeresultsEvolutionPlotter(EvolutionplotterMixin, XYEdgeresultsPlotter):
    def __init__(self, results, name='XY edge result evolution plotter',
                 info="Plot one edge result attribute versus another result attribute using matplotlib for each time interval.",
                 logger=None, **kwargs):

        self._init_common('xyedgeresultsevolutionplotter', parent=results, name=name,
                          info=info, logger=logger)

        print('XYEdgeresultsEvolutionPlotter.__init__', results)
        self._init_attributes_common(**kwargs)
        self._init_edgeattrs(self.get_edgeresults(), **kwargs)
        self._init_plotoptions(self.get_edgeresults(), **kwargs)
        self.add_save_options(**kwargs)
        self.add_anim_options(**kwargs)

    def get_figname(self):
        edgeresultsevolution = self.get_edgeresults()
        resultattrconf_x = getattr(edgeresultsevolution, self.edgeattrname_x)
        resultattrconf_y = getattr(edgeresultsevolution, self.edgeattrname_y)
        return resultattrconf_y.attrname+'_vs_'+resultattrconf_x.attrname

    def show(self):
        print('XYEdgeresultsEvolutionPlotter.show')
        edgeresultsevolution = self.get_edgeresults()

        if not self.is_create_anim_only:

            if (self.edgeattrname_x != "") & (self.edgeattrname_x != ""):
                resultattrconf_x = getattr(edgeresultsevolution, self.edgeattrname_x)
                resultattrconf_y = getattr(edgeresultsevolution, self.edgeattrname_y)
            else:
                return False

            if self.is_show_title:
                if self.title != "":
                    title = self.title
                else:
                    title = resultattrconf_y.attrname+' vs '+resultattrconf_x.attrname
            else:
                title = ''

            ids = edgeresultsevolution.get_ids()
            if len(ids) == 0:
                return False

            # valuescale = 1.0/np.max(resultattrconf[ids])
            if self.is_plot_in_one_fig:

                n_frames = self.framenumber_end - self.framenumber_begin

                n_rows = n_frames/self.n_plots_per_row
                if n_frames % self.n_plots_per_row > 0:
                    n_rows += 1
                # requires matplotlib >3.0
                # fig = plt.figure(ind_fig)
                # gs = fig.add_gridspec(n_rows, self.n_plots_per_row, hspace=0, wspace=0)
                # axs = gs.subplots(sharex='col', sharey='row')

                fig, axs = self.create_figure(n_rows=n_rows, n_cols=self.n_plots_per_row,
                                              is_tight_layout=self.is_show_title)

                # print '  n_frames',n_frames,'n_rows',n_rows,'n_plots_per_row',self.n_plots_per_row
                # print '  axs',axs
                ind_col = 0
                ind_row = 0
                for framenumber in range(self.framenumber_begin, self.framenumber_end):
                    print('    framenumber', framenumber, len(ids), 'min val', np.min(resultattrconf_x[ids][:, framenumber]), 'max val', np.max(
                        resultattrconf_x[ids][:, framenumber]), 'shape', resultattrconf_x[ids][:, framenumber].shape)

                    # get current axes
                    print('      ind_row', ind_row, 'ind_col', ind_col, 'interval',
                          edgeresultsevolution.format_interval(framenumber))
                    if n_rows == 1:
                        ax = axs[ind_col]
                    else:
                        ax = axs[ind_row, ind_col]

                    ind_col += 1
                    if ind_col == self.n_plots_per_row:
                        ind_col = 0
                        ind_row += 1

                    self.plot_xy(ax,
                                 np.array(resultattrconf_x[ids][:, framenumber], dtype=np.float64),
                                 np.array(resultattrconf_y[ids][:, framenumber], dtype=np.float64),
                                 resultattrconf_x,
                                 resultattrconf_y,
                                 title=title+' '+edgeresultsevolution.format_interval(framenumber),
                                 )

                self.show_or_save(self.get_figname())

            else:

                for framenumber in range(self.framenumber_begin, self.framenumber_end):
                    print('    framenumber', framenumber, len(ids), 'min val', np.min(resultattrconf_x[ids][:, framenumber]), 'max val', np.max(
                        resultattrconf_x[ids][:, framenumber]), 'shape', resultattrconf_x[ids][:, framenumber].shape)

                    fig, ax = self.create_figure(is_return_axes=True, is_tight_layout=self.is_show_title)

                    self.plot_xy(ax,
                                 np.array(resultattrconf_x[ids][:, framenumber], dtype=np.float64),
                                 np.array(resultattrconf_y[ids][:, framenumber], dtype=np.float64),
                                 resultattrconf_x,
                                 resultattrconf_y,
                                 title=title+' '+edgeresultsevolution.format_interval(framenumber),
                                 )
                    self.show_or_save(self.get_figname()+'_%04d' % framenumber, )

                    # if self.is_save | self.is_create_anim:
                    #    #plt.close(ind_plot) # cleans up memory
                    plt.close('all')

                self.create_anim()

       # if not self.is_save:
       #     show_plot()


class EdgeResultEvolutionplotter(EvolutionplotterMixin):
    def __init__(self, results, name='Plot evolution of edge results with Matplotlib',
                 info="Creates plots of different results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('edgeresultevolutionplotter', parent=results, name=name,
                          info=info, logger=logger)

        # print 'Resultplotter.__init__',results,self.parent
        attrsman = self.get_attrsman()
        self.add_edgeresultplotteroptions(self.parent.edgeresultsevolution.get_group_attrs('results'), **kwargs)

        self._init_attributes_common(**kwargs)

        # self.add_edgeresultplotteroptions(results.edgeresultsevolution.get_group_attrs('results'), **kwargs)

        self.add_edgeresultoptions(**kwargs)
        self.add_networkoptions(**kwargs)
        self.add_facilityoptions(**kwargs)
        self.add_zoneoptions(**kwargs)
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)
        self.add_save_options(**kwargs)
        self.add_anim_options(**kwargs)

    def get_figname(self):
        edgeresultsevolution = self.parent.edgeresultsevolution
        resultattrconf = getattr(edgeresultsevolution, self.edgeattrname)
        return resultattrconf.attrname

    def show(self):
        print('EdgeResultEvolutionplotter.show')
        edgeresultsevolution = self.parent.edgeresultsevolution
        if not self.is_create_anim_only:
            if (self.edgeattrname != ""):
                resultattrconf = getattr(edgeresultsevolution, self.edgeattrname)
                ids = edgeresultsevolution.get_ids()
                if self.is_show_title:
                    if self.title != "":
                        title = self.title
                    else:
                        title = resultattrconf.get_name()
            else:
                # no attr chosen!?
                return

            if len(ids) == 0:
                return

            # print '  show',self.edgeattrname
            # if self.axis  is None:

            valuescale = 1.0/np.max(resultattrconf[ids])

            if self.is_plot_in_one_fig:

                n_frames = self.framenumber_end - self.framenumber_begin

                n_rows = n_frames/self.n_plots_per_row
                if n_frames % self.n_plots_per_row > 0:
                    n_rows += 1

                fig, axs = self.create_figure(n_rows=n_rows, n_cols=self.n_plots_per_row,
                                              is_tight_layout=self.is_show_title)

                ind_col = 0
                ind_row = 0
                for framenumber in range(self.framenumber_begin, self.framenumber_end):
                    print('    framenumber', framenumber, len(ids), 'min val', np.min(resultattrconf[ids][:, framenumber]), 'max val', np.max(
                        resultattrconf[ids][:, framenumber]), 'shape', resultattrconf[ids][:, framenumber].shape)

                    # get current axes
                    print('      ind_row', ind_row, 'ind_col', ind_col, 'interval',
                          edgeresultsevolution.format_interval(framenumber))
                    if n_rows == 1:
                        ax = axs[ind_col]
                    else:
                        ax = axs[ind_row, ind_col]

                    ind_col += 1
                    if ind_col == self.n_plots_per_row:
                        ind_col = 0
                        ind_row += 1
                    # cax =  fig.add_axes()
                    # print '    fig.add_axes()', fig.add_axes()
                    self.plot_results_on_map(ax, ids, resultattrconf[ids][:, framenumber],  # <<<<<< np.log(np.mean(resultattrconf[ids][:,framenumber:framenumber+self.delta],1)))
                                             title=title+' '+edgeresultsevolution.format_interval(framenumber),
                                             valuelabel=resultattrconf.format_symbol(),
                                             valuescale=valuescale,
                                             # cax = cax,
                                             is_colorbar=framenumber == self.framenumber_end-1
                                             )

                self.show_or_save(self.get_figname())

            else:

                for framenumber in range(self.framenumber_begin, self.framenumber_end):
                    print('    framenumber', framenumber, len(ids), 'min val', np.min(resultattrconf[ids][:, framenumber]), 'max val', np.max(
                        resultattrconf[ids][:, framenumber]), 'shape', resultattrconf[ids][:, framenumber].shape)

                    fig, axis = self.create_figure(is_return_axes=True, is_tight_layout=self.is_show_title)
                    self.plot_results_on_map(axis, ids, resultattrconf[ids][:, framenumber],
                                             title=title+' '+edgeresultsevolution.format_interval(framenumber),
                                             valuelabel=resultattrconf.format_symbol(),
                                             valuescale=valuescale)

                    self.show_or_save(self.get_figname()+'_%04d' % framenumber, is_show=True)
                    plt.close('all')

                self.create_anim()


class EdgeFlowSumEvolutionPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Edgeflow sum evolution plotter',
                 info="Plot the evolution of the sum of simulated edge flows with Matplotlib",
                 logger=None, **kwargs):

        self._init_common('edgeflowsumevolutionplotter', parent=results, name=name,
                          info=info, logger=logger)
        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.is_plot_measured_flows = attrsman.add(cm.AttrConf('is_plot_measured_flows', kwargs.get('is_plot_measured_flows', False),
                                                               groupnames=['options'],
                                                               name='Plot detected flows',
                                                               info='Plot measured flows from detector data. You need to add the measured flows to the results before.',
                                                               ))
        self.color_measured_flows = attrsman.add(cm.AttrConf('color_measured_flows', kwargs.get('color_measured_flows', np.array([0, 0, 1, 1], dtype=np.float32)),
                                                             groupnames=['options'],
                                                             perm='wr',
                                                             metatype='color',
                                                             name='Color of detected flows',
                                                             info='Color of detected flows.',
                                                             ))

        self.is_plot_simulated_flows = attrsman.add(cm.AttrConf('is_plot_simulated_flows', kwargs.get('is_plot_simulated_flows', True),
                                                                groupnames=['options'],
                                                                name='Plot simulated flows',
                                                                info='Plot simulated flows from edgeresults.',
                                                                ))

        self.color_simulated_flows = attrsman.add(cm.AttrConf('color_simulated_flows', kwargs.get('color_simulated_flows', np.array([0.5, 0.8, 0.8, 1], dtype=np.float32)),
                                                              groupnames=['options'],
                                                              perm='wr',
                                                              metatype='color',
                                                              name='Color of  simulated flows',
                                                              info='Color of  simulated flows from edgeresults.',
                                                              ))

        kwargs['is_grid'] = True
        self.add_plotoptions_lineplot(is_no_markers=True, **kwargs)
        self.add_save_options(**kwargs)

    def do(self):
        return self.show()

    def show(self):
        print('EdgeFlowSumEvolutionPlotter.show')

        ax1, ind_fig = next_plot(ind_fig=0)
        ax2, ind_fig = next_plot(ind_fig)

        results = self.parent
        simulation = results.parent
        demand = simulation.parent.demand

        if self.is_plot_measured_flows:
            detectorflows = demand.detectorflows
            detector_flows = detectorflows.flowmeasurements
            ids_detflow = detector_flows.get_ids()

            det_hours = detector_flows.timeperiods[ids_detflow]
            det_flows = detector_flows.flows_passenger[ids_detflow] + detector_flows.flows_heavyduty[ids_detflow]

            step_hours = np.sort(np.unique(det_hours))
            interval_time = step_hours[1]-step_hours[0]

            print('step_hours', step_hours)
            print('interval_time', interval_time)

            if self.is_plot_simulated_flows:
                edgeresultsevolution = simulation.results.edgeresultsevolution

                if (len(edgeresultsevolution) > 0) & (edgeresultsevolution.time_interval.get_value() == interval_time):
                    # need to adjust the time steps to fit simulated time range
                    step_hours_new = []
                    dep_times_sim = np.arange(edgeresultsevolution.n_interval.get_value(
                    )+1, dtype=np.int32)*edgeresultsevolution.time_interval.get_value()+edgeresultsevolution.time_start_recording.get_value()
                    print('  dep_times_sim=', len(dep_times_sim), dep_times_sim)

                    # not efficient , but save
                    for time_step in step_hours:
                        if time_step in dep_times_sim:
                            step_hours_new.append(time_step)

                    step_hours = np.array(step_hours_new, dtype=np.int32)
                else:
                    # no usable evolution results
                    return False

        elif self.is_plot_simulated_flows:
            # only simulated, no detectorflows
            edgeresultsevolution = simulation.results.edgeresultsevolution

            if len(edgeresultsevolution) > 0:
                interval_time = edgeresultsevolution.time_interval.get_value()
                dep_times_sim = np.arange(edgeresultsevolution.n_interval.get_value()+1, dtype=np.int32) * \
                    interval_time+edgeresultsevolution.time_start_recording.get_value()

                print('  dep_times_sim=', len(dep_times_sim), dep_times_sim)
                step_hours = dep_times_sim

            else:
                # no usable evolution results
                return False

        print('  step_hours', len(step_hours), '=', step_hours)

        # this is to center the bins, step_hours is in seconds
        # bins = (step_hours/3600.-interval_time/7200.).tolist()
        # bins.append(step_hours[-1]/3600.+interval_time/7200.)

        # ............Measured
        if self.is_plot_measured_flows:
            step_flows = []
            for step_hour in step_hours:
                step_flow = np.sum(det_flows[(det_hours == step_hour)])
                step_flows.append(step_flow)

            ax1.step(step_hours/3600., step_flows/np.sum(step_flows),
                     label='Detected density',
                     color=self.color_measured_flows,
                     linestyle='-', linewidth=self.width_line,
                     # marker = 'o', markersize = self.size_marker,
                     alpha=self.alpha_results
                     )

            ax2.step(step_hours/3600., step_flows,
                     label='Detected flows',
                     color=self.color_measured_flows,
                     linestyle='-', linewidth=self.width_line,
                     # marker = 'o', markersize = self.size_marker,
                     alpha=self.alpha_results
                     )

        # ............Simulated
        if self.is_plot_simulated_flows:

            if len(edgeresultsevolution) > 0:
                if self.is_plot_measured_flows:
                    lanes = demand.parent.net.lanes
                    ids_det = detectorflows.detectors.get_ids()
                    ids_detectoredge = []
                    ids_det
                    for ids_lane_det in detectorflows.detectors.ids_lanes[ids_det]:
                        if ids_lane_det is not None:
                            ids_detectoredge.append(lanes.ids_edge[ids_lane_det[0]])

                            # id_edge_res = edgeresultsevolution.ids_edge.get_id_from_index(id_edge_det)
                            # list_flows = edgeresultsevolution.entered[id_edge_res]
                            # for flow, i in zip(list_flows, list(range(len(list_flows)))):
                            #    for j in range(flow):
                            #        dep_sim.append(np.random.randint(dep_times_sim[i], dep_times_sim[i]+edgeresultsevolution.time_interval.get_value(),1)/3600.)
                else:
                    # take all edges if no detectors are needed foe comparisson
                    ids_detectoredge = demand.parent.net.edges.get_ids()

                print('  len(ids_detectoredge)', len(ids_detectoredge))
                simflows = np.array(
                    edgeresultsevolution.entered[edgeresultsevolution.ids_edge.get_ids_from_indices_save(ids_detectoredge)])

                print('  simflows.shape', simflows.shape)
                sumsimflows = np.sum(simflows, axis=0)
                sumsimflows_tot = np.sum(sumsimflows)

                # this is a bit dirty, but sometimes sumsimflows
                # includes or exclodes the last hour
                n_steps = min(len(sumsimflows), len(step_hours))
                # values, bins = np.histogram(dep_sim, bins=bins, density = True)
                ax1.step(step_hours[:n_steps]/3600., sumsimflows[:n_steps]/sumsimflows_tot,
                         label='Simulated density',
                         color=self.color_simulated_flows,
                         linestyle='-', linewidth=self.width_line,
                         # marker = 'o', markersize = self.size_marker,
                         alpha=self.alpha_results
                         )

                # values, bins = np.histogram(dep_sim, bins=bins, density = False)

                ax2.step(step_hours[:n_steps]/3600., sumsimflows[:n_steps],
                         label='Simulated flows',
                         color=self.color_simulated_flows,
                         linestyle='-', linewidth=self.width_line,
                         # marker = 'o', markersize = self.size_marker,
                         alpha=self.alpha_results
                         )

                # ~ demand.parent.simulation.edgesresults

        ax1.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax1.grid(self.is_grid)
        if self.is_show_title:
            if self.title != "":
                ax1.set_title(self.title, fontsize=self.size_titlefont)
            else:
                ax1.set_title('Sum of edge-flow density over time', fontsize=self.size_titlefont)
        ax1.set_xlabel('Time [h]', fontsize=self.size_labelfont)
        ax1.set_ylabel('Sum of flow density', fontsize=self.size_labelfont)
        ax1.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax1.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('flowsumdensity_over_time')

        ax2.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax2.grid(self.is_grid)
        if self.is_show_title:
            if self.title != "":
                ax2.set_title(self.title, fontsize=self.size_titlefont)
            else:
                ax2.set_title('Sum of edge-flows over time', fontsize=self.size_titlefont)
        ax2.set_xlabel('Time [h]', fontsize=self.size_labelfont)
        ax2.set_ylabel('Sum of flows', fontsize=self.size_labelfont)
        ax2.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax2.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('flowsum_over_time')

        # self.set_axisborder(ax)

        if not self.is_save:
            show_plot()

        return True


class ElectricalEnergyResultsPlotter(PlotoptionsMixin, Process):
    def __init__(self, results, name='Electrical energy plotter',
                 info="Plot electrical energy results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('electricalenergyresultsplotter', parent=results, name=name,
                          info=info, logger=logger)

        print('ElectricalEnergyResultsPlotter.__init__', results, self.parent, len(self.get_eneryresults()))
        attrsman = self.get_attrsman()

        self.add_plotoptions_lineplot(**kwargs)
        self.add_save_options(**kwargs)

    def get_eneryresults(self):
        return self.parent.electricenergy_vehicleresults

    def show(self):
        eneryresults = self.get_eneryresults()
        print('show', eneryresults)
        # print '  dir(vehicleman)',dir(vehicleman)

        print('  len(eneryresults)', len(eneryresults))
        if len(eneryresults) > 0:
            plt.close("all")
            self.plot_power()

    def plot_power(self):
        print('plot_power')
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

        print('SpeedprofilePlotter.__init__', results, self.parent, len(self.get_results()))
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
        print('show', results)
        id_res = results.ids_sumo.get_id_from_index(self.id_veh_sumo)

        print('  len(results)', len(results), results.ids_sumo.has_index(self.id_veh_sumo))
        if results.ids_sumo.has_index(self.id_veh_sumo):
            id_res = results.ids_sumo.get_id_from_index(self.id_veh_sumo)
            plt.close("all")
            self.plot_speedprofile(np.array(results.times.get_value()), np.array(results.speeds[id_res]))

        else:
            return False

    def plot_speedprofile(self, times, speeds):
        print('plot_speedprofile')

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
        self.show_or_save('speedprofile_'+self.id_veh_sumo)


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

        print('TraveltimePlotter.__init__', results, self.parent)
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
        # for id_edge, val in tree.items():
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
        ids_edge = np.array(list(edgetimestree.keys()), dtype=np.int32)
        edgetimes = np.array(list(edgetimestree.values()), dtype=np.float32)

        self.plot_results_on_map(ax, ids_edge=ids_edge,
                                 values=edgetimes,
                                 valuelabel='Travel time [s]',
                                 )

        if 0:  # self.is_isochrone:

            print('isochrone plot not yet implemented')

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
        self.show_or_save('accumulated_traveltimes')

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
# max_perc_var = 20.
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
        print(isochrone_shapes)
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
                print(zone_shape)
                for zone_shape_coords, i in zip(isochrone_shape, list(range(len(isochrone_shape)))):
                    print(i, len(isochrone_shape))
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
