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
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>

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
 * NLTriggerBuilder
 * This class builds the sources
 */
class NLTriggerBuilder {
public:
    /// builds a MSTriggeredSource - source
    static MSTrigger *buildTrigger(MSNet &net,
        const std::string &id, 
        const std::string &objecttype, const std::string &objectid, 
        const std::string &objectattr,
        std::string file, std::string base);
private:
    static MSLaneSpeedTrigger *buildLaneSpeedTrigger(MSNet &net,
        const std::string &id, MSLane &lane,
        const std::string &file);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLTriggerBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//

