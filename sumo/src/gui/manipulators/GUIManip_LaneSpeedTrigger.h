#ifndef GUIManip_LaneSpeedTrigger_h
#define GUIManip_LaneSpeedTrigger_h
//---------------------------------------------------------------------------//
//                        GUILaneDrawer_FGnT.h -
//  Manipulation GUI for variable speed signs
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
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
// Revision 1.1  2004/07/02 08:24:33  dkrajzew
// possibility to manipulate vss in the gui added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIManipulator.h"
#include <utils/foxtools/FXRealSpinDial.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIManip_LaneSpeedTrigger
 */
class GUIManip_LaneSpeedTrigger : public GUIManipulator {
    FXDECLARE(GUIManip_LaneSpeedTrigger)
public:
    enum {
        MID_USER_DEF = FXDialogBox::ID_LAST,
        MID_OPTION,
        MID_CLOSE,
        ID_LAST
    };
    /// Constructor
    GUIManip_LaneSpeedTrigger(GUIApplicationWindow &app,
        const std::string &name, GUIGlObject_AAManipulatable &o,
        int xpos, int ypos);

    /// Destructor
    virtual ~GUIManip_LaneSpeedTrigger();


    long onCmdOverride(FXObject*,FXSelector,void*);
    long onCmdClose(FXObject*,FXSelector,void*);
    long onCmdUserDef(FXObject*,FXSelector,void*);
    long onUpdUserDef(FXObject*,FXSelector,void*);
    long onCmdChangeOption(FXObject*,FXSelector,void*);

private:
    GUIApplicationWindow *myParent;

    FXRealSpinDial *myUserDefinedSpeed;

    FXint myChosenValue;

    FXDataTarget myChosenTarget;

    FXDataTarget mySpeedTarget;

    float mySpeed;

protected:
    GUIManip_LaneSpeedTrigger() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

