/****************************************************************************/
/// @file    GNEAdditionalHandler
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// Builds trigger objects for netedit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEAdditionalHandler_h
#define GNEAdditionalHandler_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position.h>

#include "GNECalibrator.h"
#include "GNEVariableSpeedSign.h"
#include "GNERerouterInterval.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;
class GNEViewNet;
class GNEUndoList;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEDetectorE3;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNEAdditionalHandler
/// @brief Builds trigger objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNEAdditionalHandler : public SUMOSAXHandler {
public:
    /// @brief Constructor
    GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEAdditionalHandler();

    /// @name inherited from GenericSAXHandler
    /// @{
    /**@brief Called on the opening of a tag;
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element, const SUMOSAXAttributes& attrs);
    /// @}

    /// @name parsing methods
    ///
    /// These methods parse the attributes for each of the described trigger
    /// and call the according methods to build the trigger
    /// @{
    /**@brief Builds a vaporization
     * @param[in] attrs SAX-attributes which define the vaporizer
     * @param[in] tag of the additional
     * @note recheck throwing the exception
     */
    void parseAndBuildVaporizer(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Variable Speed Signal (lane speed trigger)
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     * @see buildLaneSpeedTrigger
     */
    void parseAndBuildVariableSpeedSign(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a rerouter
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a bus stop
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildBusStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a container stop
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildContainerStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a charging station
     * @param[in] attrs SAXattributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildChargingStation(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a mesoscopic or microscopic calibrator
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildCalibrator(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a induction loop detector (E1)
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildDetectorE1(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a lane area detector (E2)
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildDetectorE2(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a multi entry exit detector (E3)
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildDetectorE3(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Entry detector
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildDetectorEntry(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Exit detector
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildDetectorExit(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds routeProbe
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildRouteProbe(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses flow values of Calibrators
     * @param[in] attrs SAX-attributes which define the flows
     * @param[in] tag of the additional
     */
    void parseCalibratorFlow(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses step values of VariableSpeedSigns
     * @param[in] attrs SAX-attributes which define the steps
     * @param[in] tag of the additional
     */
    void parseVariableSpeedSignStep(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);
    /// @}

    /// @name building methods
    ///
    /// Called with parsed values, these methods build the trigger.
    /// @{
    /**@brief Build additionals
     * @param[in] viewNet pointer to viewNet in wich additional will be created
     * @param[in] tag tag of the additiona lto create
     * @param[in] values map with the attributes and values of the additional to create
     * @return true if was sucesfully created, false in other case
     */
    static bool buildAdditional(GNEViewNet* viewNet, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values);

    /**@brief Builds a bus stop
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] startPos Begin position of the bus stop on the lane
     * @param[in] endPos End position of the bus stop on the lane
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the bus stop can not be added to the net (is duplicate)
     */
    static bool buildBusStop(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal startPos, SUMOReal endPos, const std::vector<std::string>& lines);

    /**@brief Builds a container stop
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the container stop
     * @param[in] lane The lane the container stop is placed on
     * @param[in] startPos Begin position of the container stop on the lane
     * @param[in] endPos End position of the container stop on the lane
     * @param[in] lines Names of the bus lines that halt on this container stop
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the container stop can not be added to the net (is duplicate)
     */
    static bool buildContainerStop(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal startPos, SUMOReal endPos, const std::vector<std::string>& lines);

    /**@brief Builds a charging Station
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the charging Station
     * @param[in] lane The lane the charging Station is placed on
     * @param[in] startPos Begin position of the charging Station on the lane
     * @param[in] endPos End position of the charging Station on the lane
     * @param[in] chargingPower power charged in every timeStep
     * @param[in] efficiency efficiency of the charge
     * @param[in] chargeInTransit enable or disable charge in transit
     * @param[in] chargeDelay delay in the charge
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the charging Station can not be added to the net (is duplicate)
     */
    static bool buildChargingStation(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal startPos, SUMOReal endPos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay);

    /**@brief Builds a induction loop detector (E1)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the detector
     * @param[in] lane The lane the detector is placed on
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] splitByType If set, the collected values will be additionally reported on per-vehicle type base.
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    static bool buildDetectorE1(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal pos, SUMOReal freq, const std::string& filename, bool splitByType);

    /**@brief Builds a lane Area Detector (E2)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the detector
     * @param[in] lane The lane the detector is placed on
     * @param[in] pos position of the detector on the lane
     * @param[in[ length length of the detector
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] cont Holds the information whether detectors longer than a lane shall be cut off or continued
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @param[in] jamThreshold The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    static bool buildDetectorE2(GNEViewNet* viewNet, const std::string& id, GNELane* lane, SUMOReal pos, SUMOReal length, SUMOReal freq, const std::string& filename,
                                bool cont, const SUMOReal timeThreshold, SUMOReal speedThreshold, SUMOReal jamThreshold);

    /**@brief Builds a multi entry exit detector (E3)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the detector
     * @param[in] pos position of the detector in the map
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    static bool buildDetectorE3(GNEViewNet* viewNet, const std::string& id, Position pos, SUMOReal freq, const std::string& filename, const SUMOReal timeThreshold, SUMOReal speedThreshold);

    /**@brief Builds a entry detector (E3)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] E3Parent pointer to E3 detector parent
     * @param[in] lane The lane in which the entry detector is placed on
     * @param[in] pos position of the entry detector on the lane
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the entry detector can not be added to the net (invalid parent or lane)
     */
    static bool buildDetectorEntry(GNEViewNet* viewNet, GNEDetectorE3* E3Parent, GNELane* lane, SUMOReal pos);

    /**@brief Builds a exit detector (E3)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] E3Parent pointer to E3 detector parent
     * @param[in] lane The lane in which the exit detector is placed on
     * @param[in] pos position of the exit detector on the lane
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the exit detector can not be added to the net (invalid parent or lane
     */
    static bool buildDetectorExit(GNEViewNet* viewNet, GNEDetectorE3* E3Parent, GNELane* lane, SUMOReal pos);

    /**@brief builds a microscopic calibrator
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the calibrator
     * @param[in] edge The edge the calibrator is placed at
     * @param[in] pos The position on the edge the calibrator lies at
     * @param[in] outfile te file in which write results
     * @param[in] flowValues flow of calibrator
     * @return true if was sucesfully created, false in other case
     * @todo Is the position correct/needed
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the entry detector can not be added to the net (is duplicate)
     */
    static bool buildCalibrator(GNEViewNet* viewNet, const std::string& id, GNEEdge* edge, SUMOReal pos, const std::string& outfile, SUMOReal freq, const std::vector<GNECalibrator::GNECalibratorFlow*>& flowValues);

    /**@brief builds a rerouter
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the rerouter
     * @param[in] pos position of the rerouter in the map
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     * @param[in] file The file to read the reroute definitions from
     * @return true if was sucesfully created, false in other case
     */
    static bool buildRerouter(GNEViewNet* viewNet, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, SUMOReal prob, const std::string& file, bool off);

    /**@brief builds a Route probe
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the routeprobe
     * @param[in] edge The edges the routeprobe is placed at
     * @param[in] freq the aggregation period the values the routeprobe collects shall be summed up.
     * @param[in] file The file to read the routeprobe definitions from
     * @param[in] begin The time at which to start generating output
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the entry detector can not be added to the net (is duplicate)
     */
    static bool buildRouteProbe(GNEViewNet* viewNet, const std::string& id, GNEEdge* edge, SUMOReal freq, const std::string& file, SUMOReal begin);

    /**@brief Builds a VariableSpeedSign (lane speed trigger)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the lane speed trigger
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     * @param[in] VSSValues Step and speed values of variable speed signal
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the entry detector can not be added to the net (is duplicate)
     */
    static bool buildVariableSpeedSign(GNEViewNet* viewNet, const std::string& id, Position pos, const std::vector<GNELane*>& destLanes, const std::string& file, const std::map<SUMOReal, SUMOReal>& VSSValues);

    /**@brief Builds a vaporizer (lane speed trigger)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] id The id of the lane speed trigger
     * @param[in] edge edge in which tis vaporizer is placed
     * @param[in] startTime time in which this vaporizer start
     * @param[in] end time in which this vaporizer ends
     * @return true if was sucesfully created, false in other case
     * @exception ProcessError If the XML definition file is errornous
     */
    static bool buildVaporizer(GNEViewNet* viewNet, const std::string& id, GNEEdge* edge, SUMOReal startTime, SUMOReal end);

    /**@brief Helper method to obtain the filename
     * @param[in] attrs The attributes to obtain the file name from
     * @param[in] base The base path (the path the loaded additional file lies in)
     * @return The (expanded) path to the named file
     * @todo Recheck usage of the helper class
     */
    std::string getFileName(const SUMOSAXAttributes& attrs, const std::string& base, const bool allowEmpty = false);

    /**@brief extracts the position, checks whether it shall be mirrored and checks whether it is within the lane.
     * @param[in] attrs The attributes to obtain the position from
     * @param[in] lane The lane the position shall be valid for
     * @param[in] tt The trigger type (for user output)
     * @param[in] tid The trigger id (for user output)
     * @return The position on the lane
     * @exception InvalidArgument If the position is beyond the lane
     */
    SUMOReal getPosition(const SUMOSAXAttributes& attrs, GNELane& lane, const std::string& tt, const std::string& tid);

    /**@brief check if the position of an stoppingPlace over a lane is valid
    * @param[in] startPos Start position of stoppingPlace
    * @param[in] endPos End position of stoppingPlace
    * @param[in] laneLength Lenght of the lane
    * @param[in] minLength Min length of the stoppingPlace
    * @param[in] friendlyPos Attribute of stoppingPlace
    * @return true if the stoppingPlace position is valid, false in otherweise
    */
    bool checkStopPos(SUMOReal& startPos, SUMOReal& endPos, const SUMOReal laneLength, const SUMOReal minLength, const bool friendlyPos);

    /**@brief reset last Tag and clear non-valid Additionals (For example, created E3 withouts Entry/Exit childs)
     * @note must be called after a XML parsing
     */
    void resetLastTag();

protected:
    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief Pointer to the last inserted E3
    GNEDetectorE3* myE3Parent;

    /// @brief pointer to rerouterInterval in which insert GNEClosingReroute, GNEDestProbReroute, etc.
    GNERerouterInterval* rerouterIntervalToInsertValues;

    /// @brief last used Tag
    SumoXMLTag myLastTag;

    /// @brief used to check hierarchy of aditionals with parents and childs (for example, E3)
    bool checkAdditionalParent(SumoXMLTag currentTag);

private:
    /// @brief get parsed attribute of XML and show warnings if there are problems
    template<typename T>
    static T getParsedAttribute(const SUMOSAXAttributes& attrs, const char* objectid, SumoXMLTag tag, SumoXMLAttr attribute, bool& abort, bool report = true);

    /// @brief get special attribute friendly position, used in stopping places
    bool getFriendlyPosition(const SUMOSAXAttributes& attrs, const char* objectid);
};


#endif
