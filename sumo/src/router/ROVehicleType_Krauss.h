#ifndef ROVehicleType_Krauss_h
#define ROVehicleType_Krauss_h

#include <string>
#include <iostream>
#include "ROVehicleType.h"

class ROVehicleType_Krauss : public ROVehicleType {
private:
	double _a, _b, _eps, _length, _maxSpeed;
public:
    static double _defA, _defB, _defEPS, _defLENGTH, _defMAXSPEED;
public:
    ROVehicleType_Krauss();
	ROVehicleType_Krauss(const std::string &id, double a, double b, double eps, double length, double maxSpeed);
	~ROVehicleType_Krauss();
	std::ostream &xmlOut(std::ostream &os);
};

#endif
