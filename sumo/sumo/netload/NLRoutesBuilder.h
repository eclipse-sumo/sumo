#ifndef NLRoutesBuilder_h
#define NLRoutesBuilder_h
/***************************************************************************
                          NLRoutesBuilder.h
			  Container for MSNet::Route-structures during their
			  building
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.2  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:25  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:45  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:10  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "../microsim/MSNet.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdge;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLNetBuilder
 * NLRoutesBuilder is the container for routes while they are build until 
 * their transfering to the MSNet::RouteDict
 * The result of the operations are single MSNet::Route-instances
 */
class NLRoutesBuilder {
private:
    /// the current route
    MSNet::Route *m_pActiveRoute;
    /// the id of the current route
    std::string        m_ActiveId;
public:
    /// standard constructor
    NLRoutesBuilder();
    /// standard destructor
    ~NLRoutesBuilder();
    /// opens a route for the addition of edges
    void openRoute(const std::string &id);
    /// adds an edge to the route
    void addEdge(MSEdge *edge);
    /** closes (ends) the building of a route. 
        Afterwards no edges may be added to it; 
        this method may throw exceptions when 
        a) the route is empty or 
        b) another route with the same id already exists */
    void closeRoute();
private:
    /** invalid copy constructor */
    NLRoutesBuilder(const NLRoutesBuilder &s);
    /** invalid assignment operator */
    NLRoutesBuilder &operator=(const NLRoutesBuilder &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLRoutesBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
