#include <iostream>
#include <string>
#include "ReferencedItem.h"
#include "ROVehicleType.h"

ROVehicleType::ROVehicleType(const std::string &id)
    : ReferencedItem(), _id(id)
{
}

ROVehicleType::~ROVehicleType()
{
}

std::ostream &
ROVehicleType::xmlOut(std::ostream &os)
{
	return os;
}

std::string
ROVehicleType::getID() const {
    return _id;
}
