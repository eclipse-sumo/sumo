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
// Revision 1.13  2003/07/16 15:16:26  dkrajzew
// unneeded uncommented files removed
//
// Revision 1.12  2003/07/07 08:08:33  dkrajzew
// The restart-button was removed and the play-button has now the function to continue the simulation if it has been started before
//
// Revision 1.11  2003/06/24 14:28:53  dkrajzew
// first steps towards a settings manipulation applied
//
// Revision 1.10  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on end and be started with a simulation now;
//
// Revision 1.9  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
//
// Revision 1.8  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.7  2003/04/16 10:12:12  dkrajzew
// fontrendeder removed temporarily
//
// Revision 1.6  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.5  2003/04/04 15:04:53  roessel
// Added #include <qtoolbutton.h>
//
// Revision 1.4  2003/03/17 14:03:24  dkrajzew
// Dialog about simulation restart debugged
//
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
#include <vector>
#include <iostream>
#include <qmainwindow.h>
#include <qworkspace.h>
#include <qtoolbutton.h>
#include <utils/glutils/FontStorage.h>

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
class QWidget;
class QSimulationEndedEvent;


/* =========================================================================
 * class definition
 * ======================================================================= */
/**
 * The main window of the SUMO-gui. Contains the file opening support and
 * a canvas to display the simulation representations in
 */
class GUIApplicationWindow:
    public QMainWindow/*,
    public MsgRetriever*/
{
    // is a Q-OBJECT
    Q_OBJECT

public:
    /** constructor */
    GUIApplicationWindow(int glWidth, int glHeight, bool quitOnEnd,
        const std::string &config);

    /** destructor */
    ~GUIApplicationWindow();

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    void addChild(QWidget *child, bool updateOnSimStep=true);

    void removeChild(QWidget *child);

private slots:
    /** called from the menu, this method allows to choose a simulation
        to load and loads it */
    void load();

    /** this method closes all windows and deletes the current simulation */
    void closeAllWindows();

    /** starts the loaded simulation if any exists, via the RunThread */
    void start();

    /** stops the loaded simulation if any exists, via the RunThread */
    void stop();

    /** performs a single simulation step via the RunThread */
    void singleStep();

    /** opens a new simulation display */
    void openNewWindow();

    /** shows the screen about SUMO */
    void about();

    /** shows the screen about Qt */
    void aboutQt();

    /// Shows the application settings dialog
    void appSettings();

    /// Shows the simulation settings dialog
    void simSettings();

    /** called before the sub-window menu is opened */
    void windowsMenuAboutToShow();

    /** called before the sub-window menu is opened */
    void settingsMenuAboutToShow();

    /** !!! some methods for windows alignment */
    void windowsMenuActivated( int id );

    /// Shows the settings for the window
    void windowSetings(int window);

public:
    /// The opengl-font renderer
    FontStorage myFonts;


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

    /// Builds the menu bar entry "File"
    void buildFileMenu();

    /// Builds the menu bar entry "Setting"
    void buildSettingsMenu();

    /// Builds the menu bar entry "Windows"
    void buildWindowsMenu();

    /// Builds the menu bar entry "Help"
    void buildHelpMenu();

    /** resumes the simulation, if any exists, via the RunThread */
    void resume();


    /** called when a net was loaded */
    void netLoaded(QSimulationLoadedEvent *ec);

    /// resets the simulation tool bar after a simulation was deleted
    void resetSimulationToolBar();

    /// Process an event which informaed about the end of the simulation
    void processSimulationEndEvent(QSimulationEndedEvent *e);


private:
    /** the name of the simulation */
    std::string _name;

    /** the workspace */
    QWorkspace* ws;

    /** the file, the simulation and the windowtoolbar */
    QToolBar *fileTools, *simTools, *_winTools;

    /** the windows and the settings menus */
    QPopupMenu *windowsMenu, *settingsMenu;

    /** the thread that loads simulations */
    GUILoadThread *_loadThread;

    /** the thread that runs simulations */
    GUIRunThread *_runThread;

    /** buttons to control the simulation process */
    QToolButton *_startSimButton, *_stopSimButton, //*_resumeSimButton,
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

    /// The openGL-maximum screen sizes
    int myGLWidth, myGLHeight;

    std::vector<QWidget*> mySubWindows;

    /// (y/n): the window shall be closed when the simulation has ended
    bool myQuitOnEnd;

    /// (y/n): the gui loads and starts a simulation at the beginning
    bool myStartAtBegin;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIApplicationWindow.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

