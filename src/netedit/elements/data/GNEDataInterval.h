/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDataInterval.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data elements
/****************************************************************************/
#ifndef GNEDataInterval_h
#define GNEDataInterval_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEDataSet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDataInterval
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */
class GNEDataInterval : public GNEAttributeCarrier {

public:
    /**@brief Constructor
     * @param[in] dataSetParent GNEDataSet to which this data interval belongs.
     * @param[in] begin interval begin
     * @param[in] end interval end
     */
    GNEDataInterval(GNEDataSet *dataSetParent, const double begin, const double end);

    /// @brief Destructor
    ~GNEDataInterval();

    /// @name members and functions relative to write data elements into XML
    /// @{
    /**@brief writte data element element into a xml file
     * @param[in] device device in which write parameters of data element element
     */
    void writeDataInterval(OutputDevice& device) const;

    /// @brief check if current data element is valid to be writed into XML (by default true, can be reimplemented in children)
    bool isDataIntervalValid() const;

    /// @brief return a string with the current data element problem (by default empty, can be reimplemented in children)
    std::string getDataIntervalProblem() const;

    /// @brief fix data element problem (by default throw an exception, has to be reimplemented in children)
    void fixDataIntervalProblem();
    /// @}

    /// @brief Returns a pointer to GNEDataSet parent
    GNEDataSet* getDataSetParent() const;

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
    /// @brief GNEDataSet parent to which this data interval belongs.
    GNEDataSet *myDataSetParent;

    /// @brief begin interval
    double myBegin;

    /// @brief end interval
    double myEnd;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEDataInterval(const GNEDataInterval&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDataInterval& operator=(const GNEDataInterval&) = delete;
};


#endif

/****************************************************************************/

