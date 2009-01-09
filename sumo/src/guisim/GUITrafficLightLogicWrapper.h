/****************************************************************************/
/// @file    GUITrafficLightLogicWrapper.h
/// @author  Daniel Krajzewicz
/// @date    Oct/Nov 2003
/// @version $Id$
///
// A wrapper for tl-logics to allow their visualisation and interaction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#ifdef _MSC_VER
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
                                MSTLLogicControl &control, MSTrafficLightLogic &tll) throw();

    /// Destructor
    ~GUITrafficLightLogicWrapper() throw();



    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the tls logic
     * @see GUIGlObject::getMicrosimID
     */
    const std::string &getMicrosimID() const throw();


    /** @brief Returns the type of the object as coded in GUIGlObjectType
     *
     * @return GLO_TLLOGIC (is a tls logic)
     * @see GUIGlObject::getType
     * @see GUIGlObjectType
     */
    GUIGlObjectType getType() const throw() {
        return GLO_TLLOGIC;
    }


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings &s) const throw();
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

        long onCmdSwitchTLS2Off(FXObject*,FXSelector,void*);
        long onCmdSwitchTLSLogic(FXObject*,FXSelector,void*);

    protected:
        /// protected constructor for FOX
        GUITrafficLightLogicWrapperPopupMenu() { }

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

