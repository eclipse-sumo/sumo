#include <string>
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "ROVehicleBuilder.h"


ROVehicleBuilder::ROVehicleBuilder()
{
}


ROVehicleBuilder::~ROVehicleBuilder()
{
}


ROVehicle *
ROVehicleBuilder::buildVehicle(const std::string &id, RORouteDef *route,
                               unsigned int depart, ROVehicleType *type,
                               const RGBColor &color, int period, int repNo)
{
    return new ROVehicle(*this, id, route, depart, type, color, period, repNo);
}


RORunningVehicle *
ROVehicleBuilder::buildRunningVehicle(const std::string &id,
                                      RORouteDef *route, long time,
                                      ROVehicleType *type,
                                      const std::string &lane,
                                      float pos, float speed,
                                      const RGBColor &col, int period,
                                      int repNo)
{
    return new RORunningVehicle(*this, id, route, time, type, lane, pos, speed,
        col, period, repNo);
}


