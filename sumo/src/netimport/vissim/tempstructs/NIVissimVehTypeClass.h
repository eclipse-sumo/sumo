#ifndef NIVissimVehTypeClass_h
#define NIVissimVehTypeClass_h

#include <string>
#include <map>
#include <utils/gfx/RGBColor.h>
#include <utils/common/IntVector.h>

class NIVissimVehTypeClass {
public:
    NIVissimVehTypeClass(int id, const std::string &name,
        const RGBColor &color, IntVector &types);
    ~NIVissimVehTypeClass();
    static bool dictionary(int id, const std::string &name,
        const RGBColor &color, IntVector &types);
    static bool dictionary(int id, NIVissimVehTypeClass *o);
    static NIVissimVehTypeClass *dictionary(int name);
private:
    int myID;
    std::string myName;
    RGBColor myColor;
    IntVector myTypes;
private:
    typedef std::map<int, NIVissimVehTypeClass*> DictType;
    static DictType myDict;
};

#endif

