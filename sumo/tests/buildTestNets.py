# Builds all example networks
# $Id:
# $Log:

import os
import sys

# main
mRoot = "./"
if len(sys.argv)>1:
	mRoot = sys.argv[1]
for root, dirs, files in os.walk(mRoot):
    for ignoreDir in ['.svn', 'foreign']:
        if ignoreDir in dirs:
            dirs.remove(ignoreDir)

    for file in files:
        if(file.endswith(".netc.cfg")):
            print "----------------------------------"
            print "Runnning: " + file
            if(sys.platform=="win32"):
                (cin, cout) = os.popen4("..\\bin\\netconvert -v -c " + os.path.join(root, file))
            else:
                (cin, cout) = os.popen4("../src/sumo-netconvert -v -c " + os.path.join(root, file))
            line = cout.readline()
            while line:
                 print line[:-1]
                 line = cout.readline()
            print "----------------------------------\n"
        if(file.endswith(".netg.cfg")):
            print "----------------------------------"
            print "Runnning: " + file
            if(sys.platform=="win32"):
                (cin, cout) = os.popen4("..\\bin\\netgen -v -c " + os.path.join(root, file))
            else:
                (cin, cout) = os.popen4("../src/sumo-netgen -v -c " + os.path.join(root, file))
            line = cout.readline()
            while line:
                 print line[:-1]
                 line = cout.readline()
            print "----------------------------------\n"
