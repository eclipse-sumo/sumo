#include <string>
#include <iostream>
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "ROVehicle.h"

using namespace std;

ROVehicle::ROVehicle(const std::string &id, RORouteDef *route, long depart,
                     ROVehicleType *type)
	: _id(id), _type(type), _route(route), _depart(depart)
{
}

ROVehicle::~ROVehicle()
{
}


RORouteDef *
ROVehicle::getRoute() const
{
    return _route;
}


ROVehicleType *
ROVehicle::getType() const
{
    return _type;
}


void ROVehicle::xmlOut(std::ostream &os) const
{
    os << "<vehicle id=\"" << _id << "\"";
    os << " type=\"" << _type->getID() << "\"";
	os << " route=\"" << _route->getID() << "\"";
	os << " depart=\"" << _depart << "\"";
	os << "/>" << endl;
}

std::string
ROVehicle::getID() const
{
    return _id;
}

long
ROVehicle::getDepartureTime() const
{
    return _depart;
}
