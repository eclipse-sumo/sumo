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
// Revision 1.9  2004/07/02 08:26:11  dkrajzew
// aggregation debugged and saving option added
//
// Revision 1.8  2004/03/19 12:42:59  dkrajzew
// porting to FOX
//
// Revision 1.7  2003/11/12 14:08:24  dkrajzew
// clean up after recent changes
//
// Revision 1.6  2003/11/11 08:43:04  dkrajzew
// synchronisation problems of parameter tracker updates patched
//
// Revision 1.5  2003/07/30 12:51:42  dkrajzew
// bugs on resize and font loading partially patched
//
// Revision 1.4  2003/07/30 08:50:42  dkrajzew
// tracker debugging (not yet completed)
//
// Revision 1.3  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/06/05 11:38:47  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <fx.h>
#include <fx3d.h>
#include <gui/GUIGlObject.h>
#include <utils/foxtools/FXMutex.h>
#include "TrackerValueDesc.h"
#include <guisim/guilogging/GLObjectValuePassConnector.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>      /* OpenGL header file */
#endif // _WIN32


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUIParameterTracker : public FXMainWindow
{
    FXDECLARE(GUIParameterTracker)
public:
    enum {
        MID_AGGREGATIONINTERVAL = FXMainWindow::ID_LAST,
        ID_LAST
    };
    /// Constructor (one value is defined)
    GUIParameterTracker( GUIApplicationWindow &app, const std::string &name,
        GUIGlObject &o, int xpos, int ypos);

    /// Constructor (the tracker is empty)
    GUIParameterTracker(GUIApplicationWindow &app);

    /// Destructor
    ~GUIParameterTracker();

    /// Creates the window
    void create();

    /// Returns the information about the largest width allowed for openGL-windows
    int getMaxGLWidth() const;

    /// Returns the information about the largest height allowed for openGL-windows
    int getMaxGLHeight() const;

    /// Adds a further time line to display
    void addTracked(GUIGlObject &o, ValueSource<double> *src,
        TrackerValueDesc *newTracked);

    /// Called on window resizing
    long onConfigure(FXObject*,FXSelector,void*);

    /// Called if the window shall be repainted
    long onPaint(FXObject*,FXSelector,void*);

    /// Called on a simulation step
    long onSimStep(FXObject*,FXSelector,void*);

    /// Called when the aggregation interval (combo) has been changed
    long onCmdChangeAggregation(FXObject*,FXSelector,void*);

protected:
    /// Adds a further variable to display
    void addVariable(GUIGlObject *o, const std::string &name);

public:
    /**
     * @class GUIParameterTrackerPanel
     * This panel lies within the GUIParameterTracker being the main widget.
     * It is the widget responsible for displaying the information while
     * GUIParameterTracker only provides window-facilities.
     */
    class GUIParameterTrackerPanel : public FXGLCanvas {
        FXDECLARE(GUIParameterTrackerPanel)
    public:
        /// Constructor
        GUIParameterTrackerPanel(FXComposite *c, GUIApplicationWindow &app,
            GUIParameterTracker &parent);

        /// Destructor
        ~GUIParameterTrackerPanel();

        /// needed to update
        friend class GUIParameterTracker;

        /// Called on window resizing
        long onConfigure(FXObject*,FXSelector,void*);

        /// Called if the window shall be repainted
        long onPaint(FXObject*,FXSelector,void*);

        /// Called on a simulation step
        long onSimStep(FXObject*sender,FXSelector,void*);

    private:
        /// Draws all values
        void drawValues();

        /// Draws a single value
        void drawValue(TrackerValueDesc &desc, float namePos);

        /// Applies the max and min of the value(t) to the current size of the panel
        float patchHeightVal(TrackerValueDesc &desc, double d);

    private:
        /// The parent window
        GUIParameterTracker *myParent;

        /// A lock for drawing operations
        FXEX::FXMutex _lock; // !!! (same as in abstract view)

        /// the sizes of the window
        int _widthInPixels, _heightInPixels;

        /// The main application
        GUIApplicationWindow *myApplication;

    protected:
        /// FOX needs this
        GUIParameterTrackerPanel() { }
    };

public:
    /// the panel may change some things
    friend class GUIParameterTrackerPanel;

private:
    /// Builds the tool bar
    void buildToolBar();


protected:
    /// The main application
    GUIApplicationWindow *myApplication;

    /// Definition of the container for logged values
    typedef std::vector<TrackerValueDesc*> TrackedVarsVector;

    /// The list of tracked values
    TrackedVarsVector myTracked;

    /// The panel to display the values in
    GUIParameterTrackerPanel *myPanel;

    /// Definition of a list of value passing objects
    typedef std::vector<GLObjectValuePassConnector<double>*> ValuePasserVector;

    /// The value sources
    ValuePasserVector myValuePassers;

    /// for some menu detaching fun
    FXToolBarShell *myToolBarDrag;

    /// A combo box to select an aggregation interval
    FXComboBox *myAggregationInterval;

    /// The simulation delay
    FXdouble myAggregationDelay;

    /// The application tool bar
    FXToolBar *myToolBar;

protected:
    /// Fox needs this
    GUIParameterTracker() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

