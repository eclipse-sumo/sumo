#include "ROSUMOHandlerBase.h"
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "RONet.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/gfx/GfxConvHelper.h>
#include "ROVehicleType_Krauss.h"


using namespace std;


ROSUMOHandlerBase::ROSUMOHandlerBase(RONet &net,
                                     const std::string &dataName,
                                     const std::string &file)
    : ROTypedXMLRoutesLoader(net, file),
    myDataName(dataName)
{
}


ROSUMOHandlerBase::~ROSUMOHandlerBase()
{
}


float
ROSUMOHandlerBase::getFloatReporting(const Attributes &attrs, AttrEnum attr,
                                     const std::string &id,
                                     const std::string &name)
{
    try {
        return getFloat(attrs, attr);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(string("Missing ") + name + string(" in vehicle '") +
            id + string("'."));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(name + string(" in vehicle '") +
            id + string("' is not numeric."));
    }
    return -1;
}


/*
RGBColor
ROSUMOHandlerBase::parseColor(const Attributes &attrs,
                              const std::string &id)
{
    try {
        return GfxConvHelper::parseColor(getString(attrs, SUMO_ATTR_COLOR));
    } catch (EmptyData) {
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(string("Color in vehicle '")
            + id + string("' is not numeric."));
    }
    return RGBColor(-1, -1, -1);
}
*/


ROVehicleType*
ROSUMOHandlerBase::getVehicleType(const Attributes &attrs,
                                  const std::string &id)
{
    ROVehicleType *type = 0;
    try {
        string name = getString(attrs, SUMO_ATTR_TYPE);
        type = _net.getVehicleType(name);
        if(type==0) {
            MsgHandler::getErrorInstance()->inform(string("The type of the vehicle '") +
                name + string("' is not known."));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(string("Missing type in vehicle '") +
            id + string("'."));
    }
    return type;
}


long
ROSUMOHandlerBase::getVehicleDepartureTime(const Attributes &attrs,
                                           const std::string &id)
{
    long time = -1;
    try {
        time = getLong(attrs, SUMO_ATTR_DEPART);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(string("Missing departure time in vehicle '") +
            id + string("'."));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(string("Non-numerical departure time in vehicle '") +
            id + string("'."));
    }
    return time;
}


RORouteDef *
ROSUMOHandlerBase::getVehicleRoute(const Attributes &attrs,
                                   const std::string &id)
{
    RORouteDef *route = 0;
    try {
        string name = getString(attrs, SUMO_ATTR_ROUTE);
        route = _net.getRouteDef(name);
        if(route==0) {
            MsgHandler::getErrorInstance()->inform(string("The route of the vehicle '") +
                name + string("' is not known."));
            return 0;
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(string("Missing route in vehicle '") +
            id + string("'."));
    }
    return route;
}


RGBColor
ROSUMOHandlerBase::parseColor(const Attributes &attrs,
                              const std::string &type,
                              const std::string &id)
{
    RGBColor col;
    try {
        col = GfxConvHelper::parseColor(getString(attrs, SUMO_ATTR_COLOR));
    } catch (EmptyData) {
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            string("The color definition for ") + type + (" '") +
            id + string("' is malicious."));
    }
    return col;
}


std::string
ROSUMOHandlerBase::getDataName() const
{
    return myDataName;
}


void
ROSUMOHandlerBase::startVehicle(const Attributes &attrs)
{
    // get the vehicle id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing id in vehicle.");
        return;
    }
    // get vehicle type
    ROVehicleType *type = getVehicleType(attrs, id);
    // get the departure time
    long time = getVehicleDepartureTime(attrs, id);
    // get the route id
    RORouteDef *route = getVehicleRoute(attrs, id);
    // get the vehicle color
    RGBColor color = parseColor(attrs, "vehicle", id);
    // build the vehicle
    // get further optional information
    int repOffset = getIntSecure(attrs, SUMO_ATTR_PERIOD, -1);
    int repNumber = getIntSecure(attrs, SUMO_ATTR_REPNUMBER, -1);
    if(!MsgHandler::getErrorInstance()->wasInformed()) {
        _net.addVehicle(id,
            new ROVehicle(id, route, time, type, color,
                repOffset, repNumber));
        _currentTimeStep = time;
    }
}


void
ROSUMOHandlerBase::startVehType(const Attributes &attrs)
{
    // get the vehicle type id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing id in vtype.");
        return;
    }
    // get the other values
    float maxspeed = getFloatReporting(attrs, SUMO_ATTR_MAXSPEED, id, "maxspeed");
    float length = getFloatReporting(attrs, SUMO_ATTR_LENGTH, id, "length");
    float accel = getFloatReporting(attrs, SUMO_ATTR_ACCEL, id, "accel");
    float decel = getFloatReporting(attrs, SUMO_ATTR_DECEL, id, "decel");
    float sigma = getFloatReporting(attrs, SUMO_ATTR_SIGMA, id, "sigma");
    RGBColor color = parseColor(attrs, "vehicle type", id);
    // build the vehicle type after checking
    //  by now, only vehicles using the krauss model are supported
    if(maxspeed>0&&length>0&&accel>0&&decel>0&&sigma>0) {
        _net.addVehicleType(
            new ROVehicleType_Krauss(
                id, color, length, accel, decel, sigma, maxspeed));
    }
}


