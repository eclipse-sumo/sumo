/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/common/Parameterised.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;
class GNEDataInterval;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDataSet : public GNEAttributeCarrier, public Parameterised {

public:
    /// @brief attribute colors
    class AttributeColors {

    public:
        /// @brief default constructor
        AttributeColors();

        /// @brief update value for an specific attribute
        void updateValues(const std::string& attribute, const double value);

        /// @brief update value for all attributes
        void updateAllValues(const AttributeColors& attributeColors);

        /// @brief check if given attribute exist (needed for non-double attributes)
        bool exist(const std::string& attribute) const;

        /// @brief get minimum value
        double getMinValue(const std::string& attribute) const;

        /// @brief get maximum value
        double getMaxValue(const std::string& attribute) const;

        /// @brief clear AttributeColors
        void clear();

    private:
        /// @brief map with the minimum and maximum value
        std::map<std::string, std::pair<double, double> > myMinMaxValue;

        /// @brief Invalidated assignment operator.
        AttributeColors& operator=(const AttributeColors&) = delete;
    };

    /**@brief Constructor
     * @param[in] net pointer to GNEViewNet of this data element element belongs
     */
    GNEDataSet(GNENet* net, const std::string dataSetID);

    /// @brief Destructor
    ~GNEDataSet();

    /// @brief get GNEHierarchicalElement associated with this AttributeCarrier
    GNEHierarchicalElement* getHierarchicalElement();

    /// @brief get GUIGlObject associated with this AttributeCarrier
    GUIGlObject* getGUIGlObject();

    /// @brief get GUIGlObject associated with this AttributeCarrier (constant)
    const GUIGlObject* getGUIGlObject() const;

    /// @brief update attribute colors deprecated
    void updateAttributeColors();

    /// @brief all attribute colors
    const GNEDataSet::AttributeColors& getAllAttributeColors() const;

    /// @brief specific attribute colors
    const std::map<SumoXMLTag, GNEDataSet::AttributeColors>& getSpecificAttributeColors() const;

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns element position in view
    Position getPositionInView() const;

    /// @brief write data set
    void writeDataSet(OutputDevice& device) const;

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw from contour (green)
    bool checkDrawFromContour() const;

    /// @brief check if draw from contour (magenta)
    bool checkDrawToContour() const;

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const;

    /// @brief check if draw over contour (orange)
    bool checkDrawOverContour() const;

    /// @brief check if draw delete contour (pink/white)
    bool checkDrawDeleteContour() const;

    /// @brief check if draw select contour (blue)
    bool checkDrawSelectContour() const;

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const;

    /// @}

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
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;
    /// @}

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

protected:
    /// @brief dataSet ID
    std::string myDataSetID;

    /// @brief map with dataIntervals children sorted by begin
    std::map<const double, GNEDataInterval*> myDataIntervalChildren;

    /// @brief all attribute colors
    GNEDataSet::AttributeColors myAllAttributeColors;

    /// @brief specific attribute colors
    std::map<SumoXMLTag, GNEDataSet::AttributeColors> mySpecificAttributeColors;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief check if a new GNEDataInterval with the given begin and end can be inserted in current GNEDataSet
    static bool checkNewInterval(const std::map<const double, GNEDataInterval*>& dataIntervalMap, const double newBegin, const double newEnd);

    /// @brief Invalidated copy constructor.
    GNEDataSet(const GNEDataSet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDataSet& operator=(const GNEDataSet&) = delete;
};

/****************************************************************************/
