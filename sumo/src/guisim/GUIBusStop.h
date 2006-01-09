#ifndef GUIBusStop_h
#define GUIBusStop_h
//---------------------------------------------------------------------------//
//                        GUIBusStop.h -
//  GUI-version of MSBusStop
//                           -------------------
//  begin                : Wed, 07.12.2005
//  copyright            : (C) 2005 by DLR http://ivf.dlr.de/
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
// Revision 1.1  2006/01/09 11:53:00  dkrajzew
// bus stops implemented
//
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
#include <microsim/trigger/MSBusStop.h>
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
 * @class GUIBusStop
 * This is the gui-version of the MSLaneSpeedTrigger-object
 */
class GUIBusStop
    : public MSBusStop,
    public GUIGlObject_AbstractAdd {
public:
    /** constructor */
    GUIBusStop(const std::string &id, MSNet &net,
        const std::vector<std::string> &lines, MSLane &lane,
        SUMOReal frompos, SUMOReal topos);

    /** destructor */
    ~GUIBusStop();

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
    void drawGL_FG(SUMOReal scale, SUMOReal upscale);

    /// Draws the detector in simple-geometry mode
    void drawGL_SG(SUMOReal scale, SUMOReal upscale);

    /// Returns the detector's coordinates
    Position2D getPosition() const;
    //@}

    Boundary getBoundary() const;

    GUIManipulator *openManipulator(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

private:
    void doPaint(const Position2DVector &shape, const DoubleVector &rots,
        const DoubleVector &lengths, const Position2D &spos, SUMOReal scale, SUMOReal upscale);

public:
    class GUIBusStopPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUIBusStopPopupMenu)
    public:

        GUIBusStopPopupMenu(GUIMainWindow &app,
            GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUIBusStopPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

    protected:
        GUIBusStopPopupMenu() { }

    };
/*
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
            const std::string &name, GUIBusStop &o,
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

        GUIBusStop *myObject;

    protected:
        GUIManip_LaneSpeedTrigger() { }

    };
*/
private:
    /// The rotations of the shape parts (for full geometry)
    DoubleVector myFGShapeRotations;

    /// The lengths of the shape parts (for full geometry)
    DoubleVector myFGShapeLengths;

    /// The shape (for full geometry)
    Position2DVector myFGShape;

    /// The position of the sign (for full geometry)
    Position2D myFGSignPos;

    /// The rotations of the shape parts (for simple geometry)
    DoubleVector mySGShapeRotations;

    /// The lengths of the shape parts (for simple geometry)
    DoubleVector mySGShapeLengths;

    /// The shape (for simple geometry)
    Position2DVector mySGShape;

    /// The position of the sign (for simple geometry)
    Position2D mySGSignPos;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

