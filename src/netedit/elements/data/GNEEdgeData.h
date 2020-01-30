/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEEdgeData.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for edge data
/****************************************************************************/
#ifndef GNEEdgeData_h
#define GNEEdgeData_h


// ===========================================================================
// included modules
// ===========================================================================

#include "GNEGenericData.h"

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEEdgeData
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEEdgeData : public GNEGenericData {

public:
    /**@brief Constructor
     * @param[in] dataIntervalParent pointer to data interval parent
     * @param[in] edgeParent poinetr to Edge parent
     */
    GNEEdgeData(GNEDataInterval* dataIntervalParent, GNEEdge *edgeParent);

    /// @brief Destructor
    ~GNEEdgeData();

    /// @name members and functions relative to write data sets into XML
    /// @{
    /**@brief writte data set element into a xml file
     * @param[in] device device in which write parameters of data set element
     */
    void writeEdgeData(OutputDevice& device) const;

    /// @brief check if current data set is valid to be writed into XML (by default true, can be reimplemented in children)
    bool isEdgeDataValid() const;

    /// @brief return a string with the current data set problem (by default empty, can be reimplemented in children)
    std::string getEdgeDataProblem() const;

    /// @brief fix data set problem (by default throw an exception, has to be reimplemented in children)
    void fixEdgeDataProblem();
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

    /**@brief method for setting the attribute and letting the object perform data set changes
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

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEEdgeData(const GNEEdgeData&) = delete;

    /// @brief Invalidated assignment operator.
    GNEEdgeData& operator=(const GNEEdgeData&) = delete;
};


#endif

/****************************************************************************/

