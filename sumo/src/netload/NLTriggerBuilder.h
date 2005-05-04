#ifndef NLTriggerBuilder_h
#define NLTriggerBuilder_h
/***************************************************************************
                          NLTriggerBuilder.h
                          A building helper for triggers
                             -------------------
    begin                : Thu, 17 Oct 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.4  2005/05/04 08:43:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSTrigger;
class MSNet;
class MSLaneSpeedTrigger;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class
 * This class builds trigger objects in their non-gui version
 */
class NLTriggerBuilder {
public:
    /// Constructor
    NLTriggerBuilder();

    /// Destructor
    ~NLTriggerBuilder();

    /** @brief builds the specified trigger
        The certain type and purpose of the trigger is not yet known */
    MSTrigger *buildTrigger(MSNet &net,
        const std::string &id,
        const std::string &objecttype, const std::string &objectid,
        const std::string &objectattr,
        std::string file, std::string base);

protected:
    /// builds a lane speed trigger
    virtual MSLaneSpeedTrigger *buildLaneSpeedTrigger(MSNet &net,
        const std::string &id, const std::vector<MSLane*> &destLanes,
        const std::string &file);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
