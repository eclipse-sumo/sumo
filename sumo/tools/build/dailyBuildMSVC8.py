#!/usr/bin/env python
from __future__ import with_statement
import re
from datetime import date
import optparse, os, glob, subprocess, zipfile, shutil, datetime, sys
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', 'xml'))
import status, schemaCheck

optParser = optparse.OptionParser()
optParser.add_option("-r", "--root-dir", dest="rootDir",
                     default="D:\\Sumo", help="root for svn and log output")
optParser.add_option("-s", "--suffix", default="", help="suffix to the fileprefix")
optParser.add_option("-p", "--project", default="trunk\\sumo\\build\\msvc8\\prj.sln",
                     help="path to project solution relative to the root dir")
optParser.add_option("-b", "--bin-dir", dest="binDir", default="trunk\\sumo\\bin",
                     help="directory containg the binaries, relative to the root dir")
optParser.add_option("-t", "--tests-dir", dest="testsDir", default="trunk\\sumo\\tests",
                     help="directory containg the tests, relative to the root dir")
optParser.add_option("-e", "--sumo-exe", dest="sumoExe", default="sumo",
                     help="name of the sumo executable")
optParser.add_option("-m", "--remote-dir", dest="remoteDir",
                     help="directory to move the results to")
(options, args) = optParser.parse_args()

env = os.environ
env["SMTP_SERVER"]="smtprelay.dlr.de"
nightlyDir="M:\\Daten\\Sumo\\Nightly"
compiler="D:\\Programme\\Microsoft Visual Studio 8\\Common7\\IDE\\devenv.exe"
for platform in ["Win32", "x64"]:
    env["FILEPREFIX"]="msvc8" + options.suffix + platform
    prefix = os.path.join(options.rootDir, env["FILEPREFIX"])
    if options.remoteDir:
        prefix = os.path.join(options.remoteDir, env["FILEPREFIX"])
    makeLog = prefix + "Release.log"
    makeAllLog = prefix + "Debug.log"
    statusLog = prefix + "status.log"
    testLog = prefix + "test.log"
    xmlLog = prefix + "xml.log"
    env["SUMO_BATCH_RESULT"] = os.path.join(options.rootDir, env["FILEPREFIX"]+"batch_result")
    env["SUMO_REPORT"] = prefix + "report"
    binaryZip = os.path.join(nightlyDir, "sumo-%s-bin.zip" % env["FILEPREFIX"])

    for f in [makeLog, makeAllLog, binaryZip] + glob.glob(os.path.join(options.rootDir, options.binDir, "*.exe")):
        try:
            os.remove(f)
        except WindowsError:
            pass
    if platform == "Win32":
        with open(makeLog, 'w') as log:
            subprocess.call("svn.exe up %s\\trunk" % options.rootDir, stdout=log, stderr=subprocess.STDOUT)
        match_update = re.search('Updated .*to revision (\d*)\.', open(makeLog).read())
        if match_update:
            svnrev = match_update.group(1)
        else:
            print "No changes since last update, skipping build and test"
            sys.exit()
    subprocess.call(compiler+" /rebuild Release|%s %s\\%s /out %s" % (platform, options.rootDir, options.project, makeLog))
    programSuffix = envSuffix = ""
    if platform == "x64":
        envSuffix="_64"
        programSuffix="64"
    log = open(makeLog, 'a')
    try:
        zipf = zipfile.ZipFile(binaryZip, 'w', zipfile.ZIP_DEFLATED)
        for f in [env["PROJ_GDAL"+envSuffix]+"\\bin\\proj.dll", env["PROJ_GDAL"+envSuffix]+"\\bin\\gdal16.dll",
                  env["XERCES"+envSuffix]+"\\bin\\xerces-c_3_0.dll",
                  env["FOX16"+envSuffix]+"\\lib\\FOXDLL-1.6.dll"] + glob.glob(os.path.join(options.rootDir, options.binDir, "*.exe")):
            zipf.write(f, os.path.basename(f))
            if platform == "Win32":
                try:
                    shutil.copy2(f, nightlyDir)
                except IOError, (errno, strerror):
                    print >> log, "Warning: Could not copy %s to %s!" % (f, nightlyDir)
                    print >> log, "I/O error(%s): %s" % (errno, strerror)
        zipf.close()
    except IOError, (errno, strerror):
        print >> log, "Warning: Could not zip to %s!" % binaryZip
        print >> log, "I/O error(%s): %s" % (errno, strerror)
    log.close()
    subprocess.call(compiler+" /rebuild Debug|%s %s\\%s /out %s" % (platform, options.rootDir, options.project, makeAllLog))

# run tests
    env["TEXTTEST_TMP"] = os.path.join(options.rootDir, env["FILEPREFIX"]+"texttesttmp")
    env["TEXTTEST_HOME"] = os.path.join(options.rootDir, options.testsDir)
    shutil.rmtree(env["TEXTTEST_TMP"], True)
    shutil.rmtree(env["SUMO_REPORT"], True)
    os.mkdir(env["SUMO_REPORT"])
    for name in ["dfrouter", "duarouter", "jtrrouter", "netconvert", "netgen", "od2trips", "sumo", "polyconvert", "sumo-gui", "activitygen"]:
        binary = os.path.join(options.rootDir, options.binDir, name + programSuffix + ".exe")
        if name == "sumo":
            binary = os.path.join(options.rootDir, options.binDir, options.sumoExe + programSuffix + ".exe")
        if name == "sumo-gui":
            if options.sumoExe == "meso":
                binary = os.path.join(options.rootDir, options.binDir, "meso-gui" + programSuffix + ".exe")
            if os.path.exists(binary):
                env["GUISIM_BINARY"] = binary
        elif os.path.exists(binary):
            env[name.upper()+"_BINARY"] = binary
    log = open(testLog, 'w')
    # provide more information than just the date:
    nameopt = " -name %sr%s" % (date.today().strftime("%d%b%y"), svnrev)
    subprocess.call("texttest.py -b "+env["FILEPREFIX"]+nameopt, stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call("texttest.py -a sumo.gui -b "+env["FILEPREFIX"], stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call("texttest.py -b "+env["FILEPREFIX"]+" -coll", stdout=log, stderr=subprocess.STDOUT, shell=True)
    ago = datetime.datetime.now() - datetime.timedelta(30)
    subprocess.call('texttest.py -s "batch.ArchiveRepository session='+env["FILEPREFIX"]+' before=%s"' % ago.strftime("%d%b%Y"),
                    stdout=log, stderr=subprocess.STDOUT, shell=True)
    log.close()
    log = open(xmlLog, 'w')
    schemaCheck.main(env["TEXTTEST_HOME"], log)
    log.close()
    log = open(statusLog, 'w')
    status.printStatus(makeLog, makeAllLog, env["TEXTTEST_TMP"], env["SMTP_SERVER"], log)
    log.close()
    if not options.remoteDir:
        toPut = " ".join([env["SUMO_REPORT"], makeLog, makeAllLog, testLog, xmlLog, statusLog, binaryZip])
        subprocess.call('WinSCP3.com behrisch,sumo@web.sourceforge.net /privatekey=%s\\key.ppk /command "option batch on" "option confirm off" "put %s /home/groups/s/su/sumo/htdocs/daily/" "exit"' % (options.rootDir, toPut))
