# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2013-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    helpers.py
# @author  Daniel Krajzewicz
# @author  Laura Bieker
# @author  Michael Behrisch
# @author  Mirko Barthauer
# @date    2013-11-11

from __future__ import absolute_import
from __future__ import print_function

import gc
import matplotlib
from ..options import ArgumentParser
from pylab import arange, close, cm, figure, legend, log, plt, savefig, show, title
from pylab import xlabel, xlim, xticks, ylabel, ylim, yticks
from matplotlib.ticker import FuncFormatter as ff
from matplotlib.collections import LineCollection
mpl_version = tuple(map(int, matplotlib.__version__.split(".")[:3]))

# http://datadebrief.blogspot.de/2010/10/plotting-sunrise-sunset-times-in-python.html


def m2hm0(x, i):
    h = int(x / 3600)
    return '%(h)02d' % {'h': h}


def m2hm1(x, i):
    h = int(x / 3600)
    m = int((x % 3600) / 60)
    return '%(h)02d:%(m)02d' % {'h': h, 'm': m}


def m2hm2(x, i):
    h = int(x / 3600)
    m = int((x % 3600) / 60)
    s = int(x % 60)
    return '%(h)02d:%(m)02d:%(s)02d' % {'h': h, 'm': m, 's': s}


def addPlotOptions(ap):
    ap.add_argument("--colors", dest="colors", category="visualization",
                    default=None, help="Defines the colors to use")
    ap.add_argument("--colormap", dest="colormap", category="visualization",
                    default="nipy_spectral", help="Defines the colormap to use")
    ap.add_argument("-l", "--labels", dest="labels", category="visualization",
                    default=None, help="Defines the labels to use")
    ap.add_argument("--xlim", dest="xlim", category="visualization",
                    default=None, help="Defines x-limits of the figure XMIN,XMAX")
    ap.add_argument("--ylim", dest="ylim", category="visualization",
                    default=None, help="Defines y-limits of the figure YMIN,YMAX")
    ap.add_argument("--xticks", dest="xticks", category="visualization",
                    default=None, help="Set x-axis ticks XMIN,XMAX,XSTEP,XSIZE or XSIZE")
    ap.add_argument("--yticks", dest="yticks", category="visualization",
                    default=None, help="Set y-axis ticks YMIN,YMAX,YSTEP,YSIZE or YSIZE")
    ap.add_argument("--xticks-file", dest="xticksFile", category="input", type=ap.file,
                    default=None, help="Load x-axis ticks from file (LABEL or FLOAT:LABEL per line)")
    ap.add_argument("--yticks-file", dest="yticksFile", category="input", type=ap.file,
                    default=None, help="Load y-axis ticks from file (LABEL or FLOAT:LABEL per line)")
    ap.add_argument("--xtime0", dest="xtime0", action="store_true", category="time",
                    default=False, help="Use a time formatter for x-ticks (hh)")
    ap.add_argument("--ytime0", dest="ytime0", action="store_true", category="time",
                    default=False, help="Use a time formatter for y-ticks (hh)")
    ap.add_argument("--xtime1", dest="xtime1", action="store_true", category="time",
                    default=False, help="Use a time formatter for x-ticks (hh:mm)")
    ap.add_argument("--ytime1", dest="ytime1", action="store_true", category="time",
                    default=False, help="Use a time formatter for y-ticks (hh:mm)")
    ap.add_argument("--xtime2", dest="xtime2", action="store_true", category="time",
                    default=False, help="Use a time formatter for x-ticks (hh:mm:ss)")
    ap.add_argument("--ytime2", dest="ytime2", action="store_true", category="time",
                    default=False, help="Use a time formatter for y-ticks (hh:mm:ss)")
    ap.add_argument("--xgrid", dest="xgrid", action="store_true", category="visualization",
                    default=False, help="Enable grid on x-axis")
    ap.add_argument("--ygrid", dest="ygrid", action="store_true", category="visualization",
                    default=False, help="Enable grid on y-axis")
    ap.add_argument("--xticksorientation", dest="xticksorientation", category="visualization",
                    type=float, default=None, help="Set the orientation of the x-axis ticks")
    ap.add_argument("--yticksorientation", dest="yticksorientation", category="visualization",
                    type=float, default=None, help="Set the orientation of the x-axis ticks")
    ap.add_argument("--xlabel", dest="xlabel", category="visualization",
                    default=None, help="Set the x-axis label")
    ap.add_argument("--ylabel", dest="ylabel", category="visualization",
                    default=None, help="Set the y-axis label")
    ap.add_argument("--xlabelsize", dest="xlabelsize", category="visualization",
                    type=int, default=16, help="Set the size of the x-axis label")
    ap.add_argument("--ylabelsize", dest="ylabelsize", category="visualization",
                    type=int, default=16, help="Set the size of the x-axis label")
    ap.add_argument("--marker", dest="marker", default=None, category="visualization",
                    help="marker for single points (default o for scatter, None otherwise)")
    ap.add_argument("--linestyle", dest="linestyle", default="-", category="visualization",
                    help="plot line style (default -)")
    ap.add_argument("--title", dest="title", category="visualization",
                    default=None, help="Set the title")
    ap.add_argument("--titlesize", dest="titlesize", category="visualization",
                    type=int, default=16, help="Set the title size")
    ap.add_argument("--adjust", dest="adjust", category="visualization",
                    default=None, help="Adjust the subplots LEFT,BOTTOM or LEFT,BOTTOM,RIGHT,TOP")
    ap.add_argument("-s", "--size", dest="size", category="visualization",
                    default=False, help="Defines the figure size X,Y")
    ap.add_argument("--no-legend", dest="nolegend", action="store_true", category="visualization",
                    default=False, help="Disables the legend")
    ap.add_argument("--legend-position", dest="legendposition", category="visualization",
                    default=None, help="Sets the legend position")
    ap.add_argument("--dpi", dest="dpi", type=float, category="visualization",
                    default=None, help="Define dpi resolution for figures")
    ap.add_argument("--alpha", type=float,
                    default=1., help="Define background transparency of the figure in the range 0..1")


def addInteractionOptions(optParser):
    optParser.add_option("-o", "--output", category="output", dest="output", metavar="FILE",
                         type=ArgumentParser.file_list,
                         default=None, help="Comma separated list of filename(s) the figure shall be written to")
    optParser.add_option("-b", "--blind", dest="blind", action="store_true",
                         default=False, help="If set, the figure will not be shown")


def addNetOptions(optParser):
    optParser.add_option("-w", "--default-width", dest="defaultWidth",
                         type=float, default=.1, help="Defines the default edge width")
    optParser.add_option("--default-color", dest="defaultColor",
                         default='k', help="Defines the default edge color")


def applyPlotOptions(fig, ax, options):
    if options.xlim:
        xlim(float(options.xlim.split(",")[0]), float(
            options.xlim.split(",")[1]))
    if options.yticksorientation:
        ax.tick_params(
            axis='y', which='major', tickdir=options.xticksorientation)
    if options.xticks:
        vals = options.xticks.split(",")
        if len(vals) == 1:
            ax.tick_params(axis='x', which='major', labelsize=float(vals[0]))
        elif len(vals) == 4:
            xticks(arange(float(vals[0]), float(vals[1]), float(vals[2])), size=float(vals[3]))
        else:
            raise ValueError(
                "Error: ticks must be given as one float (<SIZE>) or four floats (<MIN>,<MAX>,<STEP>,<SIZE>)")
    if options.xticksFile:
        xticks(*parseTicks(options.xticksFile))
    if options.xtime0:
        if max(ax.get_xticks()) < 3600:
            print("Warning: x ticks not suited for hh format.")
        ax.xaxis.set_major_formatter(ff(m2hm0))
    if options.xtime1:
        if max(ax.get_yticks()) < 60:
            print("Warning: x ticks not suited for hh:mm format.")
        ax.xaxis.set_major_formatter(ff(m2hm1))
    if options.xtime2:
        ax.xaxis.set_major_formatter(ff(m2hm2))
    if options.xgrid:
        ax.xaxis.grid(True)
    if options.xlabel:
        xlabel(options.xlabel, size=options.xlabelsize)
    if options.xticksorientation:
        labels = ax.get_xticklabels()
        for label in labels:
            label.set_rotation(options.xticksorientation)

    if options.ylim:
        ylim(float(options.ylim.split(",")[0]), float(
            options.ylim.split(",")[1]))
    if options.yticks:
        vals = options.yticks.split(",")
        if len(vals) == 1:
            ax.tick_params(axis='y', which='major', labelsize=float(vals[0]))
        elif len(vals) == 4:
            yticks(
                arange(float(vals[0]), float(vals[1]), float(vals[2])), size=float(vals[3]))
        else:
            raise ValueError(
                "Error: ticks must be given as one float (<SIZE>) or four floats (<MIN>,<MAX>,<STEP>,<SIZE>)")
    if options.yticksFile:
        yticks(*parseTicks(options.yticksFile))
    if options.ytime0:
        if max(ax.get_yticks()) < 3600:
            print("Warning: y ticks not suited for hh format.")
        ax.yaxis.set_major_formatter(ff(m2hm0))
    if options.ytime1:
        if max(ax.get_yticks()) < 60:
            print("Warning: y ticks not suited for hh:mm format.")
        ax.yaxis.set_major_formatter(ff(m2hm1))
    if options.ytime2:
        ax.yaxis.set_major_formatter(ff(m2hm2))
    if options.ygrid:
        ax.yaxis.grid(True)
    if options.ylabel:
        ylabel(options.ylabel, size=options.ylabelsize)
    if options.yticksorientation:
        labels = ax.get_yticklabels()
        for label in labels:
            label.set_rotation(options.yticksorientation)

    if options.title:
        title(options.title, size=options.titlesize)
    if options.adjust:
        vals = options.adjust.split(",")
        if len(vals) == 2:
            fig.subplots_adjust(left=float(vals[0]), bottom=float(vals[1]))
        elif len(vals) == 4:
            fig.subplots_adjust(left=float(vals[0]), bottom=float(
                vals[1]), right=float(vals[2]), top=float(vals[3]))
        else:
            raise ValueError(
                "Error: adjust must be given as two floats (<LEFT>,<BOTTOM>) or four floats " +
                "(<LEFT>,<BOTTOM>,<RIGHT>,<TOP>)")
    if options.alpha is not None:
        alpha = max(0., min(1., options.alpha))
        fig.patch.set_alpha(alpha)
        ax.patch.set_alpha(alpha)


def plotNet(net, colors, widths, options):
    shapes = []
    c = []
    w = []
    for e in net._edges:
        shapes.append(e.getShape())
        if e._id in colors:
            c.append(colors[str(e._id)])
        else:
            c.append(options.defaultColor)
        if e._id in widths:
            w.append(widths[str(e._id)])
        else:
            w.append(options.defaultWidth)

    line_segments = LineCollection(shapes, linewidths=w, colors=c, linestyles=options.linestyle)
    ax = plt.gca()
    ax.add_collection(line_segments)
    ax.set_xmargin(0.1)
    ax.set_ymargin(0.1)
    ax.autoscale_view(True, True, True)


def getColorMap(options):
    if mpl_version < (3, 6, 0):
        return matplotlib.cm.get_cmap(options.colormap)
    return matplotlib.colormaps[options.colormap]


def getColor(options, i, a):
    if options.colors:
        v = options.colors.split(",")
        if i >= len(v):
            raise ValueError("Error: not enough colors given")
        return v[i]
    if options.colormap[0] == '#':
        colormap = parseColorMap(options.colormap[1:])
        if mpl_version < (3, 6, 0):
            cm.register_cmap(name="CUSTOM", cmap=colormap)
        else:
            matplotlib.colormaps.register(name="CUSTOM", cmap=colormap)
        options.colormap = "CUSTOM"
    cNorm = matplotlib.colors.Normalize(vmin=0, vmax=a)
    scalarMap = matplotlib.cm.ScalarMappable(norm=cNorm, cmap=getColorMap(options))
    return scalarMap.to_rgba(i)


def getLabel(f, i, options):
    label = f
    if options.labels:
        label = options.labels.split(",")[i]
    return label


def openFigure(options):
    if options.size:
        x = float(options.size.split(",")[0])
        y = float(options.size.split(",")[1])
        fig = figure(figsize=(x, y))
    else:
        fig = figure()
    ax = fig.add_subplot(111)
    return fig, ax


def closeFigure(fig, ax, options, haveLabels=True, optOut=None):
    if haveLabels and not options.nolegend:
        if options.legendposition:
            legend(loc=options.legendposition)
        else:
            legend()
    applyPlotOptions(fig, ax, options)
    if options.output or optOut is not None:
        n = options.output
        myDpi = options.dpi
        if myDpi is not None:
            myDpi = float(myDpi)
        if optOut is not None:
            n = optOut
        for o in n.split(","):
            savefig(o, dpi=myDpi)
    if not options.blind:
        show()
    try:
        fig.clf()
    except:  # noqa
        pass
    close()
    gc.collect()


def logNormalise(values, maxValue):
    if not maxValue:
        for e in values:
            if not maxValue or maxValue < values[e]:
                maxValue = values[e]
    emin = None
    emax = None
    for e in values:
        if values[e] != 0:
            values[e] = log(values[e]) / log(maxValue)
        if not emin or emin > values[e]:
            emin = values[e]
        if not emax or emax < values[e]:
            emax = values[e]
    if emax is not None and emin is not None:
        valRange = emax - emin
        if valRange == 0:
            valRange = 1
        for e in values:
            values[e] = (values[e] - emin) / valRange


def linNormalise(values, minColorValue, maxColorValue):
    if minColorValue is not None and maxColorValue is not None:
        valRange = maxColorValue - minColorValue
        if valRange == 0:
            valRange = 1
        for e in values:
            values[e] = (values[e] - minColorValue) / valRange


def toHex(val):
    """Converts the given value (0-255) into its hexadecimal representation"""
    hex = "0123456789abcdef"
    return hex[int(val / 16)] + hex[int(val - int(val / 16) * 16)]


def toFloat(val):
    """Converts the given value (0-255) into its hexadecimal representation"""
    hex = "0123456789abcdef"
    return float(hex.find(val[0]) * 16 + hex.find(val[1]))


def toColor(val, colormap):
    """Converts the given value (0-1) into a color definition parseable by matplotlib"""
    for i in range(0, len(colormap) - 1):
        if colormap[i + 1][0] > val:
            scale = (val - colormap[i][0]) / \
                (colormap[i + 1][0] - colormap[i][0])
            r = colormap[i][1][0] + \
                (colormap[i + 1][1][0] - colormap[i][1][0]) * scale
            g = colormap[i][1][1] + \
                (colormap[i + 1][1][1] - colormap[i][1][1]) * scale
            b = colormap[i][1][2] + \
                (colormap[i + 1][1][2] - colormap[i][1][2]) * scale
            return "#" + toHex(r) + toHex(g) + toHex(b)
    return "#" + toHex(colormap[-1][1][0]) + toHex(colormap[-1][1][1]) + toHex(colormap[-1][1][2])


def parseColorMap(mapDef):
    ret = {"red": [], "green": [], "blue": []}
    defs = mapDef.split(",")
    for d in defs:
        (value, color) = d.split(":")
        value = float(value)
        r = color[1:3]
        g = color[3:5]
        b = color[5:7]
        # ret.append( (float(value), ( toFloat(r), toFloat(g), toFloat(b) ) ) )
        ret["red"].append((value, toFloat(r) / 255., toFloat(r) / 255.))
        ret["green"].append((value, toFloat(g) / 255., toFloat(g) / 255.))
        ret["blue"].append((value, toFloat(b) / 255., toFloat(b) / 255.))

        # ret.append( (value, color) )
    colormap = matplotlib.colors.LinearSegmentedColormap("CUSTOM", ret, 1024)
    return colormap


def parseTicks(tickfile):
    # whether we're loading <FLOAT>:<LABEL> instead of <LABEL>
    haveOffsets = True
    offsets = []
    labels = []
    with open(tickfile) as tf:
        for line in tf:
            line = line.strip()
            if not line:
                continue
            of_label = line.split(':')
            try:
                of = float(of_label[0])
                offsets.append(of)
                if len(of_label) > 1:
                    labels.append(' '.join(of_label[1:]))
                else:
                    # also accept <FLOAT> format
                    labels.append(str(of))
            except ValueError:
                haveOffsets = False
                labels.append(line)

    if not haveOffsets:
        offsets = range(len(labels))
    return offsets, labels
