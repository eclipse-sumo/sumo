/****************************************************************************/
/// @file    TplCheck.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
/// @version $Id$
///
// Some methods for check type of dates
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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

    /// @brief check if a String can be parsed into a SUMOReal
    /// @ToDo check overflows
    static bool _str2SUMOReal(const std::string& data) {
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
