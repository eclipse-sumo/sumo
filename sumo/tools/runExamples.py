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
	if(file.endswith(".sumo.cfg")):
		print "--------------------------"
		print "Executing " + file
		print "--------------------------"
		ret = os.system("..\\bin\\sumo -v -c " + file)
