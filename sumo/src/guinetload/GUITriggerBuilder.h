/****************************************************************************/
/// @file    GUITriggerBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Builds trigger objects for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUITriggerBuilder_h
#define GUITriggerBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <netload/NLTriggerBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrigger;
class MSNet;
class MSLaneSpeedTrigger;
class MSCalibrator;
class MSTriggerControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITriggerBuilder
 * @brief Builds trigger objects for guisim
 */
class GUITriggerBuilder : public NLTriggerBuilder {
public:
    /// @brief Constructor
    GUITriggerBuilder();


    /// @brief Destructor
    ~GUITriggerBuilder();


protected:
    /// @name building methods
    ///
    /// Override NLTriggerBuilder-methods for building guisim-objects
    /// @see NLTriggerBuilder
    //@{

    /** @brief Builds a lane speed trigger
     *
     * Simply calls the GUILaneSpeedTrigger constructor.
     *
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] id The id of the lane speed trigger
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     * @see MSLaneSpeedTrigger
     * @exception ProcessError If the XML definition file is errornous
     */
    virtual MSLaneSpeedTrigger* buildLaneSpeedTrigger(MSNet& net,
            const std::string& id, const std::vector<MSLane*>& destLanes,
            const std::string& file);


    /** @brief builds an rerouter
     *
     * Simply calls the GUITriggeredRerouter constructor.
     *
     * @param[in] net The net the rerouter belongs to
     * @param[in] id The id of the rerouter
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     * @param[in] file The file to read the reroute definitions from
     */
    virtual MSTriggeredRerouter* buildRerouter(MSNet& net,
            const std::string& id, MSEdgeVector& edges,
            SUMOReal prob, const std::string& file, bool off);


    /** @brief Builds a bus stop
     *
     * Simply calls the GUIBusStop / GUIContainerStop constructor.
     *
     * @param[in] net The net the stop belongs to
     * @param[in] id The id of the stop
     * @param[in] lines Names of the lines that halt on this stop
     * @param[in] lane The lane the stop is placed on
     * @param[in] frompos Begin position of the stop on the lane
     * @param[in] topos End position of the stop on the lane
     * @param[in] element which kind of stop is to be built
     * @exception InvalidArgument If the stop can not be added to the net (is duplicate)
     */
    virtual void buildStoppingPlace(MSNet& net, const std::string& id,
                                    const std::vector<std::string>& lines, MSLane* lane,
                                    SUMOReal frompos, SUMOReal topos, const SumoXMLTag element);


    /** @brief Builds a parking area
     *
     * Simply calls the GUIParkingArea constructor.
     *
     * @param[in] net The net the parking area belongs to
     * @param[in] id The id of the parking area
     * @param[in] lines Names of the lines that halt on this parking area
     * @param[in] lane The lane the parking area is placed on
     * @param[in] frompos Begin position of the parking area on the lane
     * @param[in] topos End position of the parking area on the lane
     * @param[in] capacity Capacity of the parking area
     * @param[in] width Width of the default lot rectangle
     * @param[in] length Length of the default lot rectangle
     * @param[in] angle Angle of the default lot rectangle
     * @exception InvalidArgument If the parking area can not be added to the net (is duplicate)
     */
    virtual void beginParkingArea(MSNet& net, const std::string& id,
                                  const std::vector<std::string>& lines, MSLane* lane,
                                  SUMOReal frompos, SUMOReal topos,
                                  unsigned int capacity,
                                  SUMOReal width, SUMOReal length, SUMOReal angle);


    /** @brief Builds a charging station
     *
     * Simply calls the ChrgSTN constructor.
     *
     * @param[in] net The net the charging station belongs to
     * @param[in] id The id of the charging station
     * @param[in] lane The lane the charging station is placed on
     * @param[in] frompos Begin position of the charging station on the lane
     * @param[in] topos End position of the charging station on the lane
     * @param[in] chargingPower energy charged in every timeStep
     * @param[in] efficiency efficiency of the charge
     * @param[in] chargeInTransit enable or disable charge in transit
     * @param[in] chargeDelay delay in the charge
     * @exception InvalidArgument If the charging station can not be added to the net (is duplicate)
     */
    virtual void buildChargingStation(MSNet& net, const std::string& id, MSLane* lane,
                                      SUMOReal frompos, SUMOReal topos, SUMOReal chargingPower, SUMOReal efficiency,
                                      bool chargeInTransit, int chargeDelay);

    /** @brief builds a microscopic calibrator
     *
     * Simply calls the MSCalibrator constructor.
     *
     * @param[in] net The net the calibrator belongs to
     * @param[in] id The id of the calibrator
     * @param[in] edge The edge the calibrator is placed at
     * @param[in] pos The position on the edge the calibrator lies at
     * @param[in] file The file to read the flows from
     * @todo Is the position correct/needed
     */
    virtual MSCalibrator* buildCalibrator(MSNet& net,
                                          const std::string& id, MSEdge* edge, SUMOReal pos,
                                          const std::string& file, const std::string& outfile,
                                          const SUMOTime freq, const MSRouteProbe* probe);
    /// @}

};


#endif

/****************************************************************************/

