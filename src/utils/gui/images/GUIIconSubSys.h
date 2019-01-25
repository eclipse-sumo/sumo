/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIIconSubSys.h
/// @author  Daniel Krajzewicz
/// @date    Jul 2003
/// @version $Id$
///
// A class to manage icons of SUMO
/****************************************************************************/
#ifndef GUIIconSubSys_h
#define GUIIconSubSys_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include "GUIIcons.h"


// ===========================================================================
// class definitions
// ===========================================================================
class GUIIconSubSys {
public:
    /**@brief Initiate GUIIconSubSys
     * @param[in] a FOX Toolkit APP
     */
    static void initIcons(FXApp* a);

    /**@brief returns a icon previously defined in the enum GUIIcon
     * @param[in] which GUIIcon enum
     */
    static FXIcon* getIcon(GUIIcon which);

    /// @brief close GUIIconSubSys
    static void close();

private:
    /// @brief constructor is private because is called by the static function init(FXApp* a)
    GUIIconSubSys(FXApp* a);

    /// @brief destructor
    ~GUIIconSubSys();

    /// @brief instance of GUIIconSubSys
    static GUIIconSubSys* myInstance;

    /// @brief vector with the icons
    FXIcon* myIcons[ICON_MAX];
};


#endif

/****************************************************************************/

