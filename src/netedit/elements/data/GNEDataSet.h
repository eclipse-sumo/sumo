/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNEDataSet.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data elements
/****************************************************************************/
#pragma once


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEDataInterval;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDataSet : public GNEAttributeCarrier {

public:
    /**@brief Constructor
     * @param[in] viewNet pointer to GNEViewNet of this data element element belongs
     */
    GNEDataSet(GNEViewNet* viewNet, const std::string dataSetID);

    /// @brief Destructor
    ~GNEDataSet();

    /// @brief get ID
    const std::string& getID() const;

    /// @brief set new ID in dataSet
    void setDataSetID(const std::string& newID);

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief update dotted contour
    void updateDottedContour();

    /// @brief Returns element position in view
    Position getPositionInView() const;

    /// @brief Returns a pointer to GNEViewNet in which data element element is located
    GNEViewNet* getViewNet() const;

    /// @brief write data set
    void writeDataSet(OutputDevice& device) const;

    /// @name data interval children
    /// @{

    /// @brief add data interval child
    void addDataIntervalChild(GNEDataInterval* dataInterval);

    /// @brief add data interval child
    void removeDataIntervalChild(GNEDataInterval* dataInterval);

    /// @brief check if given data interval exist
    bool dataIntervalChildrenExist(GNEDataInterval* dataInterval) const;

    /// @brief update data interval begin
    void updateDataIntervalBegin(const double oldBegin);

    /// @brief check if a new GNEDataInterval with the given begin and end can be inserted in current GNEDataSet
    bool checkNewInterval(const double newBegin, const double newEnd);

    /// @brief check if new begin or end for given GNEDataInterval is given
    bool checkNewBeginEnd(const GNEDataInterval* dataInterval, const double newBegin, const double newEnd);

    /// @brief return interval
    GNEDataInterval* retrieveInterval(const double begin, const double end) const;

    /// @brief get data interval children
    const std::map<const double, GNEDataInterval*>& getDataIntervalChildren() const;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /// @brief select attribute carrier using GUIGlobalSelection
    void selectAttributeCarrier(bool changeFlag = true);

    /// @brief unselect attribute carrier using GUIGlobalSelection
    void unselectAttributeCarrier(bool changeFlag = true);

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

    /// @brief check if attribute carrier must be drawn using selecting color.
    bool drawUsingSelectColor() const;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const;

    /**@brief method for setting the attribute and letting the object perform data element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for enable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void enableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for disable attribute
     * @param[in] key The attribute key
     * @param[in] undoList The undoList on which to register changes
     * @note certain attributes can be only enabled, and can produce the disabling of other attributes
     */
    void disableAttribute(SumoXMLAttr key, GNEUndoList* undoList);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

protected:
    /// @brief The GNEViewNet this data element element belongs
    GNEViewNet* myViewNet;

    /// @brief dataSet ID
    std::string myDataSetID;

    /// @brief map with dataIntervals children sorted by begin
    std::map<const double, GNEDataInterval*> myDataIntervalChildren;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief method for enabling the attribute and nothing else (used in GNEChange_EnableAttribute)
    void setEnabledAttribute(const int enabledAttributes);

    /// @brief check if a new GNEDataInterval with the given begin and end can be inserted in current GNEDataSet
    static bool checkNewInterval(const std::map<const double, GNEDataInterval*>& dataIntervalMap, const double newBegin, const double newEnd);

    /// @brief Invalidated copy constructor.
    GNEDataSet(const GNEDataSet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDataSet& operator=(const GNEDataSet&) = delete;
};

/****************************************************************************/

