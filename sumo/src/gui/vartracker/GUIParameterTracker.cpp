//---------------------------------------------------------------------------//
//                        GUIParameterTracker.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.8  2003/07/30 12:51:42  dkrajzew
// bugs on resize and font loading partially patched
//
// Revision 1.7  2003/07/30 08:50:42  dkrajzew
// tracker debugging (not yet completed)
//
// Revision 1.6  2003/07/22 14:57:42  dkrajzew
// wrong order of initialisations-warning patched
//
// Revision 1.5  2003/07/18 12:30:14  dkrajzew
// removed some warnings
//
// Revision 1.4  2003/06/18 11:02:33  dkrajzew
// new fonts usage added
//
// Revision 1.3  2003/06/05 11:38:47  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <qdialog.h>
#include <qevent.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qrect.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <utils/convert/ToString.h>
#include <utils/common/StringUtils.h>
#include "GUIParameterTracker.h"
#include <gui/GUIGlObject.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUINet.h>
#include <gui/icons/filesave.xpm>


#ifndef WIN32
#include "GUIParameterTracker.moc"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParameterTracker::GUIParameterTracker(GUIApplicationWindow &app)
        : myApplication(app)
{
    buildFileTools();
    buildFileMenu();
    setCaption("Tracker");
    setBaseSize(200, 300);
    setMinimumSize(200, 300);
    app.addChild(this, true);
    myPanel = new
        GUIParameterTrackerPanel(myApplication, *this);
    myPanel->setGeometry(QRect( 0, 0, 200, 300));
    show();
}



GUIParameterTracker::GUIParameterTracker(GUIApplicationWindow &app,
                                         const std::string &name,
                                         GUIGlObject &o,
                                         DoubleValueSource *src,
                                         int xpos, int ypos)
        : myApplication(app)
{
    buildFileMenu();
    buildFileTools();
    setCaption("Tracker");
    setBaseSize(300, 200);
    setMinimumSize(300, 200);
    app.addChild(this, true);
    myPanel = new
        GUIParameterTrackerPanel(myApplication, *this);
    setCentralWidget(myPanel);
    addVariable(&o, name, src);
    myPanel->move(xpos, ypos);
    show();
}


GUIParameterTracker::~GUIParameterTracker()
{
    myApplication.removeChild(this);
}


void
GUIParameterTracker::addVariable( GUIGlObject *o, const std::string &name,
                                 DoubleValueSource *src)
{
    TrackerValueDesc *newTracked =
        new TrackerValueDesc(
            name, RGBColor(0, 0, 0), o, src);
    myTracked.push_back(newTracked);
}


bool
GUIParameterTracker::event ( QEvent *e )
{
    if(e->type()!=QEvent::User) {
        return QMainWindow::event(e);
    }
    for(TrackedVarsVector::iterator i=myTracked.begin(); i!=myTracked.end(); i++) {
        TrackerValueDesc *desc = *i;
        desc->simStep();
    }
    repaint();
    return TRUE;
}


int
GUIParameterTracker::getMaxGLWidth() const
{
    return myApplication.getMaxGLWidth();
}


int
GUIParameterTracker::getMaxGLHeight() const
{
    return myApplication.getMaxGLHeight();
}


void
GUIParameterTracker::paintEvent ( QPaintEvent *e )
{
    myPanel->paintEvent(e);
    QMainWindow::paintEvent(e);
}


void
GUIParameterTracker::resizeEvent ( QResizeEvent *e )
{
/*    myPanel->setGeometry(QRect( 0, 0,
		e->size().width(), e->size().height()));*/
	QMainWindow::resizeEvent(e);
}



void
GUIParameterTracker::buildFileMenu()
{
/*
    QPixmap saveIcon;
    saveIcon = QPixmap( filesave );
    // build the file-menu
    _fileMenu = new QPopupMenu( this );
    menuBar()->insertItem( "&File", _fileMenu );
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( "&Close", this, SLOT(closeAllWindows()), CTRL+Key_W );
*/
}


void
GUIParameterTracker::buildFileTools()
{
    /*
    QPixmap saveIcon;

    fileTools = new QToolBar( "file operations", this);
    addToolBar( fileTools, tr( "File Operations" ), Top, TRUE );

    saveIcon = QPixmap( filesave );
    QToolButton *fileSave = new QToolButton( saveIcon, "Open File",
        QString::null, this, SLOT(load()), fileTools, "open file" );
        */
}







GUIParameterTracker::GUIParameterTrackerPanel::GUIParameterTrackerPanel(
        GUIApplicationWindow &app, GUIParameterTracker &parent)
    : QGLWidget(&parent),
    myParent(parent), _noDrawing(0), myApplication(app)
{
}

GUIParameterTracker::GUIParameterTrackerPanel::~GUIParameterTrackerPanel()
{
}


void
GUIParameterTracker::GUIParameterTrackerPanel::initializeGL()
{
    _lock.lock();
    _widthInPixels = myParent.getMaxGLWidth();
    _heightInPixels = myParent.getMaxGLHeight();
    glViewport( 0, 0, myParent.getMaxGLWidth()-1, myParent.getMaxGLHeight()-1 );
    glClearColor( 1.0, 1.0, 1.0, 1 );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND );
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    myFontRenderer.add(myApplication.myFonts.get("std11"));
    myFontRenderer.add(myApplication.myFonts.get("std10"));
    myFontRenderer.add(myApplication.myFonts.get("std9"));
    myFontRenderer.add(myApplication.myFonts.get("std8"));
    myFontRenderer.add(myApplication.myFonts.get("std7"));
//    myFontRenderer.add(myApplication.myFonts.get("std6"));
//    myFontRenderer.add(myApplication.myFonts.get("std5"));
    _lock.unlock();
}


void
GUIParameterTracker::GUIParameterTrackerPanel::resizeGL( int width, int height )
{
    _lock.lock();
    _widthInPixels = width;
    _heightInPixels = height;
    glViewport( 0, 0, _widthInPixels, _heightInPixels );
    _lock.unlock();
}

void
GUIParameterTracker::GUIParameterTrackerPanel::paintGL()
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
    drawValues();
    _noDrawing--;
    glFlush();
    swapBuffers();
    _lock.unlock();
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValues()
{
    // compute which font to use
    int fontIdx = (_widthInPixels-300) / 100;
    if(fontIdx<0) fontIdx = 0;
    if(fontIdx>4) fontIdx = 4;
    myFontRenderer.SetActiveFont(4-fontIdx);
    //
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    size_t run = 0;
    for(TrackedVarsVector::iterator i=myParent.myTracked.begin(); i!=myParent.myTracked.end(); i++) {
        TrackerValueDesc *desc = *i;
        drawValue(*desc,
            (float) _widthInPixels / (float) myParent.myTracked.size() * (float) run);
        run++;
    }
    myFontRenderer.Draw(_widthInPixels, _heightInPixels);//this, width, height);
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValue(TrackerValueDesc &desc,
                                                         float namePos)
{
    // apply scaling
    glPushMatrix();

    // apply the positiopn offset of the display
    glScaled(0.8, 0.8, 1);
    // apply value range scaling
    double ys = 2.0 / desc.getRange();
    glScaled(1.0, ys, 1.0);
    glTranslated(-1.0, -desc.getYCenter(), 0);

    // set color
    const RGBColor &col = desc.getColor();
    float red = col.red();
    float green = col.green();
    float blue = col.blue();
    // draw value bounderies
    glBegin( GL_LINES );
    // draw value bounderies and descriptions
    glColor4f(red, green, blue, 0.5f);
    myFontRenderer.SetColor(red*0.5f, green*0.5f, blue*0.5f);
        // draw minimum boundery
    glVertex2d(0, desc.getMin());
    glVertex2d(2.0, desc.getMin());
    myFontRenderer.StringOut(0,
        patchHeightVal(desc, desc.getMin()),
        StringUtils::trim(desc.getMin(), 2));
        // draw maximum boundery
    glVertex2d(0, desc.getMax());
    glVertex2d(2.0, desc.getMax());
    myFontRenderer.StringOut(0,
        patchHeightVal(desc, desc.getMax()),
        StringUtils::trim(desc.getMax(), 2));
        // draw some further lines
    glColor4f(red, green, blue, 0.3f);
    for(int a=1; a<6; a++) {
        double ypos = (desc.getRange()) / 6.0 * (double) a + desc.getMin();
        glVertex2d(0, ypos);
        glVertex2d(2.0, ypos);
    }
    glEnd();

    const std::vector<float> &values = desc.getValues();
    if(values.size()<2) {
        glPopMatrix();
        return;
    }

    // init values
    double xStep = 2.0 / ((double) values.size());
    std::vector<float>::const_iterator i = values.begin();
    double yp = (*i++);
    double xp = 0;
    // draw lines
    glBegin( GL_LINES );
    // set color
    glColor4f(red, green, blue, 1.0f);
    for(; i!=values.end(); i++) {
        double yn = (*i);
        double xn = xp + xStep;
        glVertex2d(xp, yp);
        glVertex2d(xn, yn);
        yp = yn;
        xp = xn;
    }
    glEnd();
    glPopMatrix();
    // add current value string
    myFontRenderer.SetColor(red, green, blue);
    myFontRenderer.StringOut(0,
        patchHeightVal(desc, yp),
        StringUtils::trim(yp, 2));
    myFontRenderer.StringOut(namePos, 0,
        desc.getName());
}


float
GUIParameterTracker::GUIParameterTrackerPanel::patchHeightVal(TrackerValueDesc &desc,
                                                              double d)
{
    float height = (float) _heightInPixels;
    float range = (float) desc.getRange();
    float yoff = (float) desc.getYCenter();
//    float max = (float) desc.getMax();
    float abs = (height) * (((float)d-yoff)/range) * 0.8f;
    return (height * 0.5f) - abs - 6;
}







/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIParameterTracker.icc"
//#endif

// Local Variables:
// mode:C++
// End:


