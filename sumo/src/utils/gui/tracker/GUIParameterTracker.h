/****************************************************************************/
/// @file    GUIParameterTracker.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A window which displays the time line of one (or more) value(s)
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
#ifndef GUIParameterTracker_h
#define GUIParameterTracker_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <fx.h>
#include <fx3d.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include "TrackerValueDesc.h"
#include <guisim/GLObjectValuePassConnector.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>      /* OpenGL header file */
#endif // _WIN32


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class GUIParameterTracker : public FXMainWindow
{
    FXDECLARE(GUIParameterTracker)
public:
    enum {
        MID_AGGREGATIONINTERVAL = FXMainWindow::ID_LAST,
        MID_SAVE,
        ID_LAST
    };
    /// Constructor (one value is defined)
    GUIParameterTracker(GUIMainWindow &app, const std::string &name,
                        GUIGlObject &o, int xpos, int ypos);

    /// Constructor (the tracker is empty)
    GUIParameterTracker(GUIMainWindow &app, const std::string &name);

    /// Destructor
    ~GUIParameterTracker();

    /// Creates the window
    void create();

    /// Returns the information about the largest width allowed for openGL-windows
    int getMaxGLWidth() const;

    /// Returns the information about the largest height allowed for openGL-windows
    int getMaxGLHeight() const;

    /// Adds a further time line to display
    void addTracked(GUIGlObject &o, ValueSource<SUMOReal> *src,
                    TrackerValueDesc *newTracked);

    /// Called on window resizing
    long onConfigure(FXObject*,FXSelector,void*);

    /// Called if the window shall be repainted
    long onPaint(FXObject*,FXSelector,void*);

    /// Called on a simulation step
    long onSimStep(FXObject*,FXSelector,void*);

    /// Called when the aggregation interval (combo) has been changed
    long onCmdChangeAggregation(FXObject*,FXSelector,void*);

    /// Called when the data shall be saved
    long onCmdSave(FXObject*,FXSelector,void*);


protected:
    /// Adds a further variable to display
    void addVariable(GUIGlObject *o, const std::string &name, size_t recordBegin);

public:
    /**
     * @class GUIParameterTrackerPanel
     * This panel lies within the GUIParameterTracker being the main widget.
     * It is the widget responsible for displaying the information while
     * GUIParameterTracker only provides window-facilities.
     */
    class GUIParameterTrackerPanel : public FXGLCanvas
    {
        FXDECLARE(GUIParameterTrackerPanel)
    public:
        /// Constructor
        GUIParameterTrackerPanel(FXComposite *c, GUIMainWindow &app,
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
        void drawValue(TrackerValueDesc &desc, SUMOReal namePos);

        /// Applies the max and min of the value(t) to the current size of the panel
        SUMOReal patchHeightVal(TrackerValueDesc &desc, SUMOReal d);

    private:
        /// The parent window
        GUIParameterTracker *myParent;

        /// the sizes of the window
        int myWidthInPixels, myHeightInPixels;

        /// The main application
        GUIMainWindow *myApplication;

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
    GUIMainWindow *myApplication;

    /// Definition of the container for logged values
    typedef std::vector<TrackerValueDesc*> TrackedVarsVector;

    /// The list of tracked values
    TrackedVarsVector myTracked;

    /// The panel to display the values in
    GUIParameterTrackerPanel *myPanel;

    /// Definition of a list of value passing objects
    typedef std::vector<GLObjectValuePassConnector<SUMOReal>*> ValuePasserVector;

    /// The value sources
    ValuePasserVector myValuePassers;

    /// for some menu detaching fun
    FXToolBarShell *myToolBarDrag;

    /// A combo box to select an aggregation interval
    FXComboBox *myAggregationInterval;

    /// The simulation delay
    FXdouble myAggregationDelay;

    /// The tracker tool bar
    FXToolBar *myToolBar;

protected:
    /// Fox needs this
    GUIParameterTracker() { }

};


#endif

/****************************************************************************/

