#ifndef GUIApplicationWindow_h
#define GUIApplicationWindow_h
//---------------------------------------------------------------------------//
//                        GUIApplicationWindow.h -
//  Class for the main gui window
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
// Revision 1.3  2003/03/12 16:55:16  dkrajzew
// centering of objects debugged
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <qmainwindow.h>
#include <qworkspace.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class SUMOView;
class OptionsCont;
class GUINet;
class GUILoadThread;
class GUIRunThread;
class QSimulationLoadedEvent;
class QLabel;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * The main window of the SUMO-gui. Contains the file opening support and
 * a canvas to display the simulation representations in
 */
class GUIApplicationWindow: public QMainWindow
{
    // is a Q-OBJECT
    Q_OBJECT

public:
    /** constructor */
    GUIApplicationWindow();

    /** destructor */
    ~GUIApplicationWindow();

private slots:
    /** called from the menu, this method allows to choose a simulation
        to load and loads it */
    void load();

    /** this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /** starts the loaded simulation if any via the RunThread */
    void start();

    /** stops the loaded simulation if any via the RunThread */
    void stop();

    /** resumes the simulation, if any via the RunThread */
    void resume();

    /** performs a single simulation step via the RunThread */
    void singleStep();

    /** opens a new simulation display */
    void openNewWindow();

    /** shows the screen about SUMO */
    void about();

    /** shows the screen about Qt */
    void aboutQt();

    /** called before the sub-window menu is opened */
    void windowsMenuAboutToShow();

    /** !!! some methods for windows alignment */
    void windowsMenuActivated( int id );

public slots:
    /** sets the editable simulation delay (in milliseconds) */
    void setSimulationDelay(int value);

protected:
    /** called when an event occures */
    bool event(QEvent *e);

private:
    /** builds the file toolbar */
    void buildFileTools();

    /** builds the simulation toolbar */
    void buildSimulationTools();

    /** builds the window toobar (allows top open new window) */
    void buildWindowsTools();

    /** called when a net was loaded */
    void netLoaded(QSimulationLoadedEvent *ec);

    /// resets the simulation tool bar after a simulation was deleted
    void resetSimulationToolBar();

private:
    /** the name of the simulation */
    std::string _name;

    /** the workspace */
    QWorkspace* ws;

    /** the file, the simulation and the windowtoolbar */
    QToolBar *fileTools, *simTools, *_winTools;

    /** the windows menu */
    QPopupMenu* windowsMenu;

    /** the thread that loads simulations */
    GUILoadThread *_loadThread;

    /** the thread that runs simulations */
    GUIRunThread *_runThread;

    /** buttons to control the simulation process */
    QToolButton *_startSimButton, *_stopSimButton, *_resumeSimButton,
        *_singleStepButton;

    /** button for window adding */
    QToolButton *_windowAdder;

    /** the information whether the simulation was started before */
    bool _wasStarted;

    /** the label containing the current simulation step */
    QLabel *_simStepLabel;

    /// The current view number
    size_t myViewNumber;

    /// The id of the "load" menu entry 
    int _loadID;
    /// the pointer to the tool bar "load" entry
    QToolButton *_fileOpen;
    /// the menu holding the file operations
    QPopupMenu * _fileMenu;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIApplicationWindow.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

