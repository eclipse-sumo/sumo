#include "NBEdgeCont.h"
#include "NBEdge.h"
#include "NBConnection.h"


NBConnection::NBConnection(NBEdge *from, NBEdge *to)
    : myFrom(from), myTo(to)
{
    myFromID = from->getID();
    myToID = to->getID();
}


NBConnection::NBConnection(const std::string &fromID, NBEdge *from,
                           const std::string &toID, NBEdge *to)
    : myFrom(from), myTo(to), myFromID(fromID), myToID(toID)
{
}


NBConnection::~NBConnection()
{
}


NBConnection::NBConnection(const NBConnection &c)
    : myFrom(c.myFrom), myTo(c.myTo),
    myFromID(c.myFromID), myToID(c.myToID)
{
}



NBEdge *
NBConnection::getFrom() const
{
    return myFrom;
}


NBEdge *
NBConnection::getTo() const
{
    return myTo;
}


bool
NBConnection::replaceFrom(NBEdge *which, NBEdge *by)
{
    if(myFrom==which) {
        myFrom = by;
        return true;
    }
    return false;
}


bool
NBConnection::replaceTo(NBEdge *which, NBEdge *by)
{
    if(myTo==which) {
        myTo = by;
        return true;
    }
    return false;
}


bool
operator<(const NBConnection &c1, const NBConnection &c2)
{
    return
        std::pair<NBEdge*, NBEdge*>(c1.getFrom(), c1.getTo())
        <
        std::pair<NBEdge*, NBEdge*>(c2.getFrom(), c2.getTo());
}



bool
NBConnection::check()
{
    myFrom = checkFrom();
    myTo = checkTo();
    return myFrom!=0 && myTo!=0;
}


NBEdge *
NBConnection::checkFrom()
{
    NBEdge *e = NBEdgeCont::retrieve(myFromID);
    // ok, the edge was not changed
    if(e==myFrom) {
        return myFrom;
    }
    // try to get the edge
    return NBEdgeCont::retrievePossiblySplitted(myFromID, myToID, true);
}


NBEdge *
NBConnection::checkTo()
{
    NBEdge *e = NBEdgeCont::retrieve(myToID);
    // ok, the edge was not changed
    if(e==myTo) {
        return myTo;
    }
    // try to get the edge
    return NBEdgeCont::retrievePossiblySplitted(myToID, myFromID, false);
}
