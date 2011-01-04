/****************************************************************************/
/// @file    GUIParameterTracker.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A window which displays the time line of one (or more) value(s)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fstream>

#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include "GUIParameterTracker.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/common/SUMOTime.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIParameterTracker) GUIParameterTrackerMap[]={
    FXMAPFUNC(SEL_CONFIGURE, 0,                       GUIParameterTracker::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0,                       GUIParameterTracker::onPaint),
    FXMAPFUNC(SEL_COMMAND,   MID_SIMSTEP,             GUIParameterTracker::onSimStep),
    FXMAPFUNC(SEL_COMMAND,   GUIParameterTracker::MID_AGGREGATIONINTERVAL, GUIParameterTracker::onCmdChangeAggregation),
    FXMAPFUNC(SEL_COMMAND,   GUIParameterTracker::MID_SAVE, GUIParameterTracker::onCmdSave),

};

// Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUIParameterTracker,FXMainWindow,GUIParameterTrackerMap,ARRAYNUMBER(GUIParameterTrackerMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIParameterTracker::GUIParameterTracker(GUIMainWindow &app,
        const std::string &name) throw()
        : FXMainWindow(app.getApp(),"Tracker",NULL,NULL,DECOR_ALL,20,20,300,200),
        myApplication(&app) {
    buildToolBar();
    app.addChild(this, true);
    FXVerticalFrame *glcanvasFrame = new FXVerticalFrame(this, FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0,0,0,0,0);
    myPanel = new GUIParameterTrackerPanel(glcanvasFrame, *myApplication, *this);
    setTitle(name.c_str());
    setIcon(GUIIconSubSys::getIcon(ICON_APP_TRACKER));
}


GUIParameterTracker::~GUIParameterTracker() throw() {
    myApplication->removeChild(this);
    for (std::vector<TrackerValueDesc*>::iterator i1=myTracked.begin(); i1!=myTracked.end(); i1++) {
        delete(*i1);
    }
    // deleted by GUINet
    for (std::vector<GLObjectValuePassConnector<SUMOReal>*>::iterator i2=myValuePassers.begin(); i2!=myValuePassers.end(); i2++) {
        delete(*i2);
    }
    delete myToolBarDrag;
    delete myToolBar;
}


void
GUIParameterTracker::create() {
    FXMainWindow::create();
    myToolBarDrag->create();
}


void
GUIParameterTracker::buildToolBar() throw() {
    myToolBarDrag = new FXToolBarShell(this,FRAME_NORMAL);
    myToolBar = new FXToolBar(this,myToolBarDrag, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
    new FXToolBarGrip(myToolBar, myToolBar, FXToolBar::ID_TOOLBARGRIP, TOOLBARGRIP_DOUBLE);
    // save button
    new FXButton(myToolBar,"\t\tSave the data...",
                 GUIIconSubSys::getIcon(ICON_SAVE), this, GUIParameterTracker::MID_SAVE,
                 ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // aggregation interval combo
    myAggregationInterval =
        new FXComboBox(myToolBar, 8, this, MID_AGGREGATIONINTERVAL,
                       FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP|COMBOBOX_STATIC);
    myAggregationInterval->appendItem("1s");
    myAggregationInterval->appendItem("1min");
    myAggregationInterval->appendItem("5min");
    myAggregationInterval->appendItem("15min");
    myAggregationInterval->appendItem("30min");
    myAggregationInterval->appendItem("60min");
    myAggregationInterval->setNumVisible(6);
}


void
GUIParameterTracker::addTracked(GUIGlObject &o, ValueSource<SUMOReal> *src,
                                TrackerValueDesc *newTracked) throw() {
    myTracked.push_back(newTracked);
    // build connection (is automatically set into an execution map)
    myValuePassers.push_back(new GLObjectValuePassConnector<SUMOReal>(o, src, newTracked));
}


long
GUIParameterTracker::onConfigure(FXObject *sender, FXSelector sel, void *data) {
    myPanel->onConfigure(sender, sel, data);
    return FXMainWindow::onConfigure(sender, sel, data);
}


long
GUIParameterTracker::onPaint(FXObject *sender, FXSelector sel, void *data) {
    myPanel->onPaint(sender, sel, data);
    return FXMainWindow::onPaint(sender, sel, data);
}


long
GUIParameterTracker::onSimStep(FXObject*,FXSelector,void*) {
    update();
    return 1;
}


long
GUIParameterTracker::onCmdChangeAggregation(FXObject*,FXSelector,void*) {
    int index = myAggregationInterval->getCurrentItem();
    size_t aggInt = 0;
    switch (index) {
    case 0:
        aggInt = 1;
        break;
    case 1:
        aggInt = 60;
        break;
    case 2:
        aggInt = 60 * 5;
        break;
    case 3:
        aggInt = 60 * 15;
        break;
    case 4:
        aggInt = 60 * 30;
        break;
    case 5:
        aggInt = 60 * 60;
        break;
    default:
        throw 1;
        break;
    }
    for (std::vector<TrackerValueDesc*>::iterator i1=myTracked.begin(); i1!=myTracked.end(); i1++) {
        (*i1)->setAggregationSpan(TIME2STEPS(aggInt));
    }
    return 1;
}


long
GUIParameterTracker::onCmdSave(FXObject*,FXSelector,void*) {
    FXString file = MFXUtils::getFilename2Write(this, "Save Data", ".csv", GUIIconSubSys::getIcon(ICON_EMPTY), gCurrentFolder);
    if (file=="") {
        return 1;
    }
    try {
        OutputDevice &dev = OutputDevice::getDevice(file.text());
        // write header
        std::vector<TrackerValueDesc*>::iterator i;
        dev << "# ";
        for (i=myTracked.begin(); i!=myTracked.end(); ++i) {
            if (i!=myTracked.begin()) {
                dev << ';';
            }
            TrackerValueDesc *tvd = *i;
            dev << tvd->getName();
        }
        dev << '\n';
        // count entries
        size_t max = 0;
        for (i=myTracked.begin(); i!=myTracked.end(); ++i) {
            TrackerValueDesc *tvd = *i;
            size_t sizei = tvd->getAggregatedValues().size();
            if (max<sizei) {
                max = sizei;
            }
            tvd->unlockValues();
        }
        // write entries
        for (unsigned int j=0; j<max; j++) {
            for (i=myTracked.begin(); i!=myTracked.end(); ++i) {
                if (i!=myTracked.begin()) {
                    dev << ';';
                }
                TrackerValueDesc *tvd = *i;
                dev << tvd->getAggregatedValues()[j];
                tvd->unlockValues();
            }
            dev << '\n';
        }
        dev.close();
    } catch (IOError &e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", e.what());
    }
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
    FXComposite *c, GUIMainWindow &app,
    GUIParameterTracker &parent) throw()
        : FXGLCanvas(c, app.getGLVisual(), app.getBuildGLCanvas(), (FXObject*) 0, (FXSelector) 0, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 300, 200),
        myParent(&parent), myApplication(&app) {}


GUIParameterTracker::GUIParameterTrackerPanel::~GUIParameterTrackerPanel() throw() {}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValues() throw() {
    pfSetScale((SUMOReal) 0.1);
    pfSetScaleXY((SUMOReal)(.1*300./myWidthInPixels), (SUMOReal)(.1*300./(SUMOReal) myHeightInPixels));
    //
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    size_t run = 0;
    for (std::vector<TrackerValueDesc*>::iterator i=myParent->myTracked.begin(); i!=myParent->myTracked.end(); i++) {
        TrackerValueDesc *desc = *i;
        drawValue(*desc,
                  (SUMOReal) myWidthInPixels / (SUMOReal) myParent->myTracked.size() *(SUMOReal) run);
        run++;
    }
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValue(TrackerValueDesc &desc,
        SUMOReal /*namePos*/) throw() {
    // apply scaling
    glPushMatrix();

    // apply the positiopn offset of the display
    glScaled(0.8, 0.8, 1);
    // apply value range scaling
    SUMOReal ys = (SUMOReal) 2.0 / (SUMOReal) desc.getRange();
    glScaled(1.0, ys, 1.0);
    glTranslated(-1.0, -desc.getYCenter(), 0);

    // set color
    const RGBColor &col = desc.getColor();
    SUMOReal red = (SUMOReal) col.red();
    SUMOReal green = (SUMOReal) col.green();
    SUMOReal blue = (SUMOReal) col.blue();
    // draw value bounderies
    // draw minimum boundary
    glBegin(GL_LINES);
    glVertex2d(0, desc.getMin());
    glVertex2d(2.0, desc.getMin());
    glEnd();
    glBegin(GL_LINES);
    glVertex2d(0, desc.getMax());
    glVertex2d(2.0, desc.getMax());
    glEnd();
    glColor4f(red, green, blue, 0.3f);
    for (int a=1; a<6; a++) {
        SUMOReal ypos = (desc.getRange()) / (SUMOReal) 6.0 * (SUMOReal) a + desc.getMin();
        glBegin(GL_LINES);
        glVertex2d(0, ypos);
        glVertex2d(2.0, ypos);
        glEnd();
    }
    const std::vector<SUMOReal> &values = desc.getAggregatedValues();
    SUMOReal latest = 0;
    if (values.size()<2) {
        glPopMatrix();
        desc.unlockValues();
        return;
    } else {
        latest = values.back();
        // init values
        SUMOReal xStep = (SUMOReal) 2.0 / (SUMOReal) values.size();
        std::vector<SUMOReal>::const_iterator i = values.begin();
        SUMOReal yp = (*i);
        SUMOReal xp = 0;
        i++;
        glColor4f(red, green, blue, 1.0f);
        for (; i!=values.end(); i++) {
            SUMOReal yn = (*i);
            SUMOReal xn = xp + xStep;
            glBegin(GL_LINES);
            glVertex2d(xp, yp);
            glVertex2d(xn, yn);
            glEnd();
            yp = yn;
            xp = xn;
        }
        desc.unlockValues();
        glPopMatrix();
    }

    // draw value bounderies and descriptions
    glColor3d(red, green, blue);

    // draw min time
    SUMOTime beginStep = desc.getRecordingBegin();
    std::string begStr = time2string(beginStep);
    SUMOReal w = pfdkGetStringWidth(begStr.c_str());
    glRotated(180, 1, 0, 0);
    pfSetPosition(0, 0);
    glTranslated(-0.8-w/2., 0.88, 0);
    pfDrawString(begStr.c_str());
    glTranslated(0.8+w/2., -0.88, 0);
    glRotated(-180, 1, 0, 0);

    // draw max time
    glRotated(180, 1, 0, 0);
    pfSetPosition(0, 0);
    glTranslated(0.75, 0.88, 0);
    pfDrawString(time2string(beginStep + static_cast<SUMOTime>(values.size() * desc.getAggregationSpan())).c_str());
    glTranslated(-0.75, -0.88, 0);
    glRotated(-180, 1, 0, 0);

    // draw min value
    glRotated(180, 1, 0, 0);
    pfSetPosition(0, 0);
    glTranslated(-0.98, 0.82, 0);
    pfDrawString(toString(desc.getMin()).c_str());
    glTranslated(0.98, -0.82, 0);
    glRotated(-180, 1, 0, 0);

    // draw max value
    glRotated(180, 1, 0, 0);
    pfSetPosition(0, 0);
    glTranslated(-0.98, -0.78, 0);
    pfDrawString(toString(desc.getMax()).c_str());
    glTranslated(0.98, 0.78, 0);
    glRotated(-180, 1, 0, 0);

    // draw current value
    glRotated(180, 1, 0, 0);
    pfSetPosition(0, 0);
    SUMOReal p = (SUMOReal) 0.8 -
                 ((SUMOReal) 1.6 / (desc.getMax()-desc.getMin()) * (latest-desc.getMin()));
    glTranslated(-0.98, p+.02, 0);
    pfDrawString(toString(latest).c_str());
    glTranslated(0.98, -(p+.02), 0);
    glRotated(-180, 1, 0, 0);

    // draw name
    glRotated(180, 1, 0, 0);
    pfSetPosition(0, 0);
    glTranslated(-0.98, -.92, 0);
    pfDrawString(desc.getName().c_str());
    glTranslated(0.98, .92, 0);
    glRotated(-180, 1, 0, 0);
}


long
GUIParameterTracker::GUIParameterTrackerPanel::onConfigure(FXObject*,
        FXSelector,void*) {
    if (makeCurrent()) {
        myWidthInPixels = myParent->getWidth();
        myHeightInPixels = myParent->getHeight();
        if (myWidthInPixels!=0&&myHeightInPixels!=0) {
            glViewport(0, 0, myWidthInPixels-1, myHeightInPixels-1);
            glClearColor(1.0, 1.0, 1.0, 1);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
            glEnable(GL_ALPHA_TEST);
            glDisable(GL_COLOR_MATERIAL);
            glLineWidth(1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        makeNonCurrent();
    }
    return 1;
}


long
GUIParameterTracker::GUIParameterTrackerPanel::onPaint(FXObject*,
        FXSelector,void*) {
    if (!isEnabled()) {
        return 1;
    }
    if (makeCurrent()) {
        myWidthInPixels = getWidth();
        myHeightInPixels = getHeight();
        if (myWidthInPixels!=0&&myHeightInPixels!=0) {
            glViewport(0, 0, myWidthInPixels-1, myHeightInPixels-1);
            glClearColor(1.0, 1.0, 1.0, 1);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
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
    return 1;
}



/****************************************************************************/

