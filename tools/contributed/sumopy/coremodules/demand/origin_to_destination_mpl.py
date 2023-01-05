# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
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

# @file    origin_to_destination_mpl.py
# @author  Joerg Schweizer
# @date   2012

import os
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patche
from coremodules import demand
#from coremodules.demand.origin_to_destination import OdIntervals
from coremodules.misc.matplottools import *
import agilepy.lib_base.xmlman as xm

from agilepy.lib_base.processes import Process


class OdPlots(PlotoptionsMixin, Process):
    def __init__(self, ident, demand, logger=None, **kwargs):
        print 'OdPlots.__init__'
        self._init_common(ident,
                          parent=demand,
                          name='OD plots',
                          logger=logger,
                          info='Plot of OD data.',
                          )
        attrsman = self.set_attrsman(cm.Attrsman(self))
        modes = demand.parent.net.modes
        self.od_table = demand.odintervals.generate_odflows()

        self.zonefillmode = attrsman.add(cm.AttrConf('zonefillmode', kwargs.get('zonefillmode', 'zone color'),
                                                     choices=['zone color', 'flows in - flows out',
                                                              'flows in + flows out', 'flows in', 'flows out'],
                                                     groupnames=['options'],
                                                     name='Zone fill mode',
                                                     info='Defines how each zone is filled.',
                                                     ))

        ids_mode = list(set(self.od_table.ids_mode.get_value()))
        modechoices = {}
        for id_mode, modename in zip(ids_mode, modes.names[ids_mode]):
            modechoices[modename] = id_mode

        self.ids_mode = attrsman.add(cm.ListConf('ids_mode', 1*ids_mode,
                                                 groupnames=['options'],
                                                 choices=modechoices,
                                                 name='Modes',
                                                 info="""Modes to be visualized""",
                                                 ))

        ids_inter = demand.odintervals.get_ids()
        intervalchoices = {}
        for id_int, time_start, time_end in zip(ids_inter, demand.odintervals.times_start[ids_inter], demand.odintervals.times_end[ids_inter]):
            intervalchoices[str(int(time_start/60.0))+'min-'+str(int(time_end/60.0)) +
                            'min'] = (int(time_start), int(time_end))

        self.intervals = attrsman.add(cm.ListConf('intervals', intervalchoices.values(),
                                                  groupnames=['options'],
                                                  choices=intervalchoices,
                                                  name='Intervals',
                                                  info="""Time intervals that get visualized""",
                                                  ))

        self.color_zones = attrsman.add(cm.AttrConf('color_zones',  kwargs.get('color_zones', np.array([0.9, 0.9, 0.9, 1.0], dtype=np.float32)),
                                                    groupnames=['options', 'zones'],
                                                    perm='wr',
                                                    metatype='color',
                                                    name='Zone color',
                                                    info='Fill color of Zone, in case the "zone color" is set to "color"',
                                                    ))

        self.alpha_zones = attrsman.add(cm.AttrConf('alpha_zones', kwargs.get('alpha_zones', 0.3),
                                                    groupnames=['options', 'zones'],
                                                    name='Zone transparency',
                                                    info='Transparency of fill color of zones.',
                                                    ))

        self.color_zoneborders = attrsman.add(cm.AttrConf('color_zoneborders', kwargs.get('color_zoneborders', np.array([0.0, 0.7, 0.0, 1.0], dtype=np.float32)),
                                                          groupnames=['options', 'zones'],
                                                          perm='wr',
                                                          metatype='color',
                                                          name='Zone border color',
                                                          info='Color of zone border.',
                                                          ))
        self.linewidth_zoneborders = attrsman.add(cm.AttrConf('linewidth_zoneborders', kwargs.get('linewidth_zoneborders', 3),
                                                              groupnames=['options', 'zones'],
                                                              name='Zone border line widtg',
                                                              info='Transparency of fill color of zones.',
                                                              ))

        self.is_show_id_zone = attrsman.add(cm.AttrConf('is_show_id_zone', kwargs.get('is_show_id_zone', True),
                                                        groupnames=['options'],
                                                        name='Show zone IDs',
                                                        info='Show ID of each zone.',
                                                        ))

        self.is_show_zonevalues = attrsman.add(cm.AttrConf('is_show_zonevalues', kwargs.get('is_show_zonevalues', False),
                                                           groupnames=['options'],
                                                           name='Show zone values',
                                                           info='Show values of each zone. Values depend on the choise of "zonefillmode"',
                                                           ))

        self.is_show_flows = attrsman.add(cm.AttrConf('is_show_flows', kwargs.get('is_show_flows', True),
                                                      groupnames=['options'],
                                                      name='Show OD flows',
                                                      info='Show OD flows as arrows between the zone, these are sometimes called the "desired lines".',
                                                      ))

        self.color_flows = attrsman.add(cm.AttrConf('color_flows', kwargs.get('color_flows', np.array([0.3, 0.9, 0.3, 1.0], dtype=np.float32)),
                                                    groupnames=['options', 'zones'],
                                                    perm='wr',
                                                    metatype='color',
                                                    name='Flow color',
                                                    info='Flow color if color does not vary with flow value.',
                                                    ))

        self.is_show_flow_colorvalues = attrsman.add(cm.AttrConf('is_show_flow_colorvalues', kwargs.get('is_show_flow_colorvalues', False),
                                                                 groupnames=['options'],
                                                                 name='Show OD flows colors',
                                                                 info='Show OD flows as colored arrows, where color varies with flow value.',
                                                                 ))

        self.width_flows_max = attrsman.add(cm.AttrConf('width_flows_max', kwargs.get('width_flows_max', 100.0),
                                                        groupnames=['options'],
                                                        name='Max. OD flow width',
                                                        info='Max. OD flow width on map.',
                                                        ))
        # self.is_show_flowvalues = attrsman.add(cm.AttrConf( 'is_show_flowvalues', kwargs.get('is_show_flowvalues', False),
        #                                groupnames = ['options'],
        #                                name = 'Show flow values',
        #                                info = 'Label flows with flow values.',
        #                                ))

        self.add_networkoptions(**kwargs)
        self.add_facilityoptions(**kwargs)
        # self.add_zoneoptions(**kwargs)
        self.add_plotoptions_mapbase(**kwargs)
        self.add_plotoptions_base(**kwargs)
        self.add_save_options(**kwargs)

    def show(self):
        print 'OdPlots.show'
        # if self.axis  is None:
        scenario = self.parent.get_scenario()
        zones = scenario.landuse.zones
        ids_zone = zones.get_ids()
        n_zones = len(ids_zone)

        ids = self.od_table.get_ids()

        #outflows = np.zeros(n_zones, dtype = np.int32)
        #inflows  = np.zeros(n_zones, dtype = np.int32)
        outflows = {}
        inflows = {}
        balance = {}
        totals = {}
        odflows = {}
        for id_zone in ids_zone:
            outflows[id_zone] = 0
            inflows[id_zone] = 0
            balance[id_zone] = 0
            totals[id_zone] = 0

        for id_zone_orig, id_zone_dest, id_mode, time_start, time_end, n_trips\
            in zip(self.od_table.ids_orig[ids],
                   self.od_table.ids_dest[ids],
                   self.od_table.ids_mode[ids],
                   self.od_table.times_start[ids],
                   self.od_table.times_end[ids],
                   self.od_table.tripnumbers[ids],
                   ):
            inter = (int(time_start), int(time_end))
            # print '  id_zone_orig',id_zone_orig,'id_zone_dest',id_zone_dest,'inter',inter,inter in self.intervals
            if (inter in self.intervals)\
                    & (id_mode in list(self.ids_mode)):
                outflows[id_zone_orig] += n_trips
                inflows[id_zone_dest] += n_trips
                od = (id_zone_orig, id_zone_dest)
                if odflows.has_key(od):
                    odflows[od] += n_trips
                else:
                    odflows[od] = n_trips

        for id_zone in ids_zone:
            balance[id_zone] = inflows[id_zone] - outflows[id_zone]
            totals[id_zone] = inflows[id_zone] + outflows[id_zone]
            # debug
            print ' id_zone', id_zone, 'in', inflows[id_zone], 'out', outflows[id_zone], 'balance', balance[id_zone], 'totals', totals[id_zone]

        unit = self.unit_mapscale
        mapscale = self.get_attrsman().get_config('unit_mapscale').mapscales[unit]
        ax = init_plot(tight_layout=True)

        self.plot_net(ax, mapscale=mapscale,  unit=unit, is_configure=False)

        #self.zonefillmode  = ['zone color','flows in - flows out','flows in + flows out','flows in','flows out'],
        if self.zonefillmode == 'flows in - flows out':
            ids_zone = balance.keys()
            values = balance.values()
        elif self.zonefillmode == 'flows in + flows out':
            ids_zone = totals.keys()
            values = totals.values()
        elif self.zonefillmode == 'flows in':
            ids_zone = inflows.keys()
            values = inflows.values()
        elif self.zonefillmode == 'flows out':
            ids_zone = outflows.keys()
            values = outflows.values()
        else:
            # dummy
            ids_zone = balance.keys()
            values = balance.values()
        ppatches = []
        for id_zone, shape, value in zip(ids_zone, zones.shapes[ids_zone], values):

            poly = mpl.patches.Polygon(np.array(shape)[:, :2]*mapscale,
                                       linewidth=self.linewidth_zoneborders,
                                       edgecolor=self.color_zoneborders,
                                       facecolor=self.color_zones,
                                       fill='true',
                                       alpha=self.alpha_zones,
                                       zorder=0)
            ppatches.append(poly)
            if self.is_show_zonevalues & (self.zonefillmode != 'zone color'):
                if self.is_show_id_zone:
                    text = "ID:%d, val: %d" % (id_zone, value)
                else:
                    text = "%d" % (value)

                ax.text(zones.coords[id_zone][0], zones.coords[id_zone][1],
                        text,
                        ha='left', va='bottom',
                        fontsize=int(0.8*self.size_labelfont),
                        #backgroundcolor = 'w',
                        bbox=dict(edgecolor='k', facecolor='w', alpha=0.8),
                        zorder=1000,
                        )

            elif self.is_show_id_zone:
                ax.text(zones.coords[id_zone][0], zones.coords[id_zone][1],
                        "ID:%d" % (id_zone),
                        ha='left', va='bottom',
                        fontsize=int(0.8*self.size_labelfont),
                        #backgroundcolor = 'w',
                        bbox=dict(edgecolor='k', facecolor='w', alpha=0.8),
                        zorder=1000,
                        )

        if (self.zonefillmode != 'zone color') & (not self.is_show_flow_colorvalues):
            if self.zonefillmode == 'flows in - flows out':
                cmap = mpl.cm.seismic
            else:
                cmap = mpl.cm.jet
            patchcollection = PatchCollection(ppatches, cmap=cmap, alpha=self.alpha_zones)
            print '  values', values
            patchcollection.set_array(np.array(values, dtype=np.float32))

            ax.add_collection(patchcollection)

            cbar = plt.colorbar(patchcollection)

            cbar.ax.set_ylabel(self.zonefillmode)
            for l in cbar.ax.yaxis.get_ticklabels():
                # l.set_weight("bold")
                l.set_fontsize(self.size_labelfont)
        else:
            for patch in ppatches:
                ax.add_patch(patch)

        if self.is_show_flows:
            values_raw = np.array(odflows.values(), dtype=np.float32)
            widthcoeff = self.width_flows_max/np.max(values_raw)

            apatches = []
            for od, flow in odflows.iteritems():
                id_zone_orig, id_zone_dest = od
                x1, y1 = zones.coords[id_zone_orig][:2]
                x2, y2 = zones.coords[id_zone_dest][:2]
                width = widthcoeff*flow

                # if self.is_show_flowvalues & self.is_show_flow_colorvalues:
                #    text = str(int(flow))
                # else:
                #    text = ''
                print '  x1, y1', x1, y1, 'x2, y2', x2, y2
                if id_zone_orig != id_zone_dest:
                    patch = FancyArrow(x1*mapscale, y1*mapscale, (x2-x1)*mapscale, (y2-y1)*mapscale,
                                       width=width,
                                       antialiased=True,
                                       edgecolor='black', facecolor=self.color_flows,
                                       head_width=1.5*width, head_length=1.2*width,
                                       # label = text,# not putting number on arrow but on legend
                                       length_includes_head=True,
                                       # overhang = 1.2,# ugly, not working properly
                                       fill=True,
                                       shape='right',  # right,full #show only right half
                                       zorder=2)

                else:
                    patch = Circle((x1*mapscale, y1*mapscale,),
                                   width,
                                   antialiased=True,
                                   edgecolor='black',
                                   facecolor=self.color_flows,
                                   fill=True,
                                   zorder=30,
                                   )

                apatches.append(patch)

            if self.is_show_flow_colorvalues:
                cmap = mpl.cm.jet
                apatchcollection = PatchCollection(apatches, cmap=cmap, alpha=1.0)
                # print '  values_raw',values_raw
                apatchcollection.set_array(values_raw)

                ax.add_collection(apatchcollection)

                cbar = plt.colorbar(apatchcollection)

                cbar.ax.set_ylabel(self.zonefillmode)
                for l in cbar.ax.yaxis.get_ticklabels():
                    # l.set_weight("bold")
                    l.set_fontsize(self.size_labelfont)
            else:
                for patch in apatches:
                    ax.add_patch(patch)

        # if self.is_show_flows & self.is_show_flow_colorvalues:
        #    #plt.legend(apatches, fontsize=self.size_labelfont)
        #    plt.legend(fontsize=self.size_labelfont)

        self.configure_map(ax, title='Zone to Zone flows', unit=unit)

        if self.is_save:
            plt.subplots_adjust(left=0.12, bottom=0.1, right=0.86, top=0.9, wspace=0.2, hspace=0.2)
            self.save_fig('odflows')

        if not self.is_save:
            show_plot()

        return True
