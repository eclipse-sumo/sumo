#ifndef NIVissimDistrictConnection_h
#define NIVissimDistrictConnection_h

#include <map>
#include <string>
#include <utils/geom/Position2D.h>
#include <utils/common/IntVector.h>
#include <utils/common/DoubleVector.h>

class NIVissimDistrictConnection {
public:
    NIVissimDistrictConnection(int id, const std::string &name,
        const IntVector &districts, const DoubleVector &percentages,
        int edgeid, double position,
        const IntVector &assignedVehicles);
    ~NIVissimDistrictConnection();
    Position2D geomPosition() const;
    double getPosition() const {
        return myPosition;
    }

public:
    static bool dictionary(int id, const std::string &name,
        const IntVector &districts, const DoubleVector &percentages,
        int edgeid, double position, const IntVector &assignedVehicles);
    static bool dictionary(int id, NIVissimDistrictConnection *o);
    static NIVissimDistrictConnection *dictionary(int id);
    static void dict_BuildDistricts();
    static NIVissimDistrictConnection *dict_findForEdge(int edgeid);


private:
    int myID;
    std::string myName;
    IntVector myDistricts;
    typedef std::map<int, double> DistrictPercentages;
    DistrictPercentages myPercentages;
    int myEdgeID;
    double myPosition;
    IntVector myAssignedVehicles;
private:
    typedef std::map<int, NIVissimDistrictConnection*> DictType;
    static DictType myDict;
};

#endif
