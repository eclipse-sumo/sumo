#ifndef ROVehicle_h
#define ROVehicle_h

#include <string>
#include <iostream>

class ROVehicleType;
class RORouteDef;

class ROVehicle {
protected:
	std::string _id;
	ROVehicleType *_type;
	RORouteDef *_route;
	long _depart;
public:
	ROVehicle(const std::string &id, RORouteDef *route, long depart,
        ROVehicleType *type);
	virtual ~ROVehicle();
    RORouteDef *getRoute() const;
    ROVehicleType *getType() const;
	virtual void xmlOut(std::ostream &os) const;
    std::string getID() const;
    long getDepartureTime() const;
};

#endif

