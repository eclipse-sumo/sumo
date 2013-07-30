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
import os, sys
from os.path import join 
import optparse, os, glob, sys, shutil, subprocess
from collections import defaultdict

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
SUMO_HOME = join(THIS_DIR, '..')
sys.path.append(join(SUMO_HOME, "tools"))

from sumolib import checkBinary
os.environ["PATH"] += os.pathsep + join(SUMO_HOME, 'bin')

def get_options(args=None):
    optParser = optparse.OptionParser(usage="%prog <options> <test directory>")
    optParser.add_option("-o", "--output", default=".", help="send output to directory")
    optParser.add_option("-f", "--file", help="read list of source and target dirs from")
    optParser.add_option("-i", "--intelligent-names", dest="names", action="store_true",
                         default=False, help="generate cfg name from directory name")
    optParser.add_option("-a", "--application", help="sets the application to be used")
    optParser.add_option("-s", "--skip-configuration",
            dest="skip_configuration", default=False, action="store_true",
            help="skips creation of an application config from the options.app file")
    options, args = optParser.parse_args(args=args)
    if not options.file and len(args) == 0:
        optParser.print_help()
        sys.exit(1)
    options.args = args
    return options

def merge(srcDir, dstDir):
    """merge contents of srcDir recursively into dstDir"""
    for dir, subdirs, files in os.walk(srcDir):
        dst = dir.replace(srcDir, dstDir)
        if not os.path.exists(dst):
            #print "creating dir '%s' as a copy of '%s'" % (dst, srcDir)
            os.mkdir(dst)
        for file in files:
            #print "copying file '%s' to '%s'" % (join(dir, file), join(dst, file))
            shutil.copy(join(dir, file), join(dst, file))


def main(options):
    targets = {}
    if options.file:
        dirname = os.path.dirname(options.file)
        for line in open(options.file):
            line = line.strip()
            if line and line[0] != '#':
                key, value = line.split(':')
                targets[join(dirname, key)] = join(dirname, value)
    for val in options.args:
        source_and_maybe_target = val.split(':') + [""]
        targets[source_and_maybe_target[0]] = source_and_maybe_target[1]

    for source, target in targets.iteritems():
        outputFiles = glob.glob(join(source, "output.[0-9a-z]*"))
        # XXX we should collect the options.app.variant files in all parent
        # directories instead. This would allow us to save config files for all variants
        appName = set([f.split('.')[-1] for f in outputFiles])
        if len(appName) != 1:
            if options.application in appName:
                appName = set([options.application])
            else:
                print >> sys.stderr, "Skipping %s because the application was not unique (found %s)." % (source, appName)
                continue
        app = iter(appName).next()
        optionsFiles = []
        potentials = defaultdict(list)
        source = os.path.realpath(source)
        curDir = source
        if curDir[-1] == os.path.sep:
            curDir = os.path.dirname(curDir)
        while True:
            for f in os.listdir(curDir):
                path = join(curDir, f)
                if not f in potentials or os.path.isdir(path):
                    potentials[f].append(path)
                if f == "options."+app:
                    optionsFiles.append(path)
            if curDir == os.path.dirname(curDir) or os.path.exists(join(curDir, "config."+app)):
                break
            curDir = os.path.dirname(curDir)
        config = join(curDir, "config."+app)
        if not os.path.exists(config):
            print >> sys.stderr, "Config '%s' not found for %s." % (config, source)
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
            if entry and "copy_test_path" in entry[0] and entry[1] in potentials:
                if "net" in app or not net or entry[1][-8:] != ".net.xml" or entry[1] == net:
                    toCopy = potentials[entry[1]][0]
                    if os.path.isdir(toCopy):
                        if entry[0] == "copy_test_path_merge":
                            # copy from least specific to most specific
                            for toCopy in reversed(potentials[entry[1]]):
                                merge(toCopy, join(testPath, os.path.basename(toCopy)))
                        else:
                            # only use the most specific version
                            shutil.copytree(toCopy, os.path.join(testPath,
                                os.path.basename(toCopy)))
                    else:
                        shutil.copy2(toCopy, testPath)
        oldWorkDir = os.getcwd()
        os.chdir(testPath)
        if (not options.skip_configuration 
                and app in ["dfrouter", "duarouter", "jtrrouter", "marouter", "netconvert", 
                    "netgen", "netgenerate", "od2trips", "polyconvert", "sumo", "activitygen"]):
            if "meso" in testPath and app == "sumo":
                app = "meso"
            if app == "netgen":
                # binary is now called differently but app still has the old name
                app = "netgenerate"
            subprocess.call([checkBinary(app)] + appOptions)
        os.chdir(oldWorkDir)

if __name__ == "__main__":
    main(get_options())

