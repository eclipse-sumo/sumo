import os, sys, json
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:   
    sys.exit("please declare environment variable 'SUMO_HOME'")

from flask import Flask
from flask import jsonify
# import the library
import sumolib
# parse the net
net = sumolib.net.readNet('osm.net.xml')


app = Flask(__name__)

@app.route("/getNodes")
def getNodes():
    outJson = []
    for i in net._id2node.values():
        outJson.append({"id":i._id, "coords": i._coord,"shape": i._shape})
       

    return jsonify(outJson)

@app.route("/getEdges")
def getEdges():
    outJson = []
    for i in net._id2edge.values():
        outJson.append({"id":i._id, "priority": i._priority,"from": i._from._id,"to": i._to._id, "lanes": [{"id":j._params['origId'],"shape": j._shape, "speed":j._speed, "width":j._width} for j in i._lanes]})
       

    return jsonify(outJson)

@app.route("/range")
def getRange():
    outJson = []
    i = net._ranges
    outJson.append({"xmin":i[0][0], "xmax":i[0][1], "ymin":i[1][0],"ymax":i[1][1]})
       

    return jsonify(outJson)


if __name__=='__main__':
    app.run(host='0.0.0.0',debug=True)
# retrieve the successor node ID of an edge
#nextNodeID = net.getEdge('myEdgeID').getToNode().getID()