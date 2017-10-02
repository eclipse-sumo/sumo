#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    status.py
# @author  Michael Behrisch
# @author  Laura Bieker
# @date    2007-03-13
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import sys
import smtplib
import re
from os.path import basename, commonprefix
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
    print(build, end=' ', file=out)
    print(datetime.now().ctime(), file=out)
    print("--", file=out)
    print(basename(makeLog), file=out)
    warnings = 0
    errors = 0
    svnLocked = False
    for l in open(makeLog):
        if ("svn: Working copy" in l and "locked" in l) or "svn: Failed" in l:
            svnLocked = True
            failed += l
        warnings, errors, failed = findErrors(l, warnings, errors, failed)
    if svnLocked:
        failed += "svn up failed\n\n"
    print(warnings, "warnings", file=out)
    if errors:
        print(errors, "errors", file=out)
        failed += "make failed\n\n"
    print("--\nbatchreport\n--", file=out)
    print(basename(makeAllLog), file=out)
    warnings = 0
    errors = 0
    for l in open(makeAllLog):
        warnings, errors, failed = findErrors(l, warnings, errors, failed)
    print(warnings, "warnings", file=out)
    if errors:
        print(errors, "errors", file=out)
        failed += "make debug failed\n\n"
    print("--", file=out)
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
            print("Could not send mail.")

if __name__ == "__main__":
    printStatus(sys.argv[1], sys.argv[2], sys.argv[3], sys.stdout, sys.argv[4])
