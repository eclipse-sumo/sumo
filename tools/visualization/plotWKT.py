import os
import sys
from matplotlib.pyplot import figure, plot, gca, show
from shapely import wkt
from shapely.geometry import Polygon, MultiPolygon

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("Please set environment variable 'SUMO_HOME' to your SUMO binary directory.")

import sumolib
from sumolib.options import ArgumentParser

def plotPolygonWithHoles(polygon, options):
    x, y = polygon.exterior.coords.xy
    plot(x, y, color=options.color)
    for hole in polygon.interiors:
        if Polygon(hole).area > options.area_threshold:
            x, y = hole.coords.xy
            plot(x, y, color=options.color)

def plotMultiPolygonWithHoles(multipolygon, options):
    for polygon in multipolygon.geoms:
        plotPolygonWithHoles(polygon, options['color'])

def main(args=None):
    ap = ArgumentParser(description="Plot a polygon, or the difference between two polygons, given in WKT format.")
    ap.add_argument('-f','--filename', category='input', type=ap.file, help='Name of a WKT file', required=True)
    ap.add_argument('-o','--other-filename', category='input', type=ap.file, help='Name of another WKT file')
    ap.add_argument('-a', '--area-threshold', category='processing', default=0.01, help='Area threshold used to filter small holes')
    ap.add_argument('-m', '--color', category='processing', default='blue', help='Color used to draw the polygons')
    options = ap.parse_args(args=args)

    with open(options.filename) as file:
        wktString = file.read()
        polygon = wkt.loads(wktString)
        if options.other_filename:
            with open(options.other_filename) as otherFile:
                otherWKTString = otherFile.read()
                otherPolygon = polygon.loads(otherWKTString)
                polygon = polygon.difference(otherPolygon)
    
    figure()
    if type(polygon) == Polygon:
        plotPolygonWithHoles(polygon, options)
    elif type(polygon) == MultiPolygon:
        plotMultiPolygonWithHoles(polygon, options)
    gca().set_aspect('equal')
    show()

if __name__ == "__main__":
    try:
        main()
    except ValueError as e:
        sys.exit(e)