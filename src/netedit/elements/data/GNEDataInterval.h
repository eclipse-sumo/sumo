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
/// @file    GNEDataInterval.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDataSet.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEGenericData;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDataInterval : public GNEAttributeCarrier, public GNEHierarchicalElement {

public:
    /**@brief Constructor
     * @param[in] dataSetParent GNEDataSet to which this data interval belongs.
     * @param[in] begin interval begin
     * @param[in] end interval end
     */
    GNEDataInterval(GNEDataSet* dataSetParent, const double begin, const double end);

    /// @brief Destructor
    ~GNEDataInterval();

    /// @brief methods to retrieve the elements linked to this dataInterval
    /// @{

    /// @brief get GNEHierarchicalElement associated with this dataInterval
    GNEHierarchicalElement* getHierarchicalElement() override;

    /// @brief get GNEMoveElement associated with this dataInterval
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this dataInterval
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this dataInterval (const)
    const Parameterised* getParameters() const override;

    /// @brief get GUIGlObject associated with this dataInterval
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this dataInterval (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @}

    /// @brief update generic data child IDs
    void updateGenericDataIDs();

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

    /// @name members and functions relative to write data elements into XML
    /// @{
    /// @brief check if current data element is valid to be written into XML (by default true, can be reimplemented in children)
    bool isDataIntervalValid() const;

    /// @brief return a string with the current data element problem (by default empty, can be reimplemented in children)
    std::string getDataIntervalProblem() const;

    /// @brief fix data element problem (by default throw an exception, has to be reimplemented in children)
    void fixDataIntervalProblem();
    /// @}

    /// @brief Returns a pointer to GNEDataSet parent
    GNEDataSet* getDataSetParent() const;

    /// @name generic data children
    /// @{

    /// @brief add generic data child
    void addGenericDataChild(GNEGenericData* genericData);

    /// @brief add generic data child
    void removeGenericDataChild(GNEGenericData* genericData);

    /// @brief check if given generic data is child of this data interval
    bool hasGenericDataChild(GNEGenericData* genericData) const;

    /// @brief get generic data children
    const std::vector<GNEGenericData*>& getGenericDataChildren() const;

    /// @brief check if there is already a edgeRel single defined in the given edge
    bool edgeRelSingleExists(const GNEEdge* edge) const;

    /// @brief check if there is already a edgeRel defined between two edges
    bool edgeRelExists(const GNEEdge* fromEdge, const GNEEdge* toEdge) const;

    /// @brief check if there is already a TAZRel defined in one TAZ
    bool TAZRelExists(const GNEAdditional* TAZ) const;

    /// @brief check if there is already a TAZRel defined between two TAZs
    bool TAZRelExists(const GNEAdditional* fromTAZ, const GNEAdditional* toTAZ) const;

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

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;
    /// @}

protected:
    /// @brief all attribute colors
    GNEDataSet::AttributeColors myAllAttributeColors;

    /// @brief specific attribute colors
    std::map<SumoXMLTag, GNEDataSet::AttributeColors> mySpecificAttributeColors;

    /// @brief GNEDataSet parent to which this data interval belongs.
    GNEDataSet* myDataSetParent;

    /// @brief begin interval
    double myBegin;

    /// @brief end interval
    double myEnd;

    /// @brief vector with generic data children
    std::vector<GNEGenericData*> myGenericDataChildren;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief Invalidated copy constructor.
    GNEDataInterval(const GNEDataInterval&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDataInterval& operator=(const GNEDataInterval&) = delete;
};

/****************************************************************************/
