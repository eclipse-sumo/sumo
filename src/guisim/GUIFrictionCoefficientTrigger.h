/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GUIFrictionCoefficientTrigger.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 26.04.2004
///
// Changes the friction allowed on a set of lanes (gui version)
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <string>
#include <microsim/trigger/MSFrictionCoefficientTrigger.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <gui/GUIManipulator.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIFrictionCoefficientTrigger
 * @brief Changes the friction allowed on a set of lanes (gui version)
 *
 * This is the gui-version of the MSFrictionCoefficientTrigger-object
 */
class GUIFrictionCoefficientTrigger
    : public MSFrictionCoefficientTrigger,
      public GUIGlObject_AbstractAdd {
public:
    /** @brief Constructor
     * @param[in] idStorage The gl-id storage for giving this object an gl-id
     * @param[in] id The id of the lane friction trigger
     * @param[in] destLanes List of lanes affected by this friction trigger
     * @param[in] file Name of the file to read the frictions to set from
     */
    GUIFrictionCoefficientTrigger(const std::string& id,
                        const std::vector<MSLane*>& destLanes,
                        const std::string& file);

    /** destructor */
    ~GUIFrictionCoefficientTrigger();

    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent);

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent);

    /// @brief return exaggeration asociated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    //@}

    GUIManipulator* openManipulator(GUIMainWindow& app,
                                    GUISUMOAbstractView& parent);

public:
    class GUIFrictionCoefficientTriggerPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUIFrictionCoefficientTriggerPopupMenu)
    public:

        GUIFrictionCoefficientTriggerPopupMenu(GUIMainWindow& app,
                                     GUISUMOAbstractView& parent, GUIGlObject& o);

        ~GUIFrictionCoefficientTriggerPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*, FXSelector, void*);

    protected:
        GUIFrictionCoefficientTriggerPopupMenu() { }

    };

    class GUIManip_FrictionCoefficientTrigger : public GUIManipulator {
        FXDECLARE(GUIManip_FrictionCoefficientTrigger)
    public:
        enum {
            MID_USER_DEF = FXDialogBox::ID_LAST,
            MID_PRE_DEF,
            MID_OPTION,
            MID_CLOSE,
            ID_LAST
        };
        /// Constructor
        GUIManip_FrictionCoefficientTrigger(GUIMainWindow& app,
                                  const std::string& name, GUIFrictionCoefficientTrigger& o,
                                  int xpos, int ypos);

        /// Destructor
        virtual ~GUIManip_FrictionCoefficientTrigger();

        long onCmdOverride(FXObject*, FXSelector, void*);
        long onCmdClose(FXObject*, FXSelector, void*);
        long onCmdUserDef(FXObject*, FXSelector, void*);
        long onUpdUserDef(FXObject*, FXSelector, void*);
        long onCmdPreDef(FXObject*, FXSelector, void*);
        long onUpdPreDef(FXObject*, FXSelector, void*);
        long onCmdChangeOption(FXObject*, FXSelector, void*);

    private:
        GUIMainWindow* myParent;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        double myFriction;

        FXDataTarget myFrictionTarget;

        FXRealSpinner* myUserDefinedFriction;

        FXComboBox* myPredefinedValues;

        GUIFrictionCoefficientTrigger* myObject;

    protected:
        GUIManip_FrictionCoefficientTrigger() { }

    };

private:
    /// Definition of a positions container
    typedef std::vector<Position> PosCont;

    /// Definition of a rotation container
    typedef std::vector<double> RotCont;

private:
    /// The positions in full-geometry mode
    PosCont myFGPositions;

    /// The rotations in full-geometry mode
    RotCont myFGRotations;

    /// The boundary of this rerouter
    Boundary myBoundary;

    /// The information whether the friction shall be shown in percent or decimal
    bool myShowAsPercent;

    /// Storage for last value to avoid string recomputation
    mutable double myLastValue;

    /// Storage for friction string to avoid recomputation
    mutable std::string myLastValueString;

};
