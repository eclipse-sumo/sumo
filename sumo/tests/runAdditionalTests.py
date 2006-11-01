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

def performInList(files):
	print " Checking TLS for invalid combinations"
	for file in files:
		if(file == "net.netconvert.exe" or file.endswith(".net.xml")):
			os.system("checkTLS.py " + file)

def checkSame(files):
	print " Checking for files being the same"
	content1 = ""
	for file in files:
		if content1=="":
			fd = open(file)
			content1 = fd.readlines()
			fd.close()
		else:
			fd = open(file)
			content = fd.readlines()
			if(content != content1):
				print "  Files '" + files[0] + "' and '" + file + "' differ!"
			fd.close()

# main
print "Performing additional tests in 'tests'..."
performInList(getStructure("./"))
print "Performing additional tests in 'data'..."
performInList(getStructure("../data/"))

# checks that the same outpout is generated
checkSame(("netconvert/XML2/types/cross_notypes/net.netconvert.exe", "netconvert/XML2/types/cross_notypes_v/net.netconvert.exe", "netconvert/XML2/types/cross_usingtypes/net.netconvert.exe", "netconvert/XML2/types/cross_usingtypes_v/net.netconvert.exe"))
checkSame(("netconvert/XML2/speed_in_kmh/cross_notypes_kmh/net.netconvert.exe", "netconvert/XML2/speed_in_kmh/cross_notypes_kmh_v/net.netconvert.exe", "netconvert/XML2/speed_in_kmh/cross_usingtypes_kmh/net.netconvert.exe", "netconvert/XML2/speed_in_kmh/cross_usingtypes_kmh_v/net.netconvert.exe"))

