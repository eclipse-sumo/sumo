#ifndef GUIParameterTracker_h
#define GUIParameterTracker_h
//---------------------------------------------------------------------------//
//                        GUIParameterTracker.h -
// A window which displays the time line of one (or more) value(s)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.3  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/06/05 11:38:47  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <qgl.h>
#include <qdialog.h>
#include <gui/GUIGlObject.h>
#include <utils/qutils/NewQMutex.h>
#include <utils/glutils/lfontrenderer.h>
#include "TrackerValueDesc.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class QPaintEvent;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIParameterTracker : public QDialog
{
    Q_OBJECT
public:
    /// Constructor (one value is defined)
    GUIParameterTracker( GUIApplicationWindow *app,
        GUIGlObject *o, size_t itemNo );

    /// Constructor (the tracker is empty)
    GUIParameterTracker( GUIApplicationWindow *app );

    /// Destructor
    ~GUIParameterTracker();

    /// Returns the information about the largest width allowed for openGL-windows
    int getMaxGLWidth() const;

    /// Returns the information about the largest height allowed for openGL-windows
    int getMaxGLHeight() const;

protected:
    /// Adds a further variable to display
    void addVariable( GUIGlObject *o, size_t itemNo );

    /// Callback for events
    bool event ( QEvent *e );

    /// Callback for the resize-event
    void resizeEvent ( QResizeEvent * );

    /// Callback for the paint-event
    void paintEvent ( QPaintEvent * );

private:
    /**
     * @class GUIParameterTrackerPanel
     * This panel lies within the GUIParameterTracker being the main widget.
     * It is the widget responsible for displaying the information while
     * GUIParameterTracker only provides window-facilities.
     */
    class GUIParameterTrackerPanel : public QGLWidget {
    public:
        /// Constructor
        GUIParameterTrackerPanel(GUIApplicationWindow *app,
            GUIParameterTracker *parent);

        /// Destructor
        ~GUIParameterTrackerPanel();

        /// needed to update
        friend class GUIParameterTracker;

    protected:
        /// derived from QGLWidget, this method initialises the openGL canvas
        void initializeGL();

        /// called when the canvas has been resized
        void resizeGL( int, int );

        /// performs the painting of the simulation
        void paintGL();

    private:
        /// Draws all values
        void drawValues();

        /// Draws a single value
        void drawValue(TrackerValueDesc &desc, float namePos);

        /// Applies the max and min of the value(t) to the current size of the panel
        float patchHeightVal(TrackerValueDesc &desc, double d);


    private:
        /// The parent window
        GUIParameterTracker &myParent;

        /// A lock for drawing operations
        NewQMutex _lock; // !!! (same as in abstract view)

        /// Information how many times the drawing method was called at once
        size_t _noDrawing;

        /// the sizes of the window
        int _widthInPixels, _heightInPixels;

        /// The openGL-font drawer
        LFontRenderer myFontRenderer;

        /// The main application
        GUIApplicationWindow *myApplication;

    };

public:
    /// the panel may change some things
    friend class GUIParameterTrackerPanel;

protected:
    /// The main application
    GUIApplicationWindow *myApplication;

    /// Definition of the container for logged values
    typedef std::vector<TrackerValueDesc*> TrackedVarsVector;

    /// The list of tracked values
    TrackedVarsVector myTracked;

    /// The panel to display the values in
    GUIParameterTrackerPanel *myPanel;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIParameterTracker.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

