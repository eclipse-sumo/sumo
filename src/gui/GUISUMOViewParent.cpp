/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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

#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXMenuButtonTooltip.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <gui/dialogs/GUIDialog_GLObjChooser.h>
#include <guisim/GUIPerson.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUINet.h>
#include <guisim/GUIVehicleControl.h>
#include <guisim/GUITransportableControl.h>

#include "GUIViewTraffic.h"
#include "GUIApplicationWindow.h"
#include "GUISUMOViewParent.h"

#include <mesogui/GUIMEVehicleControl.h>

#include <osgview/GUIOSGView.h>

#define SPEEDFACTOR_SCALE 100.0

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUISUMOViewParent) GUISUMOViewParentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MAKESNAPSHOT,   GUISUMOViewParent::onCmdMakeSnapshot),
    //        FXMAPFUNC(SEL_COMMAND,  MID_ALLOWROTATION,  GUISUMOViewParent::onCmdAllowRotation),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,      GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_E_LOCATEEDGE,          GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,       GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_P_LOCATEPERSON,        GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_C_LOCATECONTAINER,     GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_T_LOCATETLS,           GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,    GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_O_LOCATEPOI,           GUISUMOViewParent::onCmdLocate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_L_LOCATEPOLY,          GUISUMOViewParent::onCmdLocate),

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
    myGUIMainWindowParent->addGLChild(this);
}


GUISUMOAbstractView*
GUISUMOViewParent::init(FXGLCanvas* share, GUINet& net, GUISUMOViewParent::ViewType type) {
    switch (type) {
        default:
        case VIEW_2D_OPENGL:
            myView = new GUIViewTraffic(myChildWindowContentFrame, *myGUIMainWindowParent, this, net, myGUIMainWindowParent->getGLVisual(), share);
            break;
#ifdef HAVE_OSG
        case VIEW_3D_OSG:
            myView = new GUIOSGView(myChildWindowContentFrame, *myGUIMainWindowParent, this, net, myGUIMainWindowParent->getGLVisual(), share);
            break;
#endif
    }
    myView->buildViewToolBars(this);
    if (myGUIMainWindowParent->isGaming()) {
        myStaticNavigationToolBar->hide();
    }
    return myView;
}


GUISUMOViewParent::~GUISUMOViewParent() {
    myGUIMainWindowParent->removeGLChild(this);
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
    myGLObjChooser[GLObjChooser->getMessageId()] = nullptr;
}


long
GUISUMOViewParent::onCmdMakeSnapshot(FXObject* sender, FXSelector, void*) {
    MFXCheckableButton* button = dynamic_cast<MFXCheckableButton*>(sender);
    // check if cast was successfully
    if (button) {
        if (button->amChecked()) {
            myView->endSnapshot();
            button->setChecked(false);
            return 1;
        }
        // get the new file name
        FXFileDialog opendialog(this, TL("Save Snapshot"));
        opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::CAMERA));
        opendialog.setSelectMode(SELECTFILE_ANY);
#ifdef HAVE_FFMPEG
        opendialog.setPatternList("All Image and Video Files (*.gif,*.bmp,*.xpm,*.pcx,*.ico,*.rgb,*.xbm,*.tga,*.png,*.jpg,*.jpeg,*.tif,*.tiff,*.ps,*.eps,*.pdf,*.svg,*.tex,*.pgf,*.h264,*.hevc,*.mp4)\n"
                                  "All Video Files (*.h264,*.hevc,*.mp4)\n"
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
        if (file.find(".") == std::string::npos) {
            file.append(".png");
            WRITE_MESSAGE(TL("No file extension was specified - saving Snapshot as PNG."));
        }
        std::string error = myView->makeSnapshot(file);
        if (error == "video") {
            button->setChecked(!button->amChecked());
        } else if (error != "") {
            FXMessageBox::error(this, MBOX_OK, TL("Saving failed."), "%s", error.c_str());
        } else {
            WRITE_MESSAGE(TL("Snapshot successfully saved!"));
        }
    }
    return 1;
}


std::vector<GUIGlID>
GUISUMOViewParent::getObjectIDs(int messageId) const {
    switch (messageId) {
        case MID_HOTKEY_SHIFT_J_LOCATEJUNCTION:
            return static_cast<GUINet*>(GUINet::getInstance())->getJunctionIDs(myGUIMainWindowParent->listInternal());
        case MID_HOTKEY_SHIFT_E_LOCATEEDGE:
            return GUIEdge::getIDs(myGUIMainWindowParent->listInternal());
        case MID_HOTKEY_SHIFT_V_LOCATEVEHICLE: {
            std::vector<GUIGlID> vehicles;
            if (MSGlobals::gUseMesoSim) {
                static_cast<GUIMEVehicleControl*>(static_cast<GUINet*>(MSNet::getInstance())->getGUIMEVehicleControl())->insertVehicleIDs(vehicles);
            } else {
                static_cast<GUIVehicleControl&>(MSNet::getInstance()->getVehicleControl()).insertVehicleIDs(
                    vehicles, myGUIMainWindowParent->listParking(), myGUIMainWindowParent->listTeleporting());
            }
            return vehicles;
        }
        case MID_HOTKEY_SHIFT_P_LOCATEPERSON: {
            std::vector<GUIGlID> persons;
            static_cast<GUITransportableControl&>(MSNet::getInstance()->getPersonControl()).insertIDs(persons);
            return persons;
        }
        case MID_HOTKEY_SHIFT_C_LOCATECONTAINER: {
            // get containers
            std::vector<GUIGlID> containers;
            static_cast<GUITransportableControl&>(MSNet::getInstance()->getContainerControl()).insertIDs(containers);
            return containers;
        }
        case MID_HOTKEY_SHIFT_T_LOCATETLS:
            return static_cast<GUINet*>(GUINet::getInstance())->getTLSIDs();
        case MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL:
            return GUIGlObject_AbstractAdd::getIDList(GLO_ADDITIONALELEMENT);
        case MID_HOTKEY_SHIFT_O_LOCATEPOI:
            return static_cast<GUIShapeContainer&>(GUINet::getInstance()->getShapeContainer()).getPOIIds();
        case MID_HOTKEY_SHIFT_L_LOCATEPOLY:
            return static_cast<GUIShapeContainer&>(GUINet::getInstance()->getShapeContainer()).getPolygonIDs();
        default:
            throw ProcessError(TL("Unknown Message ID in onCmdLocate"));
    }
}


long
GUISUMOViewParent::onCmdLocate(FXObject*, FXSelector sel, void*) {
    int messageId = FXSELID(sel);
    if (myGLObjChooser.count(messageId) == 0 || myGLObjChooser[messageId] == nullptr) {
        FXIcon* icon = nullptr;
        std::string titleString = "";
        switch (messageId) {
            case MID_HOTKEY_SHIFT_J_LOCATEJUNCTION:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION);
                titleString = TL("Junction Chooser");
                break;
            case MID_HOTKEY_SHIFT_E_LOCATEEDGE:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE);
                titleString = TL("Edge Chooser");
                break;
            case MID_HOTKEY_SHIFT_V_LOCATEVEHICLE:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE);
                titleString = TL("Vehicle Chooser");
                break;
            case MID_HOTKEY_SHIFT_P_LOCATEPERSON:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON);
                titleString = TL("Person Chooser");
                break;
            case MID_HOTKEY_SHIFT_C_LOCATECONTAINER:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATECONTAINER);
                titleString = TL("Container Chooser");
                break;
            case MID_HOTKEY_SHIFT_T_LOCATETLS:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATETLS);
                titleString = TL("Traffic Lights Chooser");
                break;
            case MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATEADD);
                titleString = TL("Additional Objects Chooser");
                break;
            case MID_HOTKEY_SHIFT_O_LOCATEPOI:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI);
                titleString = TL("POI Chooser");
                break;
            case MID_HOTKEY_SHIFT_L_LOCATEPOLY:
                icon = GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY);
                titleString = TL("Polygon Chooser");
                break;
            default:
                throw ProcessError(TL("Unknown Message ID in onCmdLocate"));
        }

        myGLObjChooser[messageId] = new GUIDialog_GLObjChooser(this, messageId, icon, titleString.c_str(), getObjectIDs(messageId), GUIGlObjectStorage::gIDStorage);

    } else {
        myGLObjChooser[messageId]->restore();
        myGLObjChooser[messageId]->setFocus();
        myGLObjChooser[messageId]->raise();
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


/****************************************************************************/
