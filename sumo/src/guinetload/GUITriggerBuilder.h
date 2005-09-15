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
// Revision 1.3  2005/09/15 11:06:03  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 12:00:09  dkrajzew
// level 3 warnings removed; code style adapted
//
// Revision 1.1  2004/07/02 08:39:56  dkrajzew
// visualisation of vss' added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <netload/NLTriggerBuilder.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSTrigger;
class MSNet;
class MSLaneSpeedTrigger;
class MSTriggerControl;


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

    /// builds an emitter
    virtual MSTriggeredEmitter *buildLaneEmitTrigger(MSNet &net,
        const std::string &id, MSLane *destLane, double pos,
        const std::string &file);

    /// builds an emitter
    virtual MSTriggeredRerouter *buildRerouter(MSNet &net,
        const std::string &id, std::vector<MSEdge*> &edges,
        float prob, const std::string &file);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
