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
// Revision 1.2  2003/11/26 09:37:07  dkrajzew
// moving of the view when reaching the left border implemented; display of a time scale implemented
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
#include <bitset>
#include <utility>
#include <qgl.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <microsim/MSNet.h>
#include <helpers/ValueRetriever.h>
#include <utils/qutils/NewQMutex.h>
#include <utils/glutils/lfontrenderer.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIApplicationWindow;
class MSTrafficLightLogic;


/* =========================================================================
 * type definitions
 * ======================================================================= */
/** @brief Definition of a phase description
    Within the first bitsets, all links having gree are marked, within the second
    all links having yellow */
typedef std::pair<std::bitset<64>, std::bitset<64> > SimplePhaseDef;

/// Definition of a complete phase information, including the time
typedef std::pair<MSNet::Time, SimplePhaseDef> CompletePhaseDef;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUITLLogicPhasesTrackerWindow
 * This window displays a phase diagram for a chosen tl-logic.
 */
class GUITLLogicPhasesTrackerWindow
        : public QMainWindow,
          public ValueRetriever<CompletePhaseDef> {

    /// is a q-object
    Q_OBJECT
public:
    /// Constructor
    GUITLLogicPhasesTrackerWindow(GUIApplicationWindow &app,
        MSTrafficLightLogic &logic);

    /// Destructor
    ~GUITLLogicPhasesTrackerWindow();

    /// Returns the information about the largest width allowed for openGL-windows
    int getMaxGLWidth() const;

    /// Returns the information about the largest height allowed for openGL-windows
    int getMaxGLHeight() const;

    /// Adds a further phase definition
    void addValue(CompletePhaseDef def);

protected:
    /// Callback for events
    bool event ( QEvent *e );

    /// Callback for the resize-event
    void resizeEvent ( QResizeEvent * );

    /// Callback for the paint-event
    void paintEvent ( QPaintEvent * );

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
    class GUITLLogicPhasesTrackerPanel : public QGLWidget {
    public:
        /// Constructor
        GUITLLogicPhasesTrackerPanel(GUIApplicationWindow &app,
            GUITLLogicPhasesTrackerWindow &parent);

        /// Destructor
        ~GUITLLogicPhasesTrackerPanel();

        /// Returns the height
        size_t getHeightInPixels() const;

        /// Returns the width
        size_t getWidthInPixels() const;

        /// needed to update
        friend class GUITLLogicPhasesTrackerWindow;

    protected:
        /// derived from QGLWidget, this method initialises the openGL canvas
        void initializeGL();

        /// called when the canvas has been resized
        void resizeGL( int, int );

        /// performs the painting of the simulation
        void paintGL();

    private:
        /// The parent window
        GUITLLogicPhasesTrackerWindow &myParent;

        /// A lock for drawing operations
        NewQMutex _lock; // !!! (same as in abstract view)

        /// Information how many times the drawing method was called at once
        size_t _noDrawing;

        /// the sizes of the window
        int _widthInPixels, _heightInPixels;

        /// The main application
        GUIApplicationWindow &myApplication;
    };

public:
    /// Draws all values
    void drawValues(GUITLLogicPhasesTrackerPanel &caller);

    /// Sets the fonts used
    void setFontRenderer(GUITLLogicPhasesTrackerPanel &caller);

private:
    /** @brief Computes the offsets that determine te first drawn item
        The first offset is the index of the first item to draw within
        the vector; the second offset is the offsets in pixels */
    void computeOffsets(size_t width, size_t firstXPixelOffset);

private:
    /// The main application
    GUIApplicationWindow &myApplication;

    /// The logic to display
    MSTrafficLightLogic &myTLLogic;

    /// The list of phases
    PhasesVector myPhases;

    /// The list of phase durations
    DurationsVector myDurations;

    /// The panel to draw on
    GUITLLogicPhasesTrackerPanel *myPanel;

    /// A lock to avoid addition of new values while drawing
    NewQMutex myLock;

    /** @brief The names of links
        This holds an enumeration only - used to avoid time consuming
        string representation of ints */
    std::vector<std::string> myLinkNames;

    /// The openGL-font drawer
    LFontRenderer myFontRenderer;

    /// The index of the first phase that fits into the window
    size_t myFirstPhase2Show;

    /// The offset to draw the first phase (left offset)
    size_t myFirstPhaseOffset;

    /// The time the diagram begins at
    MSNet::Time myFirstTime2Show;

    /// The first time a phase was added at
    MSNet::Time myBeginTime;

    /// The last time a phase was added at
    MSNet::Time myLastTime;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
