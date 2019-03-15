# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    miscutils.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2012-05-08
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function
import sys
import time
import os
import math
import colorsys
import socket
import random
from collections import defaultdict

# append import path stanca:
# THIS_DIR == os.path.basename(__file__)
# sys.path.append(os.path.join(THIS_DIR, 'foo', 'bar'))

# http://www.python.org/dev/peps/pep-0326/


def round(value):  # to round in Python 3 like in Python 2
    if value < 0:
        return math.ceil(value - 0.5)
    else:
        return math.floor(value + 0.5)


class _ExtremeType(object):

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

# decorator for timing a function


def benchmark(func):
    def benchmark_wrapper(*args, **kwargs):
        started = time.time()
        now = time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.localtime())
        print('function %s called at %s' % (func.__name__, now))
        sys.stdout.flush()
        result = func(*args, **kwargs)
        print('function %s finished after %f seconds' %
              (func.__name__, time.time() - started))
        sys.stdout.flush()
        return result
    return benchmark_wrapper


class Statistics:

    def __init__(self, label=None, abs=False, histogram=False, printMin=True, scale=1):
        self.label = label
        self.min = uMax
        self.min_label = None
        self.max = uMin
        self.max_label = None
        self.values = []
        self.abs = abs
        self.printMin = printMin
        self.scale = scale
        if histogram:
            self.counts = defaultdict(int)
        else:
            self.counts = None

    def add(self, v, label=None):
        self.values.append(v)
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
            return mean, math.sqrt(sumSq / limit)
        else:
            return None

    def relStdDev(self, limit=None):
        """return the relative standard deviation optionally limited to the last limit values"""
        moments = self.meanAndStdDev(limit)
        if moments is None:
            return None
        return moments[1] / moments[0]

    def mean(self):
        """return the median value"""
        # XXX rename this method
        if len(self.values) > 0:
            return sorted(self.values)[len(self.values) / 2]
        else:
            return None

    def mean_abs(self):
        """return the median of absolute values"""
        # XXX rename this method
        if len(self.values) > 0:
            return sorted(map(abs, self.values))[len(self.values) / 2]
        else:
            return None

    def average_absolute_deviation_from_mean(self):
        if len(self.values) > 0:
            m = self.avg()
            return sum([abs(v - m) for v in self.values]) / len(self.values)
        else:
            return None

    def median(self):
        return self.mean()

    def median_abs(self):
        return self.mean_abs()

    def quartiles(self):
        s = sorted(self.values)
        return s[len(self.values) // 4], s[len(self.values) // 2], s[3 * len(self.values) // 4]

    def rank(self, fraction):
        if len(self.values) > 0:
            return sorted(self.values)[int(round(len(self.values) * fraction + 0.5))]
        else:
            return None

    def histogram(self):
        return [(k * self.scale, self.counts[k]) for k in sorted(self.counts.keys())]

    def __str__(self):
        if len(self.values) > 0:
            min = ''
            if self.printMin:
                min = 'min %.2f%s, ' % (self.min,
                                        ('' if self.min_label is None else ' (%s)' % (self.min_label,)))
            result = '%s: count %s, %smax %.2f%s, mean %.2f' % (
                self.label, len(self.values), min,
                self.max,
                ('' if self.max_label is None else ' (%s)' %
                 (self.max_label,)),
                self.avg())
            result += ' Q1 %.2f, median %.2f, Q3 %.2f' % self.quartiles()
            if self.abs:
                result += ', mean_abs %.2f, median_abs %.2f' % (
                    self.avg_abs(), self.mean_abs())
            if self.counts is not None:
                result += '\n histogram: %s' % self.histogram()
            return result
        else:
            return '%s: no values' % self.label


def geh(m, c):
    """Error function for hourly traffic flow measures after Geoffrey E. Havers"""
    if m + c == 0:
        return 0
    else:
        return math.sqrt(2 * (m - c) * (m - c) / (m + c))


# temporarily change working directory using 'with' statement
class working_dir:

    def __init__(self, dir):
        self.dir = dir
        self.origdir = os.getcwd()

    def __enter__(self):
        os.chdir(self.dir)

    def __exit__(self, type, value, traceback):
        os.chdir(self.origdir)


class Colorgen:
    DISTINCT = [
        (0.17, 1.0, 0.5),
        (0.0, 0.9, 1.0),
        (0.35, 0.67, 0.71),
        (0.14, 0.9, 1.0),
        (0.56, 1.0, 0.78),
        (0.07, 0.8, 0.96),
        (0.79, 0.83, 0.71),
        (0.5, 0.71, 0.94),
        (0.84, 0.79, 0.94),
        (0.2, 0.76, 0.96),
        (0.0, 0.24, 0.98),
        (0.5, 1.0, 0.5),
        (0.77, 0.25, 1.0),
        (0.09, 0.76, 0.67),
        (0.15, 0.22, 1.0),
        (0.0, 1.0, 0.5),
        (0.38, 0.33, 1.0),
        (0.67, 1.0, 0.5),
    ]

    def __init__(self, hsv, cycleLength=10.67):
        self.hsv = hsv
        self.cycle = [random.randint(0, 255) for x in self.hsv]
        self.cycleOffset = int(round(256 / cycleLength))
        self.distinctIndex = 0

    def get_value(self, opt, index):
        if opt == 'random':
            return random.random()
        elif opt == 'cycle':
            # the 255 below is intentional to get all color values when cycling long enough
            self.cycle[index] = (self.cycle[index] + self.cycleOffset) % 255
            return self.cycle[index] / 255.0
        elif opt == 'cycle':
            # the 255 below is intentional to get all color values when cycling long enough
            self.cycle[index] = (self.cycle[index] + self.cycleOffset) % 255
            return self.cycle[index] / 255.0
        elif opt == 'distinct':
            if index == 0:
                self.distinctIndex = (self.distinctIndex + 1) % len(self.DISTINCT)
            return self.DISTINCT[self.distinctIndex][index]
        else:
            return float(opt)

    def floatTuple(self):
        """return color as a tuple of floats each in [0,1]"""
        return colorsys.hsv_to_rgb(*[self.get_value(o, i) for i, o in enumerate(self.hsv)])

    def byteTuple(self):
        """return color as a tuple of bytes each in [0,255]"""
        return tuple([int(round(255 * x)) for x in self.floatTuple()])

    def __call__(self):
        """return constant or randomized rgb-color string"""
        return ','.join(map(str, self.byteTuple()))


def getFreeSocketPort(numTries=10):
    for i in range(numTries):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.bind(('', 0))
            p = s.getsockname()[1]
            s.close()
            return p
        except socket.error:
            pass
    return None


def getSocketStream(port, mode='rb'):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("localhost", port))
    s.listen(1)
    conn, addr = s.accept()
    return conn.makefile(mode)


# euclidean distance between two coordinates in the plane
def euclidean(a, b):
    return math.sqrt((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2)


def parseTime(t):
    try:
        return float(t)
    except ValueError:
        pass
    # prepended zero is ignored if the date value already contains days
    days, hours, minutes, seconds = ([0] + list(map(float, t.split(':'))))[-4:]
    return 3600 * 24 * days + 3600 * hours + 60 * minutes + seconds
