#ifndef NIVissimConnection_h
#define NIVissimConnection_h

#include <string>
#include <map>
#include <utils/common/IntVector.h>
#include "NIVissimExtendedEdgePoint.h"
#include <utils/geom/Position2D.h>
#include <utils/geom/AbstractPoly.h>
#include "NIVissimAbstractEdge.h"
#include "NIVissimClosedLanesVector.h"
#include "NIVissimBoundedClusterObject.h"

class NIVissimConnection
        : public NIVissimBoundedClusterObject,
          public NIVissimAbstractEdge {
public:
    enum Direction {
        NIVC_DIR_RIGHT,
        NIVC_DIR_LEFT,
        NIVC_DIR_ALL
    };

    NIVissimConnection(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &from_def,
        const NIVissimExtendedEdgePoint &to_def,
        const Position2DVector &geom,
        Direction direction, double dxnothalt, double dxeinordnen,
        double zuschlag1, double zuschlag2, double seglength,
        const IntVector &assignedVehicles,
        const NIVissimClosedLanesVector &clv);
    ~NIVissimConnection();
    void computeBounding();
    int getFromEdgeID() const;
    int getToEdgeID() const;
    double getFromPosition() const;
    double getToPosition() const;
    Position2D getFromGeomPosition() const;
    Position2D getToGeomPosition() const;
    void setNodeCluster();
    void unsetCluster();
    const Boundery &getBoundingBox() const;

    //    void buildFurtherNode();
    void buildGeom();

public:
    static bool dictionary(int id, const std::string &name,
        const NIVissimExtendedEdgePoint &from_def,
        const NIVissimExtendedEdgePoint &to_def,
        const Position2DVector &geom,
        Direction direction, double dxnothalt, double dxeinordnen,
        double zuschlag1, double zuschlag2, double seglength,
        const IntVector &assignedVehicles,
        const NIVissimClosedLanesVector &clv);
    static bool dictionary(int id, NIVissimConnection *o);
    static NIVissimConnection *dictionary(int id);
    static IntVector getWithin(const AbstractPoly &poly);
    static void buildNodeClusters();
    static IntVector getForEdge(int edgeid, bool omitNodeAssigned=true);
    static void dict_buildNBEdgeConnections();
    static void dict_assignToEdges();
    const IntVector &getFromLanes() const;
    const IntVector &getToLanes() const;
/*    static void assignNodes();
    static void buildFurtherNodes();
    static IntVector getOutgoingForEdge(int edgeid);
    static IntVector getIncomingForEdge(int edgeid);*/

private:
    std::string myName;
    NIVissimExtendedEdgePoint myFromDef, myToDef;
    Direction myDirection;
    double myDXNothalt, myDXEinordnen;
    double myZuschlag1, myZuschlag2;
    IntVector myAssignedVehicles;
    NIVissimClosedLanesVector myClosedLanes;
private:
    typedef std::map<int, NIVissimConnection*> DictType;
    static DictType myDict;
};

#endif


