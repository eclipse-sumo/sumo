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
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <helpers/Command.h>
#include <microsim/MSLaneSpeedTrigger.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/geom/HaveBoundary.h>
#include <utils/geom/Position2D.h>


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
 * GUILaneSpeedTrigger
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
    void drawGL_FG(double scale);

    /// Draws the detector in simple-geometry mode
    void drawGL_SG(double scale);

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

private:
    /// Definition of a positions container
    typedef std::vector<Position2D> PosCont;

    /// Definition of a rotation container
    typedef std::vector<double> RotCont;

private:
    void doPaint(const PosCont &pos, const RotCont rot, double scale);

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
    float myLastValue;

    /// Storage for speed string to avoid recomputation
    std::string myLastValueString;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

