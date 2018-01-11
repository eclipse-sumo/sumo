/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorRoute.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// Route used by GNECalibrators
/****************************************************************************/
#ifndef GNECalibratorRoute_h
#define GNECalibratorRoute_h


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
class GNEEdge;
class GNECalibratorDialog;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorRoute
 * vehicle route used by GNECalibrators
 */
class GNECalibratorRoute : public GNEAttributeCarrier {

public:
    /// @brief default constructor (used only in GNECalibratorDialog)
    GNECalibratorRoute(GNECalibratorDialog* calibratorDialog);

    /// @brief parameter constructor
    GNECalibratorRoute(GNECalibrator* calibratorParent, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color);

    /// @brief destructor
    ~GNECalibratorRoute();

    /// @brief write Route values into a XML
    void writeRoute(OutputDevice& device);

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief get GNEEdges of Calibrator ROute
    const std::vector<GNEEdge*>& getGNEEdges() const;

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

    /// @brief route in which this flow is used
    std::string myRouteID;

    /// @brief edges of route
    std::vector<GNEEdge*> myEdges;

    /// @brief color of ROute
    RGBColor myColor;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNECalibratorRoute(GNECalibratorRoute*) = delete;

    /// @brief Invalidated assignment operator.
    GNECalibratorRoute& operator=(GNECalibratorRoute*) = delete;
};

#endif
/****************************************************************************/
