#ifndef RODFEdge_h
#define RODFEdge_h
//---------------------------------------------------------------------------//
//                        RODFEdge.h -
//  An edge the router may route through
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 01.03.2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/03/08 12:51:29  dkrajzew
// further work on the dfrouter
//
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
 * A router's edge extended by the definition about the probability a
 *  vehicle's probabilities to choose a certain following edge over time.
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

private:
    std::vector<FlowDef> myFlows;

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

