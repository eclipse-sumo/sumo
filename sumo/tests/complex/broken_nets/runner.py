#!/usr/bin/env python

import os,subprocess,sys,time
import xml.dom.minidom as dom
from copy import copy

changes = [
	[ "net/edge[0]@from", "" ],
	[ "net/edge[1]@from", "" ],
	[ "net/edge[0]@from", "a" ],
	[ "net/edge[1]@from", "a" ],
	[ "net/edge[0]@from", "<remove>" ],
	[ "net/edge[1]@from", "<remove>" ],
	[ "net/edge[0]@function", "" ],
	[ "net/edge[1]@function", "" ],
	[ "net/edge[0]@function", "a" ],
	[ "net/edge[1]@function", "a" ],
	[ "net/edge[0]@function", "<remove>" ],
	[ "net/edge[1]@function", "<remove>" ],
	[ "net/edge[0]@id", "" ],
	[ "net/edge[1]@id", "" ],
	[ "net/edge[0]@id", "a" ],
	[ "net/edge[1]@id", "a" ],
	[ "net/edge[0]@id", "<remove>" ],
	[ "net/edge[1]@id", "<remove>" ],
	[ "net/edge[0]@to", "" ],
	[ "net/edge[1]@to", "" ],
	[ "net/edge[0]@to", "a" ],
	[ "net/edge[1]@to", "a" ],
	[ "net/edge[0]@to", "<remove>" ],
	[ "net/edge[1]@to", "<remove>" ],

	[ "net/edge[0]/lane[0]@depart", "" ],
	[ "net/edge[0]/lane[1]@depart", "" ],
	[ "net/edge[1]/lane[0]@depart", "" ],
	[ "net/edge[1]/lane[1]@depart", "" ],
	[ "net/edge[0]/lane[0]@depart", "a" ],
	[ "net/edge[0]/lane[1]@depart", "a" ],
	[ "net/edge[1]/lane[0]@depart", "a" ],
	[ "net/edge[1]/lane[1]@depart", "a" ],
	[ "net/edge[0]/lane[0]@depart", "<remove>" ],
	[ "net/edge[0]/lane[1]@depart", "<remove>" ],
	[ "net/edge[1]/lane[0]@depart", "<remove>" ],
	[ "net/edge[1]/lane[1]@depart", "<remove>" ],
	[ "net/edge[0]/lane[0]@id", "" ],
	[ "net/edge[0]/lane[1]@id", "" ],
	[ "net/edge[1]/lane[0]@id", "" ],
	[ "net/edge[1]/lane[1]@id", "" ],
	[ "net/edge[0]/lane[0]@id", "a_0" ],
	[ "net/edge[0]/lane[1]@id", "a_1" ],
	[ "net/edge[1]/lane[0]@id", "a_0" ],
	[ "net/edge[1]/lane[1]@id", "a_1" ],
	[ "net/edge[0]/lane[0]@id", "<remove>" ],
	[ "net/edge[0]/lane[1]@id", "<remove>" ],
	[ "net/edge[1]/lane[0]@id", "<remove>" ],
	[ "net/edge[1]/lane[1]@id", "<remove>" ],
	[ "net/edge[0]/lane[0]@length", "" ],
	[ "net/edge[0]/lane[1]@length", "" ],
	[ "net/edge[1]/lane[0]@length", "" ],
	[ "net/edge[1]/lane[1]@length", "" ],
	[ "net/edge[0]/lane[0]@length", "a" ],
	[ "net/edge[0]/lane[1]@length", "a" ],
	[ "net/edge[1]/lane[0]@length", "a" ],
	[ "net/edge[1]/lane[1]@length", "a" ],
	[ "net/edge[0]/lane[0]@length", "<remove>" ],
	[ "net/edge[0]/lane[1]@length", "<remove>" ],
	[ "net/edge[1]/lane[0]@length", "<remove>" ],
	[ "net/edge[1]/lane[1]@length", "<remove>" ],
	[ "net/edge[0]/lane[0]@shape", "" ],
	[ "net/edge[0]/lane[1]@shape", "" ],
	[ "net/edge[1]/lane[0]@shape", "" ],
	[ "net/edge[1]/lane[1]@shape", "" ],
	[ "net/edge[0]/lane[0]@shape", "a" ],
	[ "net/edge[0]/lane[1]@shape", "a" ],
	[ "net/edge[1]/lane[0]@shape", "a" ],
	[ "net/edge[1]/lane[1]@shape", "a" ],
	[ "net/edge[0]/lane[0]@shape", "<remove>" ],
	[ "net/edge[0]/lane[1]@shape", "<remove>" ],
	[ "net/edge[1]/lane[0]@shape", "<remove>" ],
	[ "net/edge[1]/lane[1]@shape", "<remove>" ],
	[ "net/edge[0]/lane[0]@shape", "a,-4.95 100.00,-4.95" ],
	[ "net/edge[0]/lane[1]@shape", "a,-4.95 100.00,-4.95" ],
	[ "net/edge[1]/lane[0]@shape", "a,-4.95 100.00,-4.95" ],
	[ "net/edge[1]/lane[1]@shape", "a,-4.95 100.00,-4.95" ],
	[ "net/edge[0]/lane[0]@shape", "0.00,a 100.00,-4.95" ],
	[ "net/edge[0]/lane[1]@shape", "0.00,a 100.00,-4.95" ],
	[ "net/edge[1]/lane[0]@shape", "0.00,a 100.00,-4.95" ],
	[ "net/edge[1]/lane[1]@shape", "0.00,a 100.00,-4.95" ],
	[ "net/edge[0]/lane[0]@shape", "0.00,-4.95 a,-4.95" ],
	[ "net/edge[0]/lane[1]@shape", "0.00,-4.95 a,-4.95" ],
	[ "net/edge[1]/lane[0]@shape", "0.00,-4.95 a,-4.95" ],
	[ "net/edge[1]/lane[1]@shape", "0.00,-4.95 a,-4.95" ],
	[ "net/edge[0]/lane[0]@shape", "0.00,-4.95 a 100.00,-4.95" ],
	[ "net/edge[0]/lane[1]@shape", "0.00,-4.95 a 100.00,-4.95" ],
	[ "net/edge[1]/lane[0]@shape", "0.00,-4.95 a 100.00,-4.95" ],
	[ "net/edge[1]/lane[1]@shape", "0.00,-4.95 a 100.00,-4.95" ],
	[ "net/edge[0]/lane[0]@shape", "100.00,-4.95" ],
	[ "net/edge[0]/lane[1]@shape", "0100.00,-4.95" ],
	[ "net/edge[1]/lane[0]@shape", "100.00,-4.95" ],
	[ "net/edge[1]/lane[1]@shape", "100.00,-4.95" ],

	[ "net/junction[0]@id", "" ],
	[ "net/junction[1]@id", "" ],
	[ "net/junction[2]@id", "" ],
	[ "net/junction[0]@id", "a" ],
	[ "net/junction[1]@id", "a" ],
	[ "net/junction[2]@id", "a" ],
	[ "net/junction[0]@id", "<remove>" ],
	[ "net/junction[1]@id", "<remove>" ],
	[ "net/junction[2]@id", "<remove>" ],
	[ "net/junction[0]@type", "" ],
	[ "net/junction[1]@type", "" ],
	[ "net/junction[2]@type", "" ],
	[ "net/junction[0]@type", "a" ],
	[ "net/junction[1]@type", "a" ],
	[ "net/junction[2]@type", "a" ],
	[ "net/junction[0]@type", "<remove>" ],
	[ "net/junction[1]@type", "<remove>" ],
	[ "net/junction[2]@type", "<remove>" ],
	[ "net/junction[1]@incLanes", "a_0 1_1" ],
	[ "net/junction[1]@incLanes", "1_0 a_1" ],
	[ "net/junction[2]@incLanes", "a_0 2_1" ],
	[ "net/junction[2]@incLanes", "2_0 a_1" ],
]

def tinyPath(xmlStruct, path, newValue):
	attribute = None
	if path.find("@")>=0:
		path, attribute = path.split("@")
	defs = path.split("/")
	item = xmlStruct
	for d in defs:
		if d.find("[")>=0:
			iname = d[:d.find("[")]
			index = int(d[d.find("[")+1:d.find("]")])
		else:
			iname = d
			index = 0
		items = item.getElementsByTagName(iname)
		item = items[index]
	if attribute:
		if newValue=="<remove>":
			item.removeAttribute(attribute)
		else:
			item.setAttribute(attribute, newValue)
	else:
		raise "?"

def checkBinary(binPath, name):
    binary = os.environ.get("%s_BINARY" % name.upper(), os.path.join(binPath, name))
    if os.name == "nt" and binary[-4:] != ".exe":
        binary += ".exe"
    if not os.path.exists(binary):
        return name
    return binary

call = []
binPath = os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', 'bin')
if sys.argv[1]=="sumo":
    call.append(checkBinary(binPath, 'sumo'))
    call.append("--no-step-log")
    call.append("--no-duration-log")
elif sys.argv[1]=="dfrouter":
    call.append(checkBinary(binPath, 'dfrouter'))
    call.append("--detector-files")
    call.append("input_additional.add.xml")
elif sys.argv[1]=="duarouter":
    call.append(checkBinary(binPath, 'duarouter'))
    call.append("-o")
    call.append("dummy.xml")
    call.append("-t")
    call.append("input_additional.add.xml")
elif sys.argv[1]=="jtrrouter":
    call.append(checkBinary(binPath, 'jtrrouter'))
    call.append("-o")
    call.append("dummy.xml")
    call.append("-t")
    call.append("input_additional.add.xml")
else:
    print >> sys.stderr, "Unsupported application defined"



netconvertBinary = checkBinary(binPath, 'netconvert')

# build the correct network, first
print ">>> Building the correct network"
retcode = subprocess.call([netconvertBinary, "-c", "netconvert.netc.cfg"], stdout=sys.stdout, stderr=sys.stderr)
print ">>> Trying the correct network"
call.append("-n")
call.append("net.net.xml")
retcode = subprocess.call(call, stdout=sys.stdout, stderr=sys.stderr)
if retcode!=0:
	print "Error on processing the 'correct' network!"
	sys.exit()
print ">>> ok...\n"

# check broken network processing
call = call[:-1]
call.append("mod.net.xml")
print "Running broken net"
for c in changes:
	tree = dom.parse("net.net.xml")
	tinyPath(tree, c[0], c[1])
	writer = open('mod.net.xml', 'w')
	tree.writexml(writer)
	writer.close()
	print >> sys.stderr, "------------------ " + c[0] + ":" + c[1]
	sys.stderr.flush()
	retcode = subprocess.call(call, stdout=sys.stdout, stderr=sys.stderr)
	sys.stderr.flush()
	sys.stdout.flush()
	if retcode!=1:
		print >> sys.stderr, " Wrong error code returned (" + str(retcode) + ")!"

