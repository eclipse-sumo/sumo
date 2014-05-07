/****************************************************************************/
/// @file    NBTrafficLightDefinition.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The base class for traffic light logic definitions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBTrafficLightDefinition_h
#define NBTrafficLightDefinition_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <set>
#include <utils/common/StdDefs.h>
#include <utils/common/Named.h>
#include <utils/common/VectorHelper.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Parameterised.h>
#include "NBCont.h"
#include "NBConnection.h"
#include "NBConnectionDefs.h"
#include "NBLinkPossibilityMatrix.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;
class OptionsCont;
class NBTrafficLightLogic;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTrafficLightDefinition
 * @brief The base class for traffic light logic definitions
 *
 * A base class is necessary, as we have two cases: a) the logic is given by
 *  the imported network, or b) the logic is not given and we have to compute
 *  it by ourselves. In the first case, NBLoadedTLDef should be used, in the
 *  second NBOwnTLDef.
 *
 * @see NBLoadedTLDef
 * @see NBOwnTLDef
 */
class NBTrafficLightDefinition : public Named, public Parameterised {
public:

    static const std::string DefaultProgramID;

    /**
     * @enum TLColor
     * @brief An enumeration of possible tl-signal states
     */
    enum TLColor {
        /// @brief Signal shows red
        TLCOLOR_RED,
        /// @brief Signal shows yellow
        TLCOLOR_YELLOW,
        /// @brief Signal shows red/yellow (unused)
        TLCOLOR_REDYELLOW,
        /// @brief Signal shows green
        TLCOLOR_GREEN,
        /// @brief Signal is blinking yellow
        TLCOLOR_BLINK
    };


    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] junctions List of junctions controlled by this tls
     * @param[in] programID The id of the added program ("subID")
     * @param[in] offset The offset of the plan
     * @param[in] type The algorithm type for the computed traffic light
     */
    NBTrafficLightDefinition(const std::string& id,
                             const std::vector<NBNode*>& junctions,
                             const std::string& programID,
                             SUMOTime offset,
                             TrafficLightType type);


    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] junction The (single) junction controlled by this tls
     * @param[in] programID The id of the added program ("subID")
     * @param[in] offset The offset of the plan
     * @param[in] type The algorithm type for the computed traffic light
     */
    NBTrafficLightDefinition(const std::string& id,
                             NBNode* junction,
                             const std::string& programID,
                             SUMOTime offset,
                             TrafficLightType type);


    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] programID The id of the added program ("subID")
     * @param[in] offset The offset of the plan
     * @param[in] type The algorithm type for the computed traffic light
     */
    NBTrafficLightDefinition(const std::string& id, const std::string& programID,
                             SUMOTime offset,
                             TrafficLightType type);


    /// @brief Destructor
    virtual ~NBTrafficLightDefinition();


    /** @brief Computes the traffic light logic
     *
     * Does some initialisation at first, then calls myCompute to finally
     *  build the tl-logic
     *
     * @param[in] ec The edge container in order to retrieve edge information
     * @param[in] oc The options container holding options needed during the building
     * @return The built logic (may be 0)
     */
    NBTrafficLightLogic* compute(const NBEdgeCont& ec, OptionsCont& oc);



    /// @name Access to controlled nodes
    /// @{

    /** @brief Adds a node to the traffic light logic
     * @param[in] node A further node that shall be controlled by the tls
     */
    virtual void addNode(NBNode* node);


    /** @brief Removes the given node from the list of controlled nodes
     * @param[in] node The node that shall not be controlled by the tls any more
     */
    virtual void removeNode(NBNode* node);


    /** @brief Returns the list of controlled nodes
     * @return Controlled nodes
     */
    const std::vector<NBNode*>& getNodes() const {
        return myControlledNodes;
    }
    /// @}


    /** @brief Returns the information whether the described flow must let any other flow pass
     *
     * If the from/to connection passes only one junction (from is incoming into
     *  same node as to outgoes from) the node is asked whether the flow must brake-
     * Otherwise true is returned (recheck!)
     * "from" must be an incoming edge into one of the participating nodes!
     * @param[in] from The connection's start edge
     * @param[in] to The connection's end edge
     * @return Whether the described connection must brake (has higher priorised foes)
     */
    bool mustBrake(const NBEdge* const from, const NBEdge* const to) const;


    /** @brief Returns the information whether the described flow must let the other flow pass
     * @param[in] possProhibited The maybe prohibited connection
     * @param[in] possProhibitor The maybe prohibiting connection
     * @param[in] regardNonSignalisedLowerPriority Whether the right of way rules without traffic lights shall be regarded
     * @return Whether the second flow prohibits the first one
     * @see forbids
     */
    bool mustBrake(const NBConnection& possProhibited,
                   const NBConnection& possProhibitor,
                   bool regardNonSignalisedLowerPriority) const;

    /** @brief Returns the information whether the described flow must let any other flow pass
     * @param[in] possProhibitedFrom The maybe prohibited connection's begin
     * @param[in] possProhibitedTo The maybe prohibited connection's end
     * @param[in] possProhibitorFrom The maybe prohibiting connection's begin
     * @param[in] possProhibitorTo The maybe prohibiting connection's end
     * @param[in] regardNonSignalisedLowerPriority Whether the right of way rules without traffic lights shall be regarded
     * @return Whether the second flow prohibits the first one
     * @see forbids
     */
    bool mustBrake(const NBEdge* const possProhibitedFrom, const NBEdge* const possProhibitedTo,
                   const NBEdge* const possProhibitorFrom, const NBEdge* const possProhibitorTo,
                   bool regardNonSignalisedLowerPriority) const;


    /** @brief Returns the information whether "prohibited" flow must let "prohibitor" flow pass
     * @param[in] possProhibitedFrom The maybe prohibited connection's begin
     * @param[in] possProhibitedTo The maybe prohibited connection's end
     * @param[in] possProhibitorFrom The maybe prohibiting connection's begin
     * @param[in] possProhibitorTo The maybe prohibiting connection's end
     * @param[in] regardNonSignalisedLowerPriority Whether the right of way rules without traffic lights shall be regarded
     * @return Whether the second flow prohibits the first one
     * @see forbids
     */
    bool forbids(const NBEdge* const possProhibitorFrom, const NBEdge* const possProhibitorTo,
                 const NBEdge* const possProhibitedFrom, const NBEdge* const possProhibitedTo,
                 bool regardNonSignalisedLowerPriority) const;


    /** @brief Returns the information whether the given flows cross
     * @param[in] from1 The starting edge of the first stream
     * @param[in] to1 The ending edge of the first stream
     * @param[in] from2 The starting edge of the second stream
     * @param[in] to2 The ending edge of the second stream
     * @return Whether both stream are foes (cross)
     */
    bool foes(const NBEdge* const from1, const NBEdge* const to1,
              const NBEdge* const from2, const NBEdge* const to2) const;


    /** @brief Informs edges about being controlled by a tls
     * @param[in] ec The container of edges
     */
    virtual void setTLControllingInformation(const NBEdgeCont& ec) const = 0;


    /** @brief Builds the list of participating nodes/edges/links
     */
    virtual void setParticipantsInformation();


    /** @brief Adds the given ids into the list of inner edges controlled by the tls
     * @param[in] edges The list of edge ids which shall be controlled despite lying with the jointly controlled node cluster
     */
    void addControlledInnerEdges(const std::vector<std::string>& edges);

    /** @brief Retrieve the ids of edges explicitly controlled by the tls
     */
    std::vector<std::string> getControlledInnerEdges() const;

    /** @brief Replaces occurences of the removed edge in incoming/outgoing edges of all definitions
     * @param[in] removed The removed edge
     * @param[in] incoming The edges to use instead if an incoming edge was removed
     * @param[in] outgoing The edges to use instead if an outgoing edge was removed
     */
    virtual void remapRemoved(NBEdge* removed,
                              const EdgeVector& incoming, const EdgeVector& outgoing) = 0;


    /** @brief Replaces a removed edge/lane
     * @param[in] removed The edge to replace
     * @param[in] removedLane The lane of this edge to replace
     * @param[in] by The edge to insert instead
     * @param[in] byLane This edge's lane to insert instead
     */
    virtual void replaceRemoved(NBEdge* removed, int removedLane,
                                NBEdge* by, int byLane) = 0;

    /// @brief patches (loaded) signal plans by modifying lane indices
    virtual void shiftTLConnectionLaneIndex(NBEdge* edge, int offset) {
        UNUSED_PARAMETER(edge);
        UNUSED_PARAMETER(offset);
    }

    /** @brief returns the information whether the given link is a left-mover
     * @param[in] from The connection's start edge
     * @param[in] to The connection's end edge
     * @return Whether the connection is a left-mover
     */
    bool isLeftMover(const NBEdge* const from, const NBEdge* const to) const;


    /** @brief Returns the list of incoming edges (must be build first)
     * @return The edges which are incoming into the tls
     */
    const EdgeVector& getIncomingEdges() const;


    /// @brief returns the controlled links (depends on previous call to collectLinks)
    const NBConnectionVector& getControlledLinks() const {
        return myControlledLinks;
    }


    /** @brief Returns the ProgramID
     * @return The ID of the program (subID)
     */
    const std::string& getProgramID() const {
        return mySubID;
    };


    /** @brief Sets the programID
     * @param[in] programID The new ID of the program (subID)
     */
    void setProgramID(const std::string& programID) {
        mySubID = programID;
    }


    /** @brief Returns the offset
     * @return Offset
     */
    SUMOTime getOffset() {
        return myOffset;
    }


    /// @brief get the algorithm type (static etc..)
    TrafficLightType getType() const {
        return myType;
    }

protected:
    /** @brief Computes the traffic light logic finally in dependence to the type
     * @param[in] ec The edge container
     * @param[in] brakingTime Duration a vehicle needs for braking in front of the tls
     * @return The computed logic
     */
    virtual NBTrafficLightLogic* myCompute(const NBEdgeCont& ec,
                                           unsigned int brakingTime) = 0;


    /** @brief Collects the links participating in this traffic light
     * @exception ProcessError If a link could not be found
     */
    virtual void collectLinks() = 0;


    /** @brief Build the list of participating edges
     */
    void collectEdges();


    /** @brief Computes the time vehicles may need to brake
     *
     * This time depends on the maximum speed allowed on incoming junctions.
     * It is computed as max_speed_allowed / minimum_vehicle_decleration
     */
    unsigned int computeBrakingTime(SUMOReal minDecel) const;


    // @return whether this traffic light is invalid and should be computed
    virtual bool amInvalid() const;

    /// @brief helper method for use in NBOwnTLDef and NBLoadedSUMOTLDef
    void collectAllLinks();


protected:
    /// @brief The container with participating nodes
    std::vector<NBNode*> myControlledNodes;

    /// @brief The list of incoming edges
    EdgeVector myIncomingEdges;

    /// @brief The list of edges within the area controlled by the tls
    EdgeVector myEdgesWithin;

    /// @brief The list of controlled links
    NBConnectionVector myControlledLinks;

    /// @brief Set of inner edges that shall be controlled, though
    std::set<std::string> myControlledInnerEdges;

    /// @brief The tls program's subid
    std::string mySubID;

    /// @brief The offset in the program
    SUMOTime myOffset;

    /// @brief The algorithm type for the traffic light
    TrafficLightType myType;


};


#endif

/****************************************************************************/

