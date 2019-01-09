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
    /**@brief Builds a Vaporizer
     * @param[in] attrs SAX-attributes which define the vaporizer
     * @param[in] tag of the demand element
     * @note recheck throwing the exception
     */
    void parseAndBuildVaporizer(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag);

    /**@brief Parse generic parameter and insert it in the last created demand element
     * @param[in] attrs SAX-attributes which define the generic parameter
     */
    void parseGenericParameter(const SUMOSAXAttributes& attrs);

    /// @}

    /// @name building methods
    ///
    /// Called with parsed values, these methods build the trigger.
    /// @{
    /**@brief Build demand elements
     * @param[in] viewNet pointer to viewNet in wich demand element will be created
     * @param[in] allowUndoRedo enable or disable remove created demand element with ctrl + Z / ctrl + Y
     * @param[in] tag tag of the additiona lto create
     * @param[in] values map with the attributes and values of the demand element to create
     * @return true if was sucesfully created, false in other case
     */
    static GNEDemandElement* buildDemandElement(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values);

    /**@brief Builds a bus stop
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created demand element with ctrl + Z / ctrl + Y
     * @param[in] id The id of the bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] startPos Begin position of the bus stop on the lane
     * @param[in] endPos End position of the bus stop on the lane
     * @param[in] name Name of busStop
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] blockMovemet enable or disable block movement
     * @return true if was sucesfully created, false in other case
     * @exception InvalidArgument If the bus stop can not be added to the net (is duplicate)
     */
    static GNEDemandElement* buildBusStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const std::string& startPos, const std::string& endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition, bool blockMovement);

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
