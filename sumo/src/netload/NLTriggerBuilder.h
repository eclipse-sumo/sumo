/****************************************************************************/
/// @file    NLTriggerBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Oct 2002
/// @version $Id: $
///
// A building helper for triggers
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
#ifndef NLTriggerBuilder_h
#define NLTriggerBuilder_h
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

#include <string>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
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

#ifdef HAVE_MESOSIM
class METriggeredCalibrator;
class MESegment;
class METriggeredScaler;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLTriggerBuilder
 * This class builds trigger objects in their non-gui version
 */
class NLTriggerBuilder
{
public:
    /// Constructor
    NLTriggerBuilder();

    /// Destructor
    virtual ~NLTriggerBuilder();

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

#ifdef HAVE_MESOSIM
    /// Builds a mesoscopic calibrator
    METriggeredCalibrator *parseAndBuildCalibrator(MSNet &net,
            const Attributes &attrs, const std::string &base,
            const NLHandler &helper);

    /// Builds a mesoscopic scaler
    METriggeredScaler *parseAndBuildScaler(MSNet &net,
                                           const Attributes &attrs, const std::string &base,
                                           const NLHandler &helper);
#endif
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

#ifdef HAVE_MESOSIM
    /// builds a calibrator
    virtual METriggeredCalibrator *buildCalibrator(MSNet &net,
            const std::string &id, MESegment *edge, SUMOReal pos,
            const std::string &rfile, const std::string &file);

    /// builds a scaler
    virtual METriggeredScaler *buildScaler(MSNet &net,
                                           const std::string &id, MESegment *edge, SUMOReal pos,
                                           SUMOReal scale);
#endif

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


#endif

/****************************************************************************/

