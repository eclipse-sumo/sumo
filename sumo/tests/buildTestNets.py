#!/usr/bin/env python
# Builds all example networks
# $Id$

import os, sys, subprocess

mRoot = "."
if len(sys.argv)>1:
    mRoot = sys.argv[1]
binPrefix = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'bin', 'net')
for root, dirs, files in os.walk(mRoot):
    if ".svn" in dirs:
        dirs.remove(".svn")
    for file in files:
        if file.endswith(".netc.cfg") or file.endswith(".netg.cfg"):
            exe = binPrefix + "gen"
            if file.endswith(".netc.cfg"):
                exe = binPrefix + "convert"
            print "----------------------------------"
            print "Rebuilding config: " + file
            curDir = os.getcwd()
            os.chdir(root)
            subprocess.call([exe, "--save-configuration", file+".tmp", "-c", file], stdout=sys.stdout, stderr=sys.stderr)
            os.rename(file+".tmp", file)
            os.chdir(curDir)
            print "Running: " + file
            subprocess.call([exe, "-v", "-c", os.path.join(root, file)], stdout=sys.stdout, stderr=sys.stderr)
            print "----------------------------------\n"
