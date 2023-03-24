#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    status.py
# @author  Michael Behrisch
# @author  Laura Bieker
# @date    2007-03-13

from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import subprocess
import smtplib
import re
import io
import time
from os.path import basename, commonprefix
from datetime import datetime
import logging
import logging.handlers


def killall(debugSuffix, binaries):
    bins = set([name + suff + ".exe" for name in binaries for suff in debugSuffix])
    printLog("Looking for running instances of %s." % bins)
    for i in range(2):  # killing twice is better than once ;-)
        clean = True
        for taskline in subprocess.check_output(["tasklist", "/nh"], universal_newlines=True).splitlines():
            task = taskline.split()
            if task and task[0] in bins:
                printLog("Found %s." % task)
                log_subprocess(["taskkill", "/f", "/im", task[0]])
                printLog("Sent kill to all %s (try %s)." % (task[0], i))
                bins.remove(task[0])
                clean = False
        if clean:
            return
        time.sleep(10)


def set_rotating_log(filename, remove=None):
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    try:
        if remove:
            logger.removeHandler(remove)
        handler = logging.handlers.TimedRotatingFileHandler(filename, when="midnight", backupCount=5)
        logger.addHandler(handler)
        return handler
    except Exception as e:
        logger.error(e)
        return None


def log_subprocess(call, env=None, cwd=None):
    process = subprocess.Popen(call, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                               shell=True, cwd=cwd, env=env)
    with process.stdout:
        for line in process.stdout:
            logging.info(line.rstrip().decode("ascii", "ignore"))
    return process.wait()


def printLog(msg):
    logging.info(u"%s: %s" % (datetime.now(), msg))


def findErrors(line, warnings, errors, failed):
    if re.search("[Ww]arn[ui]ng[: ]", line) or "[WARNING]" in line:
        if " test-case " not in line:
            warnings += 1
    if re.search("[Ee]rror[: ]", line) or re.search("[Ff]ehler:", line) or "[ERROR]" in line:
        if " test-case " not in line:
            errors += 1
            failed += line
    return warnings, errors, failed


def printStatus(makeLog, makeAllLog, smtpServer="localhost", out=sys.stdout, toAddr="sumo-tests@dlr.de", testLog=None):
    failed = ""
    build = commonprefix([basename(makeLog), basename(makeAllLog)])
    print(build, end=' ', file=out)
    print(datetime.fromtimestamp(os.stat(makeLog).st_ctime).ctime(), file=out)
    print("--", file=out)
    print(basename(makeLog), file=out)
    warnings = 0
    errors = 0
    svnLocked = False
    for ml in io.open(makeLog, errors="replace"):
        if ("svn: Working copy" in ml and "locked" in ml) or "svn: Failed" in ml:
            svnLocked = True
            failed += ml
        warnings, errors, failed = findErrors(ml, warnings, errors, failed)
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
    for ml in io.open(makeAllLog, errors="replace"):
        warnings, errors, failed = findErrors(ml, warnings, errors, failed)
    print(warnings, "warnings", file=out)
    if errors:
        print(errors, "errors", file=out)
        failed += "make debug failed\n\n"
    print("--", file=out)
    if testLog:
        print(basename(testLog), file=out)
        print(datetime.now().ctime(), file=out)
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
        except Exception:
            print("Could not send mail.")


if __name__ == "__main__":
    printStatus(sys.argv[1], sys.argv[2], sys.argv[3], sys.stdout, sys.argv[4],
                sys.argv[5] if len(sys.argv) > 5 else None)
