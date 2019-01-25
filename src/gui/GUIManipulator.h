/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIManipulator.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @date    Jun 2004
/// @version $Id$
///
// Abstract GUI manipulation class
/****************************************************************************/
#ifndef GUIManipulator_h
#define GUIManipulator_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

