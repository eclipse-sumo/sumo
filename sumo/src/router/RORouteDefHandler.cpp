#include <string>
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include "RORouteDef.h"
#include "RONet.h"
#include "ROOrigDestRouteDef.h"
#include "RORouteDefHandler.h"
#include "RORouteDefList.h"
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "ROTypedRoutesLoader.h"

using namespace std;

RORouteDefHandler::RORouteDefHandler(RONet &net)
    : ROTypedXMLRoutesLoader(net)
{
}


RORouteDefHandler::RORouteDefHandler(RONet &net,
                                     const std::string &fileName)
    : ROTypedXMLRoutesLoader(net, fileName)
{
/*    _attrHandler.add(SUMO_ATTR_id, "id");
    _attrHandler.add(SUMO_ATTR_time, "time");
    _attrHandler.add(SUMO_ATTR_from, "from");
    _attrHandler.add(SUMO_ATTR_to, "to");
    _attrHandler.add(SUMO_ATTR_type, "type");
    _attrHandler.add(SUMO_ATTR_speed, "speed");
    _attrHandler.add(SUMO_ATTR_pos, "pos");
    _attrHandler.add(SUMO_ATTR_lane, "lane");*/
}


RORouteDefHandler::~RORouteDefHandler()
{
}

void RORouteDefHandler::myStartElement(int element, const std::string &name,
                                      const Attributes &attrs)
{
    if(element==SUMO_TAG_TRIPDEF) {
        // get the vehicle id, the edges, the speed and position and
        //  the departure time
        string id = getVehicleID(attrs);
        ROEdge *from = getEdge(attrs, "origin", SUMO_ATTR_FROM, id);
        ROEdge *to = getEdge(attrs, "destination", SUMO_ATTR_TO, id);
        string typeID = getVehicleType(attrs);
        float pos = getOptionalFloat(attrs, "pos", SUMO_ATTR_POS, id);
        float speed = getOptionalFloat(attrs, "speed", SUMO_ATTR_SPEED, id);
        long time = getDepartureTime(attrs, id);
        string lane = getLane(attrs);
        // recheck attributes
        if(from==0||to==0||time<0)
            return;
        // add the vehicle type, the vehicle and the route to the net
        RORouteDef *route = new ROOrigDestRouteDef(id, from, to);
        ROVehicleType *type = _net.addVehicleType(typeID);
        if(pos>0||speed>0) {
            _net.addVehicle(id, new RORunningVehicle(id, route, time, type, lane, pos, speed));
        } else {
            _net.addVehicle(id, new ROVehicle(id, route, time, type));
        }
        _net.addRouteDef(route);
        _netRouteRead = true;
        _currentTimeStep = time;
    }
}


std::string
RORouteDefHandler::getVehicleID(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch(EmptyData) {
    }
    // get a valid vehicle id
    while(id.length()==0) {
        string tmp = _idSupplier.getNext();
        if(!_net.isKnownVehicleID(tmp))
            id = tmp;
    }
        // and save this vehicle id
    _net.addVehicleID(id);
    return id;
}


ROEdge *
RORouteDefHandler::getEdge(const Attributes &attrs, const std::string &purpose,
                           AttrEnum which, const string &id)
{
    try {
        string id = getString(attrs, which);
        return _net.getEdge(id);
    } catch(EmptyData) {
        SErrorHandler::add(string("Missing ") +
            purpose + string(" edge in description of a route."));
        if(id.length()!=0) {
            SErrorHandler::add(string(" Vehicle id='") + id + string("'."));
        }
    }
    return 0;
}

std::string
RORouteDefHandler::getVehicleType(const Attributes &attrs)
{
    try {
        return getString(attrs, SUMO_ATTR_TYPE);
    } catch(EmptyData) {
        return "SUMO_DEFAULTTYPE";
    }
}

float
RORouteDefHandler::getOptionalFloat(const Attributes &attrs,
                                    const std::string &name,
                                    AttrEnum which,
                                    const std::string &place)
{
    try {
        return getFloat(attrs, SUMO_ATTR_POS);
    } catch (EmptyData) {
    } catch (NumberFormatException) {
        SErrorHandler::add(string("The value of '") + name +
            string("' should be numeric but is not."));
        if(place.length()!=0)
            SErrorHandler::add(string(" Route id='") + place + string("'"));
    }
    return -1;
}

long
RORouteDefHandler::getDepartureTime(const Attributes &attrs,
                                    const std::string &id)
{
    // get the departure time
    try {
        return getLong(attrs, SUMO_ATTR_DEPART);
    } catch(EmptyData) {
        SErrorHandler::add("Missing departure time in description of a route.");
        if(id.length()!=0)
            SErrorHandler::add(string(" Vehicle id='") + id + string("'."));
    } catch (NumberFormatException) {
        SErrorHandler::add("The value of the departure time should be numeric but is not.");
        if(id.length()!=0)
            SErrorHandler::add(string(" Route id='") + id + string("'"));
    }
    return -1;
}


string
RORouteDefHandler::getLane(const Attributes &attrs)
{
    try {
        return getString(attrs, SUMO_ATTR_LANE);
    } catch (EmptyData) {
        return "";
    }
}


void
RORouteDefHandler::myCharacters(int element, const std::string &name,
                               const std::string &chars)
{
}


void
RORouteDefHandler::myEndElement(int element, const std::string &name)
{
}

ROTypedRoutesLoader *
RORouteDefHandler::getAssignedDuplicate(const std::string &file) const
{
    return new RORouteDefHandler(_net, file);
}

std::string
RORouteDefHandler::getDataName() const {
    return "XML-route definitions";
}


