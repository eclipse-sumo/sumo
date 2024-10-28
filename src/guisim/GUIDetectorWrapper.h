/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUIDetectorWrapper.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// The base class for detector wrapper
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDetectorWrapper
 * This is the base class for detector wrapper; As detectors may have a
 * position only or additionally a length, different display mechanisms are
 * necessary. These must be implemented in class erived from this one,
 * according to the wrapped detectors' properties.
 */
class GUIDetectorWrapper : public GUIGlObject_AbstractAdd {

public:
    /// Constructor
    GUIDetectorWrapper(GUIGlObjectType type, const std::string& id, FXIcon* icon);

    /// Destructor
    ~GUIDetectorWrapper();

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief return exaggeration associated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;
    /// @}

    /**
     * @class GUIBaseVehiclePopupMenu
     *
     * A popup-menu for detectors. In comparison to the normal popup-menu, this one
     *  also allows to trigger virtual detector calls
     */
    class PopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(PopupMenu)

    public:
        PopupMenu(GUIMainWindow& app, GUISUMOAbstractView& parent, GUIGlObject& o);

        /// @brief Destructor
        virtual ~PopupMenu() {};

        /// @brief called to set/reset virtual detector calls
        long onCmdSetOverride(FXObject*, FXSelector, void*);


    protected:
        FOX_CONSTRUCTOR(PopupMenu)
    };

protected:

    /// @brief whether this detector has an active virtual detector call
    virtual bool haveOverride() const {
        return false;
    }

    virtual void toggleOverride() const { }

    /// @brief whether this detector supports virtual detector calls
    bool mySupportsOverride;


};
