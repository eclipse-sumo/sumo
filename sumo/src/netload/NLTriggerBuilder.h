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
// Revision 1.8  2005/10/10 12:11:33  dkrajzew
// debugging
//
// Revision 1.7  2005/10/07 11:41:49  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:12  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
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
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSTrigger;
class MSNet;
class MSLaneSpeedTrigger;
class MSTriggeredEmitter;
class NLHandler;
class MSTriggeredRerouter;
class MSLane;
class MSEdge;

#ifdef HAVE_MESOSIM
class METriggeredCalibrator;
class MESegment;
#endif


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NLTriggerBuilder
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
    MSTrigger *buildTrigger(MSNet &net, const Attributes &attrs,
        const std::string &base, const NLHandler &helper);

protected:
    /// builds a lane speed trigger
    MSLaneSpeedTrigger *parseAndBuildLaneSpeedTrigger(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);

    /// builds an emitter
    MSTriggeredEmitter *parseAndBuildLaneEmitTrigger(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);

    /// builds a rerouter
    MSTriggeredRerouter *parseAndBuildRerouter(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);

#ifdef HAVE_MESOSIM
    /// Builds a mesoscopic calibrator
    METriggeredCalibrator *parseAndBuildCalibrator(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);
#endif

protected:
    /// builds a lane speed trigger
    virtual MSLaneSpeedTrigger *buildLaneSpeedTrigger(MSNet &net,
        const std::string &id, const std::vector<MSLane*> &destLanes,
        const std::string &file);

    /// builds an emitter
    virtual MSTriggeredEmitter *buildLaneEmitTrigger(MSNet &net,
        const std::string &id, MSLane *destLane, SUMOReal pos,
        const std::string &file);

#ifdef HAVE_MESOSIM
    /// builds a calibrator
    virtual METriggeredCalibrator *buildCalibrator(MSNet &net,
        const std::string &id, MESegment *edge, SUMOReal pos,
        const std::string &rfile, const std::string &file);
#endif

    /// builds an emitter
    virtual MSTriggeredRerouter *buildRerouter(MSNet &net,
        const std::string &id, std::vector<MSEdge*> &edges,
        SUMOReal prob, const std::string &file);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
