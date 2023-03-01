import os, sys, json,logging,time
import shutil
import random
from flask import Flask
from flask import jsonify
from shapely.ops import unary_union,transform
from itertools import combinations
from shapely.geometry import Polygon,MultiPolygon,LineString,Point
import numpy as np
import xml.etree.ElementTree as ET
import overpy,pyproj
from functools import partial
import argparse,subprocess
import colorsys,fileinput,xml.dom
parser = argparse.ArgumentParser(
                    prog = 'Sumo Unity traci backend uygulaması',
                    description = 'Bu program unity 3B simulasyonu ile sumo arasında bilgi akışını sağlar',
                    epilog = 'MFS')
parser.add_argument('-n', '--net',help='sumo config dosyasının bulunduğu dosya yolu belirtilir.') 
args = parser.parse_args()



sumoRoadPoligons = []
numofHill = 50
numofTree = 600
numofhouse = 50


def exeExists(binary):
    if os.name == "nt" and binary[-4:] != ".exe":
        binary += ".exe"
    return os.path.exists(binary)

def pyExists(binary):
    if os.name == "nt" and binary[-3:] != ".py":
        binary += ".py"
    return os.path.exists(binary)

def checkBinary(name, bindir=None,ispy=None,local=None):
    """
    Checks for the given binary in the places, defined by the environment
    variables SUMO_HOME and <NAME>_BINARY.
    """
    if local:
        return name
    else:
        if not ispy:
            if name == "sumo-gui":
                envName = "GUISIM_BINARY"
            else:
                envName = name.upper() + "_BINARY"
            env = os.environ
            join = os.path.join
            if envName in env and exeExists(env.get(envName)):
                return env.get(envName)
            if bindir is not None:
                binary = join(bindir, name)
                if exeExists(binary):
                    return binary
            if "SUMO_HOME" in env:
                binary = join(env.get("SUMO_HOME"), "bin", name)
                if exeExists(binary):
                    return binary
            if bindir is None:
                binary = os.path.abspath(join(os.path.dirname(__file__), '..', '..', 'bin', name))
                if exeExists(binary):
                    return binary
            if name[-1] != "D" and name[-5:] != "D.exe":
                binaryD = (name[:-4] if name[-4:] == ".exe" else name) + "D"
                found = checkBinary(binaryD, bindir)
                if found != binaryD:
                    return found
            return name
        else:
            
            env = os.environ
            join = os.path.join
            
            if bindir is not None:
                binary = join(bindir, name)
                if pyExists(binary):
                    return binary
            if "SUMO_HOME" in env:
                binary = join(env.get("SUMO_HOME"), "tools", name)
                if pyExists(binary):
                    return binary
            if bindir is None:
                binary = os.path.abspath(join(os.path.dirname(__file__), '..', '..', 'bin', name))
                if pyExists(binary):
                    return binary
            if name[-1] != "D" and name[-5:] != "D.py":
                binaryD = (name[:-4] if name[-4:] == ".py" else name) + "D"
                found = checkBinary(binaryD, bindir)
                if found != binaryD:
                    return found
            return name


# ==================================================================================================
# -- find traci module -----------------------------------------------------------------------------
# =================================================================================================

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
typemapdir = os.path.join("${SUMO_HOME}" if "SUMO_HOME" in os.environ else SUMO_HOME, "data", "typemap")

import sumolib
import osmGet
import osmBuild
# ==================================================================================================
# -- sumo integration imports ----------------------------------------------------------------------
# ==================================================================================================
from sumo_simulation import SumoSimulation
from constants import INVALID_ACTOR_ID

# ==================================================================================================
# -- synchronization_loop --------------------------------------------------------------------------
# ==================================================================================================


class SimulationSynchronization(object):
    """
    SimulationSynchronization class is responsible for the synchronization of sumo and carla
    simulations.
    """
    def __init__(self,
                 sumo_simulation,
                 
                 tls_manager='none',
                 sync_vehicle_color=False,
                 sync_vehicle_lights=False):

        self.sumo = sumo_simulation

        

        self.tlsConnections = {}
        for i in self.sumo.net._id2tls.keys():
            self.tlsConnections[i] = {}
            for y in self.sumo.net._id2tls[i]._connections:
                try:
                    self.tlsConnections[i][y[0].getID()][0].append(y[2])
                    self.tlsConnections[i][y[0].getID()][1].append(y[1].getID())
                except:
                    self.tlsConnections[i][y[0].getID()] = [[y[2]],[y[1].getID()]]
           
                

        self.tls_manager = tls_manager
        self.sync_vehicle_color = sync_vehicle_color
        self.sync_vehicle_lights = sync_vehicle_lights


        # Mapped actor ids.
        self.sumo2carla_ids = {}  # Contains only actors controlled by sumo.
        self.carla2sumo_ids = {}  # Contains only actors controlled by carla.

        self.sumoActors = {}
        self.sumoActors_person = {}

        self.lightStatus = {}
        offset = self.sumo.get_net_offset()


    def tick(self):
        """
        Tick to simulation synchronization
        """
        # -----------------
        # sumo-->carla sync
        # -----------------
        self.sumo.tick()

        try:

            # Destroying sumo arrived actors in carla.
            for sumo_actor_id in self.sumo.destroyed_actors:
                
                self.sumoActors.pop(sumo_actor_id)

            # Spawning new sumo actors in carla (i.e, not controlled by carla).
            sumo_spawned_actors = self.sumo.spawned_actors #- set(self.sumoActors.values())
            for sumo_actor_id in sumo_spawned_actors:
                self.sumo.subscribe(sumo_actor_id)
                sumo_actor = self.sumo.get_actor(sumo_actor_id)
                self.sumoActors[sumo_actor_id] = sumo_actor
                

            # Updating sumo actors in carla.
            for sumo_actor_id in self.sumoActors:
                sumo_actor = self.sumo.get_actor(sumo_actor_id)
                self.sumoActors[sumo_actor_id] = sumo_actor


            ################ Person ################
            # Destroying sumo arrived actors in carla.
            for sumo_actor_id in self.sumo.destroyed_actors_person:
                
                self.sumoActors_person.pop(sumo_actor_id)

            # Spawning new sumo actors in carla (i.e, not controlled by carla).
            sumo_spawned_actors = self.sumo.spawned_actors_person #- set(self.sumoActors.values())
            for sumo_actor_id in sumo_spawned_actors:
                self.sumo.subscribe(sumo_actor_id,False)
                sumo_actor = self.sumo.get_actor(sumo_actor_id,False)
                self.sumoActors_person[sumo_actor_id] = sumo_actor
                

            # Updating sumo actors in carla.
            for sumo_actor_id in self.sumoActors_person:
                sumo_actor = self.sumo.get_actor(sumo_actor_id,False)
                self.sumoActors_person[sumo_actor_id] = sumo_actor
                



            # Updates traffic lights in carla based on sumo information.
            if self.tls_manager == 'sumo':
                state = ""
                self.lightStatus = {}
                for tlid, program_id in self.sumo.traffic_light_manager._current_program.items():
                    state = self.sumo.traffic_light_manager._tls[tlid][program_id].states[self.sumo.traffic_light_manager._current_phase[tlid]]

                    for edge in self.tlsConnections[tlid].keys():
                        temp = []
                        for line in self.tlsConnections[tlid][edge][0]:
                            if state[line].lower() == 'g':
                                temp.append(0)
                            elif state[line].lower() == 'y':
                                temp.append(1)
                            else:
                                temp.append(2)

                        self.lightStatus[edge] = temp



                common_landmarks = self.sumo.traffic_light_ids
                for landmark_id in common_landmarks:
                    sumo_tl_state = self.sumo.get_traffic_light_state(landmark_id)

            

        except Exception as err:
            print(err)

        
     

    def close(self):
        """
        Cleans synchronization.
        """
        # Configuring carla simulation in async mode.
        

        for sumo_actor_id in self.carla2sumo_ids.values():
            self.sumo.destroy_actor(sumo_actor_id)

        # Closing sumo and carla client.
   
        self.sumo.close()



app = Flask(__name__)
synchronization = ""

#@app.route("/initMap")
def initMap(netFile,sumo_host=None,sumo_port=None,sumo_gui=True,client_order=1,tls_manager="sumo",sync_vehicle_color=False,sync_vehicle_lights=False):
    try:
        """
        Entry point for sumo-carla co-simulation.
        """
        global synchronization

        sumo_cfg_file= netFile #"netFile/osmMap5/osm.sumocfg"
        #sumo_cfg_file="netFile/testMultiCar/T2.sumocfg"

        step_length=0.05
        
        sumo_simulation = SumoSimulation(sumo_cfg_file, step_length, sumo_host,
                                        sumo_port, sumo_gui, client_order)
        
        synchronization = SimulationSynchronization(sumo_simulation, tls_manager,
                                                    sync_vehicle_color, sync_vehicle_lights)
        
        # unityPath = checkBinary("sumoTest.exe")
        # subprocess.Popen([unityPath])
        
        #return jsonify("ok")
    except Exception as err:
        synchronization.close()

@app.route("/check")
def checkConnection():
    return jsonify("ok")

@app.route("/close")
def closeConnection():
    if synchronization:
        synchronization.close()
        return jsonify("ok")
    else:
        return jsonify("false")


@app.route("/tick")
def simStep():
    try:
        step_length=0.05
        if type(synchronization) == str:
            return jsonify("map is not loaded..")
         

        synchronization.tick()
         
         
        outJson = []
        for i in synchronization.sumoActors.keys():
            
            outJson.append({"id":i, "coords": synchronization.sumoActors[i][2],"shape": synchronization.sumoActors[i][4],"signal":synchronization.sumoActors[i][3],"color":synchronization.sumoActors[i][5],"type":synchronization.sumoActors[i][0],"vclass":synchronization.sumoActors[i][1].value,"extent":synchronization.sumoActors[i][4]})
        
        for i in synchronization.sumoActors_person.keys():
            outJson.append({"id":"p"+i, "coords": synchronization.sumoActors_person[i][2],"shape": synchronization.sumoActors_person[i][4],"signal":synchronization.sumoActors_person[i][3],"color":synchronization.sumoActors_person[i][5],"type":synchronization.sumoActors_person[i][0],"vclass":"person","extent":[1,1,1]})
        
        return jsonify(outJson)

    except Exception as err:
        synchronization.close()



@app.route("/getTls")
def getTls():
    outJson = []
    for i in synchronization.lightStatus.keys():
        outJson.append({"name":i, "status": synchronization.lightStatus[i]})

    return jsonify(outJson)


@app.route("/getNodes")
def getNodes():
    outJson = []
    for i in synchronization.sumo.net._id2node.values():
        outJson.append({"id":i._id, "coords": i._coord,"shape": i._shape3D})

    return jsonify(outJson)

@app.route("/getEdges")
def getEdges():
    count = 0
    outJson = []
    for i in synchronization.sumo.net._id2edge.values():
        lines = []
        isYaya=False
        isTram=False
        isTls = False
        isBike = False
        isGreen = False
        isBus = False

        for j,c in zip(i._lanes,range(len(i._lanes))):
            if 'pedestrian' in j._allowed and len(j._allowed) < 2:
                isYaya= True
            else:
                isYaya= False
            
            if 'bicycle' in j._allowed and len(j._allowed) < 2:
                isBike= True
            else:
                isBike= False
            
            if 'bus' in j._allowed and len(j._allowed) < 2:
                isBus= True
            else:
                isBus= False
            
            if  len(j._allowed) == 0 :
                isGreen= True
            else:
                isGreen= False

            allowedTrains = [xf in j._allowed for xf in ('tram','rail_urban','rail','rail_electric','rail_fast','rail_slow','lightrail','cityrail')]
            if any(allowedTrains) and len(j._allowed) == allowedTrains.count(True):
                isTram= True
            else:
                isTram= False

            if j._edge._tls is not None:
                isTls = True
            else:
                isTls = False

            lines.append({"id":j._params['origId']+str(count+c) if 'origId' in j._params else count+c,"shape": j._shape3D, "speed":j._speed, "width":j._width, 
            "yaya":isYaya, "tls":isTls, "isTram":isTram, "isBike":isBike, "isGreen":isGreen, "isBus":isBus} )
            
            sumoRoadPoligons.append(LineString(j._shape3D).buffer(j._width+10))

        outJson.append({"id":i._id, "priority": i._priority,"from": i._from._id,"to": i._to._id,"lanes":lines}) # ,"tls":i._tls
        count +=10
    
    

    return jsonify(outJson)

def getLocationInPoligon(min_x,min_y,max_x,max_y,height,width,poly):
     
    rand_x = random.uniform(min_x, max_x)
    rand_y = random.uniform(min_y, max_y)
    for i in range(15):
        rand_x = random.uniform(min_x, max_x)
        rand_y = random.uniform(min_y, max_y)

        # left = Point([rand_x, rand_y])
        # bottom = Point([rand_x, rand_y - height])
        # right = Point([rand_x + width, rand_y - height])
        # top = Point([rand_x + width, rand_y])

        new_poly = Point(rand_x,rand_y).buffer(height)    #Polygon([left, bottom, right, top])

        if poly.contains(new_poly):
            return (rand_x,rand_y,new_poly)
    
    return -1,-1,-1

def poligonSubsruction(terrain,d):
    if terrain.intersects(d)==True:
        nonoverlap = terrain.symmetric_difference(d)
    else:
        nonoverlap = terrain
    
    return nonoverlap


def fetchBuildings(netOfset, globalCoors,proj):
    """
    globalCoors, localCoors are bbox with format of xmin ymin xmax ymax
    """
    px1, py1, px2, py2 = list(map(float,globalCoors.split(",")))  

    nox, noy = list(map(float,netOfset.split(",")))   

    api = overpy.Overpass() 
    # r = api.query(f"""
    # [out:json][timeout:25];
    # // gather results
    # (
    # node["building"]["height"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
    # way["building"]["height"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
    # relation["building"]["height"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}");
    # );
    # // print results
    # out body;
    # >;
    # out skel qt;
    # """)

    r = api.query(f"""
        [out:json][timeout:25];
        // gather results
        (
        node["building"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
        way["building"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
        relation["building"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}");
        );
        // print results
        out body;
        >;
        out skel qt;
        """)

    if len(r.ways) == 0:
        r = api.query(f"""
        [out:json][timeout:25];
        // gather results
        (
        node["building"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
        way["building"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}"); 
        relation["building"](poly:"{py1} {px1} {py1} {px2} {py2} {px2} {py2} {px1}");
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
    pyproj.Proj(proj))

    for way in r.ways:
        buldingpoly = []
        for node in way.nodes:
            if way.tags:
                pr2 = transform(project, Point(float(node.lat) , float(node.lon)))
                x = pr2.x + nox
                y = pr2.y + noy

                # x = ((x-orgP1.x) / (orgP2.x - orgP1.x)) * xrange + lx1
                # y = ((y-orgP1.y) / (orgP2.y - orgP1.y)) * yrange + ly1

                try:
                    buldingpoly.append([x,y,float(way.tags["height"])])
                except:
                    buldingpoly.append([x,y,1]) 
        coords.append(buldingpoly[:-1]) 
    return coords
    


@app.route("/range")
def getRange():
    outJson = []
    
    treeLocations = []
    houseLocations = []
    terrainResolution = 256
    hillRadius = 10
    
    i = np.array(synchronization.sumo.net._ranges).transpose().tolist()
    
    d = unary_union(sumoRoadPoligons)
    
    ### crete Terrain height map
    terrainheightMap = np.zeros((terrainResolution,terrainResolution))
    xResoulution = abs(i[1][0]-i[0][0])/terrainResolution
    yResoulution = abs(i[1][1]-i[0][1])/terrainResolution

    x = np.array(range(hillRadius))  
    y = np.array(range(hillRadius))  
    z = np.array(-(x**2+y**2))

    z= (z-np.min(z))
    z = z/np.max(z)

    terrain = Polygon([(i[0][0],i[0][1]),(i[1][0],i[0][1]),(i[1][0],i[1][1]),(i[0][0],i[1][1])])

    nonoverlap = poligonSubsruction(terrain,d)
    df = []

  
    if (len(synchronization.sumo.buildings)>0):
        houseLocations = synchronization.sumo.buildings
    else:
        if synchronization.sumo.net._location['projParameter'] != "!":
            houses = fetchBuildings(synchronization.sumo.net._location['netOffset'],synchronization.sumo.net._location['origBoundary'],synchronization.sumo.net._location['projParameter'] )
        else:
            houses = []
        if len(houses) == 0:
            for k in range(numofhouse):
                x,y,geom = getLocationInPoligon(i[0][0],i[0][1],i[1][0],i[1][1],yResoulution*10,xResoulution*15,nonoverlap)
                if x == -1:
                    continue
                
                nonoverlap = poligonSubsruction(nonoverlap,geom)
                
                houseLocations.append([[geom.bounds[0],geom.bounds[1],0],[geom.bounds[2],geom.bounds[3],0]])
                
        else:
            for k in houses:
                geom = Polygon(k)
                
                
                nonoverlap = poligonSubsruction(nonoverlap,geom)
                
            houseLocations = houses


    for k in range(numofHill):
        x,y,geom = getLocationInPoligon(i[0][0],i[0][1],i[1][0],i[1][1],yResoulution*17,xResoulution*15,nonoverlap)
        
        if x == -1:
            continue
        try:
            x = x - i[0][0]
            y = y - i[0][1]
            df.append(geom)

            x = int(x/xResoulution)   if  int(x/xResoulution) < 246 else 246
            y = int(y/yResoulution)   if int(y/yResoulution) < 246 else 246
            
            terrainheightMap[y:y+hillRadius,x:x+hillRadius]  = z
        except:
            continue
    
    for k in range(numofTree):
        x,y,geom = getLocationInPoligon(i[0][0],i[0][1],i[1][0],i[1][1],yResoulution*0.2,xResoulution*15,nonoverlap)
        

        if x == -1:
            continue
        x = x - i[0][0]
        y = y - i[0][1]
        treeLocations.append([x/abs(i[1][0]-i[0][0]),y/abs(i[1][1]-i[0][1])])
    
    


    
    
    outJson.append({"xmin":i[0][0], "ymin":i[0][1], "xmax":i[1][0],"ymax":i[1][1], "heightMap":terrainheightMap.tolist(), "houseLocation":houseLocations, "treeLocations":treeLocations})
    return jsonify(outJson)


if __name__=='__main__':
    initMap(args.net)
    app.run(host='0.0.0.0',debug=False)
    


