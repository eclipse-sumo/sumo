#ifndef NIVissimTrafficDescription_h
#define NIVissimTrafficDescription_h

#include <string>
#include <map>
#include "NIVissimVehicleClassVector.h"

class NIVissimTrafficDescription {
public:
    NIVissimTrafficDescription(const std::string &id, const std::string &name,
        const NIVissimVehicleClassVector &vehicleTypes);
    ~NIVissimTrafficDescription();
    static bool dictionary(const std::string &id, const std::string &name,
        const NIVissimVehicleClassVector &vehicleTypes);
    static bool dictionary(const std::string &id, NIVissimTrafficDescription *o);
    static NIVissimTrafficDescription *dictionary(const std::string &id);
    static void clearDict();
private:
    std::string myID;
    std::string myName;
    NIVissimVehicleClassVector myVehicleTypes;
private:
    typedef std::map<std::string, NIVissimTrafficDescription*> DictType;
    static DictType myDict;
};

#endif

