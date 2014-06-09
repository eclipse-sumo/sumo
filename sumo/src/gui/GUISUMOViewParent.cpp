/****************************************************************************/
/// @file    GUISUMOViewParent.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Andreas Gaubatz
/// @date    Sept 2002
/// @version $Id$
///
// A single child window which contains a view of the simulation area
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <vector>
#include <guisim/GUIShapeContainer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_GLObjChooser.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUINet.h>
#include <guisim/GUIVehicleControl.h>
#include <microsim/MSJunction.h>
#include "GUIGlobals.h"
#include "GUIViewTraffic.h"
#include "GUIApplicationWindow.h"
#include "GUISUMOViewParent.h"

#ifdef HAVE_OSG
#include <osgview/GUIOSGView.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUISUMOViewParent) GUISUMOViewParentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,   GUISUMOViewParent::onCmdMakeSnapshot),
    //        FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,  GUISUMOViewParent::onCmdAllowRotation),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION, GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,     GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,  GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,      GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,      GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,      GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,     GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMSTEP,        GUISUMOViewParent::onSimStep),

};

// Object implementation
FXIMPLEMENT(GUISUMOViewParent, GUIGlChildWindow, GUISUMOViewParentMap, ARRAYNUMBER(GUISUMOViewParentMap))


// ===========================================================================
// member method definitions
// ===========================================================================
GUISUMOViewParent::GUISUMOViewParent(FXMDIClient* p, FXMDIMenu* mdimenu,
                                     const FXString& name,
                                     GUIMainWindow* parentWindow,
                                     FXIcon* ic, FXuint opts,
                                     FXint x, FXint y, FXint w, FXint h)
    : GUIGlChildWindow(p, parentWindow, mdimenu, name, ic, opts, x, y, w, h) {
    myParent->addChild(this, false);
}


GUISUMOAbstractView*
GUISUMOViewParent::init(FXGLCanvas* share, GUINet& net, GUISUMOViewParent::ViewType type) {
    switch (type) {
        default:
        case VIEW_2D_OPENGL:
            myView = new GUIViewTraffic(myContentFrame, *myParent, this, net, myParent->getGLVisual(), share);
            break;
#ifdef HAVE_OSG
        case VIEW_3D_OSG:
            myView = new GUIOSGView(myContentFrame, *myParent, this, net, myParent->getGLVisual(), share);
            break;
#endif
    }
    myView->buildViewToolBars(*this);
    if (myParent->isGaming()) {
        myNavigationToolBar->hide();
    }
    return myView;
}


GUISUMOViewParent::~GUISUMOViewParent() {
    myParent->removeChild(this);
}


void
GUISUMOViewParent::toggleGaming() {
    if (myParent->isGaming()) {
        myNavigationToolBar->hide();
    } else {
        myNavigationToolBar->show();
    }
}


long
GUISUMOViewParent::onCmdMakeSnapshot(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Save Snapshot");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("All Image Files (*.gif, *.bmp, *.xpm, *.pcx, *.ico, *.rgb, *.xbm, *.tga, *.png, *.jpg, *.jpeg, *.tif, *.tiff, *.ps, *.eps, *.pdf, *.svg, *.tex, *.pgf)\n"
                              "GIF Image (*.gif)\nBMP Image (*.bmp)\nXPM Image (*.xpm)\nPCX Image (*.pcx)\nICO Image (*.ico)\n"
                              "RGB Image (*.rgb)\nXBM Image (*.xbm)\nTARGA Image (*.tga)\nPNG Image  (*.png)\n"
                              "JPEG Image (*.jpg, *.jpeg)\nTIFF Image (*.tif, *.tiff)\n"
                              "Postscript (*.ps)\nEncapsulated Postscript (*.eps)\nPortable Document Format (*.pdf)\n"
                              "Scalable Vector Graphics (*.svg)\nLATEX text strings (*.tex)\nPortable LaTeX Graphics (*.pgf)\n"
                              "All Files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute() || !MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    gCurrentFolder = opendialog.getDirectory();
    std::string file = opendialog.getFilename().text();
    std::string error = myView->makeSnapshot(file);
    if (error != "") {
        FXMessageBox::error(this, MBOX_OK, "Saving failed.", "%s", error.c_str());
    }
    return 1;
}


long
GUISUMOViewParent::onCmdLocate(FXObject*, FXSelector sel, void*) {
    GUIGlObjectType type;
    std::vector<GUIGlID> ids;
    GUIIcon icon;
    std::string title;
    switch (FXSELID(sel)) {
        case MID_LOCATEJUNCTION:
            type = GLO_JUNCTION;
            ids = static_cast<GUINet*>(GUINet::getInstance())->getJunctionIDs(myParent->listInternal());
            icon = ICON_LOCATEJUNCTION;
            title = "Junction Chooser";
            break;
        case MID_LOCATEEDGE:
            type = GLO_EDGE;
            ids = GUIEdge::getIDs(myParent->listInternal());
            icon = ICON_LOCATEEDGE;
            title = "Edge Chooser";
            break;
        case MID_LOCATEVEHICLE:
            type = GLO_VEHICLE;
            static_cast<GUIVehicleControl&>(MSNet::getInstance()->getVehicleControl()).insertVehicleIDs(ids);
            icon = ICON_LOCATEVEHICLE;
            title = "Vehicle Chooser";
            break;
        case MID_LOCATETLS:
            type = GLO_TLLOGIC;
            ids = static_cast<GUINet*>(GUINet::getInstance())->getTLSIDs();
            icon = ICON_LOCATETLS;
            title = "Traffic Lights Chooser";
            break;
        case MID_LOCATEADD:
            type = GLO_ADDITIONAL;
            ids = GUIGlObject_AbstractAdd::getIDList();
            icon = ICON_LOCATEADD;
            title = "Additional Objects Chooser";
            break;
        case MID_LOCATEPOI:
            type = GLO_POI;
            ids = static_cast<GUIShapeContainer&>(GUINet::getInstance()->getShapeContainer()).getPOIIds();
            icon = ICON_LOCATEPOI;
            title = "POI Chooser";
            break;
        case MID_LOCATEPOLY:
            type = GLO_POLYGON;
            ids = static_cast<GUIShapeContainer&>(GUINet::getInstance()->getShapeContainer()).getPolygonIDs();
            icon = ICON_LOCATEPOLY;
            title = "Polygon Chooser";
            break;
        default:
            throw ProcessError("Unknown Message ID in onCmdLocate");
    }
    myLocatorPopup->popdown();
    myLocatorButton->killFocus();
    myLocatorPopup->update();
    GUIDialog_GLObjChooser* chooser = new GUIDialog_GLObjChooser(
        this, GUIIconSubSys::getIcon(icon), title.c_str(), type, ids, GUIGlObjectStorage::gIDStorage);
    chooser->create();
    chooser->show();
    return 1;
}


long
GUISUMOViewParent::onSimStep(FXObject*, FXSelector, void*) {
    myView->update();
    myView->checkSnapshots();
    return 1;
}


bool
GUISUMOViewParent::isSelected(GUIGlObject* o) const {
    GUIGlObjectType type = o->getType();
    if (gSelected.isSelected(type, o->getGlID())) {
        return true;
    } else if (type == GLO_EDGE) {
        GUIEdge* edge = dynamic_cast<GUIEdge*>(o);
        if (edge == 0) {
            // hmph, just some security stuff
            return false;
        }
        const std::vector<MSLane*>& lanes = edge->getLanes();
        for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
            GUILane* l = dynamic_cast<GUILane*>(*j);
            if (l != 0 && gSelected.isSelected(GLO_LANE, l->getGlID())) {
                return true;
            }
        }
        return false;
    } else {
        return false;
    }
}


/****************************************************************************/

