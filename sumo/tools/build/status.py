#!/usr/bin/python

import os, sys
from os.path import basename, join
from datetime import datetime

makelog = sys.argv[1]
makealllog = sys.argv[2]
print basename(makelog), datetime.now().ctime()
print "--"
print basename(makelog)
warnings = 0
errors = 0
for l in file(makelog):
    if l.find("warning") > -1:
        warnings = warnings + 1
    if l.find("error") > -1:
        errors = errors + 1
print warnings, "warnings"
if errors:
    print errors, "errors"
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
    if l.find("warning") > -1:
        warnings = warnings + 1
    if l.find("error") > -1:
        errors = errors + 1
print warnings, "warnings"
if errors:
    print errors, "errors"
print "--"
