/****************************************************************************/
/// @file    NBTrafficLightDefinition.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The definition of a traffic light logic
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
#include <utils/common/Named.h>
#include <utils/common/VectorHelper.h>
#include <utils/common/SUMOTime.h>
#include "NBCont.h"
#include "NBConnection.h"
#include "NBConnectionDefs.h"
#include "NBLinkPossibilityMatrix.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;
class OptionsCont;
class NBTrafficLightLogicVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTrafficLightDefinition
 * @brief The definition of a traffic light logic
 */
class NBTrafficLightDefinition : public Named
{
public:
    /**
     * @enum TLColor
     * @brief An enumeration of possible tl-signal states
     */
    enum TLColor {
        /// Signal shows red
        TLCOLOR_RED,
        /// Signal shows yellow
        TLCOLOR_YELLOW,
        /// Signal shows red/yellow (!!! unused)
        TLCOLOR_REDYELLOW,
        /// Signal shows green
        TLCOLOR_GREEN,
        /// Signal is blinking yellow
        TLCOLOR_BLINK
    };


    /// Constructor
    NBTrafficLightDefinition(const std::string &id,
                             const std::set<NBNode*> &junctions) throw();

    /// Constructor
    NBTrafficLightDefinition(const std::string &id,
                             NBNode *junction) throw();

    /// Constructor
    NBTrafficLightDefinition(const std::string &id, std::string type,
                             NBNode *junction) throw();

    /// Constructor
    NBTrafficLightDefinition(const std::string &id) throw();

    /// Destructor
    virtual ~NBTrafficLightDefinition() throw();

    /** @brief Computes the traffic light logic
        Does some initialisation at first, then calls myCompute to finally build the tl-logic */
    NBTrafficLightLogicVector *compute(const NBEdgeCont &ec,
                                       OptionsCont &oc);

    /// Adds a node to the traffic light logic
    void addNode(NBNode *node);

    /// @brief Removes the given node from the list of controlled nodes
    void removeNode(NBNode *node);


    /** @brief Returns the information whether the described flow must let any other flow pass
     *
     * If the from/to connection passes only one junction (from is incoming into
     *  same node as to outgoes from) the node is asked whether the flow must brake-
     * Otherwise true is returned (recheck!)
     * "from" must be an incoming edge into one of the participating nodes!
     */
    bool mustBrake(NBEdge *from, NBEdge *to) const;

    /** @brief Returns the information whether the described flow must let any other flow pass
     *
     * uses "forbids"
     */
    bool mustBrake(const NBConnection &possProhibited,
                   const NBConnection &possProhibitor,
                   bool regardNonSignalisedLowerPriority) const;

    /** @brief Returns the information whether the described flow must let any other flow pass
     *
     * uses "forbids"
     */
    bool mustBrake(NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
                   NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
                   bool regardNonSignalisedLowerPriority) const;

    /** @brief Returns the information whether "prohibited" flow must let "prohibitor" flow pass
     */
    bool forbids(NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
                 NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
                 bool regardNonSignalisedLowerPriority) const;

    /// Returns the information whether the given flows cross
    bool foes(NBEdge *from1, NBEdge *to1,
              NBEdge *from2, NBEdge *to2) const;

    /// Inserts the information about being controlled by a tls into participating edges
    virtual void setTLControllingInformation(const NBEdgeCont &ec) const = 0;

    /// Builds the list of participating edges/links
    virtual void setParticipantsInformation();

    /// Adds the given ids into the list of edges not controlled by the tls
    void addControlledInnerEdges(const std::vector<std::string> &edges);

    /// Remaps loaded information in the case an edge was removed
    virtual void remapRemoved(NBEdge *removed,
                              const EdgeVector &incoming, const EdgeVector &outgoing) = 0;

    /// Remaps loaded information in the case an edge was removed
    virtual void replaceRemoved(NBEdge *removed, int removedLane,
                                NBEdge *by, int byLane) = 0;

    /// returns the information whether the given link is a left-mover
    bool isLeftMover(NBEdge *from, NBEdge *to) const;

    /// Returns the list of incoming edges (must be build first)
    const EdgeVector &getIncomingEdges() const;

protected:
    /// Computes the traffic light logic finally in dependence to the type
    virtual NBTrafficLightLogicVector *myCompute(const NBEdgeCont &ec,
            size_t breakingTime,
            std::string type) = 0;

    /// Returns a pair of <number participating lanes, number participating links>
    std::pair<unsigned int, unsigned int> getSizes() const;

    /// Build the list of participating links
    virtual void collectLinks();

    /// Build the list of participating edges
    void collectEdges();

private:
    /** @brief Computes the time vehicles may need to brake
     *
     * This time depends on the maximum speed allowed on incoming junctions.
     * It is computed as max_speed_allowed / minimum_vehicle_decleration
     */
    size_t computeBrakingTime(SUMOReal minDecel) const;

protected:

    // the type of the traffic light (traffic-light, actuated, agentbased)
    std::string myType;

    /// Definition of the container type for participating nodes
    typedef std::set<NBNode*> NodeCont;

    /// The container with participating nodes
    NodeCont myControlledNodes;

    /// The list of incoming edges
    EdgeVector myIncomingEdges;

    /// The list of edges within the area controlled by the tls
    EdgeVector myEdgesWithin;

    /// The list of controlled links
    NBConnectionVector myControlledLinks;

    /// List of inner edges that shall be controlled, though
    std::vector<std::string> myControlledInnerEdges;

};


#endif

/****************************************************************************/

