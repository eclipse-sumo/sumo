#include <string>
#include <sstream>

#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qprinter.h>
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

#include "GUISUMOView.h"
#include "GUILoadThread.h"
#include "GUIRunThread.h"
#include "GUIApplicationWindow.h"
#include "QSimulationStepEvent.h"
#include "QSimulationLoadedEvent.h"
#include "GUIEvents.h"
#include "icons/filesave.xpm"
#include "icons/fileopen.xpm"
#include "icons/fileprint.xpm"
#include "icons/play.xpm"
#include "icons/stop.xpm"
#include "icons/cont.xpm"
#include "icons/step.xpm"
#include "icons/new_window.xpm"


const char * fileOpenText = "Click this button to open a <em>new file</em>. <br><br>"
"You can also select the <b>Open command</b> from the File menu.";

const char * filePrintText = "Click this button to print the file you "
"are editing.\n\n"
"You can also select the Print command from the File menu.";

const char * startSimText = "Click this button to start the loaded simulation. <br><br>"
"You can also select the <b>Start Simulation</b> from the Simulation menu.";

const char * stopSimText = "Click this button to interupt the running simulation. <br><br>"
"You can also select the <b>Stop Simulation</b> from the Simulation menu.";

const char * resumeSimText = "Click this button to resume a stopped simulation. <br><br>"
"You can also select the <b>Resume Simulation</b> from the Simulation menu.";

const char * singleSimStepText = "Click this button to perform a single simulation step. <br><br>"
"You can also select the <b>Single Step</b> from the Simulation menu.";



GUIApplicationWindow::GUIApplicationWindow()
    : QMainWindow( 0, "example application main window", WDestructiveClose ),
    _loadThread(new GUILoadThread(this)),
    _runThread(new GUIRunThread(this, 5))
{
#ifndef QT_NO_PRINTER
    printer = new QPrinter;
#endif

    buildFileTools();
    buildSimulationTools();
    buildWindowsTools();
    int id;

    QPixmap openIcon, saveIcon;
#ifndef QT_NO_PRINTER
    QPixmap printIcon;
    printIcon = QPixmap( fileprint );
#endif

    openIcon = QPixmap( fileopen );
    saveIcon = QPixmap( filesave );

    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );

    id = file->insertItem( openIcon, "&Open",
			   this, SLOT(load()), CTRL+Key_O );
    file->setWhatsThis( id, fileOpenText );

#ifndef QT_NO_PRINTER
    file->insertSeparator();
    id = file->insertItem( printIcon, "&Print",
			   this, SLOT(print()), CTRL+Key_P );
    file->setWhatsThis( id, filePrintText );
#endif
    file->insertSeparator();
    file->insertItem( "&Close", this, SLOT(closeAllWindows()), CTRL+Key_W );
    file->insertItem( "&Quit", qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );

    windowsMenu = new QPopupMenu( this );
    windowsMenu->setCheckable( TRUE );
    connect( windowsMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowsMenuAboutToShow() ) );
    menuBar()->insertItem( "&Windows", windowsMenu );

    menuBar()->insertSeparator();
    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertItem( "&Help", help );

    help->insertItem( "&About", this, SLOT(about()), Key_F1);
    help->insertItem( "About&Qt", this, SLOT(aboutQt()));
    help->insertSeparator();
    help->insertItem( "What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1);

    QVBox* vb = new QVBox( this );
    vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    ws = new QWorkspace( vb );
    setCentralWidget( vb );

    statusBar()->message( "Ready", 2000 );
    _runThread->start();
}

GUIApplicationWindow::~GUIApplicationWindow()
{
    delete _loadThread;
    delete _runThread;
//    delete _runThread;
#ifndef QT_NO_PRINTER
    delete printer;
#endif
}

void
GUIApplicationWindow::buildFileTools()
{
    QPixmap openIcon, saveIcon;

    fileTools = new QToolBar( this, "file operations" );
    addToolBar( fileTools, tr( "File Operations" ), Top, TRUE );

    openIcon = QPixmap( fileopen );
    QToolButton * fileOpen = new QToolButton( openIcon, "Open File",
        QString::null, this, SLOT(load()), fileTools, "open file" );
    QWhatsThis::add( fileOpen, fileOpenText );
#ifndef QT_NO_PRINTER
    QPixmap printIcon;

    printIcon = QPixmap( fileprint );
    QToolButton * filePrint
	= new QToolButton( printIcon, "Print File", QString::null,
			   this, SLOT(print()), fileTools, "print file" );
    QWhatsThis::add( filePrint, filePrintText );
#endif

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
    QToolButton *button = new QToolButton( icon, "Open New Window",
        QString::null, this, SLOT(openNewWindow()), _winTools,
        "open new window" );
}



void GUIApplicationWindow::load()
{
    QString fn = QFileDialog::getOpenFileName( QString::null, QString("*.cfg"), this );
    if ( !fn.isEmpty() ) {
        closeAllWindows();
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
        statusBar()->message( QString("'")
            + QString(ec->_file.c_str())
            + QString("' loaded."), 2000 );
        _runThread->init(ec->_net, ec->_begin, ec->_end, ec->_craw);
        _wasStarted = false;
        _startSimButton->setEnabled(true);
        openNewWindow();
    }
}

void
GUIApplicationWindow::openNewWindow()
{
    if(!_runThread->simulationAvailable()) {
        statusBar()->message( "No simulation loaded!", 2000 );
        return;
    }
    GUISUMOView* w = new GUISUMOView( ws, 0, WDestructiveClose,
        _runThread->getNet() );
    connect( w, SIGNAL( message(const QString&, int) ), statusBar(), SLOT( message(const QString&, int )) );
    w->setCaption(_name.c_str());
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
        if( QMessageBox::information( this, "sumo v0.8",
            "Do you really want to restart the simulation\n",
            QMessageBox::Information,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No) == 1) {
            return;
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



void GUIApplicationWindow::print()
{
#ifndef QT_NO_PRINTER
    GUISUMOView* m = (GUISUMOView*)ws->activeWindow();
    if ( m )
	m->print( printer );
#endif
}


void GUIApplicationWindow::closeAllWindows()
{
    GUISUMOView* m = (GUISUMOView*)ws->activeWindow();
    while(m) {
    	m->close();
        m = (GUISUMOView*)ws->activeWindow();
    }
    _runThread->deleteSim();
}

void GUIApplicationWindow::about()
{
    QMessageBox::about( this, "SUMO GUI v1.0",
			"Simulation of Urban MObility v1.0\n "
			"A traffic simulation by IVF/DLR & ZAIK (http://www.ivf.dlr.de).");
}


void GUIApplicationWindow::aboutQt()
{
    QMessageBox::aboutQt( this, "SUMO GUI v1.0" );
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

/*
void 
GUIApplicationWindow::updateScreens()
{
    QWidgetList windows = ws->windowList();
    for ( int i = 0; i < int(windows.count()); ++i ) {
        QThread::postEvent( windows.at(i), new QPaintEvent( QRect(0, 0, 800, 600) ) );
        QObject *o = windows.at(i) ;
//        cout << windows.at(i) << endl;
//        windows.at(i)->repaint();
    }
}*/

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
            QWidgetList windows = ws->windowList();
            for ( int i = 0; i < int(windows.count()); ++i ) {
                QApplication::postEvent( windows.at(i), 
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
