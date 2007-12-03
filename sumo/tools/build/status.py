#!/usr/bin/env python

import os, sys, smtplib
from os.path import basename, join, expandvars, exists, commonprefix
from datetime import datetime

failed = ""
makelog = sys.argv[1]
makealllog = sys.argv[2]
print commonprefix([basename(makelog), basename(makealllog)]),
print datetime.now().ctime()
print "--"
print basename(makelog)
warnings = 0
errors = 0
for l in file(makelog):
    if l.find("warning ") > -1:
        warnings = warnings + 1
    if l.find("error ") > -1:
        errors = errors + 1
        failed += l
print warnings, "warnings"
if errors:
    print errors, "errors"
    failed += "make failed\n\n"
print "--"
if len(sys.argv) > 3:
    texttesttmp = sys.argv[3]
    for root, dirs, files in os.walk(texttesttmp):
        for f in files:
            if f.startswith("batchreport"):
                b = open(join(root, f))
                l = b.readline()
                if l.startswith("FAILED") or l.startswith("succeeded"):
                	print f, l,
                b.close()
print "--"
print basename(makealllog)
warnings = 0
errors = 0
for l in file(makealllog):
    if l.find("warning ") > -1:
        warnings = warnings + 1
    if l.find("error ") > -1:
        errors = errors + 1
        failed += l
print warnings, "warnings"
if errors:
    print errors, "errors"
    failed += "make debug failed\n\n"
print "--"

if failed and len(sys.argv) > 4:
    fromAddr = "michael.behrisch@dlr.de"
    toAddr = "delphi-dev@dlr.de"
    message = """\
From: %s
To: %s
Subject: Error occured while building SUMO.

%s
""" % (fromAddr, toAddr, failed)
    server = smtplib.SMTP(sys.argv[4])        
    server.sendmail(fromAddr, toAddr, message)
    server.quit()
