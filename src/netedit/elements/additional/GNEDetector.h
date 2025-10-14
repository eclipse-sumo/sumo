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
/// @file    GNEDetector.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
// A abstract class to define common parameters of detectors placed over lanes
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEAdditional.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNEDetector : public GNEAdditional, public Parameterised {

public:
    /**@brief Default constructor
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] tag Type of xml tag that define the detector (SUMO_TAG_INDUCTION_LOOP, SUMO_TAG_LANE_AREA_DETECTOR, etc...)
     */
    GNEDetector(GNENet* net, SumoXMLTag tag);

    /**@brief Constructor
     * @param[in] id Gl-id of the detector (Must be unique)
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] filename file in which this AttributeCarrier is stored
     * @param[in] tag Type of xml tag that define the detector (SUMO_TAG_INDUCTION_LOOP, SUMO_TAG_LANE_AREA_DETECTOR, etc...)
     * @param[in] period the aggregation period the values the detector collects shall be summed up.
     * @param[in] vehicleTypes space separated list of vehicle type ids to consider
     * @param[in] nextEdges list of edge ids that must all be part of the future route of the vehicle to qualify for detection
     * @param[in] detectPersons detect persons instead of vehicles (pedestrians or passengers)
     * @param[in] outputFilename The path to the output file.
     * @param[in] name detector name
     * @param[in] parameters generic parameters
     */
    GNEDetector(const std::string& id, GNENet* net, const std::string& filename, SumoXMLTag tag,
                const SUMOTime period, const std::string& outputFilename, const std::vector<std::string>& vehicleTypes,
                const std::vector<std::string>& nextEdges, const std::string& detectPersons, const std::string& name,
                const Parameterised::Map& parameters);

    /**@brief Constructor
     * @param[in] additionalParent parent additional of this detector (ID will be generated automatically)
     * @param[in] tag Type of xml tag that define the detector (SUMO_TAG_INDUCTION_LOOP, SUMO_TAG_LANE_AREA_DETECTOR, etc...)
     * @param[in] pos position of the detector on the lane
     * @param[in] period the aggregation period the values the detector collects shall be summed up.
     * @param[in] parentLanes vector of parent lanes
     * @param[in] outputFilename The path to the output file.
     * @param[in] name detector name
     * @param[in] parameters generic parameters
     */
    GNEDetector(GNEAdditional* additionalParent, SumoXMLTag tag, const SUMOTime period, const std::string& outputFilename,
                const std::string& name, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEDetector();

    /// @brief methods to retrieve the elements linked to this detector
    /// @{

    /// @brief get parameters associated with this detector
    Parameterised* getParameters() override;

    /// @brief get parameters associated with this detector (constant)
    const Parameterised* getParameters() const override;

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const override;

    /// @}

    /// @name Functions related with geometry of element
    /// @{

    /// @brief Returns position of additional in view
    Position getPositionInView() const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /// @brief split geometry
    void splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement, const GNENetworkElement* newElement, GNEUndoList* undoList);

    /// @}

    /// @name inherited from GUIGLObject
    /// @{

    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    std::string getParentName() const;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /* @brief method for getting the Attribute of an XML key in positionVector format
     * @param[in] key The attribute key
     * @return positionVector with the value associated to key
     */
    PositionVector getAttributePositionVector(SumoXMLAttr key) const override;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const override;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const override;

    /// @}

protected:
    /// @brief The aggregation period the values the detector collects shall be summed up.
    SUMOTime myPeriod = 0;

    /// @brief The path to the output file
    std::string myOutputFilename;

    /// @brief attribute vehicle types
    std::vector<std::string> myVehicleTypes;

    /// @brief next edges
    std::vector<std::string> myNextEdges;

    /// @brief detect persons
    std::string myDetectPersons;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getDetectorAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getDetectorAttributeDouble(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in position format
     * @param[in] key The attribute key
     * @return position with the value associated to key
     */
    Position getDetectorAttributePosition(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setDetectorAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isDetectorValid(SumoXMLAttr key, const std::string& value);

    /**@brief write additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeDetectorValues(OutputDevice& device) const;

    /// @brief set attribute after validation
    void setDetectorAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief draw E1 shape
    void drawE1Shape(const GUIVisualizationSettings::Detail d, const double exaggeration,
                     const RGBColor& mainColor, const RGBColor& secondColor) const;

    /// @brief draw E1 detector Logo
    void drawE1DetectorLogo(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const double exaggeration, const std::string& logo, const RGBColor& textColor) const;

    /// @brief draw E2 detector Logo
    void drawE2DetectorLogo(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const double exaggeration, const std::string& logo, const RGBColor& textColor) const;

private:
    /// @brief Invalidate return position of additional
    const Position& getPosition() const = delete;

    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos) = delete;
};
