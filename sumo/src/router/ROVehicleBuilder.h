#ifndef ROVehicleBuilder_h
#define ROVehicleBuilder_h

#include <string>
#include <utils/common/SUMOTime.h>

class RORouteDef;
class ROVehicle;
class RORunningVehicle;
class RGBColor;
class ROVehicleType;

class ROVehicleBuilder {
public:
    ROVehicleBuilder();

    virtual ~ROVehicleBuilder();

    virtual ROVehicle *buildVehicle(const std::string &id, RORouteDef *route,
        unsigned int depart, ROVehicleType *type,
        const RGBColor &color, int period, int repNo);

    virtual RORunningVehicle *buildRunningVehicle(const std::string &id,
        RORouteDef *route, SUMOTime time, ROVehicleType *type,
        const std::string &lane, float pos, float speed,
        const RGBColor &col, int period, int repNo);


};


#endif
