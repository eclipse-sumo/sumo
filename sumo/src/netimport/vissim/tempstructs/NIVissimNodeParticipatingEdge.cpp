#include "NIVissimEdge.h"
#include "NIVissimConnection.h"
#include "NIVissimNodeParticipatingEdge.h"



NIVissimNodeParticipatingEdge::NIVissimNodeParticipatingEdge(
        int edgeid, double frompos, double topos)
    : myEdgeID(edgeid), myFromPos(frompos), myToPos(topos)
{
}

NIVissimNodeParticipatingEdge::~NIVissimNodeParticipatingEdge()
{
}


int
NIVissimNodeParticipatingEdge::getID() const
{
    return myEdgeID;
}


bool
NIVissimNodeParticipatingEdge::positionLiesWithin(double pos) const
{
    return
        (myFromPos<myToPos && myFromPos>=pos && myToPos<=pos)
        ||
        (myFromPos>myToPos && myFromPos<=pos && myToPos>=pos);
}

/*
Position2D
NIVissimNodeParticipatingEdge::getFrom2DPosition() const
{
    NIVissimAbstractEdge *edge = NIVissimAbstractEdge::dictionary(myEdgeID);
    if(edge->hasGeom()) {
        if(myFromPos>0) {
            return edge->getGeomPosition(myFromPos);
        } else {
            return edge->getBeginPosition();
        }
    }
    // it must be a connection otherwise
    NIVissimConnection *c = NIVissimConnection::dictionary(myEdgeID);
    return c->getFromGeomPosition();
}


Position2D
NIVissimNodeParticipatingEdge::getTo2DPosition() const
{
    NIVissimAbstractEdge *edge = NIVissimAbstractEdge::dictionary(myEdgeID);
    if(edge->hasGeom()) {
        if(myToPos>0) {
            return edge->getGeomPosition(myToPos);
        } else {
            return edge->getEndPosition();
        }
    }
    // it must be a connection otherwise
    NIVissimConnection *c = NIVissimConnection::dictionary(myEdgeID);
    return c->getFromGeomPosition();
}
*/


double
NIVissimNodeParticipatingEdge::getFromPos() const
{
    return myFromPos;
}


double
NIVissimNodeParticipatingEdge::getToPos() const
{
    return myToPos;
}

