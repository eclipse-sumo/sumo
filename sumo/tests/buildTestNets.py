#!/usr/bin/env python
# Builds all example networks
# $Id$

import os, sys

mRoot = "."
if len(sys.argv)>1:
    mRoot = sys.argv[1]
binPrefix = os.path.join(os.path.dirname(sys.argv[0]), '..', 'bin', 'net')
for root, dirs, files in os.walk(mRoot):
    if ".svn" in dirs:
        dirs.remove(".svn")
    for file in files:
        if file.endswith(".netc.cfg") or file.endswith(".netg.cfg"):
            print "----------------------------------"
            print "Running: " + file
            if file.endswith(".netc.cfg"):
                (cin, cout) = os.popen4(binPrefix+"convert -v -c " + os.path.join(root, file))
            else:
                (cin, cout) = os.popen4(binPrefix+"gen -v -c " + os.path.join(root, file))
            line = cout.readline()
            while line:
                print line[:-1]
                line = cout.readline()
            print "----------------------------------\n"
