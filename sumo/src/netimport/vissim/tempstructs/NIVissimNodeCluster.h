#ifndef NIVissimNodeCluster_h
#define NIVissimNodeCluster_h

#include <map>
#include <utils/common/IntVector.h>
#include <utils/geom/Position2D.h>

class NBNode;

class NIVissimNodeCluster {
public:
    NIVissimNodeCluster(int id, int nodeid, int tlid,
        const IntVector &connectors,
        const IntVector &disturbances);
    ~NIVissimNodeCluster();
    int getID() const { return myID; }
    void buildNBNode();
    bool recheckEdgeChanges();
    NBNode *getNBNode() const;
//    void addNodesEdges();


public:
    static bool dictionary(int id, NIVissimNodeCluster *o);
    static int dictionary(int nodeid, int tlid,
        const IntVector &connectors,
        const IntVector &disturbances);
    static NIVissimNodeCluster *dictionary(int id);
    static size_t contSize();
    static void assignToEdges();
    static void buildNBNodes();
    static void dict_recheckEdgeChanges();
//    static void dict_addNodesEdges();
    static int getFromNode(int edgeid);
    static int getToNode(int edgeid);
    static void _debugOut(std::ostream &into);


//    static NIVissimEdgePosMap getParticipatingEdgePositions();
//    static void container_computePositions();
private:
    int myID;
    int myNodeID;
    int myTLID;
    IntVector myConnectors;
    IntVector myDisturbances;
    Position2D myPosition;
    typedef std::map<int, NIVissimNodeCluster*> DictType;
    static DictType myDict;
    static int myCurrentID;
    NBNode *myNBNode;
};

#endif
