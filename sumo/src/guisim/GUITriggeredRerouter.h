#ifndef GUITriggeredRerouter_h
#define GUITriggeredRerouter_h
//---------------------------------------------------------------------------//
//                        GUITriggeredRerouter.h -
//  The gui-version of MSTriggeredRerouter
//                           -------------------
//  begin                : Mon, 25.07.2005
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <utils/helpers/Command.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
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
class MSEdge;
class GUIManipulator;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUITriggeredRerouter
 * This is the gui-version of the MSTriggeredSource-object
 */
class GUITriggeredRerouter
    : public MSTriggeredRerouter,
    public GUIGlObject_AbstractAdd {
public:
    /** constructor */
    GUITriggeredRerouter(const std::string &id, MSNet &net,
        const std::vector<MSEdge*> &edges, SUMOReal prob,
        const std::string &aXMLFilename);

    /** destructor */
    ~GUITriggeredRerouter();

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
    class GUITriggeredRerouterPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUITriggeredRerouterPopupMenu)
    public:

        GUITriggeredRerouterPopupMenu(GUIMainWindow &app,
            GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUITriggeredRerouterPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

    protected:
        GUITriggeredRerouterPopupMenu() { }

    };


    class GUIManip_TriggeredRerouter : public GUIManipulator {
        FXDECLARE(GUIManip_TriggeredRerouter)
    public:
        enum {
            MID_USER_DEF = FXDialogBox::ID_LAST,
            MID_PRE_DEF,
            MID_OPTION,
            MID_CLOSE,
            ID_LAST
        };
        /// Constructor
        GUIManip_TriggeredRerouter(GUIMainWindow &app,
            const std::string &name, GUITriggeredRerouter &o,
            int xpos, int ypos);

        /// Destructor
        virtual ~GUIManip_TriggeredRerouter();

        long onCmdOverride(FXObject*,FXSelector,void*);
        long onCmdClose(FXObject*,FXSelector,void*);
        long onCmdUserDef(FXObject*,FXSelector,void*);
        long onUpdUserDef(FXObject*,FXSelector,void*);
        long onCmdChangeOption(FXObject*,FXSelector,void*);

    private:
        GUIMainWindow *myParent;

        FXRealSpinDial *myUsageProbabilityDial;

        FXint myChosenValue;

        FXDataTarget myChosenTarget;

        FXDataTarget myUsageProbabilityTarget;

        SUMOReal myUsageProbability;

        GUITriggeredRerouter *myObject;

    protected:
        GUIManip_TriggeredRerouter() { }

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

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

