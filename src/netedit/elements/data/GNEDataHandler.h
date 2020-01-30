/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Builds data objects for netedit
/****************************************************************************/
#ifndef GNEDataHandler_h
#define GNEDataHandler_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/geom/Position.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEEdge;
class GNEEdgeData;
class GNEGenericData;
class GNEDataElement;
class GNEDataInterval;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNEDataHandler
/// @brief Builds data objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNEDataHandler : public SUMOSAXHandler {
public:

    /// @brief Stack used to save the last inserted element
    struct HierarchyInsertedDatas {

        /// @brief insert new element (called only in function myStartElement)
        void insertElement(SumoXMLTag tag);

        /// @brief commit element insertion (used to save last correct created element)
        void commitElementInsertion(GNEGenericData* additionalCreated);

        /// @brief pop last inserted element (used only in function myEndElement)
        void popElement();

        /// @brief retrieve parent generic data correspond to current status of myInsertedElements
        GNEGenericData* retrieveParentGenericData(GNEViewNet* viewNet, SumoXMLTag expectedTag) const;

        /// @brief return last generic data inserted
        GNEGenericData* getLastInsertedGenericData() const;

    private:
        /// @brief vector used as stack
        std::vector<std::pair<SumoXMLTag, GNEGenericData*> > myInsertedElements;
    };

    /// @brief Constructor
    GNEDataHandler(const std::string& file, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEDataHandler();

    /// @name inherited from ShapeHandler
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

    /// @name building methods
    ///
    /// Called with parsed values, these methods build the data.
    /// @{
    /**@brief Build datas
     * @param[in] viewNet pointer to viewNet in wich data will be created
     * @param[in] allowUndoRedo enable or disable remove created data with ctrl + Z / ctrl + Y
     * @param[in] tag tag of the data to create
     * @param[in] attrs SUMOSAXAttributes with attributes
     * @param[in] HierarchyInsertedDatas pointer to HierarchyInsertedDatas (can be null)
     * @return true if was sucesfully created, false in other case
     */
    static bool buildData(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, const SUMOSAXAttributes& attrs, HierarchyInsertedDatas* insertedDatas);

    /**@brief Builds a edgeData
     * @param[in] viewNet viewNet in which element will be inserted
     * @param[in] allowUndoRedo enable or disable remove created data with ctrl + Z / ctrl + Y
     * @param[in] dataInterval GNEDataInterval in which this edgeData is saved
     * @param[in] edge GNEEdge parent
     * @exception InvalidArgument If the edgeData can not be added to the net
     */
    static GNEEdgeData* buildEdgeData(GNEViewNet* viewNet, bool allowUndoRedo, GNEDataInterval *dataInterval, GNEEdge* edge);

protected:
    /// @name parsing methods
    ///
    /// These methods parse the attributes for each of the described data
    /// and call the according methods to build the data
    /// @{
    /**@brief Parses his values and builds a bus stop
     * @param[in] attrs SAX-attributes which define the data
     */
    static bool parseAndBuildEdgeData(GNEViewNet* viewNet, bool allowUndoRedo, const SUMOSAXAttributes& attrs, HierarchyInsertedDatas* insertedDatas);

    /// @}

private:
    /**@brief Parse parameter and insert it in the last created data
     * @param[in] attrs SAX-attributes which define the parameter
     */
    void parseParameter(const SUMOSAXAttributes& attrs);

    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief HierarchyInsertedDatas used for insert children
    HierarchyInsertedDatas myHierarchyInsertedGenericDatas;

    /// @brief invalidate copy constructor
    GNEDataHandler(const GNEDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNEDataHandler& operator=(const GNEDataHandler& s) = delete;
};


#endif
