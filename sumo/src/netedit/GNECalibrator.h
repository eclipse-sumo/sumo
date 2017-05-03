/****************************************************************************/
/// @file    GNECalibrator.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
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
#include "GNECalibratorRoute.h"
#include "GNECalibratorFlow.h"
#include "GNECalibratorVehicleType.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNERouteProbe;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibrator
 * class for represent Calibratos in netedit
 */
class GNECalibrator : public GNEAdditional {
public:

    /**@brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this calibrator belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position of the calibrator on the edge (Currently not used)
     * @param[in] frequency the aggregation interval in which to calibrate the flows
     * @param[in] output The output file for writing calibrator information
     * @param[in] calibratorRoutes vector with the calibratorRoutes of calibrator
     * @param[in] calibratorFlows vector with the calibratorFlows of calibrator
     * @param[in] calibratorVehicleTypes vector with the CalibratorVehicleType of calibrator
     */
    GNECalibrator(const std::string& id, GNELane* lane, GNEViewNet* viewNet, double pos, double frequency, const std::string& output,
                  const std::vector<GNECalibratorRoute>& calibratorRoutes, const std::vector<GNECalibratorFlow>& calibratorFlows,
                  const std::vector<GNECalibratorVehicleType>& calibratorVehicleTypes);

    /// @brief Destructor
    ~GNECalibrator();

    /// @brief change the position of the calibrator geometry
    void moveAdditionalGeometry(double, double);

    /// @brief updated geometry changes in the attributes of additional
    void commmitAdditionalGeometryMoved(double, double, GNEUndoList*);

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry c6hanges (i.e. lane moved)
    void updateGeometry();

    /// @brief Returns position of Calibrator in view
    Position getPositionInView() const;

    /// @brief open Calibrator Dialog
    void openAdditionalDialog();

    /**@brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device) const;

    /// @brief add calibrator vehicleType
    void addCalibratorVehicleType(const GNECalibratorVehicleType& vehicleType);

    /// @brief add calibrator flow
    void addCalibratorFlow(const GNECalibratorFlow& flow);

    /// @brief add calibrator route
    void addCalibratorRoute(const GNECalibratorRoute& route);

    /// @brief get calibrator vehicleTypes
    const std::vector<GNECalibratorVehicleType>& getCalibratorVehicleTypes() const;

    /// @brief get calibrator flows
    const std::vector<GNECalibratorFlow>& getCalibratorFlows() const;

    /// @brief get calibrator routes
    const std::vector<GNECalibratorRoute>& getCalibratorRoutes() const;

    /// @brief set calibrator vehicleTypes values
    void setCalibratorVehicleTypes(const std::vector<GNECalibratorVehicleType>& calibratorVehicleType);

    /// @brief set calibrator flow values
    void setCalibratorFlows(const std::vector<GNECalibratorFlow>& calibratorFlows);

    /// @brief set calibrator route values
    void setCalibratorRoutes(const std::vector<GNECalibratorRoute>& calibratorRoutes);

    /// @brief generate a vehicleType ID
    std::string generateVehicleTypeID() const;

    /// @brief generate a flow ID
    std::string generateFlowID() const;

    /// @brief generate a route ID
    std::string generateRouteID() const;

    /// @brief check if a vehicelType with ID = vehicleTypeID exists
    bool vehicleTypeExists(std::string vehicleTypeID) const;

    /// @brief check if a flow with ID = flowID exists
    bool flowExists(std::string flowID) const;

    /// @brief check if a route with ID = routeID exists
    bool routeExists(std::string routeID) const;

    /**@brief get reference to calibrator vehicle type
     * @throw ivalid argument if calibrator doesn't exist
     */
    const GNECalibratorVehicleType& getCalibratorVehicleType(const std::string& vehicleTypeID);

    /**@brief get reference to calibrator flow
     * @throw ivalid argument if calibrator doesn't exist
     */
    const GNECalibratorFlow& getCalibratorFlow(const std::string& flowID);

    /**@brief get reference calibrator route
     * @throw ivalid argument if calibrator doesn't exist
     */
    const GNECalibratorRoute& getCalibratorRoute(const std::string& routeID);

    /// @name inherited from GUIGlObject
    /// @{
    /// @brief Returns the name of the parent object
    /// @return This object's parent id
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
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief Frequency of calibrator
    double myFrequency;

    /// @brief output of calibrator
    std::string myOutput;

    /// @brief pointer to current RouteProbe
    GNERouteProbe* myRouteProbe;

    /// @brief calibrator route values
    std::vector<GNECalibratorRoute> myCalibratorRoutes;

    /// @brief calibrator flow values
    std::vector<GNECalibratorFlow> myCalibratorFlows;

    /// @brief calibrator vehicleType values
    std::vector<GNECalibratorVehicleType> myCalibratorVehicleTypes;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNECalibrator(const GNECalibrator&);

    /// @brief Invalidated assignment operator.
    GNECalibrator& operator=(const GNECalibrator&);
};

#endif
/****************************************************************************/
