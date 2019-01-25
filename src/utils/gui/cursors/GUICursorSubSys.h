/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUICursorSubSys.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2018
/// @version $Id$
///
// A class to manage icons of SUMO
/****************************************************************************/
#ifndef GUICursorSubSys_h
#define GUICursorSubSys_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include "GUICursors.h"


// ===========================================================================
// class definitions
// ===========================================================================
class GUICursorSubSys {
public:
    /**@brief Initiate GUICursorSubSys
     * @param[in] a FOX Toolkit APP
     */
    static void initCursors(FXApp* a);

    /**@brief returns a cursor previously defined in the enum GUICursor
     * @param[in] which GUICursor enum
     */
    static FXCursor* getCursor(GUICursor which);

    /// @brief close GUICursorSubSys
    static void close();

private:
    /// @brief constructor is private because is called by the static function init(FXApp* a)
    GUICursorSubSys(FXApp* a);

    /// @brief destructor
    ~GUICursorSubSys();

    /// @brief instance of GUICursorSubSys
    static GUICursorSubSys* myInstance;

    /// @brief vector with the icons
    FXCursor* myCursors[CURSOR_MAX];
};


#endif

/****************************************************************************/

