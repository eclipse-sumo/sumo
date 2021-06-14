##
# \file EvPowertrainXml2Csv.py
# \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
# \copyright Eclipse Public License v2.0
#            (https://www.eclipse.org/legal/epl-2.0/)
# \brief This application converts the XML output of the VKA EV powertrain to a
#        CSV file.
#


import argparse
import sys
import csv
import xml.etree.ElementTree as ET




# Parse command line arguments
argumentParser = argparse.ArgumentParser(description="Convert the XML output"
    + " of the VKA EV powertrain model to a CSV file.")
argumentParser.add_argument("xml_file", type=str, help="EV powertrain XML"
    + " output")
argumentParser.add_argument("csv_file", type=str, help="Converted CSV file"
    + " that shall be created")
parsedArguments = argumentParser.parse_args(sys.argv[1:])


# Convert
with open(parsedArguments.csv_file, "w") as csvFile:
  # Create the CSV file
  fieldnames = ["t", "ID", "v", "a", "slope", "P", "E", "state_valid"]
  dictWriter = csv.DictWriter(csvFile, delimiter=";", fieldnames=fieldnames)
  dictWriter.writeheader()

  # Parse the XML file
  evPowertrainExport = ET.parse(parsedArguments.xml_file).getroot()
  for timestep in evPowertrainExport:
    if timestep.tag != "timestep":
      continue
    t = float(timestep.attrib["time"])

    for vehicle in timestep:
      if vehicle.tag != "vehicle":
        continue
      dictWriter.writerow({ "t" : t, "ID" : str(vehicle.attrib["id"]),
          "v" : float(vehicle.attrib["speed"]),
          "a" : float(vehicle.attrib["acceleration"]),
          "slope" : float(vehicle.attrib["slope"]),
          "P" : float(vehicle.attrib["powerConsumption"]),
          "E" : float(vehicle.attrib["energyConsumption"]),
          "state_valid" : int(vehicle.attrib["stateValid"]) })

