#!/usr/bin/env python
"""
@file    dailyBuildMSVC.py
@author  Michael Behrisch
@author  Jakob Erdmann
@author  Laura Bieker
@date    2008
@version $Id$

Does the nightly svn update on the windows server and the visual
studio build. The script is also used for the meso build.
Some paths especially for the names of the texttest output dirs are
hard coded into this script.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function
import re
import io
from datetime import date
import optparse
import os
import glob
import subprocess
import zipfile
import shutil
import datetime
import sys

import runSikulixServer
import status
import wix


def repositoryUpdate(options, repoLogFile):
    if options.no_update:
        return ""
    with open(repoLogFile, 'w') as log:
        subprocess.call(
            "svn.exe up %s\\trunk" % options.rootDir, stdout=log, stderr=subprocess.STDOUT)
    update_log = open(repoLogFile).read()
    match_rev = re.search('At revision (\d*)\.', update_log)
    if match_rev:
        svnrev = match_rev.group(1)
    else:
        open(repoLogFile, 'a').write("Error parsing svn revision\n")
        sys.exit()
    end_marker = 'Fetching external'
    if end_marker in update_log:
        update_lines = len(
            update_log[:update_log.index(end_marker)].splitlines())
    else:
        open(repoLogFile, 'a').write("Error parsing svn output\n")
        sys.exit()

    if update_lines < 3 and not options.force:
        open(repoLogFile, 'a').write(
            "No changes since last update, skipping build and test\n")
        print("No changes since last update, skipping build and test")
        sys.exit()
    return svnrev


def runTests(options, env, svnrev, debugSuffix=""):
    if options.no_tests:
        return
    prefix = env["FILEPREFIX"] + debugSuffix
    env["SUMO_BATCH_RESULT"] = os.path.join(
        options.rootDir, prefix + "batch_result")
    env["SUMO_REPORT"] = os.path.join(options.remoteDir, prefix + "report")
    testLog = os.path.join(options.remoteDir, prefix + "test.log")
    env["TEXTTEST_TMP"] = os.path.join(
        options.rootDir, prefix + "texttesttmp")
    env["TEXTTEST_HOME"] = os.path.join(options.rootDir, options.testsDir)
    shutil.rmtree(env["TEXTTEST_TMP"], True)
    if not os.path.exists(env["SUMO_REPORT"]):
        os.makedirs(env["SUMO_REPORT"])
    for name in ["dfrouter", "duarouter", "jtrrouter", "marouter", "netconvert", "netgenerate",
                 "od2trips", "sumo", "polyconvert", "sumo-gui", "activitygen",
                 "emissionsDrivingCycle", "emissionsMap"]:
        image = name + debugSuffix + ".exe"
        subprocess.call(["taskkill", "/f", "/im", image])
        binary = os.path.join(options.rootDir, options.binDir, image)
        if name == "sumo-gui":
            if os.path.exists(binary):
                env["GUISIM_BINARY"] = binary
        elif os.path.exists(binary):
            env[name.upper() + "_BINARY"] = binary
    log = open(testLog, 'w')
    # provide more information than just the date:
    fullOpt = ["-b", prefix, "-name", "%sr%s" %
               (date.today().strftime("%d%b%y"), svnrev)]
    ttBin = "texttestc.py"
    if options.suffix == "extra":
        runInternalTests.runInternal(
            debugSuffix, fullOpt, log, True, True, debugSuffix == "")
    else:
        subprocess.call([ttBin] + fullOpt, env=env,
                        stdout=log, stderr=subprocess.STDOUT, shell=True)
        subprocess.call([ttBin, "-a", "sumo.gui"] + fullOpt, env=env,
                        stdout=log, stderr=subprocess.STDOUT, shell=True)
        # Check if sikulixServer is already opened
        # if runSikulixServer.checkStatus() == False :
        #    runSikulixServer.startSikulixServer()
        subprocess.call([ttBin, "-a", "netedit.gui"] + fullOpt, env=env,
                        stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call([ttBin, "-b", env["FILEPREFIX"], "-coll"], env=env,
                    stdout=log, stderr=subprocess.STDOUT, shell=True)
    log.close()


optParser = optparse.OptionParser()
optParser.add_option("-r", "--root-dir", dest="rootDir",
                     default=r"D:\Sumo", help="root for svn and log output")
optParser.add_option(
    "-s", "--suffix", default="", help="suffix to the fileprefix")
optParser.add_option("-p", "--project", default=r"trunk\sumo\build\msvc10\prj.sln",
                     help="path to project solution relative to the root dir")
optParser.add_option("-b", "--bin-dir", dest="binDir", default=r"trunk\sumo\bin",
                     help="directory containg the binaries, relative to the root dir")
optParser.add_option("-t", "--tests-dir", dest="testsDir", default=r"trunk\sumo\tests",
                     help="directory containg the tests, relative to the root dir")
optParser.add_option("-m", "--remote-dir", dest="remoteDir",
                     default=r"O:\Daten\Sumo\daily",
                     help="directory to move the results to")
optParser.add_option("-l", "--add-solution", dest="addSln",
                     default=r"trunk\sumo\build\msvc10\tools.sln",
                     help="path to an additional solution to build")
optParser.add_option("-d", "--dll-dirs", dest="dllDirs",
                     default=r"Win32:bin,x64:bin64",
                     help="path to dependency dlls for the relevant platforms")
optParser.add_option("-f", "--force", action="store_true",
                     default=False, help="force rebuild even if no source changed")
optParser.add_option("-u", "--no-update", action="store_true",
                     default=False, help="skip repository update")
optParser.add_option("-n", "--no-tests", action="store_true",
                     default=False, help="skip tests")
(options, args) = optParser.parse_args()

sys.path.append(os.path.join(options.rootDir, options.testsDir))
import runInternalTests

env = os.environ
if "SUMO_HOME" not in env:
    env["SUMO_HOME"] = os.path.dirname(
        os.path.dirname(os.path.dirname(__file__)))
env["PYTHON"] = "python"
env["SMTP_SERVER"] = "smtprelay.dlr.de"
compiler = r"D:\Programme\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe"
msvcVersion = "msvc10"
if "VS100COMNTOOLS" in env:
    compiler = os.path.join(env["VS100COMNTOOLS"], "..", "IDE", "devenv.exe")
if "VS120COMNTOOLS" in env:
    compiler = os.path.join(env["VS120COMNTOOLS"], "..", "IDE", "devenv.exe")
    msvcVersion = "msvc12"
svnrev = repositoryUpdate(options, os.path.join(
    options.remoteDir, msvcVersion + options.suffix + "Update.log"))

maxTime = 0
sumoAllZip = None
for fname in glob.glob(os.path.join(options.remoteDir, "sumo-all-*.zip")):
    if os.path.getmtime(fname) > maxTime:
        maxTime = os.path.getmtime(fname)
        sumoAllZip = fname
platformDlls = [entry.split(":") for entry in options.dllDirs.split(",")]
for platform, dllDir in platformDlls:
    env["FILEPREFIX"] = msvcVersion + options.suffix + platform
    prefix = os.path.join(options.remoteDir, env["FILEPREFIX"])
    makeLog = prefix + "Release.log"
    makeAllLog = prefix + "Debug.log"
    statusLog = prefix + "status.log"
    binDir = "sumo-svn/bin/"

    for f in [makeLog, makeAllLog] + glob.glob(os.path.join(options.rootDir, options.binDir, "*.exe")):
        try:
            os.remove(f)
        except WindowsError:
            pass
    subprocess.call(compiler + " /rebuild Release|%s %s\\%s /out %s" %
                    (platform, options.rootDir, options.project, makeLog))
    if options.addSln:
        subprocess.call(compiler + " /rebuild Release|%s %s\\%s /out %s" %
                        (platform, options.rootDir, options.addSln, makeLog))
    envSuffix = ""
    if platform == "x64":
        envSuffix = "_64"
    # we need to use io.open here due to http://bugs.python.org/issue16273
    log = io.open(makeLog, 'a')
    if sumoAllZip:
        try:
            binaryZip = sumoAllZip.replace("-all-", "-%s-" % env["FILEPREFIX"])
            zipf = zipfile.ZipFile(binaryZip, 'w', zipfile.ZIP_DEFLATED)
            srcZip = zipfile.ZipFile(sumoAllZip)
            write = False
            for f in srcZip.namelist():
                if f.count('/') == 1:
                    write = False
                if f.endswith('/') and f.count('/') == 2:
                    write = (f.endswith('/bin/') or f.endswith('/examples/')
                             or f.endswith('/tools/') or f.endswith('/data/') or f.endswith('/docs/'))
                    if f.endswith('/bin/'):
                        binDir = f
                elif f.endswith('/') and '/docs/' in f and f.count('/') == 3:
                    write = not f.endswith('/doxygen/')
                elif write or os.path.basename(f) in ["COPYING", "README"]:
                    zipf.writestr(f, srcZip.read(f))
            srcZip.close()
            dllPath = os.path.join(options.rootDir, dllDir)
            for f in glob.glob(os.path.join(dllPath, "*.dll")) + glob.glob(os.path.join(dllPath, "*", "*.dll")):
                zipf.write(f, os.path.join(binDir, f[len(dllPath) + 1:]))
            files_to_zip = (
                glob.glob(os.path.join(options.rootDir, options.binDir, "*.exe")) +
                glob.glob(os.path.join(options.rootDir, options.binDir, "*.jar")) +
                glob.glob(os.path.join(options.rootDir, options.binDir, "*.bat")))
            for f in files_to_zip:
                zipf.write(f, os.path.join(binDir, os.path.basename(f)))
            zipf.close()
            if options.suffix == "":
                # installers only for the vanilla build
                wix.buildMSI(
                    binaryZip, binaryZip.replace(".zip", ".msi"), log=log)
        except IOError as ziperr:
            (errno, strerror) = ziperr.args
            print("Warning: Could not zip to %s!" % binaryZip, file=log)
            print("I/O error(%s): %s" % (errno, strerror), file=log)
    try:
        setup = os.path.join(env["SUMO_HOME"], 'tools', 'game', 'setup.py')
        subprocess.call(
            ['python', setup, binaryZip], stdout=log, stderr=subprocess.STDOUT)
    except Exception as e:
        print("Warning: Could not create nightly sumo-game.zip! (%s)" %
              e, file=log)
    log.close()
    subprocess.call(compiler + " /rebuild Debug|%s %s\\%s /out %s" %
                    (platform, options.rootDir, options.project, makeAllLog))
    if options.addSln:
        subprocess.call(compiler + " /rebuild Debug|%s %s\\%s /out %s" %
                        (platform, options.rootDir, options.addSln, makeAllLog))
    runTests(options, env, svnrev)
    log = open(statusLog, 'w')
    status.printStatus(makeLog, makeAllLog, env["SMTP_SERVER"], log)
    log.close()
runTests(options, env, svnrev, "D")
log = open(prefix + "Dstatus.log", 'w')
status.printStatus(makeAllLog, makeAllLog, env["SMTP_SERVER"], log)
log.close()
