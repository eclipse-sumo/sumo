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

# @file    virtualpop_results_mpl.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

import os
import numpy as np
from collections import OrderedDict
# import  matplotlib as mpl
from agilepy.lib_base.geometry import *
from coremodules.misc.matplottools import *
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patche
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.geometry import *
from agilepy.lib_base.processes import Process


class IteratePlotter(PlotoptionsMixin, Process):
    def __init__(self, scenario, name='Virtualpopulation Iterate Matplotlob Plotter',
                 info="""Creates plots to show results from iteratively executed simulation runs with the virtual population. 
                             The data is produced by the script script_vp_iterate.py
                             The result data must also be loaded into the scenario.
                             """,
                 logger=None, **kwargs):

        self._init_common('virtualpopiterateplotter', parent=scenario,
                          name=name,
                          info=info, logger=logger)

        print('VirtualpopIteratePlotter.__init__', self.parent)
        attrsman = self.get_attrsman()

        self.is_strategy_share = attrsman.add(cm.AttrConf('is_strategy_share', kwargs.get('is_strategy_share', True),
                                                          groupnames=['options', 'plots'],
                                                          name='Plot shares',
                                                          plotfunction=self.plot_strategy_share,
                                                          info=self.plot_strategy_share.__doc__,
                                                          ))

        self.is_strategy_time_est_tot = attrsman.add(cm.AttrConf('is_strategy_time_est_tot', kwargs.get('is_strategy_time_est_tot', True),
                                                                 groupnames=['options', 'plots'],
                                                                 name='Plot total est times',
                                                                 plotfunction=self.plot_strategy_time_tot_est,
                                                                 info='',
                                                                 ))

        self.is_strategy_time_tot = attrsman.add(cm.AttrConf('is_strategy_time_tot', kwargs.get('is_strategy_time_tot', True),
                                                             groupnames=['options', 'plots'],
                                                             name='Plot total times',
                                                             plotfunction=self.plot_strategy_time_tot,
                                                             info='Plot effective times for each stategy over iterations.',
                                                             ))

        self.is_strategy_changes = attrsman.add(cm.AttrConf('is_strategy_changes', kwargs.get('is_strategy_changes', True),
                                                            groupnames=['options', 'plots'],
                                                            name='Plot total changes',
                                                            plotfunction=self.plot_strategy_changes,
                                                            info=self.plot_strategy_changes.__doc__,
                                                            ))

        self.add_plotoptions_lineplot(**kwargs)
        self.add_save_options(**kwargs)

    def show(self):
        print('show')
        # if self.axis  is None:
        self.init_figures()
        plt.rc('lines', linewidth=self.width_line)
        # plt.rc('axes', prop_cycle=(cycler('color', ['r', 'g', 'b', 'y']) +
        #                    cycler('linestyle', ['-', '--', ':', '-.'])))

        for plotattr in self.get_attrsman().get_group('plots'):
            print('  ', plotattr.attrname, plotattr.get_value())
            if plotattr.get_value():
                plotattr.plotfunction()

        # if not self.is_save:
        show_plot()

    def get_scenario(self):
        return self.parent

    def get_resultattrconf(self, attrname):
        scenario = self.get_scenario()
        simresults = scenario.simulation.results
        vpiterstats = simresults.get_resultobj('vpiterstats')
        return getattr(vpiterstats, attrname)

    def plot_strategy_share(self):
        """
        Plot total est times for each stategy over iterations.
        """
        print('plot_strategy_share')
        scenario = self.get_scenario()
        simresults = scenario.simulation.results
        vpiterstats = simresults.get_resultobj('vpiterstats')
        iters = vpiterstats.get_ids()

        fig = self.create_figure()
        ax = fig.add_subplot(111)

        # sum total triptimes
        virtualpop = scenario.demand.virtualpop
        strategies = virtualpop.get_strategies()
        ids_strat = strategies.get_ids()
        trips_tot = np.zeros(len(iters), dtype=np.float32)
        for id_strat, strategy in zip(ids_strat, strategies.strategies[ids_strat]):
            trips_tot += self.get_resultattrconf(vpiterstats.get_stratcountattr(strategy))[iters]

        # plots for each strategy
        i = 0
        for id_strat, strategy in zip(ids_strat, strategies.strategies[ids_strat]):
            ident_strat = strategy.get_ident()
            name_strat = strategy.get_name()
            counts = self.get_resultattrconf(vpiterstats.get_stratcountattr(strategy))
            print('  name_strat', name_strat, 'counts', counts)
            if np.any(counts[iters] > 0):
                ax.plot(iters, 1.0*counts[iters]/trips_tot*100,
                        label=name_strat,
                        color=get_color(i),
                        linestyle='-',
                        linewidth=self.width_line,
                        marker=get_marker(i),
                        markersize=self.size_marker,
                        alpha=self.alpha_results,
                        )
                i += 1

        # for attrconf in vpiterstats.get_group('est. times'):

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_show_title:
            ax.set_title('Strategy shares', fontsize=self.size_titlefont)

        ax.set_xlabel('Iterations', fontsize=self.size_labelfont)
        ax.set_ylabel('Share [%]', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        # fig.tight_layout()
        if self.is_save:
            # rootname of figure will be rootfilename of scenario
            self.save_fig('fig_shares')

    def plot_strategy_changes(self):
        """'Plot strategy changes over iterations."""
        print('plot_strategy_changes')
        scenario = self.get_scenario()
        simresults = scenario.simulation.results
        vpiterstats = simresults.get_resultobj('vpiterstats')
        iters = vpiterstats.get_ids()

        fig = self.create_figure()
        ax = fig.add_subplot(111)

        ax.plot(iters, vpiterstats.numbers_changes[iters],
                # label = 'All strategies',
                color=self.color_line,
                linestyle='-',
                linewidth=self.width_line+1,
                marker='s', markersize=self.size_marker,
                )

        ax.grid(self.is_grid)
        if self.is_show_title:
            ax.set_title('Strategy changes', fontsize=self.size_titlefont)

        ax.set_xlabel('Iterations', fontsize=self.size_labelfont)
        ax.set_ylabel('Number of changes', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        # fig.tight_layout()
        if self.is_save:
            # rootname of figure will be rootfilename of scenario
            self.save_fig('fig_changes')

    def plot_strategy_time_tot_est(self):
        print('plot_strategy_time_tot_est')
        scenario = self.get_scenario()
        simresults = scenario.simulation.results
        vpiterstats = simresults.get_resultobj('vpiterstats')
        iters = vpiterstats.get_ids()

        fig = self.create_figure()
        ax = fig.add_subplot(111)

        ax.plot(iters, vpiterstats.times_tot_est[iters]/3600,
                label='All strategies',
                color='k',
                linestyle='-',
                linewidth=self.width_line+1,
                # marker = 's', markersize = 4*self.width_line,
                )

        # plot total times for each strategy
        virtualpop = scenario.demand.virtualpop
        strategies = virtualpop.get_strategies()
        ids_strat = strategies.get_ids()
        i = 0
        for id_strat, strategy in zip(ids_strat, strategies.strategies[ids_strat]):
            ident_strat = strategy.get_ident()
            name_strat = strategy.get_name()
            times_tot = self.get_resultattrconf(vpiterstats.get_stratesttimeattr(strategy))
            # print '  times_tot',times_tot
            if np.any(times_tot[iters] > 0):
                ax.plot(iters, times_tot[iters]/3600,
                        label=name_strat,
                        color=get_color(i),
                        linestyle='-',
                        linewidth=self.width_line,
                        marker=get_marker(i),
                        markersize=self.size_marker,
                        alpha=self.alpha_results,
                        )
                i += 1

        # for attrconf in vpiterstats.get_group('est. times'):

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_show_title:
            ax.set_title('Total est. travel times by strategy', fontsize=self.size_titlefont)

        ax.set_xlabel('Iterations', fontsize=self.size_labelfont)
        ax.set_ylabel('Tot. est. travel times [h]', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        # fig.tight_layout()
        if self.is_save:
            # rootname of figure will be rootfilename of scenario
            self.save_fig('fig_time_tot_est')

    def plot_strategy_time_tot(self):
        print('plot_strategy_time_tot')
        scenario = self.get_scenario()
        simresults = scenario.simulation.results
        vpiterstats = simresults.get_resultobj('vpiterstats')
        iters = vpiterstats.get_ids()  # not suppress last

        fig = self.create_figure()
        ax = fig.add_subplot(111)

        ax.plot(iters, vpiterstats.times_tot[iters]/3600,
                label='All strategies',
                color='k',
                linestyle='-',
                linewidth=self.width_line+1,
                # marker = 's', markersize = 4*self.width_line,
                )

        # plot total times for each strategy
        virtualpop = scenario.demand.virtualpop
        strategies = virtualpop.get_strategies()
        ids_strat = strategies.get_ids()
        i = 0
        for id_strat, strategy in zip(ids_strat, strategies.strategies[ids_strat]):
            ident_strat = strategy.get_ident()
            name_strat = strategy.get_name()
            times_tot = self.get_resultattrconf(vpiterstats.get_strattimeattr(strategy))
            # print '  times_tot',times_tot
            if np.any(times_tot > 0):
                ax.plot(iters, times_tot[iters]/3600,
                        label=name_strat,
                        color=get_color(i),
                        linestyle='-',
                        linewidth=self.width_line,
                        marker=get_marker(i),
                        markersize=self.size_marker,
                        alpha=self.alpha_results,
                        )
                i += 1

        # for attrconf in vpiterstats.get_group('est. times'):

        ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_show_title:
            ax.set_title('Total travel times by strategy', fontsize=self.size_titlefont)
        ax.set_xlabel('Iterations', fontsize=self.size_labelfont)
        ax.set_ylabel('Tot. travel times [h]', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        # fig.tight_layout()
        if self.is_save:
            # rootname of figure will be rootfilename of scenario
            self.save_fig('fig_time_tot')


class StrategyPlotter(PlotoptionsMixin, Process):
    def __init__(self, virtualpop, name='Plot strategy results with Matplotlib',
                 info="Creates plots of different strategy results using matplotlib",
                 logger=None, **kwargs):

        self._init_common('strategyresultplotter', parent=virtualpop, name=name,
                          info=info, logger=logger)

        print('StrategyPlotter.__init__', self.parent)
        attrsman = self.get_attrsman()

        self.is_strategy_share = attrsman.add(cm.AttrConf('is_strategy_share', kwargs.get('is_strategy_share', True),
                                                          groupnames=['options', 'plots'],
                                                          name='Plot strategy shares',
                                                          plotfunction=self.plot_strategy_share,
                                                          info='Plot share of currently chosen strategies.',
                                                          ))

        self.is_strategy_time_est_mean = attrsman.add(cm.AttrConf('is_strategy_time_est_mean', kwargs.get('is_strategy_time_est_mean', True),
                                                                  groupnames=['options', 'plots'],
                                                                  name='Plot strategy mean est times',
                                                                  plotfunction=self.plot_strategy_times_est,
                                                                  info='Plot strategy mean est times.',
                                                                  ))

        self.is_strategy_timefactors_est = attrsman.add(cm.AttrConf('is_strategy_timefactors_est', kwargs.get('is_strategy_timefactors_est', True),
                                                                    groupnames=['options', 'plots'],
                                                                    name='Plot strategy times factors',
                                                                    plotfunction=self.plot_strategy_timefactors_est,
                                                                    info=self.plot_strategy_timefactors_est.__doc__,
                                                                    ))

        self.is_strategy_timefactors_exec = attrsman.add(cm.AttrConf('is_strategy_timefactors_exec', kwargs.get('is_strategy_timefactors_exec', True),
                                                                     groupnames=['options', 'plots'],
                                                                     name='Plot strategy exec times factors',
                                                                     plotfunction=self.plot_strategy_timefactors_exec,
                                                                     info=self.plot_strategy_timefactors_exec.__doc__,
                                                                     ))

        # other
        # self.n_bins = attrsman.add(cm.AttrConf(  'n_bins', kwargs.get('n_bins',10),
        #                                groupnames = ['options'],
        #                                name = 'Bin number',
        #                                info = 'Number of bins for histograms.',
        #                                ))

        self.timeint_bins = attrsman.add(cm.AttrConf('timeint_bins', kwargs.get('timeint_bins', 0.5),
                                                     groupnames=['options'],
                                                     name='Bin time int.',
                                                     unit='s',
                                                     info='Size of time intervals in histograms.',
                                                     ))

        self.color_chart = attrsman.add(cm.AttrConf('color_chart', kwargs.get('color_chart', np.array([0.3, 0.1, 0.9, 1], dtype=np.float32)),
                                        groupnames=['options'],
                                        perm='wr',
                                        metatype='color',
                                        name='Chart color',
                                        info='Main Color of chart bars.',
                                                    ))

        self.add_plotoptions_lineplot(**kwargs)
        self.add_save_options(**kwargs)

    def show(self):
        print('show')
        # if self.axis  is None:
        self.init_figures()
        plt.rc('lines', linewidth=self.width_line)
        # plt.rc('axes', prop_cycle=(cycler('color', ['r', 'g', 'b', 'y']) +
        #                    cycler('linestyle', ['-', '--', ':', '-.'])))

        for plotattr in self.get_attrsman().get_group('plots'):
            print('  ', plotattr.attrname, plotattr.get_value())
            if plotattr.get_value():
                plotattr.plotfunction()

        if not self.is_save:
            show_plot()

    def plot_strategy_share(self):
        print('plot_strategy_share')
        fig = self.create_figure()
        ax = fig.add_subplot(111)

        virtualpop = self.parent
        ids_pers = virtualpop.get_ids()
        n_pers = len(ids_pers)

        plans = virtualpop.get_plans()
        strategytable = virtualpop.get_strategies()

        ids_strat = strategytable.get_ids()
        n_strat = len(ids_strat)
        index_strat = np.arange(n_strat)
        names_strat = np.zeros(n_strat, dtype=np.object_)
        values = np.zeros(n_strat, dtype=np.float32)

        ids_allstrategies = plans.ids_strategy[virtualpop.ids_plan[ids_pers]]
        for i, id_strat, strategy in zip(index_strat, ids_strat, strategytable.strategies[ids_strat]):
            names_strat[i] = strategy.get_name()
            count = np.sum(ids_allstrategies == id_strat)
            values[i] = float(count)/n_pers
            print(('Strategy with id ' + str(id_strat) + ' has a mode share of ' + str(1.0*values[i]*100) + '%'))
        colors = strategytable.colors[ids_strat]
        inds = np.argsort(values)
        inds_nz = values[inds] > 0

        inds_plot = np.arange(len(index_strat[inds_nz]))
        bar_width = 0.45
        opacity = 0.5
        # error_config = {'ecolor': '0.3'}

        rects = ax.barh(inds_plot, values[inds][inds_nz],
                        # align='center',
                        alpha=opacity,
                        height=bar_width, color=colors[inds][inds_nz],  # self.color_chart,
                        # yerr=std_women, error_kw=error_config,
                        linewidth=self.width_line,
                        # facecolor=colors[inds][inds_nz],
                        )

 #       ax.plot(iters, 1.0*counts[iters]/trips_tot*100,
 #                   label = name_strat,
 #                   color = get_color(i),
 #                   linestyle = '-',
 #                   linewidth = self.width_line,
 #                   marker = get_marker(i),
 #                   markersize = self.size_marker,
 #                   alpha = self.alpha_results,
 #                   )

        ax.set_yticks(inds_plot + bar_width / 2)
        ax.set_yticklabels(names_strat[inds][inds_nz])
        ax.legend()

        # ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_show_title:
            ax.set_title('Strategy shares', fontsize=self.size_titlefont)
        ax.set_xlabel('Share', fontsize=self.size_labelfont)
        ax.set_ylabel('Strategies', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        # fig.tight_layout()
        if self.is_save:
            self.save_fig('virtualpop_strategy_share_current')

    def plot_strategy_times_est(self):
        print('plot_strategy_times_est')
        fig = self.create_figure()
        ax = fig.add_subplot(111)

        virtualpop = self.parent
        ids_pers = virtualpop.get_ids()
        n_pers = len(ids_pers)

        plans = virtualpop.get_plans()
        ids_plan = plans.get_ids()
        ids_strategy = plans.ids_strategy[ids_plan]
        times_est = plans.times_est[ids_plan]

        strategytable = virtualpop.get_strategies()

        ids_strat = strategytable.get_ids()
        n_strat = len(ids_strat)
        index_strat = np.arange(n_strat)
        names_strat = np.zeros(n_strat, dtype=np.object_)
        values = np.zeros(n_strat, dtype=np.float32)

        for i, id_strat, strategy in zip(index_strat, ids_strat, strategytable.strategies[ids_strat]):
            names_strat[i] = strategy.get_name()
            inds_strat = np.flatnonzero((ids_strategy == id_strat) & (times_est > 0))
            values[i] = np.mean(times_est[inds_strat])

        colors = strategytable.colors[ids_strat]
        inds = np.argsort(values)[::-1]
        inds_nz = values[inds] > 0

        inds_plot = np.arange(len(index_strat[inds_nz]))

        bar_width = 0.45
        opacity = 0.5
        # error_config = {'ecolor': '0.3'}

        rects = ax.barh(inds_plot, values[inds][inds_nz],
                        # align='center',
                        alpha=opacity,
                        height=bar_width, color=colors[inds][inds_nz],
                        # yerr=std_women, error_kw=error_config,
                        linewidth=self.width_line,
                        # facecolor=colors[inds][inds_nz],
                        )

        ax.set_yticks(inds_plot + bar_width / 2)
        ax.set_yticklabels(names_strat[inds][inds_nz])
        ax.legend()

        # ax.legend(loc='best',shadow=True, fontsize=self.size_labelfont)
        ax.grid(self.is_grid)
        if self.is_show_title:
            ax.set_title('Mean estimated travel time of strategies', fontsize=self.size_titlefont)

        ax.set_xlabel('Mean est. time [s]', fontsize=self.size_labelfont)
        ax.set_ylabel('Strategies', fontsize=self.size_labelfont)
        ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
        ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))
        # fig.tight_layout()
        if self.is_save:
            self.save_fig('virtualpop_strategy_time_est_mean')

    def plot_strategy_timefactors_est(self):
        """
        For each strategy plot its estimated travel time factor with respect to the fastest strategy of the same person
        """
        print('plot_strategy_timefactors_est')
        fig = self.create_figure()
        ax = fig.add_subplot(111)

        virtualpop = self.parent
        ids_pers = virtualpop.get_ids()
        n_pers = len(ids_pers)

        plans = virtualpop.get_plans()
        ids_plan = plans.get_ids()
        times_plan = plans.times_est  # [ids_plan]

        strategytable = virtualpop.get_strategies()

        ids_strat = strategytable.get_ids()
        n_strat = len(ids_strat)
        index_strat = np.arange(n_strat)
        names_strat = np.zeros(n_strat, dtype=np.object_)
        values = np.zeros(n_strat, dtype=np.float32)
        strategytimefactors = {}  # np.zeros(n_strat, dtype = np.object_)
        for i, id_strat, strategy in zip(index_strat, ids_strat, strategytable.strategies[ids_strat]):
            strategytimefactors[id_strat] = []

        for id_pers, ids_plan in zip(ids_pers, virtualpop.lists_ids_plan[ids_pers]):
            if len(ids_plan) > 1:
                times_strat = times_plan[ids_plan]
                inds_time = np.argsort(times_strat)
                ids_plan_sort = np.array(ids_plan, dtype=np.int32)[inds_time]
                # print '  id_pers,ids_plan',id_pers,ids_plan
                # print '  times_strat',times_strat,type(times_strat)
                # print '  inds_time',inds_time,type(inds_time)
                time_win = times_strat[inds_time[0]]
                # print '  time_win',time_win
                # print '  inds_time[1:]',inds_time[1:]
                # print '  ids_plan_sort',ids_plan_sort
                # print '  times_strat[inds_time[1:]]',times_strat[inds_time[1:]]
                if time_win > 0:
                    for id_strat, timefac in zip(plans.ids_strategy[ids_plan_sort[1:]], times_strat[inds_time[1:]]/time_win):
                        strategytimefactors[id_strat].append(timefac)

        x_min = 0.0
        x_max = 0.0
        for id_strat, timefactors in strategytimefactors.items():
            if len(timefactors) > 0:
                x_max = max(x_max, np.max(timefactors))

        # bins = np.linspace(x_min,x_max,self.n_bins)
        bins = np.arange(x_min, x_max, self.timeint_bins)
        if len(bins) > 0:
            for id_strat, timefactors in strategytimefactors.items():
                if len(timefactors) > 0:
                    self.plot_hist(ax, np.array(timefactors, dtype=np.float32),
                                   bins=bins,
                                   color=strategytable.colors[id_strat],
                                   facecolor=strategytable.colors[id_strat],
                                   label=strategytable.names[id_strat],
                                   is_normed=False,
                                   is_cumulative=False,
                                   is_rel_frequ=True,
                                   histtype='stepfilled',  # {'bar', 'barstacked', 'step', 'stepfilled'}
                                   is_percent=False,
                                   alpha=0.5,
                                   )

            ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
            ax.grid(self.is_grid)
            if self.is_show_title:
                ax.set_title('Estimated time factor with respect to fastest strategy', fontsize=self.size_titlefont)
            ax.set_xlabel('Estimated time factor', fontsize=self.size_labelfont)
            ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
            ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
            ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

            if self.is_save:
                self.save_fig('virtualpop_strategy_timefactor_est')

    def plot_strategy_timefactors_exec(self):
        """
        For each strategy plot its travel executive time factor with respect to the fastest strategy of the same person
        """
        print('plot_strategy_timefactors_est')
        fig = self.create_figure()
        ax = fig.add_subplot(111)

        virtualpop = self.parent
        ids_pers = virtualpop.get_ids()
        n_pers = len(ids_pers)

        plans = virtualpop.get_plans()
        ids_plan = plans.get_ids()
        times_plan = plans.times_exec  # [ids_plan]

        strategytable = virtualpop.get_strategies()

        ids_strat = strategytable.get_ids()
        n_strat = len(ids_strat)
        index_strat = np.arange(n_strat)
        names_strat = np.zeros(n_strat, dtype=np.object_)
        values = np.zeros(n_strat, dtype=np.float32)
        strategytimefactors = {}  # np.zeros(n_strat, dtype = np.object_)
        for i, id_strat, strategy in zip(index_strat, ids_strat, strategytable.strategies[ids_strat]):
            strategytimefactors[id_strat] = []

        n_nonexistant = 0
        for id_pers, ids_plan in zip(ids_pers, virtualpop.lists_ids_plan[ids_pers]):
            if len(ids_plan) > 1:
                times_strat = times_plan[ids_plan]
                inds_time = np.argsort(times_strat)
                ids_plan_sort = np.array(ids_plan, dtype=np.int32)[inds_time]
                # print '  id_pers,ids_plan',id_pers,ids_plan
                # print '  times_strat',times_strat,type(times_strat)
                # print '  inds_time',inds_time,type(inds_time)
                time_win = times_strat[inds_time[0]]
                # print '  time_win',time_win
                # print '  inds_time[1:]',inds_time[1:]
                # print '  ids_plan_sort',ids_plan_sort
                # print '  times_strat[inds_time[1:]]',times_strat[inds_time[1:]]
                if time_win > 0:
                    for id_strat, timefac in zip(plans.ids_strategy[ids_plan_sort[1:]], times_strat[inds_time[1:]]/time_win):
                        strategytimefactors[id_strat].append(timefac)
                else:
                    n_nonexistant += 1

        if n_nonexistant > 0:
            print('  WARNING: only %d of %d  persons have not completed all strategies' % (n_nonexistant, n_pers))
        x_min = 0.0
        x_max = 0.0
        for id_strat, timefactors in strategytimefactors.items():
            if len(timefactors) > 0:
                x_max = max(x_max, np.max(timefactors))

        # bins = np.linspace(x_min,x_max,self.n_bins)
        bins = np.arange(x_min, x_max, self.timeint_bins)
        if len(bins) > 0:
            for id_strat, timefactors in strategytimefactors.items():
                if len(timefactors) > 0:
                    self.plot_hist(ax, np.array(timefactors, dtype=np.float32),
                                   bins=bins,
                                   color=strategytable.colors[id_strat],
                                   facecolor=strategytable.colors[id_strat],
                                   label=strategytable.names[id_strat],
                                   is_normed=False,
                                   is_cumulative=False,
                                   is_rel_frequ=True,
                                   histtype='stepfilled',  # {'bar', 'barstacked', 'step', 'stepfilled'}
                                   is_percent=False,
                                   alpha=0.5,
                                   )

            ax.legend(loc='best', shadow=True, fontsize=self.size_labelfont)
            ax.grid(self.is_grid)
            if self.is_show_title:
                ax.set_title('Time factor with respect to fastest strategy', fontsize=self.size_titlefont)
            ax.set_xlabel('Time factor', fontsize=self.size_labelfont)
            ax.set_ylabel('Probability distribution', fontsize=self.size_labelfont)
            ax.tick_params(axis='x', labelsize=int(0.8*self.size_labelfont))
            ax.tick_params(axis='y', labelsize=int(0.8*self.size_labelfont))

            if self.is_save:
                self.save_fig('virtualpop_strategy_timefactor_est')


class ActivityPlotter(PlotoptionsMixin, Process):
    def __init__(self, ident, virtualpop, logger=None, **kwargs):
        print('ActivityPlotter.__init__')
        self._init_common(ident,
                          parent=virtualpop,
                          name='VP plots',
                          logger=logger,
                          info='Plot of VP data.',
                          )
        attrsman = self.set_attrsman(cm.Attrsman(self))
        strategies = virtualpop.get_strategies()
        activitytypes = virtualpop.parent.activitytypes
        landusetypes = virtualpop.parent.get_scenario().landuse.landusetypes

        self.facilityfillmode = attrsman.add(cm.AttrConf('facilityfillmode', kwargs.get('facilityfillmode', 'residents per facility'),
                                                         choices=['residents per facility',
                                                                  'destinations per facility'],  # 'capacities'],
                                                         groupnames=['options'],
                                                         name='Facility fill mode',
                                                         info='Defines how each facility is filled. The destinations refer to each intermediate activity',
                                                         ))

        self.is_specific_facility = attrsman.add(cm.AttrConf('is_specific_facility', kwargs.get('is_specific_facility', False),
                                                             groupnames=['options', 'zones'],
                                                             name='Plot speficic facility',
                                                             info='Plot for a specific facility.',
                                                             ))

        self.specific_facility = attrsman.add(cm.AttrConf('specific_facility', kwargs.get('specific_facility', -1),
                                                          groupnames=['options', 'zones'],
                                                          name='Facility',
                                                          info='Specific facility to be considered for the plot, if it is requested to plot for a specific facility.',
                                                          ))

        ids_landusetype = list(set(landusetypes.get_ids()))
        landusechoices = {}
        for id_landuse, landusename in zip(ids_landusetype, landusetypes.typekeys[ids_landusetype]):
            landusechoices[landusename] = id_landuse

        self.ids_landusetype = attrsman.add(cm.ListConf('ids_landusetype', 1*ids_landusetype,
                                            groupnames=['options'],
                                            choices=landusechoices,
                                            name='Landuses',
                                            info="""Landuses to be visualized. To be considered for the facility fill mode: destinations per facility""",
                                                        ))

        ids_activitytype = list(set(activitytypes.get_ids()))
        activitychoices = {}
        for id_activity, activityname in zip(ids_activitytype, activitytypes.names[ids_activitytype]):
            activitychoices[activityname] = id_activity

        self.ids_activitytype = attrsman.add(cm.ListConf('ids_activitytype', 1*ids_activitytype,
                                                         groupnames=['options'],
                                                         choices=activitychoices,
                                                         name='Activities',
                                                         info="""Activities to be visualized. To be considered for the facility fill mode: destinations per facility""",
                                                         ))

        self.is_select_strategies = attrsman.add(cm.AttrConf('is_select_strategies', kwargs.get('is_select_strategies', False),
                                                             groupnames=['options'],
                                                             name='Select strategies',
                                                             info='If plans are created, it is possible to select destinations per different strategies.',
                                                             ))

        ids_strategy = list(set(strategies.get_ids()))
        strategychoices = {}
        for id_strategy, strategyname in zip(ids_strategy, strategies.names[ids_strategy]):
            strategychoices[strategyname] = id_strategy

        self.ids_strategy = attrsman.add(cm.ListConf('ids_strategy', 1*ids_strategy,
                                                     groupnames=['options'],
                                                     choices=strategychoices,
                                                     name='Strategies',
                                                     info="""Strategies to be visualized: for each person will be checked the current strategy in the selected plan. To be considered for the facility fill mode: destinations per facility. This is active if 'select strategies' is activated""",
                                                     ))
        # ~ self.alpha_facilities = attrsman.add(cm.AttrConf(  'alpha_facilities', kwargs.get('alpha_facilities', 0.3),
        # ~ groupnames = ['options','zones'],
        # ~ name = 'Building transparency',
        # ~ info = 'Transparency of the fill color of buildings. This will apply in the case the facility value is zero.',
        # ~ ))

        # ~ self.alpha_amenities = attrsman.add(cm.AttrConf(  'alpha_amenities', kwargs.get('alpha_amenities', 0.9),
        # ~ groupnames = ['options','zones'],
        # ~ name = 'Amenities transparency',
        # ~ info = 'Transparency of the fill color of amenities. This will apply in the case the facility value is zero.',
        # ~ ))

        self.alpha_cmap = attrsman.add(cm.AttrConf('alpha_cmap', kwargs.get('alpha_cmap', 0.8),
                                                   groupnames=['options', 'zones'],
                                                   name='Color map transparency',
                                                   info='Transparency of the fill colormap for facilities.',
                                                   ))

        self.is_use_landusetype_color_for_bordes = attrsman.add(cm.AttrConf('is_use_landusetype_color_for_bordes', kwargs.get('is_use_landusetype_color_for_bordes', True),
                                                                            groupnames=['options'],
                                                                            name='Use landtype color',
                                                                            info='Use facilities landusetype color for bordes.',
                                                                            ))

        self.linewidth_facilityborders = attrsman.add(cm.AttrConf('linewidth_facilityborders', kwargs.get('linewidth_facilityborders', 2),
                                                                  groupnames=['options', 'zones'],
                                                                  name='Facility border line width',
                                                                  info='Width of the border of facilities.',
                                                                  ))

        self.is_show_id_facility = attrsman.add(cm.AttrConf('is_show_id_facility', kwargs.get('is_show_id_facility', False),
                                                            groupnames=['options'],
                                                            name='Show facility IDs',
                                                            info='Show ID of each facility.',
                                                            ))

        self.is_show_facilityvalues = attrsman.add(cm.AttrConf('is_show_facilityvalues', kwargs.get('is_show_facilityvalues', False),
                                                               groupnames=['options'],
                                                               name='Show facility values',
                                                               info='Show values of each facility. Values depend on the choise of "facilityfillmode"',
                                                               ))

        self.add_networkoptions(**kwargs)
        # ~ self.add_facilityoptions(**kwargs)
        self.add_zoneoptions(**kwargs)
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)
        self.add_save_options(**kwargs)

    def show(self):
        print('OdPlots.show')
        # if self.axis  is None:
        scenario = self.parent.get_scenario()
        demand = scenario.demand
        landuse = scenario.landuse
        virtualpop = self.parent
        ids_vp = virtualpop.get_ids()
        households = virtualpop.get_households()
        ids_hsh = households.get_ids()
        facilities = scenario.landuse.facilities
        ids_fac = facilities.get_ids()
        activities = virtualpop.get_activities()

# -----------------------Guess facilities and values

        # ~ #Capacities
        # ~ if self.facilityfillmode == 'capacities':
        # ~ capacities = {}
        # ~ for id_fac in ids_fac:
        # ~ residents[id_fac] = 0
        # ~ if self.is_specific_facility:
        # ~ capacities[self.specific_facility] = facilities.capacities[self.specific_facility]
        # ~ else:
        # ~ for id_fac in ids_fac:
        # ~ capacities[id_fac] = facilities.capacities[id_fac]

        # Residents
        if self.facilityfillmode == 'residents per facility':
            residents = {}
            for id_fac in ids_fac:
                residents[id_fac] = 0
            if self.is_specific_facility:
                ids_hsh_fac = ids_hsh[(households.buildings[ids_hsh] == self.specific_facility)]
                for id_hsh in ids_hsh_fac:
                    residents[self.specific_facility] += households.sizes[id_hsh]
            else:
                for id_hsh in ids_hsh:
                    id_fac = households.buildings[id_hsh]
                    residents[id_fac] += households.sizes[id_hsh]

        # Destinations fac
        if self.facilityfillmode == 'destinations per facility':
            destinations_fac = {}
            for id_fac in ids_fac:
                destinations_fac[id_fac] = 0
            if self.is_specific_facility:
                ids_hsh_fac = ids_hsh[(households.buildings[ids_hsh] == self.specific_facility)]
                vps = []
                for id_vp in ids_vp:
                    if virtualpop.ids_household[id_vp] in ids_hsh_fac:
                        vps.append(id_vp)
                ids_vp = vps

            for id_vp in ids_vp:
                ids_activity = virtualpop.activitypatterns[id_vp]
                ids_dest_activity = ids_activity[(activities.ids_activitytype[ids_activity]
                                                  != demand.activitytypes.names.get_id_from_index('home'))]
                # filter act
                # ~ print 'ids_activity',ids_activity
                # ~ print 'ids_dest_activity',ids_dest_activity
                if len(ids_dest_activity) > 0:
                    ids_dest_activity_act = []
                    for id_dest_activity in ids_dest_activity:
                        if activities.ids_activitytype[id_dest_activity] in self.ids_activitytype:
                            ids_dest_activity_act.append(id_dest_activity)
                    # ~ print 'ids_dest_activity_act',ids_dest_activity_act
                    if len(ids_dest_activity_act) > 0:
                        # filter land
                        ids_dest_activity_lan = []
                        for id_dest_activity in ids_dest_activity_act:
                            if facilities.ids_landusetype[activities.ids_facility[id_dest_activity]] in self.ids_landusetype:
                                ids_dest_activity_lan.append(id_dest_activity)
                        # ~ print 'ids_dest_activity_lan',ids_dest_activity_lan
                        if len(ids_dest_activity_lan) > 0:
                            # filter strategy
                            if self.is_select_strategies:
                                id_plan = virtualpop.ids_plan[id_vp]
                                id_strategy = virtualpop.plans.ids_strategy[id_plan]
                                # ~ print 'id_strategy',id_strategy
                                # ~ print 'self.ids_strategy',self.ids_strategy
                                if id_strategy in self.ids_strategy:
                                    for id_dest_activity in ids_dest_activity_lan:
                                        destinations_fac[activities.ids_facility[id_dest_activity]] += 1
                            else:
                                for id_dest_activity in ids_dest_activity_lan:
                                    print(id_dest_activity)
                                    destinations_fac[activities.ids_facility[id_dest_activity]] += 1
        # ~ #Destinations act
        # ~ if self.facilityfillmode == 'destinations per activity':
            # ~ destinations_act = {}
            # ~ for id_fac in ids_fac:
                # ~ destinations_act[id_fac] = 0
            # ~ if self.is_specific_facility:
                # ~ ids_hsh_fac = ids_hsh[(households.buildings[ids_hsh] == self.specific_facility)]
                # ~ ids_vp = ids_vp[(virtualpop.ids_household[ids_vp] in ids_hsh_fac)]
            # ~ for id_vp in ids_vp:
                # ~ ids_activity = virtualpop.activitypatterns[id_vp]
                # ~ ids_dest_activity = ids_activity[(activities.ids_activitytype[ids_activity] in self.ids_activity)]
                # ~ for id_dest_activity in ids_dest_activity:
                    # ~ destinations_act[activities.ids_facility[id_dest_activity]]+=1
        # ~ #Destinations lan
        # ~ if self.facilityfillmode == 'destinations per landuse':
            # ~ destinations_fac = {}
            # ~ for id_fac in ids_fac:
                # ~ destinations_fac[id_fac] = 0
            # ~ if self.is_specific_facility:
                # ~ ids_hsh_fac = ids_hsh[(households.buildings[ids_hsh] == self.specific_facility)]
                # ~ ids_vp = ids_vp[(virtualpop.ids_household[ids_vp] in ids_hsh_fac)]
            # ~ for id_vp in ids_vp:
                # ~ ids_activity = virtualpop.activitypatterns[id_vp]
                # ~ ids_dest_activity = ids_activity[(activities.ids_activitytype[ids_activity] != demand.activitytypes.names.get_id_from_index('home'))]

                # ~ for id_dest_activity in ids_dest_activity:
                    # ~ if facilities.ids_landusetype[activities.ids_facility[id_dest_activity]] in self.ids_landuse:
                        # ~ destinations_lan[activities.ids_facility[id_dest_activity]]+=1
        # ~ #Destinations str
        # ~ if self.facilityfillmode == 'destinations per strategy':
            # ~ destinations_str = {}
            # ~ for id_fac in ids_fac:
                # ~ destinations_str[id_fac] = 0
            # ~ if self.is_specific_facility:
                # ~ ids_hsh_fac = ids_hsh[(households.buildings[ids_hsh] == self.specific_facility)]
                # ~ ids_vp = ids_vp[(virtualpop.ids_household[ids_vp] in ids_hsh_fac)]
            # ~ for id_vp in ids_vp:
                # ~ id_plan = virtualpop.ids_plan[id_vp]
                # ~ id_strategy = virtualpop.plans.ids_strategy[id_plan]
                # ~ if id_strategy in self.ids_strategy:
                    # ~ ids_activity = virtualpop.activitypatterns[id_vp]
                    # ~ ids_dest_activity = ids_activity[(activities.ids_activitytype[ids_activity] != demand.activitytypes.names.get_id_from_index('home'))]
                    # ~ for id_dest_activity in ids_dest_activity:
                        # ~ destinations_str[activities.ids_facility[id_dest_activity]]+=1

        unit = self.unit_mapscale
        mapscale = self.get_attrsman().get_config('unit_mapscale').mapscales[unit]
        ax = init_plot(tight_layout=True)
        self.plot_net(ax, mapscale=mapscale,  unit=unit, is_configure=False)

# -----------------------Import facilities and values
        # ['residents per facility', 'destinations per facility', 'destinations per landuse','destinations per strategy', 'destinations per activity']
        if self.facilityfillmode == 'occupancies':
            ids_fac = list(occupancies.keys())
            values = list(occupancies.values())
            if self.is_specific_facility:
                title = 'Occupancies of facility %i' % (self.specific_facility)
            else:
                title = 'Occupancies per facility'

        if self.facilityfillmode == 'capacities':
            ids_fac = list(capacities.keys())
            values = list(capacities.values())
            if self.is_specific_facility:
                title = 'Capacities of facility %i' % (self.specific_facility)
            else:
                title = 'Capacities per facility'

        if self.facilityfillmode == 'residents per facility':
            ids_fac = list(residents.keys())
            values = list(residents.values())
            if self.is_specific_facility:
                title = 'Residents per residential facility from facility %i' % (self.specific_facility)
            else:
                title = 'Residents per residential facility'

        if self.facilityfillmode == 'destinations per facility':
            ids_fac = list(destinations_fac.keys())
            values = list(destinations_fac.values())
            if self.is_specific_facility:
                title = 'Destinations per facility from facility %i' % (self.specific_facility)
            else:
                title = 'Destinations per facility'
        # ~ if self.facilityfillmode == 'destinations per landuse':
            # ~ ids_fac = destinations_lan.keys()
            # ~ values = destinations_lan.values()
            # ~ if self.is_specific_facility:
                # ~ title = 'Destinations per landuse from facility %i'%(self.specific_facility)
            # ~ else:
                # ~ title = 'Destinations per landuse'
        # ~ if self.facilityfillmode == 'destinations per strategy':
            # ~ ids_fac = destinations_str.keys()
            # ~ values = destinations_str.values()
            # ~ if self.is_specific_facility:
                # ~ title = 'Destinations per strategy from facility %i'%(self.specific_facility)
            # ~ else:
                # ~ title = 'Destinations per strategy'
        # ~ if self.facilityfillmode == 'destinations per activity':
            # ~ ids_fac = destinations_act.keys()
            # ~ values = destinations_act.values()
            # ~ if self.is_specific_facility:
                # ~ title = 'Destinations per activity from facility %i'%(self.specific_facility)
            # ~ else:
                # ~ title = 'Destinations per activity'

        ppatches = []
        edgecolors = []
        values_positive = []
# -----------------------Add polygons
        for id_fac, shape, value in zip(ids_fac, facilities.shapes[ids_fac], values):

            if self.is_use_landusetype_color_for_bordes:
                edge_color = landuse.landusetypes.colors[facilities.ids_landusetype[id_fac]]
            else:
                edge_color = 'b'

            if '_area' in landuse.landusetypes.typekeys[facilities.ids_landusetype[id_fac]]:
                poly = mpl.patches.Polygon(np.array(shape)[:, :2]*mapscale,
                                           linewidth=self.linewidth_facilityborders,
                                           edgecolor=edge_color,
                                           facecolor='w',
                                           fill='true',
                                           alpha=0.2,
                                           zorder=0)
            else:
                poly = mpl.patches.Polygon(np.array(shape)[:, :2]*mapscale,
                                           linewidth=self.linewidth_facilityborders,
                                           edgecolor=edge_color,
                                           facecolor='w',
                                           fill='true',
                                           alpha=0.2,
                                           zorder=0)
            if value > 0:
                edgecolors.append(edge_color)
                values_positive.append(value)
                ppatches.append(poly)
            else:
                ax.add_patch(poly)

# -----------------------Fac values +
            if self.is_show_facilityvalues:
                if self.is_show_id_facility:
                    text = "ID:%d, val: %d" % (id_fac, value)
                else:
                    text = "%d" % (value)

                ax.text(facilities.centroids[id_fac][0], facilities.centroids[id_fac][1],
                        text,
                        ha='left', va='bottom',
                        fontsize=int(0.8*self.size_labelfont),
                        # backgroundcolor = 'w',
                        bbox=dict(edgecolor='k', facecolor='w', alpha=0.8),
                        zorder=1000,
                        )

            elif self.is_show_id_facility:
                ax.text(facilities.centroids[id_fac][0], facilities.centroids[id_fac][1],
                        "ID:%d" % (id_fac),
                        ha='left', va='bottom',
                        fontsize=int(0.8*self.size_labelfont),
                        # backgroundcolor = 'w',
                        bbox=dict(edgecolor='k', facecolor='w', alpha=0.8),
                        zorder=1000,
                        )

# -----------------------Color map +
        cmap = mpl.cm.jet
        patchcollection = PatchCollection(
            ppatches, cmap=cmap, linewidths=self.linewidth_facilityborders, edgecolors=edgecolors, alpha=self.alpha_cmap)
        patchcollection.set_array(np.array(values_positive, dtype=np.float32))
        ax.add_collection(patchcollection)
        cbar = plt.colorbar(patchcollection)
        cbar.ax.set_ylabel(self.facilityfillmode)
        for l in cbar.ax.yaxis.get_ticklabels():
            # l.set_weight("bold")
            l.set_fontsize(self.size_labelfont)

# -----------------------Save
        if self.title == '':
            pass
        else:
            title == self.title

        self.configure_map(ax, title=title, unit=unit)
        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('activitymap')

# -----------------------Plot +
        if not self.is_save:
            show_plot()

        return True
