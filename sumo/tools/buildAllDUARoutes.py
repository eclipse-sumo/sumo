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
files = getStructure("../data/examples")
for file in files:
	if(file.endswith(".dua.cfg")):
		print "--------------------------"
		print "Executing " + file
		print "--------------------------"
		if(sys.platform=="win32"):
			ret = os.system("..\\bin\\duarouter -v -c " + file)
		else:
			ret = os.system("../src/sumo-duarouter -v -c " + file)
