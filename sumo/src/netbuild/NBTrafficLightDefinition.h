#ifndef NBTrafficLightDefinition_h
#define NBTrafficLightDefinition_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightDefinition.h -
//  The definition of a traffic light logic
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
// Revision 1.4  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.3  2003/06/24 08:21:01  dkrajzew
// some further work on importing traffic lights
//
// Revision 1.2  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.1  2003/06/05 11:43:20  dkrajzew
// definition class for traffic lights added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <utils/common/Named.h>
#include <utils/common/DoubleVector.h>
#include "NBCont.h"
#include "NBConnection.h"
#include "NBConnectionDefs.h"
#include "NBLinkPossibilityMatrix.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;
class OptionsCont;
class NBTrafficLightLogicVector;
class NBTrafficLightPhases;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBTrafficLightDefinition
    : public Named {

public:

    enum TLColor {
        TLCOLOR_UNKNOWN,
        TLCOLOR_RED,
        TLCOLOR_YELLOW,
        TLCOLOR_REDYELLOW,
        TLCOLOR_GREEN,
        TLCOLOR_BLINK,
        TLCOLOR_BLUE // :-)
    };


    /// Constructor
    NBTrafficLightDefinition(const std::string &id,
        const std::vector<NBNode*> &junctions);

    /// Constructor
    NBTrafficLightDefinition(const std::string &id,
        NBNode *junction);

    /// Constructor
    NBTrafficLightDefinition(const std::string &id);

    /// Destructor
    virtual ~NBTrafficLightDefinition();

    /** @brief Computes the traffic light logic
        Does some initialisation at first, then calls myCompute to finally build the tl-logic */
    NBTrafficLightLogicVector *compute(OptionsCont &oc);

    /// Adds a node to the traffic light logic
    void addNode(NBNode *node);

	bool mustBrake(NBEdge *from, NBEdge *to) const;

    bool mustBrake(const NBConnection &conn,
        const NBConnection &source) const;

    bool mustBrake(NBEdge *from1, NBEdge *to1, NBEdge *from2, NBEdge *to2) const;

    bool forbids(NBEdge *from1, NBEdge *to1,
        NBEdge *from2, NBEdge *to2) const;

    bool foes(NBEdge *from1, NBEdge *to1,
        NBEdge *from2, NBEdge *to2) const;

public:
    virtual void remapRemoved(NBEdge *removed,
        const EdgeVector &incoming, const EdgeVector &outgoing) = 0;

    virtual void replaceRemoved(NBEdge *removed, size_t removedLane,
        NBEdge *by, size_t byLane) = 0;

protected:
    /// Computes the traffic light logic finally in dependence to the type
    virtual NBTrafficLightLogicVector *myCompute(size_t breakingTime,
        bool buildAll) = 0;

    /// Collects the nodes participating in this traffic light
    virtual void collectNodes() = 0;

    virtual void collectLinks() = 0;

    std::pair<size_t, size_t> getSizes() const;



private:
    NBTrafficLightLogicVector *buildLoadedTrafficLights(
        size_t breakingTime);

    NBTrafficLightLogicVector *buildOwnTrafficLights(
        size_t breakingTime, bool buildAll) const;


/*    NBTrafficLightLogic *buildTrafficLightsLogic(const std::string &key,
        size_t noLinks, const PhaseIndexVector &phaseList,
        NBLinkCliqueContainer &cliquen,
        const NBRequestEdgeLinkIterator &cei1) const;*/
/*
    std::pair<std::bitset<64>, std::bitset<64> >
        buildPhaseMasks(size_t time) const;
*/
    size_t computeBrakingTime(double minDecel) const;


    void collectEdges();

    /// returns the information whether the given link is a left-mover
    bool isLeftMover(NBEdge *from, NBEdge *to) const;


    friend class NBRequestEdgeLinkIterator;

protected:


    /// Definition of the container type for participating nodes
    typedef std::vector<NBNode*> NodeCont;

    /// The container with participating nodes
    NodeCont _nodes;

    ///
    EdgeVector _incoming;

    EdgeVector _within;

//    EdgeVector _outgoing;

    NBConnectionVector _links;

};




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBTrafficLightDefinition.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

