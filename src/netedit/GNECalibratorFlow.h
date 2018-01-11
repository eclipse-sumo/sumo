/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorFlow.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// Flow used by GNECalibrators
/****************************************************************************/
#ifndef GNECalibratorFlow_h
#define GNECalibratorFlow_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/RGBColor.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNECalibrator;
class GNECalibratorDialog;
class GNECalibratorVehicleType;
class GNECalibratorRoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorFlow
 * flow flow used by GNECalibrators
 */
class GNECalibratorFlow : public GNEAttributeCarrier {

public:

    /// @brief type of flow
    enum TypeOfFlow {
        GNE_CALIBRATORFLOW_INVALID,
        GNE_CALIBRATORFLOW_VEHSPERHOUR,
        GNE_CALIBRATORFLOW_PERIOD,
        GNE_CALIBRATORFLOW_PROBABILITY
    };

    /// @brief constructor (used only in GNECalibratorDialog)
    GNECalibratorFlow(GNECalibratorDialog* calibratorDialog);

    /// @brief parameter constructor
    GNECalibratorFlow(GNECalibrator* calibratorParent, const std::string& flowID, GNECalibratorVehicleType* vehicleType, GNECalibratorRoute* route,
                      const RGBColor& color, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane,
                      const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, bool reroute,
                      const std::string& departPosLat, const std::string& arrivalPosLat, double begin, double end, double vehsPerHour, double period,
                      double probability, int number, GNECalibratorFlow::TypeOfFlow flowType);

    /// @brief destructor
    ~GNECalibratorFlow();

    /// @brief write Flow values into a XML
    void writeFlow(OutputDevice& device);

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief get type of flow
    GNECalibratorFlow::TypeOfFlow getFlowType() const;

    /// @brief set type of flow
    void setFlowType(GNECalibratorFlow::TypeOfFlow type);

    /// @brief inherited from GNEAttributeCarrier
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
    * @param[in] net optionally the GNENet to inform about gui updates
    */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform additional changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief pointer to calibrator parent
    GNECalibrator* myCalibratorParent;

    /// @brief ID of flow
    std::string myFlowID;

    /// @brief type of flow
    GNECalibratorVehicleType* myVehicleType;

    /// @brief route in which this flow is used
    GNECalibratorRoute* myRoute;

    /// @brief color of flow
    RGBColor myColor;

    /// @brief depart lane
    std::string myDepartLane;

    /// @brief depart position
    std::string myDepartPos;

    /// @brief depart speed
    std::string myDepartSpeed;

    /// @brief arrival lane
    std::string myArrivalLane;

    /// @brief arrival pos
    std::string myArrivalPos;

    /// @brief arrival speed
    std::string myArrivalSpeed;

    /// @brief line of bus/container stop
    std::string myLine;

    /// @brief number of person
    int myPersonNumber;

    /// @brief number of container
    int myContainerNumber;

    /// @brief reroute
    bool myReroute;

    /// @brief departPosLat
    std::string myDepartPosLat;

    //// @brief arrivalPosLat
    std::string myArrivalPosLat;

    /// @name specific of flows
    /// @{
    /// @brief time step begin
    double myBegin;

    /// @brief time step end
    double myEnd;

    /// @brief flows per hour
    double myVehsPerHour;

    /// @brief period
    double myPeriod;

    /// @brief probability
    double myProbability;

    /// @brief number of flow
    int myNumber;
    /// @}

    /// @brief type of flow
    TypeOfFlow myFlowType;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNECalibratorFlow(const GNECalibratorFlow&) = delete;

    /// @brief Invalidated assignment operator
    GNECalibratorFlow& operator=(const GNECalibratorFlow&) = delete;
};

#endif
/****************************************************************************/
