/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
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

// ===========================================================================
// class declarations
// ===========================================================================

class GNEVariableSpeedSign;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEVariableSpeedSignStep
 * class used to represent a interval used in variable speed sign
 */
class GNEVariableSpeedSignStep {
public:
    /// @brief default constructor
    GNEVariableSpeedSignStep(GNEVariableSpeedSign* variableSpeedSignParent);

    /// @brief constructor
    GNEVariableSpeedSignStep(GNEVariableSpeedSign* variableSpeedSignParent, double time, double speed);

    /// @brief destructor
    ~GNEVariableSpeedSignStep();

    /// @brief get variable speed sign parent
    GNEVariableSpeedSign* getVariableSpeedSignParent() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get time
    double getTime() const;

    /// @brief get speed
    double getSpeed() const;

    /**@brief set time
     * @brief return true if was sucesfully set, false in other case
     */
    bool setTime(double time);

    /**@brief set speed time
     * @brief return true if was sucesfully set, false in other case
     */
    bool setSpeed(double speed);

    /// @brief overload operator ==
    bool operator==(const GNEVariableSpeedSignStep& variableSpeedSignStep) const;

    /// @brief overload operator !=
    bool operator!=(const GNEVariableSpeedSignStep& variableSpeedSignStep) const;

    /// @brief overload operator >
    bool operator>(const GNEVariableSpeedSignStep& variableSpeedSignStep) const;

    /// @brief overload operator <
    bool operator<(const GNEVariableSpeedSignStep& variableSpeedSignStep) const;

    /// @brief overload operator >=
    bool operator>=(const GNEVariableSpeedSignStep& variableSpeedSignStep) const;

    /// @brief overload operator <=
    bool operator<=(const GNEVariableSpeedSignStep& variableSpeedSignStep) const;

protected:
    /// @brief pointer to variable speed sign parent
    GNEVariableSpeedSign* myVariableSpeedSignParent;

    /// @brief timeStep
    double myTime;

    /// @brief speed in this timeStep
    double mySpeed;

    /// @brief XML Tag of a variable speed sign interval
    SumoXMLTag myTag;
};

#endif

/****************************************************************************/
