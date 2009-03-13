/****************************************************************************/
/// @file    GUITLLogicPhasesTrackerWindow.h
/// @author  Daniel Krajzewicz
/// @date    Oct/Nov 2003
/// @version $Id$
///
// A window displaying the phase diagram of a tl-logic
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
#ifndef GUITLLogicPhasesTrackerWindow_h
#define GUITLLogicPhasesTrackerWindow_h


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
#include <bitset>
#include <utility>
#include <fx.h>
#include <fx3d.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/common/ValueRetriever.h>
#include <guisim/GLObjectValuePassConnector.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/common/SUMOTime.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/MFXMutex.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class GUIMainWindow;
class MSTrafficLightLogic;
class GUITrafficLightLogicWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUITLLogicPhasesTrackerWindow
 * @brief This window displays a phase diagram for a chosen tl-logic.
 */
class GUITLLogicPhasesTrackerWindow
        : public FXMainWindow,
            public ValueRetriever<std::pair<SUMOTime, MSPhaseDefinition> > {
    FXDECLARE(GUITLLogicPhasesTrackerWindow)
public:
    /// Constructor to track current phases
    GUITLLogicPhasesTrackerWindow(GUIMainWindow &app,
                                  MSTrafficLightLogic &logic, GUITrafficLightLogicWrapper &wrapper,
                                  ValueSource<std::pair<SUMOTime, MSPhaseDefinition> > *src);

    /// Constructor to show the complete phase diagram
    GUITLLogicPhasesTrackerWindow(
        GUIMainWindow &app,
        MSTrafficLightLogic &logic, GUITrafficLightLogicWrapper &wrapper,
        const MSSimpleTrafficLightLogic::Phases &phases);

    /// Destructor
    ~GUITLLogicPhasesTrackerWindow();

    /// Creates the window (FOX-Toolkit)
    void create();

    /// Returns the information about the largest width allowed for openGL-windows
    int getMaxGLWidth() const;

    /// Returns the information about the largest height allowed for openGL-windows
    int getMaxGLHeight() const;

    /// Adds a further phase definition
    void addValue(std::pair<SUMOTime, MSPhaseDefinition>  def);

    /// Sets the time the display shall be shown as beginning at
    void setBeginTime(SUMOTime time);

    //{
    /// called on size change
    long onConfigure(FXObject *sender, FXSelector sel, void *data);

    /// called if the widget shall be repainted
    long onPaint(FXObject *sender, FXSelector sel, void *data);

    /// called on a simulation step
    long onSimStep(FXObject *sender, FXSelector sel, void *data);
    //}

public:
    /// Definition of a storage for phases
    typedef std::vector<MSPhaseDefinition> PhasesVector;

    /** @brief This list of stored phases
        Only new phases are stored; The times are not stored at all, they
        are just used to steer the output */
    typedef std::vector<size_t> DurationsVector;

    /**
     * @class GUITLLogicPhasesTrackerPanel
     * The canvas for the visualisation.
     * The drawing itself id done by the parent.
     */
    class GUITLLogicPhasesTrackerPanel : public FXGLCanvas {
        FXDECLARE(GUITLLogicPhasesTrackerPanel)
    public:
        /// Constructor
        GUITLLogicPhasesTrackerPanel(FXComposite *c,
                                     GUIMainWindow &app, GUITLLogicPhasesTrackerWindow &parent);

        /// Destructor
        ~GUITLLogicPhasesTrackerPanel();

        /// Returns the height
        size_t getHeightInPixels() const;

        /// Returns the width
        size_t getWidthInPixels() const;

        /// needed to update
        friend class GUITLLogicPhasesTrackerWindow;

        //{
        /// called on size change
        long onConfigure(FXObject*,FXSelector,void*);

        /// called if the widget shall be repainted
        long onPaint(FXObject*,FXSelector,void*);
        //}

    private:
        /// The parent window
        GUITLLogicPhasesTrackerWindow *myParent;

        /// the sizes of the window
        int myWidthInPixels, myHeightInPixels;

        /// The main application
        GUIMainWindow *myApplication;

    protected:
        /// protected constructor for FOX
        GUITLLogicPhasesTrackerPanel() { }

    };

public:
    /// Draws all values
    void drawValues(GUITLLogicPhasesTrackerPanel &caller);

private:
    /// The main application
    GUIMainWindow *myApplication;

    /// The logic to display
    MSTrafficLightLogic *myTLLogic;

    /// The list of phases
    PhasesVector myPhases;

    /// The list of phase durations
    DurationsVector myDurations;

    /// The panel to draw on
    GUITLLogicPhasesTrackerPanel *myPanel;

    /// A lock to avoid addition of new values while drawing
    MFXMutex myLock;

    /** @brief The names of links
        This holds an enumeration only - used to avoid time consuming
        string representation of ints */
    std::vector<std::string> myLinkNames;

    /// The index of the first phase that fits into the window
    size_t myFirstPhase2Show;

    /// The offset to draw the first phase (left offset)
    size_t myFirstPhaseOffset;

    /// The time the diagram begins at
    SUMOTime myFirstTime2Show;

    /// The first time a phase was added at
    SUMOTime myBeginTime;

    /// The last time a phase was added at
    SUMOTime myLastTime;

    /// The connector for retrival of further phases
    GLObjectValuePassConnector<std::pair<SUMOTime, MSPhaseDefinition> > *myConnector;

    /// Information whether the tracking mode is on
    bool myAmInTrackingMode;

    /// The tool bar drag (tracking mode)
    FXToolBarShell *myToolBarDrag;

    /// The tool bar (tracking mode)
    FXToolBar *myToolBar;

    /// The offset changer (tracking mode)
    FXRealSpinDial *myBeginOffset;

protected:
    /// protected constructor for FOX
    GUITLLogicPhasesTrackerWindow() { }

};


#endif

/****************************************************************************/

