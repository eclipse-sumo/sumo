using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class paths 
{
     // vehicle paths 
    //public static string moskvitch = "models/vehicle/Moskvitch/moskvitch";
    //public static string[] moskvitchTexture = new string[]
    //{
    //     "models/vehicle/Moskvitch/beige.jpg",
    //     "models/vehicle/Moskvitch/black.jpg",
    //     "models/vehicle/Moskvitch/blue.jpg",
    //     "models/vehicle/Moskvitch/green.jpg",
    //     "models/vehicle/Moskvitch/normal.jpg",
    //     "models/vehicle/Moskvitch/specular.jpg",
    //     "models/vehicle/Moskvitch/white.jpg",
    //     "models/vehicle/Moskvitch/orange.jpg"
    //   };

    //public static string ambulance = "models/vehicle/Ambulance/Ambulance";
    //public static string Bus = "models/vehicle/Bus/Bus";
    //public static string Firetruck = "models/vehicle/Firetruck/Firetruck";
    //public static string Hatchback = "models/vehicle/Hatchback/Hatchback";
    //public static string Limousine = "models/vehicle/Limousine/Limousine";
    //public static string Monster = "models/vehicle/Monster/Monster";
    //public static string Moskvitch = "models/vehicle/Moskvitch/Moskvitch";
    //public static string Muscle = "models/vehicle/Muscle/Muscle";
    //public static string Muscle2 = "models/vehicle/Muscle2/Muscle2";
    //public static string Pickup = "models/vehicle/Pickup/Pickup";
    //public static string Police = "models/vehicle/Police/Police";
    //public static string Police2 = "models/vehicle/Police2/Police2";
    //public static string Roadster = "models/vehicle/Roadster/Roadster";
    //public static string Sedan = "models/vehicle/Sedan/Sedan";
    //public static string Sports = "models/vehicle/Sports/Sports";
    //public static string SUV = "models/vehicle/SUV/SUV";
    //public static string Taxi = "models/vehicle/Taxi/Taxi";
    //public static string Truck = "models/vehicle/Truck/Truck";
    //public static string Truck2 = "models/vehicle/Truck2/Truck2";
    //public static string Van = "models/vehicle/Van/Van";
    //public static string w2222 = "models/vehicle/w2222/w2222";


    /// new Vehicles
    /// 
    public static string[] emergency = { "reOrdered/Ambulance_1A", "reOrdered/Firetruck_1A"  };
    public static string[] bus = { "reOrdered/Bus_1A", "reOrdered/Bus_1B", "reOrdered/Bus_1C" };
    public static string[] car = { "reOrdered/Compact_1A", "reOrdered/Compact_1B", "reOrdered/Compact_2A",
    "reOrdered/Compact_2A","reOrdered/Compact_2B","reOrdered/Coupe_1A","reOrdered/Coupe_1B",
    "reOrdered/Coupe_1C", "reOrdered/Coupe_2A","reOrdered/PickUp_1A","reOrdered/Sedan_2A",
     "reOrdered/Sedan_3A","reOrdered/Sedan_4A", "reOrdered/Sedan_4B","reOrdered/Supersport_1A"};
    public static string[] police = { "reOrdered/Interceptor_1A", "reOrdered/Interceptor_2A" };
    public static string[] motorcycle = { "reOrdered/Scooter_1A" };
    public static string[] authority = { "reOrdered/Sedan_1A" };
    public static string[] taxi = { "reOrdered/Taxi_1A", "reOrdered/Taxi_1B" };
    public static string[] van = { "reOrdered/Van_1A" };
    public static string[] train = { "reOrdered/train" };
    public static string[] army = { "reOrdered/army" };
    public static string[] truck = { "reOrdered/truck" };
    


    //////// road metarial
    ///textures
    public static string roadMetarialPath = "VRoad/Materials/Lanes/Textures/";
    public static string roadEdget = "VRoad/Materials/Lanes/Textures/LaneWith_RoadEdge_tex";
    public static string roadMidt = "VRoad/Materials/Lanes/Textures/LaneWith_LaneWith_tex";
    public static string road1t = "VRoad/Materials/Lanes/Textures/RoadEdge_RoadEdge_tex";

    ///materials
    public static string roadEdge = "VRoad/Materials/Lanes/LaneWith_RoadEdge";
    public static string roadMid = "VRoad/Materials/Lanes/LaneWith_LaneWith";
    public static string road1 = "VRoad/Materials/Lanes/RoadEdge_RoadEdge";
    public static string roadyaya = "VRoad/Materials/Lanes/pavement2";
    public static string roadBike = "VRoad/Materials/Lanes/pavement7";
    public static string roadtrain = "VRoad/Materials/Lanes/railway";
    public static string roadgreen = "VRoad/Materials/Lanes/pavement20";
    public static string roadBus = "VRoad/Materials/Lanes/Lane_ForBus";



    ////////// person
    public static string yaya = "SuitedMan/Prefabs/person";
    public static string yaya2 = "models/person/human1";



    ////////////road assets
    public static string tLight = "models/traffic_light/traffic_light_a.FBX";
    public static string tLight2 = "models/traffic_light/trafficLight";


    /////////// Traffic ligths assets
    public static string redMat = "models/traffic_light/Materials/red_mat";
    public static string yellowMat = "models/traffic_light/Materials/yellow_mat";
    public static string greenMat = "models/traffic_light/Materials/green_mat";
    public static string redLight = "models/traffic_light/Materials/red_light";
    public static string yellowLight = "models/traffic_light/Materials/yellow_light";
    public static string greenLight = "models/traffic_light/Materials/green_light";

    public static string green = "models/traffic_light/Materials/SignalGreen";
    public static string red = "models/traffic_light/Materials/SignalRed";
    public static string yellow = "models/traffic_light/Materials/SignalYellow";
    public static string off = "models/traffic_light/Materials/SignalOff";



    ////////// Trees 
    public static string trees = "NatureStarterKit2/Nature/";
    public static string ground1 = "NatureStarterKit2/Textures/ground01";
    public static string ground2 = "NatureStarterKit2/Textures/ground02";

    ////////// Houses 
    /// <summary>
    /// 
    /// 
    /// </summary>
    public static string[] buildings = { "models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01b_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01c_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_4et_01a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_4et_01_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_01a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_01_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_02a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_02_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03b_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03c_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03d_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03e_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03f_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_04_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_05_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_06_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_01a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_01b_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_01_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_02a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_02b_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_02_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_03a_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_03b_low",
"models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_03_low" };

    public static string apartment1 = "models/houses/apartment1.fbx";
    public static string apartment2 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01a_low";
    public static string apartment3 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01b_low";
    public static string apartment4 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01c_low";
    public static string apartment5 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_2et_01_low";
    public static string apartment6 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_4et_01a_low";
    public static string apartment7 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_4et_01_low";
    public static string apartment8 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_01a_low";
    public static string apartment9 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_01_low";
    public static string apartment10 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_02a_low";
    public static string apartment11 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_02_low";
    public static string apartment12 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03a_low";
    public static string apartment13 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03b_low";
    public static string apartment14 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03c_low";
    public static string apartment15 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03d_low";
    public static string apartment16 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03e_low";
    public static string apartment17 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03f_low";
    public static string apartment18 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_03_low";
    public static string apartment19 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_04_low";
    public static string apartment20 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_05_low";
    public static string apartment21 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_5et_06_low";
    public static string apartment22 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_01a_low";
    public static string apartment23 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_01b_low";
    public static string apartment24 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_01_low";
    public static string apartment25 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_02a_low";
    public static string apartment26 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_02b_low";
    public static string apartment27 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_02_low";
    public static string apartment28 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_03a_low";
    public static string apartment29 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_03b_low";
    public static string apartment30 = "models/houses/russian_buildings/prefabs/buildings_low/rus_build_9et_03_low";






}

