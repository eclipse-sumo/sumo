#!/usr/bin/env python
# Builds all example networks
# $Id$

import os, sys

mRoot = "."
if len(sys.argv)>1:
    mRoot = sys.argv[1]
for root, dirs, files in os.walk(mRoot):
    if ".svn" in dirs:
        dirs.remove(".svn")

    for file in files:
        if file.endswith(".netc.cfg"):
            print "----------------------------------"
            print "Runnning: " + file
            if os.name=="nt":
                (cin, cout) = os.popen4("..\\bin\\netconvert -v -c " + os.path.join(root, file))
            else:
                (cin, cout) = os.popen4("../src/sumo-netconvert -v -c " + os.path.join(root, file))
            line = cout.readline()
            while line:
                 print line[:-1]
                 line = cout.readline()
            print "----------------------------------\n"
        if file.endswith(".netg.cfg"):
            print "----------------------------------"
            print "Runnning: " + file
            if os.name=="nt":
                (cin, cout) = os.popen4("..\\bin\\netgen -v -c " + os.path.join(root, file))
            else:
                (cin, cout) = os.popen4("../src/sumo-netgen -v -c " + os.path.join(root, file))
            line = cout.readline()
            while line:
                 print line[:-1]
                 line = cout.readline()
            print "----------------------------------\n"
