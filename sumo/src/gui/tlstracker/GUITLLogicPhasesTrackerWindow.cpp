/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <qgl.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <iostream>
#include <gui/GUIApplicationWindow.h>
#include "GUITLLogicPhasesTrackerWindow.h"
#include <microsim/MSTrafficLightLogic.h>
#include <microsim/MSLink.h>
#include <utils/convert/ToString.h>


#ifndef WIN32
#include "GUITLLogicPhasesTrackerWindow.moc"
#endif

using namespace std;

GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerWindow(
        GUIApplicationWindow &app, MSTrafficLightLogic &logic)
        : myApplication(app), myTLLogic(logic)
{
    setCaption("Tracker");
    setBaseSize(300, 200);
    setMinimumSize(300, 200);
    app.addChild(this, true);
    for(size_t i=0; i<myTLLogic.getLinks().size(); i++) {
        myLinkNames.push_back(toString<size_t>(i));
    }
    myPanel = new
        GUITLLogicPhasesTrackerPanel(myApplication, *this);
    setCentralWidget(myPanel);
//    myPanel->move(xpos, ypos);
    show();
}


GUITLLogicPhasesTrackerWindow::~GUITLLogicPhasesTrackerWindow()
{
}



bool
GUITLLogicPhasesTrackerWindow::event ( QEvent *e )
{
    if(e->type()!=QEvent::User) {
        return QMainWindow::event(e);
    }
    update();
    return TRUE;
}


int
GUITLLogicPhasesTrackerWindow::getMaxGLWidth() const
{
    return myApplication.getMaxGLWidth();
}


int
GUITLLogicPhasesTrackerWindow::getMaxGLHeight() const
{
    return myApplication.getMaxGLHeight();
}


void
GUITLLogicPhasesTrackerWindow::paintEvent ( QPaintEvent *e )
{
    myPanel->paintEvent(e);
    QMainWindow::paintEvent(e);
}


void
GUITLLogicPhasesTrackerWindow::resizeEvent ( QResizeEvent *e )
{
	QMainWindow::resizeEvent(e);
}



void
GUITLLogicPhasesTrackerWindow::drawValues(GUITLLogicPhasesTrackerPanel &caller)
{
    // compute which font to use
    //
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(-1, -1, 0);
    glScaled(2, 2, 0);
    glDisable(GL_TEXTURE_2D);
    size_t run = 0;
    // draw the horizontal lines dividing the signal groups
    glColor3d(1, 1, 1);
    myFontRenderer.SetColor(1, 1, 1);
    glBegin(GL_LINES);
    //
    double height = (double) caller.getHeightInPixels();
    double width = (double) caller.getWidthInPixels();
    double h4 = ((double) 4 / height);
    double h10 = ((double) 10 / height);
    double h16 = ((double) 16 / height);
    double h20 = ((double) 20 / height);


    double h = 1.0 - h10;
    double h2 = 12;
    size_t i;
    for(i=0; i<myTLLogic.getLinks().size()+1; i++) {
        // draw the bar
        glVertex2f(0, h);
        glVertex2f((double) 30 / width, h);
        // draw the name
        if(i<myTLLogic.getLinks().size()) {
            myFontRenderer.StringOut(2, h2 - h10, myLinkNames[i]);
            h2 += 20;
        }
        h -= h20;
    }
    glEnd();
    // names closure
    h += (double) 20 / height;
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f((double) 30 / width, 1.0);
    glVertex2f((double) 30 / width, h);
    glEnd();
    // phases
    myLock.lock();
    PhasesVector::iterator pi = myPhases.begin();
    DurationsVector::iterator pd = myDurations.begin();
    double x = (double) 31 / width;
    for(i=30; i<caller.getWidthInPixels()&&pd!=myDurations.end(); ) {
        size_t duration = *pd;
        double h = 1.0 - h10;
        double x2 = x + (double) duration / width;
        for(size_t j=0; j<myTLLogic.getLinks().size(); j++) {
            MSLink::LinkState state =
                (*pi).first.test(j)==true
                ? MSLink::LINKSTATE_TL_GREEN
                : (*pi).second.test(j)==true
                    ? MSLink::LINKSTATE_TL_YELLOW
                    : MSLink::LINKSTATE_TL_RED;
            switch(state) {
            case MSLink::LINKSTATE_TL_GREEN:
                glColor3f(0, 1.0, 0);
                glBegin(GL_QUADS);
                glVertex2f(x, h - h16);
                glVertex2f(x, h - h4);
                glVertex2f(x2, h - h4);
                glVertex2f(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_YELLOW:
                glColor3f(1.0, 1.0, 0);
                glBegin(GL_QUADS);
                glVertex2f(x, h - h16);
                glVertex2f(x, h - h4);
                glVertex2f(x2, h - h4);
                glVertex2f(x2, h - h16);
                glEnd();
                break;
            case MSLink::LINKSTATE_TL_RED:
                glColor3f(1.0, 0, 0);
                glBegin(GL_LINES);
                glVertex2f(x, h - h10);
                glVertex2f(x2, h - h10);
                glEnd();
                break;
            default:
                break;
            }
            h -= h20;
        }
        i += duration;
        pi++;
        pd++;
        x = x2;
    }
    myLock.unlock();

    // just tests...
    glBegin(GL_LINES);
    glColor3d(1, 1, 0);
    glVertex2f(0, 0);
    glVertex2f(1, 1);
    glColor3d(1, 0, 1);
    glVertex2f(0, 1);
    glVertex2f(1, 0);
    glEnd();
    myFontRenderer.Draw(caller.getWidthInPixels(),
        caller.getHeightInPixels());
}


void
GUITLLogicPhasesTrackerWindow::addValue(SimplePhaseDef def)
{
    cout << "Ret:" << def.first << '/' << def.second  << endl;
    myLock.lock();
    if(myPhases.size()==0||*(myPhases.end()-1)!=def) {
        myPhases.push_back(def);
        myDurations.push_back(1);
    } else {
        *(myDurations.end()-1) += 1;
    }
    myLock.unlock();
}

void
GUITLLogicPhasesTrackerWindow::setFontRenderer(GUITLLogicPhasesTrackerPanel &caller)
{
    myFontRenderer.add(myApplication.myFonts.get("std10"));
}

/*
void
GUITLLogicPhasesTrackerWindow::buildFileMenu()
{
    QPixmap saveIcon;
    saveIcon = QPixmap( filesave );
    // build the file-menu
    _fileMenu = new QPopupMenu( this );
    menuBar()->insertItem( "&File", _fileMenu );
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( "&Close", this, SLOT(closeAllWindows()), CTRL+Key_W );
}


void
GUITLLogicPhasesTrackerWindow::buildFileTools()
{
    QPixmap saveIcon;

    fileTools = new QToolBar( "file operations", this);
    addToolBar( fileTools, tr( "File Operations" ), Top, TRUE );

    saveIcon = QPixmap( filesave );
    QToolButton *fileSave = new QToolButton( saveIcon, "Open File",
        QString::null, this, SLOT(load()), fileTools, "open file" );
}
*/
/*
void
GUITLLogicPhasesTrackerWindow::addTracked(TrackerValueDesc *newTracked)
{
    myTracked.push_back(newTracked);
}
*/






GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::GUITLLogicPhasesTrackerPanel(
        GUIApplicationWindow &app, GUITLLogicPhasesTrackerWindow &parent)
    : QGLWidget(&parent),
    myParent(parent), _noDrawing(0), myApplication(app)
{
}

GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::~GUITLLogicPhasesTrackerPanel()
{
}


void
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::initializeGL()
{
    _lock.lock();
    _widthInPixels = myParent.getMaxGLWidth();
    _heightInPixels = myParent.getMaxGLHeight();
    glViewport( 0, 0, myParent.getMaxGLWidth()-1, myParent.getMaxGLHeight()-1 );
    glClearColor( 0, 0, 0, 1 );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND );
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    myParent.setFontRenderer(*this);
    _lock.unlock();
}


void
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::resizeGL( int width, int height )
{
    _lock.lock();
    _widthInPixels = width;
    _heightInPixels = height;
    glViewport( 0, 0, _widthInPixels, _heightInPixels );
    _lock.unlock();
}

void
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::paintGL()
{
    _lock.lock();
    // return if the canvas is not visible
    if(!isVisible()) {
		_lock.unlock();
        return;
    }
    _noDrawing++;
    // ...and return when drawing is already being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    _widthInPixels = width();
    _heightInPixels = height();
    // draw
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    myParent.drawValues(*this);
    _noDrawing--;
    glFlush();
    swapBuffers();
    _lock.unlock();
}


size_t
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::getHeightInPixels() const
{
    return _heightInPixels;
}

size_t
GUITLLogicPhasesTrackerWindow::GUITLLogicPhasesTrackerPanel::getWidthInPixels() const
{
    return _widthInPixels;
}



