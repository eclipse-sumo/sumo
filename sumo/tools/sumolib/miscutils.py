"""
@file    miscutils.py
@author  Jakob.Erdmann@dlr.de
@date    2012-05-08
@version $Id$

Common utility functions

Copyright (C) 2007-2013 DLR/FS, Germany
All rights reserved

This is a duplicate of tools/util/miscutils.py from the VABENE repository
"""
import StringIO
import sys
import subprocess
import time
import os
import imp
import csv
import math
from collections import defaultdict

# append import path stanca:
#THIS_DIR == os.path.basename(__file__)
#sys.path.append(os.path.join(THIS_DIR, 'foo', 'bar'))

# http://www.python.org/dev/peps/pep-0326/
class _ExtremeType(object):

    def __init__(self, cmpr, rep):
        object.__init__(self)
        self._cmpr = cmpr
        self._rep = rep

    def __cmp__(self, other):
        if isinstance(other, self.__class__) and\
           other._cmpr == self._cmpr:
            return 0
        return self._cmpr

    def __repr__(self):
        return self._rep

uMax = _ExtremeType(1, "uMax")
uMin = _ExtremeType(-1, "uMin")

# decorator for timing a function
def benchmark(func):
    def benchmark_wrapper(*args, **kwargs):
        started = time.time()
        now = time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.localtime())
        print('function %s called at %s' % (func.__name__, now))
        sys.stdout.flush()
        result = func(*args, **kwargs)
        print('function %s finished after %f seconds' % (func.__name__, time.time() - started))
        sys.stdout.flush()
        return result
    return benchmark_wrapper


class Statistics:
    def __init__(self, label=None, abs=False, histogram=False, printMin=True):
        self.label = label
        self.min = uMax
        self.min_label = None
        self.max = uMin
        self.max_label = None
        self.values = []
        self.abs = abs
        self.printMin = printMin
        if histogram:
            self.counts = defaultdict(lambda:0)
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
            self.counts[round(v)] += 1

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
            return sorted(map(abs,self.values))[len(self.values) / 2]
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

    def rank(self, fraction):
        if len(self.values) > 0:
            return sorted(self.values)[int(round(len(self.values) * fraction + 0.5))]
        else:
            return None

    def __str__(self):
        if len(self.values) > 0:
            min = 'min %.2f (%s), ' % (self.min, self.min_label) if self.printMin else ''
            result = '"%s": count %s, %smax %.2f (%s), mean %.2f, median %.2f' % (
                    self.label, len(self.values), min,
                    self.max, self.max_label, self.avg(), self.mean())
            if self.abs:
                result += ', mean_abs %.2f, median_abs %.2f' % (self.avg_abs(), self.mean_abs())
            if self.counts is not None:
                result += '\nhistogram: %s' % [(k,self.counts[k]) for k in sorted(self.counts.keys())]
            return result
        else:
            return '"%s": no values' % self.label


def geh(m,c):
    """Error function for hourly traffic flow measures after Geoffrey E. Havers"""
    if m+c == 0:
        return 0
    else:
        return math.sqrt(2 * (m-c) * (m-c) / (m+c))


# temporarily change working directory using 'with' statement
class working_dir:
    def __init__(self, dir):
        self.dir = dir
        self.origdir = os.getcwd()

    def __enter__(self):
        os.chdir(self.dir)

    def __exit__(self, type, value, traceback):
        os.chdir(self.origdir)

