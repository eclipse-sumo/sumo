#ifndef MSCORN_H
#define MSCORN_H

#include <iostream>

class MSVehicle;

class MSCORN {
public:
    enum Function {
        CORN_OUT_TRIPOUTPUT = 0,
        CORN_OUT_EMISSIONS = 1,
        CORN_VEH_REALDEPART = 2,
        CORN_MEAN_VEH_TRAVELTIME = 3,
        CORN_MEAN_VEH_WAITINGTIME = 4,
        CORN_VEHCONTROL_WANTS_DEPARTURE_INFO = 6,
        CORN_VEHCONTROL_WANTS_EDGECHANGE_INFO = 7,
        CORN_VEH_WASREROUTET = 8,
        CORN_VEH_OLDROUTE1 = 9,
        CORN_VEH_OLDROUTE2 = 10,
        CORN_VEH_OLDROUTE3 = 11,
        CORN_VEH_OLDROUTE4 = 12,
        CORN_VEH_OLDROUTE5 = 13,
        CORN_VEH_OLDROUTE6 = 14,
        CORN_VEH_SAVEREROUTING = 15,
        CORN_VEH_LASTREROUTEOFFSET = 16,
        CORN_VEH_NUMBREROUTE = 17,
        CORN_MAX = 18,
    };

    static void init();
    static void clear();
    static bool wished(Function f);
    static void setWished(Function f);

public:
    static void setTripInfoOutput(std::ostream *s);
    static void compute_TripInfoOutput(MSVehicle *v);


private:
    static std::ostream *myTripInfoOut;
    static bool myWished[CORN_MAX];

private:
    MSCORN();
    ~MSCORN();
};


#endif
