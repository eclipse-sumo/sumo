#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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

selects two attributes for x and y axis and a third (id-attribute) for grouping
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
from sumolib import openz  # noqa
from sumolib.miscutils import uMin, uMax, parseTime  # noqa
from sumolib.options import ArgumentParser, RawDescriptionHelpFormatter  # noqa
import sumolib.visualization.helpers  # noqa

INDEX_ATTR = "@INDEX"
RANK_ATTR = "@RANK"
COUNT_ATTR = "@COUNT"
DENS_ATTR = "@DENSITY"
BOX_ATTR = "@BOX"
NONE_ATTR = "@NONE"
NONE_ATTR_DEFAULT = 0
ID_ATTR_DEFAULT = ""  # use filename instead

POST_PROCESSING_ATTRS = [RANK_ATTR, COUNT_ATTR, BOX_ATTR, DENS_ATTR]
SYMBOLIC_ATTRS = POST_PROCESSING_ATTRS + [INDEX_ATTR]
NON_DATA_ATTRS = SYMBOLIC_ATTRS + [NONE_ATTR]


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

    optParser.add_option("files", nargs='+', category="input", type=optParser.file_list,
                         help="List of XML files to plot")
    optParser.add_option("-x", "--xattr", help="attribute for x-axis")
    optParser.add_option("-y", "--yattr", help="attribute for y-axis")
    optParser.add_option("-i", "--idattr", default="id", help="attribute for grouping data points into lines")
    optParser.add_option("--xelem", help="element for x-axis")
    optParser.add_option("--yelem", help="element for y-axis")
    optParser.add_option("--idelem", help="element for grouping data points into lines")
    optParser.add_option("-s", "--show", action="store_true", category="output",
                         default=False, help="show plot directly")
    optParser.add_option("--csv-output", dest="csv_output", category="output",
                         help="write plot as csv")
    optParser.add_option("--filter-ids", dest="filterIDs", help="only plot data points from the given list of ids")
    optParser.add_option("-p", "--pick-distance", dest="pickDist", type=float, default=1,
                         help="pick lines within the given distance in interactive plot mode")
    optParser.add_option("--label", help="plot label (default input file name")
    optParser.add_option("-j", "--join-files", action="store_true", dest="joinFiles", default=False,
                         help="Do not distinguis data points by file")
    optParser.add_option("--join-x", action="store_true", dest="joinx", default=False,
                         help="if --xattr is a list concatenate the values")
    optParser.add_option("--join-y", action="store_true", dest="joiny", default=False,
                         help="if --yattr is a list concatenate the values")
    optParser.add_option("--xfactor", help="multiplier for x-data", type=float, default=1)
    optParser.add_option("--yfactor", help="multiplier for y-data", type=float, default=1)
    optParser.add_option("--xbin", help="binning size for x-data", type=float)
    optParser.add_option("--ybin", help="binning size for y-data", type=float)
    optParser.add_option("--xclamp", default=None,
                         help="clamp x values to range A:B or half-range A: / :B")
    optParser.add_option("--yclamp", default=None,
                         help="clamp y values to range A:B or half-range A: / :B")
    optParser.add_option("--invert-yaxis", dest="invertYAxis", action="store_true",
                         default=False, help="Invert the Y-Axis")
    optParser.add_option("--scatterplot", action="store_true", category="visualization",
                         default=False, help="Draw a scatterplot instead of lines")
    optParser.add_option("--barplot", action="store_true", category="visualization",
                         default=False, help="Draw a bar plot parallel to the y-axis")
    optParser.add_option("--hbarplot", action="store_true", category="visualization",
                         default=False, help="Draw a bar plot parallel to the x-axis")
    optParser.add_option("--legend", action="store_true", default=False, category="visualization", help="Add legend")
    optParser.add_option("-v", "--verbose", action="store_true", default=False, help="tell me what you are doing")
    sumolib.visualization.helpers.addPlotOptions(optParser)
    sumolib.visualization.helpers.addInteractionOptions(optParser)

    options = optParser.parse_args(args=args)

    options.attrOptions = ['idattr', 'xattr', 'yattr']
    options.attrElems = [options.idelem, options.xelem, options.yelem]

    if options.filterIDs is not None:
        options.filterIDs = set(options.filterIDs.split(','))

    for a in options.attrOptions:
        if getattr(options, a) is None:
            sys.exit("mandatory argument --%s is missing" % a)

    if options.xlabel is None:
        if options.xattr == BOX_ATTR:
            if options.idattr:
                options.xlabel = options.idattr
            else:
                options.xlabel = "file"
        else:
            options.xlabel = options.xattr
        if options.xclamp is not None:
            options.xlabel += " clamp(%s)" % options.xclamp

    if options.ylabel is None:
        if options.yattr == BOX_ATTR:
            if options.idattr:
                options.ylabel = options.idattr
            else:
                options.ylabel = "file"
        else:
            options.ylabel = options.yattr
        if options.yclamp is not None:
            options.ylabel += " clamp(%s)" % options.yclamp

    # keep old presets from before integration of common options
    options.nolegend = not options.legend
    if options.show:
        sys.stderr.write("Option --show is now set by default and will be removed in the future." +
                         "Use --blind to disable the plot window\n")

    if options.xattr == BOX_ATTR and options.yattr == BOX_ATTR:
        sys.exit("Boxplot can only be specified for one dimension")
    options.boxplot = options.xattr == BOX_ATTR or options.yattr == BOX_ATTR

    if options.barplot and options.hbarplot:
        sys.exit("Barplot can only be specified for one axis")

    options.barbin = 0
    if options.barplot:
        if options.xbin is None:
            options.xbin = 1.0
            if options.verbose:
                print("Binning set to %s for barplot. Use option --xbin to set a custom value." % options.xbin)
        options.barbin = options.xbin

    if options.hbarplot:
        if options.ybin is None:
            options.ybin = 1.0
            if options.verbose:
                print(("Binning set to %s for horizontal barplot." +
                       " Use option --ybin to set a custom value.") % options.xbin)
        options.barbin = options.ybin

    options.xclampRange = interpretClamp(options.xclamp)
    options.yclampRange = interpretClamp(options.yclamp)

    return options


def interpretClamp(clamp):
    if clamp is None:
        return None
    clamp = clamp.split(":")
    if len(clamp) != 2:
        sys.exit("Clamp option requires a single ':' value")
    cmin = float(clamp[0]) if clamp[0] else uMin
    cmax = float(clamp[1]) if clamp[1] else uMax
    return cmin, cmax


def write_csv(data, fname):
    with open(fname, 'w') as f:
        for dataID in sorted(data.keys(), key=str):
            vals = data[dataID]
            f.write('# "%s"\n' % dataID)
            for x in zip(*vals):
                f.write(" ".join(map(str, x)) + "\n")
            #  2 blank lines indicate a new data block in gnuplot
            f.write('\n\n')


def short_names(filenames, noEmpty):
    if len(filenames) == 1:
        return filenames
    reversedNames = [''.join(reversed(f)) for f in filenames]
    prefix = os.path.commonprefix(filenames)
    suffix = os.path.commonprefix(reversedNames)
    prefixLen = len(prefix)
    suffixLen = len(suffix)
    shortened = [f[prefixLen:-suffixLen] for f in filenames]
    if noEmpty and any([not f for f in shortened]):
        # make longer to avoid empty file names
        base = os.path.basename(prefix)
        shortened = [base + f for f in shortened]
    return shortened


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
    foundAll = False
    dataAttrs = [a for a in allAttrs if a not in NON_DATA_ATTRS]
    for fname in options.files:
        with openz(fname) as xmlf:
            for event, elem in ET.iterparse(xmlf, ("start", "end")):
                if event == "start":
                    level += 1
                    for a, e in zip(attrOptions, options.attrElems):
                        attrOrig = getattr(options, a)
                        for attr in attr2parts[attrOrig]:
                            if attr in elem.keys():
                                if e is not None and e != elem.tag:
                                    # print("skipping attribute '%s' in element '%s' (required elem '%s'" %
                                    #       (attr, elem.tag, e))
                                    continue
                                elem2level[elem.tag] = level
                                if attr in attr2elem:
                                    oldTag = attr2elem[attr]
                                    if oldTag != elem.tag:
                                        if elem2level[oldTag] < level:
                                            attr2elem[attr] = elem.tag
                                        print("Warning: found %s '%s' in element '%s' (level %s) and "
                                              "element '%s' (level %s). Using '%s'." %
                                              (a, attr, oldTag, elem2level[oldTag], elem.tag, level, attr2elem[attr]),
                                              file=sys.stderr)
                                else:
                                    attr2elem[attr] = elem.tag
                    if len(attr2elem) == len(dataAttrs):
                        # all attributes have been seen
                        foundAll = True
                        break
                elif event == "end":
                    level -= 1
        if foundAll:
            break

    if not elem2level:
        print("Error: No elements found in input files.", file=sys.stderr)
        sys.exit()

    if len(attr2elem) != len(allAttrs):
        for a in attrOptions:
            attrOrig = getattr(options, a)
            for attr in attr2parts[attrOrig]:
                if attr not in attr2elem:
                    lvlElem = [(lv, el) for el, lv in elem2level.items()]
                    minLevelElem = sorted(lvlElem)[-1][1]
                    if attr in SYMBOLIC_ATTRS:
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

    allElems = sorted(list(set(attr2elem.values())))
    attrs = [getattr(options, a) for a in attrOptions]

    # we don't know the order of the elements and we cannot get it from our xml parser
    skippedLines = defaultdict(int)
    if len(allElems) == 2:
        def datastream(xmlfile):
            missingParents = 0
            elems = sorted(allElems, key=lambda e: elem2level[e])
            mE0 = "<%s " % elems[0]
            mE1 = "<%s " % elems[1]
            attrs0 = [a for a in allAttrs if attr2elem[a] == elems[0]]
            attrs1 = [a for a in allAttrs if attr2elem[a] == elems[1]]
            mAs0 = [(a, re.compile('%s="([^"]*)"' % a)) for a in attrs0]
            mAs1 = [(a, re.compile('%s="([^"]*)"' % a)) for a in attrs1]

            values = {}  # attr -> value
            index = 0
            foundParent = False
            with openz(xmlfile) as xmlf:
                for line in xmlf:
                    if mE0 in line:
                        foundParent = not parseValues(index, line, mAs0, values, skippedLines)
                    if mE1 in line:
                        if not foundParent:
                            print("Warning: Skipped element '%s' without parent element '%s'" % (elems[1], elems[0]),
                                  file=sys.stderr)
                            missingParents += 1
                            continue
                        skip = parseValues(index, line, mAs1, values, skippedLines)
                        if not skip:
                            for toYield in combineValues(attrs, attr2parts, values, splitX, splitY):
                                yield toYield
                                index += 1

            for attr, count in skippedLines.items():
                print("Warning: Skipped %s lines because of missing attributes '%s'." % (
                    count, attr), file=sys.stderr)
            if missingParents:
                print("Use options --xelem, --yelem, --idelem to resolve ambiguous elements")

        return datastream

    elif len(allElems) == 1:
        def datastream(xmlfile):
            missingParents = 0
            mE = "<%s " % allElems[0]
            mAs = [re.compile('%s="([^"]*)"' % a) for a in allAttrs]
            index = 0
            with openz(xmlfile) as xmlf:
                for line in xmlf:
                    if mE in line:
                        values = {}  # attr -> value
                        skip = parseValues(index, line, zip(allAttrs, mAs), values, skippedLines)
                        if not skip:
                            for toYield in combineValues(attrs, attr2parts, values, splitX, splitY):
                                yield toYield
                                index += 1

            for attr, count in skippedLines.items():
                print("Warning: Skipped %s lines because of missing attributes '%s'." % (
                    count, attr), file=sys.stderr)
            if missingParents:
                print("Use options --xelem, --yelem, --idelem to resolve ambiguous elements")

        return datastream

    else:
        sys.exit("Found attributes at elements %s but at most 2 elements are supported" % allElems)


def parseValues(index, line, attributePatterns, values, skippedLines):
    skip = False
    for a, r in attributePatterns:
        m = r.search(line)
        if m:
            values[a] = m.groups()[0]
        elif a == INDEX_ATTR:
            values[a] = index
        elif a in POST_PROCESSING_ATTRS:
            # set in post-processing
            values[a] = 0
        elif a == NONE_ATTR:
            values[a] = NONE_ATTR_DEFAULT
        else:
            skip = True
            skippedLines[a] += 1
    return skip


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
        assert len(toYield) == 3
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


def binned(value, binsize):
    if binsize is not None:
        return int(value / binsize) * binsize
    else:
        return value


def clamped(value, clamp):
    if clamp is not None:
        return max(clamp[0], min(clamp[1], value))
    else:
        return value


def countPoints(xvalues, normalize=False):
    counts = defaultdict(lambda: 0)
    for x in xvalues:
        counts[x] += 1
    xres = sorted(counts.keys())
    yres = [counts[x] for x in xres]
    if normalize:
        total = float(sum(yres))
        if total > 0:
            yres = [c / total for c in yres]
    return xres, yres


def makeNumeric(val):
    if isnumeric(val):
        return val
    try:
        return int(val)
    except ValueError:
        try:
            return float(val)
        except ValueError:
            return val


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

    shortFileNames = short_names(options.files, False)
    titleFileNames = short_names(options.files, True)
    plt.xlabel(options.xlabel)
    plt.ylabel(options.ylabel)
    plt.title(','.join(titleFileNames) if options.label is None else options.label)
    xdata = 0
    ydata = 1

    data = defaultdict(lambda: list(([] for i in range(2))))

    numericXCount = 0
    stringXCount = 0
    numericYCount = 0
    stringYCount = 0

    usableIDs = 0
    idFromSplitAttrs = ',' in options.xattr or ',' in options.yattr

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
            if options.idattr == NONE_ATTR and not idFromSplitAttrs:
                dataID = titleFileNames[0 if options.joinFiles else fileIndex]
            elif len(options.files) > 1 and not options.joinFiles:
                suffix = shortFileNames[fileIndex]
                if len(suffix) > 0:
                    dataID = str(dataID) + "#" + suffix
            x = interpretValue(x)
            y = interpretValue(y)
            if isnumeric(x):
                numericXCount += 1
                x *= options.xfactor
                x = clamped(x, options.xclampRange)
                x = binned(x, options.xbin)
            else:
                stringXCount += 1
            if isnumeric(y):
                numericYCount += 1
                y *= options.yfactor
                y = clamped(y, options.yclampRange)
                y = binned(y, options.ybin)
            else:
                stringYCount += 1

            data[dataID][xdata].append(x)
            data[dataID][ydata].append(y)
            filteredIDs += 1
        if totalIDs == 0 or filteredIDs == 0 or options.verbose:
            print("Found %s datapoints in %s and kept %s" % (totalIDs, datafile, filteredIDs))
        usableIDs += filteredIDs

    if usableIDs == 0:
        if len(options.files) > 1:
            print("Found no usable datapoints%s in %s files" % len(options.files))
        sys.exit()

    minY = uMax
    maxY = uMin
    minX = uMax
    maxX = uMin

    barOffset = 0
    barWidth = options.barbin / (len(data.items()) + 1)

    for dataID, d in data.items():

        if numericXCount > 0 and stringXCount > 0:
            keepNumeric(d, xdata)
        if numericYCount > 0 and stringYCount > 0:
            keepNumeric(d, ydata)

        if options.xattr == RANK_ATTR:
            d[ydata].sort(reverse=True)
            d[xdata] = list(range(len(d[xdata])))

        if options.yattr == RANK_ATTR:
            d[xdata].sort(reverse=True)
            d[ydata] = list(range(len(d[ydata])))

        if options.xattr == COUNT_ATTR:
            d[ydata], d[xdata] = countPoints(d[ydata])

        if options.yattr == COUNT_ATTR:
            d[xdata], d[ydata] = countPoints(d[xdata])

        if options.xattr == DENS_ATTR:
            d[ydata], d[xdata] = countPoints(d[ydata], True)

        if options.yattr == DENS_ATTR:
            d[xdata], d[ydata] = countPoints(d[xdata], True)

        if options.xticksFile:
            applyTicks(d, xdata, options.xticksFile)
        if options.yticksFile:
            applyTicks(d, ydata, options.yticksFile)

        xvalues = d[xdata]
        yvalues = d[ydata]

        if len(xvalues) == 0:
            assert len(yvalues) == 0
            continue

        minY = min(minY, min(yvalues))
        maxY = max(maxY, max(yvalues))
        minX = min(minX, min(xvalues))
        maxX = max(maxX, max(xvalues))

        if not options.boxplot:

            if options.barplot or options.hbarplot:
                if options.barplot:
                    if numericXCount > 0:
                        center = [x + barOffset * barWidth for x in xvalues]
                    else:
                        center = [x + barOffset * barWidth for x in range(len(xvalues))]
                        plt.xticks(range(len(xvalues)), xvalues)
                    plt.bar(center, yvalues, width=barWidth, label=dataID)
                else:
                    if numericYCount > 0:
                        center = [y + barOffset * barWidth for y in yvalues]
                    else:
                        center = [y + barOffset * barWidth for y in range(len(yvalues))]
                        plt.yticks(range(len(yvalues)), yvalues)
                    plt.barh(center, xvalues, height=barWidth, label=dataID)
                barOffset += 1

            else:
                linestyle = options.linestyle
                marker = options.marker
                if options.scatterplot or (min(yvalues) == max(yvalues) and min(xvalues) == max(xvalues)):
                    linestyle = ''
                    if marker is None:
                        marker = 'o'
                plt.plot(xvalues, yvalues, linestyle=linestyle, marker=marker, picker=True, label=dataID)

    if options.boxplot:
        labels = sorted(data.keys(), key=makeNumeric)
        vertical = options.xattr == BOX_ATTR
        xyIndex = ydata if vertical else xdata
        boxdata = [data[dataID][xyIndex] for dataID in labels]
        if vertical:
            plt.xticks(range(len(labels)), labels)
        else:
            plt.yticks(range(len(labels)), labels)
        plt.boxplot(boxdata, vert=options.xattr == BOX_ATTR)

    if options.invertYAxis:
        plt.axis([minX, maxX, maxY, minY])

    if options.csv_output is not None:
        write_csv(data, options.csv_output)

    ax = fig.axes[0]
    sumolib.visualization.helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
    main(getOptions())
