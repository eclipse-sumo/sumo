/****************************************************************************/
/// @file    NBTrafficLightDefinition.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
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

#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <set>
#include <utils/common/Named.h>
#include <utils/common/DoubleVector.h>
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
class NBTrafficLightPhases;


// ===========================================================================
// class definitions
// ===========================================================================
class NBTrafficLightDefinition
            : public Named
{

public:

    enum TLColor {
        TLCOLOR_UNKNOWN,
        TLCOLOR_RED,
        TLCOLOR_YELLOW,
        TLCOLOR_REDYELLOW,
        TLCOLOR_GREEN,
        TLCOLOR_BLINK
    };


    /// Constructor
    NBTrafficLightDefinition(const std::string &id,
                             const std::set<NBNode*> &junctions);

    /// Constructor
    NBTrafficLightDefinition(const std::string &id,
                             NBNode *junction);

    /// Constructor
    NBTrafficLightDefinition(const std::string &id, std::string type,
                             NBNode *junction);

    /// Constructor
    NBTrafficLightDefinition(const std::string &id);

    /// Destructor
    virtual ~NBTrafficLightDefinition();

    /** @brief Computes the traffic light logic
        Does some initialisation at first, then calls myCompute to finally build the tl-logic */
    NBTrafficLightLogicVector *compute(const NBEdgeCont &ec,
                                       OptionsCont &oc);

    /// Adds a node to the traffic light logic
    void addNode(NBNode *node);

    bool mustBrake(NBEdge *from, NBEdge *to) const;

    bool mustBrake(const NBConnection &possProhibited,
                   const NBConnection &possProhibitor,
                   bool regardNonSignalisedLowerPriority) const;

    bool mustBrake(NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
                   NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
                   bool regardNonSignalisedLowerPriority) const;

    bool forbids(NBEdge *possProhibitorFrom, NBEdge *possProhibitorTo,
                 NBEdge *possProhibitedFrom, NBEdge *possProhibitedTo,
                 bool regardNonSignalisedLowerPriority) const;

    bool foes(NBEdge *from1, NBEdge *to1,
              NBEdge *from2, NBEdge *to2) const;

    virtual void setTLControllingInformation(const NBEdgeCont &ec) const = 0;

    virtual void setParticipantsInformation();

public:
    virtual void remapRemoved(NBEdge *removed,
                              const EdgeVector &incoming, const EdgeVector &outgoing) = 0;

    virtual void replaceRemoved(NBEdge *removed, int removedLane,
                                NBEdge *by, int byLane) = 0;

protected:
    /// Computes the traffic light logic finally in dependence to the type
    virtual NBTrafficLightLogicVector *myCompute(const NBEdgeCont &ec,
            size_t breakingTime,
            std::string type, bool buildAll) = 0;

    std::pair<size_t, size_t> getSizes() const;

    virtual void collectLinks();

    void collectEdges();

private:
    NBTrafficLightLogicVector *buildLoadedTrafficLights(
        size_t breakingTime);

    NBTrafficLightLogicVector *buildOwnTrafficLights(
        size_t breakingTime, bool buildAll) const;

    size_t computeBrakingTime(SUMOReal minDecel) const;

    /// returns the information whether the given link is a left-mover
    bool isLeftMover(NBEdge *from, NBEdge *to) const;


    friend class NBRequestEdgeLinkIterator;

protected:

    // the type of the traffic light (traffic-light, actuated, agentbased)
    std::string myType;

    /// Definition of the container type for participating nodes
    typedef std::set<NBNode*> NodeCont;

    /// The container with participating nodes
    NodeCont _nodes;

    ///
    EdgeVector _incoming;

    EdgeVector _within;

    NBConnectionVector _links;

};


#endif

/****************************************************************************/

