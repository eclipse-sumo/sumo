import xml.etree.ElementTree as ET
import xml.dom.minidom
import sys
import getopt


# parse input
opts, args = getopt.getopt(sys.argv[1:], "i:o:t:v:", ["input=", "output=", "time=", "vehicleid="])

# check arguments
if (len(opts) == 0):
    print ('usage: mergeBatteryOutput.py -i <battery input file> -o <battery merged output file> -t <time to merge> -v <vehicleid>')
    sys.exit()

# parse arguments
for opt, arg in opts:
    if opt in ("-i", "--input"):
        inputFile = arg
    elif opt in ("-o", "--output"):
        outputFile = arg
    elif opt in ("-t", "--time"):
        timeToSplit = int(arg)
    elif opt in ("-v", "--vehicleid"):
        vehicleID = arg

# declare timeStep counter
timeStepCounter = 0
currentTimeStep = 0

# declare variables for sum
energyConsumed = 0.0
totalEnergyConsumed = 0.0
totalEnergyRegenerated = 0.0
energyCharged = 0.0
energyChargedInTransit = 0.0
energyChargedStopped = 0.0
timeStopped = 0.0

# create output tree
outputRoot = ET.Element('battery-export')

# load battery outuput
tree = ET.parse(inputFile)

# iterate over all time Steps
for timeStep in tree.getroot():
    currentTimeStep = timeStep.attrib["time"]
    for vehicle in timeStep:
        if (vehicle.attrib["id"] == vehicleID):
            # update values
            energyConsumed += float(vehicle.attrib["energyConsumed"])
            totalEnergyConsumed += float(vehicle.attrib["totalEnergyConsumed"])
            totalEnergyRegenerated += float(vehicle.attrib["totalEnergyRegenerated"])
            energyCharged += float(vehicle.attrib["energyCharged"])
            energyChargedInTransit += float(vehicle.attrib["energyChargedInTransit"])
            energyChargedStopped += float(vehicle.attrib["energyChargedStopped"])
            timeStopped += float(vehicle.attrib["timeStopped"])
            # update counter
            timeStepCounter += 1
            # check timeStepCounter
            if (timeStepCounter >= timeToSplit):
                # add time value
                timestepOutput = ET.SubElement(outputRoot, 'timestep')
                timestepOutput.set("time", timeStep.attrib["time"])
                # add vehicle
                vehicleOutput = ET.SubElement(timestepOutput, 'vehicle')
                vehicleOutput.set("id", vehicle.attrib["id"])
                # write vehicle values
                vehicleOutput.set("energyConsumed", str(energyConsumed))
                vehicleOutput.set("totalEnergyConsumed", str(totalEnergyConsumed))
                vehicleOutput.set("totalEnergyRegenerated", str(totalEnergyRegenerated))
                vehicleOutput.set("energyCharged", str(energyCharged))
                vehicleOutput.set("energyChargedInTransit", str(energyChargedInTransit))
                vehicleOutput.set("energyChargedStopped", str(energyChargedStopped))
                vehicleOutput.set("timeStopped", str(timeStopped))
                # reset values
                energyConsumed = 0.0
                totalEnergyConsumed = 0.0
                totalEnergyRegenerated = 0.0
                energyCharged = 0.0
                energyChargedInTransit = 0.0
                energyChargedStopped = 0.0
                timeStopped = 0.0
                # reset timeStepCounter
                timeStepCounter = 0
# write last
if (timeStepCounter > 0):
    # add time value
    timestepOutput = ET.SubElement(outputRoot, 'timestep')
    timestepOutput.set("time", currentTimeStep)
    # add vehicle
    vehicleOutput = ET.SubElement(timestepOutput, 'vehicle')
    vehicleOutput.set("id", vehicle.attrib["id"])
    # write vehicle values
    vehicleOutput.set("energyConsumed", str(energyConsumed))
    vehicleOutput.set("totalEnergyConsumed", str(totalEnergyConsumed))
    vehicleOutput.set("totalEnergyRegenerated", str(totalEnergyRegenerated))
    vehicleOutput.set("energyCharged", str(energyCharged))
    vehicleOutput.set("energyChargedInTransit", str(energyChargedInTransit))
    vehicleOutput.set("energyChargedStopped", str(energyChargedStopped))
    vehicleOutput.set("timeStopped", str(timeStopped))

# write Output
outputRootStr = ET.tostring(outputRoot, encoding="utf-8", method="xml")
dom = xml.dom.minidom.parseString(outputRootStr)
prettyDom = dom.toprettyxml()
with open(outputFile, "w") as f:
    f.write(prettyDom)