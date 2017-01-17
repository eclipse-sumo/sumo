/****************************************************************************/
/// @file    GUIManipulator.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @date    Jun 2004
/// @version $Id$
///
// Abstract GUI manipulation class
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
#ifndef GUIManipulator_h
#define GUIManipulator_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIMainWindow;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIManipulator
 */
class GUIManipulator : public FXDialogBox {
    FXDECLARE(GUIManipulator)
public:
    /// Constructor
    GUIManipulator(GUIMainWindow& app, const std::string& name,
                   int xpos, int ypos);

    /// Destructor
    virtual ~GUIManipulator();

protected:
    /// FOX needs this
    GUIManipulator() { }

};


#endif

/****************************************************************************/

