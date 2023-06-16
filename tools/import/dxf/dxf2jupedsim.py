import argparse as ap
import sys
import ezdxf

parser = ap.ArgumentParser()
parser.add_argument('-f','--file', help='The DXF file to read from', required=True)
args = vars(parser.parse_args())

filename = args['file']

try:
    dxf = ezdxf.readfile(filename)
except IOError:
    print("Input file is not a DXF file or generic I/O error occured.")
    sys.exit(1)
except ezdxf.DXFStructureError:
    print("Invalid or corrupted input DXF file.")
    sys.exit(2)

walkable_areas = []
obstacles = []
msp = dxf.modelspace()
for entity in msp.query("LWPOLYLINE"):
    vertices = list(entity.vertices())
    if entity.dxf.layer == "walkable_areas":
        walkable_areas.append(vertices)
    elif entity.dxf.layer == "obstacles":
        obstacles.append(vertices)
    else:
        print("Polygon belonging to unknown layer.")
        sys.exit(3)

def convert_to_string(polygon):
    s = []
    for coordinates in polygon:
        s.append(f"{coordinates[0]},{coordinates[1]}")
    return " ".join(s)

def polygon_as_XML_element(polygon, typename, color):
    return f'\t<poly id="{typename[9:]+"_"+str(index)}" type="{typename}" color="{color}" shape="{convert_to_string(polygon)}"/>\n'

with open(filename[:-3] + "add.xml", "w") as add:
    add.write("<additionals>\n")
    for index, polygon in enumerate(walkable_areas):
        add.write(polygon_as_XML_element(polygon, "jupedsim.walkable_area", "blue"))
    for index, polygon in enumerate(obstacles):
        add.write(polygon_as_XML_element(polygon, "jupedsim.obstacle", "red"))
    add.write("</additionals>")