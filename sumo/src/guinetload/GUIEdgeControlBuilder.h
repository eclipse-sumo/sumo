/****************************************************************************/
/// @file    GUIEdgeControlBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A builder for edges during the loading derived from the
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIEdgeControlBuilder_h
#define GUIEdgeControlBuilder_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <netload/NLEdgeControlBuilder.h>
#include <utils/geom/Position2DVector.h>
#include <guisim/GUIEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;

#ifdef RAKNET_DEMO
class Street;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * This class is reponsible for building GUIEdge-objects
 */
class GUIEdgeControlBuilder : public NLEdgeControlBuilder
{
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
                                            SUMOReal maxSpeed, SUMOReal length, bool isDepart,
                                            const Position2DVector &shape, const std::string &vclasses);

    MSEdge *closeEdge();


private:
    /// The gl-object id giver
    GUIGlObjectStorage &myGlObjectIDStorage;

#ifdef RAKNET_DEMO
    Street *myStreet;
#endif

private:
    /** invalid copy constructor */
    GUIEdgeControlBuilder(const GUIEdgeControlBuilder &s);

    /** invalid assignment operator */
    GUIEdgeControlBuilder &operator=(const GUIEdgeControlBuilder &s);

};


#endif

/****************************************************************************/

