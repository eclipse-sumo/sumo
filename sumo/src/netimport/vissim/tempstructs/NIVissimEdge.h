#ifndef NIVissimEdge_h
#define NIVissimEdge_h

#include "NIVissimConnectionCluster.h"
#include <vector>
#include <string>
#include <map>
#include <utils/geom/Position2DVector.h>
#include "NIVissimAbstractEdge.h"
#include "NIVissimClosedLanesVector.h"

class NBNode;

class NIVissimEdge
        : public NIVissimAbstractEdge {
public:
    NIVissimEdge(int id, const std::string &name,
        const std::string &type, int noLanes, double zuschlag1,
        double zuschlag2, double length,
        const Position2DVector &geom,
        const NIVissimClosedLanesVector &clv);
    ~NIVissimEdge();
    void setNodeCluster(int nodeid);
    void buildGeom();
    void addIncomingConnection(int id);
    void addOutgoingConnection(int id);
    NBNode *getNodeAt(const Position2D &p, NBNode *other=0);
    Position2D getBegin2D() const;
    Position2D getEnd2D() const;
    double getLength() const;

    void mergedInto(NIVissimConnectionCluster *old,
        NIVissimConnectionCluster *act);

    void removeFromConnectionCluster(NIVissimConnectionCluster *c);
    void addToConnectionCluster(NIVissimConnectionCluster *c);

    friend class NIVissimNodeDef_Edges;
/*    bool crosses(const Position2DVector &poly) const;
    std::pair<double, double> getCrossingRange(const Position2DVector &poly) const;
    int getID() const;
    */
public:
    static bool dictionary(int id, const std::string &name,
        const std::string &type, int noLanes, double zuschlag1,
        double zuschlag2, double length,
        const Position2DVector &geom,
        const NIVissimClosedLanesVector &clv);
    static bool dictionary(int id, NIVissimEdge *o);
    static NIVissimEdge *dictionary(int id);
    static void buildConnectionClusters();
    static void dict_buildNBEdges();
//    static void assignConnectorsAndDisturbances();


private:
    void buildNBEdge();
    NBNode *getFromNode();
    NBNode *getToNode();
    std::pair<NBNode*, NBNode*> resolveSameNode();

private:
/*    void checkPosition1(const Position2D &p1, const Position2D &p2,
        double length, const Position2DVector &poly,
        std::pair<double, double> &positions) const;*/
    static NBNode *getNodeSecure(int nodeid, const Position2D &pos,
        const std::string &possibleName);

private:
    class connection_position_sorter {
    public:
        /// constructor
        explicit connection_position_sorter(int edgeid);

        /// comparing operation
        int operator() (int c1id, int c2id) const;

    private:
        int myEdgeID;
    };


    class connection_cluster_position_sorter {
    public:
        /// constructor
        explicit connection_cluster_position_sorter(int edgeid);

        /// comparing operation
        int operator() (NIVissimConnectionCluster *cc1,
            NIVissimConnectionCluster *cc2) const;

    private:
        int myEdgeID;
    };




private:

    typedef std::vector<NIVissimConnectionCluster*> ConnectionClusters;
    std::string myName;
    std::string myType;
    int myNoLanes;
    double myZuschlag1, myZuschlag2;
    NIVissimClosedLanesVector myClosedLanes;
/*    IntVector myDisturbers;
    IntVector myDisturbings;
    IntVector myOutgingConnectors;
    IntVector myIncomingConnectors;*/
    ConnectionClusters myConnectionClusters;
    IntVector myIncomingConnections;
    IntVector myOutgoingConnections;

private:
    typedef std::map<int, NIVissimEdge*> DictType;
    static DictType myDict;
    static int myMaxID;
};

#endif

