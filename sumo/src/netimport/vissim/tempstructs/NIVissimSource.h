#ifndef NIVissimSource_h
#define NIVissimSource_h

#include <string>
#include <map>

class NIVissimSource {
public:
    NIVissimSource(const std::string &id, const std::string &name,
        const std::string &edgeid, double q, bool exact,
        int vehicle_combination, double beg, double end);
    ~NIVissimSource();
    static bool dictionary(const std::string &id, const std::string &name,
        const std::string &edgeid, double q, bool exact,
        int vehicle_combination, double beg, double end);
    static bool dictionary(const std::string &id, NIVissimSource *o);
    static NIVissimSource *dictionary(const std::string &id);
private:
    std::string myID;
    std::string myName;
    std::string myEdgeID;
    double myQ;
    bool myExact;
    int myVehicleCombination;
    double myTimeBeg;
    double myTimeEnd;

private:
    typedef std::map<std::string, NIVissimSource*> DictType;
    static DictType myDict;
};


#endif
