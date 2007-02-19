/****************************************************************************/
/// @file    GUISUMOViewParent.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A window that controls the display(s) of the simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/geom/Position2D.h>
#include <utils/geom/Boundary.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSJunction.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUINet.h>

#include <string>
#include <vector>
#include "GUIGlobals.h"
#include "dialogs/GUIDialog_GLObjChooser.h"
#include "GUIViewTraffic.h"
#include "GUIApplicationWindow.h"
#include "GUISUMOViewParent.h"
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/foxtools/MFXImageHelper.h>

#ifdef HAVE_MESOSIM
#include <mesogui/GUIViewMesoEdges.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


const FXchar patterns[]=
    "\nGIF Image (*.gif)"
    "\nBMP Image (*.bmp)"
    "\nXPM Image (*.xpm)"
    "\nPCX Image (*.pcx)"
    "\nICO Image (*.ico)"
    "\nRGB Image  (*.rgb)"
    "\nXBM Image  (*.xbm)"
    "\nTARGA Image  (*.tga)"
#ifdef HAVE_PNG_H
    "\nPNG Image  (*.png)"
#endif
#ifdef HAVE_JPEG_H
    "\nJPEG Image (*.jpg)"
#endif
#ifdef HAVE_TIFF_H
    "\nTIFF Image (*.tif)"
#endif
    "All Image Files (*.gif, *.bmp, *.xpm, *.pcx, *.ico, *.rgb, *.xbm, *.tga, *.png, *.jpg, *.tif)"
    "All Files (*)"
    ;

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUISUMOViewParent) GUISUMOViewParentMap[]=
    {
        FXMAPFUNC(SEL_COMMAND,  MID_RECENTERVIEW,   GUISUMOViewParent::onCmdRecenterView),
        FXMAPFUNC(SEL_COMMAND,  MID_SHOWLEGEND,     GUISUMOViewParent::onCmdShowLegend),
        FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,   GUISUMOViewParent::onCmdMakeSnapshot),
        FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,  GUISUMOViewParent::onCmdAllowRotation),
        FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION, GUISUMOViewParent::onCmdLocateJunction),
        FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,     GUISUMOViewParent::onCmdLocateEdge),
        FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,  GUISUMOViewParent::onCmdLocateVehicle),
        FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,      GUISUMOViewParent::onCmdLocateAdd),
        FXMAPFUNC(SEL_COMMAND,  MID_SIMSTEP,        GUISUMOViewParent::onSimStep),

    };

// Object implementation
FXIMPLEMENT(GUISUMOViewParent, GUIGlChildWindow, GUISUMOViewParentMap, ARRAYNUMBER(GUISUMOViewParentMap))


// ===========================================================================
// member method definitions
// ===========================================================================
GUISUMOViewParent::GUISUMOViewParent(FXMDIClient* p,
                                     FXGLCanvas * /*share!!!*/,FXMDIMenu *mdimenu,
                                     const FXString& name, GUINet &/*net!!!*/,
                                     GUIMainWindow *parentWindow,
                                     ViewType /*view!!!*/, FXIcon* ic, FXPopup* /*pup!!!*/,
                                     FXuint opts,
                                     FXint /*x!!!*/, FXint /*y!!!*/, FXint /*w!!!*/, FXint /*h!!!*/)
        : GUIGlChildWindow(p, mdimenu, name, ic, 0, opts, 10, 10, 300, 200),
        myParent(parentWindow), _zoomingFactor(100),
        _showLegend(true), _allowRotation(false), _chooser(0)

{
    myParent->addChild(this, false);
}


void
GUISUMOViewParent::init(ViewType view, FXGLCanvas *share, GUINet &net)
{
    // Make MDI Window Menu
    setTracking();
    FXVerticalFrame *glcanvasFrame =
        new FXVerticalFrame(this,
                            FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
                            0,0,0,0,0,0,0,0);
    // build the tool bar
    buildToolBar(glcanvasFrame);
    switch (view) {
    default:
    case MICROSCOPIC_VIEW:
        if (share!=0) {
            _view =
                new GUIViewTraffic(glcanvasFrame, *myParent, this, net,
                                   myParent->getGLVisual(), share);
        } else {
            _view =
                new GUIViewTraffic(glcanvasFrame, *myParent, this, net,
                                   myParent->getGLVisual());
        }
        break;
#ifdef HAVE_MESOSIM
    case EDGE_MESO_VIEW:
        if (share!=0) {
            _view =
                new GUIViewMesoEdges(glcanvasFrame, *myParent, this,
                                     net, myParent->getGLVisual(), share);
        } else {
            _view =
                new GUIViewMesoEdges(glcanvasFrame, *myParent, this,
                                     net, myParent->getGLVisual());
        }
        break;
#endif
    }
    _view->buildViewToolBars(*this);
}


GUISUMOViewParent::~GUISUMOViewParent()
{
    myParent->removeChild(this);
    delete myToolBar;
//    delete myToolBarDrag;
}


void
GUISUMOViewParent::buildToolBar(FXComposite *c)
{
    myToolBar = new FXToolBar(c,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);

    // build the view settings
    // recenter view
    new FXButton(myToolBar,
                 "\tRecenter View\tRecenter view to the simulated Area.",
                 GUIIconSubSys::getIcon(ICON_RECENTERVIEW), this, MID_RECENTERVIEW,
                 ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // show legend
    new MFXCheckableButton(_showLegend,
                           myToolBar,"\tShow Legend\tToggle whether the Legend shall be shown.",
                           GUIIconSubSys::getIcon(ICON_SHOWLEGEND), this, MID_SHOWLEGEND,
                           ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // make snapshot
    new FXButton(myToolBar,
                 "\tMake Snapshot\tMakes a snapshot of the view.",
                 GUIIconSubSys::getIcon(ICON_CAMERA), this, MID_MAKESNAPSHOT,
                 ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    // allow rotation
    /*    new MFXCheckableButton(_allowRotation,
            myToolBar,"\tAllow Rotation\tToggle whether Scene rotation is allowed.",
            GUIIconSubSys::getIcon(ICON_ALLOWROTATION), this, MID_ALLOWROTATION,
            ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
    */
}


void
GUISUMOViewParent::create()
{
    FXMDIChild::create();
    _view->create();
    myToolBar->create();
}


long
GUISUMOViewParent::onCmdRecenterView(FXObject*,FXSelector,void*)
{
    _zoomingFactor = 100;
    _view->recenterView();
    _view->update();
    return 1;
}


long
GUISUMOViewParent::onCmdShowLegend(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _showLegend = button->amChecked();
    _view->update();
    return 1;
}


long
GUISUMOViewParent::onCmdMakeSnapshot(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this, "Save Snapshot");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList(patterns);
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if (!opendialog.execute()) {
        return 1;
    }
    gCurrentFolder = opendialog.getDirectory().text();
    string file = opendialog.getFilename().text();
    FXColor *buf = _view->getSnapshot();
    // mirror
    size_t mwidth = _view->getWidth();
    size_t mheight = _view->getHeight();
    FXColor *paa = buf;
    FXColor *pbb = buf + mwidth * (mheight-1);
    do {
        FXColor *pa=paa;
        paa+=mwidth;
        FXColor *pb=pbb;
        pbb-=mwidth;
        do {
            FXColor t=*pa;
            *pa++=*pb;
            *pb++=t;
        } while (pa<paa);
    } while (paa<pbb);
    // save
    try {
        MFXImageHelper::saveimage(getApp(), file,
                                  _view->getWidth(), _view->getHeight(), buf);
    } catch (...) {
        string msg = "Could not save '" + file + "'.\nMaybe the extension is unknown.";
        FXMessageBox::error(this, MBOX_OK, "Saving failed.",
                            msg.c_str());
    }
    /*
    // Save the image.
    FXFileStream stream;
    stream.open(file.c_str(), FXStreamSave);
    fxsaveBMP(stream, buf, _view->getWidth(), _view->getHeight());
    */
    FXFREE(&buf);

    return 1;
}


long
GUISUMOViewParent::onCmdAllowRotation(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _allowRotation = button->amChecked();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateJunction(FXObject *,FXSelector,void*)
{
    _view->getLocatorPopup(*this)->popdown();
    _view->getLocatorPopup(*this)->killFocus();
    _view->getLocatorPopup(*this)->lower();
    _view->getLocatorPopup(*this)->update();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_JUNCTION, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateEdge(FXObject *sender,FXSelector,void*)
{
    static_cast<FXButton*>(sender)->getParent()->hide();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_EDGE, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateVehicle(FXObject *sender,FXSelector,void*)
{
    static_cast<FXButton*>(sender)->getParent()->hide();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_VEHICLE, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUISUMOViewParent::onCmdLocateAdd(FXObject *sender,FXSelector,void*)
{
    static_cast<FXButton*>(sender)->getParent()->hide();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_ADDITIONAL, gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


SUMOReal
GUISUMOViewParent::getZoomingFactor() const
{
    return _zoomingFactor;
}


void
GUISUMOViewParent::setZoomingFactor(SUMOReal val)
{
    _zoomingFactor = val;
}


void
GUISUMOViewParent::setView(GUIGlObject *o)
{
    _view->centerTo(o);
}


bool
GUISUMOViewParent::showLegend() const
{
    return _showLegend;
}


bool
GUISUMOViewParent::allowRotation() const
{
    return _allowRotation;
}


int
GUISUMOViewParent::getMaxGLWidth() const
{
    return myParent->getMaxGLWidth();
}

int
GUISUMOViewParent::getMaxGLHeight() const
{
    return myParent->getMaxGLHeight();
}


long
GUISUMOViewParent::onSimStep(FXObject*,FXSelector,void*)
{
    _view->update();
    return 1;
}



/****************************************************************************/

