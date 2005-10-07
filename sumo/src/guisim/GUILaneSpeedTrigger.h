#ifndef GUILaneSpeedTrigger_h
#define GUILaneSpeedTrigger_h
//---------------------------------------------------------------------------//
//                        GUILaneSpeedTrigger.h -
//  Class that realises the setting of a lane's maximum speed triggered by
//      values read from a file
//                           -------------------
//  begin                : Mon, 26.04.2004
//  copyright            : (C) 2004 by DLR http://ivf.dlr.de/
//  author               : Daniel Krajzewicz
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 07:59:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.2  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.1  2004/07/02 08:55:10  dkrajzew
// visualisation of vss added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <utils/helpers/Command.h>
#include <microsim/trigger/MSLaneSpeedTrigger.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>
#include <gui/GUIManipulator.h>
#include <utils/foxtools/FXRealSpinDial.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSLane;
class GUIManipulator;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUILaneSpeedTrigger
 * This is the gui-version of the MSLaneSpeedTrigger-object
 */
class GUILaneSpeedTrigger
    : public MSLaneSpeedTrigger,
    public GUIGlObject_AbstractAdd {
public:
    /** constructor */
    GUILaneSpeedTrigger(const std::string &id, MSNet &net,
        const std::vector<MSLane*> &destLanes,
        const std::string &aXMLFilename);

    /** destructor */
    ~GUILaneSpeedTrigger();

    //@{ From GUIGlObject
    /// Returns an own popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;
    //@}

    //@{ From GUIAbstractAddGlObject
    /// Draws the detector in full-geometry mode
    void drawGL_FG(SUMOReal scale);

    /// Draws the detector in simple-geometry mode
    void drawGL_SG(SUMOReal scale);

    /// Returns the detector's coordinates
    Position2D getPosition() const;
    //@}

    Boundary getBoundary() const;

    GUIManipulator *openManipulator(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

public:
    class GUILaneSpeedTriggerPopupMenu : public GUIGLObjectPopupMenu {
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

        FXRealSpinDial *myUserDefinedSpeed;

        FXComboBox *myPredefinedValues;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        FXDataTarget mySpeedTarget;

        SUMOReal mySpeed;

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
    void doPaint(const PosCont &pos, const RotCont rot, SUMOReal scale);

private:
    /// The positions in full-geometry mode
    PosCont myFGPositions;

    /// The rotations in full-geometry mode
    RotCont myFGRotations;

    /// The positions in simple-geometry mode
    PosCont mySGPositions;

    /// The rotations in simple-geometry mode
    RotCont mySGRotations;

    /// The information whether the speed shall be shown in m/s or km/h
    bool myShowAsKMH;

    /// Storage for last value to avoid string recomputation
    SUMOReal myLastValue;

    /// Storage for speed string to avoid recomputation
    std::string myLastValueString;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

