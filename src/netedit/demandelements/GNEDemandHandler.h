/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDemandHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Builds demand objects for netedit
/****************************************************************************/
#ifndef GNEDemandHandler_h
#define GNEDemandHandler_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/xml/SUMOSAXAttributes.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEEdge;
class GNEDemandElement;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNEDemandHandler
/// @brief Builds trigger objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNEDemandHandler : public SUMOSAXHandler {
public:
    /// @brief Constructor
    GNEDemandHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements = true, GNEDemandElement* demandElementParent = nullptr);

    /// @brief Destructor
    ~GNEDemandHandler();

    /// @name inherited from GenericSAXHandler
    /// @{
    /**@brief Called on the opening of a tag;
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element, const SUMOSAXAttributes& attrs);

    /** @brief Called when a closing tag occurs
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(int element);

    /// @}

    /// @name parsing methods
    ///
    /// These methods parse the attributes for each of the described trigger
    /// and call the according methods to build the trigger
    /// @{
    /**@brief Parse and build a Route
     * @param[in] attrs SAX-attributes which define the route
     */
    void parseAndBuildRoute(const SUMOSAXAttributes& attrs);

    /**@brief Parses and build a vehicle type
     * @param[in] attrs SAX-attributes which define the vehicle types
     */
    void parseAndBuildVehicleType(const SUMOSAXAttributes& attrs);

    /**@brief Parses and build a vehicle
     * @param[in] attrs SAX-attributes which define the vehicle types
     */
    void parseAndBuildVehicle(const SUMOSAXAttributes& attrs);

    /**@brief Parses and build a flow
     * @param[in] attrs SAX-attributes which define the vehicle types
     */
    void parseAndBuildFlow(const SUMOSAXAttributes& attrs);

    /**@brief Parse generic parameter and insert it in the last created demand element
     * @param[in] attrs SAX-attributes which define the generic parameter
     */
    void parseGenericParameter(const SUMOSAXAttributes& attrs);

    /// @}

    /// @name building methods
    ///
    /// Called with parsed values, these methods build the trigger.
    /// @{

    /**@brief Builds a Route
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created demand element with ctrl + Z / ctrl + Y
     * @param[in] routeID The id of the route (must be unique)
     * @param[in] edges route's edges
     * @param[in] edges route's color
     * @return pointer to element if was sucesfully created, nullptr in other case
     * @exception InvalidArgument If the bus stop can not be added to the net (is duplicate)
     */
    static GNEDemandElement* buildRoute(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color);

    /**
    DOCUMENTAR
    */
    static GNEDemandElement* buildVehicleType(GNEViewNet* viewNet, bool allowUndoRedo, std::string vehicleTypeID,
            double accel, double decel, double sigma, double tau, double length, double minGap, double maxSpeed,
            double speedFactor, double speedDev, const RGBColor& color, SUMOVehicleClass vClass, const std::string& emissionClass,
            SUMOVehicleShape shape, double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
            const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
            double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat);

    /**@brief builds a calibrator flow
    * @param[in] viewNet viewNet in which element will be inserted
    * @param[in] allowUndoRedo enable or disable remove created demand element with ctrl + Z / ctrl + Y
    * @param[in] id The name of the vehicle 
    * @param[in] type The id of the vehicle type to use for this vehicle. 
    * @param[in] route The id of the route the vehicle shall drive along 
    * @param[in] color This vehicle's color 
    * @param[in] depart The time step at which the vehicle shall enter the network; see #depart. Alternatively the vehicle departs once a person enters or a container is loaded
    * @param[in] departLane The lane on which the vehicle shall be inserted; see #departLane. default: "first"
    * @param[in] departPos The position at which the vehicle shall enter the net; see #departPos. default: "base"
    * @param[in] departSpeed The speed with which the vehicle shall enter the network; see #departSpeed. default: 0
    * @param[in] arrivalLane The lane at which the vehicle shall leave the network; see #arrivalLane. default: "current"
    * @param[in] arrivalPos The position at which the vehicle shall leave the network; see #arrivalPos. default: "max"
    * @param[in] arrivalSpeed The speed with which the vehicle shall leave the network; see #arrivalSpeed. default: "current"
    * @param[in] line A string specifying the id of a public transport line which can be used when specifying person rides
    * @param[in] personNumber The number of occupied seats when the vehicle is inserted. default: 0
    * @param[in] containerNumber The number of occupied container places when the vehicle is inserted. default: 0
    * @param[in] reroute List of intermediate edges that shall be passed on rerouting. 
    * @param[in] via List of intermediate edges that shall be passed on rerouting. 
    * @param[in] departPosLat The lateral position on the departure lane at which the vehicle shall enter the net; see Simulation/SublaneModel. default: "center"
    * @param[in] arrivalPosLat The lateral position on the arrival lane at which the vehicle shall arrive; see Simulation/SublaneModel. by default the vehicle does not care about lateral arrival position 
    * @return true if was sucesfully created, false in other case
    */
    static GNEDemandElement* buildVehicle(GNEViewNet* viewNet, bool allowUndoRedo, GNEDemandElement* route, GNEDemandElement* vType, const RGBColor& color, 
            double depart, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane, 
            const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, 
            bool reroute, const std::string& departPosLat, const std::string& arrivalPosLat);


    /**@brief builds a flow
    * @param[in] viewNet viewNet in which element will be inserted
    * @param[in] allowUndoRedo enable or disable remove created demand element with ctrl + Z / ctrl + Y
    * @param[in] id The name (unique) of flow 
    * @param[in] type The id of the vehicle's flow type to use for this vehicle's flow. 
    * @param[in] route The id of the route the vehicle's flow shall drive along 
    * @param[in] color This vehicle's flow's color 
    * @param[in] departLane The lane on which the vehicle's flow shall be inserted; see #departLane. default: "first"
    * @param[in] departPos The position at which the vehicle's flow shall enter the net; see #departPos. default: "base"
    * @param[in] departSpeed The speed with which the vehicle's flow shall enter the network; see #departSpeed. default: 0
    * @param[in] arrivalLane The lane at which the vehicle's flow shall leave the network; see #arrivalLane. default: "current"
    * @param[in] arrivalPos The position at which the vehicle's flow shall leave the network; see #arrivalPos. default: "max"
    * @param[in] arrivalSpeed The speed with which the vehicle's flow shall leave the network; see #arrivalSpeed. default: "current"
    * @param[in] line A string specifying the id of a public transport line which can be used when specifying person rides
    * @param[in] personNumber The number of occupied seats when the vehicle's flow is inserted. default: 0
    * @param[in] containerNumber The number of occupied container places when the vehicle's flow is inserted. default: 0
    * @param[in] reroute List of intermediate edges that shall be passed on rerouting. 
    * @param[in] via List of intermediate edges that shall be passed on rerouting. 
    * @param[in] departPosLat The lateral position on the departure lane at which the vehicle's flow shall enter the net; see Simulation/SublaneModel. default: "center"
    * @param[in] arrivalPosLat The lateral position on the arrival lane at which the vehicle's flow shall arrive; see Simulation/SublaneModel. by default the vehicle's flow does not care about lateral arrival position 
    * @param[in] begin first vehicle's flow departure time 
    * @param[in] end end of departure interval (if undefined, defaults to 24 hours) 
    * @param[in] vehsPerHour number of vehicles per hour, equally spaced (not together with period or probability) 
    * @param[in] period insert equally spaced vehicles at that period (not together with vehsPerHour or probability) 
    * @param[in] probability probability for emitting a vehicle's flow each second (not together with vehsPerHour or period)
    * @param[in] number total number of vehicles, equally spaced 
    * @return true if was sucesfully created, false in other case
    * @todo Is the position correct/needed
    * @return true if was sucesfully created, false in other case
    * @exception InvalidArgument If the entry detector can not be added to the net (is duplicate)
    */
    static GNEDemandElement* buildFlow(GNEViewNet* viewNet, bool allowUndoRedo, GNEDemandElement* route, GNEDemandElement* vType, const RGBColor& color,
            const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane, const std::string& arrivalPos, 
            const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, bool reroute, const std::string& departPosLat,
            const std::string& arrivalPosLat, double begin, double end, const std::string& vehsPerHour, const std::string& period, const std::string& probability, int number);

    /// @}
    /**@brief builds a calibrator flow
    * @param[in] viewNet viewNet in which element will be inserted
    * @param[in] allowUndoRedo enable or disable remove created additional with ctrl + Z / ctrl + Y

    * @param[in] id The name of the vehicle 
    * @param[in] type The id of the vehicle type to use for this vehicle. 
    * @param[in] route The id of the route the vehicle shall drive along 
    * @param[in] color This vehicle's color 
    * @param[in] depart The time step at which the vehicle shall enter the network; see #depart. Alternatively the vehicle departs once a person enters or a container is loaded
    * @param[in] departLane The lane on which the vehicle shall be inserted; see #departLane. default: "first"
    * @param[in] departPos The position at which the vehicle shall enter the net; see #departPos. default: "base"
    * @param[in] departSpeed The speed with which the vehicle shall enter the network; see #departSpeed. default: 0
    * @param[in] arrivalLane The lane at which the vehicle shall leave the network; see #arrivalLane. default: "current"
    * @param[in] arrivalPos The position at which the vehicle shall leave the network; see #arrivalPos. default: "max"
    * @param[in] arrivalSpeed The speed with which the vehicle shall leave the network; see #arrivalSpeed. default: "current"
    * @param[in] line A string specifying the id of a public transport line which can be used when specifying person rides
    * @param[in] personNumber The number of occupied seats when the vehicle is inserted. default: 0
    * @param[in] containerNumber The number of occupied container places when the vehicle is inserted. default: 0
    * @param[in] reroute List of intermediate edges that shall be passed on rerouting. 
    * @param[in] via List of intermediate edges that shall be passed on rerouting. 
    * @param[in] departPosLat The lateral position on the departure lane at which the vehicle shall enter the net; see Simulation/SublaneModel. default: "center"
    * @param[in] arrivalPosLat The lateral position on the arrival lane at which the vehicle shall arrive; see Simulation/SublaneModel. by default the vehicle does not care about lateral arrival position 

    * @param[in] begin first vehicle departure time 
    * @param[in] end end of departure interval (if undefined, defaults to 24 hours) 
    * @param[in] vehsPerHour number of vehicles per hour, equally spaced (not together with period or probability) 
    * @param[in] period insert equally spaced vehicles at that period (not together with vehsPerHour or probability) 
    * @param[in] probability probability for emitting a vehicle each second (not together with vehsPerHour or period), see also Simulation/Randomness
    * @param[in] number total number of vehicles, equally spaced 

    * @return true if was sucesfully created, false in other case
    * @todo Is the position correct/needed
    * @return true if was sucesfully created, false in other case
    * @exception InvalidArgument If the entry detector can not be added to the net (is duplicate)
    */


private:
    /// @brief Stack used to save the last inserted element
    struct HierarchyInsertedDemandElements {

        /// @brief insert new element (called only in function myStartElement)
        void insertElement(SumoXMLTag tag);

        /// @brief commit element insertion (used to save last correct created element)
        void commitElementInsertion(GNEDemandElement* demandElementCreated);

        /// @brief pop last inserted element (used only in function myEndElement)
        void popElement();

        /// @brief retrieve demand element parent correspond to current status of myInsertedElements
        GNEDemandElement* retrieveDemandElementParent(GNEViewNet* viewNet, SumoXMLTag expectedTag) const;

        /// @brief return last demand element inserted
        GNEDemandElement* getLastInsertedDemandElement() const;

    private:
        /// @brief vector used as stack
        std::vector<std::pair<SumoXMLTag, GNEDemandElement*> > myInsertedElements;
    };

    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief flag to check if created demand elements must be undo and redo
    bool myUndoDemandElements;

    /// @brief pointer to parent demand element (used for loading demand element childs placed in a different XML)
    GNEDemandElement* myDemandElementParent;

    /// @brief HierarchyInsertedDemandElements used for insert childs
    HierarchyInsertedDemandElements myHierarchyInsertedDemandElements;
};


#endif
