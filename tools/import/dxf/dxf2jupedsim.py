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

polygons = []
polygon = []
start = None
end = None
for entity in dxf.modelspace():
    if entity.dxftype() == "LINE":
        if not start:
            start = entity.dxf.start
            polygon.append(tuple(start))
        if not end or entity.dxf.start == end:
            end = entity.dxf.end
            polygon.append(tuple(end))
        if entity.dxf.end == start:
            polygons.append(polygon)
            end = None
            start = None
            polygon = []

# for entity in dxf.modelspace():
#     if entity.dxftype() == "LINE":
#         print(entity.dxf.start, entity.dxf.end)

def convert_to_string(polygon):
    s = []
    for coordinates in polygon:
        s.append(f"{coordinates[0]},{coordinates[1]}")
    return " ".join(s)

with open(filename[:-3] + "add.xml", "w") as add:
    add.write("<additionals>\n")
    for index, polygon in enumerate(polygons):
        add.write(f'\t<poly id="{index}" shape="{convert_to_string(polygon)}"/>\n')
    add.write("</additionals>")