/****************************************************************************/
/// @file    NLEdgeControlBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Interface for building edges
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
 * As instances of the MSEdge-class contain references to other instances of
 *  this class which may not yet be known at their generation, they are
 *  prebuild first and initialised with their correct values in a second step.
 *
 * While building instances of MSEdge, these are stored in a preallocated list to
 *  avoid memory fraction. For the same reason, the list of edges, later split
 *  into two lists, one containing single-lane-edges and one containing multi-lane-edges,
 *  is preallocated to the size that was previously computed by counting the edges
 *  in the first parser step.
 */
class NLEdgeControlBuilder
{
public:
    /// definition of the used storage for edges
    typedef std::vector<MSEdge*> EdgeCont;

public:
    /** @brief standard constructor;
        the parameter is a hint for the maximal number of lanes inside an edge */
    NLEdgeControlBuilder(unsigned int storageSize=10);

    /// Destructor
    virtual ~NLEdgeControlBuilder();

    /** Prepares the builder for the building of the specified number of
        edges (preallocates ressources) */
    void prepare(unsigned int no);

    /** @brief Adds an edge with the given id to the list of edges;
        This method throws an ProcessError when the id was
        already used for another edge */
    virtual MSEdge *addEdge(const std::string &id);

    /// chooses the previously added edge as the current edge
    void chooseEdge(const std::string &id,
                    MSEdge::EdgeBasicFunction function,
                    bool inner);


    /** @brief Adds a lane to the current edge;
        This method throws an ProcessError when the
        lane is marked to be the depart lane and another so marked lane
        was added before */
    virtual MSLane *addLane(/*MSNet &net, */const std::string &id,
                                            SUMOReal maxSpeed, SUMOReal length, bool isDepart,
                                            const Position2DVector &shape, const std::string &vclasses);

    /// closes (ends) the addition of lanes to the current edge
    void closeLanes();

    /** Begins the specification of lanes that may be used to reach the given
        edge from the current edge */
    void openAllowedEdge(MSEdge *edge);

    /** @brief Adds a lane that may be used to reach the edge previously specified by "openAllowedEdge"
        This method throws a ProcessError when the lane is
        not belonging to the current edge */
    void addAllowed(MSLane *lane);

    /// closes the specification of lanes that may be used to reach an edge
    void closeAllowedEdge();

    /** @brief Closes the building of an edge;
        The edge is completely described by now and may not be opened again */
    virtual MSEdge *closeEdge();

    /// builds the MSEdgeControl-class which holds all edges
    MSEdgeControl *build();

    MSEdge *getActiveEdge() const;

    void parseVehicleClasses(const std::string &allowedS,
                             std::vector<SUMOVehicleClass> &allowed,
                             std::vector<SUMOVehicleClass> &disallowed);

    size_t getEdgeCapacity() const;


protected:
    /// @brief A running numer for lane numbering
    unsigned int myCurrentNumericalLaneID;

    /// @brief A running numer for edge numbering
    unsigned int myCurrentNumericalEdgeID;

    /** @brief storage for edges
     *
     * to allow the splitting of edges after their number is known, they are hold inside this vector
     *  and laterly moved into two vectors, one for single-lane-edges and one for multi-lane-edges
     *  respectively.
     * @todo Check whether this can be a plain vector, too, not a pointer to one
     */
    EdgeCont *myEdges;

    /// @brief pointer to the currently chosen edge
    MSEdge *myActiveEdge;

    /// @brief pointer to a temporary lane storage
    MSEdge::LaneCont          *m_pLaneStorage;

    /// @brief list of the lanes that belong to the current edge
    MSEdge::LaneCont          *m_pLanes;

    /// @brief pointer to the following edge the structure is currently working on
    MSEdge                    *m_pCurrentDestination;

    /// connection to following edges from the current edge
    MSEdge::AllowedLanesCont  *m_pAllowedLanes;

    /// pointer to the depart lane
    MSLane                    *m_pDepartLane;

    /// number of single-lane-edges
    unsigned int              m_iNoSingle;

    /// number of multi-lane-edges
    unsigned int              m_iNoMulti;

    /// the function of the current edge
    MSEdge::EdgeBasicFunction m_Function;

    //
    bool myIsInner;

    OutputDevice *myEdgesLaneChangeOutputDevice;

private:
    /** invalidated copy constructor */
    NLEdgeControlBuilder(const NLEdgeControlBuilder &s);

    /** invalidated assignment operator */
    NLEdgeControlBuilder &operator=(const NLEdgeControlBuilder &s);

};


#endif

/****************************************************************************/

