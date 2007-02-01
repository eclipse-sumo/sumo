/****************************************************************************/
/// @file    GUITrafficLightLogicWrapper.h
/// @author  Daniel Krajzewicz
/// @date    Oct/Nov 2003
/// @version $Id: $
///
// A wrapper for tl-logics to allow their visualisation and interaction
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
#ifndef GUITrafficLightLogicWrapper_h
#define GUITrafficLightLogicWrapper_h
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/tracker/GUITLLogicPhasesTrackerWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObjectStorage;
class MSTrafficLightLogic;
class GUIMainWindow;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUITrafficLightLogicWrapper
 * This class is responsible for the visualisation of tl-logics and the
 *  interaction with them.
 */
class GUITrafficLightLogicWrapper : public GUIGlObject
{
public:
    /// Constructor
    GUITrafficLightLogicWrapper(GUIGlObjectStorage &idStorage,
                                MSTLLogicControl &control, MSTrafficLightLogic &tll);

    /// Destructor
    ~GUITrafficLightLogicWrapper();

    //@{ From GUIGlObject
    /// Returns the popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent);

    /// Returns the parameter window
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    const std::string &microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;

    /// Returns the boundary to which the object shall be centered
    Boundary getCenteringBoundary() const;
    //@}


    /// Builds a GUITLLogicPhasesTrackerWindow which will receive new phases
    void begin2TrackPhases();

    /// Builds a GUITLLogicPhasesTrackerWindow which displays the phase diagram
    void showPhases();

    /// Builds a GUITLLogicPhasesTrackerWindow which displays the phase diagram
    void switchTLSLogic(int to);

    /// Returns the index of the given link within the according tls
    int getLinkIndex(MSLink *link) const;

public:
    /**
     * @class GUITrafficLightLogicWrapperPopupMenu
     * The popup-menu for a TLS-logic. Adds the functionality to open a
     *  view on the tls-logic and to start tracking of the tls-logic.
     */
class GUITrafficLightLogicWrapperPopupMenu : public GUIGLObjectPopupMenu
    {
        FXDECLARE(GUITrafficLightLogicWrapperPopupMenu)
    public:
        /// Constructor
        GUITrafficLightLogicWrapperPopupMenu(GUIMainWindow &app,
                                             GUISUMOAbstractView &parent, GUIGlObject &o);

        /// Destructor
        ~GUITrafficLightLogicWrapperPopupMenu();

        /// Called if the phases shall be shown
        long onCmdShowPhases(FXObject*,FXSelector,void*);

        /// Called if the phases shall be begun to track
        long onCmdBegin2TrackPhases(FXObject*,FXSelector,void*);

        long onCmdSwitchTLSLogic(FXObject*,FXSelector,void*);

    protected:
        /// protected constructor for FOX
        GUITrafficLightLogicWrapperPopupMenu()
        { }

    };

private:
    /// Reference to the according tls
    MSTLLogicControl &myTLLogicControl;

    /// The wrapped tl-logic
    MSTrafficLightLogic &myTLLogic;

    /// The main application
    GUIMainWindow *myApp;

};


#endif

/****************************************************************************/

