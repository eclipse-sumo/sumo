#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    extractTest.py
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Mirko Barthauer
# @date    2009-07-08

"""
Extract all files for a test case into a new dir.
It may copy more files than needed because it copies everything
that is mentioned in the config under copy_test_path.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import stat
import sys
from os.path import join
import glob
import shutil
import shlex
import subprocess
from collections import defaultdict

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
SUMO_HOME = os.path.dirname(THIS_DIR)
sys.path.append(join(SUMO_HOME, "tools"))

import sumolib  # noqa

# cannot use ':' because it is a component of absolute paths on windows
SOURCE_DEST_SEP = ';'


def get_options(args=None):
    optParser = sumolib.options.ArgumentParser(usage="%(prog)s <options> [<test directory>[;<target_directory>]]*")
    optParser.add_option("-o", "--output", category="output",  default=".", help="send output to directory")
    optParser.add_option("-f", "--file", category="input", help="read list of source and target dirs from")
    optParser.add_option("-p", "--python-script", category="input",
                         help="name of a python script to generate for a batch run")
    optParser.add_option("-i", "--intelligent-names", category="processing", dest="names", action="store_true",
                         default=False, help="generate cfg name from directory name")
    optParser.add_option("-v", "--verbose", category="processing", action="store_true", default=False,
                         help="more information")
    optParser.add_option("-a", "--application", category="processing", help="sets the application to be used")
    optParser.add_option("-s", "--skip-configuration", category="processing", default=False, action="store_true",
                         help="skips creation of an application config from the options.app file")
    optParser.add_option("-x", "--skip-validation", category="processing", default=False, action="store_true",
                         help="remove all options related to XML validation")
    optParser.add_option("-d", "--no-subdir", category="processing", dest="noSubdir", action="store_true",
                         default=False, help="store test files directly in the output directory")
    optParser.add_option("--depth", category="processing", type=int, default=1,
                         help="maximum depth when descending into testsuites")
    optParser.add_option("test_dir", nargs="*", category="input", help="read list of source and target dirs from")
    options = optParser.parse_args(args)
    if not options.file and not options.test_dir:
        optParser.error("Please specify either an input file or a test directory")
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
                ls = line.split(SOURCE_DEST_SEP) + [""]
                ls[0] = join(dirname, ls[0])
                ls[1] = join(dirname, ls[1])
                targets.append(ls[:3])
    for val in options.test_dir:
        source_and_maybe_target = val.split(SOURCE_DEST_SEP) + ["", ""]
        targets.append(source_and_maybe_target[:3])
    depth = options.depth
    while depth > 0:
        newTargets = []
        for source, target, app in targets:
            source = os.path.realpath(source)
            suites = glob.glob(os.path.join(source, "testsuite.*"))
            if suites:
                with open(suites[0]) as s:
                    for line in s:
                        line = line.strip()
                        if line and not line.startswith("#"):
                            if target == "" and not options.noSubdir:
                                target = generateTargetName(os.path.realpath(join(SUMO_HOME, "tests")), source)
                            newTargets.append((os.path.join(source, line), os.path.join(target, line), app))
            else:
                newTargets.append((source, target, app))
        targets = newTargets
        depth -= 1

    if options.python_script:
        if not os.path.exists(os.path.dirname(options.python_script)):
            os.makedirs(os.path.dirname(options.python_script))
        pyBatch = open(options.python_script, 'w')
        pyBatch.write('''#!/usr/bin/env python
import subprocess, sys, os, multiprocessing
from os.path import abspath, dirname, join
THIS_DIR = abspath(dirname(__file__))
SUMO_HOME = os.environ.get("SUMO_HOME", dirname(dirname(THIS_DIR)))
os.environ["SUMO_HOME"] = SUMO_HOME
calls = [
''')
    for source, target, app in targets:
        optionsFiles = defaultdict(list)
        configFiles = defaultdict(list)
        potentials = defaultdict(list)
        source = os.path.realpath(source)
        curDir = source
        if curDir[-1] == os.path.sep:
            curDir = os.path.dirname(curDir)
        while True:
            for f in sorted(os.listdir(curDir)):
                path = join(curDir, f)
                if f not in potentials or os.path.isdir(path):
                    potentials[f].append(path)
                if f.startswith("options."):
                    optionsFiles[f[8:]].append(path)
                if f.startswith("config."):
                    configFiles[f[7:]].append(path)
            if curDir == os.path.realpath(join(SUMO_HOME, "tests")) or curDir == os.path.dirname(curDir):
                break
            curDir = os.path.dirname(curDir)
        if not configFiles:
            print("Config not found for %s." % source, file=sys.stderr)
            continue
        if len(glob.glob(os.path.join(source, "testsuite.*"))) > 0:
            print("Directory %s seems to contain a test suite." % source, file=sys.stderr)
            continue
        if app == "":
            for v in configFiles.keys():
                if "." not in v:
                    app = v
                    break
        haveVariant = False
        for variant in sorted(set(optionsFiles.keys()) | set(configFiles.keys())):
            if options.application not in (None, "ALL", variant, variant.split(".")[-1]):
                continue
            if options.application is None and len(glob.glob(os.path.join(source, "*" + variant))) == 0:
                if options.verbose:
                    print("ignoring variant %s for '%s'" % (variant, source))
                continue
            haveVariant = True
            cfg = configFiles[variant] + configFiles[app]
            if target == "" and not options.noSubdir:
                target = generateTargetName(os.path.dirname(cfg[-1]), source)
            testPath = os.path.abspath(join(options.output, target))
            if not os.path.exists(testPath):
                os.makedirs(testPath)
            net = None
            skip = False
            appOptions = []
            optFiles = optionsFiles[app] + ([] if variant == app else optionsFiles[variant])
            for f in sorted(optFiles, key=lambda o: o.count(os.sep)):
                newOptions = []
                clearOptions = None
                with open(f) as oFile:
                    for o in shlex.split(oFile.read()):
                        if skip:
                            skip = False
                            continue
                        if o == "--xml-validation" and options.skip_validation:
                            skip = True
                            continue
                        if o == "{CLEAR}":
                            appOptions = []
                            continue
                        if o == "{CLEAR":
                            clearOptions = []
                            continue
                        if clearOptions is not None:
                            if o[-1] == "}":
                                clearOptions.append(o[:-1])
                                numClear = len(clearOptions)
                                for idx in range(len(appOptions) - numClear + 1):
                                    if appOptions[idx:idx+numClear] == clearOptions:
                                        del appOptions[idx:idx+numClear]
                                        clearOptions = None
                                        break
                            else:
                                clearOptions.append(o)
                            continue
                        if o[0] == "-" and o in appOptions:
                            idx = appOptions.index(o)
                            if idx < len(appOptions) - 1 and appOptions[idx + 1][0] != "-":
                                del appOptions[idx:idx+2]
                            else:
                                del appOptions[idx:idx+1]
                        newOptions.append(o)
                        if "=" in o:
                            o = o.split("=")[-1]
                        if o[-8:] == ".net.xml":
                            net = o
                appOptions += newOptions
            nameBase = "test"
            if options.names:
                nameBase = os.path.basename(target)
            if "." in variant:
                nameBase += variant.split(".")[-1]
            isNetdiff = False
            for a in appOptions:
                if "netdiff.py" in a:
                    isNetdiff = True
            exclude = []
            # gather copy_test_path exclusions
            for configFile in cfg:
                with open(configFile) as config:
                    for line in config:
                        entry = line.strip().split(':')
                        if entry and entry[0] == "test_data_ignore":
                            exclude.append(entry[1])
            # copy test data from the tree
            for configFile in cfg:
                with open(configFile) as config:
                    for line in config:
                        entry = line.strip().split(':')
                        if entry and "copy_test_path" in entry[0] and entry[1] in potentials:
                            if "net" in app or isNetdiff or not net or entry[1][-8:] != ".net.xml" or entry[1] == net:
                                toCopy = potentials[entry[1]][0]
                                if os.path.isdir(toCopy):
                                    # copy from least specific to most specific
                                    merge = entry[0] == "copy_test_path_merge"
                                    for toCopy in reversed(potentials[entry[1]]):
                                        copy_merge(toCopy, join(testPath, os.path.basename(toCopy)), merge, exclude)
                                else:
                                    shutil.copy2(toCopy, testPath)
            if options.python_script:
                if app == "netgen":
                    call = ['join(SUMO_HOME, "bin", "netgenerate")'] + ['"%s"' % a for a in appOptions]
                elif app == "tools":
                    call = ['sys.executable'] + ['"%s"' % a for a in appOptions]
                    call[1] = 'join(SUMO_HOME, "%s")' % appOptions[0]
                elif app == "complex":
                    call = ['sys.executable']
                    for a in appOptions:
                        if a.endswith(".py"):
                            if os.path.exists(join(testPath, os.path.basename(a))):
                                call.insert(1, '"./%s"' % os.path.basename(a))
                            else:
                                call.insert(1, 'join(SUMO_HOME, "%s")' % a)
                        else:
                            call.append('"%s"' % a)
                else:
                    call = ['join(SUMO_HOME, "bin", "%s")' % app] + ['"%s"' % a for a in appOptions]
                prefix = os.path.commonprefix((testPath, os.path.abspath(pyBatch.name)))
                up = os.path.abspath(pyBatch.name)[len(prefix):].count(os.sep) * "../"
                pyBatch.write('    (r"%s", [%s], r"%s%s"),\n' %
                              (testPath[len(prefix):], ', '.join(call), up, testPath[len(prefix):]))
            if options.skip_configuration:
                continue
            oldWorkDir = os.getcwd()
            os.chdir(testPath)
            haveConfig = False
            # look for python executable
            pythonPath = os.environ["PYTHON"] if "PYTHON" in os.environ else os.environ.get("PYTHON_HOME", "python")
            if os.path.isdir(pythonPath):
                pythonPath = os.path.join(pythonPath, "python")
            if app in ["dfrouter", "duarouter", "jtrrouter", "marouter", "netconvert",
                       "netgen", "netgenerate", "od2trips", "polyconvert", "sumo", "activitygen"]:
                if app == "netgen":
                    # binary is now called differently but app still has the old name
                    app = "netgenerate"
                if options.verbose:
                    print("calling %s for testPath '%s' with options '%s'" %
                          (sumolib.checkBinary(app), testPath, " ".join(appOptions)))
                try:
                    haveConfig = subprocess.call([sumolib.checkBinary(app)] + appOptions +
                                                 ['--save-configuration', '%s.%scfg' %
                                                  (nameBase, app[:4])]) == 0
                except OSError:
                    print("Executable %s not found, generating shell scripts instead of config." % app, file=sys.stderr)
                if not haveConfig:
                    appOptions.insert(0, '"$SUMO_HOME/bin/%s"' % app)
            elif app == "tools":
                for i, a in enumerate(appOptions):
                    if a.endswith(".py"):
                        del appOptions[i:i+1]
                        appOptions[0:0] = [pythonPath, '"$SUMO_HOME/%s"' % a]
                        break
                    if a.endswith(".jar"):
                        del appOptions[i:i+1]
                        appOptions[0:0] = ["java", "-jar", '"$SUMO_HOME/%s"' % a]
                        break
            elif app == "complex":
                for i, a in enumerate(appOptions):
                    if a.endswith(".py"):
                        if os.path.exists(join(testPath, os.path.basename(a))):
                            a = os.path.basename(a)
                        else:
                            a = '"$SUMO_HOME/%s"' % a
                        del appOptions[i:i+1]
                        appOptions[0:0] = [pythonPath, a]
                        break
            if not haveConfig:
                if options.verbose:
                    print("generating shell scripts for testPath '%s' with call '%s'" %
                          (testPath, " ".join(appOptions)))
                cmd = [ao if " " not in ao else "'%s'" % ao for ao in appOptions]
                with open(nameBase + ".sh", "w") as sh:
                    sh.write("#!/bin/bash\n")
                    sh.write(" ".join(cmd))
                os.chmod(sh.name, os.stat(sh.name).st_mode | stat.S_IXUSR)
                cmd = [o.replace("$SUMO_HOME", "%SUMO_HOME%") if " " not in o else '"%s"' % o for o in appOptions]
                with open(nameBase + ".bat", "w") as bat:
                    bat.write(" ".join(cmd))
            os.chdir(oldWorkDir)
        if not haveVariant:
            print("No suitable variant found for %s." % source, file=sys.stderr)
    if options.python_script:
        pyBatch.write("""]
procs = []
def check():
    for d, p in procs:
        if p.wait() != 0:
            print("Error: '%s' failed for '%s'!" % (" ".join(getattr(p, "args", [str(p.pid)])), d))
            sys.exit(1)

for dir, call, wd in calls:
    procs.append((dir, subprocess.Popen(call, cwd=join(THIS_DIR, wd))))
    if len(procs) == multiprocessing.cpu_count():
        check()
        procs = []
check()
""")
        pyBatch.close()
        os.chmod(pyBatch.name, os.stat(pyBatch.name).st_mode | stat.S_IXUSR)


if __name__ == "__main__":
    main(get_options())
