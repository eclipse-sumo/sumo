#ifndef NIVissimConnectionCluster_h
#define NIVissimConnectionCluster_h

#include <iostream>
#include <vector>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundery.h>
#include <utils/common/IntVector.h>

class NBNode;
class NIVissimConnection;

/**
 * @class NIVissimConnectionCluster
 * This class holds a list of connections either all outgoing or all
 * incoming within an edge, which do lies close together.
 * This shall be the connections which belong to a single node.
 * It still are not all of the node's connections, as other edges
 * may participate to this node, too.
 */
class NIVissimConnectionCluster {
public:
    /** @brief Constructor
        Build the boundery; The boundery includes both incoming and outgoing nodes */
    NIVissimConnectionCluster(const IntVector &connections, int nodeCluster,
        int edgeid);

    NIVissimConnectionCluster(const IntVector &connections,
        const Boundery &boundery, int nodeCluster, const IntVector &edges);

    /// Destructor
    ~NIVissimConnectionCluster();

    /// Returns the information whether the given cluster overlaps the current
    bool overlapsWith(NIVissimConnectionCluster *c, double offset=0) const;

    bool hasNodeCluster() const;

    NBNode *getNBNode() const;

    bool around(const Position2D &p, double offset=0) const;

    double getPositionForEdge(int edgeid) const;

    friend class NIVissimEdge; // !!! debug

public:
    /** @brief Tries to joind clusters participating within a node
        This is done by joining clusters which overlap */
    static void join();

    static void buildNodeClusters();

    static void searchForConnection(int id);

    static void _debugOut(std::ostream &into);

    static size_t dictSize();

    static void dict_recheckNodes();

    static int getNextFreeNodeID();

    static void clearDict();

    static void addNodes();

    //static void dict_checkDoubleNodes();

    static void addTLs();

private:
    class NodeSubCluster {
    public:
        NodeSubCluster(NIVissimConnection *c);
        ~NodeSubCluster();
        void add(NIVissimConnection *c);
        void add(const NodeSubCluster &c);
        size_t size() const;
        void setConnectionsFree();
        bool overlapsWith(const NodeSubCluster &c, double offset=0);
        IntVector getConnectionIDs() const;
        friend class NIVissimConnectionCluster;
    public:
        Boundery myBoundery;
        typedef std::vector<NIVissimConnection*> ConnectionCont;
        ConnectionCont myConnections;
    };

private:
    /// Adds the second cluster
    void add(NIVissimConnectionCluster *c);

    void removeConnections(const NodeSubCluster &c);

    void recomputeBoundery();

    void recheckEdges();

    bool joinable(NIVissimConnectionCluster *c2);


private:
    /// List of connection-ids which participate within this cluster
    IntVector myConnections;

    /// The boundery of the cluster
    Boundery myBoundery;

    /// The node the cluster is assigned to
    int myNodeCluster;

    // The edge which holds the cluster
    IntVector myEdges;

    IntVector myNodes;

    IntVector myTLs;

	IntVector myOutgoingEdges, myIncomingEdges;

private:
    typedef std::vector<NIVissimConnectionCluster*> ContType;
    static ContType myClusters;
    static int myFirstFreeID;
};

#endif

