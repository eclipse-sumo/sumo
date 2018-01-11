/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NLEdgeControlBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Interface for building edges
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
#include <utils/geom/PositionVector.h>


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
    /// @brief Constructor
    NLEdgeControlBuilder();


    /// @brief Destructor
    virtual ~NLEdgeControlBuilder();


    /** @brief Begins building of an MSEdge
     *
     * Builds an instance of MSEdge using "buildEdge". Stores it
     *  as the current edge in "myActiveEdge" and appends it to the list
     *  of built edges ("myEdges").
     *
     * The given information is used to build the edge.
     * @param[in] id The id of the edge
     * @param[in] function The function of the edge
     * @param[in] streetName The street name of the edge
     * @exception InvalidArgument If an edge with the same name was already built
     */
    void beginEdgeParsing(const std::string& id, const SumoXMLEdgeFunc function,
                          const std::string& streetName, const std::string& edgeType, int priority);


    /** @brief Adds a lane to the current edge
     *
     * @param[in] id The lane's id
     * @param[in] maxSpeed The speed allowed on this lane
     * @param[in] length The lane's length
     * @param[in] shape The shape of the lane
     * @param[in] width The width of the lane
     * @param[in] permissions Encoding of vehicle classes that may drive on this lane
     * @param[in] index The index of this lane within its parent edge
     * @see SUMOVehicleClass
     * @see MSLane
     * @todo Definitely not a good way
     */
    virtual MSLane* addLane(const std::string& id, double maxSpeed,
                            double length, const PositionVector& shape,
                            double width, SVCPermissions permissions, int index, bool isRampAccel);


    /** @brief Adds a neighbor to the current lane
     *
     * @param[in] id The lane's id
     * @see MSLane
     */
    virtual void addNeigh(const std::string id);


    /** @brief Closes the building of an edge;
        The edge is completely described by now and may not be opened again */
    virtual MSEdge* closeEdge();

    /// builds the MSEdgeControl-class which holds all edges
    MSEdgeControl* build();


    /** @brief Builds an edge instance (MSEdge in this case)
     *
     * Builds an MSEdge-instance using the given name and the current index
     *  "myCurrentNumericalEdgeID". Post-increments the index, returns
     *  the built edge.
     *
     * @param[in] id The id of the edge to build
     * @param[in] streetName The street name of the edge to build
     */
    virtual MSEdge* buildEdge(const std::string& id, const SumoXMLEdgeFunc function,
                              const std::string& streetName, const std::string& edgeType, const int priority);

    /** @brief add the crossingEdges in a crossing edge if present
     *
     * @param[in] the vector of crossed edges id
     */
    virtual void addCrossingEdges(const std::vector<std::string>&);

protected:
    /// @brief A running number for lane numbering
    int myCurrentNumericalLaneID;

    /// @brief A running number for edge numbering
    int myCurrentNumericalEdgeID;

    /// @brief Temporary, internal storage for built edges
    MSEdgeVector myEdges;

    /// @brief pointer to the currently chosen edge
    MSEdge* myActiveEdge;

    /// @brief pointer to a temporary lane storage
    std::vector<MSLane*>* myLaneStorage;

private:
    /// @brief invalidated copy constructor
    NLEdgeControlBuilder(const NLEdgeControlBuilder& s);

    /// @brief invalidated assignment operator
    NLEdgeControlBuilder& operator=(const NLEdgeControlBuilder& s);

};


#endif

/****************************************************************************/

