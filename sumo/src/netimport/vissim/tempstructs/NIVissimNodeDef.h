#ifndef NIVissimNodeDef_h
#define NIVissimNodeDef_h

#include <string>
#include <map>
#include <utils/common/IntVector.h>
#include <utils/geom/Boundery.h>
#include "NIVissimBoundedClusterObject.h"
#include "NIVissimExtendedEdgePointVector.h"
#include "NIVissimNodeCluster.h"

class NIVissimNodeDef
        : public NIVissimBoundedClusterObject {
public:
    NIVissimNodeDef(int id, const std::string &name);
    virtual ~NIVissimNodeDef();
    int buildNodeCluster();
    virtual void computeBounding() = 0;
    bool partialWithin(const AbstractPoly &p) const;
    virtual void searchAndSetConnections() = 0;

//    virtual bool trafficLightMatches(
//        const NIVissimExtendedEdgePointVector &edges) = 0;
//    virtual bool connectionMatches(int fromID, int toID) = 0;
//    void addConnection(int connID);
//    void setTL(int tlID);
//    virtual bool assignDisturbancesToNode() = 0;
public:
    static bool dictionary(int id, NIVissimNodeDef *o);
    static NIVissimNodeDef *dictionary(int id);
    static IntVector getWithin(const AbstractPoly &p);
    static void buildNodeClusters();
    static void dict_assignConnectionsToNodes();
    static size_t dictSize();
/*
    static int searchAndSetMatchingTLParent(int tlID, const
        NIVissimExtendedEdgePointVector &edges);
    static int searchAndSetMatchingConnectionParent(int connID,
        int fromEdge, int toEdge);
    static void assignDisturbances();
    */
protected:
    int myID;
    std::string myName;
//    Boundery myBoundery;
//    int myTLID;
//    IntVector myConnections;
//    IntVector myDisturbances;

private:
    typedef std::map<int, NIVissimNodeDef*> DictType;
    static DictType myDict;
};

#endif
