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
        const IntVector &disturbances,
		bool amEdgeSplitOnly);
    ~NIVissimNodeCluster();
    int getID() const { return myID; }
    void buildNBNode();
    bool recheckEdgeChanges();
    NBNode *getNBNode() const;
    Position2D getPos() const;
//    void addNodesEdges();
    std::string getNodeName() const;


public:
    static bool dictionary(int id, NIVissimNodeCluster *o);
    static int dictionary(int nodeid, int tlid, const IntVector &connectors,
		const IntVector &disturbances, bool amEdgeSplitOnly);
    static NIVissimNodeCluster *dictionary(int id);
    static size_t contSize();
    static void assignToEdges();
    static void buildNBNodes();
    static void dict_recheckEdgeChanges();
//    static void dict_addNodesEdges();
    static int getFromNode(int edgeid);
    static int getToNode(int edgeid);
    static void _debugOut(std::ostream &into);
    static void dict_addDisturbances();
    static void clearDict();
    static void setCurrentVirtID(int id);



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
	bool myAmEdgeSplit;
};

#endif
