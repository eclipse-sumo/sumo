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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <algorithm>


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
        // Data empty does't mean 0
        if (data.size() == 0) {
            return false;
        }
        for (int i = 0; i < (int)data.size(); i++) {
            if (data.at(i) == '+' || data.at(i) == '-') {
                if (i != 0) {
                    return false;
                }
            } else if (data.at(i) < '0' || data.at(i) > '9') {
                return false;
            }
        }
        return true;
    }

    /// @brief check if a String can be parsed into a double
    /// @ToDo check overflows
    static bool _str2double(const std::string& data) {
        bool dot = false;
        if (data.size() == 0) {
            return false;
        }
        for (int i = 0; i < (int)data.size(); i++) {
            if (data.at(i) == '+' || data.at(i) == '-') {
                if (i != 0) {
                    return false;
                }
            } else if (data.at(i) == '.') {
                if (data.at(i) == '.' && !dot) {
                    dot = true;
                } else {
                    return false;
                }
            } else if (data.at(i) < '0' || data.at(i) > '9') {
                return false;
            }
        }
        return true;
    }

    /// @brief check if a String can be parsed into a Bool
    static bool _str2bool(const std::string& data) {
        std::string dataToLower = data;
        std::transform(dataToLower.begin(), dataToLower.end(), dataToLower.begin(), ::tolower);
        if (data == "1" || data == "yes" || data == "true"  || data == "on"  || data == "x" || data == "t" ||
                data == "0" || data == "no"  || data == "false" || data == "off" || data == "-" || data == "f") {
            return true;
        } else {
            return false;
        }
    }

    /// @brief check if a String can be parsed into a SUMOTime
    /// @ToDo check overflows
    static bool _str2SUMOTime(const std::string& data) {
        // Data empty does't mean 0
        if (data.size() == 0) {
            return false;
        }
        for (int i = 0; i < (int)data.size(); i++) {
            if (data.at(i) == '+') {
                if (i != 0) {
                    return false;
                }
            } else if (data.at(i) < '0' || data.at(i) > '9') {
                return false;
            }
        }
        return true;
    }
};


#endif

/****************************************************************************/
