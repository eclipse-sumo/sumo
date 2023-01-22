#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2007-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    plotXMLAttributes.py
# @author  Jakob Erdmann
# @author  Mirko Barthauer
# @date    2021-11-05

"""
This script plots arbitrary xml attributes from xml files
Individual trajectories can be clicked in interactive mode to print the data Id on the console

selects two attributs for x and y axis and a third (id-attribute) for grouping
of data points into lines

"""
from __future__ import absolute_import
from __future__ import print_function
import os
import re
import sys
from collections import defaultdict
import fnmatch
import matplotlib
if 'matplotlib.backends' not in sys.modules:
    if 'TEXTTEST_SANDBOX' in os.environ or (os.name == 'posix' and 'DISPLAY' not in os.environ):
        matplotlib.use('Agg')
import matplotlib.pyplot as plt  # noqa

try:
    import xml.etree.cElementTree as ET
except ImportError as e:
    print("recovering from ImportError '%s'" % e)
    import xml.etree.ElementTree as ET

sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..'))
from sumolib.xml import _open  # noqa
from sumolib.miscutils import uMin, uMax, parseTime  # noqa
from sumolib.options import ArgumentParser, RawDescriptionHelpFormatter  # noqa
import sumolib.visualization.helpers  # noqa

RANK_ATTR = "@RANK"
NONE_ATTR = "@NONE"
NONE_ATTR_DEFAULT = 0


def getOptions(args=None):
    optParser = ArgumentParser(
        description='Plot arbitrary attributes from xml files',
        epilog='Individual trajectories can be clicked in interactive mode to print the data Id on the console\n'
        'selects two attributs for x and y axis and optionally a third (id-attribute)\n'
        'for grouping of data points into lines\n\n'
        'Example\n'
        '  plotXMLAttributes.py -x started -y initialPersons -s stopout.xml\n'
        '    plots passengers over time for vehicles from SUMO stop output',
        formatter_class=RawDescriptionHelpFormatter, conflict_handler='resolve')

    sumolib.visualization.helpers.addPlotOptions(optParser)
    sumolib.visualization.helpers.addInteractionOptions(optParser)
    optParser.add_option("-x", "--xattr",  help="attribute for x-axis")
    optParser.add_option("-y", "--yattr",  help="attribute for y-axis")
    optParser.add_option("-i", "--idattr",  default="id", help="attribute for grouping data points into lines")
    optParser.add_option("--xelem",  help="element for x-axis")
    optParser.add_option("--yelem",  help="element for y-axis")
    optParser.add_option("--idelem",  help="element for grouping data points into lines")
    optParser.add_option("-s", "--show", action="store_true", default=False, help="show plot directly")
    optParser.add_option("--csv-output", dest="csv_output", help="write plot as csv", metavar="FILE")
    optParser.add_option("--filter-ids", dest="filterIDs", help="only plot data points from the given list of ids")
    optParser.add_option("-p", "--pick-distance", dest="pickDist", type=float, default=1,
                         help="pick lines within the given distance in interactive plot mode")
    optParser.add_option("--label", help="plot label (default input file name")
    optParser.add_option("--join-x", action="store_true", dest="joinx", default=False,
                         help="if --xattr is a list concatenate the values")
    optParser.add_option("--join-y", action="store_true", dest="joiny", default=False,
                         help="if --yattr is a list concatenate the values")
    optParser.add_option("--xfactor", help="multiplier for x-data", type=float, default=1)
    optParser.add_option("--yfactor", help="multiplier for y-data", type=float, default=1)
    optParser.add_option("--invert-yaxis", dest="invertYAxis", action="store_true",
                         default=False, help="Invert the Y-Axis")
    optParser.add_option("--scatterplot", action="store_true",
                         default=False, help="Draw a scatterplot instead of lines")
    optParser.add_option("--legend", action="store_true", default=False, help="Add legend")
    optParser.add_option("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")
    optParser.add_argument("files", nargs='+', help="List of XML files to plot")

    options = optParser.parse_args(args=args)

    options.attrOptions = ['idattr', 'xattr', 'yattr']
    options.attrElems = [options.idelem, options.xelem, options.yelem]

    if options.filterIDs is not None:
        options.filterIDs = set(options.filterIDs.split(','))

    for a in options.attrOptions:
        if getattr(options, a) is None:
            sys.exit("mandatory argument --%s is missing" % a)

    if options.xlabel is None:
        options.xlabel = options.xattr
    if options.ylabel is None:
        options.ylabel = options.yattr

    # keep old presets from before integration of common options
    options.nolegend = not options.legend
    options.blind = not options.show
    if options.output is None:
        options.output = "plot.png"

    return options


def write_csv(data, fname):
    with open(fname, 'w') as f:
        for veh, vals in sorted(data.items()):
            f.write('# "%s"\n' % veh)
            for x in zip(*vals):
                f.write(" ".join(map(str, x)) + "\n")
            #  2 blank lines indicate a new data block in gnuplot
            f.write('\n\n')


def short_names(filenames):
    if len(filenames) == 1:
        return filenames
    reversedNames = [''.join(reversed(f)) for f in filenames]
    prefixLen = len(os.path.commonprefix(filenames))
    suffixLen = len(os.path.commonprefix(reversedNames))
    return [f[prefixLen:-suffixLen] for f in filenames]


def onpick(event):
    mevent = event.mouseevent
    print("dataID=%s x=%d y=%d" % (event.artist.get_label(), mevent.xdata, mevent.ydata))


def getDataStream(options):
    # determine elements and nesting for the given attributes
    # by reading from the first file

    attrOptions = options.attrOptions
    attr2elem = {}
    elem2level = {}

    # handle attribute lists
    allAttrs = set()
    attr2parts = {}
    for a in attrOptions:
        attr = getattr(options, a)
        parts = attr.split(',')
        allAttrs.update(parts)
        attr2parts[attr] = parts

    splitX = len(attr2parts[options.xattr]) > 1 and not options.joinx
    splitY = len(attr2parts[options.yattr]) > 1 and not options.joiny

    level = 0
    for event, elem in ET.iterparse(_open(options.files[0]), ("start", "end")):
        if event == "start":
            level += 1
            for a, e in zip(attrOptions, options.attrElems):
                attrOrig = getattr(options, a)
                for attr in attr2parts[attrOrig]:
                    if attr in elem.keys():
                        if e is not None and e != elem.tag:
                            # print("skipping attribute '%s' in element '%s' (required elem '%s'" % (attr, elem.tag, e))
                            continue
                        elem2level[elem.tag] = level
                        if attr in attr2elem:
                            oldTag = attr2elem[attr]
                            if oldTag != elem.tag:
                                if elem2level[oldTag] < level:
                                    attr2elem[attr] = elem.tag
                                print("Warning: found %s '%s' in element '%s' (level %s) and element '%s' (level %s)."
                                      " Using '%s'." % (
                                          a, attr, oldTag, elem2level[oldTag],
                                          elem.tag, level, attr2elem[attr]))
                        else:
                            attr2elem[attr] = elem.tag
            if len(attr2elem) == len(allAttrs):
                # all attributes have been seen
                break
        elif event == "end":
            level -= 1

    if len(attr2elem) != len(allAttrs):
        for a in attrOptions:
            attrOrig = getattr(options, a)
            for attr in attr2parts[attrOrig]:
                if attr not in attr2elem:
                    lvlElem = [(lv, el) for el, lv in elem2level.items()]
                    minLevelElem = sorted(lvlElem)[-1][1]
                    if attr == RANK_ATTR:
                        attr2elem[attr] = minLevelElem
                    else:
                        msg = "%s '%s' not found in %s" % (a, attr, options.files[0])
                        if a == 'idattr':
                            if attr != NONE_ATTR:
                                print(msg, file=sys.stderr)
                                options.idattr = NONE_ATTR
                            attr2elem[attr] = minLevelElem
                            allAttrs.remove(attr)
                            allAttrs.add(NONE_ATTR)
                            attr2parts[NONE_ATTR] = [NONE_ATTR]
                        else:
                            sys.exit("Mandatory " + msg)

    allElems = list(set(attr2elem.values()))
    attrs = [getattr(options, a) for a in attrOptions]

    # we don't know the order of the elements and we cannot get it from our xml parser
    if len(allElems) == 2:
        def datastream(xmlfile):
            skippedLines = defaultdict(int)
            elems = sorted(allElems, key=lambda e: elem2level[e])
            mE0 = "<%s " % elems[0]
            mE1 = "<%s " % elems[1]
            attrs0 = [a for a in allAttrs if attr2elem[a] == elems[0]]
            attrs1 = [a for a in allAttrs if attr2elem[a] == elems[1]]
            mAs0 = [(a, re.compile('%s="([^"]*)"' % a)) for a in attrs0]
            mAs1 = [(a, re.compile('%s="([^"]*)"' % a)) for a in attrs1]

            values = {}  # attr -> value
            index = 0
            for line in _open(xmlfile):
                if mE0 in line:
                    for a, r in mAs0:
                        values[a] = r.search(line).groups()[0]
                if mE1 in line:
                    skip = False
                    for a, r in mAs1:
                        m = r.search(line)
                        if m:
                            values[a] = m.groups()[0]
                        elif a == RANK_ATTR:
                            values[a] = index
                        elif a == NONE_ATTR:
                            values[a] = NONE_ATTR_DEFAULT
                        else:
                            skip = True
                            skippedLines[a] += 1
                    if not skip:
                        for toYield in combineValues(attrs, attr2parts, values, splitX, splitY):
                            yield toYield
                            index += 1

            for attr, count in skippedLines.items():
                print("Warning: Skipped %s lines because of missing attributes '%s'." % (
                    count, attr), file=sys.stderr)

        return datastream

    elif len(allElems) == 1:
        def datastream(xmlfile):
            mE = "<%s " % allElems[0]
            mAs = [re.compile('%s="([^"]*)"' % a) for a in allAttrs]
            index = 0
            for line in _open(xmlfile):
                if mE in line:
                    skip = False
                    values = {}  # attr -> value
                    for a, r in zip(allAttrs, mAs):
                        if a == RANK_ATTR:
                            values[a] = index
                        elif a == NONE_ATTR:
                            values[a] = NONE_ATTR_DEFAULT
                        else:
                            m = r.search(line)
                            if m:
                                values[a] = m.groups()[0]
                            else:
                                skip = True
                    if not skip:
                        for toYield in combineValues(attrs, attr2parts, values, splitX, splitY):
                            yield toYield
                            index += 1
        return datastream

    else:
        sys.exit("Found attributes at elements %s but at most 2 elements are supported" % allElems)


def combineValues(attrs, attr2parts, values, splitX, splitY):
    needSplit = splitX or splitY
    toYield = []
    for a, split in zip(attrs, [False, splitX, splitY]):
        if len(attr2parts[a]) == 1:
            if needSplit:
                toYield.append([values[a]])
            else:
                toYield.append(values[a])
        else:
            v = [values[ap] for ap in attr2parts[a]]
            if needSplit and split:
                toYield.append(v)
            elif needSplit:
                toYield.append(['|'.join(v)])
            else:
                toYield.append('|'.join(v))

    if needSplit:
        assert(len(toYield) == 3)
        splitIndex = 0
        for i in toYield[0]:
            for ix, x in enumerate(toYield[1]):
                for iy, y in enumerate(toYield[2]):
                    if attrs[0] == NONE_ATTR:
                        # build label from x or y parts
                        i = ""
                        if len(attr2parts[attrs[1]]) > 1:
                            i += attr2parts[attrs[1]][ix]
                        if len(attr2parts[attrs[2]]) > 1:
                            if i != "":
                                i += "|"
                            i += attr2parts[attrs[2]][iy]
                    yield [i, x, y]
                    splitIndex += 1
    else:
        yield toYield


def interpretValue(value):
    try:
        return parseTime(value)
    except ValueError:
        # use as category
        return value


def isnumeric(value):
    return type(value) == int or type(value) == float


def keepNumeric(d, xyIndex):
    res_x = []
    res_y = []
    for i in range(len(d[xyIndex])):
        if isnumeric(d[xyIndex][i]):
            res_x.append(d[0][i])
            res_y.append(d[1][i])
    d[0][:] = res_x
    d[1][:] = res_y


def useWildcards(labels):
    for label in labels:
        if "*" in label or "?" in label or ("[" in label and "]" in label):
            return True
    return False


def applyTicks(d, xyIndex, ticksFile):
    offsets, labels = sumolib.visualization.helpers.parseTicks(ticksFile)
    l2o = dict(zip(labels, offsets))
    if useWildcards(labels):
        def getOffset(val):
            for label in labels:
                if fnmatch.fnmatch(val, label):
                    return l2o[label]
            return None

    else:
        def getOffset(val):
            return l2o.get(val)

    res_x = []
    res_y = []
    for i in range(len(d[xyIndex])):
        val = d[xyIndex][i]
        o = getOffset(val)
        if o is not None:
            point = [d[0][i], d[1][i]]
            point[xyIndex] = o
            res_x.append(point[0])
            res_y.append(point[1])

    d[0][:] = res_x
    d[1][:] = res_y


def main(options):

    dataStream = getDataStream(options)

    fig = plt.figure(figsize=(14, 9), dpi=100)
    fig.canvas.mpl_connect('pick_event', onpick)

    shortFileNames = short_names(options.files)
    plt.xlabel(options.xlabel)
    plt.ylabel(options.ylabel)
    plt.title(','.join(shortFileNames) if options.label is None else options.label)
    xdata = 0
    ydata = 1

    data = defaultdict(lambda: tuple(([] for i in range(2))))

    numericXCount = 0
    stringXCount = 0
    numericYCount = 0
    stringYCount = 0

    for fileIndex, datafile in enumerate(options.files):
        totalIDs = 0
        filteredIDs = 0
        for dataID, x, y in dataStream(datafile):
            totalIDs += 1
            if options.filterIDs:
                # Try for regular strings.
                flag1 = dataID not in options.filterIDs
                # Try for wildcards.
                flag2 = False
                for filterID in options.filterIDs:
                    if '*' in filterID:
                        if not fnmatch.fnmatch(dataID, filterID):
                            continue
                        flag2 = True
                if flag1 and not flag2:
                    continue
            if len(options.files) > 1:
                suffix = shortFileNames[fileIndex]
                if len(suffix) > 0:
                    dataID += "#" + suffix
            x = interpretValue(x)
            y = interpretValue(y)
            if isnumeric(x):
                numericXCount += 1
                x *= options.xfactor
            else:
                stringXCount += 1
            if isnumeric(y):
                numericYCount += 1
                y *= options.yfactor
            else:
                stringYCount += 1

            data[dataID][xdata].append(x)
            data[dataID][ydata].append(y)
            filteredIDs += 1
        if totalIDs == 0 or filteredIDs == 0 or options.verbose:
            print("Found %s datapoints in %s and kept %s" % (totalIDs, datafile, filteredIDs))

    if filteredIDs == 0:
        sys.exit()

    minY = uMax
    maxY = uMin
    minX = uMax
    maxX = uMin

    for dataID, d in data.items():

        if numericXCount > 0 and stringXCount > 0:
            keepNumeric(d, xdata)
        if numericYCount > 0 and stringYCount > 0:
            keepNumeric(d, ydata)

        if options.xticksFile:
            applyTicks(d, xdata, options.xticksFile)
        if options.yticksFile:
            applyTicks(d, ydata, options.yticksFile)

        xvalues = d[xdata]
        yvalues = d[ydata]

        if len(xvalues) == 0:
            assert(len(yvalues) == 0)
            continue

        minY = min(minY, min(yvalues))
        maxY = max(maxY, max(yvalues))
        minX = min(minX, min(xvalues))
        maxX = max(maxX, max(xvalues))

        linestyle = options.linestyle
        marker = options.marker
        if options.scatterplot or (min(yvalues) == max(yvalues) and min(xvalues) == max(xvalues)):
            linestyle = ''
            if marker is None:
                marker = 'o'
        plt.plot(xvalues, yvalues, linestyle=linestyle, marker=marker, picker=True, label=dataID)

    if options.invertYAxis:
        plt.axis([minX, maxX, maxY, minY])

    if options.csv_output is not None:
        write_csv(data, options.csv_output)

    ax = fig.axes[0]
    sumolib.visualization.helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
    main(getOptions())
