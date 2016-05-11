/****************************************************************************/
/// @file    TplCheck.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2016
/// @version $Id: TplCheck.h 20584 2016-04-28 11:32:24Z palcraft $
///
// Some methods for check type of dates
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
    /// @ToDo check Oveflorws
    static bool _checkInt(const std::string &data) {
        if(data.size() == 0)
            return false;
        for(int i = 0; i < data.size(); i++)
            if(!(data.at(i) >= '0' || data.at(i) <= '9' || data.at(i) != '+' || data.at(i) != '-'))
                return false;
        return true;
    }

    /// @brief check if a String can be parsed into a SUMOReal
    /// @ToDo check Oveflorws
    static bool _checkSUMOReal(const std::string &data) {
        bool dot = false;
        if(data.size() == 0)
            return false;
        for(int i = 0; i < data.size(); i++)
            if(data.at(i) < '0' || data.at(i) > '9')
                if(data.at(i) == '.' && !dot)
                    dot = true;
                else
                    return false;
        return true;
    }
    
    /// @brief check if a String can be parsed into a Bool
    /// @ToDo check other posibilities (positive, negative, +, -,...)
    static bool _checkBool(const std::string &data) {
        if(data == "true" || data == "false" || data == "1" || data == "0")
            return true;
        else
            return false;
    }

    /// @brief check if a String can be parsed into a SUMOTime
    /// @ToDo check other posibilities (positive, negative, +, -,...)
    static bool _checkSUMOTime(const std::string &data) {
        if(data.size() == 0)
            return false;
        for(int i = 0; i < data.size(); i++)
            if(!(data.at(i) >= '0' || data.at(i) <= '9'))
                return false;
        return true;
    }
};


#endif

/****************************************************************************/
