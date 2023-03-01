using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class sumoLanes 
{
    public string id;
    public int index;
    public float speed;
    public float width;
    public List<List<double>>  shape;
    public bool isYaya;
    public bool isTls;
    public bool isTram;
    public bool isBike;
    public bool isGreen;
    public bool isBus;

    public sumoLanes(string id, int index, float speed, List<List<double>> shape, float width, bool isYaya, bool isTls, bool isTram, bool isBike, bool isGreen, bool isBus)
    {
        this.id = id;
        this.index = index;
        this.speed = speed;
        this.width = width;
        this.shape = shape;
        this.isYaya = isYaya;
        this.isTls = isTls;
        this.isTram = isTram;
        this.isBike = isBike;
        this.isGreen = isGreen;
        this.isBus = isBus;
    }

    public void update(string id, int index, float speed, List<List<double>> shape,float width, bool isYaya, bool isTls, bool isTram, bool isBike, bool isGreen, bool isBus)
    {
        this.id = id;
        this.index = index;
        this.speed = speed;
        this.shape = shape;
        this.width = width;
        this.isYaya = isYaya;
        this.isTls = isTls;
        this.isTram = isTram;
        this.isBike = isBike;
        this.isBike = isGreen;
        this.isBike = isBus;

    }
}
