#ifndef NIVissimTL_h
#define NIVissimTL_h

#include <map>
#include <string>
#include <vector>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/AbstractPoly.h>
#include <utils/common/IntVector.h>
#include <utils/common/DoubleVector.h>
#include "NIVissimBoundedClusterObject.h"

class NBNode;

class NIVissimTL
        : public NIVissimBoundedClusterObject {
public:
    NIVissimTL(int id, const std::string &type, const std::string &name,
        double absdur, double offset);
    ~NIVissimTL();
/*
    void addTrafficLight(int id,const std::string &name,
        int groupid, int edgeid, int laneno,
        double position, const IntVector &vehicleTypes);
        */
/*
    void openGroup(int id, const std::string &name);
    void currentGroup_setStaticGreen();
    void currentGroup_setStaticRed();
    void currentGroup_addTimes(const DoubleVector &times);
    void currentGroup_addYellowTimes(double tredyellow, double tyellow);
    void endGroup();
*/
    void computeBounding();
    int buildNodeCluster();
    std::string getType() const;
    int getID() const;
    void setNodeID(int id);
//    void assignNode();

public:
    static bool dictionary(int id, const std::string &type,
        const std::string &name, double absdur, double offset);
    static bool dictionary(int id, NIVissimTL *o);
    static NIVissimTL *dictionary(int id);
//    static void assignNodes();
    static IntVector getWithin(const AbstractPoly &poly, double offset);
    static void buildNodeClusters();
    static void clearDict();
    static bool dict_SetSignals();

public:
    class NIVissimTLSignal;
    class NIVissimTLSignalGroup;
    typedef std::map<int, NIVissimTLSignal *> SSignalDictType;
    typedef std::map<int, NIVissimTLSignalGroup *> SGroupDictType;
    typedef std::map<int, SSignalDictType> SignalDictType;
    typedef std::map<int, SGroupDictType> GroupDictType;

    class NIVissimTLSignal {
    public:
        NIVissimTLSignal(int lsaid, int id, const std::string &name,
            const IntVector &groupids, int edgeid, int laneno,
            double position, const IntVector &assignedVehicleTypes);
        ~NIVissimTLSignal();
        bool isWithin(const Position2DVector &poly) const;
        Position2D getPosition() const;
        void addTo(NBNode *node) const;

    public:
        static bool dictionary(int lsaid, int id, NIVissimTLSignal *o);
        static NIVissimTLSignal *dictionary(int lsaid, int id);
        static void clearDict();
        static SSignalDictType getSignalsFor(int tlid);

    protected:
        int myLSA;
        int myID;
        std::string myName;
        IntVector myGroupIDs;
        int myEdgeID;
        int myLane;
        double myPosition;
        IntVector myVehicleTypes;
        static SignalDictType myDict;
    };

    class NIVissimTLSignalGroup {
    public:
        NIVissimTLSignalGroup(int lsaid, int id, const std::string &name,
            bool isGreenBegin, const DoubleVector &times,
            double tredyellow, double tyellow);
        ~NIVissimTLSignalGroup();
        void addTo(NBNode *node) const;
    public:
        static bool dictionary(int lsaid, int id, NIVissimTLSignalGroup *o);
        static NIVissimTLSignalGroup *dictionary(int lsaid, int id);
        static void clearDict();
        static SGroupDictType getGroupsFor(int tlid);

    public:
/*        void setStaticGreen();
        void setStaticRed();
        void addTimes(const DoubleVector &times);
        void addYellowTimes(double tredyellow, double tyellow);
        */
    private:
        int myLSA;
        int myID;
        std::string myName;
        DoubleVector myTimes;
        bool myFirstIsRed;
        double myTRedYellow, myTYellow;
        static GroupDictType myDict;
    };

protected:
    int myID;
    std::string myName;
    double myAbsDuration;
    double myOffset;
    NIVissimTLSignalGroup *myCurrentGroup;
    int myNodeID;
    std::string myType;
private:
    typedef std::map<int, NIVissimTL*> DictType;
    static DictType myDict;
};

#endif


