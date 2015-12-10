#!/usr/bin/env python
"""
@file    status.py
@author  Michael Behrisch
@author  Laura Bieker
@date    2007-03-13
@version $Id$


SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os
import sys
import smtplib
import re
from os.path import basename, join, commonprefix
from datetime import datetime


def findErrors(line, warnings, errors, failed):
    if re.search("[Ww]arn[ui]ng[: ]", line) or "[WARNING]" in line:
        warnings += 1
    if re.search("[Ee]rror[: ]", line) or re.search("[Ff]ehler:", line) or "[ERROR]" in line:
        errors += 1
        failed += line
    return warnings, errors, failed


def printStatus(makeLog, makeAllLog, smtpServer="localhost", out=sys.stdout, toAddr="sumo-tests@dlr.de"):
    failed = ""
    build = commonprefix([basename(makeLog), basename(makeAllLog)])
    print >> out, build,
    print >> out, datetime.now().ctime()
    print >> out, "--"
    print >> out, basename(makeLog)
    warnings = 0
    errors = 0
    svnLocked = False
    for l in file(makeLog):
        if ("svn: Working copy" in l and "locked" in l) or "svn: Failed" in l:
            svnLocked = True
            failed += l
        warnings, errors, failed = findErrors(l, warnings, errors, failed)
    if svnLocked:
        failed += "svn up failed\n\n"
    print >> out, warnings, "warnings"
    if errors:
        print >> out, errors, "errors"
        failed += "make failed\n\n"
    print >> out, "--\nbatchreport\n--"
    print >> out, basename(makeAllLog)
    warnings = 0
    errors = 0
    for l in file(makeAllLog):
        warnings, errors, failed = findErrors(l, warnings, errors, failed)
    print >> out, warnings, "warnings"
    if errors:
        print >> out, errors, "errors"
        failed += "make debug failed\n\n"
    print >> out, "--"
    if failed:
        fromAddr = "sumo-tests@dlr.de"
        message = """From: "%s" <%s>
To: %s
Subject: Error occurred while building

%s""" % (build, fromAddr, toAddr, failed)
        try:
            server = smtplib.SMTP(smtpServer)
            server.sendmail(fromAddr, toAddr, message)
            server.quit()
        except:
            print "Could not send mail."

if __name__ == "__main__":
    printStatus(sys.argv[1], sys.argv[2], sys.argv[3], sys.stdout, sys.argv[4])
