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
Some paths especially for the temp dir and the compiler are
hard coded into this script.

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import with_statement
import re, io
from datetime import date
import optparse, os, glob, subprocess, zipfile, shutil, datetime, sys
import status, wix

optParser = optparse.OptionParser()
optParser.add_option("-r", "--root-dir", dest="rootDir",
                     default=r"D:\Sumo", help="root for svn and log output")
optParser.add_option("-s", "--suffix", default="", help="suffix to the fileprefix")
optParser.add_option("-p", "--project", default=r"trunk\sumo\build\msvc10\prj.sln",
                     help="path to project solution relative to the root dir")
optParser.add_option("-b", "--bin-dir", dest="binDir", default=r"trunk\sumo\bin",
                     help="directory containg the binaries, relative to the root dir")
optParser.add_option("-t", "--tests-dir", dest="testsDir", default=r"trunk\sumo\tests",
                     help="directory containg the tests, relative to the root dir")
optParser.add_option("-e", "--sumo-exe", dest="sumoExe", default="sumo",
                     help="name of the sumo executable")
optParser.add_option("-m", "--remote-dir", dest="remoteDir", default=r"O:\Daten\Sumo\daily",
                     help="directory to move the results to")
optParser.add_option("-a", "--add-build-config-prefix", dest="addConf",
                     help="prefix of an additional configuration to build")
optParser.add_option("-f", "--force", action="store_true",
                     default=False, help="force rebuild even if no source changed")
(options, args) = optParser.parse_args()

sys.path.append(os.path.join(options.rootDir, options.testsDir))
import runInternalTests

env = os.environ
env["SMTP_SERVER"]="smtprelay.dlr.de"
env["TEMP"]=env["TMP"]=r"D:\Delphi\texttesttmp"
nightlyDir=r"O:\Daten\Sumo\Nightly"
compiler=r"D:\Programme\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe"
svnrev=""
for platform in ["Win32", "x64"]:
    env["FILEPREFIX"]="msvc10" + options.suffix + platform
    prefix = os.path.join(options.remoteDir, env["FILEPREFIX"])
    makeLog = prefix + "Release.log"
    makeAllLog = prefix + "Debug.log"
    statusLog = prefix + "status.log"
    testLog = prefix + "test.log"
    env["SUMO_BATCH_RESULT"] = os.path.join(options.rootDir, env["FILEPREFIX"]+"batch_result")
    env["SUMO_REPORT"] = prefix + "report"
    binaryZip = os.path.join(nightlyDir, "sumo-%s-svn.zip" % env["FILEPREFIX"])
    binDir = "sumo-svn/bin/"

    for f in [makeLog, makeAllLog, binaryZip] + glob.glob(os.path.join(options.rootDir, options.binDir, "*.exe")):
        try:
            os.remove(f)
        except WindowsError:
            pass
    if platform == "Win32":
        with open(makeLog, 'w') as log:
            subprocess.call("svn.exe up %s\\trunk" % options.rootDir, stdout=log, stderr=subprocess.STDOUT)
        update_log = open(makeLog).read()
        match_rev = re.search('At revision (\d*)\.', update_log)
        if match_rev:
            svnrev = match_rev.group(1)
        else:
            open(makeLog, 'a').write("Error parsing svn revision\n")
            sys.exit()
        end_marker = 'Fetching external'        
        if end_marker in update_log:
            update_lines = len(update_log[:update_log.index(end_marker)].splitlines())
        else:
            open(makeLog, 'a').write("Error parsing svn output\n")
            sys.exit()

        if update_lines < 3 and not options.force:
            open(makeLog, 'a').write("No changes since last update, skipping build and test\n")
            print "No changes since last update, skipping build and test"
            sys.exit()

    subprocess.call(compiler+" /rebuild Release|%s %s\\%s /out %s" % (platform, options.rootDir, options.project, makeLog))
    if options.addConf:
        subprocess.call(compiler+" /rebuild %sRelease|%s %s\\%s /out %s" % (options.addConf, platform, options.rootDir, options.project, makeLog))
    programSuffix = envSuffix = ""
    if platform == "x64":
        envSuffix="_64"
        programSuffix="64"
    # we need to use io.open here due to http://bugs.python.org/issue16273
    log = io.open(makeLog, 'a')
    try:
        maxTime = 0
        for fname in glob.glob(os.path.join(nightlyDir, "sumo-src-*.zip")):
            if os.path.getmtime(fname) > maxTime:
                maxTime = os.path.getmtime(fname)
                maxFile = fname
        if maxTime > 0:
            binaryZip = maxFile.replace("-src-", "-%s-" % env["FILEPREFIX"])
            zipf = zipfile.ZipFile(binaryZip, 'w', zipfile.ZIP_DEFLATED)
            srcZip = zipfile.ZipFile(maxFile)
            write = False
            for f in srcZip.namelist():
                if f.count('/') == 1:
                    write = False
                if f.endswith('/') and f.count('/') == 2:
                    write = (f.endswith('/bin/') or f.endswith('/examples/') or f.endswith('/tools/'))
                    if f.endswith('/bin/'):
                        binDir = f
                elif write or os.path.basename(f) in ["COPYING", "README"]:
                    zipf.writestr(f, srcZip.read(f))
            srcZip.close()
        else:
            zipf = zipfile.ZipFile(binaryZip, 'w', zipfile.ZIP_DEFLATED)
        if os.path.exists(maxFile.replace("-src-", "-doc-")):
            docZip = zipfile.ZipFile(maxFile.replace("-src-", "-doc-"))
            for f in docZip.namelist():
                if not "/doxygen/" in f:
                    zipf.writestr(f, docZip.read(f))
            docZip.close()
        files_to_zip = (
                glob.glob(os.path.join(env["XERCES"+envSuffix], "bin", "xerces-c_?_?.dll")) +
                glob.glob(os.path.join(env["PROJ_GDAL"+envSuffix], "bin", "*.dll")) +
                glob.glob(os.path.join(env["FOX16"+envSuffix], "lib",
                                       "FOXDLL-1.6.dll")) +
                glob.glob(os.path.join(env["FOX16"+envSuffix], "lib",
                                       "libpng*.dll")) +
                glob.glob(os.path.join(nightlyDir, "msvc?100.dll")) +
                glob.glob(os.path.join(options.rootDir, options.binDir, "*.exe")) +
                glob.glob(os.path.join(options.rootDir, options.binDir, "*.jar")) +
                glob.glob(os.path.join(options.rootDir, options.binDir, "*.bat")))
        for f in files_to_zip:
            zipf.write(f, os.path.join(binDir, os.path.basename(f)))
            if platform == "Win32" and not f.startswith(nightlyDir):
                try:
                    shutil.copy2(f, nightlyDir)
                except IOError, (errno, strerror):
                    print >> log, "Warning: Could not copy %s to %s!" % (f, nightlyDir)
                    print >> log, "I/O error(%s): %s" % (errno, strerror)
        zipf.close()
        wix.buildMSI(binaryZip, binaryZip.replace(".zip", ".msi"), platformSuffix=programSuffix)
        shutil.copy2(binaryZip, options.remoteDir)
    except IOError, (errno, strerror):
        print >> log, "Warning: Could not zip to %s!" % binaryZip
        print >> log, "I/O error(%s): %s" % (errno, strerror)
    if platform == "Win32" and options.sumoExe == "sumo":
        try:
            setup = os.path.join(os.path.dirname(__file__), '..', 'game', 'setup.py')
            subprocess.call(['python', setup], stdout=log, stderr=subprocess.STDOUT)
        except Exception as e:
            print >> log, "Warning: Could not create nightly sumogame.zip! (%s)" % e
    log.close()
    subprocess.call(compiler+" /rebuild Debug|%s %s\\%s /out %s" % (platform, options.rootDir, options.project, makeAllLog))
    if options.addConf:
        subprocess.call(compiler+" /rebuild %sDebug|%s %s\\%s /out %s" % (options.addConf, platform, options.rootDir, options.project, makeAllLog))

# run tests
    env["TEXTTEST_TMP"] = os.path.join(options.rootDir, env["FILEPREFIX"]+"texttesttmp")
    env["TEXTTEST_HOME"] = os.path.join(options.rootDir, options.testsDir)
    if "SUMO_HOME" not in env:
        env["SUMO_HOME"] = os.path.join(os.path.dirname(__file__), '..', '..')
    shutil.rmtree(env["TEXTTEST_TMP"], True)
    shutil.rmtree(env["SUMO_REPORT"], True)
    os.mkdir(env["SUMO_REPORT"])
    for name in ["dfrouter", "duarouter", "jtrrouter", "netconvert", "netgenerate", "od2trips", "sumo", "polyconvert", "sumo-gui", "activitygen"]:
        binary = os.path.join(options.rootDir, options.binDir, name + programSuffix + ".exe")
        if name == "sumo-gui":
            if os.path.exists(binary):
                env["GUISIM_BINARY"] = binary
        elif os.path.exists(binary):
            env[name.upper()+"_BINARY"] = binary
    log = open(testLog, 'w')
    # provide more information than just the date:
    nameopt = " -name %sr%s" % (date.today().strftime("%d%b%y"), svnrev)
    if options.sumoExe == "meso":
        runInternalTests.runInternal(programSuffix, "-b "+env["FILEPREFIX"]+nameopt, log)
    else:
        subprocess.call("texttest.py -b "+env["FILEPREFIX"]+nameopt, stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call("texttest.py -a sumo.gui -b "+env["FILEPREFIX"]+nameopt, stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call("texttest.py -b "+env["FILEPREFIX"]+" -coll", stdout=log, stderr=subprocess.STDOUT, shell=True)
#    ago = datetime.datetime.now() - datetime.timedelta(30)
#    subprocess.call('texttest.py -s "batch.ArchiveRepository session='+env["FILEPREFIX"]+' before=%s"' % ago.strftime("%d%b%Y"),
#                    stdout=log, stderr=subprocess.STDOUT, shell=True)
    log.close()
    log = open(statusLog, 'w')
    status.printStatus(makeLog, makeAllLog, env["TEXTTEST_TMP"], env["SMTP_SERVER"], log)
    log.close()
