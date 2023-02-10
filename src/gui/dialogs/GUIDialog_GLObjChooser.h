/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIDialog_GLObjChooser.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Class for the window that allows to choose a street, junction or vehicle
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/windows/GUIDialog_ChooserAbstract.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOViewParent;
class GUIGlChildWindow;
class GUIGlObjectStorage;
class GUIGlObject;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_GLObjChooser
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_GLObjChooser : public GUIDialog_ChooserAbstract {


public:
    /** @brief Constructor
     * @param[in] SUMOViewParent The calling view (SUMO-GUI)
     * @param[in] viewParent The calling view (netedit)
     * @param[in] icon The icon to use
     * @param[in] title The title to use
     * @param[in] glStorage The storage to retrieve ids from
     */
    GUIDialog_GLObjChooser(GUISUMOViewParent* SUMOViewParent, int messageId,
                           FXIcon* icon, const FXString& title, const std::vector<GUIGlID>& ids,
                           GUIGlObjectStorage& glStorage);

    /// @brief Destructor
    virtual ~GUIDialog_GLObjChooser();

private:
    /// @brief SUMO-GUI View parent
    GUISUMOViewParent* mySUMOViewParent;
};
