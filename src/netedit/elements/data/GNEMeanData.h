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
/// @file    GNEMeanData.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// Class for representing MeanData
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/elements/GNEHierarchicalElement.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMeanData : public GNEAttributeCarrier, public GNEHierarchicalElement {

public:
    /// @brief Default constructor
    GNEMeanData(SumoXMLTag tag, std::string ID, GNENet* net, const std::string& filename);

    /// @brief Parameter constructor
    GNEMeanData(SumoXMLTag tag, std::string ID, GNENet* net, const std::string& filename, const std::string& file,
                const std::string& type, const SUMOTime period, const SUMOTime begin, const SUMOTime end,
                const bool trackVehicles, const std::vector<SumoXMLAttr>& writtenAttributes, const bool aggregate,
                const std::vector<std::string>& edges, const std::string& edgeFile, const std::string& excludeEmpty,
                const bool withInternal, const std::vector<std::string>& detectPersons, const double minSamples,
                const double maxTravelTime, const std::vector<std::string>& vTypes, const double speedThreshold);

    /// @brief Destructor
    ~GNEMeanData();

    /// @brief methods to retrieve the elements linked to this meanData
    /// @{

    /// @brief get GNEHierarchicalElement associated with this meanData
    GNEHierarchicalElement* getHierarchicalElement() override;

    /// @brief get GNEMoveElement associated with this meanData
    GNEMoveElement* getMoveElement() const override;

    /// @brief get parameters associated with this meanData
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this meanData (constant)
    const Parameterised* getParameters() const override;

    /// @brief get GUIGlObject associated with this meanData
    GUIGlObject* getGUIGlObject() override;

    /// @brief get GUIGlObject associated with this meanData (constant)
    const GUIGlObject* getGUIGlObject() const override;

    /// @}

    /**@brief write meanData element into a xml file
     * @param[in] device device in which write parameters of meanData element
     */
    void writeMeanData(OutputDevice& device) const;

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

    /**@brief method for setting the attribute and letting the object perform data set changes
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
    /// @brief id
    std::string myID;

    /// @brief filename
    std::string myFile;

    /// @brief type
    std::string myType;

    /// @brief period
    SUMOTime myPeriod = 0;

    /// @brief begin
    SUMOTime myBegin = 0;

    /// @brief end
    SUMOTime myEnd = 0;

    /// @brief Whether vehicles are tracked
    bool myTrackVehicles = false;

    /// @brief bit mask for checking attributes to be written
    std::vector<SumoXMLAttr> myWrittenAttributes;

    /// @brief whether the data for all edges shall be aggregated
    bool myAggregate = false;

    /// @brief list of edges
    std::vector<std::string> myEdges;

    /// @brief edge file
    std::string myEdgeFile;

    /// @brief exclude empty
    std::string myExcludeEmpty;

    /// @brief width internal
    bool myWithInternal = false;

    /// @brief detect persons
    std::vector<std::string> myDetectPersons;

    /// @brief minSamples
    double myMinSamples = 0;

    /// @brief max travel time
    double myMaxTravelTime = 0;

    /// @brief VTypes
    std::vector<std::string> myVTypes;

    /// @brief speed threshold
    double mySpeedThreshold = 0;

private:
    /// @brief method for setting the attribute and nothing else (used in GNEChange_Attribute)
    void setAttribute(SumoXMLAttr key, const std::string& value) override;

    /// @brief Invalidated copy constructor.
    GNEMeanData(const GNEMeanData&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMeanData& operator=(const GNEMeanData&) = delete;
};

/****************************************************************************/
