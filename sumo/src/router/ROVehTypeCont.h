#ifndef ROVehTypeCont_h
#define ROVehTypeCont_h

#include <string>
#include <map>
#include <utils/common/NamedObjectCont.h>

class ROVehicleType;

class ROVehTypeCont : public NamedObjectCont<ROVehicleType*> {
private:
    ROVehicleType *_defaultType;
public:
    ROVehTypeCont(ROVehicleType *defType);
    ~ROVehTypeCont();
    ROVehicleType *getDefault() const;
private:
    /// we made the copy constructor invalid
    ROVehTypeCont(const ROVehTypeCont &src);
    /// we made the assignment operator invalid
    ROVehTypeCont &operator=(const ROVehTypeCont &src);
};

#endif
