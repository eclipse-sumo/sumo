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
print "Performing additional tests in 'tests'..."
files = getStructure("./")
print " Checking TLS for invalid combinations"
for file in files:
    if(file == "net.netconvert.exe" or file.endswith(".net.xml")):
        os.system("checkTLS.py " + file)
