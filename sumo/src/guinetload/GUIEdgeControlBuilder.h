#ifndef GUIEdgeControlBuilder_h
#define GUIEdgeControlBuilder_h
//---------------------------------------------------------------------------//
//                        GUIEdgeControlBuilder.h -
//  A builder for edges during the loading derived from the
//      NLEdgeControlBuilder
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
// Revision 1.10  2005/09/15 11:06:03  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/07/12 12:15:41  dkrajzew
// new loading of edges implemented
//
// Revision 1.8  2005/05/04 07:55:28  dkrajzew
// added the possibility to load lane geometries into the non-gui simulation; simulation speedup due to avoiding multiplication with 1;
//
// Revision 1.7  2004/07/02 08:39:12  dkrajzew
// using global selection storage
//
// Revision 1.6  2004/04/02 11:15:25  dkrajzew
// changes due to the visualisation of the selection status
//
// Revision 1.5  2004/03/19 12:56:48  dkrajzew
// porting to FOX
//
// Revision 1.4  2003/09/05 14:57:12  dkrajzew
// first steps for reading of internal lanes
//
// Revision 1.3  2003/07/07 08:13:15  dkrajzew
// first steps towards the usage of a real lane and junction geometry implemented
//
// Revision 1.2  2003/02/07 10:38:19  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <netload/NLEdgeControlBuilder.h>
#include <utils/geom/Position2DVector.h>
#include <guisim/GUIEdge.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSJunction;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This class is reponsible for building GUIEdge-objects
 */
class GUIEdgeControlBuilder : public NLEdgeControlBuilder {
public:
    /** standard constructor; the parameter is a hint for the maximal number
        of lanes inside an edge */
    GUIEdgeControlBuilder(GUIGlObjectStorage &glObjectIDStorage,
        unsigned int storageSize=10);

    /// standard destructor
    ~GUIEdgeControlBuilder();

    /** adds an edge with the given id to the list of edges; this method
        throws an XMLIdAlreadyUsedException when the id was already used for
        another edge */
    MSEdge *addEdge(const std::string &id);

    /// Builds the lane to add
    virtual MSLane *addLane(/*MSNet &net, */const std::string &id,
        double maxSpeed, double length, bool isDepart,
        const Position2DVector &shape);

    MSEdge *closeEdge();


private:
    /// The gl-object id giver
    GUIGlObjectStorage &myGlObjectIDStorage;

private:
    /** invalid copy constructor */
    GUIEdgeControlBuilder(const GUIEdgeControlBuilder &s);

    /** invalid assignment operator */
    GUIEdgeControlBuilder &operator=(const GUIEdgeControlBuilder &s);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

