#ifndef GUITriggerBuilder_h
#define GUITriggerBuilder_h
/***************************************************************************
                          GUITriggerBuilder.h
                          A building helper for triggers
                             -------------------
    begin                : Mon, 26.04.2004
    copyright            : (C) 2004 by DLR http://ivf.dlr.de/
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
// Revision 1.1  2004/07/02 08:39:56  dkrajzew
// visualisation of vss' added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <netload/NLTriggerBuilder.h>


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
class GUITriggerBuilder : public NLTriggerBuilder {
public:
    /// Constructor
    GUITriggerBuilder();

    /// Destructor
    ~GUITriggerBuilder();

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
