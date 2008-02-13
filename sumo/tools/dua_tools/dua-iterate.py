from optparse import OptionParser
import os
import sys
import time


def writeRouteConf(step, options, file, output):
	fd = open("iteration_" + str(step) + ".rou.cfg", "w")
	fd.write("<configuration>\n")
	fd.write("<move-on-short>x</move-on-short>\n") # !!!
	fd.write("<remove-loops>x</remove-loops>\n") # !!!
	fd.write("   <files>\n")
	fd.write("      <net-file>" + options.net + "</net-file>\n")
	fd.write("      <output>" + output + "</output>\n")
	if(step==0):
		fd.write("      <t>" + file + "</t>\n")
	else:
		fd.write("      <alternatives>" + file + "</alternatives>\n")
		fd.write("      <weights>dump_" + str(step-1) + "_" + str(options.aggregation) + ".xml</weights>\n")
	fd.write("   </files>\n")
	fd.write("   <process>\n")
	fd.write("      <begin>" + str(options.begin) + "</begin>\n")
	fd.write("      <end>" + str(options.end) + "</end>\n")
	fd.write("   </process>\n")
	fd.write("   <reports>\n")
	if options.verbose:
		fd.write("      <verbose>x</verbose>\n")
	if options.continueOnUnbuild:
		fd.write("      <continue-on-unbuild>x</continue-on-unbuild>\n")
	if not options.withWarnings:
		fd.write("      <suppress-warnings>x</suppress-warnings>\n")
	fd.write("   </reports>\n")
	fd.write("</configuration>\n")
	fd.close()

def writeSUMOConf(step, options, files):
	fd = open("iteration_" + str(step) + ".sumo.cfg", "w")
	fd.write("<configuration>\n")
	fd.write("   <files>\n")
	fd.write("      <net-file>" + options.net + "</net-file>\n")
	fd.write("      <route-files>" + files + "</route-files>\n")
	fd.write("      <dump-basename>dump_" + str(step) + "</dump-basename>\n")
	fd.write("      <dump-intervals>" + str(options.aggregation) + "</dump-intervals>\n")
	if not options.noEmissions:
		fd.write("      <emissions>emissions_" + str(step) + ".xml</emissions>\n")
	if not options.noTripinfo:
		fd.write("      <tripinfo>tripinfo_" + str(step) + ".xml</tripinfo>\n")
	if options.additional!="":
		fd.write("      <additional-files>" + options.additional + "</additional-files>\n")
	fd.write("   </files>\n")
	fd.write("   <process>\n")
	fd.write("      <begin>" + str(options.begin) + "</begin>\n")
	fd.write("      <end>" + str(options.end) + "</end>\n")
	fd.write("      <route-steps>" + str(options.routeSteps) + "</route-steps>\n")
	if options.incBase>0:
		fd.write("      <incremental-dua-base>" + str(options.incBase) + "</incremental-dua-base>\n")
		fd.write("      <incremental-dua-step>" + str(options.incValue*(step+1)) + "</incremental-dua-step>\n")
	if options.mesosim:
		fd.write("      <mesosim>x</mesosim>\n")
	fd.write("   </process>\n")
	fd.write("   <reports>\n")
	if options.verbose:
		fd.write("      <verbose>x</verbose>\n")
	if not options.withWarnings:
		fd.write("      <suppress-warnings>x</suppress-warnings>\n")
	fd.write("   </reports>\n")
	fd.write("</configuration>\n")
	fd.close()

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-C", "--continue-on-unbuild", action="store_true", dest="continueOnUnbuild",
                     default=False, help="continues on unbuild routes")
optParser.add_option("-W", "--with-warnings", action="store_true", dest="withWarnings",
                     default=False, help="enables warnings")

optParser.add_option("-n", "--net-file", dest="net",
                     help="SUMO network (mandatory)", metavar="FILE")
optParser.add_option("-t", "--trips", dest="trips",
                     help="trips in step 0", metavar="FILE")

optParser.add_option("-b", "--begin", dest="begin",
                     type="int", default=0, help="Set simulation/routing begin")
optParser.add_option("-e", "--end", dest="end",
                     type="int", default=86400, help="Set simulation/routing end")
optParser.add_option("-R", "--route-steps", dest="routeSteps",
                     type="int", default=200, help="Set simulation route steps")
optParser.add_option("-a", "--aggregation", dest="aggregation",
                     type="int", default=900, help="Set main weights aggregation period")
optParser.add_option("-A", "--gA", dest="gA",
                     type="float", default=.5, help="Sets Gawron's Alpha")
optParser.add_option("-B", "--gBeta", dest="gBeta",
                     type="float", default=.9, help="Sets Gawron's Beta")

optParser.add_option("-E", "--disable-emissions", dest="noEmissions",
                     default=False, help="No emissions are written by the simulation")
optParser.add_option("-T", "--disable-tripinfos", dest="noTripinfo",
                     default=False, help="No tripinfos are written by the simulation")
optParser.add_option("-m", "--mesosim", dest="mesosim",
                     default=False, help="Whether mesosim shall be used")
optParser.add_option("--inc-base", dest="incBase",
                     type="int", default=-1, help="Give the incrementation base")
optParser.add_option("--incrementation", dest="incValue",
                     type="int", default=1, help="Give the incrementation")
optParser.add_option("-+", "--additional", dest="additional",
                     default="", help="Additional files")


optParser.add_option("-f", "--first-step", dest="firstStep",
                     type="int", default=0, help="First DUA step")
optParser.add_option("-l", "--last-step", dest="lastStep",
                     type="int", default=50, help="Last DUA step")
optParser.add_option("-p", "--path", dest="path",
                     default="%SUMO%\\", help="Path to binaries")
(options, args) = optParser.parse_args()


fdm = open("dua-log.txt", "w")
fds = open("dua-log-quiet.txt", "w")
joiner = ","
tripFiles = options.trips.split(",")
for step in range(options.firstStep, options.lastStep):
	btimeA = time.time()
	print "> Executing step " + str(step)
	fds.write("> Executing step " + str(step)+ "\n")

	# router
	files = []
	for tripFile in tripFiles:
		file = tripFile
		if step>0:
			file = tripFile[:tripFile.find(".")] + "_" + str(step-1) + ".rou.xml.alt"
		output = tripFile[:tripFile.find(".")] + "_" + str(step) + ".rou.xml"
		print ">> Running router with " + file
		btime = time.localtime()
		btime2 = time.time()
		fds.write(">> Running router with " + file + "\n")
		print ">>> Begin time " + time.asctime(btime) + " (" + str(btime2)  + ")"
		fds.write(">>> Begin time " + time.asctime(btime) + " (" + str(btime2) + ")\n")
		writeRouteConf(step, options, file, output)
		if options.verbose:
			print "> Call: " + options.path + "duarouter -c " + "iteration_" + str(step) + ".rou.cfg"
		if(sys.platform=="win32"):		
			(cin, cout) = os.popen4(options.path + "duarouter -c " + "iteration_" + str(step) + ".rou.cfg")
		else:
			(cin, cout) = os.popen4(options.path + "sumo-duarouter -c " + "iteration_" + str(step) + ".rou.cfg")
		line = cout.readline()
		while line:
			if options.verbose:
				print line[:-1]
			fdm.write(line)
			line = cout.readline()
		etime = time.localtime()
		etime2 = time.time()
		print ">>> End time " + time.asctime(etime) + " (" + str(etime2) + ")"
		fds.write(">>> End time " + time.asctime(etime) + " (" + str(etime2) + ")\n")
		print ">>> Duration " + str(etime2-btime2)
		fds.write(">>> Duration " + str(etime2-btime2) + "\n")
		print "<<"
		fds.write("<<\n")
		etime = time.localtime()
		file = file[:file.find(".")] + "_" + str(step) + ".rou.xml"
		files.append(output)

	# simulation
	print ">> Running simulation"
	btime = time.localtime()
	btime2 = time.time()
	fds.write(">> Running simulation\n")
	print ">>> Begin time " + time.asctime(btime) + " (" + str(btime2) + ")"
	fds.write(">>> Begin time " + time.asctime(btime) + " (" + str(btime2) + ")\n")
	writeSUMOConf(step, options, joiner.join(files))
	if options.verbose:
		print "> Call: " + options.path + "sumo -c " + "iteration_" + str(step) + ".sumo.cfg"
	(cin, cout) = os.popen4(options.path + "sumo -c " + "iteration_" + str(step) + ".sumo.cfg")
	line = cout.readline()
	while line:
		if options.verbose:
			print line[:-1]
		fdm.write(line)
		line = cout.readline()
	etime = time.localtime()
	etime2 = time.time()
	print ">>> End time " + time.asctime(etime) + " (" + str(etime2) + ")"
	fds.write(">>> End time " + time.asctime(etime) + " (" + str(etime2) + ")\n")
	print ">>> Duration " + str(etime2-btime2)
	fds.write(">>> Duration " + str(etime2-btime2) + "\n")
	print "<<"

	btimeA = time.time()
	print "< Step " + str(step) + " ended (duration: " + str(time.time() - btimeA)
	fds.write("< Step " + str(step) + " ended (duration: " + str(time.time() - btimeA) + "\n")
	print "------------------\n"
	fds.write("------------------\n\n")

fdm.close()
fds.close()

