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
// Revision 1.13  2004/03/19 12:42:59  dkrajzew
// porting to FOX
//
// Revision 1.12  2003/11/20 13:18:34  dkrajzew
// further work on aggregated views
//
// Revision 1.11  2003/11/12 14:08:23  dkrajzew
// clean up after recent changes
//
// Revision 1.10  2003/11/11 08:43:04  dkrajzew
// synchronisation problems of parameter tracker updates patched
//
// Revision 1.9  2003/08/14 13:40:10  dkrajzew
// a lower priorised update-method is now used
//
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
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>

#include <guisim/GUINet.h>
#include <utils/convert/ToString.h>
#include <utils/common/StringUtils.h>
#include "GUIParameterTracker.h"
#include <gui/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIGlObject.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/textures/GUITexturesHelper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIParameterTracker) GUIParameterTrackerMap[]={
    FXMAPFUNC(SEL_CONFIGURE, 0,           GUIParameterTracker::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0,           GUIParameterTracker::onPaint),
    FXMAPFUNC(SEL_COMMAND,   MID_SIMSTEP, GUIParameterTracker::onSimStep),

};

  // Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUIParameterTracker,FXMainWindow,GUIParameterTrackerMap,ARRAYNUMBER(GUIParameterTrackerMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParameterTracker::GUIParameterTracker(GUIApplicationWindow &app)
    : FXMainWindow(gFXApp,"Tracker",NULL,NULL,DECOR_ALL,0,0,300,200),
    myApplication(&app)
{
    buildFileTools();
    buildFileMenu();
    app.addChild(this, true);
	FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
    myPanel = new GUIParameterTrackerPanel(glcanvasFrame,
        *myApplication, *this);
}


GUIParameterTracker::GUIParameterTracker(GUIApplicationWindow &app,
                                         const std::string &name,
                                         GUIGlObject &o,
                                         int xpos, int ypos)
    : FXMainWindow(app.getApp(),"Tracker",NULL,NULL,DECOR_ALL,0,0,300,200),
    myApplication(&app)
{
    buildFileMenu();
    buildFileTools();
    setTitle("Tracker");
    app.addChild(this, true);
	FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,
        FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
        0,0,0,0,0,0,0,0);
    myPanel = new GUIParameterTrackerPanel(glcanvasFrame,
        *myApplication, *this);
}


GUIParameterTracker::~GUIParameterTracker()
{
    myApplication->removeChild(this);
    for(TrackedVarsVector::iterator i1=myTracked.begin(); i1!=myTracked.end(); i1++) {
        delete (*i1);
    }
    for(ValuePasserVector::iterator i2=myValuePassers.begin(); i2!=myValuePassers.end(); i2++) {
        delete (*i2);
    }
}


void
GUIParameterTracker::addVariable( GUIGlObject *o, const std::string &name)
{
    TrackerValueDesc *newTracked =
        new TrackerValueDesc(name, RGBColor(0, 0, 0), o);
    myTracked.push_back(newTracked);
}


int
GUIParameterTracker::getMaxGLWidth() const
{
    return myApplication->getMaxGLWidth();
}


int
GUIParameterTracker::getMaxGLHeight() const
{
    return myApplication->getMaxGLHeight();
}


void
GUIParameterTracker::buildFileMenu()
{
}


void
GUIParameterTracker::buildFileTools()
{
}


void
GUIParameterTracker::addTracked(GUIGlObject &o,
                                ValueSource<double> *src,
                                TrackerValueDesc *newTracked)
{
    myTracked.push_back(newTracked);
    // build connection (is automatically set into an execution map)
    myValuePassers.push_back(new GLObjectValuePassConnector<double>(o,
        src, newTracked));
}


long
GUIParameterTracker::onConfigure(FXObject *sender, FXSelector sel, void *data)
{
    myPanel->onConfigure(sender, sel, data);
    return FXMainWindow::onConfigure(sender, sel, data);
}


long
GUIParameterTracker::onPaint(FXObject *sender, FXSelector sel, void *data)
{
    myPanel->onPaint(sender, sel, data);
    return FXMainWindow::onPaint(sender, sel, data);
}


long
GUIParameterTracker::onSimStep(FXObject*sender,FXSelector,void*)
{
    update();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUIParameterTracker::GUIParameterTrackerPanel-methods
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIParameterTracker::GUIParameterTrackerPanel) GUIParameterTrackerPanelMap[]={
    FXMAPFUNC(SEL_CONFIGURE, 0, GUIParameterTracker::GUIParameterTrackerPanel::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0, GUIParameterTracker::GUIParameterTrackerPanel::onPaint),

};

  // Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUIParameterTracker::GUIParameterTrackerPanel,FXGLCanvas,GUIParameterTrackerPanelMap,ARRAYNUMBER(GUIParameterTrackerPanelMap))



GUIParameterTracker::GUIParameterTrackerPanel::GUIParameterTrackerPanel(
        FXComposite *c, GUIApplicationWindow &app,
        GUIParameterTracker &parent)
    : FXGLCanvas(c, app.getGLVisual(), app.getBuildGLCanvas(), (FXObject*) 0, (FXSelector) 0, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 300, 200),
    myParent(&parent), myApplication(&app)
{
}


GUIParameterTracker::GUIParameterTrackerPanel::~GUIParameterTrackerPanel()
{
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValues()
{
    // compute which font to use
    int fontIdx = (_widthInPixels-300) / 100;
    if(fontIdx<0) fontIdx = 0;
    if(fontIdx>4) fontIdx = 4;
    GUITexturesHelper::getFontRenderer().SetActiveFont(4-fontIdx);
    //
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    size_t run = 0;
    for(TrackedVarsVector::iterator i=myParent->myTracked.begin(); i!=myParent->myTracked.end(); i++) {
        TrackerValueDesc *desc = *i;
        drawValue(*desc,
            (float) _widthInPixels / (float) myParent->myTracked.size() * (float) run);
        run++;
    }
    GUITexturesHelper::getFontRenderer().Draw(_widthInPixels, _heightInPixels);//this, width, height);
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
    GUITexturesHelper::getFontRenderer().SetColor(red*0.5f, green*0.5f, blue*0.5f);
        // draw minimum boundery
    glVertex2d(0, desc.getMin());
    glVertex2d(2.0, desc.getMin());
    GUITexturesHelper::getFontRenderer().StringOut(3,
        patchHeightVal(desc, desc.getMin()),
        StringUtils::trim(desc.getMin(), 2));
        // draw maximum boundery
    glVertex2d(0, desc.getMax());
    glVertex2d(2.0, desc.getMax());
    GUITexturesHelper::getFontRenderer().StringOut(3,
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
        desc.unlockValues();
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
    desc.unlockValues();
    glPopMatrix();
    // add current value string
    GUITexturesHelper::getFontRenderer().SetColor(red, green, blue);
    GUITexturesHelper::getFontRenderer().StringOut(3,
        patchHeightVal(desc, yp), StringUtils::trim(yp, 2));
    GUITexturesHelper::getFontRenderer().StringOut(namePos+3, 0,
        desc.getName());
}


float
GUIParameterTracker::GUIParameterTrackerPanel::patchHeightVal(TrackerValueDesc &desc,
                                                              double d)
{
    float height = (float) _heightInPixels;
    float range = (float) desc.getRange();
    float yoff = (float) desc.getYCenter();
    float abs = (height) * (((float)d-yoff)/range) * 0.8f;
    return (height * 0.5f) - abs - 6;
}


long
GUIParameterTracker::GUIParameterTrackerPanel::onConfigure(FXObject*,
                                                           FXSelector,void*)
{
//    return 1;
    if(makeCurrent()) {
//        _lock.lock();
        _widthInPixels = myParent->getMaxGLWidth();
        _heightInPixels = myParent->getMaxGLHeight();
        glViewport( 0, 0, _widthInPixels-1, _heightInPixels-1 );
        glClearColor( 1.0, 1.0, 1.0, 1 );
        glDisable( GL_DEPTH_TEST );
        glDisable( GL_LIGHTING );
        glDisable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND );
        glEnable(GL_ALPHA_TEST);
        glDisable(GL_COLOR_MATERIAL);
        glLineWidth(1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        makeNonCurrent();
//        _lock.unlock();
    }
    return 1;
}


long
GUIParameterTracker::GUIParameterTrackerPanel::onPaint(FXObject*,
                                                       FXSelector,void*)
{
    if(!isEnabled()) {
        return 1;
    }
    if(_lock.locked()) {
        return 1;
    }
    _lock.lock();
    if(makeCurrent()) {
        _widthInPixels = getWidth();
        _heightInPixels = getHeight();
        if(_widthInPixels!=0&&_heightInPixels!=0) {
            glViewport( 0, 0, _widthInPixels-1, _heightInPixels-1 );
            glClearColor( 1.0, 1.0, 1.0, 1 );
            glDisable( GL_DEPTH_TEST );
            glDisable( GL_LIGHTING );
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND );
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_COLOR_MATERIAL);
            glLineWidth(1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // draw
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            drawValues();
            glFlush();
            swapBuffers();
        }
        makeNonCurrent();
    }
    _lock.unlock();
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


