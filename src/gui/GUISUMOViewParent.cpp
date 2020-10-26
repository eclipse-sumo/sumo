/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUISUMOViewParent.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Andreas Gaubatz
/// @date    Sept 2002
///
// A single child window which contains a view of the simulation area
/****************************************************************************/
#include <config.h>

#include <string>
#include <vector>
#include <fxkeys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/gui/images/GUIIcons.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_GLObjChooser.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIPerson.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUINet.h>
#include <guisim/GUIVehicleControl.h>
#include <guisim/GUITransportableControl.h>
#include <microsim/MSJunction.h>
#include <microsim/MSGlobals.h>

#include "GUIGlobals.h"
#include "GUIViewTraffic.h"
#include "GUIApplicationWindow.h"
#include "GUISUMOViewParent.h"

#include <mesogui/GUIMEVehicleControl.h>

#ifdef HAVE_OSG
#include <osgview/GUIOSGView.h>
#endif

#define SPEEDFACTOR_SCALE 100.0

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUISUMOViewParent) GUISUMOViewParentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,   GUISUMOViewParent::onCmdMakeSnapshot),
    //        FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,  GUISUMOViewParent::onCmdAllowRotation),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION, GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,     GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,  GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPERSON,   GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATECONTAINER, GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,      GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,      GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,      GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,     GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_SPEEDFACTOR,    GUISUMOViewParent::onUpdSpeedFactor),
    FXMAPFUNC(SEL_COMMAND,  MID_SPEEDFACTOR,    GUISUMOViewParent::onCmdSpeedFactor),
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
                                     FXint x, FXint y, FXint w, FXint h) :
    GUIGlChildWindow(p, parentWindow, mdimenu, name, nullptr, ic, opts, x, y, w, h) {
    buildSpeedControlToolbar();
    myParent->addGLChild(this);
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
    myView->buildViewToolBars(this);
    if (myParent->isGaming()) {
        myStaticNavigationToolBar->hide();
    }
    return myView;
}


GUISUMOViewParent::~GUISUMOViewParent() {
    myParent->removeGLChild(this);
}


void
GUISUMOViewParent::setToolBarVisibility(const bool value) {
    if (value) {
        myStaticNavigationToolBar->show();
    } else {
        myStaticNavigationToolBar->hide();
    }
}


void 
GUISUMOViewParent::eraseGLObjChooser(GUIDialog_GLObjChooser* GLObjChooser) {
    if (GLObjChooser == nullptr) {
        throw ProcessError("ChooserDialog already deleted");
    } else if (GLObjChooser == myGLObjChooser.ACChooserJunction) {
        myGLObjChooser.ACChooserJunction = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserEdges) {
        myGLObjChooser.ACChooserEdges = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserVehicles) {
        myGLObjChooser.ACChooserVehicles = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserPersons) {
        myGLObjChooser.ACChooserPersons = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserRoutes) {
        myGLObjChooser.ACChooserRoutes = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserStops) {
        myGLObjChooser.ACChooserStops = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserTLS) {
        myGLObjChooser.ACChooserTLS = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserContainer) {
        myGLObjChooser.ACChooserContainer = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserAdditional) {
        myGLObjChooser.ACChooserAdditional = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserPOI) {
        myGLObjChooser.ACChooserPOI = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserPolygon) {
        myGLObjChooser.ACChooserPolygon = nullptr;
    } else if (GLObjChooser == myGLObjChooser.ACChooserProhibition) {
        myGLObjChooser.ACChooserProhibition = nullptr;
    } else {
        throw ProcessError("Unregistered chooserDialog");
    }
}


long
GUISUMOViewParent::onCmdMakeSnapshot(FXObject* sender, FXSelector, void*) {
    MFXCheckableButton* button = dynamic_cast<MFXCheckableButton*>(sender);
    // check if cast was sucesfully
    if (button) {
        if (button->amChecked()) {
            myView->endSnapshot();
            button->setChecked(false);
            return 1;
        }
        // get the new file name
        FXFileDialog opendialog(this, "Save Snapshot");
        opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::EMPTY));
        opendialog.setSelectMode(SELECTFILE_ANY);
#ifdef HAVE_FFMPEG
        opendialog.setPatternList("All Image and Video Files (*.gif,*.bmp,*.xpm,*.pcx,*.ico,*.rgb,*.xbm,*.tga,*.png,*.jpg,*.jpeg,*.tif,*.tiff,*.ps,*.eps,*.pdf,*.svg,*.tex,*.pgf,*.h264,*.hevc)\n"
                                  "All Video Files (*.h264,*.hevc)\n"
#else
        opendialog.setPatternList("All Image Files (*.gif,*.bmp,*.xpm,*.pcx,*.ico,*.rgb,*.xbm,*.tga,*.png,*.jpg,*.jpeg,*.tif,*.tiff,*.ps,*.eps,*.pdf,*.svg,*.tex,*.pgf)\n"
#endif
                                  "GIF Image (*.gif)\nBMP Image (*.bmp)\nXPM Image (*.xpm)\nPCX Image (*.pcx)\nICO Image (*.ico)\n"
                                  "RGB Image (*.rgb)\nXBM Image (*.xbm)\nTARGA Image (*.tga)\nPNG Image  (*.png)\n"
                                  "JPEG Image (*.jpg,*.jpeg)\nTIFF Image (*.tif,*.tiff)\n"
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
        if (error == "video") {
            button->setChecked(!button->amChecked());
        } else if (error != "") {
            FXMessageBox::error(this, MBOX_OK, "Saving failed.", "%s", error.c_str());
        }
    }
    return 1;
}


long
GUISUMOViewParent::onCmdLocate(FXObject*, FXSelector sel, void*) {
    switch (FXSELID(sel)) {
        case MID_LOCATEJUNCTION: {
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserJunction) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserJunction->restore();
                myGLObjChooser.ACChooserJunction->setFocus();
            } else {
                myGLObjChooser.ACChooserJunction = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), "Junction Chooser", 
                    static_cast<GUINet*>(GUINet::getInstance())->getJunctionIDs(myParent->listInternal()), 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATEEDGE: {
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserEdges) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserEdges->restore();
                myGLObjChooser.ACChooserEdges->setFocus();
            } else {
                myGLObjChooser.ACChooserEdges = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), "Edge Chooser", 
                    GUIEdge::getIDs(myParent->listInternal()), 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATEVEHICLE: {
            // get vehicles
            std::vector<GUIGlID> vehicles;
            if (MSGlobals::gUseMesoSim) {
                static_cast<GUIMEVehicleControl*>(static_cast<GUINet*>(MSNet::getInstance())->getGUIMEVehicleControl())->insertVehicleIDs(vehicles);
            } else {
                static_cast<GUIVehicleControl&>(MSNet::getInstance()->getVehicleControl()).insertVehicleIDs(
                    vehicles, myParent->listParking(), myParent->listTeleporting());
            }
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserVehicles) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserVehicles->restore();
                myGLObjChooser.ACChooserVehicles->setFocus();
            } else {
                myGLObjChooser.ACChooserVehicles = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), "Vehicle Chooser", 
                    vehicles, 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATEPERSON: {
            // get persons
            std::vector<GUIGlID> persons;
            static_cast<GUITransportableControl&>(MSNet::getInstance()->getPersonControl()).insertIDs(persons);
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserPersons) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserPersons->restore();
                myGLObjChooser.ACChooserPersons->setFocus();
            } else {
                myGLObjChooser.ACChooserPersons = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), "Person Chooser", 
                    persons, 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATECONTAINER: {
            // get containers
            std::vector<GUIGlID> containers;
            static_cast<GUITransportableControl&>(MSNet::getInstance()->getContainerControl()).insertIDs(containers);
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserContainer) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserContainer->restore();
                myGLObjChooser.ACChooserContainer->setFocus();
            } else {
                myGLObjChooser.ACChooserContainer = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATECONTAINER), "Container Chooser", 
                    containers, 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATETLS: {
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserTLS) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserTLS->restore();
                myGLObjChooser.ACChooserTLS->setFocus();
            } else {
                myGLObjChooser.ACChooserTLS = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), "Traffic Lights Chooser", 
                    static_cast<GUINet*>(GUINet::getInstance())->getTLSIDs(), 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATEADD: {
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserAdditional) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserAdditional->restore();
                myGLObjChooser.ACChooserAdditional->setFocus();
            } else {
                myGLObjChooser.ACChooserAdditional = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), "Additional Objects Chooser", 
                    GUIGlObject_AbstractAdd::getIDList(GLO_ADDITIONALELEMENT), 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATEPOI: {
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserPOI) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserPOI->restore();
                myGLObjChooser.ACChooserPOI->setFocus();
            } else {
                myGLObjChooser.ACChooserPOI = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), "POI Chooser", 
                    static_cast<GUIShapeContainer&>(GUINet::getInstance()->getShapeContainer()).getPOIIds(), 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        case MID_LOCATEPOLY: {
            // check if dialog is already opened
            if (myGLObjChooser.ACChooserPolygon) {
                // restore focus in the existent chooser dialog
                myGLObjChooser.ACChooserPolygon->restore();
                myGLObjChooser.ACChooserPolygon->setFocus();
            } else {
                myGLObjChooser.ACChooserPolygon = new GUIDialog_GLObjChooser(this,
                    GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), "Polygon Chooser", 
                    static_cast<GUIShapeContainer&>(GUINet::getInstance()->getShapeContainer()).getPolygonIDs(), 
                    GUIGlObjectStorage::gIDStorage);
            }
            break;
        }
        default:
            throw ProcessError("Unknown Message ID in onCmdLocate");
    }
    myLocatorPopup->popdown();
    myLocatorButton->killFocus();
    myLocatorPopup->update();
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
        if (edge == nullptr) {
            // hmph, just some security stuff
            return false;
        }
        const std::vector<MSLane*>& lanes = edge->getLanes();
        for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
            GUILane* l = dynamic_cast<GUILane*>(*j);
            if (l != nullptr && gSelected.isSelected(GLO_LANE, l->getGlID())) {
                return true;
            }
        }
        return false;
    } else {
        return false;
    }
}


long
GUISUMOViewParent::onKeyPress(FXObject* o, FXSelector sel, void* ptr) {
    myView->onKeyPress(o, sel, ptr);
    return 0;
}


long
GUISUMOViewParent::onKeyRelease(FXObject* o, FXSelector sel, void* ptr) {
    myView->onKeyRelease(o, sel, ptr);
    return 0;
}


void
GUISUMOViewParent::buildSpeedControlToolbar() {
    auto toolbar = myGripNavigationToolbar ? myGripNavigationToolbar : myStaticNavigationToolBar;
    new FXVerticalSeparator(toolbar, GUIDesignVerticalSeparator);

    //myToolBarDragSpeed = new FXToolBarShell(this, GUIDesignToolBar);
    //myToolBarSpeed = new FXToolBar(toolbar, myToolBarDragSpeed, GUIDesignToolBarRaisedSameTop);
    //mySpeedFactorSlider = new FXSlider(myToolBarSpeed, this, MID_SPEEDFACTOR, LAYOUT_FIX_WIDTH | SLIDER_ARROW_UP | SLIDER_TICKS_TOP, 0, 0, 300, 10, 0, 0, 5, 0);
    mySpeedFactorSlider = new FXSlider(toolbar, this, MID_SPEEDFACTOR, LAYOUT_FIX_WIDTH | SLIDER_ARROW_UP | SLIDER_TICKS_TOP, 0, 0, 200, 10, 0, 0, 5, 0);
    mySpeedFactorSlider->setRange(0, 200);
    mySpeedFactorSlider->setHeadSize(10);
    mySpeedFactorSlider->setIncrement(1);
    mySpeedFactorSlider->setTickDelta(100);
    mySpeedFactorSlider->setValue(100);
    mySpeedFactorSlider->setHelpText("Control speedFactor of tracked object");
    //mySpeedFactorSlider->hide();
}

long
GUISUMOViewParent::onCmdSpeedFactor(FXObject*, FXSelector, void*) {
    if (myView != nullptr && myView->getTrackedID() != GUIGlObject::INVALID_ID) {
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(myView->getTrackedID());
        if (o != nullptr) {
            const double speedFactor = mySpeedFactorSlider->getValue() / SPEEDFACTOR_SCALE;
            if (o->getType() == GLO_VEHICLE) {
                MSBaseVehicle* veh = dynamic_cast<MSBaseVehicle*>(o);
                veh->setChosenSpeedFactor(speedFactor);
            } else if (o->getType() == GLO_PERSON) {
                //MSPerson* person = dynamic_cast<MSPerson*>(o);
                //person->setChosenSpeedFactor(speedFactor);
            }
            mySpeedFactorSlider->setTipText(toString(speedFactor).c_str());
        }

    }
    return 1;
}

long
GUISUMOViewParent::onUpdSpeedFactor(FXObject* sender, FXSelector, void* ptr) {
    bool disable = myView == nullptr || myView->getTrackedID() == GUIGlObject::INVALID_ID;
    sender->handle(this, FXSEL(SEL_COMMAND, disable ? ID_DISABLE : ID_ENABLE), ptr);
    if (disable) {
        mySpeedFactorSlider->hide();
    } else {
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(myView->getTrackedID());
        if (o != nullptr) {
            if (o->getType() == GLO_VEHICLE) {
                MSBaseVehicle* veh = dynamic_cast<MSBaseVehicle*>(o);
                mySpeedFactorSlider->setValue((int)(veh->getChosenSpeedFactor() * SPEEDFACTOR_SCALE));
            } else if (o->getType() == GLO_PERSON) {
                MSPerson* person = dynamic_cast<MSPerson*>(o);
                mySpeedFactorSlider->setValue((int)(person->getChosenSpeedFactor() * SPEEDFACTOR_SCALE));
            }
            mySpeedFactorSlider->show();
        } else {
            myView->stopTrack();
            mySpeedFactorSlider->hide();
        }
    }
    return 1;
}

// ===========================================================================
// private
// ===========================================================================

GUISUMOViewParent::GLObjChooser::GLObjChooser() :
    ACChooserJunction(nullptr),
    ACChooserEdges(nullptr),
    ACChooserVehicles(nullptr),
    ACChooserPersons(nullptr),
    ACChooserContainer(nullptr),
    ACChooserTLS(nullptr),
    ACChooserAdditional(nullptr),
    ACChooserPOI(nullptr),
    ACChooserPolygon(nullptr),
    ACChooserRoutes(nullptr),
    ACChooserStops(nullptr),
    ACChooserProhibition(nullptr) {
}


GUISUMOViewParent::GLObjChooser::~GLObjChooser() {
    // remove all dialogs
    // delete for a nullptr is valid, so no check needed
    delete ACChooserJunction;
    delete ACChooserEdges;
    delete ACChooserVehicles;
    delete ACChooserPersons;
    delete ACChooserContainer;
    delete ACChooserTLS;
    delete ACChooserAdditional;
    delete ACChooserPOI;
    delete ACChooserPolygon;
    delete ACChooserRoutes;
    delete ACChooserStops;
    delete ACChooserProhibition;
}

/****************************************************************************/
