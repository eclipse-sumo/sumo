using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net;
using System.IO;
using Newtonsoft.Json;


public class nodeJson
{
    public List<double> coords { get; set; }
    public string id { get; set; }
    public List<List<double>> shape { get; set; }
}

// Root myDeserializedClass = JsonConvert.DeserializeObject<List<Root>>(myJsonResponse);
public class Lane
{
    public string id { get; set; }
    public List<List<double>> shape { get; set; }
    public float speed { get; set; }
    public float width { get; set; }
    public bool yaya { get; set; }
    public bool tls { get; set; }
    public bool isTram { get; set; }
    public bool isBike { get; set; }
    public bool isGreen { get; set; }
    public bool isBus { get; set; }

}

public class Edges
{
    public string from { get; set; }
    public string id { get; set; }
    public List<Lane> lanes { get; set; }
    public int priority { get; set; }
    public string to { get; set; }
}



public class Range
{
    public float xmax { get; set; }
    public float xmin { get; set; }
    public float ymax { get; set; }
    public float ymin { get; set; }
    public List<List<float>> heightMap { get; set; }
    public List<List<List<float>>> houseLocation { get; set; }
    public List<List<float>> treeLocations { get; set; }
}


public class Vehicle
{
    public List<double> coords { get; set; }
    public string id { get; set; }
    public List<double> shape { get; set; }
    public int signal { get; set; }
    public List<float> color { get; set; }
    public List<float> extent { get; set; }
    public string type { get; set; }
    public string vclass { get; set; }
}

public class Tls
{
    public string name { get; set; }
    public List<int> status { get; set; }
}



public class sumoNet
{
    

    public Dictionary<string, sumoNodes> nodes = new Dictionary<string, sumoNodes>();
    public Dictionary<string, sumoEdges> edges = new Dictionary<string, sumoEdges>();
    public Dictionary<string, sumoLanes> lanes = new Dictionary<string, sumoLanes>();
    public Dictionary<string, sumoVehicle> vehicles = new Dictionary<string, sumoVehicle>();
    public Dictionary<string, sumoLights> tlights = new Dictionary<string, sumoLights>();

    public int xmin;
    public int xmax;
    public int ymin;
    public int ymax;
    public List<List<float>> terrainHeightMap;
    public List<List<List<float>>> houseLocation;
    public List<List<float>> treeLocations;

    public bool isRunning = false;

    public string sumoUrl = "http://127.0.0.1:5000";
    public sumoNet()
    {

    }

    public bool testConnection()
    {
        try
        {
            var request = WebRequest.Create($"{sumoUrl}/check");
            request.Method = "GET";
            request.Timeout = 100;
            request.ContentType = "application/json";
            var definition = new { id = "" };

            using var webStream = request.GetResponse().GetResponseStream();
        }
        catch
        {
            bool a = false;
            return a;
        }
        return true;
    }

    public bool closeConnection()
    {
        try
        {
            var request = WebRequest.Create($"{sumoUrl}/close");
            request.Method = "GET";
            request.Timeout = 100;
            request.ContentType = "application/json";
            var definition = new { id = "" };

            using var webStream = request.GetResponse().GetResponseStream();
        }
        catch
        {
            bool a = false;
            return a;
        }
        return true;
    }
    public void initiliazeMap()
    {
        var request = WebRequest.Create($"{sumoUrl}/initMap");
        request.Method = "GET";
        request.Timeout = 1000000;
        request.ContentType = "application/json";
        var definition = new { id = "" };

        using var webStream = request.GetResponse().GetResponseStream();
    }

        public void initiliazeNodes()
    {
        var request = WebRequest.Create($"{sumoUrl}/getNodes");
        request.Method = "GET";
        request.ContentType = "application/json";
        var definition = new { id = "" };

        using var webStream = request.GetResponse().GetResponseStream();
        using var reader = new StreamReader(webStream);
        var data = reader.ReadToEnd();
        // Root myDeserializedClass = JsonConvert.DeserializeObject<List<Root>>(myJsonResponse);
    

        var myDeserializedClass = JsonConvert.DeserializeObject<List<nodeJson>>(data);
        for (int i = 0; i < myDeserializedClass.Count; i++)
        {
            sumoNodes node = new sumoNodes(myDeserializedClass[i].id, (float)myDeserializedClass[i].coords[0], (float)myDeserializedClass[i].coords[1], (float)myDeserializedClass[i].coords[2], myDeserializedClass[i].shape);
            nodes.Add(myDeserializedClass[i].id, node);
        }

    }

    public void initiliazeEdges()
    {
        var request = WebRequest.Create($"{sumoUrl}/getEdges");
        request.Method = "GET";
        request.ContentType = "application/json";
        var definition = new { id = "" };

        using var webStream = request.GetResponse().GetResponseStream();
        using var reader = new StreamReader(webStream);
        var data = reader.ReadToEnd();
      


        var myDeserializedClass = JsonConvert.DeserializeObject<List<Edges>>(data);

        for (int i = 0; i < myDeserializedClass.Count; i++)
        {
            // sumoget by idddddd ekle string id, int index, double speed, List<double[]> shape

            sumoEdges edge = new sumoEdges(myDeserializedClass[i].id, (int)myDeserializedClass[i].priority, nodes[myDeserializedClass[i].from], nodes[myDeserializedClass[i].to]);
            for (int j = 0; j < myDeserializedClass[i].lanes.Count; j++)
            {
                

                sumoLanes lane = new sumoLanes(myDeserializedClass[i].lanes[j].id, j, myDeserializedClass[i].lanes[j].speed, myDeserializedClass[i].lanes[j].shape, myDeserializedClass[i].lanes[j].width, myDeserializedClass[i].lanes[j].yaya, myDeserializedClass[i].lanes[j].tls, myDeserializedClass[i].lanes[j].isTram, myDeserializedClass[i].lanes[j].isBike, myDeserializedClass[i].lanes[j].isGreen, myDeserializedClass[i].lanes[j].isBus);
                if (!lanes.ContainsKey(myDeserializedClass[i].lanes[j].id))
                {
                    lanes.Add(myDeserializedClass[i].lanes[j].id,lane);
                    edge.addLane(myDeserializedClass[i].lanes[j].id, j, (float)myDeserializedClass[i].lanes[j].speed, myDeserializedClass[i].lanes[j].shape, myDeserializedClass[i].lanes[j].width, myDeserializedClass[i].lanes[j].yaya, myDeserializedClass[i].lanes[j].tls, myDeserializedClass[i].lanes[j].isTram, myDeserializedClass[i].lanes[j].isBike, myDeserializedClass[i].lanes[j].isGreen, myDeserializedClass[i].lanes[j].isBus);
                }
                    
            }
                
            edges.Add(myDeserializedClass[i].id, edge);
        }

    }

    public void setRange()
    {
         
        var request = WebRequest.Create($"{sumoUrl}/range");
        request.Method = "GET";
        request.ContentType = "application/json";
         

        using var webStream = request.GetResponse().GetResponseStream();
        using var reader = new StreamReader(webStream);
        var data = reader.ReadToEnd();



        var myDeserializedClass = JsonConvert.DeserializeObject<List<Range>>(data);

        xmax = (int)myDeserializedClass[0].xmax;
        xmin = (int)myDeserializedClass[0].xmin;
        ymax = (int)myDeserializedClass[0].ymax;
        ymin = (int)myDeserializedClass[0].ymin;
        terrainHeightMap = myDeserializedClass[0].heightMap;
        houseLocation = myDeserializedClass[0].houseLocation;
        treeLocations = myDeserializedClass[0].treeLocations;

    }

    public void vehiclesOnTheScene()
    {
        isRunning = true;
        var request = WebRequest.Create($"{sumoUrl}/tick");
        request.Method = "GET";
        request.ContentType = "application/json";

        try {
            using var webStream = request.GetResponse().GetResponseStream();
            
            using var reader = new StreamReader(webStream);
            var data = reader.ReadToEnd();
            if (data == "\"map is not loaded..\"\n")
            {
                isRunning = false;
            }

            vehicles.Clear();
            var myDeserializedClass = JsonConvert.DeserializeObject<List<Vehicle>>(data);

            for (int i = 0; i < myDeserializedClass.Count; i++)
            {
                sumoVehicle vehicle = new sumoVehicle(myDeserializedClass[i].id, (float)myDeserializedClass[i].coords[0], (float)myDeserializedClass[i].coords[1], (float)myDeserializedClass[i].coords[2], myDeserializedClass[i].shape, myDeserializedClass[i].signal, myDeserializedClass[i].color, myDeserializedClass[i].type, myDeserializedClass[i].vclass, myDeserializedClass[i].extent);
                vehicles.Add(myDeserializedClass[i].id, vehicle);
            }
        }
        catch
        {
            vehicles.Clear();
            isRunning = false;
        }

        //// traffic light update
        request = WebRequest.Create($"{sumoUrl}/getTls");
        request.Method = "GET";
        request.ContentType = "application/json";

        try
        {
            using var webStream = request.GetResponse().GetResponseStream();

            using var reader = new StreamReader(webStream);
            var data = reader.ReadToEnd();
            if (data == "\"map is not loaded..\"\n")
            {
                isRunning = false;
            }

            tlights.Clear();
            var myDeserializedClass = JsonConvert.DeserializeObject<List<Tls>>(data);

            for (int i = 0; i < myDeserializedClass.Count; i++)
            {
                sumoLights light = new sumoLights(myDeserializedClass[i].name, myDeserializedClass[i].status);
                tlights.Add(myDeserializedClass[i].name, light);
            }
        }
        catch
        {
            vehicles.Clear();
            isRunning = false;
        }


    }


    }
    

   


