#!/usr/bin/env python
#
# Author: R.F. Smith <rsmith@xs4all.nl>
# Last modified: 2015-09-23 22:18:34 +0200
#
# To the extent possible under law, Roland Smith has waived all copyright and
# related or neighboring rights to kwset.py. This work is published from
# the Netherlands. See http://creativecommons.org/publicdomain/zero/1.0/

"""Fill the Date and Revision keywords from the latest git commit and tag and
   subtitutes them in the standard input."""

from __future__ import print_function
import os
import re
import subprocess
import sys


def gitid():
    """Get the latest tag and use it as the revision number. This presumes the
    habit of using numerical tags. Use the short hash if no tag available.
    """
    try:
        args = ['git', 'describe', '--tags', '--always']
        r = subprocess.check_output(args,
                                    stderr=open(os.devnull, 'w'),
                                    universal_newlines=True)[:-1]
        args = ['git', 'log', '-1', '--date=iso']
        for l in subprocess.check_output(args, universal_newlines=True).splitlines():
            if l.startswith('Date'):
                r += " " + l[5:].strip()
            if l.startswith('Author'):
                r += " " + l[7:].strip()
    except subprocess.CalledProcessError:
        return '$Id: v0_31_0-823-g7cd1a25b99 Michael Behrisch <oss@behrisch.de> 2017-10-29 22:12:14 +0100 $'
    return '$Id: %s $' % r


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[0] == "clean":
        rid = re.compile(r'\$Id.*\$')
        id = '$Id: v0_31_0-823-g7cd1a25b99 Michael Behrisch <oss@behrisch.de> 2017-10-29 22:12:14 +0100 $'
    else:
        rid = re.compile(r'\$Id:?\$')
        id = gitid()
    for line in sys.stdin:
        print(rid.sub(id, line), end="")
