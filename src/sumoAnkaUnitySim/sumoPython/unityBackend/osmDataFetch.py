import overpy,pyproj
from functools import partial
from shapely.ops import unary_union,transform
from itertools import combinations
from shapely.geometry import Polygon,MultiPolygon,LineString,Point

api = overpy.Overpass()




def fetchBuildings(globalCoors="80.19865036010742,12.999205411301194,80.35160064697266,13.159557869376798", localCoors="0.00,0.00,453.17,320.86"):
    """
    globalCoors, localCoors are bbox with format of xmin ymin xmax ymax
    """
    px1, py1, px2, py2 = list(map(float,globalCoors.split(","))) 
    lx1, ly1, lx2, ly2 = list(map(float,localCoors.split(",")))  

    api = overpy.Overpass() #41.067630 28.988478 41.067630 28.998823 41.074900 28.998823
    # r = api.query(f"""
    # node["Building"](poly:"{py1} {px1} {py1} {px2} {py2} {px2}");
    # out center;
    # """)
    r = api.query(f"""
    [out:json][timeout:25];
    // gather results
    (
    // query part for: “building=* and height=*”
    node["building"]["height"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
    way["building"]["height"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
    relation["building"]["height"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}");
    );
    // print results
    out body;
    >;
    out skel qt;
    """)
    coords  = []
    project = partial(
    pyproj.transform,
    pyproj.Proj('epsg:4326'),
    pyproj.Proj('epsg:3857'))

    orgP1 = transform(project, Point(px1, py1))
    orgP2 = transform(project, Point(px2, py2))

    xrange = lx2 - lx1
    yrange = ly2 - ly1
    f = open("fff.txt","w")
    for way in r.ways:
        buldingpoly = []
        for node in way.nodes:
            if way.tags:
                pr2 = transform(project, Point(float(node.lon), float(node.lat)))
                x = pr2.x
                y = pr2.y

                x = ((x-orgP1.x) / (orgP2.x - orgP1.x)) * xrange + lx1
                y = ((y-orgP1.y) / (orgP2.y - orgP1.y)) * yrange + ly1

                try:
                  buldingpoly.append([y,x,float(way.tags["height"])])
                except:
                  buldingpoly.append([y,x,0]) 
        f.write(Polygon(buldingpoly).wkt+"\n")
    # coords += [(float(node.lon), float(node.lat)) 
    #         for node in r.nodes if len(node.tags) !=0]
    
    f.close()
    return coords

fetchBuildings()