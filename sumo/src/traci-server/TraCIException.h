/****************************************************************************/
/// @file    TraCIException.h
/// @author  Axel Wegener
/// @author  Friedemann Wesner
/// @author  Christoph Sommer
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2007/10/24
/// @version $Id$
///
/// Exception thrown by TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <stdexcept>


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class TraCIException
*/
class TraCIException : public std::runtime_error {
public:
    /** constructor */
    TraCIException(std::string what)
        : std::runtime_error(what) {}
};

#endif

#endif
