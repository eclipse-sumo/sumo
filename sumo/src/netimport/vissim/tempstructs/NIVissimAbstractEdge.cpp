#include <map>
#include <cassert>
#include "NIVissimAbstractEdge.h"


NIVissimAbstractEdge::DictType NIVissimAbstractEdge::myDict;

NIVissimAbstractEdge::NIVissimAbstractEdge(int id,
                                           const Position2DVector &geom)
    : myID(id), myGeom(geom), myNode(-1)
{
    dictionary(id, this);
}


NIVissimAbstractEdge::~NIVissimAbstractEdge()
{
}


bool
NIVissimAbstractEdge::dictionary(int id, NIVissimAbstractEdge *e)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = e;
        return true;
    }
    return false;
}


NIVissimAbstractEdge *
NIVissimAbstractEdge::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}



Position2D
NIVissimAbstractEdge::getGeomPosition(double pos) const
{
    return myGeom.positionAtLengthPosition(pos);
}


Position2D
NIVissimAbstractEdge::getBeginPosition() const
{
    return myGeom.at(0);
}


Position2D
NIVissimAbstractEdge::getEndPosition() const
{
    return myGeom.at(myGeom.size()-1);
}


bool
NIVissimAbstractEdge::hasGeom() const
{
    return myGeom.size()>0;
}



void
NIVissimAbstractEdge::splitAndAssignToNodes()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimAbstractEdge *e = (*i).second;
        e->splitAssigning();
    }
}

void
NIVissimAbstractEdge::splitAssigning()
{
}





bool
NIVissimAbstractEdge::crossesEdge(NIVissimAbstractEdge *c) const
{
    return myGeom.intersects(c->myGeom);
}


Position2D
NIVissimAbstractEdge::crossesEdgeAtPoint(NIVissimAbstractEdge *c) const
{
    return myGeom.intersectsAtPoint(c->myGeom);
}


IntVector
NIVissimAbstractEdge::getWithin(const AbstractPoly &p)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimAbstractEdge *e = (*i).second;
        if(e->overlapsWith(p)) {
            ret.push_back(e->myID);
        }
    }
    return ret;
}


bool
NIVissimAbstractEdge::overlapsWith(const AbstractPoly &p) const
{
    return myGeom.overlapsWith(p);
}


bool
NIVissimAbstractEdge::hasNodeCluster() const
{
    return myNode!=-1;
}


int
NIVissimAbstractEdge::getID() const
{
    return myID;
}

void
NIVissimAbstractEdge::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


