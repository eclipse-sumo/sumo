//---------------------------------------------------------------------------//
//                        GUIApplicationWindow.cpp
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.21  2003/08/20 11:55:49  dkrajzew
// "Settings"-menu added
//
// Revision 1.20  2003/08/14 13:41:33  dkrajzew
// a lower priorised update-method is now used
//
// Revision 1.19  2003/07/30 12:50:22  dkrajzew
// subwindows do not close at main window closing-bug patched
//
// Revision 1.18  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.17  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.16  2003/07/16 15:16:26  dkrajzew
// unneeded uncommented files removed
//
// Revision 1.15  2003/07/07 08:08:33  dkrajzew
// The restart-button was removed and the play-button has now the function to continue the simulation if it has been started before
//
// Revision 1.14  2003/06/24 14:28:53  dkrajzew
// first steps towards a settings manipulation applied
//
// Revision 1.13  2003/06/19 10:56:03  dkrajzew
// user information about simulation ending added; the gui may shutdown on end and be started with a simulation now;
//
// Revision 1.12  2003/06/18 11:04:22  dkrajzew
// new error processing adapted; new usage of fonts adapted
//
// Revision 1.11  2003/06/05 06:26:15  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.10  2003/05/21 15:15:40  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.9  2003/05/20 13:05:33  dkrajzew
// temporary font patch
//
// Revision 1.8  2003/05/20 09:23:53  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.7  2003/04/16 10:12:11  dkrajzew
// fontrendeder removed temporarily
//
// Revision 1.6  2003/04/16 09:50:03  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.5  2003/03/17 14:03:23  dkrajzew
// Dialog about simulation restart debugged
//
// Revision 1.4  2003/03/12 16:55:14  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/02/07 10:34:14  dkrajzew
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
#include <sstream>
#include <algorithm>
#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qvbox.h>
#include <qstatusbar.h>
#include <qworkspace.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qdialog.h>
#include <qwhatsthis.h>
#include <qworkspace.h>
#include <qspinbox.h>
#include <qmessagebox.h>
#include <qlabel.h>

#include <guisim/GUINet.h>
#include <utils/convert/ToString.h>

#include <utils/fonts/arial11.h>
#include <utils/fonts/arial10.h>
#include <utils/fonts/arial9.h>
#include <utils/fonts/arial8.h>
#include <utils/fonts/arial7.h>
#include <utils/fonts/arial6.h>
#include <utils/fonts/arial5.h>

#include <sumo_version.h>

#include "GUISUMOViewParent.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "GUIApplicationWindow.h"
#include "QSimulationStepEvent.h"
#include "QSimulationLoadedEvent.h"
#include "QSimulationEndedEvent.h"
#include "QMessageEvent.h"
#include "GUIEvents.h"
#include "qdialogs/QAboutSUMO.h"
#include "qdialogs/QApplicationSettings.h"
#include "qdialogs/QSimulationSettings.h"
#include "qdialogs/QMicroscopicViewSettings.h"
#include "icons/filesave.xpm"
#include "icons/fileopen.xpm"
#include "icons/play.xpm"
#include "icons/stop.xpm"
//#include "icons/cont.xpm"
#include "icons/step.xpm"
#include "icons/new_window.xpm"

#ifndef WIN32
#include "GUIApplicationWindow.moc"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * window hint definitions
 * ======================================================================= */
const char * fileOpenText = "Click this button to open a <em>new file</em>. <br><br>"
"You can also select the <b>Open command</b> from the File menu.";

const char * startSimText = "Click this button to start the loaded simulation. <br><br>"
"You can also select the <b>Start Simulation</b> from the Simulation menu.";

const char * stopSimText = "Click this button to interupt the running simulation. <br><br>"
"You can also select the <b>Stop Simulation</b> from the Simulation menu.";

/*const char * resumeSimText = "Click this button to resume a stopped simulation. <br><br>"
"You can also select the <b>Resume Simulation</b> from the Simulation menu.";
*/
const char * singleSimStepText = "Click this button to perform a single simulation step. <br><br>"
"You can also select the <b>Single Step</b> from the Simulation menu.";


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIApplicationWindow::GUIApplicationWindow(int glWidth, int glHeight,
                                           bool quitOnEnd,
                                           const std::string &config)
    : QMainWindow( 0, "example application main window", WDestructiveClose ),
    _loadThread(0), _runThread(0),
    myGLWidth(glWidth), myGLHeight(glHeight),
    myQuitOnEnd(quitOnEnd), myStartAtBegin(false)
{
    // recheck the maximum sizes
    QWidget *d = QApplication::desktop();
    myGLWidth = myGLWidth < d->width() ? myGLWidth : d->width();
    myGLHeight = myGLHeight < d->height() ? myGLHeight : d->height();

    // build additional threads
    _loadThread = new GUILoadThread(this);
    _runThread = new GUIRunThread(this, 1);

    // initialise font drawing
    myFonts.add("std11", arial11);
    myFonts.add("std10", arial10);
    myFonts.add("std9", arial9);
    myFonts.add("std8", arial8);
    myFonts.add("std7", arial7);
    myFonts.add("std6", arial6);
    myFonts.add("std5", arial5);

    // build tool bars
    buildFileTools();
    buildSimulationTools();
    buildWindowsTools();

    // build menu bar
    buildFileMenu();
    buildSettingsMenu();
    buildWindowsMenu();
    buildHelpMenu();

    // make the window a mdi-window
    QVBox* vb = new QVBox( this );
    vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    ws = new QWorkspace( vb );
    setCentralWidget( vb );

    // set the status bar
    statusBar()->message( "Ready", 2000 );

    // set the caption
    string caption = string("SUMO ") + string(version)
        + string(" - no simulation loaded");
    setCaption( caption.c_str());

    // start the simulation-thread
    _runThread->start();

    // check whether a simulation shall be started on begin
    if(config!="") {
        _fileOpen->setEnabled(FALSE);
        _fileMenu->setItemEnabled(_loadID, FALSE);
        _loadThread->init(config);
        _loadThread->start();
        statusBar()->message(
            QString("Loading '") + config.c_str() + QString("'"));
        myStartAtBegin = true;
    }
}


GUIApplicationWindow::~GUIApplicationWindow()
{
    _runThread->prepareDestruction();
    size_t i;
    for(i=0; i<mySubWindows.size(); i++) {
        mySubWindows[i]->close();
/*        QApplication::postEvent( mySubWindows[i],
            new QCloseEvent());*/
    }
    delete _loadThread;
    for(i=0; i<mySubWindows.size(); i++) {
        while(mySubWindows[i]->isVisible());
        delete mySubWindows[i];
    }
    //
    _runThread->wait();
    delete _runThread;
}


void
GUIApplicationWindow::buildFileTools()
{
    QPixmap openIcon, saveIcon;

    fileTools = new QToolBar( this, "file operations" );
    addToolBar( fileTools, tr( "File Operations" ), Top, TRUE );

    openIcon = QPixmap( fileopen );
    _fileOpen = new QToolButton( openIcon, "Open File",
        QString::null, this, SLOT(load()), fileTools, "open file" );
    QWhatsThis::add( _fileOpen, fileOpenText );
    (void)QWhatsThis::whatsThisButton( fileTools );
}


void
GUIApplicationWindow::buildSimulationTools()
{
    QPixmap icon;
    simTools = new QToolBar( this, "simulation operations" );
    addToolBar( simTools, tr( "Simulation Operations" ), Top, FALSE );

    // add "start simulation" - button
    icon = QPixmap( play_xpm );
    _startSimButton = new QToolButton( icon, "Start Simulation",
        QString::null, this, SLOT(start()), simTools, "start simulation" );
    QWhatsThis::add( _startSimButton, startSimText );
    _startSimButton->setEnabled(false);
    // add "stop simulation" - button
    icon = QPixmap( stop_xpm );
    _stopSimButton = new QToolButton( icon, "Stop Simulation", QString::null,
        this, SLOT(stop()), simTools, "stop simulation" );
    QWhatsThis::add( _stopSimButton, stopSimText );
    _stopSimButton->setEnabled(false);
    // add "single step" - button
    icon = QPixmap( step_xpm );
    _singleStepButton = new QToolButton( icon, "Single Step",
        QString::null, this, SLOT(singleStep()), simTools,
        "single step" );
    QWhatsThis::add( _singleStepButton, singleSimStepText );
    _singleStepButton->setEnabled(false);
    // add the information about the current time step
    QLabel *label = new QLabel(QString("Current Step:"),
        simTools, "current simulation step");
    label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    _simStepLabel = new QLabel(QString("-"), simTools, "step information");
    _simStepLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    _simStepLabel->setFixedSize(100, 20);
    // add the simulation delay button
    QSpinBox *spin = new QSpinBox(1, 10000, 10,
        simTools, "simulation speed factor");
    spin->setValue(100);
    connect( spin, SIGNAL(valueChanged(int)),
        this, SLOT(setSimulationDelay(int)) );
}


void
GUIApplicationWindow::buildWindowsTools()
{
    QPixmap icon;
    _winTools = new QToolBar( this, "window operations" );
    addToolBar( _winTools, tr( "Window Operations" ), Top, FALSE );

    // add "start simulation" - button
    icon = QPixmap( new_window_xpm );
    _windowAdder = new QToolButton( icon, "Open New Window",
        QString::null, this, SLOT(openNewWindow()), _winTools,
        "open new window" );
    _windowAdder->setEnabled(false);
}


void
GUIApplicationWindow::buildFileMenu()
{
    QPixmap openIcon, saveIcon;
    openIcon = QPixmap( fileopen );
    saveIcon = QPixmap( filesave );
    // build the file-menu
    _fileMenu = new QPopupMenu( this );
    menuBar()->insertItem( "&File", _fileMenu );
    _loadID = _fileMenu->insertItem( openIcon, "&Open", this, SLOT(load()), CTRL+Key_O );
    _fileMenu->setWhatsThis( _loadID, fileOpenText );
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( "&Close", this, SLOT(closeAllWindows()), CTRL+Key_W );
    _fileMenu->insertItem( "&Quit", qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );
}


void
GUIApplicationWindow::buildSettingsMenu()
{
    _settingsMenu = new QPopupMenu( this );
    menuBar()->insertItem("&Settings", _settingsMenu);
    connect( _settingsMenu, SIGNAL( aboutToShow() ),
        this, SLOT( settingsMenuAboutToShow() ) );
}


void
GUIApplicationWindow::buildWindowsMenu()
{
    windowsMenu = new QPopupMenu( this );
    windowsMenu->setCheckable( TRUE );
    connect( windowsMenu, SIGNAL( aboutToShow() ), this, SLOT( windowsMenuAboutToShow() ) );
    menuBar()->insertItem( "&Windows", windowsMenu );
    menuBar()->insertSeparator();
}


void
GUIApplicationWindow::buildHelpMenu()
{
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem( "&Help", help );
    // build the help-menu
    help->insertItem( "&About", this, SLOT(about()), Key_F1);
    help->insertItem( "About&Qt", this, SLOT(aboutQt()));
    help->insertSeparator();
    help->insertItem( "What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1);
}


void GUIApplicationWindow::load()
{
    QString fn = QFileDialog::getOpenFileName( QString::null, QString("*.sumo.cfg"), this );
    if ( !fn.isEmpty() ) {
        closeAllWindows();
        _fileOpen->setEnabled(FALSE);
        _fileMenu->setItemEnabled(_loadID, FALSE);
        _loadThread->init(string(fn.ascii()));
        _loadThread->start();
        statusBar()->message( QString("Loading '") + fn + QString("'"));
    }  else {
    	statusBar()->message( "Loading aborted", 2000 );
    }
}


void
GUIApplicationWindow::netLoaded(QSimulationLoadedEvent *ec)
{
    if(ec->_net==0) {
        statusBar()->message( QString("Loading of '")
            + QString(ec->_file.c_str())
            + QString("' failed!!!"), 2000 );
    } else {
        myViewNumber = 0;
        statusBar()->message( QString("'")
            + QString(ec->_file.c_str())
            + QString("' loaded."), 2000 );
        _runThread->init(ec->_net, ec->_begin, ec->_end, ec->_craw);
        _wasStarted = false;
        _startSimButton->setEnabled(true);
        openNewWindow();
        string caption = string("SUMO ") + string(version)
            + string(" - ") + ec->_file;
        setCaption( caption.c_str());
        _windowAdder->setEnabled(true);
    }
    _fileOpen->setEnabled(TRUE);
    _fileMenu->setItemEnabled(_loadID, TRUE);
    if(myStartAtBegin&&ec->_net!=0) {
        start();
    }
}


void
GUIApplicationWindow::openNewWindow()
{
    if(!_runThread->simulationAvailable()) {
        statusBar()->message( "No simulation loaded!", 2000 );
        return;
    }
    GUISUMOViewParent* w = new GUISUMOViewParent( ws, 0, WDestructiveClose,
        _runThread->getNet(), *this );
    connect( w, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( message(const QString&, int )) );
    string caption = string("View #") + toString(myViewNumber++);
    w->setCaption( caption.c_str() );
    w->setIcon( QPixmap("document.xpm") );
    // show the very first window in maximized mode
    if ( ws->windowList().isEmpty() ) {
        w->showMaximized();
    } else {
        w->show();
    }
}


void
GUIApplicationWindow::start()
{
    // check whether a net was loaded successfully
    if(!_runThread->simulationAvailable()) {
        statusBar()->message( "No simulation available!", 2000 );
        return;
    }
    // check whether it was started before and pasued;
    //  when yes, prompt the user for acknowledge
    if(_wasStarted) {
        resume();
        return;
    }
    _wasStarted = true;
    _startSimButton->setEnabled(false);
//    _resumeSimButton->setEnabled(false);
    _stopSimButton->setEnabled(true);
    _singleStepButton->setEnabled(false);
    _runThread->begin();
}


void
GUIApplicationWindow::stop()
{
    _stopSimButton->setEnabled(false);
    _startSimButton->setEnabled(true);
    _singleStepButton->setEnabled(true);
    _runThread->stop();
}


void
GUIApplicationWindow::resume()
{
    _stopSimButton->setEnabled(true);
    _startSimButton->setEnabled(false);
    _singleStepButton->setEnabled(false);
    _runThread->resume();
}


void
GUIApplicationWindow::singleStep()
{
    _runThread->singleStep();
}


void
GUIApplicationWindow::closeAllWindows()
{
    // remove trackers and other external windows
    size_t i;
    for(i=0; i<mySubWindows.size(); i++) {
        QApplication::postEvent( mySubWindows[i],
            new QCloseEvent());
    }
    //
    GUISUMOViewParent* m = (GUISUMOViewParent*)ws->activeWindow();
    while(m) {
    	m->close();
        m = (GUISUMOViewParent*)ws->activeWindow();
    }
    _runThread->deleteSim();
    resetSimulationToolBar();
    string caption = string("SUMO ") + string(version)
        + string(" - no simulation loaded");
    setCaption( caption.c_str());
    for(i=0; i<mySubWindows.size(); i++) {
        delete mySubWindows[i];
    }
    mySubWindows.clear();
    update();
}


void GUIApplicationWindow::about()
{
    QAboutSUMO aboutS(0, 0, TRUE);
    aboutS.show();
}


void GUIApplicationWindow::aboutQt()
{
    string versionString =
        string("SUMO GUI ") + string(version);
    QMessageBox::aboutQt( this, versionString.c_str() );
}


void
GUIApplicationWindow::windowsMenuAboutToShow()
{
    windowsMenu->clear();
    int cascadeId = windowsMenu->insertItem("&Cascade", ws, SLOT(cascade() ) );
    int tileId = windowsMenu->insertItem("&Tile", ws, SLOT(tile() ) );
    if ( ws->windowList().isEmpty() ) {
    	windowsMenu->setItemEnabled( cascadeId, FALSE );
    	windowsMenu->setItemEnabled( tileId, FALSE );
    }
    windowsMenu->insertSeparator();
    QWidgetList windows = ws->windowList();
    for ( int i = 0; i < int(windows.count()); ++i ) {
    	int id = windowsMenu->insertItem(windows.at(i)->caption(),
					 this, SLOT( windowsMenuActivated( int ) ) );
    	windowsMenu->setItemParameter( id, i );
    	windowsMenu->setItemChecked( id, ws->activeWindow() == windows.at(i) );
    }
}


void GUIApplicationWindow::windowsMenuActivated( int id )
{
    QWidget* w = ws->windowList().at( id );
    if ( w ) {
	    w->showNormal();
	    w->setFocus();
    }
}


bool
GUIApplicationWindow::event(QEvent *e)
{
    if(e->type()!=QEvent::User) {
        return QMainWindow::event(e);
    }
    QSUMOEvent *ownEvent = static_cast<QSUMOEvent*>(e);
    switch(ownEvent->getOwnType()) {
    case EVENT_SIMULATION_STEP:
        {
            // inform views
            QWidgetList windows = ws->windowList();
            size_t i;
            for ( i = 0; i < windows.count(); ++i ) {
                QApplication::postEvent( windows.at(i),
                    new QSimulationStepEvent());
            }
            // inform other windows
            for(i=0; i<mySubWindows.size(); i++) {
                QApplication::postEvent( mySubWindows[i],
                    new QSimulationStepEvent());
            }
            ostringstream str;
            str << _runThread->getCurrentTimeStep();
            _simStepLabel->setText(QString(str.str().c_str()));
            update();
        }
        return TRUE;
    case EVENT_SIMULATION_LOADED:
        netLoaded(static_cast<QSimulationLoadedEvent*>(e));
        repaint();
        return TRUE;
    case EVENT_ERROR_OCCURED:
        {
            QMessageBox *myBox = new QMessageBox("An error occured!",
                static_cast<QMessageEvent*>(e)->getMsg().c_str(),
                QMessageBox::Warning,
                QMessageBox::Ok | QMessageBox::Default,
                QMessageBox::NoButton, QMessageBox::NoButton);
            myBox->exec();
        }
        return TRUE;
    case EVENT_SIMULATION_ENDED:
        processSimulationEndEvent(static_cast<QSimulationEndedEvent*>(e));
        return TRUE;
    default:
        throw 1;
    }
}


void
GUIApplicationWindow::processSimulationEndEvent(QSimulationEndedEvent *e)
{
    // build the text
    stringstream text;
    text << "The simulation has ended at time step "
        << e->getTimeStep() << "." << endl;
    switch(e->getReason()) {
    case QSimulationEndedEvent::ER_NO_VEHICLES:
        text << "Reason: All vehicles have left the simulation.";
        break;
    case QSimulationEndedEvent::ER_END_STEP_REACHED:
        text << "Reason: The final simulation step has been reached.";
        break;
    default:
        throw 1;
    }

    //
    stop();
    QMessageBox *myBox = new QMessageBox("Simulation ended",
        text.str().c_str(),
        QMessageBox::Warning,
        QMessageBox::Ok | QMessageBox::Default,
        QMessageBox::NoButton, QMessageBox::NoButton);
    myBox->exec();
    if(myQuitOnEnd) {
        qApp->closeAllWindows();
    }
}



void
GUIApplicationWindow::setSimulationDelay(int value)
{
    _runThread->setSimulationDelay(value);
}



void
GUIApplicationWindow::resetSimulationToolBar()
{
    _startSimButton->setEnabled(false);
    _stopSimButton->setEnabled(false);
    _singleStepButton->setEnabled(false);
    _simStepLabel->setText("-");
    _windowAdder->setEnabled(false);
}


int
GUIApplicationWindow::getMaxGLWidth() const
{
    return myGLWidth;
}


int
GUIApplicationWindow::getMaxGLHeight() const
{
    return myGLHeight;
}


void
GUIApplicationWindow::addChild(QWidget *child,
                               bool updateOnSimStep)
{
    mySubWindows.push_back(child);
}


void
GUIApplicationWindow::removeChild(QWidget *child)
{
    std::vector<QWidget*>::iterator i =
        std::find(mySubWindows.begin(), mySubWindows.end(), child);
    mySubWindows.erase(i);
}


void
GUIApplicationWindow::appSettings()
{
    QApplicationSettings appSettings(this, myQuitOnEnd);
    appSettings.show();
}


void
GUIApplicationWindow::simSettings()
{
    QSimulationSettings simSettings(this);
    simSettings.show();
}


void
GUIApplicationWindow::settingsMenuAboutToShow()
{
    _settingsMenu->clear();
    _settingsMenu->insertItem( "Application Settings", this, SLOT(appSettings()));
    _settingsMenu->insertItem( "Simulation Settings", this, SLOT(simSettings()));
    _settingsMenu->insertSeparator();
    QWidgetList windows = ws->windowList();
    for ( int i = 0; i < int(windows.count()); ++i ) {
    	_settingsMenu->insertItem(
            QString("Settings for:") + windows.at(i)->caption(),
					 this, SLOT( windowSetings( int ) ) );
    }
}


void
GUIApplicationWindow::windowSetings(int window)
{
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIApplicationWindow.icc"
//#endif

// Local Variables:
// mode:C++
// End:


