#ifndef NIVissimVehicleType_h
#define NIVissimVehicleType_h

#include <utils/gfx/RGBColor.h>
#include <string>
#include <map>

class NIVissimVehicleType {
public:
    NIVissimVehicleType(int id, const std::string &name,
        const std::string &category, double length, const RGBColor &color,
        double amax, double dmax);
    ~NIVissimVehicleType();
    static bool dictionary(int id, const std::string &name,
        const std::string &category, double length, const RGBColor &color,
        double amax, double dmax);
    static bool dictionary(int id, NIVissimVehicleType *o);
    static NIVissimVehicleType *dictionary(int id);
private:
    int myID;
    std::string myName;
    std::string myCategory;
    double myLength;
    RGBColor myColor;
    double myAMax, myDMax;
private:
    typedef std::map<int, NIVissimVehicleType *> DictType;
    static DictType myDict;
};

#endif
