#include <iostream>
#include "MSVehicle.h"
#include "MSCORN.h"

using namespace std;

std::ostream *MSCORN::myTripInfoOut = 0;
bool MSCORN::myWished[CORN_MAX];

void
MSCORN::init()
{
    myTripInfoOut = 0;
    for(int i=0; i<CORN_MAX; i++) {
        myWished[i] = false;
    }
}


void
MSCORN::clear()
{
}



bool
MSCORN::wished(Function f)
{
    return myWished[(int) f];
}


void
MSCORN::setWished(Function f)
{
    myWished[(int) f] = true;
    switch(f) {
    case CORN_OUT_TRIPOUTPUT:
        setWished(CORN_VEH_REALDEPART);
        break;
    case CORN_OUT_EMISSIONS:
        setWished(CORN_VEHCONTROL_WANTS_DEPARTURE_INFO);
        setWished(CORN_MEAN_VEH_TRAVELTIME);
        setWished(CORN_MEAN_VEH_WAITINGTIME);
        break;
    case CORN_VEH_REALDEPART:
        break;
    case CORN_MEAN_VEH_TRAVELTIME:
        setWished(CORN_VEH_REALDEPART);
        break;
    case CORN_MEAN_VEH_WAITINGTIME:
        setWished(CORN_VEH_REALDEPART);
        break;
    case CORN_VEHCONTROL_WANTS_DEPARTURE_INFO:
        break;
    default:
        break;
    }
}


void
MSCORN::setTripInfoOutput(std::ostream *s)
{
    myTripInfoOut = s;
}


void
MSCORN::compute_TripInfoOutput(MSVehicle *v)
{
    (*myTripInfoOut)
        << "<tripinfo id=\"" << v->id() << "\" "
        << "start=\"" << v->getCORNDoubleValue(CORN_VEH_REALDEPART) << "\" "
        << "wished=\"" << v->desiredDepart() << "\" "
        << "end=\"" << MSNet::getInstance()->getCurrentTimeStep()
        << "\"/>" << endl;
}
