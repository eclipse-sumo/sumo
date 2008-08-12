/****************************************************************************/
/// @file    GUIEdgeControlBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Derivation of NLEdgeControlBuilder which builds gui-edges
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIEdgeControlBuilder
 * @brief Derivation of NLEdgeControlBuilder which builds gui-edges
 *
 * Instead of building pure microsim-objects (MSEdge and MSLane), this class 
 *  builds GUIEdges and GUILanes.
 * @see NLEdgeControlBuilder
 */
class GUIEdgeControlBuilder : public NLEdgeControlBuilder
{
public:
    /** @brief Constructor
     *
     * @param[in] glObjectIDStorage Storage of gl-ids used to assign new ids to built edges
     */
    GUIEdgeControlBuilder(GUIGlObjectStorage &glObjectIDStorage) throw();


    /// @brief Destructor
    ~GUIEdgeControlBuilder() throw();


    /// Builds the lane to add
    virtual MSLane *addLane(const std::string &id,
        SUMOReal maxSpeed, SUMOReal length, bool isDepart,
        const Position2DVector &shape, const std::string &vclasses);


    MSEdge *closeEdge();


protected:
    /** @brief Builds an edge instance (GUIEdge in this case)
     *
     * Builds an GUIEdge-instance using the given name, the current index
     *  "myCurrentNumericalEdgeID" and the gl-id storage ("myGlObjectIDStorage"). 
     *  Post-increments the index, returns the built edge.
     *
     * @param[in] id The id of the edge to build
     */
    MSEdge *buildEdge(const std::string &id) throw();


private:
    /// @brief The gl-object id giver
    GUIGlObjectStorage &myGlObjectIDStorage;


private:
    /// @brief invalidated copy constructor
    GUIEdgeControlBuilder(const GUIEdgeControlBuilder &s);

    /// @brief invalidated assignment operator
    GUIEdgeControlBuilder &operator=(const GUIEdgeControlBuilder &s);

};


#endif

/****************************************************************************/

