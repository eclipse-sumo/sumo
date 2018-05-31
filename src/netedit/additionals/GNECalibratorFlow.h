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
#include <config.h>

#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/RGBColor.h>

#include <netedit/GNEAttributeCarrier.h>

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
    /// @brief constructor (used only in GNECalibratorDialog)
    GNECalibratorFlow(GNECalibratorDialog* calibratorDialog, GNENet* net);

    /// @brief parameter constructor
    GNECalibratorFlow(GNECalibrator* calibratorParent, GNECalibratorVehicleType* vehicleType, GNECalibratorRoute* route, const std::string &vehsPerHour, const std::string &speed,
                      const RGBColor& color, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane,
                      const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, bool reroute,
                      const std::string& departPosLat, const std::string& arrivalPosLat, double begin, double end);

    /// @brief destructor
    ~GNECalibratorFlow();

    /// @brief write Flow values into a XML
    void writeFlow(OutputDevice& device);

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief inherited from GNEAttributeCarrier
    /// @{
    /// @brief select attribute carrier
    void selectAttributeCarrier(bool);

    /// @brief unselect attribute carrier
    void unselectAttributeCarrier(bool);

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

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

    /// @brief type of flow
    GNECalibratorVehicleType* myVehicleType;

    /// @brief route in which this flow is used
    GNECalibratorRoute* myRoute;

    /// @brief flows per hour (String instead float because can be empty)
    std::string myVehsPerHour;

    /// @brief flow speed (String instead float because can be empty)
    std::string mySpeed; 

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
    /// @}

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
