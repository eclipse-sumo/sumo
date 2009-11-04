#!/usr/bin/env python
"""
@file    extractTest.py
@author  Michael.Behrisch@dlr.de
@date    2009-07-08
@version $Id$

Extract all files for a test case into a new dir.
It may copy more files than needed because it copies everything
that is mentioned in the config under copy_test_path.

Copyright (C) 2009 DLR/TS, Germany
All rights reserved
"""
import optparse, os, glob, sys, shutil, subprocess
from os.path import join 

optParser = optparse.OptionParser(usage="%prog <options> <test directory>")
optParser.add_option("-o", "--output", default=".", help="send output to directory")
optParser.add_option("-f", "--file", help="read list of source and target dirs from")
options, args = optParser.parse_args()
if not options.file and len(args) == 0:
    optParser.print_help()
    sys.exit(1)
targets = {}
if options.file:
    dirname = os.path.dirname(options.file)
    for line in open(options.file):
        line = line.strip()
        if line and line[0] != '#':
            key, value = line.split(':')
            targets[join(dirname, key)] = join(dirname, value)
for val in args:
    targets[val] = ""
for source, target in targets.iteritems():
    outputFiles = glob.glob(join(source, "output.[0-9a-z]*"))
    if len(outputFiles) != 1:
        print >> sys.stderr, "Not a unique output file in %s." % source
        continue
    app = outputFiles[0].split('.')[1]
    optionsFiles = []
    potentials = {}
    curDir = source
    while True:
        for f in os.listdir(curDir):
            path = join(curDir, f)
            if not os.path.isdir(path) and not f in potentials:
                potentials[f] = path
            if f == "options."+app:
                optionsFiles.append(path)
        if curDir == os.path.dirname(curDir) or os.path.exists(join(curDir, "config."+app)):
            break
        curDir = os.path.dirname(curDir)
    config = join(curDir, "config."+app)
    if not os.path.exists(config):
        print >> sys.stderr, "Config not found for %s." % curDir
        continue
    if target == "":
        target = source[len(os.path.commonprefix([curDir, source])):].replace(os.sep, '_')
    testPath = join(options.output, target)
    if not os.path.exists(testPath):
        os.makedirs(testPath)
    for line in open(config):
        entry = line.strip().split(':')
        if entry and entry[0] == "copy_test_path" and entry[1] in potentials:
            shutil.copy2(potentials[entry[1]], testPath)
    appOptions = []
    for f in optionsFiles:
        appOptions += open(f).read().split()
    appOptions += ['--save-configuration', 'test.%s.cfg' % app[:4]]
    oldWorkDir = os.getcwd()
    os.chdir(testPath)
    if app in ["dfrouter", "duarouter", "jtrrouter", "netconvert", "netgen", "od2trips", "polyconvert", "sumo"]:
        if os.name == "posix" and app != "sumo":
            subprocess.call(["sumo-" + app] + appOptions)
        else:
            subprocess.call([app] + appOptions)
    os.chdir(oldWorkDir)
