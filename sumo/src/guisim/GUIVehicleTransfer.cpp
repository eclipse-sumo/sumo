#include <iostream>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <gui/GUIGlObjectStorage.h>
#include <microsim/MSVehicle.h>
#include <guisim/GUINet.h>
#include "GUIVehicle.h"
#include "GUIVehicleTransfer.h"

using namespace std;


GUIVehicleTransfer::GUIVehicleTransfer()
{
}


void
GUIVehicleTransfer::removeVehicle(const std::string &id)
{
    MSVehicle *veh = MSVehicle::dictionary(id);
    static_cast<GUIVehicle*>(veh)->setRemoved();
    static_cast<GUINet*>(MSNet::getInstance())->getIDStorage().remove(
        static_cast<GUIVehicle*>(veh)->getGlID());
}
