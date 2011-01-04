/****************************************************************************/
/// @file    GUILaneSpeedTrigger.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Changes the speed allowed on a set of lanes (gui version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUILaneSpeedTrigger_h
#define GUILaneSpeedTrigger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <microsim/trigger/MSLaneSpeedTrigger.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/Position2D.h>
#include <gui/GUIManipulator.h>
#include <utils/foxtools/FXRealSpinDial.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class GUIManipulator;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILaneSpeedTrigger
 * @brief Changes the speed allowed on a set of lanes (gui version)
 *
 * This is the gui-version of the MSLaneSpeedTrigger-object
 */
class GUILaneSpeedTrigger
        : public MSLaneSpeedTrigger,
            public GUIGlObject_AbstractAdd {
public:
    /** @brief Constructor
     * @param[in] idStorage The gl-id storage for giving this object an gl-id
     * @param[in] id The id of the lane speed trigger
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     */
    GUILaneSpeedTrigger(GUIGlObjectStorage &idStorage,
                        const std::string &id,
                        const std::vector<MSLane*> &destLanes,
                        const std::string &file) throw(ProcessError);


    /** destructor */
    ~GUILaneSpeedTrigger() throw();



    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the lane speed trigger
     * @see GUIGlObject::getMicrosimID
     */
    const std::string &getMicrosimID() const throw();


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings &s) const throw();
    //@}



    GUIManipulator *openManipulator(GUIMainWindow &app,
                                    GUISUMOAbstractView &parent);

public:
    class GUILaneSpeedTriggerPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUILaneSpeedTriggerPopupMenu)
    public:

        GUILaneSpeedTriggerPopupMenu(GUIMainWindow &app,
                                     GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUILaneSpeedTriggerPopupMenu() throw();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

    protected:
        GUILaneSpeedTriggerPopupMenu() { }

    };

    class GUIManip_LaneSpeedTrigger : public GUIManipulator {
        FXDECLARE(GUIManip_LaneSpeedTrigger)
    public:
        enum {
            MID_USER_DEF = FXDialogBox::ID_LAST,
            MID_PRE_DEF,
            MID_OPTION,
            MID_CLOSE,
            ID_LAST
        };
        /// Constructor
        GUIManip_LaneSpeedTrigger(GUIMainWindow &app,
                                  const std::string &name, GUILaneSpeedTrigger &o,
                                  int xpos, int ypos);

        /// Destructor
        virtual ~GUIManip_LaneSpeedTrigger();

        long onCmdOverride(FXObject*,FXSelector,void*);
        long onCmdClose(FXObject*,FXSelector,void*);
        long onCmdUserDef(FXObject*,FXSelector,void*);
        long onUpdUserDef(FXObject*,FXSelector,void*);
        long onCmdPreDef(FXObject*,FXSelector,void*);
        long onUpdPreDef(FXObject*,FXSelector,void*);
        long onCmdChangeOption(FXObject*,FXSelector,void*);

    private:
        GUIMainWindow *myParent;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        SUMOReal mySpeed;

        FXDataTarget mySpeedTarget;

        FXRealSpinDial *myUserDefinedSpeed;

        FXComboBox *myPredefinedValues;

        GUILaneSpeedTrigger *myObject;

    protected:
        GUIManip_LaneSpeedTrigger() { }

    };

private:
    /// Definition of a positions container
    typedef std::vector<Position2D> PosCont;

    /// Definition of a rotation container
    typedef std::vector<SUMOReal> RotCont;

private:
    /// The positions in full-geometry mode
    PosCont myFGPositions;

    /// The rotations in full-geometry mode
    RotCont myFGRotations;

    /// The boundary of this rerouter
    Boundary myBoundary;

    /// The information whether the speed shall be shown in m/s or km/h
    bool myShowAsKMH;

    /// Storage for last value to avoid string recomputation
    mutable SUMOReal myLastValue;

    /// Storage for speed string to avoid recomputation
    mutable std::string myLastValueString;

};


#endif

/****************************************************************************/

