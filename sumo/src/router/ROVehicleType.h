#ifndef ROVehicleType_h
#define ROVehicleType_h

#include <iostream>
#include <string>
#include "ReferencedItem.h"

class ROVehicleType : public ReferencedItem {
protected:
    std::string _id;
public:
    ROVehicleType(const std::string &id);
	virtual ~ROVehicleType();
	virtual std::ostream &xmlOut(std::ostream &os);
    std::string getID() const;
};

#endif
