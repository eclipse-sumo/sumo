/****************************************************************************/
/// @file    GUIE1VehicleActor.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id$
///
// GUI-version of MSBusStop
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
#ifndef GUIE1VehicleActor_h
#define GUIE1VehicleActor_h


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
#include <utils/common/Command.h>
#include <microsim/trigger/MSE1VehicleActor.h>
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
 * @class GUIE1VehicleActor
 * This is the gui-version of the MSLaneSpeedTrigger-object
 */
class GUIE1VehicleActor
            : public MSE1VehicleActor,
            public GUIGlObject_AbstractAdd
{
public:
    /** constructor */
    GUIE1VehicleActor(const std::string& id, MSLane* lane,
                      SUMOReal positionInMeters, unsigned int laid,
                      unsigned int cellid, unsigned int type) throw();

    /** destructor */
    ~GUIE1VehicleActor() throw();

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
class GUIE1VehicleActorPopupMenu : public GUIGLObjectPopupMenu
    {
        FXDECLARE(GUIE1VehicleActorPopupMenu)
    public:

        GUIE1VehicleActorPopupMenu(GUIMainWindow &app,
                                   GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUIE1VehicleActorPopupMenu();

        /** @brief Called if the object's manipulator shall be shown */
        long onCmdOpenManip(FXObject*,FXSelector,void*);

    protected:
        GUIE1VehicleActorPopupMenu() { }

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
                const std::string &name, GUIE1VehicleActor &o,
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

            GUIE1VehicleActor *myObject;

        protected:
            GUIManip_LaneSpeedTrigger() { }

        };
    */
private:
    /// The position in full-geometry mode
    Position2D myFGPosition;

    /// The rotation in full-geometry mode
    SUMOReal myFGRotation;

    /// The position in simple-geometry mode
    Position2D mySGPosition;

    /// The rotation in simple-geometry mode
    SUMOReal mySGRotation;

    /// The position on the lane
    SUMOReal myPosition;

};


#endif

/****************************************************************************/

