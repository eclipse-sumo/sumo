#ifndef GUITrafficLightLogicWrapper_h
#define GUITrafficLightLogicWrapper_h
//---------------------------------------------------------------------------//
//                        GUITrafficLightLogicWrapper.h -
//  A wrapper for tl-logics to allow their visualisation and interaction
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct/Nov 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.4  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.3  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/11/26 09:48:58  dkrajzew
// time display added to the tl-logic visualisation
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/tracker/GUITLLogicPhasesTrackerWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;
class MSTrafficLightLogic;
class GUIMainWindow;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * @class GUITrafficLightLogicWrapper
 * This class is responsible for the visualisation of tl-logics and the
 *  interaction with them.
 */
class GUITrafficLightLogicWrapper
    : public GUIGlObject {
public:
    /// Constructor
    GUITrafficLightLogicWrapper(GUIGlObjectStorage &idStorage,
        MSTrafficLightLogic &tll);

    /// Destructor
    ~GUITrafficLightLogicWrapper();

    /// Returns an own popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the information whether this object is still active
	bool active() const;

    /// Returns the current phase definition
    CompletePhaseDef getPhaseDef() const;

    /// Builds a GUITLLogicPhasesTrackerWindow which displays the phase diagram
    void showPhases();

	//{
	Boundary getCenteringBoundary() const;
	//}

public:
    class GUITrafficLightLogicWrapperPopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUITrafficLightLogicWrapperPopupMenu)
    public:

        GUITrafficLightLogicWrapperPopupMenu(GUIMainWindow &app,
            GUISUMOAbstractView &parent, GUIGlObject &o);

        ~GUITrafficLightLogicWrapperPopupMenu();

        /// Called if the phases shall be shown
        long onCmdShowPhases(FXObject*,FXSelector,void*);


    protected:
        GUITrafficLightLogicWrapperPopupMenu() { }

    };

private:
    /// The wrapped tl-logic
    MSTrafficLightLogic &myTLLogic;

    /// The main application
    GUIMainWindow *myApp;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
