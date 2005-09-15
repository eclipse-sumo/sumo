#ifndef GUITLLogicPhasesTrackerWindow_h
#define GUITLLogicPhasesTrackerWindow_h
//---------------------------------------------------------------------------//
//                        GUITLLogicPhasesTrackerWindow.h -
//  A window displaying the phase diagram of a tl-logic
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
// Revision 1.5  2005/09/15 12:20:06  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 09:22:32  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/01 10:10:47  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.2  2005/01/27 14:34:20  dkrajzew
// added the possibility to display a complete phase
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.3  2004/03/19 12:42:31  dkrajzew
// porting to FOX
//
// Revision 1.2  2003/11/26 09:37:07  dkrajzew
// moving of the view when reaching the left border implemented; display of a
//  time scale implemented
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
#include <bitset>
#include <utility>
#include <fx.h>
#include <fx3d.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <utils/helpers/ValueRetriever.h>
#include <guisim/guilogging/GLObjectValuePassConnector.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/foxtools/FXMutex.h>
#include <utils/common/SUMOTime.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIMainWindow;
class MSTrafficLightLogic;
class GUITrafficLightLogicWrapper;


/* =========================================================================
 * type definitions
 * ======================================================================= */
/** @brief Definition of a phase description
    Within the first bitsets, all links having gree are marked, within the second
    all links having yellow */
typedef std::pair<std::bitset<64>, std::bitset<64> > SimplePhaseDef;

/// Definition of a complete phase information, including the time
typedef std::pair<SUMOTime, SimplePhaseDef> CompletePhaseDef;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUITLLogicPhasesTrackerWindow
 * This window displays a phase diagram for a chosen tl-logic.
 */
class GUITLLogicPhasesTrackerWindow
        : public FXMainWindow,
          public ValueRetriever<CompletePhaseDef> {
    FXDECLARE(GUITLLogicPhasesTrackerWindow)
public:
    /// Constructor to track current phases
    GUITLLogicPhasesTrackerWindow(GUIMainWindow &app,
        MSTrafficLightLogic &logic, GUITrafficLightLogicWrapper &wrapper,
        ValueSource<CompletePhaseDef> *src);

    /// Constructor to show the complete phase diagram
    GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
        GUIMainWindow &app,
        MSTrafficLightLogic &logic, GUITrafficLightLogicWrapper &wrapper,
        const MSSimpleTrafficLightLogic::Phases &phases);

    /// Destructor
    ~GUITLLogicPhasesTrackerWindow();

    /// Returns the information about the largest width allowed for openGL-windows
    int getMaxGLWidth() const;

    /// Returns the information about the largest height allowed for openGL-windows
    int getMaxGLHeight() const;

    /// Adds a further phase definition
    void addValue(CompletePhaseDef def);

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
    typedef std::vector<SimplePhaseDef> PhasesVector;

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

        /// A lock for drawing operations
        FXEX::FXMutex _lock; // !!! (same as in abstract view)

        /// the sizes of the window
        int _widthInPixels, _heightInPixels;

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
    /** @brief Computes the offsets that determine te first drawn item
        The first offset is the index of the first item to draw within
        the vector; the second offset is the offsets in pixels */
    void computeOffsets(size_t width, size_t firstXPixelOffset);

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
    FXEX::FXMutex myLock;

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
    GLObjectValuePassConnector<CompletePhaseDef> *myConnector;

    /// Information whether the tracking mode is on
    bool myAmInTrackingMode;

protected:
    /// protected constructor for FOX
    GUITLLogicPhasesTrackerWindow() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
