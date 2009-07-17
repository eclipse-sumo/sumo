/****************************************************************************/
/// @file    GUISUMOViewParent.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single child window which contains a view of the simulation area
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/common/UtilExceptions.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUISUMOViewParent) GUISUMOViewParentMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,   GUISUMOViewParent::onCmdMakeSnapshot),
    //        FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,  GUISUMOViewParent::onCmdAllowRotation),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION, GUISUMOViewParent::onCmdLocateJunction),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,     GUISUMOViewParent::onCmdLocateEdge),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,  GUISUMOViewParent::onCmdLocateVehicle),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,      GUISUMOViewParent::onCmdLocateTLS),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,      GUISUMOViewParent::onCmdLocateAdd),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATESHAPE,    GUISUMOViewParent::onCmdLocateShape),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMSTEP,        GUISUMOViewParent::onSimStep),

};

// Object implementation
FXIMPLEMENT(GUISUMOViewParent, GUIGlChildWindow, GUISUMOViewParentMap, ARRAYNUMBER(GUISUMOViewParentMap))


// ===========================================================================
// member method definitions
// ===========================================================================
GUISUMOViewParent::GUISUMOViewParent(FXMDIClient* p, FXMDIMenu *mdimenu,
                                     const FXString& name,
                                     GUIMainWindow *parentWindow,
                                     FXIcon* ic, FXuint opts,
                                     FXint x, FXint y, FXint w, FXint h)
        : GUIGlChildWindow(p, mdimenu, name, ic, opts, x, y, w, h),
        myParent(parentWindow)

{
    myParent->addChild(this, false);
}


GUISUMOAbstractView*
GUISUMOViewParent::init(FXGLCanvas *share, GUINet &net) {
    myView = new GUIViewTraffic(myContentFrame, *myParent, this, net,
                                myParent->getGLVisual(), share);
    myView->buildViewToolBars(*this);
    return myView;
}


GUISUMOViewParent::~GUISUMOViewParent() {
    myParent->removeChild(this);
//    delete myToolBarDrag;
}


void
GUISUMOViewParent::create() {
    GUIGlChildWindow::create();
}


long
GUISUMOViewParent::onCmdMakeSnapshot(FXObject*,FXSelector,void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Save Snapshot");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("All Image Files (*.gif, *.bmp, *.xpm, *.pcx, *.ico, *.rgb, *.xbm, *.tga, *.png, *.jpg, *.jpeg, *.tif, *.tiff)\n"
                              "GIF Image (*.gif)\nBMP Image (*.bmp)\nXPM Image (*.xpm)\nPCX Image (*.pcx)\nICO Image (*.ico)\n"
                              "RGB Image (*.rgb)\nXBM Image (*.xbm)\nTARGA Image (*.tga)\nPNG Image  (*.png)\n"
                              "JPEG Image (*.jpg, *.jpeg)\nTIFF Image (*.tif, *.tiff)\nAll Files (*)");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute()||!MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    gCurrentFolder = opendialog.getDirectory();
    std::string file = opendialog.getFilename().text();
    FXColor *buf = myView->getSnapshot();
    // save
    try {
        MFXImageHelper::saveImage(file, myView->getWidth(), myView->getHeight(), buf);
    } catch (InvalidArgument e) {
        std::string msg = "Could not save '" + file + "'.\n" + e.what();
        FXMessageBox::error(this, MBOX_OK, "Saving failed.", msg.c_str());
    }
    FXFREE(&buf);
    return 1;
}

void
GUISUMOViewParent::showLocator(GUIGlObjectType type) {
    myLocatorPopup->popdown();
    myLocatorButton->killFocus();
    myLocatorPopup->update();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, type, GUIGlObjectStorage::gIDStorage);
    chooser->create();
    chooser->show();
}


long
GUISUMOViewParent::onCmdLocateJunction(FXObject *,FXSelector,void*) {
    showLocator(GLO_JUNCTION);
    return 1;
}


long
GUISUMOViewParent::onCmdLocateEdge(FXObject *,FXSelector,void*) {
    showLocator(GLO_EDGE);
    return 1;
}


long
GUISUMOViewParent::onCmdLocateVehicle(FXObject *,FXSelector,void*) {
    showLocator(GLO_VEHICLE);
    return 1;
}


long
GUISUMOViewParent::onCmdLocateTLS(FXObject *,FXSelector,void*) {
    showLocator(GLO_TLLOGIC);
    return 1;
}


long
GUISUMOViewParent::onCmdLocateAdd(FXObject *,FXSelector,void*) {
    showLocator(GLO_ADDITIONAL);
    return 1;
}


long
GUISUMOViewParent::onCmdLocateShape(FXObject *sender,FXSelector,void*) {
    myLocatorPopup->popdown();
    myLocatorButton->killFocus();
    myLocatorPopup->update();
    static_cast<FXButton*>(sender)->getParent()->hide();
    GUIDialog_GLObjChooser *chooser =
        new GUIDialog_GLObjChooser(this, GLO_SHAPE, GUIGlObjectStorage::gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


void
GUISUMOViewParent::setView(GUIGlObject *o) {
    myView->centerTo(o);
}


long
GUISUMOViewParent::onSimStep(FXObject*,FXSelector,void*) {
    myView->update();
    myView->checkSnapshots();
    return 1;
}



/****************************************************************************/

