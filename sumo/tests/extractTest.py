#!/usr/bin/env python
"""
@file    extractTest.py
@author  Michael Behrisch
@date    2009-07-08
@version $Id$

Extract all files for a test case into a new dir.
It may copy more files than needed because it copies everything
that is mentioned in the config under copy_test_path.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2009-2012 DLR/TS, Germany
All rights reserved
"""
import optparse, os, glob, sys, shutil, subprocess
from os.path import join 
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', "tools"))
from sumolib import checkBinary

optParser = optparse.OptionParser(usage="%prog <options> <test directory>")
optParser.add_option("-o", "--output", default=".", help="send output to directory")
optParser.add_option("-f", "--file", help="read list of source and target dirs from")
optParser.add_option("-i", "--intelligent-names", dest="names", action="store_true",
                     default=False, help="generate cfg name from directory name")
options, args = optParser.parse_args()
if not options.file and len(args) == 0:
    optParser.print_help()
    sys.exit(1)
os.environ["PATH"] += os.pathsep + os.path.abspath(join(os.path.basename(sys.argv[0]), '..', 'bin'))
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
    # XXX we should collect the options.app.variant files in all parent
    # directories instead. This would allow us to save config files for all variants
    appName = set([f.split('.')[-1] for f in outputFiles])
    if len(appName) != 1:
        print >> sys.stderr, "Skipping %s because the application was not unique (found %s)." % (source, appName)
        continue
    app = iter(appName).next()
    optionsFiles = []
    potentials = {}
    source = os.path.realpath(source)
    curDir = source
    if curDir[-1] == os.path.sep:
        curDir = os.path.dirname(curDir)
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
        print >> sys.stderr, "Config not found for %s." % source
        continue
    if target == "":
        target = source[len(os.path.commonprefix([curDir, source])):].replace(os.sep, '_')
    testPath = os.path.abspath(join(options.output, target))
    if not os.path.exists(testPath):
        os.makedirs(testPath)
    net = None
    appOptions = []
    for f in optionsFiles:
        appOptions += open(f).read().split()
    for o in appOptions:
        if "=" in o:
            o = o.split("=")[-1]
        if o[-8:] == ".net.xml":
            net = o
    nameBase = "test"
    if options.names:
        nameBase = os.path.basename(target)
    appOptions += ['--save-configuration', '%s.%scfg' % (nameBase, app[:4])]
    for line in open(config):
        entry = line.strip().split(':')
        if entry and entry[0] == "copy_test_path" and entry[1] in potentials:
            if "net" in app or not net or entry[1][-8:] != ".net.xml" or entry[1] == net:
                shutil.copy2(potentials[entry[1]], testPath)
    oldWorkDir = os.getcwd()
    os.chdir(testPath)
    if app in ["dfrouter", "duarouter", "jtrrouter", "netconvert", "netgenerate", "od2trips", "polyconvert", "sumo", "activitygen"]:
        if "meso" in testPath and app == "sumo":
            app = "meso"
        subprocess.call([checkBinary(app)] + appOptions)
    os.chdir(oldWorkDir)
