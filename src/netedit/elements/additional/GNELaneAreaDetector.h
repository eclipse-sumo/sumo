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
/// @file    GNELaneAreaDetector.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/
#pragma once
#include <config.h>
#include "GNEDetector.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNELaneAreaDetector
 * class for detector of type E2
 */
class GNELaneAreaDetector : public GNEDetector {

public:
    /// @brief default Constructor
    GNELaneAreaDetector(SumoXMLTag tag, GNENet* net);

    /**@brief Constructor for Single-Lane E2 detectors
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] pos position of the detector on the lane
     * @param[in] length The length of the detector in meters.
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] trafficLight The traffic light that triggers aggregation when switching.
     * @param[in] filename The path to the output file.
     * @param[in] vehicleTypes space separated list of vehicle type ids to consider
     * @param[in] nextEdges list of edge ids that must all be part of the future route of the vehicle to qualify for detection
     * @param[in] detectPersons detect persons instead of vehicles (pedestrians or passengers)
     * @param[in] name E2 detector name
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @param[in] speedThreshold The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] show detector in sumo-gui
     * @param[in] parameters generic parameters
     */
    GNELaneAreaDetector(const std::string& id, GNELane* lane, GNENet* net, const double pos, const double length, const SUMOTime freq,
                        const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes,
                        const std::vector<std::string>& nextEdges, const std::string& detectPersons, const std::string& name,
                        const SUMOTime timeThreshold, double speedThreshold, const double jamThreshold, const bool friendlyPos,
                        const bool show, const Parameterised::Map& parameters);

    /**@brief Constructor for Multi-Lane detectors
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lanes vector of lanes Lane of this StoppingPlace belongs
     * @param[in] net pointer to GNENet of this additional element belongs
     * @param[in] pos position of the detector on the first lane
     * @param[in] endPos position of the detector on the last lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] trafficLight The traffic light that triggers aggregation when switching.
     * @param[in] filename The path to the output file.
     * @param[in] vehicleTypes space separated list of vehicle type ids to consider
     * @param[in] nextEdges list of edge ids that must all be part of the future route of the vehicle to qualify for detection
     * @param[in] detectPersons detect persons instead of vehicles (pedestrians or passengers)
     * @param[in] name E2 detector name
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @param[in] speedThreshold The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam
     * @param[in] friendlyPos enable or disable friendly positions
     * @param[in] show detector in sumo-gui
     * @param[in] parameters generic parameters
     */
    GNELaneAreaDetector(const std::string& id, std::vector<GNELane*> lanes, GNENet* net, double pos, double endPos, const SUMOTime freq,
                        const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes,
                        const std::vector<std::string>& nextEdges, const std::string& detectPersons, const std::string& name,
                        const SUMOTime timeThreshold, double speedThreshold, const double jamThreshold, const bool friendlyPos,
                        const bool show, const Parameterised::Map& parameters);

    /// @brief Destructor
    ~GNELaneAreaDetector();

    /// @name members and functions relative to write additionals into XML
    /// @{

    /**@brief write additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief check if current additional is valid to be written into XML
    bool isAdditionalValid() const;

    /// @brief return a string with the current additional problem
    std::string getAdditionalProblem() const;

    /// @brief fix additional problem
    void fixAdditionalProblem();

    /// @}

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @name inherited from GUIGlObject
    /// @{

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @}

    /// @name inherited from GNEPathManager::PathElement
    /// @{

    /// @brief compute pathElement
    void computePathElement();

    /**@brief Draws partial object over lane
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment lane segment
     * @param[in] offsetFront front offset
     */
    void drawLanePartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const;

    /**@brief Draws partial object over junction
     * @param[in] s The settings for the current view (may influence drawing)
     * @param[in] segment junction segment
     * @param[in] offsetFront front offset
     */
    void drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNEPathManager::Segment* segment, const double offsetFront) const;

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

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /// @}

protected:
    /// @brief end position over lane (only for Multilane E2 detectors)
    double myEndPositionOverLane = 0;

    /// @brief The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
    SUMOTime myTimeThreshold = 0;

    /// @brief The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
    double mySpeedThreshold = 0;

    /// @brief The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam
    double myJamThreshold = 0;

    /// @brief Traffic light vinculated with this E2 Detector
    std::string myTrafficLight;

    /// @brief show or hidde detector in sumo-gui
    bool myShow = true;

private:
    /// @brief draw E2 detector
    void drawE2(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                const double exaggeration) const;

    /// @brief draw E2 partial lane
    void drawE2PartialLane(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                           const GNEPathManager::Segment* segment, const double offsetFront,
                           const GUIGeometry& geometry, const double exaggeration) const;

    /// @brief draw E2 partial junction
    void drawE2PartialJunction(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                               const bool onlyContour, const double offsetFront, const GUIGeometry& geometry,
                               const double exaggeration) const;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /// @brief get start position over lane that is applicable to the shape
    double getStartGeometryPositionOverLane() const;

    /// @brief get end position over lane that is applicable to the shape
    double getEndGeometryPositionOverLane() const;

    /// @brief Invalidated copy constructor.
    GNELaneAreaDetector(const GNELaneAreaDetector&) = delete;

    /// @brief Invalidated assignment operator.
    GNELaneAreaDetector& operator=(const GNELaneAreaDetector&) = delete;
};
