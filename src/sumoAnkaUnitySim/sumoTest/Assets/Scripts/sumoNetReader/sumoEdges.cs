using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class sumoEdges 
{
    string id;
    int priority;

    sumoNodes nodeFrom;
    sumoNodes nodeTo;
    public List< sumoLanes> lanes = new List<sumoLanes>();

    public sumoEdges(string id, int priority, sumoNodes nodeFrom, sumoNodes nodeTo)
    {
        this.id = id;
        this.priority = priority;
        this.nodeFrom = nodeFrom;
        this.nodeTo = nodeTo;
    }

    public int getPriorty()
    {
        return this.priority;
    }

    public List<sumoLanes> getlines()
    {
        return this.lanes;
    }

    public sumoNodes getNodeFrom()
    {
        return nodeFrom;
    }

    public sumoNodes getNodeTo()
    {
        return nodeTo;
    }

    public string getId()
    {
        return this.id;
    }
    public void addLane(string id, int index, float speed, List<List<double>> shape, float width, bool yaya, bool tls, bool isTram, bool isBike, bool isGreen, bool isBus)
    {
        sumoLanes lane = new sumoLanes(id, index, speed, shape, width,yaya,tls,isTram, isBike, isGreen, isBus);
        lanes.Add( lane);
    }

  


}
