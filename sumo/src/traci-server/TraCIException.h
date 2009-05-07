/****************************************************************************/
/// @file    TraCIException.h
/// @author  Axel Wegener <wegener@itm.uni-luebeck.de>
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @author  Christoph Sommer <christoph.sommer@informatik.uni-erlangen.de>
/// @date    2007/10/24
/// @version $Id: TraCIException.h 7082 2009-04-28 11:16:02Z behrisch $
///
/// Exception thrown by TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TRACIEXCEPTION_H
#define TRACIEXCEPTION_H
#ifndef NO_TRACI


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
* @class TraCIServer
*/
namespace traci {
class TraCIException {
private:
    std::string what_;

public:
    TraCIException(std::string what)
            : what_(what) {}
    std::string what() const {
        return what_;
    }
};

}

#endif

#endif
