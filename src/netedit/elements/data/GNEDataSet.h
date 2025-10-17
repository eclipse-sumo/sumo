/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEHierarchicalElement.h>
#include <utils/common/Parameterised.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;
class GNEDataInterval;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDataSet : public GNEAttributeCarrier, public GNEHierarchicalElement {

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
     * @param[in] dataSetID data set ID
     * @param[in] filename file in which this dataSet is stored
     */
    GNEDataSet(const std::string& dataSetID, GNENet* net, const std::string& filename);

    /// @brief Destructor
    ~GNEDataSet();

    /// @brief methods to retrieve the elements linked to this dataSet
    /// @{

    /// @brief get GNEHierarchicalElement associated with this dataSet
    GNEHierarchicalElement* getHierarchicalElement() override;

    /// @brief get GNEMoveElement associated with this dataSet
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this dataSet
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this dataSet (constant)
    const Parameterised* getParameters() const override;

    /// @brief get GUIGlObject associated with this dataSet
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this dataSet (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @}

    /// @brief update attribute colors deprecated
    void updateAttributeColors();

    /// @brief all attribute colors
    const GNEDataSet::AttributeColors& getAllAttributeColors() const;

    /// @brief specific attribute colors
    const std::map<SumoXMLTag, GNEDataSet::AttributeColors>& getSpecificAttributeColors() const;

    /// @brief update pre-computed geometry information
    void updateGeometry() override;

    /// @brief Returns element position in view
    Position getPositionInView() const;

    /// @brief write data set
    void writeDataSet(OutputDevice& device) const;

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw from contour (green)
    bool checkDrawFromContour() const override;

    /// @brief check if draw from contour (magenta)
    bool checkDrawToContour() const override;

    /// @brief check if draw related contour (cyan)
    bool checkDrawRelatedContour() const override;

    /// @brief check if draw over contour (orange)
    bool checkDrawOverContour() const override;

    /// @brief check if draw delete contour (pink/white)
    bool checkDrawDeleteContour() const override;

    /// @brief check if draw delete contour small (pink/white)
    bool checkDrawDeleteContourSmall() const override;

    /// @brief check if draw select contour (blue)
    bool checkDrawSelectContour() const override;

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

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
    std::string getAttribute(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getAttributeDouble(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getAttributePosition(SumoXMLAttr key) const override;

    /* @brief method for getting the Attribute of an XML key in positionVector format
     * @param[in] key The attribute key
     * @return positionVector with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const override;

    /**@brief method for setting the attribute and letting the object perform data element changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) override;

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) override;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;
    /// @}

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
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief check if a new GNEDataInterval with the given begin and end can be inserted in current GNEDataSet
    static bool checkNewInterval(const std::map<const double, GNEDataInterval*>& dataIntervalMap, const double newBegin, const double newEnd);

    /// @brief Invalidated copy constructor.
    GNEDataSet(const GNEDataSet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDataSet& operator=(const GNEDataSet&) = delete;
};

/****************************************************************************/
