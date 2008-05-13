# Builds all example networks
# $Id:
# $Log:

import os
import sys

# get all files from the given folder recursively
def getStructure(dir):
	if(dir[-1:]!='/' and dir[-1:]!='\\'):
		dir = dir + '\\'
	files = os.listdir(dir)
	newFiles = []
	for file in files:
		if os.path.isdir(dir + file)==1:
			myNewFiles = getStructure(dir + file + "\\")
			for tmp in myNewFiles:
				newFiles.append(tmp)
		else:
			newFiles.append( dir + file )
	return newFiles

# main
files = getStructure("./dfrouter")
files.extend(getStructure("./duarouter"))
for file in files:
    if(file.endswith(".netc.cfg")):
        print "----------------------------------"
        print "Runnning: " + file
        if(sys.platform=="win32"):
            (cin, cout) = os.popen4("..\\bin\\netconvert -v -c " + file)
        else:
            (cin, cout) = os.popen4("../src/sumo-netconvert -v -c " + file)
        line = cout.readline()
        while line:
             print line[:-1]
             line = cout.readline()
        print "----------------------------------\n"
