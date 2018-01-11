/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVariableSpeedSignStep.h
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNEVariableSpeedSignStep_h
#define GNEVariableSpeedSignStep_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEVariableSpeedSign;
class GNEVariableSpeedSignDialog;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEVariableSpeedSignStep
 * class used to represent a interval used in variable speed sign
 */
class GNEVariableSpeedSignStep : public GNEAttributeCarrier {

public:
    /// @brief default constructor
    GNEVariableSpeedSignStep(GNEVariableSpeedSignDialog* variableSpeedSignDialog);

    /// @brief constructor
    GNEVariableSpeedSignStep(GNEVariableSpeedSign* variableSpeedSignParent, double time, double speed);

    /// @brief destructor
    ~GNEVariableSpeedSignStep();

    /// @brief get variable speed sign parent
    GNEVariableSpeedSign* getVariableSpeedSignParent() const;

    /// @brief write step into XML
    void writeStep(OutputDevice& device);

    /// @brief get time
    double getTime() const;

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
    /// @brief pointer to variable speed sign parent
    GNEVariableSpeedSign* myVariableSpeedSignParent;

    /// @brief timeStep
    double myTime;

    /// @brief speed in this timeStep
    double mySpeed;

    /// @brief XML Tag of a variable speed sign interval
    SumoXMLTag myTag;

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEVariableSpeedSignStep(const GNEVariableSpeedSignStep&) = delete;

    /// @brief Invalidated assignment operator
    GNEVariableSpeedSignStep& operator=(const GNEVariableSpeedSignStep&) = delete;
};

#endif

/****************************************************************************/
