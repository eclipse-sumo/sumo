#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    keywords.py
# @author  Michael Behrisch
# @date    2017-10-30
# @version $Id$
#
# Inspired by https://github.com/rsmith-nl/git-keywords
#  by R.F. Smith <rsmith@xs4all.nl>
# and https://github.com/turon/git-rcs-keywords
#  by Martin Turon

"""Fills the Id keyword from the latest git commit and tag and
   substitutes them in the standard input.

   To activate this filter add the following lines to your ~/.gitconfig:
   [filter "keywords"]
       clean  = .git_filters/keywords.py clean
       smudge = .git_filters/keywords.py %f
"""

from __future__ import print_function
import os
import re
import subprocess
import sys

# need to split this to avoid substitution
UNMANGLED = '$' + 'Id$'


def gitid(fname):
    """Get the latest tag and use it as the revision number. This presumes the
    habit of using numerical tags. Use the short hash if no tag available.
    """
    try:
        r = subprocess.check_output(["git", "describe", "--long", "--always"], universal_newlines=True).strip()
        if "-" in r:
            r = r.replace("-g", "-")
            m1 = r.find("-") + 1
            m2 = r.find("-", m1)
            diff = max(0, 4 - (m2 - m1))
            r = r[:m1].replace("-", "+") + (diff * "0") + r[m1:]
        args = ['git', 'log', '-1', '--date=iso', '--', fname]
        for l in subprocess.check_output(args, universal_newlines=True).splitlines():
            if l.startswith('Date'):
                r += " " + l[5:].strip()
            if l.startswith('Author'):
                if '<' in l and '>' in l:
                    r += " " + l[l.index("<") + 1:l.index(">")]
                else:
                    r += " " + l[7:].strip()
    except subprocess.CalledProcessError:
        return UNMANGLED
    return '$' + 'Id: %s %s $' % (os.path.basename(fname), r)


if __name__ == '__main__':
    if len(sys.argv) <= 1 or sys.argv[1] == "clean":
        rid = re.compile(r'\$' + r'Id.*\$')
        id = UNMANGLED
    else:
        rid = re.compile(r'\$' + r'Id:?\$')
        id = gitid(sys.argv[1])
    for line in sys.stdin:
        print(rid.sub(id, line), end="")
