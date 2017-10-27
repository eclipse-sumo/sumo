#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    extractTest.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2009-07-08
# @version $Id$

"""
Extract all files for a test case into a new dir.
It may copy more files than needed because it copies everything
that is mentioned in the config under copy_test_path.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
from os.path import join
import optparse
import os
import glob
import sys
import shutil
import subprocess
from collections import defaultdict

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
SUMO_HOME = os.path.dirname(THIS_DIR)
sys.path.append(join(SUMO_HOME, "tools"))

from sumolib import checkBinary  # noqa
os.environ["PATH"] += os.pathsep + join(SUMO_HOME, 'bin')

# cannot use ':' because it is a component of absolute paths on windows
SOURCE_DEST_SEP = ';'


def get_options(args=None):
    optParser = optparse.OptionParser(usage="%prog <options> <test directory>")
    optParser.add_option(
        "-o", "--output", default=".", help="send output to directory")
    optParser.add_option(
        "-f", "--file", help="read list of source and target dirs from")
    optParser.add_option("-i", "--intelligent-names", dest="names", action="store_true",
                         default=False, help="generate cfg name from directory name")
    optParser.add_option("-v", "--verbose", action="store_true", default=False, help="more information")
    optParser.add_option(
        "-a", "--application", help="sets the application to be used")
    optParser.add_option("-s", "--skip-configuration",
                         dest="skip_configuration", default=False, action="store_true",
                         help="skips creation of an application config from the options.app file")
    optParser.add_option("-x", "--skip-validation",
                         dest="skip_validation", default=False, action="store_true",
                         help="remove all options related to XML validation")
    options, args = optParser.parse_args(args=args)
    if not options.file and len(args) == 0:
        optParser.print_help()
        sys.exit(1)
    options.args = args
    return options


def copy_merge(srcDir, dstDir, merge, exclude):
    """merge contents of srcDir recursively into dstDir"""
    for dir, subdirs, files in os.walk(srcDir):
        for ex in exclude:
            if ex in subdirs:
                subdirs.remove(ex)
        dst = dir.replace(srcDir, dstDir)
        if os.path.exists(dst) and not merge:
            shutil.rmtree(dst)
        if not os.path.exists(dst):
            # print "creating dir '%s' as a copy of '%s'" % (dst, srcDir)
            os.mkdir(dst)
        for file in files:
            # print("copying file '%s' to '%s'" % (join(dir, file), join(dst, file)))
            shutil.copy(join(dir, file), join(dst, file))


def generateTargetName(baseDir, source):
    return source[len(os.path.commonprefix([baseDir, source])):].replace(os.sep, '_')


def main(options):
    targets = []
    if options.file:
        dirname = os.path.dirname(options.file)
        for line in open(options.file):
            line = line.strip()
            if line and line[0] != '#':
                l = line.split(SOURCE_DEST_SEP) + [""]
                l[0] = join(dirname, l[0])
                l[1] = join(dirname, l[1])
                targets.append(l[:3])
    for val in options.args:
        source_and_maybe_target = val.split(SOURCE_DEST_SEP) + ["", ""]
        targets.append(source_and_maybe_target[:3])

    for source, target, app in targets:
        outputFiles = glob.glob(join(source, "output.[0-9a-z]*"))
        # print source, target, outputFiles
        # XXX we should collect the options.app.variant files in all parent
        # directories instead. This would allow us to save config files for all
        # variants
        appName = set([f.split('.')[-1] for f in outputFiles])
        if len(appName) != 1:
            if options.application in appName:
                appName = set([options.application])
            elif app in appName:
                appName = set([app])
            else:
                print("Skipping %s because the application was not unique (found %s)." % (
                    source, appName), file=sys.stderr)
                continue
        app = next(iter(appName))
        optionsFiles = []
        configFiles = []
        potentials = defaultdict(list)
        source = os.path.realpath(source)
        curDir = source
        if curDir[-1] == os.path.sep:
            curDir = os.path.dirname(curDir)
        while True:
            for f in os.listdir(curDir):
                path = join(curDir, f)
                if f not in potentials or os.path.isdir(path):
                    potentials[f].append(path)
                if f == "options." + app:
                    optionsFiles.append(path)
            config = join(curDir, "config." + app)
            if curDir == join(SUMO_HOME, "tests") or curDir == os.path.dirname(curDir):
                break
            if os.path.exists(config):
                configFiles.append(config)
            curDir = os.path.dirname(curDir)
        if not configFiles:
            print("Config not found for %s." % source, file=sys.stderr)
            continue
        if target == "":
            target = generateTargetName(
                os.path.dirname(configFiles[-1]), source)
        testPath = os.path.abspath(join(options.output, target))
        if not os.path.exists(testPath):
            os.makedirs(testPath)
        net = None
        skip = False
        appOptions = []
        for f in reversed(optionsFiles):
            for o in open(f).read().split():
                if skip:
                    skip = False
                    continue
                if o == "--xml-validation" and options.skip_validation:
                    skip = True
                    continue
                if o == "{CLEAR}":
                    appOptions = []
                    continue
                appOptions.append(o)
                if "=" in o:
                    o = o.split("=")[-1]
                if o[-8:] == ".net.xml":
                    net = o
        nameBase = "test"
        if options.names:
            nameBase = os.path.basename(target)
        exclude = []
        # gather copy_test_path exclusions
        for config in configFiles:
            for line in open(config):
                entry = line.strip().split(':')
                if entry and entry[0] == "test_data_ignore":
                    exclude.append(entry[1])
        # copy test data from the tree
        for config in configFiles:
            for line in open(config):
                entry = line.strip().split(':')
                if entry and "copy_test_path" in entry[0] and entry[1] in potentials:
                    if "net" in app or not net or entry[1][-8:] != ".net.xml" or entry[1] == net:
                        toCopy = potentials[entry[1]][0]
                        if os.path.isdir(toCopy):
                            # copy from least specific to most specific
                            merge = entry[0] == "copy_test_path_merge"
                            for toCopy in reversed(potentials[entry[1]]):
                                copy_merge(
                                    toCopy, join(testPath, os.path.basename(toCopy)), merge, exclude)
                        else:
                            shutil.copy2(toCopy, testPath)
        if options.skip_configuration:
            continue
        oldWorkDir = os.getcwd()
        os.chdir(testPath)
        if app in ["dfrouter", "duarouter", "jtrrouter", "marouter", "netconvert",
                   "netgen", "netgenerate", "od2trips", "polyconvert", "sumo", "activitygen"]:
            appOptions += ['--save-configuration', '%s.%scfg' %
                           (nameBase, app[:4])]
            if app == "netgen":
                # binary is now called differently but app still has the old
                # name
                app = "netgenerate"
            if options.verbose:
                print("calling %s for testPath '%s' with  options '%s'" % (checkBinary(app), testPath, " ".join(appOptions)))
            subprocess.call([checkBinary(app)] + appOptions)
        elif app == "tools":
            if os.name == "posix" or options.file:
                tool = join("$SUMO_HOME", appOptions[-1])
                open(nameBase + ".sh", "w").write(tool +
                                                  " " + " ".join(appOptions[:-1]))
            if os.name != "posix" or options.file:
                tool = join("%SUMO_HOME%", appOptions[-1])
                open(nameBase + ".bat", "w").write(tool +
                                                   " " + " ".join(appOptions[:-1]))
        os.chdir(oldWorkDir)

if __name__ == "__main__":
    main(get_options())
