#!/usr/bin/python

import os, sys
from os.path import join

makelog = sys.argv[1]
makealllog = sys.argv[2]
texttesttmp = sys.argv[3]
warnings = 0
for l in file(makelog):
    if l.find("warning") > -1:
        warnings = warnings + 1
print str(warnings) + " warnings"
for root, dirs, files in os.walk(texttesttmp):
    for f in files:
        if f.startswith("batchreport"):
            b = open(join(root, f))
            print f + " " + b.readline(),
            b.close()
warnings = 0
for l in file(makealllog):
    if l.find("warning") > -1:
        warnings = warnings + 1
print str(warnings) + " warnings"
