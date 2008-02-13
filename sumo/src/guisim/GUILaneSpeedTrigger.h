/****************************************************************************/
/// @file    GUILaneSpeedTrigger.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Changes the speed allowed on a set of lanes (gui version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <utils/geom/HaveBoundary.h>
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
            public GUIGlObject_AbstractAdd
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane speed trigger
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     */
    GUILaneSpeedTrigger(const std::string &id, MSNet &net,
                        const std::vector<MSLane*> &destLanes,
                        const std::string &file) throw(ProcessError);

    /** destructor */
    ~GUILaneSpeedTrigger() throw();

    /// @name inherited from GUIGlObject
    //@{

    /// Returns an own popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent);

    /// returns the id of the object as known to microsim
    const std::string &microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;
    //@}


    /// @name inherited from GUIAbstractAddGlObject
    //@{

    /// Draws the detector in full-geometry mode
    void drawGL(SUMOReal scale, SUMOReal upscale);

    /// Returns the detector's coordinates
    Position2D getPosition() const;
    //@}

    Boundary getBoundary() const;

    GUIManipulator *openManipulator(GUIMainWindow &app,
                                    GUISUMOAbstractView &parent);

public:
class GUILaneSpeedTriggerPopupMenu : public GUIGLObjectPopupMenu
    {
        FXDECLARE(GUILaneSpeedTriggerPopupMenu)
    public:

        GUILaneSpeedTriggerPopupMenu(GUIMainWindow &app,
                                     GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUILaneSpeedTriggerPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

    protected:
        GUILaneSpeedTriggerPopupMenu() { }

    };

class GUIManip_LaneSpeedTrigger : public GUIManipulator
    {
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

    /// The information whether the speed shall be shown in m/s or km/h
    bool myShowAsKMH;

    /// Storage for last value to avoid string recomputation
    SUMOReal myLastValue;

    /// Storage for speed string to avoid recomputation
    std::string myLastValueString;

};


#endif

/****************************************************************************/

