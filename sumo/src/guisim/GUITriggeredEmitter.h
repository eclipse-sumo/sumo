#ifndef GUITriggeredEmitter_h
#define GUITriggeredEmitter_h
//---------------------------------------------------------------------------//
//                        GUITriggeredEmitter.h -
//  The gui-version of MSTriggeredEmitter
//                           -------------------
//  begin                : Thu, 21.07.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
// Revision 1.3  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/09/09 12:50:30  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2005/08/01 13:06:51  dksumo
// further triggers added
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
#include <microsim/trigger/MSTriggeredEmitter.h>
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
 * @class GUITriggeredEmitter
 * This is the gui-version of the MSTriggeredSource-object
 */
class GUITriggeredEmitter
    : public MSTriggeredEmitter,
    public GUIGlObject_AbstractAdd {
public:
    /** constructor */
    GUITriggeredEmitter(const std::string &id, MSNet &net,
        MSLane* destLanes, SUMOReal pos,
        const std::string &aXMLFilename);

    /** destructor */
    ~GUITriggeredEmitter();

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
    class GUITriggeredEmitterPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUITriggeredEmitterPopupMenu)
    public:

        GUITriggeredEmitterPopupMenu(GUIMainWindow &app,
            GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUITriggeredEmitterPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

    protected:
        GUITriggeredEmitterPopupMenu() { }

    };

    class GUIManip_TriggeredEmitter : public GUIManipulator {
        FXDECLARE(GUIManip_TriggeredEmitter)
    public:
        enum {
            MID_USER_DEF = FXDialogBox::ID_LAST,
            MID_PRE_DEF,
            MID_OPTION,
            MID_CLOSE,
            ID_LAST
        };
        /// Constructor
        GUIManip_TriggeredEmitter(GUIMainWindow &app,
            const std::string &name, GUITriggeredEmitter &o,
            int xpos, int ypos);

        /// Destructor
        virtual ~GUIManip_TriggeredEmitter();

        long onCmdOverride(FXObject*,FXSelector,void*);
        long onCmdClose(FXObject*,FXSelector,void*);
        long onCmdUserDef(FXObject*,FXSelector,void*);
        long onUpdUserDef(FXObject*,FXSelector,void*);
        long onCmdChangeOption(FXObject*,FXSelector,void*);

    private:
        GUIMainWindow *myParent;

        FXRealSpinDial *myFlowFactorDial;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        FXDataTarget myFlowFactorTarget;

        SUMOReal myFlowFactor;

        GUITriggeredEmitter *myObject;

    protected:
        GUIManip_TriggeredEmitter() { }

    };

private:
    void doPaint(const Position2D &pos, SUMOReal rot, SUMOReal scale);

private:
    /// The positions in full-geometry mode
    Position2D myFGPosition;

    /// The rotations in full-geometry mode
    SUMOReal myFGRotation;

    /// The positions in simple-geometry mode
    Position2D mySGPosition;

    /// The rotations in simple-geometry mode
    SUMOReal mySGRotation;

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

