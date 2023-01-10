# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    statistics.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2012-05-08

from __future__ import absolute_import
from __future__ import print_function
from __future__ import division
import math
import warnings
from collections import defaultdict
try:
    from numpy import sqrt, set_printoptions
except ImportError:
    from math import sqrt


def round(value):  # to round in Python 3 like in Python 2
    if value < 0:
        return math.ceil(value - 0.5)
    else:
        return math.floor(value + 0.5)


class _ExtremeType(object):
    """
    see http://www.python.org/dev/peps/pep-0326/
    """

    def __init__(self, isMax, rep):
        object.__init__(self)
        self._isMax = isMax
        self._rep = rep

    def __eq__(self, other):
        return isinstance(other, self.__class__) and other._isMax == self._isMax

    def __ne__(self, other):
        return not self == other

    def __gt__(self, other):
        return self._isMax and not self == other

    def __ge__(self, other):
        return self._isMax

    def __lt__(self, other):
        return not self._isMax and not self == other

    def __le__(self, other):
        return not self._isMax

    def __repr__(self):
        return self._rep


uMax = _ExtremeType(True, "uMax")
uMin = _ExtremeType(False, "uMin")


def setPrecision(formatstr, precision, isArray=False):
    if isArray:
        set_printoptions(precision=2)
        return formatstr.replace('%.2f', '%s')
    return formatstr.replace('%.2f', '%.' + str(int(precision)) + 'f')


class Statistics:

    def __init__(self, label=None, abs=False, histogram=False, printMin=True, scale=1, printDev=False):
        self.label = label
        self.min = uMax
        self.min_label = None
        self.max = uMin
        self.max_label = None
        self.values = []
        self.abs = abs
        self.printMin = printMin
        self.scale = scale
        self.printDev = printDev
        self.isArray = False
        if histogram:
            self.counts = defaultdict(int)
        else:
            self.counts = None

    def add(self, v, label=None):
        self.values.append(v)
        isMin = v < self.min
        if type(isMin) is not bool:
            self.isArray = True
            v = tuple(v)
        if v < self.min:
            self.min = v
            self.min_label = label
        if v > self.max:
            self.max = v
            self.max_label = label
        if self.counts is not None:
            self.counts[int(round(v / self.scale))] += 1

    def update(self, other):
        for v in other.values:
            self.add(v)

    def clear(self):
        self.min = uMax
        self.min_label = None
        self.max = uMin
        self.max_label = None
        self.values = []
        if self.counts:
            self.counts.clear()

    def count(self):
        return len(self.values)

    def avg(self):
        """return the mean value"""
        # XXX rename this method
        if len(self.values) > 0:
            return sum(self.values) / float(len(self.values))
        else:
            return None

    def avg_abs(self):
        """return the mean of absolute values"""
        # XXX rename this method
        if len(self.values) > 0:
            return sum(map(abs, self.values)) / float(len(self.values))
        else:
            return None

    def meanAndStdDev(self, limit=None):
        """return the mean and the standard deviation optionally limited to the last limit values"""
        if limit is None or len(self.values) < limit:
            limit = len(self.values)
        if limit > 0:
            mean = sum(self.values[-limit:]) / float(limit)
            sumSq = 0.
            for v in self.values[-limit:]:
                sumSq += (v - mean) * (v - mean)
            return mean, sqrt(sumSq / limit)
        else:
            return None

    def relStdDev(self, limit=None):
        """return the relative standard deviation optionally limited to the last limit values"""
        moments = self.meanAndStdDev(limit)
        if moments is None:
            return None
        return moments[1] / moments[0]

    def mean(self):
        warnings.warn("mean() is deprecated, because the name is misleading, use median() instead")
        return self.median()

    def mean_abs(self):
        warnings.warn("mean_abs() is deprecated, because the name is misleading, use median_abs() instead")
        return self.median_abs()

    def average_absolute_deviation_from_mean(self):
        if len(self.values) > 0:
            m = self.avg()
            return sum([abs(v - m) for v in self.values]) / len(self.values)
        else:
            return None

    def median(self):
        """return the median value"""
        if len(self.values) > 0:
            return sorted(self.values)[len(self.values) // 2]
        else:
            return None

    def median_abs(self):
        """return the median of absolute values"""
        if len(self.values) > 0:
            return sorted(map(abs, self.values))[len(self.values) // 2]
        else:
            return None

    def quartiles(self):
        s = sorted(self.values, key=lambda v: tuple(v) if self.isArray else v)
        return s[len(self.values) // 4], s[len(self.values) // 2], s[3 * len(self.values) // 4]

    def rank(self, fraction):
        if len(self.values) > 0:
            return sorted(self.values)[int(round(len(self.values) * fraction + 0.5))]
        else:
            return None

    def histogram(self):
        if self.counts is not None:
            return [(k * self.scale, self.counts[k]) for k in sorted(self.counts.keys())]
        else:
            return "Histogramm is deactivated"

    def toString(self, precision=2, histStyle=1):
        """histStyle
            0 : not shown
            1 : one line
            2 : fancy
            """
        if len(self.values) > 0:
            min = ''
            if self.printMin:
                min = setPrecision('min %.2f%s, ', precision, self.isArray) % (
                    self.min, ('' if self.min_label is None else ' (%s)' % (self.min_label,)))
            result = setPrecision('%s: count %s, %smax %.2f%s, mean %.2f', precision, self.isArray) % (
                self.label, len(self.values), min,
                self.max,
                ('' if self.max_label is None else ' (%s)' %
                 (self.max_label,)),
                self.avg())
            result += setPrecision(', Q1 %.2f, median %.2f, Q3 %.2f', precision, self.isArray) % self.quartiles()
            if self.abs:
                result += setPrecision(', mean_abs %.2f, median_abs %.2f', precision, self.isArray) % (
                    self.avg_abs(), self.median_abs())
            if self.printDev:
                result += (setPrecision(', stdDev %.2f', precision, self.isArray) % (self.meanAndStdDev()[1]))
            if self.counts is not None:
                if histStyle == 1:
                    result += '\n histogram: %s' % self.histogram()
                elif histStyle == 2:
                    keylen = len("%.0f" % (self.scale * max(self.counts.keys())))
                    formatStr = "%%%i.0f: %%s" % keylen
                    result = 'histogram of %s:\n%s\n%s' % (self.label,
                                                           '\n'.join([formatStr % x for x in self.histogram()]),
                                                           result)
            return result
        else:
            return '%s: no values' % self.label

    def toXML(self, precision=2, tag="statistic", indent=4, label=None):
        if label is None:
            label = self.label
        description = ' description="%s"' % label if label != '' else ''

        result = ' ' * indent + '<%s%s' % (tag, description)
        if self.count() > 0:
            result += ' count="%i"' % self.count()
            result += (setPrecision(' min="%.2f" minLabel="%s" max="%.2f" maxLabel="%s" mean="%.2f"',
                                    precision, self.isArray) %
                       (self.min, self.min_label, self.max, self.max_label, self.avg()))
            result += setPrecision(' Q1="%.2f" median="%.2f" Q3="%.2f"', precision, self.isArray) % self.quartiles()
            result += (setPrecision(' meanAbs="%.2f" medianAbs="%.2f"', precision, self.isArray) %
                       (self.avg_abs(), self.median_abs()))
            if self.printDev:
                result += (setPrecision(' stdDev="%.2f"', precision, self.isArray) %
                           (self.meanAndStdDev()[1]))
        if self.counts is not None:
            result += '>\n'
            for kv in self.histogram():
                result += setPrecision(8 * ' ' + '<hist key="%.2f" value="%i"/>\n', precision, self.isArray) % kv
            result += ' ' * indent + '</%s>\n' % tag
        else:
            result += '/>\n'
        return result

    def __str__(self):
        return self.toString()

    def normalise_to_range(self, n_min=0, n_max=1):
        """Normalises the stored list of values between n_min and n_max, Default: [0,1]"""
        ret = []
        range_length = n_max - n_min
        values_diff = max(self.values) - min(self.values)
        for val in self.values:
            temp = (((val - min(self.values))*range_length)/values_diff) + n_min
            ret.append(temp)
        return ret


def geh(m, c):
    """Error function for hourly traffic flow measures after Geoffrey E. Havers"""
    if m + c == 0:
        return 0
    else:
        return math.sqrt(2 * (m - c) * (m - c) / (m + c))


def sqv(self, m, c, scaling_factor=1000):
    """Scaling Quality Value Calculation, Ref: https://journals.sagepub.com/doi/10.1177/0361198119838849
        scaling_factor:
        Number of person trips per day (total, per mode, per purpose) : 1
        Mean trip distance in kilometers : 10
        Duration of all trips per person per day in minutes : 100
        Traffic volume per hour : 1000
        Traffic volume per day : 10000
    """
    return 1/(1 + math.sqrt(((m-c)*(m-c))/(scaling_factor*c)))
