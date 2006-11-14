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
// Revision 1.17  2006/11/14 06:47:57  dkrajzew
// code beautifying
//
// Revision 1.16  2006/11/08 16:37:22  ericnicolay
// change code for actortrigger
//
// Revision 1.15  2006/10/31 12:22:14  dkrajzew
// code beautifying
//
// Revision 1.14  2006/07/05 11:45:43  ericnicolay
// change code in buildVehicleActor
//
// Revision 1.13  2006/06/22 07:17:27  dkrajzew
// removed unneeded class declarations
//
// Revision 1.12  2006/06/13 13:17:48  dkrajzew
// removed unneeded code
//
// Revision 1.11  2006/03/27 07:20:28  dkrajzew
// vehicle actors added, joined some commonly used functions, documentation added
//
// Revision 1.10  2006/01/09 12:00:28  dkrajzew
// bus stops implemented
//
// Revision 1.9  2005/11/09 06:35:03  dkrajzew
// Emitters reworked
//
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
class MSEmitter;
class NLHandler;
class MSTriggeredRerouter;
class MSLane;
class MSEdge;
class MSBusStop;
class MSE1VehicleActor;


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
    //{ @brief Parsing methods // !!! check this in doxygen
    ///
    /// These methods parse the attributes for each of the described trigger
    ///  and call the according methods to build the trigger

    /// builds a lane speed trigger
    MSLaneSpeedTrigger *parseAndBuildLaneSpeedTrigger(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);

    /// builds an emitter
    MSEmitter *parseAndBuildLaneEmitTrigger(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);

    /// builds a rerouter
    MSTriggeredRerouter *parseAndBuildRerouter(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);

    /// builds a busstop
    MSBusStop *parseAndBuildBusStop(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);

    /// builds a vehicle actor
    MSE1VehicleActor *parseAndBuildVehicleActor(MSNet &net,
        const Attributes &attrs, const std::string &base,
        const NLHandler &helper);
    //}

protected:
    //{ @brief Building methods // !!! check this in doxygen
    ///
    /// Called with parsed values, these methods build the trigger.
    /// These methods should be overriden for the gui loader in order
    ///  to build visualizable versions of the triggers
    /// In most cases, these methods only call the constructor and
    ///  return the so build trigger

    /// builds a lane speed trigger
    virtual MSLaneSpeedTrigger *buildLaneSpeedTrigger(MSNet &net,
        const std::string &id, const std::vector<MSLane*> &destLanes,
        const std::string &file);

    /// builds an emitter
    virtual MSEmitter *buildLaneEmitTrigger(MSNet &net,
        const std::string &id, MSLane *destLane, SUMOReal pos,
        const std::string &file);

    /// builds a bus stop
    virtual MSBusStop* buildBusStop(MSNet &net,
        const std::string &id, const std::vector<std::string> &lines,
        MSLane *lane, SUMOReal frompos, SUMOReal topos);

    /// builds an emitter
    virtual MSTriggeredRerouter *buildRerouter(MSNet &net,
        const std::string &id, std::vector<MSEdge*> &edges,
        SUMOReal prob, const std::string &file);

    /// builds a vehicle actor
	virtual MSE1VehicleActor *buildVehicleActor(MSNet &net,
		const std::string &id, MSLane *lane, SUMOReal pos,
		unsigned int la, unsigned int cell, unsigned int type);
    //}

protected:
    /// Helper method to obtain the filename
    std::string getFileName(const Attributes &attrs,
        const std::string &base, const NLHandler &helper);

    /** @brief returns the lane defined by objectid
     *
     * Writes an error and throws a ProcessException if the lane does not exist.
     * The last strings given as parameter define the object type and id for
     *  building the error string.
     */
    MSLane *getLane(const Attributes &attrs, const NLHandler &helper,
        const std::string &tt, const std::string &tid);

    /** @brief returns the position on the lane checking it
     *
     * This method extracts the position, checks whether it shall be mirrored
     *  and checks whether it is within the lane. If not, an error is reported
     *  and a InvalidArgument is thrown.
     * The last strings given as parameter define the object type and id for
     *  building the error string.
     */
    SUMOReal getPosition(const Attributes &attrs, const NLHandler &helper,
        MSLane *lane, const std::string &tt, const std::string &tid);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
