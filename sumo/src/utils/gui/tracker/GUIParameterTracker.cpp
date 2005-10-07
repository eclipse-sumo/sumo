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
// Revision 1.9  2005/10/07 11:45:56  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:09:38  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:20:06  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/07/12 11:55:38  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.5  2005/05/04 09:22:26  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/12/20 14:00:25  dkrajzew
// debugging
//
// Revision 1.3  2004/12/16 12:21:38  dkrajzew
// debugging
//
// Revision 1.2  2004/11/29 09:22:58  dkrajzew
// possibility to save the timeline added
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:20:58  dksumo
// just some code beautyfying
//
// Revision 1.1  2004/10/22 12:50:55  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.15  2004/08/02 11:43:31  dkrajzew
// ported to fox 1.2; patched missing unlock on unwished program termination
//
// Revision 1.14  2004/07/02 08:26:11  dkrajzew
// aggregation debugged and saving option added
//
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <fstream>

#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include "GUIParameterTracker.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/common/SUMOTime.h>
#include <utils/glutils/polyfonts.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIParameterTracker) GUIParameterTrackerMap[]={
    FXMAPFUNC(SEL_CONFIGURE, 0,                       GUIParameterTracker::onConfigure),
    FXMAPFUNC(SEL_PAINT,     0,                       GUIParameterTracker::onPaint),
    FXMAPFUNC(SEL_COMMAND,   MID_SIMSTEP,             GUIParameterTracker::onSimStep),
    FXMAPFUNC(SEL_COMMAND,   GUIParameterTracker::MID_AGGREGATIONINTERVAL, GUIParameterTracker::onCmdChangeAggregation),
    FXMAPFUNC(SEL_COMMAND,   GUIParameterTracker::MID_SAVE, GUIParameterTracker::onCmdSave),

};

  // Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUIParameterTracker,FXMainWindow,GUIParameterTrackerMap,ARRAYNUMBER(GUIParameterTrackerMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParameterTracker::GUIParameterTracker(GUIMainWindow &app,
                                         const std::string &name)
    : FXMainWindow(app.getApp(),"Tracker",NULL,NULL,DECOR_ALL,20,20,300,200),
    myApplication(&app)
{
    buildToolBar();
    app.addChild(this, true);
    FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
    myPanel = new GUIParameterTrackerPanel(glcanvasFrame,
        *myApplication, *this);
    setTitle(name.c_str());
    setIcon( GUIIconSubSys::getIcon(ICON_APP_TRACKER) );
}


GUIParameterTracker::GUIParameterTracker(GUIMainWindow &app,
                                         const std::string &name,
                                         GUIGlObject &o,
                                         int xpos, int ypos)
    : FXMainWindow(app.getApp(),"Tracker",NULL,NULL,DECOR_ALL,20, 20,300,200),
    myApplication(&app)
{
    buildToolBar();
    app.addChild(this, true);
    FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,
        FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
        0,0,0,0,0,0,0,0);
    myPanel = new GUIParameterTrackerPanel(glcanvasFrame,
        *myApplication, *this);
    setTitle(name.c_str());
    setIcon( GUIIconSubSys::getIcon(ICON_APP_TRACKER) );
}


GUIParameterTracker::~GUIParameterTracker()
{
    myApplication->removeChild(this);
    for(TrackedVarsVector::iterator i1=myTracked.begin(); i1!=myTracked.end(); i1++) {
        delete (*i1);
    }
    // deleted by GUINet
    for(ValuePasserVector::iterator i2=myValuePassers.begin(); i2!=myValuePassers.end(); i2++) {
        delete (*i2);
    }
}


void
GUIParameterTracker::create()
{
    FXMainWindow::create();
    myToolBarDrag->create();
}


void
GUIParameterTracker::addVariable(GUIGlObject *o, const std::string &name,
								 size_t recordBegin)
{
    TrackerValueDesc *newTracked =
        new TrackerValueDesc(name, RGBColor(0, 0, 0), o, recordBegin);
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
GUIParameterTracker::buildToolBar()
{
    myToolBarDrag=new FXToolBarShell(this,FRAME_NORMAL);
    myToolBar = new FXToolBar(this,myToolBarDrag,
        LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);

    new FXToolBarGrip(myToolBar, myToolBar, FXToolBar::ID_TOOLBARGRIP,
        TOOLBARGRIP_DOUBLE);

    // save button
    new FXButton(myToolBar,"\t\tSave the data...",
        GUIIconSubSys::getIcon(ICON_SAVE), this, GUIParameterTracker::MID_SAVE,
        ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // aggregation interval combo
    myAggregationInterval =
        new FXComboBox(myToolBar, 8, this, MID_AGGREGATIONINTERVAL,
            FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
    myAggregationInterval->appendItem("1s");
    myAggregationInterval->appendItem("1min");
    myAggregationInterval->appendItem("5min");
    myAggregationInterval->appendItem("15min");
    myAggregationInterval->appendItem("30min");
    myAggregationInterval->appendItem("60min");
    myAggregationInterval->setNumVisible(6);
}


void
GUIParameterTracker::addTracked(GUIGlObject &o,
                                ValueSource<SUMOReal> *src,
                                TrackerValueDesc *newTracked)
{
    myTracked.push_back(newTracked);
    // build connection (is automatically set into an execution map)
    myValuePassers.push_back(new GLObjectValuePassConnector<SUMOReal>(o,
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


long
GUIParameterTracker::onCmdChangeAggregation(FXObject*,FXSelector,void*)
{
    int index = myAggregationInterval->getCurrentItem();
    size_t aggInt = 0;
    switch(index) {
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
    TrackedVarsVector::iterator i1;
    for(i1=myTracked.begin(); i1!=myTracked.end(); i1++) {
        (*i1)->setAggregationSpan(aggInt);
    }
    return 1;
}


long
GUIParameterTracker::onCmdSave(FXObject*,FXSelector,void*)
{
    // get the file name
    FXFileDialog opendialog(this, "Save Data As...");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.csv"); // cdd=CsvDevilData
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
        ofstream strm(file.c_str());
        if(!strm.good()) {
            return 1; // !!! inform the user
        }
        // write header
        TrackedVarsVector::iterator i;
        strm << "# ";
        for(i=myTracked.begin(); i!=myTracked.end(); ++i) {
            if(i!=myTracked.begin()) {
                strm << ';';
            }
            TrackerValueDesc *tvd = *i;
            strm << tvd->getName();
        }
        strm << endl;
        // count entries
        size_t max = 0;
        for(i=myTracked.begin(); i!=myTracked.end(); ++i) {
            TrackerValueDesc *tvd = *i;
            size_t sizei = tvd->getAggregatedValues().size();
            if(max<sizei) {
                max = sizei;
            }
            tvd->unlockValues();
        }
        // write entries
        for(unsigned int j=0; j<max; j++) {
            for(i=myTracked.begin(); i!=myTracked.end(); ++i) {
                if(i!=myTracked.begin()) {
                    strm << ';';
                }
                TrackerValueDesc *tvd = *i;
                strm << tvd->getAggregatedValues()[j];
                tvd->unlockValues();
            }
            strm << endl;
        }
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
        GUIParameterTracker &parent)
    : FXGLCanvas(c, app.getGLVisual(), app.getBuildGLCanvas(), (FXObject*) 0, (FXSelector) 0, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 300, 200),
    myParent(&parent), myApplication(&app)
{
}


GUIParameterTracker::GUIParameterTrackerPanel::~GUIParameterTrackerPanel()
{
    // just to quit cleanly on a failure
    if(_lock.locked()) {
        _lock.unlock();
    }
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValues()
{
    // compute which font to use
    /*
    SUMOReal fontIdx = ((SUMOReal) _widthInPixels-300.) / 10.;
    if(fontIdx<=0) fontIdx = 1;
    if(fontIdx>4) fontIdx = 4;
    */
    pfSetScale((SUMOReal) 0.1);
    pfSetScaleXY((SUMOReal) (.1*300./_widthInPixels), (SUMOReal) (.1*300./(SUMOReal) _heightInPixels));

//    GUITexturesHelper::getFontRenderer().SetActiveFont(4-fontIdx);
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
            (SUMOReal) _widthInPixels / (SUMOReal) myParent->myTracked.size() * (SUMOReal) run);
        run++;
    }
//    GUITexturesHelper::getFontRenderer().Draw(_widthInPixels, _heightInPixels);//this, width, height);
}


void
GUIParameterTracker::GUIParameterTrackerPanel::drawValue(TrackerValueDesc &desc,
                                                         SUMOReal namePos)
{
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
    glBegin( GL_LINES );
    glVertex2d(0, desc.getMin());
    glVertex2d(2.0, desc.getMin());
    glEnd();
    glBegin( GL_LINES );
    glVertex2d(0, desc.getMax());
    glVertex2d(2.0, desc.getMax());
    glEnd();
    glColor4f(red, green, blue, 0.3f);
    for(int a=1; a<6; a++) {
        SUMOReal ypos = (desc.getRange()) / (SUMOReal) 6.0 * (SUMOReal) a + desc.getMin();
        glBegin( GL_LINES );
        glVertex2d(0, ypos);
        glVertex2d(2.0, ypos);
        glEnd();
    }
    const std::vector<SUMOReal> &values = desc.getAggregatedValues();
    SUMOReal latest = 0;
    if(values.size()<2) {
        glPopMatrix();
        desc.unlockValues();
        return;
    } else {
        latest = values[values.size()-1];
        // init values
        SUMOReal xStep = (SUMOReal) 2.0 / (SUMOReal) values.size();
        std::vector<SUMOReal>::const_iterator i = values.begin();
        SUMOReal yp = (*i);
        SUMOReal xp = 0;
        i++;
        glColor4f(red, green, blue, 1.0f);
        for(; i!=values.end(); i++) {
            SUMOReal yn = (*i);
            SUMOReal xn = xp + xStep;
            glBegin( GL_LINES );
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
    glColor3f(red, green, blue);
//    GUITexturesHelper::getFontRenderer().SetColor(red*0.5f, green*0.5f, blue*0.5f);

    // draw min time
    SUMOTime beginStep = desc.getRecordingBegin();
    string begStr = StringUtils::trim((SUMOReal) beginStep, 2);
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
        pfDrawString(StringUtils::trim(
			(SUMOReal) beginStep + (SUMOReal) values.size() * (SUMOReal) desc.getAggregationSpan(), 2).c_str());
        glTranslated(-0.75, -0.88, 0);
    glRotated(-180, 1, 0, 0);

    // draw min value
    glRotated(180, 1, 0, 0);
        pfSetPosition(0, 0);
        glTranslated(-0.98, 0.82, 0);
        pfDrawString(StringUtils::trim(desc.getMin(), 2).c_str());
        glTranslated(0.98, -0.82, 0);
    glRotated(-180, 1, 0, 0);

    // draw max value
    glRotated(180, 1, 0, 0);
        pfSetPosition(0, 0);
        glTranslated(-0.98, -0.78, 0);
        pfDrawString(StringUtils::trim(desc.getMax(), 2).c_str());
        glTranslated(0.98, 0.78, 0);
    glRotated(-180, 1, 0, 0);

    // draw current value
    glRotated(180, 1, 0, 0);
        pfSetPosition(0, 0);
        SUMOReal p = (SUMOReal) -0.8 + ((SUMOReal) 1.6 / (desc.getMax()-desc.getMin()) * latest);
        glTranslated(-0.98, -p+.02, 0);
        pfDrawString(StringUtils::trim(latest, 2).c_str());
        glTranslated(0.98, p-.02, 0);
    glRotated(-180, 1, 0, 0);

    // draw name
    glRotated(180, 1, 0, 0);
        pfSetPosition(0, 0);
        glTranslated(-0.98, -.92, 0);
        pfDrawString(desc.getName().c_str());
        glTranslated(0.98, .92, 0);
    glRotated(-180, 1, 0, 0);

    /*

    GUITexturesHelper::getFontRenderer().StringOut(3,
        patchHeightVal(desc, desc.getMin()),
        StringUtils::trim(desc.getMin(), 2));
        // draw maximum boundary
    GUITexturesHelper::getFontRenderer().StringOut(3,
        patchHeightVal(desc, desc.getMax()),
        StringUtils::trim(desc.getMax(), 2));
        // draw some further lines
    glColor4f(red, green, blue, 0.3f);
    for(int a=1; a<6; a++) {
        SUMOReal ypos = (desc.getRange()) / 6.0 * (SUMOReal) a + desc.getMin();
        glBegin( GL_LINES );
        glVertex2d(0, ypos);
        glVertex2d(2.0, ypos);
        glEnd();
    }

    const std::vector<SUMOReal> &values = desc.getAggregatedValues();
    if(values.size()<2) {
        glPopMatrix();
        desc.unlockValues();
        return;
    }

    SUMOTime beginStep = desc.getRecordingBegin();

    // init values
    SUMOReal xStep = 2.0 / ((SUMOReal) values.size());
    std::vector<SUMOReal>::const_iterator i = values.begin();
    SUMOReal yp = (*i);
    SUMOReal xp = 0;
    i++;
    // set color
    glColor4f(red, green, blue, 1.0f);
    for(; i!=values.end(); i++) {
        SUMOReal yn = (*i);
        SUMOReal xn = xp + xStep;
        glBegin( GL_LINES );
        glVertex2d(xp, yp);
        glVertex2d(xn, yn);
        glEnd();
        yp = yn;
        xp = xn;
    }
    desc.unlockValues();
    glPopMatrix();
    // set the begin and the end time step
        // begin
    std::string val = StringUtils::trim(beginStep, 2);
    GUITexturesHelper::getFontRenderer().StringOut(
        (SUMOReal) (getWidth()/10.0*1.0
            - GUITexturesHelper::getFontRenderer().GetStringWidth(val)/2),
        (SUMOReal) (patchHeightVal(desc, desc.getMin())
            + GUITexturesHelper::getFontRenderer().GetHeight()),
        val);
        // end
    val = StringUtils::trim(beginStep + values.size()*desc.getAggregationSpan(), 2);
    GUITexturesHelper::getFontRenderer().StringOut(
        (SUMOReal) (getWidth()/10.0*9.0
            - GUITexturesHelper::getFontRenderer().GetStringWidth(val)/2),
        (SUMOReal) (patchHeightVal(desc, desc.getMin())
            + GUITexturesHelper::getFontRenderer().GetHeight()),
        val);
    // add current value string
    GUITexturesHelper::getFontRenderer().StringOut(3,
        patchHeightVal(desc, yp), StringUtils::trim(yp, 2));
    GUITexturesHelper::getFontRenderer().StringOut(namePos+3, 0,
        desc.getName());
        */
}


SUMOReal
GUIParameterTracker::GUIParameterTrackerPanel::patchHeightVal(TrackerValueDesc &desc,
                                                              SUMOReal d)
{
    SUMOReal height = (SUMOReal) _heightInPixels;
    SUMOReal range = (SUMOReal) desc.getRange();
    SUMOReal yoff = (SUMOReal) desc.getYCenter();
    SUMOReal abs = (height) * (((SUMOReal)d-yoff)/range) * 0.8f;
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


