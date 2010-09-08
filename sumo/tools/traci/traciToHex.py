#!/usr/bin/env python
# Converts all testclient.prog to hex
# $Id$

import os, sys

mRoot = "."
if len(sys.argv)>1:
    mRoot = sys.argv[1]
for root, dirs, files in os.walk(mRoot):
    if ".svn" in dirs:
        dirs.remove(".svn")
    for file in files:
        if file == "testclient.prog":
            full = os.path.join(root, file)
            out = open(full + ".hex", 'w')
            change = False
            for line in open(full):
                l = line.split()
                if l and l[0] in ["setvalue", "getvalue", "getvariable", "getvariable_plus"]:
                    if not l[1][:2] == "0x":
                        l[1] = "0x%x" % int(l[1])
                        change = True
                    if not l[2][:2] == "0x":
                        l[2] = "0x%x" % int(l[2])
                        change = True
                print >> out, " ".join(l)
            out.close()
            if change:
                if os.name != "posix":
                    os.remove(full)
                os.rename(out.name, full)
            else:
                os.remove(out.name)
