#!/usr/bin/python

import os, sys, smtplib
from os.path import basename, join, expandvars, exists
from datetime import datetime

failed = ""
makelog = sys.argv[1]
makealllog = sys.argv[2]
print basename(makelog), datetime.now().ctime()
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

if failed and len(sys.argv) > 4 and exists(sys.argv[4]):
    config = open(sys.argv[4])
    smtp_server = None
    to_addrs = None        
    for line in config:
        option = line.split(":")
        if option[0] == "smtp_server":
            smtp_server = expandvars(option[1]).strip()
        if option[0] == "batch_recipients":
            to_addrs = [a.strip() for a in expandvars(option[1]).split(",")]
    config.close()
    if smtp_server and to_addrs:
        from_addr = "michael.behrisch@dlr.de"
        message = """\
From: %s
To: %s
Subject: Error occured while building.

%s
""" % (from_addr, ", ".join(to_addrs), failed)
        server = smtplib.SMTP(smtp_server)        
        server.sendmail(from_addr, to_addrs, message)
        server.quit()
