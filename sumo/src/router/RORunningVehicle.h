#ifndef RORunningVehicle_h
#define RORunningVehicle_h

#include <string>
#include <iostream>
#include "ROVehicle.h"

class RORunningVehicle : public ROVehicle {
private:
    std::string _lane;
    float _pos;
    float _speed;
public:
    RORunningVehicle(const std::string &id, RORouteDef *route, long time,
        ROVehicleType *type,
        const std::string &lane, float pos, float speed);
    ~RORunningVehicle();
    void xmlOut(std::ostream &os) const;
};

#endif
