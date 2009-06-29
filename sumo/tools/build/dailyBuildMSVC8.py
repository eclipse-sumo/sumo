import os, glob, subprocess, zipfile, shutil, datetime

env = os.environ
for platform in ["Win32", "x64"]:
    rootDir="D:\\Sumo"
    env["FILEPREFIX"]="msvc8" + platform
    makeLog=rootDir+"\\"+env["FILEPREFIX"]+"Release.log"
    makeAllLog=rootDir+"\\"+env["FILEPREFIX"]+"Debug.log"
    statusLog=rootDir+"\\"+env["FILEPREFIX"]+"status.log"
    testLog=rootDir+"\\"+env["FILEPREFIX"]+"test.log"
    env["SUMO_BATCH_RESULT"]=rootDir+"\\"+env["FILEPREFIX"]+"batch_result"
    env["SUMO_REPORT"]=rootDir+"\\"+env["FILEPREFIX"]+"report"
    env["SMTP_SERVER"]="129.247.218.247"
    nightlyDir="N:\\Daten\\Sumo\\Nightly"
    binaryZip=nightlyDir+"\\sumo-"+env["FILEPREFIX"]+"-bin.zip"
    compiler="D:\\Programme\\Microsoft Visual Studio 8\\Common7\\IDE\\devenv.exe"

    os.chdir("..\\..")
    for f in [makeLog, makeAllLog, binaryZip] + glob.glob("bin\\*.exe"):
        try:
            os.remove(f)
        except WindowsError:
            pass
    log = open(makeLog, 'w')
    subprocess.call("svn.exe up", stdout=log, stderr=subprocess.STDOUT)
    log.close()    
    subprocess.call(compiler+" /rebuild Release|%s build\\msvc8\\prj.sln /out %s" % (platform, makeLog))
    programSuffix = suffix = ""
    if platform == "x64":
        suffix="_64"
        programSuffix="64"
    zipf = zipfile.ZipFile(binaryZip, 'w', zipfile.ZIP_DEFLATED)
    for f in [env["PROJ_GDAL"+suffix]+"\\bin\\proj.dll", env["PROJ_GDAL"+suffix]+"\\bin\\gdal16.dll",
              env["XERCES"+suffix]+"\\bin\\xerces-c_3_0.dll",
              env["FOX16"+suffix]+"\\lib\\FOXDLL-1.6.dll"] + glob.glob("bin\\*.exe"):
        zipf.write(f, os.path.basename(f))
        if platform == "Win32":
            shutil.copy(f, nightlyDir)
    zipf.close()
    subprocess.call(compiler+" /rebuild Debug|%s build\\msvc8\\prj.sln /out %s" % (platform, makeAllLog))

# run tests
    os.chdir("tests")
    env["TEXTTEST_TMP"] = rootDir + "\\" + env["FILEPREFIX"]+"texttesttmp"
    env["TEXTTEST_HOME"] = os.getcwd()
    shutil.rmtree(env["TEXTTEST_TMP"], True)
    shutil.rmtree(env["SUMO_REPORT"], True)
    os.mkdir(env["SUMO_REPORT"])
    for name in ["dfrouter", "duarouter", "jtrrouter", "netconvert", "netgen", "od2trips", "sumo", "polyconvert", "guisim"]:
        binary = os.path.join(os.getcwd(), '..', 'bin', name + programSuffix + ".exe")
        if os.path.exists(binary):
            env[name.upper()+"_BINARY"] = binary
    log = open(testLog, 'w')
    subprocess.call("runGuisimTests.py -b", stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call("texttest.py -b "+env["FILEPREFIX"], stdout=log, stderr=subprocess.STDOUT, shell=True)
    subprocess.call("texttest.py -b "+env["FILEPREFIX"]+" -coll", stdout=log, stderr=subprocess.STDOUT, shell=True)
    ago = datetime.datetime.now() - datetime.timedelta(30)
    subprocess.call('texttest.py -s "batch.ArchiveRepository session='+env["FILEPREFIX"]+' before=%s"' % ago.strftime("%d%b%Y"),
                    stdout=log, stderr=subprocess.STDOUT, shell=True)
    log.close()
    log = open(statusLog, 'w')
    os.chdir("..")
    subprocess.call("tools\\build\\status.py %s %s %s %s" % (makeLog, makeAllLog, env["TEXTTEST_TMP"], env["SMTP_SERVER"]),
                    stdout=log, shell=True)
    log.close()
    subprocess.call('WinSCP3.com behrisch,sumo@web.sourceforge.net /privatekey=%s\\key.ppk /command "option batch on" "option confirm off" "put %s %s %s %s %s /home/groups/s/su/sumo/htdocs/daily/" "exit"' % (rootDir, env["SUMO_REPORT"], makeLog, makeAllLog, statusLog, binaryZip))
    os.chdir("tools\\build")
