using System.Collections;
using System.Collections.Generic;
using UnityEngine;
//using SplineMesh;
using System;
using UnityEditor;
using System.Diagnostics;
using Random = UnityEngine.Random;
using UnityEngine.UI;
using TMPro;

public class formScene : MonoBehaviour
{


    static TerrainData terrainData;
    static string[] plants = { "tree01", "tree02", "tree03", "tree04", "bush01", "bush02", "bush03", "bush04", "bush05", "bush06" };
    bool isReady = false;
    public Vector3[] newVertices;
    public Vector2[] newUV;
    public int[] newTriangles;
    bool gameControl;

    sumoNet net = new sumoNet();

    Color defaultColor = new Color(1, 1, 0, 1);

    Dictionary<string, GameObject> objects = new Dictionary<string, GameObject>();
   
    GameObject vehicles;
    GameObject trafficLights;

    List<string> keysOnScene = new List<string>();
    List<string> keys2remove = new List<string>();


    Stopwatch watchdog;
    long simstep;
    Transform canvasText;
    GameObject roads;
   


    // Start is called before the first frame update
    IEnumerator Start()
    {
        gameControl = true;
        GameObject.FindGameObjectWithTag("canvas").transform.Find("Text").transform.Find("gamestatus").GetComponent<TMP_Text>().enabled = false;
        canvasText = GameObject.FindGameObjectWithTag("canvas").transform.Find("Text").transform.Find("con");
        canvasText.GetComponent<TMP_Text>().enabled = false;
        GameObject prefab = GameObject.CreatePrimitive(PrimitiveType.Cube);
        prefab.transform.position = new Vector3(-1000,-1000,0);
       
        
        Texture2D FlatTexture = Resources.Load<Texture2D>(paths.ground1);
        Texture2D steepTexture = Resources.Load<Texture2D>(paths.ground2 );
        
        GameObject network = new GameObject("StreetNetwork");
        roads = new GameObject("StreetRoads");
        
        trafficLights = new GameObject("TrafficLights");
        GameObject nodes = new GameObject("StreetNodes");
        GameObject houses = new GameObject("StreetHouses");
        vehicles = new GameObject("Vehicles");

        while (!net.testConnection())
        {
            
            canvasText.GetComponent<TMP_Text>().enabled = true;
            yield return new WaitForSecondsRealtime(1);
        }
        isReady = true;
        canvasText.GetComponent<TMP_Text>().enabled = false;
        //net.initiliazeMap();
        net.initiliazeNodes();
        net.initiliazeEdges();
        net.setRange();


        int length = net.xmax - net.xmin;
        int width = net.ymax - net.ymin;
        var heightmap = net.terrainHeightMap;

        int numPlants = 500;


        
        //Texture2D SteepTexture = Resources.Load<Texture2D>("Assets/Resources/grass.png");

        terrainData = new TerrainData();
        terrainData.heightmapResolution = 256;
        terrainData.alphamapResolution = 256;
        
        System.Random rand = new System.Random();
       


        var heightmap2 = new float[heightmap.Count, heightmap[0].Count];
       

        for (int i = 0; i < heightmap.Count; i++)
            for(int j = 0; j < heightmap[0].Count; j++)
                heightmap2[i,j] = heightmap[i][j]/50;
        terrainData.SetHeights(0, 0, heightmap2);


        var flatSplat = new SplatPrototype();
        var steepSplat = new SplatPrototype();

        flatSplat.texture = FlatTexture;
        steepSplat.texture = steepTexture;


        terrainData.splatPrototypes = new SplatPrototype[]
        {
                flatSplat,
                steepSplat
        };

        terrainData.RefreshPrototypes();


        var splatMap = new float[terrainData.alphamapResolution, terrainData.alphamapResolution, 2];

        for (var zRes = 0; zRes < terrainData.alphamapHeight; zRes++)
        {
            for (var xRes = 0; xRes < terrainData.alphamapWidth; xRes++)
            {
                var normalizedX = (float)xRes / (terrainData.alphamapWidth - 1);
                var normalizedZ = (float)zRes / (terrainData.alphamapHeight - 1);

                var steepness = terrainData.GetSteepness(normalizedX, normalizedZ);
                var steepnessNormalized = Mathf.Clamp(steepness / 1.5f, 0, 1f);

                splatMap[zRes, xRes, 0] = 1f - steepnessNormalized;
                splatMap[zRes, xRes, 1] = steepnessNormalized;
            }
        }

        terrainData.SetAlphamaps(0, 0, splatMap);
        terrainData.size = new Vector3(length, 300, width);

        var newTerrainGameObject = Terrain.CreateTerrainGameObject(terrainData);
        
        

        Terrain terr = newTerrainGameObject.GetComponent<Terrain>();
        terr.heightmapPixelError = 8;
        terr.materialType = UnityEngine.Terrain.MaterialType.Custom;
        terr.reflectionProbeUsage = UnityEngine.Rendering.ReflectionProbeUsage.Off;




        // create Tree objects

        List<TreePrototype> tpa = new List<TreePrototype>();

        for (int i = 0; i < plants.Length; i++)
        {
            TreePrototype tp = new TreePrototype();
            tp.prefab = Resources.Load(paths.trees + plants[i] ) as GameObject;
            tpa.Add(tp);
        }
        terrainData.treePrototypes = tpa.ToArray();

        for (int i = 0; i < net.treeLocations.Count; i++)
        {
            //float xnRel = (float)rand.NextDouble();
            //float xn = terrainData.size.x * xnRel;
            //float ynRel = (float)rand.NextDouble();
            //float yn = terrainData.size.z * ynRel;

            //float xGlobal = terr.transform.TransformPoint(new Vector3(xn, 0, yn)).x;
            //float yGlobal = terr.transform.TransformPoint(new Vector3(xn, 0, yn)).z;

            TreeInstance tree = new TreeInstance();
            tree.position = new Vector3(net.treeLocations[i][0], 0, net.treeLocations[i][1]);
            tree.color = Color.white;
            tree.lightmapColor = Color.white;
            tree.prototypeIndex = rand.Next(0, plants.Length);
            tree.heightScale = 1;
            tree.widthScale = 1;
            terr.AddTreeInstance(tree);

        }

        //////////////////// house 


        

        int nodeCount1 = 0;
        for (int i = 0; i < net.houseLocation.Count; i++)
        {
            List<Vector2> nodeShape = new List<Vector2>();
            List<Vector3> nodeShape3 = new List<Vector3>();
            var node = net.houseLocation[i];
            for (int v_i = 0; v_i < node.Count; v_i++)
            {
                Vector2 vector2d = new Vector2((float)node[v_i][0], (float)node[v_i][1]);
                Vector3 vector3d = new Vector3((float)node[v_i][0],  0.5f, (float)node[v_i][1]);
                nodeShape.Add(vector2d);
                nodeShape3.Add(vector3d);

            }


            if (nodeShape.Count > 0)
            {
                polyDraw nodepoly = new polyDraw(nodeShape.ToArray(), nodeShape3.ToArray(), (float)node[0][2]  );
                nodeShape.Clear();
                nodeShape3.Clear();
                Material texture = Resources.Load<Material>(paths.greenMat);
                
                texture.color = Color.gray;

                GameObject obj = new GameObject();
                obj.name = "node_" + nodeCount1;
                obj.transform.SetParent(houses.transform);
                //obj.transform.position = new Vector3(node.coordx,2,node.coordy);

                Mesh[] meshes = nodepoly.drawPoly();

                GameObject topObj = new GameObject();
                topObj.name = "topObject";
                topObj.transform.parent = obj.transform;
                topObj.AddComponent<MeshFilter>().mesh = meshes[0];
                topObj.AddComponent<MeshRenderer>().material = texture;
               

                GameObject buttomObj = new GameObject();
                buttomObj.name = "buttomObj";
                buttomObj.transform.parent = obj.transform;
                buttomObj.AddComponent<MeshFilter>().mesh = meshes[1];
                buttomObj.AddComponent<MeshRenderer>().material = texture;
                //buttomObj.transform.Rotate(90,0,0);

            }



            nodeCount1++;
           

        }


        //for (int i = 0; i < net.houseLocation.Count; i++)
        //{
        //    GameObject house = Instantiate(AssetDatabase.LoadMainAssetAtPath(paths.buildings[Random.Range(0, 29)])) as GameObject;
        //    house.transform.position = new Vector3(net.houseLocation[i][0] , 0, net.houseLocation[i][1]);
        //}


         
        /////////////// Create road objects //////////////////////////
        int laneCounter = 0;
        int TlsCounter = 0;


        List<string> keys = new List<string>(net.edges.Keys);

        
        for (int i = 0; i < net.edges.Count; i++)
        {


            //splineObject.transform.SetParent(network.transform);
            //GameObject splineNode = new GameObject("Node_" + i);
             

            for (int k = 0; k < net.edges[keys[i]].lanes.Count; k++)
            {
                

                for (int l = 0; l < net.edges[keys[i]].lanes[k].shape.Count; l++)
                {

                    float x = (float)net.edges[keys[i]].lanes[k].shape[l][0];
                    float y = (float)net.edges[keys[i]].lanes[k].shape[l][1];
                    float z = (float)net.edges[keys[i]].lanes[k].shape[l][2];

                    Vector3 position = new Vector3(x, z, y);

                   

                    //SplineNode splineNode1 = new SplineNode(position, position);
                    //splineObject.AddNode(splineNode1);

                    if (l < net.edges[keys[i]].lanes[k].shape.Count-1)
                        {
                        float x2 = (float)net.edges[keys[i]].lanes[k].shape[l + 1][0];
                        float y2 = (float)net.edges[keys[i]].lanes[k].shape[l + 1][1];
                        float z2 = (float)net.edges[keys[i]].lanes[k].shape[l + 1][2];
                        Vector3 position2 = new Vector3(x2, z2, y2);
                        Vector3 position3 = new Vector3((x + x2) / 2, 1, (y + y2) / 2);


                        GameObject lineHolder = new GameObject("road_" + laneCounter++);
                        lineHolder.transform.SetParent(roads.transform);
                        float xScale = Vector3.Distance(position, position2);
                        Vector3 positionBirim = new Vector3(x2-x, 0,  y2- y ) / xScale;

                        for (int sc = 0; sc < (int)xScale+1; sc++)
                            {
                            GameObject obj = Instantiate(prefab);
                            obj.name = "road_part_" + sc;
                            obj.transform.SetParent(lineHolder.transform);
                            obj.transform.rotation = Quaternion.LookRotation((position2 - position).normalized);
                            obj.transform.Rotate(new Vector3(0, 90, 0));

                            float ratio = sc / (float)((int)xScale + 1);
                            float ratio2 = (sc+1) / (float)((int)xScale + 1);


                            obj.transform.position =  (Vector3.Lerp(position, position2, ratio) + Vector3.Lerp(position, position2, ratio2)) /2;
                            Material mm;
                            if (net.edges[keys[i]].lanes[k].isYaya)
                            {
                                //texture = Resources.Load<Texture2D>(paths.roadyaya);
                                mm = Resources.Load<Material>(paths.roadyaya);

                            }
                            else if (net.edges[keys[i]].lanes[k].isTram)
                            {
                                //texture = Resources.Load<Texture2D>(paths.roadtrain);
                                mm = Resources.Load<Material>(paths.roadtrain);

                            }
                            else if (net.edges[keys[i]].lanes[k].isBike)
                            {
                                //texture = Resources.Load<Texture2D>(paths.roadtrain);
                                mm = Resources.Load<Material>(paths.roadBike);

                            }
                            else if (net.edges[keys[i]].lanes[k].isGreen)
                            {
                                //texture = Resources.Load<Texture2D>(paths.roadtrain);
                                mm = Resources.Load<Material>(paths.roadgreen);

                            }
                            else if (net.edges[keys[i]].lanes[k].isBus)
                            {
                                //texture = Resources.Load<Texture2D>(paths.roadtrain);
                                mm = Resources.Load<Material>(paths.roadBus);

                            }
                            else
                            {
                                if (net.edges[keys[i]].lanes.Count == 1)
                                {
                                    //texture = Resources.Load<Texture2D>(paths.road1);
                                    mm = Resources.Load<Material>(paths.road1);

                                }
                                else if (net.edges[keys[i]].lanes.Count == 2)
                                {
                                    if (k == 0)
                                    {
                                        //texture = Resources.Load<Texture2D>(paths.roadEdge);
                                        mm = Resources.Load<Material>(paths.roadEdge);

                                    }
                                    else
                                    {
                                        //texture = Resources.Load<Texture2D>(paths.roadEdge);
                                        mm = Resources.Load<Material>(paths.roadEdge);

                                        obj.transform.Rotate(new Vector3(0, 180, 0));
                                    }
                                }
                                else
                                {
                                    if (k == 0 || k == net.edges[keys[i]].lanes.Count - 1)
                                    {
                                        if (k == 0)
                                        {
                                            //texture = Resources.Load<Texture2D>(paths.roadEdge);
                                            mm = Resources.Load<Material>(paths.roadEdge);

                                        }
                                        else
                                        {
                                            //texture = Resources.Load<Texture2D>(paths.roadEdge);
                                            mm = Resources.Load<Material>(paths.roadEdge);
                                            obj.transform.Rotate(new Vector3(0, 180, 0));
                                        }
                                    }
                                    else
                                    {
                                        //texture = Resources.Load<Texture2D>(paths.roadMid);
                                        mm = Resources.Load<Material>(paths.roadMid);
                                    }
                                }
                            }


                            //if (net.edges[keys[i]].lanes[k].isYaya)
                            //{
                            //    mm = Resources.Load<Material>(paths.roadyaya);
                            //    obj.GetComponent<MeshRenderer>().material = mm;
                            //}
                            //else
                            //{
                            //    obj.GetComponent<MeshRenderer>().material.mainTexture = texture;
                            //}

                            obj.GetComponent<MeshRenderer>().material = mm;

                            Vector3 scale;
                            scale = new Vector3(Vector3.Distance(Vector3.Lerp(position, position2, ratio), Vector3.Lerp(position, position2, ratio2)), 1, net.edges[keys[i]].lanes[k].width );
                            //if (sc == (int)xScale )
                            //{
                            //    scale = new Vector3(xScale-sc, 1, net.edges[keys[i]].lanes[k].width / 9);
                            //}
                            //else
                            //{
                            //    scale = new Vector3(1, 1, net.edges[keys[i]].lanes[k].width / 9);
                            //}
                            
                            
                            obj.transform.localScale = scale;

                            
                        }

                        if (net.edges[keys[i]].lanes[k].isTls && k == 0)
                        {
                            GameObject obj = Instantiate(Resources.Load<GameObject>(paths.tLight2) as GameObject);
                            obj.name = "TraficLight_"+keys[i]+"_" +k; // TlsCounter++
                            obj.transform.SetParent(trafficLights.transform);
                            Quaternion rotation2Road = Quaternion.LookRotation((position2 - position).normalized);
                            obj.transform.rotation = rotation2Road;
                            obj.transform.Rotate(new Vector3(0, 180, 0));
                            Vector3 vector2add = new Vector3(net.edges[keys[i]].lanes[k].width, 0, 0);
                            Vector3 rotatedVector = Quaternion.AngleAxis(rotation2Road.eulerAngles.y, Vector3.up) * vector2add;
                            obj.transform.position = position2 + rotatedVector;
                            


                        }

                    }
                                 
                }

            }

        }

        List<string> keysnode = new List<string>(net.nodes.Keys);

        int nodeCount = 0;
        for (int i = 0; i < keysnode.Count; i++)
        {
            List<Vector2> nodeShape = new List<Vector2>();
            List<Vector3> nodeShape3 = new List<Vector3>();
            var node =  net.nodes[keysnode[i]];
            for (int v_i = 0; v_i < node.shape.Count; v_i++)
            {
                Vector2 vector2d = new Vector2((float)node.shape[v_i][0], (float)node.shape[v_i][1]);
                Vector3 vector3d = new Vector3((float)node.shape[v_i][0], (float)node.shape[v_i][2], (float)node.shape[v_i][1]);
                nodeShape.Add(vector2d);
                nodeShape3.Add(vector3d);
                
            }
            
            if (nodeShape.Count > 0)
            {
                polyDraw nodepoly = new polyDraw(nodeShape.ToArray(), nodeShape3.ToArray(),0.5f);
                nodeShape.Clear();
                nodeShape3.Clear();
                Texture2D texture = Resources.Load<Texture2D>(paths.roadMetarialPath + "NoMarking_tex");

                GameObject obj = new GameObject();
                obj.name = "node_" + nodeCount;
                obj.transform.SetParent(nodes.transform);
                //obj.transform.position = new Vector3(node.coordx,2,node.coordy);

                Mesh[] meshes = nodepoly.drawPoly();

                GameObject topObj = new GameObject();
                topObj.name = "topObject";
                topObj.transform.parent = obj.transform;
                topObj.AddComponent<MeshFilter>().mesh = meshes[0];
                topObj.AddComponent<MeshRenderer>().material.mainTexture = texture;


                GameObject buttomObj = new GameObject();
                buttomObj.name = "buttomObj";
                buttomObj.transform.parent = obj.transform;
                buttomObj.AddComponent<MeshFilter>().mesh = meshes[1];
                buttomObj.AddComponent<MeshRenderer>().material.mainTexture = texture;




            }



            //obj.transform.position = position3;

            nodeCount++;
            //// Set up game object with mesh;
            //var meshRenderer = gameObject.AddComponent<MeshRenderer>();
            //meshRenderer.material = new Material(Shader.Find("Sprites/Default"));

            //var filter = gameObject.AddComponent<MeshFilter>();
            //filter.mesh = mesh;

        }


        //////Finish
        newTerrainGameObject.transform.position = new Vector3(net.xmin, 0, net.ymin);
        terr.Flush();
        // Timer for watchdog
        watchdog = new Stopwatch();
        simstep = 500;

    }


    void FixedUpdate()
    {
        try
        {
            if (isReady)
            {

                

                watchdog.Start();
                //prefab2 = GameObject.CreatePrimitive(PrimitiveType.Cube);


                net.vehiclesOnTheScene();

                if (net.isRunning == false || Input.GetKeyDown(KeyCode.Q))
                {
                    Application.Quit();
                    net.closeConnection();
                    //UnityEditor.EditorApplication.isPlaying = false;
                }

                List<string> keys = new List<string>(net.vehicles.Keys);


                foreach (var obje in objects)
                {

                    if (!keys.Contains(obje.Key.Substring(8, obje.Key.Length - 8)))
                    {
                        keys2remove.Add(obje.Key);

                    }
                }

                foreach (var key in keys2remove)
                {
                    Destroy(objects[key]);
                    objects.Remove(key);
                }
                keys2remove.Clear();

                float minDist = 0.01f;


                for (int k = 0; k < net.vehicles.Count; k++)
                {
                    float x = (float)net.vehicles[keys[k]].coordx;
                    float y = (float)net.vehicles[keys[k]].coordy;
                    float z = (float)net.vehicles[keys[k]].coordz;

                    float sx = net.vehicles[keys[k]].extent[0];
                    float sy = net.vehicles[keys[k]].extent[1];
                    float sz = net.vehicles[keys[k]].extent[2];


                    Vector3 position = new Vector3(x, z + 0.5f, y);

                    //if (net.vehicles[keys[k]].type != "DEFAULT_PEDTYPE")
                    //{
                    //    minDist = 0.1f;
                    //}
                    //else
                    //{
                    //    minDist = 0.01f;
                    //}

                    if (keysOnScene.Contains(keys[k]))
                    {
                        Quaternion oldRotation = objects["vehicle_" + keys[k]].transform.rotation;
                        if (Vector3.Distance(position, objects["vehicle_" + keys[k]].transform.position) > minDist) //((objects["vehicle_" + keys[k]].transform.position != position))
                        {
                            objects["vehicle_" + keys[k]].transform.rotation = Quaternion.LookRotation((objects["vehicle_" + keys[k]].transform.position - position).normalized);
                            objects["vehicle_" + keys[k]].transform.Rotate(new Vector3(0, 180, 0));
                        }

                        else
                            objects["vehicle_" + keys[k]].transform.rotation = oldRotation;
                        objects["vehicle_" + keys[k]].transform.position = position;



                    }
                    else
                    {
                        GameObject obj;
                        String vehiclePath;

                        if (net.vehicles[keys[k]].type != "DEFAULT_PEDTYPE")
                        {
                            switch (net.vehicles[keys[k]].vclass)
                            {
                                case "private":
                                    vehiclePath = paths.van[(int)Random.Range(0, paths.van.Length)];
                                    break;
                                case "emergency":
                                    vehiclePath = paths.emergency[(int)Random.Range(0, paths.emergency.Length)];
                                    break;
                                case "authority":
                                    vehiclePath = paths.authority[(int)Random.Range(0, paths.authority.Length)];
                                    break;
                                case "army":
                                    vehiclePath = paths.army[(int)Random.Range(0, paths.army.Length)];
                                    break;
                                case "vip":
                                    vehiclePath = paths.van[(int)Random.Range(0, paths.van.Length)];
                                    break;
                                case "hov":
                                    vehiclePath = paths.bus[(int)Random.Range(0, paths.bus.Length)];
                                    break;
                                case "taxi":
                                    vehiclePath = paths.taxi[(int)Random.Range(0, paths.taxi.Length)];
                                    break;
                                case "bus":
                                    vehiclePath = paths.bus[(int)Random.Range(0, paths.bus.Length)];
                                    break;
                                case "coach":
                                    vehiclePath = paths.bus[(int)Random.Range(0, paths.bus.Length)];
                                    break;
                                case "truck":
                                    vehiclePath = paths.truck[(int)Random.Range(0, paths.truck.Length)];
                                    break;
                                case "trailer":
                                    vehiclePath = paths.truck[(int)Random.Range(0, paths.truck.Length)];
                                    break;
                                case "motorcycle":
                                    vehiclePath = paths.motorcycle[(int)Random.Range(0, paths.motorcycle.Length)];
                                    break;
                                case "moped":
                                    vehiclePath = paths.motorcycle[(int)Random.Range(0, paths.motorcycle.Length)];
                                    break;
                                case "bicycle":
                                    vehiclePath = paths.motorcycle[(int)Random.Range(0, paths.motorcycle.Length)];
                                    break;
                                case "tram":
                                    vehiclePath = paths.train[(int)Random.Range(0, paths.train.Length)];
                                    break;
                                case "rail_urban":
                                    vehiclePath = paths.train[(int)Random.Range(0, paths.train.Length)];
                                    break;
                                case "rail":
                                    vehiclePath = paths.train[(int)Random.Range(0, paths.train.Length)];
                                    break;
                                case "rail_electric":
                                    vehiclePath = paths.train[(int)Random.Range(0, paths.train.Length)];
                                    break;
                                case "rail_fast":
                                    vehiclePath = paths.train[(int)Random.Range(0, paths.train.Length)];
                                    break;
                                default:
                                    vehiclePath = paths.car[(int)Random.Range(0, paths.car.Length)];
                                    break;

                            }

                            obj = Instantiate(Resources.Load<GameObject>(vehiclePath) as GameObject);


                            Color customColor = new Color(net.vehicles[keys[k]].color[0], net.vehicles[keys[k]].color[1], net.vehicles[keys[k]].color[2], net.vehicles[keys[k]].color[3]);
                            if (customColor != defaultColor)
                            {

                                Material vehicleM = new Material(Shader.Find("Specular"));
                                vehicleM.color = customColor;
                                obj.transform.GetChild(0).GetComponent<MeshRenderer>().material = vehicleM;
                            }


                        }
                        else
                        {
                            obj = Instantiate(Resources.Load<GameObject>(paths.yaya2) as GameObject);

                            //obj.GetComponent<MeshRenderer>().material.mainTexture = Resources.Load<Texture>(paths.moskvitchTexture[(int)Random.Range(0, paths.moskvitchTexture.Length)]) as Texture;
                        }

                        obj.name = "vehicle_" + keys[k];
                        obj.transform.SetParent(vehicles.transform);
                        obj.transform.position = position;
                        //obj.transform.localScale = new Vector3(0.3f,0.3f,0.3f);
                        //obj.transform.localScale = new Vector3(sx,sy,sz);



                        objects.Add(obj.name, obj);
                        keysOnScene.Add(keys[k]);
                    }


                }

                List<string> keyss = new List<string>(net.tlights.Keys);
                for (int k = 0; k < net.tlights.Count; k++)
                {
                    GameObject light = GameObject.Find("TraficLight_" + keyss[k]);
                    if (light != null)
                    {
                        for (int i = 0; i < net.tlights[keyss[k]].status.Count; i++)
                        {
                            if (i < 2)
                                if (net.tlights[keyss[k]].status[i] == 0)
                                {
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(0).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.greenLight);
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(1).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.off);
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(2).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.off);
                                }
                                else if (net.tlights[keyss[k]].status[i] == 1)
                                {
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(0).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.off);
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(1).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.yellowLight);
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(2).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.off);
                                }
                                else if (net.tlights[keyss[k]].status[i] == 2)
                                {
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(0).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.off);
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(1).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.off);
                                    light.transform.GetChild(i + 1).gameObject.transform.GetChild(2).gameObject.GetComponent<MeshRenderer>().material = Resources.Load<Material>(paths.redLight);
                                }
                        }

                    }


                }



                long stopTime = watchdog.ElapsedMilliseconds;

                if (stopTime < simstep)
                {
                    StartCoroutine(waiter(simstep - stopTime));
                }
                watchdog.Reset();

            }
        }
        catch
        {
           
               
            canvasText.GetComponent<TMP_Text>().enabled = true;
            canvasText.GetComponent<TMP_Text>().text = "SUMO SERVER BA?LANTISI KOPTU";
            StartCoroutine(waiter(1));
            if (net.isRunning == false || Input.GetKeyDown(KeyCode.Q))
            {
                Application.Quit();
                net.closeConnection();
                 
            }
           
           
        }
    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            if (gameControl)
            {
                Time.timeScale = 1;
                GameObject.FindGameObjectWithTag("canvas").transform.Find("Text").transform.Find("gamestatus").GetComponent<TMP_Text>().enabled = false;
            }
            else
            {
                Time.timeScale = 0;
                GameObject.FindGameObjectWithTag("canvas").transform.Find("Text").transform.Find("gamestatus").GetComponent<TMP_Text>().enabled = true;
            }
            gameControl = !gameControl;
        }
    }
    IEnumerator waiter(long a)
    {
        yield return new WaitForSecondsRealtime(a/1000);
    }
}
