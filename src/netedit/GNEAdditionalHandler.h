/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// Builds trigger objects for netedit
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
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>


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
class GNECalibrator;
class GNEVariableSpeedSign;
class GNERerouterInterval;
class GNERerouter;
class GNECalibratorRoute;
class GNECalibratorVehicleType;


// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNEAdditionalHandler
/// @brief Builds trigger objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNEAdditionalHandler : public SUMOSAXHandler {
public:
    /// @brief Constructor
    GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet, bool undoAdditionals = true);

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

    /**@brief Parses his values and builds a Variable Speed Signal Step
    * @param[in] attrs SAX-attributes which define the trigger
    * @param[in] tag of the additional
    * @see buildLaneSpeedTrigger
    */
    void parseAndBuildVariableSpeedSignStep(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a rerouter
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] tag of the additional
     */
    void parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Rerouter Interval
    * @param[in] attrs SAX-attributes which define the trigger
    * @param[in] tag of the additional
    */
    void parseAndBuildRerouterInterval(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Closing Lane reroute
    * @param[in] attrs SAX-attributes which define the trigger
    * @param[in] tag of the additional
    */
    void parseAndBuildRerouterClosingLaneReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Closing Reroute
    * @param[in] attrs SAX-attributes which define the trigger
    * @param[in] tag of the additional
    */
    void parseAndBuildRerouterClosingReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Destiny Prob Reroute
    * @param[in] attrs SAX-attributes which define the trigger
    * @param[in] tag of the additional
    */
    void parseAndBuildRerouterDestProbReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses his values and builds a Route Prob Reroute
    * @param[in] attrs SAX-attributes which define the trigger
    * @param[in] tag of the additional
    */
    void parseAndBuildRerouterRouteProbReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

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

    /**@brief Parses route values of Calibrators
     * @param[in] attrs SAX-attributes which define the routes
     * @param[in] tag of the additional
     */
    void parseAndBuildCalibratorRoute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses vehicle type values of Calibrators
     * @param[in] attrs SAX-attributes which define the vehicle types
     * @param[in] tag of the additional
     */
    void parseAndBuildCalibratorVehicleType(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses flow values of Calibrators
     * @param[in] attrs SAX-attributes which define the flows
     * @param[in] tag of the additional
     */
    void parseAndBuildCalibratorFlow(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /// @}

    /// @name building methods
    ///
    /// Called with parsed values, these methods build the trigger.
    /// @{
    /**@brief Build additionals
     * @param[in] viewNet pointer to viewNet in wich additional will be created
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] tag tag of the additiona lto create
     * @param[in] values map with the attributes and values of the additional to create
     * @return true if was sucesfully created, false in other case
     */
    static bool buildAdditional(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values);

    /**@brief Builds a bus stop
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] startPos Begin position of the bus stop on the lane
     * @param[in] endPos End position of the bus stop on the lane
     * @param[in] name Name of busStop
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @param[in] friendlyPos enable or disable friendly position
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the bus stop can not be added to the net (is duplicate)
     */
    static bool buildBusStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double startPos, double endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition);

    /**@brief Builds a container stop
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the container stop
     * @param[in] lane The lane the container stop is placed on
     * @param[in] startPos Begin position of the container stop on the lane
     * @param[in] endPos End position of the container stop on the lane
     * @param[in] name Name of container stop
     * @param[in] lines Names of the bus lines that halt on this container stop
     * @param[in] friendlyPos enable or disable friendly position
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the container stop can not be added to the net (is duplicate)
     */
    static bool buildContainerStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double startPos, double endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition);

    /**@brief Builds a charging Station
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the charging Station
     * @param[in] lane The lane the charging Station is placed on
     * @param[in] startPos Begin position of the charging Station on the lane
     * @param[in] endPos End position of the charging Station on the lane
     * @param[in] name Name of charging station
     * @param[in] chargingPower power charged in every timeStep
     * @param[in] efficiency efficiency of the charge
     * @param[in] chargeInTransit enable or disable charge in transit
     * @param[in] chargeDelay delay in the charge
     * @param[in] friendlyPos enable or disable friendly position
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the charging Station can not be added to the net (is duplicate)
     */
    static bool buildChargingStation(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double startPos, double endPos, const std::string& name, double chargingPower, double efficiency, bool chargeInTransit, double chargeDelay, bool friendlyPosition);

    /**@brief Builds a induction loop detector (E1)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the detector
     * @param[in] lane The lane the detector is placed on
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] splitByType If set, the collected values will be additionally reported on per-vehicle type base.
     * @param[in] friendlyPos enable or disable friendly position
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    static bool buildDetectorE1(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, double freq, const std::string& filename, const std::string& vehicleTypes, bool friendlyPos);

    /**@brief Builds a lane Area Detector (E2)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
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
     * @param[in] friendlyPos enable or disable friendly position
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    static bool buildDetectorE2(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, double length, double freq, const std::string& filename,
                                bool cont, const double timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos);

    /**@brief Builds a multi entry exit detector (E3)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the detector
     * @param[in] pos position of the detector in the map
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    static bool buildDetectorE3(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, double freq, const std::string& filename, const double timeThreshold, double speedThreshold);

    /**@brief Builds a entry detector (E3)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] E3Parent pointer to E3 detector parent
     * @param[in] lane The lane in which the entry detector is placed on
     * @param[in] pos position of the entry detector on the lane
     * @param[in] friendlyPos enable or disable friendly position
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the entry detector can not be added to the net (invalid parent or lane)
     */
    static bool buildDetectorEntry(GNEViewNet* viewNet, bool allowUndoRedo, GNEDetectorE3* E3Parent, GNELane* lane, double pos, bool friendlyPos);

    /**@brief Builds a exit detector (E3)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] E3Parent pointer to E3 detector parent
     * @param[in] lane The lane in which the exit detector is placed on
     * @param[in] pos position of the exit detector on the lane
     * @param[in] friendlyPos enable or disable friendly position
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the exit detector can not be added to the net (invalid parent or lane
     */
    static bool buildDetectorExit(GNEViewNet* viewNet, bool allowUndoRedo, GNEDetectorE3* E3Parent, GNELane* lane, double pos, bool friendlyPos);

    /**@brief builds a microscopic calibrator over a lane
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the calibrator
     * @param[in] lane The lane the calibrator is placed at
     * @param[in] pos The position on the edge the calibrator lies at
     * @param[in] outfile te file in which write results
     * @return true if was sucesfully created, false in other case
     * @todo Is the position correct/needed
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the entry detector can not be added to the net (is duplicate)
     */
    static bool buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, const std::string& outfile, double freq);

    /**@brief builds a microscopic calibrator over an edge
    * @param[in] viewNet viewNet in which element will be inserted
    * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
    * @param[in] id The id of the calibrator
    * @param[in] edge The edge the calibrator is placed at
    * @param[in] pos The position on the edge the calibrator lies at
    * @param[in] outfile te file in which write results
    * @return true if was sucesfully created, false in other case
    * @todo Is the position correct/needed
    * @return true if was sucesfully created, false in other case
    * @exception InvalidArgument If the entry detector can not be added to the net (is duplicate)
    */
    static bool buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, double pos, const std::string& outfile, double freq);

    /**
    DOCUMENTAR
    */
    static bool buildCalibratorRoute(GNEViewNet* viewNet, bool allowUndoRedo, GNECalibrator* calibratorParent, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color);

    /**
    DOCUMENTAR
    */
    static bool buildCalibratorVehicleType(GNEViewNet* viewNet, bool allowUndoRedo, GNECalibrator* calibratorParent, std::string vehicleTypeID,
                                           double accel, double decel, double sigma, double tau, double length, double minGap, double maxSpeed,
                                           double speedFactor, double speedDev, const RGBColor& color, SUMOVehicleClass vClass, const std::string& emissionClass,
                                           SUMOVehicleShape shape, double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
                                           const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
                                           double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat);

    /**
    DOCUMENTAR
    */
    static bool buildCalibratorFlow(GNEViewNet* viewNet, bool allowUndoRedo, GNECalibrator* calibratorParent, const std::string& flowID, GNECalibratorRoute* route,
                                    GNECalibratorVehicleType* vtype, const RGBColor& color, const std::string& departLane, const std::string& departPos,
                                    const std::string& departSpeed, const std::string& arrivalLane, const std::string& arrivalPos, const std::string& arrivalSpeed,
                                    const std::string& line, int personNumber, int containerNumber, bool reroute, const std::string& departPosLat,
                                    const std::string& arrivalPosLat, double begin, double end, double vehsPerHour, double period, double probability, int number, int flowType);

    /**@brief builds a rerouter
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the rerouter
     * @param[in] pos position of the rerouter in the map
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     * @param[in] file The file to read the reroute definitions from
     * @return true if was sucesfully created, false in other case
     */
    static bool buildRerouter(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, double prob, const std::string& file, bool off, double timeThreshold);

    /**@brief builds a rerouter interval
    * @param[in] viewNet viewNet in which element will be inserted
    * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
    * @param[in] rerouterParent rerouter in which interval is placed
    * @param[in] begin begin of interval
    * @param[in] end end of interval
    * @return true if was sucesfully created, false in other case
    */
    static bool buildRerouterInterval(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouter* rerouterParent, double begin, double end);

    /**
    DOCUMENTAR
    */
    static bool buildClosingLaneReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, GNELane* closedLane, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles);

    /**
    DOCUMENTAR
    */
    static bool buildClosingReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions allowedVehicles, SVCPermissions disallowedVehicles);

    /**
    DOCUMENTAR
    */
    static bool builDestProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability);

    /**
    DOCUMENTAR
    */
    static bool buildRouteProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNERerouterInterval* rerouterIntervalParent, const std::string& newRouteId, double probability);

    /**@brief builds a Route probe
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the routeprobe
     * @param[in] edge The edges the routeprobe is placed at
     * @param[in] freq the aggregation period the values the routeprobe collects shall be summed up.
     * @param[in] file The file to read the routeprobe definitions from
     * @param[in] begin The time at which to start generating output
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the Route Probe can not be added to the net (is duplicate)
     */
    static bool buildRouteProbe(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, double freq, const std::string& file, double begin);

    /**@brief Builds a VariableSpeedSign (lane speed trigger)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] id The id of the lane speed trigger
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the VariableSpeedSign can not be added to the net (is duplicate)
     */
    static bool buildVariableSpeedSign(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNELane*>& destLanes, const std::string& file);

    /**@brief Builds a VariableSpeedSign Step
    * @param[in] viewNet viewNet in which element will be inserted
    * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
    * @param[in] VSSParent Variable Speed Sign parent
    * @param[in] time step's time
    * @param[in] speed step's speed
    * @return true if was sucesfully created, false in other case
    * @exception InvalidArgument If the Variable Speed Sign Step can not be added to the net (is duplicate)
    */
    static bool buildVariableSpeedSignStep(GNEViewNet* viewNet, bool allowUndoRedo, GNEVariableSpeedSign* VSSParent, double time, double speed);

    /**@brief Builds a vaporizer (lane speed trigger)
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y
     * @param[in] edge edge in which tis vaporizer is placed
     * @param[in] startTime time in which this vaporizer start
     * @param[in] end time in which this vaporizer ends
     * @return true if was sucesfully created, false in other case
     * @exception ProcessError If the XML definition file is errornous
     */
    static bool buildVaporizer(GNEViewNet* viewNet, bool allowUndoRedo, GNEEdge* edge, double startTime, double end);

    /**@brief Helper method to obtain the filename
     * @param[in] attrs The attributes to obtain the file name from
     * @param[in] base The base path (the path the loaded additional file lies in)
     * @return The (expanded) path to the named file
     * @todo Recheck usage of the helper class
     */
    std::string getFileName(const SUMOSAXAttributes& attrs, const std::string& base, const bool allowEmpty = false);

    /**@brief extracts the position, checks whether it shall be mirrored and checks whether it is within the lane.
     * @param[in] pos position of additional over lane
     * @param[in] lane The lane the position shall be valid for
     * @param[in] friendlyPos flag to indicate if friendlyPos is enabled
     * @param[in] additionalID ID of additional
     * @return The position on the lane
     */
    double getPosition(double pos, GNELane& lane, bool friendlyPos, const std::string& additionalID);

    /**@brief check if the position of an stoppingPlace over a lane is valid
    * @param[in] startPos Start position of stoppingPlace
    * @param[in] endPos End position of stoppingPlace
    * @param[in] laneLength Length of the lane
    * @param[in] minLength Min length of the stoppingPlace
    * @param[in] friendlyPos Attribute of stoppingPlace
    * @return true if the stoppingPlace position is valid, false in otherweise
    */
    static bool fixStoppinPlacePosition(double& startPos, double& endPos, const double laneLength, const double minLength, const bool friendlyPos);

    /**@brief check if the position of a detector over a lane is valid
    * @param[in] pos pos position of detector
    * @param[in] laneLength Length of the lane
    * @param[in] friendlyPos Attribute of detector
    * @return true if the detector position is valid, false in otherweise
    */
    static bool checkAndFixDetectorPositionPosition(double& pos, const double laneLength, const bool friendlyPos);

    /**@brief check if the position of a detector over a lane is valid
    * @param[in] startPos Start position of detector
    * @param[in] length length of detector
    * @param[in] laneLength Length of the lane
    * @param[in] friendlyPos Attribute of detector
    * @return true if the detector position is valid, false in otherweise
    */
    static bool fixE2DetectorPositionPosition(double& pos, double& length, const double laneLength, const bool friendlyPos);

protected:
    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief flag to check if created additionals must be undo and redo
    bool myUndoAdditionals;

    /// @brief ID of last inserted Additional parent (needed for additionasl that own a child)
    std::string myLastInsertedAdditionalParent;

private:
    /// @brief get a error message, if configuration of flow distribution is invalid
    int getTypeOfFlowDistribution(std::string flowID, double vehsPerHour, double period, double probability);
};


#endif
