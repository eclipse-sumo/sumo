#ifndef NIVissimNodeDef_Edges_h
#define NIVissimNodeDef_Edges_h

#include <string>
#include <map>
#include "NIVissimNodeParticipatingEdgeVector.h"
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimNodeDef.h"

class NIVissimNodeDef_Edges :
        public NIVissimNodeDef {
public:
    NIVissimNodeDef_Edges(int id, const std::string &name,
        const NIVissimNodeParticipatingEdgeVector &edges);
    virtual ~NIVissimNodeDef_Edges();
    static bool dictionary(int id, const std::string &name,
        const NIVissimNodeParticipatingEdgeVector &edges);
    virtual void computeBounding();
    virtual void searchAndSetConnections();
/*    virtual bool trafficLightMatches(
        const NIVissimExtendedEdgePointVector &edges);
    virtual bool connectionMatches(int fromID, int toID);
    bool assignDisturbancesToNode();*/


    class id_matches {
    public:
        explicit id_matches(int id) : myEdgeID(id) { }
        bool operator() (NIVissimNodeParticipatingEdge *e) {
            return e->getID()==myEdgeID;
        }
    private:
        int myEdgeID;
    };

    class lying_within_match {
    public:
        explicit lying_within_match(NIVissimNodeParticipatingEdge *e) : myEdge(e) { }
        bool operator() (NIVissimExtendedEdgePoint *e) {
            return e->getEdgeID()==myEdge->getID() &&
                myEdge->positionLiesWithin(e->getPosition());
        }
    private:
        NIVissimNodeParticipatingEdge *myEdge;
    };

protected:
    NIVissimNodeParticipatingEdgeVector myEdges;
};

#endif
