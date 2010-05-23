from distutils.core import setup
import py2exe, sys, shutil, os, glob, zipfile

nightlyDir="N:\\Daten\\Sumo\\Nightly"

if len(sys.argv) == 1:
    sys.argv.append("py2exe")

#setup(console=['runner.py'])

base = os.path.dirname(sys.argv[0])
dist = os.path.join(base, "dist")
for f in glob.glob(os.path.join(base, "square.*")) + ["cross.sumo.cfg", "dlr.gif", "input_additional.add.xml",
                                                      "input_additional.add.xml", "settings.xml"]:
    shutil.copy2(f, dist)
shutil.copytree("cross", os.path.join(dist, "cross"))
for dll in glob.glob(os.path.join(nightlyDir, "*.dll")):
    shutil.copy2(dll, dist)
shutil.copy2(os.path.join(nightlyDir, "sumo-gui.exe"), dist)

zipf = zipfile.ZipFile("sumogame.zip", 'w', zipfile.ZIP_DEFLATED)
root_len = len(os.path.abspath(dist))
for root, dirs, files in os.walk(dist):
    archive_root = os.path.abspath(root)[root_len:]
    for f in files:
        fullpath = os.path.join(root, f)
        archive_name = os.path.join(archive_root, f)
        zipf.write(fullpath, archive_name)
zipf.close()
