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
#include <version.cpp>

#include "GUISUMOViewParent.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "GUIApplicationWindow.h"
#include "QSimulationStepEvent.h"
#include "QSimulationLoadedEvent.h"
#include "GUIEvents.h"
#include "QAboutSUMO.h"
#include "icons/filesave.xpm"
#include "icons/fileopen.xpm"
#include "icons/play.xpm"
#include "icons/stop.xpm"
#include "icons/cont.xpm"
#include "icons/step.xpm"
#include "icons/new_window.xpm"


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

const char * resumeSimText = "Click this button to resume a stopped simulation. <br><br>"
"You can also select the <b>Resume Simulation</b> from the Simulation menu.";

const char * singleSimStepText = "Click this button to perform a single simulation step. <br><br>"
"You can also select the <b>Single Step</b> from the Simulation menu.";


/* =========================================================================
 * member method definitions
 * ======================================================================= */
GUIApplicationWindow::GUIApplicationWindow(int glWidth, int glHeight)
    : QMainWindow( 0, "example application main window", WDestructiveClose ),
    _loadThread(0), _runThread(0),
    myGLWidth(glWidth), myGLHeight(glHeight)
{
    // build additional threads
    _loadThread = new GUILoadThread(this);
    _runThread = new GUIRunThread(this, 1);

    // initialise font drawing
    myFonts.add("std", ".\\fonts\\arial11.fnt");
    myFonts.add("std", ".\\fonts\\arial10.fnt");
    myFonts.add("std", ".\\fonts\\arial9.fnt");
    myFonts.add("std", ".\\fonts\\arial8.fnt");
    myFonts.add("std", ".\\fonts\\arial7.fnt");
    myFonts.add("std", ".\\fonts\\arial6.fnt");
    myFonts.add("std", ".\\fonts\\arial5.fnt");

    // build tool bars
    buildFileTools();
    buildSimulationTools();
    buildWindowsTools();

    // build menu bar
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
    // build the windows-menu
    windowsMenu = new QPopupMenu( this );
    windowsMenu->setCheckable( TRUE );
    connect( windowsMenu, SIGNAL( aboutToShow() ), this, SLOT( windowsMenuAboutToShow() ) );
    menuBar()->insertItem( "&Windows", windowsMenu );
    menuBar()->insertSeparator();
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem( "&Help", help );
    // build the help-menu
    help->insertItem( "&About", this, SLOT(about()), Key_F1);
    help->insertItem( "About&Qt", this, SLOT(aboutQt()));
    help->insertSeparator();
    help->insertItem( "What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1);

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
}


GUIApplicationWindow::~GUIApplicationWindow()
{
    _runThread->prepareDestruction();
    _runThread->wait();
    delete _loadThread;
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
    // add "resume simulation" - button
    icon = QPixmap( cont_xpm );
    _resumeSimButton = new QToolButton( icon, "Resume Simulation",
        QString::null, this, SLOT(resume()), simTools,
        "resume simulation" );
    QWhatsThis::add( _resumeSimButton, resumeSimText );
    _resumeSimButton->setEnabled(false);
    // add "single step" - button
    icon = QPixmap( step_xpm );
    _singleStepButton = new QToolButton( icon, "Single Step",
        QString::null, this, SLOT(singleStep()), simTools,
        "single step" );
    QWhatsThis::add( _resumeSimButton, singleSimStepText );
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
}


void
GUIApplicationWindow::openNewWindow()
{
    if(!_runThread->simulationAvailable()) {
        statusBar()->message( "No simulation loaded!", 2000 );
        return;
    }
    GUISUMOViewParent* w = new GUISUMOViewParent( ws, 0, WDestructiveClose,
        _runThread->getNet(), this );
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
        switch( QMessageBox::warning( this, version,
                    "Do you really want to restart the simulation\n"
                    "All structures will start from the beginning step!\n\n",
                    "Yes", "No", 0,
                    0, 1 ) ) {
        case 0:
            break;
        case 1:
            return;
        default:
            break;
        }
    }
    _wasStarted = true;
    _startSimButton->setEnabled(false);
    _resumeSimButton->setEnabled(false);
    _stopSimButton->setEnabled(true);
    _singleStepButton->setEnabled(false);
    _runThread->begin();
}


void
GUIApplicationWindow::stop()
{
    _stopSimButton->setEnabled(false);
    _startSimButton->setEnabled(true);
    _resumeSimButton->setEnabled(true);
    _singleStepButton->setEnabled(true);
    _runThread->stop();
}


void
GUIApplicationWindow::resume()
{
    _stopSimButton->setEnabled(true);
    _startSimButton->setEnabled(false);
    _resumeSimButton->setEnabled(false);
    _singleStepButton->setEnabled(false);
    _runThread->resume();
}


void
GUIApplicationWindow::singleStep()
{
/*    _stopSimButton->setEnabled(false);
    _startSimButton->setEnabled(true);
    _resumeSimButton->setEnabled(true);
    _singleStepButton->setEnabled(true);*/
    _runThread->singleStep();
}


void GUIApplicationWindow::closeAllWindows()
{
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


void GUIApplicationWindow::windowsMenuAboutToShow()
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
            int i;
            for ( i = 0; i < int(windows.count()); ++i ) {
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
        }
        return TRUE;
    case EVENT_SIMULATION_LOADED:
        netLoaded(static_cast<QSimulationLoadedEvent*>(e));
        return TRUE;
    default:
        throw 1;
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
    _resumeSimButton->setEnabled(false);
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




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIApplicationWindow.icc"
//#endif

// Local Variables:
// mode:C++
// End:


