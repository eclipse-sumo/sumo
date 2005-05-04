#include "GUIMainWindow.h"
#include <algorithm>
#include "GUIAppEnum.h"
#include <fx3d.h>

GUIMainWindow::GUIMainWindow(FXApp* a, int glWidth, int glHeight)
    : FXMainWindow(a,"SUMO-gui main window",NULL,NULL,DECOR_ALL,20,20,600,400),
    myGLVisual(new FXGLVisual(a, VISUAL_DOUBLEBUFFER|VISUAL_STEREO)),
    myGLWidth(glWidth), myGLHeight(glHeight)
{

    FXFontDesc fdesc;
    getApp()->getNormalFont()->getFontDesc(fdesc);
    fdesc.weight = FONTWEIGHT_BOLD;
    myBoldFont = new FXFont(getApp(), fdesc);

    myTopDock=new FXDockSite(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
    myBottomDock=new FXDockSite(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
    myLeftDock=new FXDockSite(this,LAYOUT_SIDE_LEFT|LAYOUT_FILL_Y);
    myRightDock=new FXDockSite(this,LAYOUT_SIDE_RIGHT|LAYOUT_FILL_Y);
}


GUIMainWindow::~GUIMainWindow()
{
}



void
GUIMainWindow::addChild(FXMDIChild *child, bool updateOnSimStep)
{
    mySubWindows.push_back(child);
}


void
GUIMainWindow::removeChild(FXMDIChild *child)
{
    std::vector<FXMDIChild*>::iterator i =
        std::find(mySubWindows.begin(), mySubWindows.end(), child);
    if(i!=mySubWindows.end()) {
        mySubWindows.erase(i);
    }
}


void
GUIMainWindow::addChild(FXMainWindow *child, bool updateOnSimStep)
{
    myTrackerLock.lock();
    myTrackerWindows.push_back(child);
    myTrackerLock.unlock();
}


void
GUIMainWindow::removeChild(FXMainWindow *child)
{
    myTrackerLock.lock();
    std::vector<FXMainWindow*>::iterator i =
        std::find(myTrackerWindows.begin(), myTrackerWindows.end(), child);
    myTrackerWindows.erase(i);
    myTrackerLock.unlock();
}


FXFont *
GUIMainWindow::getBoldFont()
{
    return myBoldFont;
}


int
GUIMainWindow::getMaxGLWidth() const
{
    return myGLWidth;
}


int
GUIMainWindow::getMaxGLHeight() const
{
    return myGLHeight;
}

void
GUIMainWindow::updateChildren()
{
    // inform views
    myMDIClient->forallWindows(this, FXSEL(SEL_COMMAND, MID_SIMSTEP), 0);
    // inform other windows
    myTrackerLock.lock();
    for(size_t i=0; i<myTrackerWindows.size(); i++) {
        myTrackerWindows[i]->handle(this,FXSEL(SEL_COMMAND,MID_SIMSTEP), 0);
    }
    myTrackerLock.unlock();
}

FXGLVisual *
GUIMainWindow::getGLVisual() const
{
    return myGLVisual;
}


