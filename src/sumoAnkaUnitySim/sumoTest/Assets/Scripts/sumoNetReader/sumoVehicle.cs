using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;



public class sumoVehicle
{
    string id;
    public float coordx;
    public float coordy;
    public float coordz;
    public List<double> shape;
    public List<float> extent;
    public List<float> color;
    public int signal;
    public string type;
    public string vclass;

    public sumoVehicle(string id, float coordx, float coordy, float coordz, List<double> shape, int signal, List<float> color,  string type, string vclass, List<float> extent)
    {
        this.id = id;
        this.coordx = coordx;
        this.coordy = coordy;
        this.coordz = coordz;
        this.shape = shape;
        this.color = color;
        this.signal = signal;
        this.type = type;
        this.vclass = vclass;
        this.extent = extent;
    }

    public void updateCoords(float coordx, float coordy, float coordz)
    {
        this.coordx = coordx;
        this.coordy = coordy;
        this.coordz = coordz;
    }

    
}

