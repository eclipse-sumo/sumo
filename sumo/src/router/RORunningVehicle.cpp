#include <string>
#include <iostream>
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "RORunningVehicle.h"
#include "RORouteDef.h"

using namespace std;

RORunningVehicle::RORunningVehicle(const std::string &id, RORouteDef *route,
                                   long time, ROVehicleType *type,
                                   const std::string &lane, float pos,
                                   float speed)
    : ROVehicle(id, route, time, type),
    _lane(lane), _pos(pos), _speed(speed)
{
}


RORunningVehicle::~RORunningVehicle()
{
}



void RORunningVehicle::xmlOut(std::ostream &os) const {
    os << "<vehicle id=\"" << _id << "\"";
    os << " type=\"" << _type->getID() << "\"";
	os << " route=\"" << _route->getID() << "\"";
	os << " depart=\"" << _depart << "\"";
    os << " lane=\"" << _lane << "\"";
    os << " pos=\"" << _pos << "\"";
    os << " speed=\"" << _speed << "\"";
	os << "/>" << endl;
}
