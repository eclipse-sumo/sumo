#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    dailyBuildMSVC.py
# @author  Michael Behrisch
# @author  Jakob Erdmann
# @author  Laura Bieker
# @date    2008

"""
Does the nightly git pull on the windows server and the visual
studio build. The script is also used for the meso build.
Some paths especially for the names of the texttest output dirs are
hard coded into this script.
"""
from __future__ import absolute_import
from __future__ import print_function
import io
import datetime
import optparse
import os
import glob
import subprocess
import zipfile
import shutil
import sys

import filterDebugDLL
import status
import wix

env = os.environ
if "SUMO_HOME" not in env:
    env["SUMO_HOME"] = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SUMO_HOME = env["SUMO_HOME"]
env["PYTHON"] = "python"
env["SMTP_SERVER"] = "smtprelay.dlr.de"

sys.path += [os.path.join(SUMO_HOME, "tools"), os.path.join(SUMO_HOME, "tests")]
import sumolib  # noqa
import runExtraTests  # noqa

BINARIES = ("activitygen", "emissionsDrivingCycle", "emissionsMap",
            "dfrouter", "duarouter", "jtrrouter", "marouter",
            "netconvert", "netedit", "netgenerate",
            "od2trips", "polyconvert", "sumo", "sumo-gui",
            "TraCITestClient")


def repositoryUpdate(options, log):
    gitrev = ""
    cwd = os.getcwd()
    for d in options.repositories.split(","):
        os.chdir(os.path.join(options.rootDir, d))
        subprocess.call(["git", "pull"], stdout=log, stderr=subprocess.STDOUT)
        subprocess.call(["git", "submodule", "update"], stdout=log, stderr=subprocess.STDOUT)
        if gitrev == "":
            gitrev = sumolib.version.gitDescribe()
    os.chdir(cwd)
    return gitrev


def runTests(options, env, gitrev, log, debugSuffix=""):
    if not options.tests:
        return
    prefix = env["FILEPREFIX"] + debugSuffix
    env["SUMO_BATCH_RESULT"] = os.path.join(options.rootDir, prefix + "batch_result")
    env["SUMO_REPORT"] = os.path.join(options.remoteDir, prefix + "report")
    env["TEXTTEST_TMP"] = os.path.join(options.rootDir, prefix + "texttesttmp")
    env["TEXTTEST_HOME"] = os.path.join(SUMO_HOME, "tests")
    shutil.rmtree(env["TEXTTEST_TMP"], True)
    if not os.path.exists(env["SUMO_REPORT"]):
        os.makedirs(env["SUMO_REPORT"])
    for name in BINARIES:
        binary = os.path.join(SUMO_HOME, "bin", name + debugSuffix + ".exe")
        if name == "sumo-gui":
            if os.path.exists(binary):
                env["GUISIM_BINARY"] = binary
        elif os.path.exists(binary):
            env[name.upper() + "_BINARY"] = binary
    # provide more information than just the date:
    fullOpt = ["-b", prefix, "-name", "%sr%s" %
               (datetime.date.today().strftime("%d%b%y"), gitrev)]
    ttBin = "texttest"
    if options.suffix == "extra":
        runExtraTests.run(debugSuffix, fullOpt, log, True, True, debugSuffix == "")
    else:
        subprocess.call([ttBin] + fullOpt, env=env,
                        stdout=log, stderr=subprocess.STDOUT, shell=True)
        subprocess.call([ttBin, "-a", "sumo.gui"] + fullOpt, env=env,
                        stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call([ttBin, "-b", env["FILEPREFIX"], "-coll"], env=env,
                    stdout=log, stderr=subprocess.STDOUT, shell=True)
    status.killall((debugSuffix,), BINARIES)


def generateCMake(generator, platform, log, checkOptionalLibs, python):
    buildDir = os.path.join(SUMO_HOME, "build", "cmake-build-" + platform)
    cmakeOpt = ["-DCOMPILE_DEFINITIONS=MSVC_TEST_SERVER",
                "-DCHECK_OPTIONAL_LIBS=%s" % checkOptionalLibs]
    if python:
        cmakeOpt += ["-DPYTHON_EXECUTABLE=%s" % python]
    if checkOptionalLibs:
        cmakeOpt += ["-DSUMO_UTILS=True"]
    # Create directory or clear it if already exists
    try:
        if os.path.exists(buildDir):
            status.printLog("Cleaning directory of %s." % generator, log)
            shutil.rmtree(buildDir)
        os.makedirs(buildDir)
    except Exception as e:
        status.printLog("Error occured on build dir cleanup: %s." % e, log)
    status.printLog("Creating solution for %s." % generator, log)
    subprocess.call(["cmake", "../..", "-G", generator, "-A", platform] + cmakeOpt,
                    cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)
    return buildDir


optParser = optparse.OptionParser()
optParser.add_option("-r", "--root-dir", dest="rootDir",
                     default=r"D:\Sumo", help="root for git and log output")
optParser.add_option("-s", "--suffix", default="", help="suffix to the fileprefix")
optParser.add_option("-m", "--remote-dir", dest="remoteDir", default="S:\\daily",
                     help="directory to move the results to")
optParser.add_option("-n", "--no-tests", dest="tests", action="store_false",
                     default=True, help="skip tests")
optParser.add_option("-x", "--x64only", action="store_true",
                     default=False, help="skip debug build")
optParser.add_option("-p", "--python", help="path to python interpreter to use")
optParser.add_option("-c", "--msvc-version", default="msvc16",
                     help="Visual Studio version to use (either msvc12 or msvc16)")
optParser.add_option("-u", "--repositories", default="git",
                     help="repositories to update")
(options, args) = optParser.parse_args()


for platform in ["x64"]:
    env["FILEPREFIX"] = options.msvc_version + options.suffix + platform
    prefix = os.path.join(options.remoteDir, env["FILEPREFIX"])
    makeLog = prefix + "Release.log"
    makeAllLog = prefix + "Debug.log"
    statusLog = prefix + "status.log"

    status.killall(("", "D"), BINARIES)
    toClean = [makeLog, makeAllLog]
    for ext in ("*.exe", "*.ilk", "*.pdb", "*.py", "*.pyd", "*.dll", "*.lib", "*.exp", "*.jar", "*.manifest"):
        toClean += glob.glob(os.path.join(SUMO_HOME, "bin", ext))
    toClean += glob.glob(os.path.join(SUMO_HOME, "tools", "lib*", "*lib*"))
    toClean += glob.glob(os.path.join(SUMO_HOME, "share", "*", "*"))
    for f in toClean:
        try:
            os.remove(f)
        except Exception:
            pass
    for d in (glob.glob(os.path.join(SUMO_HOME, "bin", "osgPlugins*")) +
              glob.glob(os.path.join(SUMO_HOME, "tools", "*.egg-info"))):
        shutil.rmtree(d, ignore_errors=True)
    for d in glob.glob(os.path.join(SUMO_HOME, "docs", "*")):
        if os.path.basename(d) in ('examples', 'javadoc', 'man', 'pydoc', 'tutorial', 'userdoc'):
            shutil.rmtree(d, ignore_errors=True)

    # we need to use io.open here due to http://bugs.python.org/issue16273
    with io.open(makeLog, 'a') as log:
        status.printLog("Running %s build using python %s." % (options.msvc_version, sys.version), log)
        gitrev = repositoryUpdate(options, log)
        generator = "Visual Studio " + ("12 2013" if options.msvc_version == "msvc12" else "16 2019")
        buildDir = generateCMake(generator, platform, log, options.suffix == "extra", options.python)
        ret = subprocess.call(["cmake", "--build", ".", "--config", "Release"],
                              cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)
        subprocess.call(["cmake", "--build", ".", "--config", "Release", "--target", "lisum"],
                        cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)
        subprocess.call(["cmake", "--build", ".", "--config", "Release", "--target", "userdoc", "examples"],
                        cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)
        subprocess.call(["cmake", "--install", "."],
                        cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)
        plat = platform.lower().replace("x", "win")
        if options.msvc_version != "msvc16":
            plat += options.msvc_version
        for d in glob.glob(os.path.join(buildDir, "sumo-*")):
            if os.path.isdir(d):
                installDir = d
        installBase = os.path.basename(installDir)
        binaryZip = os.path.join(buildDir, "sumo-%s%s-%s" % (plat, options.suffix, installBase[5:]))
        if ret == 0:
            try:
                for f in (glob.glob(os.path.join(SUMO_HOME, "*.md")) +
                          [os.path.join(SUMO_HOME, n) for n in ("AUTHORS", "ChangeLog", "LICENSE")]):
                    shutil.copy(f, installDir)
                shutil.copytree(os.path.join(SUMO_HOME, "docs"), os.path.join(installDir, "docs"))
                shutil.copy(os.path.join(buildDir, "src", "version.h"), os.path.join(installDir, "include"))
                status.printLog("Creating sumo.zip.", log)
                shutil.make_archive(binaryZip, 'zip', buildDir, installBase)
                shutil.copy(binaryZip + ".zip", options.remoteDir)
                if options.suffix == "":
                    # installers only for the vanilla build
                    status.printLog("Creating sumo.msi.", log)
                    wix.buildMSI(binaryZip + ".zip", binaryZip + ".msi", log=log)
                    shutil.copy(binaryZip + ".msi", options.remoteDir)
            except Exception as ziperr:
                status.printLog("Warning: Could not zip to %s.zip (%s)!" % (binaryZip, ziperr), log)
        binaryZip += ".zip"

        status.printLog("Creating sumo-game.zip.", log)
        try:
            try:
                import py2exe  # noqa
                setup = os.path.join(SUMO_HOME, 'tools', 'game', 'setup.py')
                subprocess.call(['python', setup, binaryZip], stdout=log, stderr=subprocess.STDOUT)
            except ImportError:
                subprocess.call(["cmake", "--build", ".", "--target", "game"],
                                cwd=buildDir, stdout=log, stderr=subprocess.STDOUT)
                shutil.move(os.path.join(buildDir, "sumo-game.zip"), binaryZip.replace("sumo-", "sumo-game-"))
            shutil.copy(binaryZip.replace("sumo-", "sumo-game-"), options.remoteDir)
        except Exception as e:
            status.printLog("Warning: Could not create nightly sumo-game.zip! (%s)" % e, log)

        with open(makeAllLog, 'a') as debugLog:
            ret = subprocess.call(["cmake", "--build", ".", "--config", "Debug"],
                                  cwd=buildDir, stdout=debugLog, stderr=subprocess.STDOUT)
            if ret == 0:
                status.printLog("Creating sumoDebug.zip.", debugLog)
                try:
                    with zipfile.ZipFile(binaryZip.replace(plat, plat + "Debug"), 'w', zipfile.ZIP_DEFLATED) as zipf:
                        for ext in ("*D.exe", "*.dll", "*D.pdb"):
                            for f in glob.glob(os.path.join(SUMO_HOME, "bin", ext)):
                                zipf.write(f, os.path.join(installBase, "bin", os.path.basename(f)))
                        for f in glob.glob(os.path.join(SUMO_HOME, "tools", "lib*", "*lib*.p*")):
                            zipf.write(f, os.path.join(installBase, f[len(SUMO_HOME):]))
                    shutil.copy(binaryZip.replace(plat, plat + "Debug"), options.remoteDir)
                except IOError as ziperr:
                    status.printLog("Warning: Could not zip to %s (%s)!" % (binaryZip, ziperr), debugLog)

        status.printLog("Running tests.", log)
        runTests(options, env, gitrev, log)
    with open(statusLog, 'w') as log:
        status.printStatus(makeLog, makeAllLog, env["SMTP_SERVER"], log)
if not options.x64only:
    with open(makeAllLog, 'a') as debugLog:
        status.printLog("Running debug tests.", debugLog)
        runTests(options, env, gitrev, debugLog, "D")
    with open(prefix + "Dstatus.log", 'w') as log:
        status.printStatus(makeAllLog, makeAllLog, env["SMTP_SERVER"], log)
