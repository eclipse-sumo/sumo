#ifndef NIVissimNodeParticipatingEdge_h
#define NIVissimNodeParticipatingEdge_h

#include <string>
#include <utils/geom/Position2D.h>

class NIVissimNodeParticipatingEdge {
public:
    NIVissimNodeParticipatingEdge(int edgeid,
        double frompos, double topos);
    ~NIVissimNodeParticipatingEdge();
    int getID() const;
    bool positionLiesWithin(double pos) const;
    double getFromPos() const;
    double getToPos() const;
    /*
    Position2D getFrom2DPosition() const;
    Position2D getTo2DPosition() const;
    */
private:
    int myEdgeID;
    double myFromPos, myToPos;
};

#endif

