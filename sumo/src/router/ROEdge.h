#ifndef ROEdge_h
#define ROEdge_h
//---------------------------------------------------------------------------//
//                        ROEdge.h -
//  An edge the router may route through
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.4  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <utils/router/ValueTimeLine.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROEdge
 * An edge.
 */
class ROEdge {
public:
    enum EdgeType {
        ET_NORMAL,
        ET_SOURCE,
        ET_SINK
    };


	ROEdge(const std::string &id);
	~ROEdge();
    void postloadInit();

    void setEffort(double effort);
    void addLane(ROLane *lane);
    void setLane(long timeBegin, long timeEnd,
        const std::string &id, float value);
    void addSucceeder(ROEdge *s);
    bool isConnectedTo(ROEdge *e);
    size_t getNoFollowing();
    ROEdge *getFollower(size_t pos);
    double getCost(long time) const;
    long getDuration(long time) const;
    // dijkstra
    void init();
    void initRootDistance();
    float getEffort() const;
    float getNextEffort(long time) const;
    void setEffort(float dist);
    bool isInFrontList() const;
    bool addConnection(ROEdge *to, float effort);
    bool isExplored() const;
    void setExplored(bool value);
    ROEdge *getPrevKnot() const;
    void setPrevKnot(ROEdge *prev);
    std::string getID() const;
    void setType(EdgeType type);
    EdgeType getType() const;
protected:
    float getMyEffort(long time) const;
private:
    std::string _id;
    bool _explored;
    double _dist;
    ROEdge *_prevKnot;
    bool _inFrontList;
	double _effort;
    typedef std::map<ROLane*, ValueTimeLine*> LaneUsageCont;
    LaneUsageCont _laneCont;
    ValueTimeLine _ownValueLine;
    std::vector<ROEdge*> _succeeding;
//    std::vector<ROEdge*> myRealSucceder;
    bool _usingTimeLine;
    EdgeType myType;
private:
    /// we made the copy constructor invalid
    ROEdge(const ROEdge &src);
    /// we made the assignment operator invalid
    ROEdge &operator=(const ROEdge &src);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROEdge.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

