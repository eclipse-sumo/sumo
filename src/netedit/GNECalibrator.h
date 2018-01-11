/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibrator.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNECalibrator_h
#define GNECalibrator_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditional.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNERouteProbe;
class GNECalibratorFlow;
class GNECalibratorVehicleType;
class GNECalibratorRoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibrator
 * class for represent Calibratos in netedit
 */
class GNECalibrator : public GNEAdditional {

public:

    /**@brief Constructor using edge
    * @param[in] id The storage of gl-ids to get the one for this lane representation from
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] edge Edge of this calibrator belongs
    * @param[in] pos position of the calibrator on the edge (Currently not used)
    * @param[in] frequency the aggregation interval in which to calibrate the flows
    * @param[in] output The output file for writing calibrator information
    * @param[in] calibratorRoutes vector with the calibratorRoutes of calibrator
    * @param[in] calibratorFlows vector with the calibratorFlows of calibrator
    * @param[in] calibratorVehicleTypes vector with the CalibratorVehicleType of calibrator
    */
    GNECalibrator(const std::string& id, GNEViewNet* viewNet, GNEEdge* edge, double pos, double frequency, const std::string& output);

    /**@brief Constructor using lane
    * @param[in] id The storage of gl-ids to get the one for this lane representation from
    * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
    * @param[in] lane Lane of this calibrator belongs
    * @param[in] pos position of the calibrator on the edge (Currently not used)
    * @param[in] frequency the aggregation interval in which to calibrate the flows
    * @param[in] output The output file for writing calibrator information
    * @param[in] calibratorRoutes vector with the calibratorRoutes of calibrator
    * @param[in] calibratorFlows vector with the calibratorFlows of calibrator
    * @param[in] calibratorVehicleTypes vector with the CalibratorVehicleType of calibrator
    */
    GNECalibrator(const std::string& id, GNEViewNet* viewNet, GNELane* lane, double pos, double frequency, const std::string& output);

    /// @brief Destructor
    ~GNECalibrator();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief open Calibrator Dialog
    void openAdditionalDialog();

    /// @name Functions related with Calibrator items
    /// @{

    /// @brief add calibrator route
    void addCalibratorRoute(GNECalibratorRoute* route);

    /// @brief add calibrator route
    void removeCalibratorRoute(GNECalibratorRoute* route);

    /// @brief get calibrator routes
    const std::vector<GNECalibratorRoute*>& getCalibratorRoutes() const;

    /// @brief add calibrator flow
    void addCalibratorFlow(GNECalibratorFlow* flow);

    /// @brief remove calibrator flow
    void removeCalibratorFlow(GNECalibratorFlow* flow);

    /// @brief get calibrator flows
    const std::vector<GNECalibratorFlow*>& getCalibratorFlows() const;

    /// @brief add calibrator vehicleType
    void addCalibratorVehicleType(GNECalibratorVehicleType* vehicleType);

    /// @brief remove calibrator vehicleType
    void removeCalibratorVehicleType(GNECalibratorVehicleType* vehicleType);

    /// @brief get calibrator vehicleTypes
    const std::vector<GNECalibratorVehicleType*>& getCalibratorVehicleTypes() const;

    /// @}

    /// @name Functions related with geometry of element
    /// @{
    /**@brief change the position of the element geometry without saving in undoList
     * @param[in] newPosition new position of geometry
     * @note should't be called in drawGL(...) functions to avoid smoothness issues
     */
    void moveGeometry(const Position& oldPos, const Position& offset);

    /**@brief commit geometry changes in the attributes of an element after use of moveGeometry(...)
     * @param[in] oldPos the old position of additional
     * @param[in] undoList The undoList on which to register changes
     */
    void commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList);

    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of additional in view
    Position getPositionInView() const;
    /// @}

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns the name of the parent object
     * @return This object's parent id
     */
    const std::string& getParentName() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value) ;
    /// @}

protected:
    /// @brief The edge in which Calibrators are placed
    GNEEdge* myEdge;

    /// @brief The lane in which CalibratorLane are placed
    GNELane* myLane;

    /// @brief position over Lane
    double myPositionOverLane;

    /// @brief Frequency of calibrator
    double myFrequency;

    /// @brief output of calibrator
    std::string myOutput;

    /// @brief pointer to current RouteProbe
    GNERouteProbe* myRouteProbe;

    /// @brief calibrator route values
    std::vector<GNECalibratorRoute*> myCalibratorRoutes;

    /// @brief calibrator flow values
    std::vector<GNECalibratorFlow*> myCalibratorFlows;

    /// @brief calibrator vehicleType values
    std::vector<GNECalibratorVehicleType*> myCalibratorVehicleTypes;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNECalibrator(const GNECalibrator&) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibrator& operator=(const GNECalibrator&) = delete;
};

#endif
/****************************************************************************/
