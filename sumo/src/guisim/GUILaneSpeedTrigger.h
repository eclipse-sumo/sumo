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
// Revision 1.1  2004/07/02 08:55:10  dkrajzew
// visualisation of vss added
//
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
#include <gui/GUIGlObject.h>
#include <gui/GUIGlObject_AAManipulatable.h>
#include <utils/geom/HaveBoundery.h>
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
    public GUIGlObject_AAManipulatable {
public:
    /** constructor */
    GUILaneSpeedTrigger(const std::string &id, MSNet &net,
        const std::vector<MSLane*> &destLanes,
        const std::string &aXMLFilename);

    /** destructor */
    ~GUILaneSpeedTrigger();

    //@{ From GUIGlObject
    /// Returns an own popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(GUIApplicationWindow &app,
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

    double getDefaultSpeed() const;


    Boundery getBoundery() const;

    /// Returns the current speed
    double getCurrentSpeed() const;

    GUIManipulator *openManipulator(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    void setOverriding(bool val);

    void setOverridingValue(double val);

    double getLoadedSpeed();



protected:
    /** the implementation of the SAX-handler interface for reading
        element begins */
    void myStartElement(int element, const std::string &name,
        const Attributes &attrs);

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

    /// The original speed allowed on the lanes
    double myDefaultSpeed;

    /// The information whether the read speed shall be overridden
    bool myAmOverriding;

    /// The speed to use if overriding the read speed
    double mySpeedOverrideValue;

    /// The loaded speed
    double myLoadedSpeed;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

