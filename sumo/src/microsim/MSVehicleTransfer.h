#ifndef MSVehicleTransfer_h
#define MSVehicleTransfer_h


#include <vector>
#include "MSNet.h"

class MSLane;
class MSVehicle;

class MSVehicleTransfer
{
public:
    void addVeh(MSLane &firstFrom);
    void checkEmissions(MSNet::Time time);
    static MSVehicleTransfer *getInstance();
    ~MSVehicleTransfer();

private:
    MSVehicleTransfer();

    struct VehicleInformation {
        MSVehicle *myVeh;
        MSNet::Time myInsertTime;
        MSNet::Time myProceedTime;
        MSEdge *myNextPossibleEdge;

        VehicleInformation(MSVehicle *veh, MSNet::Time insertTime, MSEdge *e)
            : myVeh(veh), myInsertTime(insertTime), myProceedTime(insertTime),
            myNextPossibleEdge(e)
        { }
    };

    typedef std::vector<VehicleInformation> VehicleInfVector;
    VehicleInfVector myVehicles;
    size_t myNoTransfered;
    static MSVehicleTransfer *myInstance;
};

#endif
