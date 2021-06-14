##
# \file EvPowertrainXml2Csv.py
# \author Kevin Badalian (badalian_k@vka.rwth-aachen.de)
# \author Sven Schönberg (sven.schoenberg@c-lab.de)
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
  fieldnames = ["t", "ID", "x", "y", "z", "edgeID", "laneID", "v", "a",
      "slope", "P", "E", "state_valid"]
  dictWriter = csv.DictWriter(csvFile, delimiter=";", fieldnames=fieldnames)
  dictWriter.writeheader()

  # Parse the XML file
  evPowertrainExport = ET.iterparse(parsedArguments.xml_file,
      events=("start",))
  event, root = next(evPowertrainExport)
  for event, elem in evPowertrainExport:
    if elem.tag == "timestep":
      t = float(elem.attrib["time"])

    if elem.tag == "vehicle":
      dictWriter.writerow({ "t" : t, "ID" : str(elem.attrib["id"]),
          "x" : float(elem.attrib["x"]),
          "y" : float(elem.attrib["y"]),
          "z" : float(elem.attrib["z"]),
          "edgeID" : str(elem.attrib["edge"]),
          "laneID" : str(elem.attrib["lane"]),
          "v" : float(elem.attrib["speed"]),
          "a" : float(elem.attrib["acceleration"]),
          "slope" : float(elem.attrib["slope"]),
          "P" : float(elem.attrib["powerConsumption"]),
          "E" : float(elem.attrib["energyConsumption"]),
          "state_valid" : int(elem.attrib["stateValid"]) })

    elem.clear()
    root.clear()

