/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
     * @param[in] tag of the demand element
     * @note recheck throwing the exception
     */
    void parseAndBuildRoute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parses and build a vehicle type
     * @param[in] attrs SAX-attributes which define the vehicle types
     * @param[in] tag of the demand element
     */
    void parseAndBuildVehicleType(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

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

    /// @}
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
