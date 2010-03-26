/****************************************************************************/
/// @file    NLEdgeControlBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Interface for building edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NLEdgeControlBuilder_h
#define NLEdgeControlBuilder_h


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
#include <microsim/MSEdge.h>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdgeControl;
class MSLane;
class MSNet;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLEdgeControlBuilder
 * @brief Interface for building edges
 *
 * This class is the container for MSEdge-instances while they are build.
 *
 * While building instances of MSEdge, these are stored in a list. The list of
 *  edges is later split into two lists, one containing single-lane-edges and
 *  one containing multi-lane-edges.
 * @todo Assignment of lanes is not really well. Should be reworked after shapes are given as params.
 */
class NLEdgeControlBuilder {
public:
    /// @brief definition of the used storage for edges
    typedef std::vector<MSEdge*> EdgeCont;

public:
    /// @brief Constructor
    NLEdgeControlBuilder();


    /// @brief Destructor
    virtual ~NLEdgeControlBuilder();


    /** @brieg Begins building of an MSEdge
     *
     * Builds an instance of MSEdge using "buildEdge". Stores it
     *  as the current edge in "myActiveEdge" and appends it to the list
     *  of built edges ("myEdges").
     *
     * The given information is used to build the edge.
     * @param[in] id The id of the edge
     * @param[in] function The function of the edge
     * @exception InvalidArgument If an edge with the same name was already built
     */
    void beginEdgeParsing(const std::string &id,
                          MSEdge::EdgeBasicFunction function) throw(InvalidArgument);


    /** @brief Adds a lane to the current edge;
        This method throws an ProcessError when the
        lane is marked to be the depart lane and another so marked lane
        was added before
     * @todo Definitely not a good way
     */
    virtual MSLane *addLane(const std::string &id, SUMOReal maxSpeed,
                            SUMOReal length, bool isDepart, const Position2DVector &shape,
                            const std::vector<SUMOVehicleClass> &allowed,
                            const std::vector<SUMOVehicleClass> &disallowed);


    /** @brief Closes the building of an edge;
        The edge is completely described by now and may not be opened again */
    virtual MSEdge *closeEdge();

    /// builds the MSEdgeControl-class which holds all edges
    MSEdgeControl *build();


    /** @brief Builds an edge instance (MSEdge in this case)
     *
     * Builds an MSEdge-instance using the given name and the current index
     *  "myCurrentNumericalEdgeID". Post-increments the index, returns
     *  the built edge.
     *
     * @param[in] id The id of the edge to build
     */
    virtual MSEdge *buildEdge(const std::string &id) throw();


protected:
    /// @brief A running number for lane numbering
    unsigned int myCurrentNumericalLaneID;

    /// @brief A running number for edge numbering
    unsigned int myCurrentNumericalEdgeID;

    /// @brief Temporary, internal storage for built edges
    EdgeCont myEdges;

    /// @brief pointer to the currently chosen edge
    MSEdge *myActiveEdge;

    /// @brief pointer to a temporary lane storage
    std::vector<MSLane*> *m_pLaneStorage;

    /// pointer to the depart lane
    MSLane                    *m_pDepartLane;

    /// number of single-lane-edges
    unsigned int              m_iNoSingle;

    /// number of multi-lane-edges
    unsigned int              m_iNoMulti;

    /// the function of the current edge
    MSEdge::EdgeBasicFunction m_Function;

private:
    /// @brief invalidated copy constructor
    NLEdgeControlBuilder(const NLEdgeControlBuilder &s);

    /// @brief invalidated assignment operator
    NLEdgeControlBuilder &operator=(const NLEdgeControlBuilder &s);

};


#endif

/****************************************************************************/

