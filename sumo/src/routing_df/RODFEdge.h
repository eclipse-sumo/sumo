#ifndef RODFEdge_h
#define RODFEdge_h
/***************************************************************************
                          RODFEdge.h
    An edge within the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.3  2006/03/28 06:17:18  dkrajzew
// extending the dfrouter by distance/length factors
//
// Revision 1.2  2006/03/17 09:04:26  dkrajzew
// class-documentation added/patched
//
// Revision 1.1  2006/03/08 12:51:06  dksumo
// further work on the dfrouter
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <router/ROEdge.h>
#include <utils/geom/Position2D.h>
#include "DFDetectorFlow.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RODFEdge
 */
class RODFEdge : public ROEdge {
public:
    /// Constructor
    RODFEdge(const std::string &id, int index);

    /// Desturctor
    ~RODFEdge();

    /// Adds information about a connected edge
    void addFollower(ROEdge *s);

    void setFlows(const std::vector<FlowDef> &flows);

    const std::vector<FlowDef> &getFlows() const;

    void setFromPosition(const Position2D &p);
    void setToPosition(const Position2D &p);

    const Position2D &getFromPosition() const;
    const Position2D &getToPosition() const;

private:
    std::vector<FlowDef> myFlows;

    Position2D myFromPosition, myToPosition;

private:
    /// we made the copy constructor invalid
    RODFEdge(const RODFEdge &src);

    /// we made the assignment operator invalid
    RODFEdge &operator=(const RODFEdge &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

