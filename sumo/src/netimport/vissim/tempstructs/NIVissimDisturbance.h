#ifndef NIVissimDisturbance_h
#define NIVissimDisturbance_h

#include <map>
#include <string>
#include <utils/common/IntVector.h>
#include <utils/geom/AbstractPoly.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimBoundedClusterObject.h"
#include "NIVissimNodeParticipatingEdgeVector.h"

class NBNode;
class NBEdge;

class NIVissimDisturbance
        : public NIVissimBoundedClusterObject {
public:
    NIVissimDisturbance(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &edge,
        const NIVissimExtendedEdgePoint &by,
        double timegap, double waygap, double vmax);
    virtual ~NIVissimDisturbance();
    void computeBounding();
    void addToNode(NBNode *node);
/*    bool tryAssignToNodeSingle(int nodeid,
        const NIVissimNodeParticipatingEdgeVector &edges);*/
    std::pair<NBEdge*, NBEdge*> getConnection(NBNode *node, int aedgeid);

public:
    static bool dictionary(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &edge,
        const NIVissimExtendedEdgePoint &by,
        double timegap, double waygap, double vmax);
    static bool dictionary(int id, NIVissimDisturbance *o);
    static NIVissimDisturbance *dictionary(int id);
    static IntVector getWithin(const AbstractPoly &poly);
    static void clearDict();
    static void dict_SetDisturbances();
//    static void buildNodeClusters();

/*    static IntVector tryAssignToNode(int nodeid,
        const NIVissimNodeParticipatingEdgeVector &edges);
    static IntVector getDisturbatorsForEdge(int edgeid);
    static IntVector getDisturbtionsForEdge(int edgeid);*/

private:
    int myID;
    int myNode;
    std::string myName;
    NIVissimExtendedEdgePoint myEdge;
    NIVissimExtendedEdgePoint myDisturbance;
    double myTimeGap, myWayGap, myVMax;
private:
    typedef std::map<int, NIVissimDisturbance*> DictType;
    static DictType myDict;
    static int myRunningID;
};


#endif

