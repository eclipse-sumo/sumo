#include <iostream>
#include <utils/common/MsgHandler.h>
#include "MSLane.h"
#include "MSVehicle.h"
#include "MSVehicleTransfer.h"

using namespace std;

MSVehicleTransfer *MSVehicleTransfer::myInstance = 0;

void
MSVehicleTransfer::addVeh(MSLane &from)
{
    // remove the vehicle from the lane
    MSVehicle *veh = from.removeFirstVehicle(*this);
    // get the current edge of the vehicle
    MSEdge *e = MSEdge::dictionary(veh->getEdge()->id());
//    cout << "Vehicle beam init on edge '" << e->id() << "'." << endl;
//    cout << " Time: " <<  MSNet::getInstance()->getCurrentTimeStep()
//        << " Vehicle: " << veh->id() << endl;
    MsgHandler::getWarningInstance()->inform(
        string("Vehicle '") + veh->id() + string("' will be teleported; edge '")
        + e->id() + string("'."));
    // let the vehicle be on the one
    if(veh->proceedVirtualReturnIfEnded(*this, MSEdge::dictionary(veh->succEdge(1)->id()))) {
        MSVehicle::remove(veh->id());
        return;
    }
    // mark the next one
    e = MSEdge::dictionary(veh->getEdge()->id());
    myNoTransfered++;
//    cout << " In container:" << myVehicles.size() << " Transfers done: "
//        << myNoTransfered << endl;
    assert(e!=0);
    // save information
    myVehicles.push_back(
        VehicleInformation(veh, MSNet::getInstance()->getCurrentTimeStep(), e));
//    cout << "Vehicle is virtually on edge '" << e->id() << "'" << endl;
}


void
MSVehicleTransfer::checkEmissions(MSNet::Time time)
{
    // go through vehicles
    for(VehicleInfVector::iterator i=myVehicles.begin(); i!=myVehicles.end(); ) {
        // get the vehicle information
        VehicleInformation &desc = *i;
        MSEdge *e = desc.myNextPossibleEdge;
        // check whether the vehicle may be emitted onto a following edge
        if(e->emit(*(desc.myVeh))) {
            // remove from this if so
            MsgHandler::getWarningInstance()->inform(
                string("Vehicle '") + desc.myVeh->id()
                + string("' ends teleporting on edge '") + e->id()
                + string("'."));
            i = myVehicles.erase(i);
//            cout << "Vehicle beam end '" << e->id() << "'." << endl;
//            cout << " Time: " <<  time
//                << " Vehicle: " << desc.myVeh->id() << endl;
//            cout << " In container:" << myVehicles.size() << endl;
        } else {
            // otherwise, check whether a consecutive edge may be used
            if(desc.myProceedTime<time) {
//                cout << "Vehicle '" << desc.myVeh->id() << "' proceeds from '"
//                    << desc.myNextPossibleEdge->id() << "'";
                // get the lanes of the next edge (the one the vehicle wiil be
                //  virtually on after all these computations)
                MSLane *tmp = *(desc.myNextPossibleEdge->getLanes()->begin());
                // get the one beyond the one the vehicle moved to
                MSEdge *nextEdge = MSEdge::dictionary(desc.myVeh->succEdge(1)->id());
                // let the vehicle move to the next edge
                if(desc.myVeh->proceedVirtualReturnIfEnded(*this, nextEdge)) {
                    MSVehicle::remove(desc.myVeh->id());
                    MsgHandler::getWarningInstance()->inform(
                        string("Vehicle '") + desc.myVeh->id()
                        + string("' ends teleporting on end edge '") + e->id()
                        + string("'."));
                    i = myVehicles.erase(i);
                    continue;
                }
                desc.myNextPossibleEdge = nextEdge;
                // get the time the vehicle needs to pass the current edge
                desc.myProceedTime = time + tmp->length() / tmp->maxSpeed()
                    * 2.0; // !!! maybe, the time should be compued in other ways
//                cout << " to '" << nextEdge->id() << "'." << endl;
            }
            i++;
        }

    }
}


MSVehicleTransfer *
MSVehicleTransfer::getInstance()
{
    if(myInstance==0) {
        myInstance = new MSVehicleTransfer();
    }
    return myInstance;
}


MSVehicleTransfer::MSVehicleTransfer()
    : myNoTransfered(0)
{
}


MSVehicleTransfer::~MSVehicleTransfer()
{
    myInstance = 0;
}


