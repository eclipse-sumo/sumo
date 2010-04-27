#!/usr/bin/env python

import os, sys, smtplib
from os.path import basename, join, commonprefix
from datetime import datetime

def printStatus(makeLog, makeAllLog, textTestTmp, smtpServer, out):
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
        if "warning " in l.lower() or "warnung" in l.lower():
            warnings += 1
        if "error " in l.lower():
            errors += 1
            failed += l
    if svnLocked:
        failed += "svn up failed\n\n"
    print >> out, warnings, "warnings"
    if errors:
        print >> out, errors, "errors"
        failed += "make failed\n\n"
    print >> out, "--"
    for root, dirs, files in os.walk(textTestTmp):
        for f in files:
            if f.startswith("batchreport"):
                b = open(join(root, f))
                l = b.readline()
                if l.startswith("FAILED") or l.startswith("succeeded"):
                	print >> out, f, l,
                b.close()
    print >> out, "--"
    print >> out, basename(makeAllLog)
    warnings = 0
    errors = 0
    for l in file(makeAllLog):
        if "warning " in l.lower() or "warnung" in l.lower():
            warnings += 1
        if "error " in l.lower():
            errors += 1
            failed += l
    print >> out, warnings, "warnings"
    if errors:
        print >> out, errors, "errors"
        failed += "make debug failed\n\n"
    print >> out, "--"
    
    if failed:
        fromAddr = "michael.behrisch@dlr.de"
        toAddr = "michael.behrisch@dlr.de,daniel.krajzewicz@dlr.de,laura.bieker@dlr.de"
        message = """From: "%s" <%s>
To: %s
Subject: Error occurred while building

%s""" % (build, fromAddr, toAddr, failed)
        server = smtplib.SMTP(smtpServer)        
        server.sendmail(fromAddr, toAddr, message)
        server.quit()

if __name__ == "__main__":
    printStatus(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.stdout)
