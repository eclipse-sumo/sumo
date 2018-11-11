/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TplCheck.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
/// @version $Id$
///
// Some methods for check type of dates
/****************************************************************************/
#ifndef TplCheck_h
#define TplCheck_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <algorithm>
#include "TplConvert.h"
#include "SUMOTime.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * TplCheck
 * Some methdos that check if a string can be converted in other data types
 */
class TplCheck {
public:
    /// @brief check if a String can be parsed into a int
    /// @ToDo check overflows
    static bool _str2int(const std::string& data) {
        try {
            StringUtils::toInt(data);
            return true;
        } catch (...) {
            return false;
        }
    }

    /// @brief check if a String can be parsed into a double
    /// @ToDo check overflows
    static bool _str2double(const std::string& data) {
        try {
            StringUtils::toDouble(data);
            return true;
        } catch (...) {
            return false;
        }
    }

    /// @brief check if a String can be parsed into a Bool
    static bool _str2bool(const std::string& data) {
        try {
            StringUtils::toBool(data);
            return true;
        } catch (BoolFormatException&) {
            return false;
        }
    }

    /// @brief check if a String can be parsed into a SUMOTime
    /// @ToDo check overflows
    static bool _str2SUMOTime(const std::string& data) {
        try {
            string2time(data);
            return true;
        } catch (ProcessError&) {
            return false;
        }
    }
};


#endif

/****************************************************************************/
