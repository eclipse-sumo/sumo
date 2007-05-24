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

files = getStructure("../../src/")
for file in files:
	if(file.endswith(".h") or file.endswith(".cpp")):
		file = file.replace("/", "\\")
		os.system("AStyle.exe --style=kr -U -l " + file)
		

