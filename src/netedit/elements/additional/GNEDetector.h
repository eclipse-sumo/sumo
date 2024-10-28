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
/**
 * @class GNEdetector
 * @briefA abstract class to define common parameters and functions of detectors
 */
class GNEDetector : public GNEAdditional, public Parameterised {

public:
    /**@brief Constructor.
     * @param[in] id Gl-id of the detector (Must be unique)
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] type GUIGlObjectType of detector
     * @param[in] tag Type of xml tag that define the detector (SUMO_TAG_INDUCTION_LOOP, SUMO_TAG_LANE_AREA_DETECTOR, etc...)
     * @param[in] pos position of the detector on the lane
     * @param[in] period the aggregation period the values the detector collects shall be summed up.
     * @param[in] parentLanes vector of parent lanes
     * @param[in] vehicleTypes space separated list of vehicle type ids to consider
     * @param[in] nextEdges list of edge ids that must all be part of the future route of the vehicle to qualify for detection
     * @param[in] detectPersons detect persons instead of vehicles (pedestrians or passengers)
     * @param[in] filename The path to the output file.
     * @param[in] name detector name
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] parameters generic parameters
     */
    GNEDetector(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const double pos,
                const SUMOTime period, const std::vector<GNELane*>& parentLanes, const std::string& filename,
                const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges, const std::string& detectPersons,
                const std::string& name, const bool friendlyPos, const Parameterised::Map& parameters);

    /**@brief Constructor.
     * @param[in] additionalParent parent additional of this detector (ID will be generated automatically)
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] type GUIGlObjectType of detector
     * @param[in] tag Type of xml tag that define the detector (SUMO_TAG_INDUCTION_LOOP, SUMO_TAG_LANE_AREA_DETECTOR, etc...)
     * @param[in] pos position of the detector on the lane
     * @param[in] period the aggregation period the values the detector collects shall be summed up.
     * @param[in] parentLanes vector of parent lanes
     * @param[in] filename The path to the output file.
     * @param[in] name detector name
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] parameters generic parameters
     */
    GNEDetector(GNEAdditional* additionalParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag, FXIcon* icon, const double pos,
                const SUMOTime period, const std::vector<GNELane*>& parentLanes, const std::string& filename, const std::string& name,
                const bool friendlyPos, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNEDetector();

    /**@brief get move operation
     * @note returned GNEMoveOperation can be nullptr
     */
    GNEMoveOperation* getMoveOperation();

    /// @name members and functions relative to write additionals into XML
    /// @{

    /**@brief write additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) const = 0;

    /// @brief check if current additional is valid to be written into XML (must be reimplemented in all detector children)
    virtual bool isAdditionalValid() const = 0;

    /// @brief return a string with the current additional problem (must be reimplemented in all detector children)
    virtual std::string getAdditionalProblem() const = 0;

    /// @brief fix additional problem (must be reimplemented in all detector children)
    virtual void fixAdditionalProblem() = 0;

    /// @}

    /// @name Function related with contour drawing
    /// @{

    /// @brief check if draw move contour (red)
    bool checkDrawMoveContour() const;

    /// @}

    /// @brief get lane
    GNELane* getLane() const;

    /// @brief get position over lane
    double getPositionOverLane() const;

    /// @brief get position over lane that is applicable to the shape
    double getGeometryPositionOverLane() const;

    /// @name Functions related with geometry of element
    /// @{

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

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

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;

    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    virtual double getAttributeDouble(SumoXMLAttr key) const = 0;

    /// @brief get parameters map
    const Parameterised::Map& getACParametersMap() const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief get PopPup ID (Used in AC Hierarchy)
    std::string getPopUpID() const;

    /// @brief get Hierarchy Name (Used in AC Hierarchy)
    std::string getHierarchyName() const;

    /// @}

protected:
    /// @brief position of detector over Lane
    double myPositionOverLane = 0;

    /// @brief The aggregation period the values the detector collects shall be summed up.
    SUMOTime myPeriod = 0;

    /// @brief The path to the output file
    std::string myFilename;

    /// @brief attribute vehicle types
    std::vector<std::string> myVehicleTypes;

    /// @brief next edges
    std::vector<std::string> myNextEdges;

    /// @brief detect persons
    std::string myDetectPersons;

    /// @brief Flag for friendly position
    bool myFriendlyPosition = false;

    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getDetectorAttribute(SumoXMLAttr key) const;

    /* @brief method for getting the Attribute of an XML key in double format (to avoid unnecessary parse<double>(...) for certain attributes)
     * @param[in] key The attribute key
     * @return double with the value associated to key
     */
    double getDetectorAttributeDouble(SumoXMLAttr key) const;

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
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief set move shape
    virtual void setMoveShape(const GNEMoveResult& moveResult) = 0;

    /// @brief commit move shape
    virtual void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) = 0;

    /// @brief Invalidate return position of additional
    const Position& getPosition() const = delete;

    /// @brief Invalidate set new position in the view
    void setPosition(const Position& pos) = delete;
};
