/****************************************************************************/
/// @file    NLSucceedingLaneBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 22 Oct 2001
/// @version $Id$
///
// Container for the succeding lanes of a lane during their building
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
#ifndef NLSucceedingLaneBuilder_h
#define NLSucceedingLaneBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSLinkCont.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;
class NLJunctionControlBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLSucceedingLaneBuilder
 * A container for the temporary storage of a lanes succeeding lanes while
 * parsing them
 */
class NLSucceedingLaneBuilder
{
public:
    /// standard constructor
    NLSucceedingLaneBuilder(NLJunctionControlBuilder &jb);

    /// Destructor
    ~NLSucceedingLaneBuilder();

    /** opens the computation of a container holding the succeding lanes of
        a lane */
    void openSuccLane(const std::string &laneId);

    /// add a succeeding lane
    void addSuccLane(bool yield, const std::string &laneId,
#ifdef HAVE_INTERNAL_LANES
                     const std::string &viaID, SUMOReal pass,
#endif
                     MSLink::LinkDirection dir, MSLink::LinkState state,
                     bool internalEnd,
                     const std::string &tlid="", size_t linkNo=0);

    /// closes the building
    void closeSuccLane();

    /// returns the name of the lane the succeeding lanes are added to
    std::string getSuccingLaneName() const;

private:
    /// the id of the lane the succeeding lanes are added to
    std::string m_CurrentLane;

    /// the list of connections
    MSLinkCont   *m_SuccLanes;

    NLJunctionControlBuilder &myJunctionControlBuilder;

private:
    /** invalid copy constructor */
    NLSucceedingLaneBuilder(const NLSucceedingLaneBuilder &s);

    /** invalid assignment operator */
    NLSucceedingLaneBuilder &operator=(const NLSucceedingLaneBuilder &s);

};


#endif

/****************************************************************************/

