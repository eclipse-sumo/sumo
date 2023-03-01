using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class sumoNodes 
{
    string id;
    public float coordx;
    public float coordy;
    public float coordz;
    public List<List<double>> shape;
    public Dictionary<string,sumoLanes> lanes = new Dictionary<string, sumoLanes>();

    public  sumoNodes(string id, float coordx, float coordy, float coordz, List<List<double>> shape)
    {
        this.id = id;
        this.coordx = coordx;
        this.coordy = coordy;
        this.coordz = coordz;
        this.shape = shape;

    }

    public void addLane(string id, int index, float speed, List<List<double>> shape, float width, bool yaya, bool tls, bool isTram, bool isBike, bool isGreen, bool isBus)
    {
        sumoLanes lane = new sumoLanes(id, index, speed, shape, width, yaya, tls, isTram, isBike, isGreen, isBus);
        lanes.Add(id, lane);
    }

    public void removeLane(string id)
    {
        lanes.Remove(id);
    }

}
