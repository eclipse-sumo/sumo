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
        CORN_MAX = 5,
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
