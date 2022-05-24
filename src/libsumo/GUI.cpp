/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2022 German Aerospace Center (DLR) and others.
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
/// @file    GUI.cpp
/// @author  Michael Behrisch
/// @date    07.04.2021
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/foxtools/MsgHandlerSynchronized.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/xml/XMLSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIRunThread.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUIPerson.h>
#include <guisim/GUIContainer.h>
#include <microsim/MSFrame.h>
#include <microsim/MSNet.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <microsim/MSVehicleControl.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/Helper.h>
#include <libsumo/GUI.h>


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults GUI::mySubscriptionResults;
ContextSubscriptionResults GUI::myContextSubscriptionResults;
GUIApplicationWindow* GUI::myWindow = nullptr;
FXApp* GUI::myApp = nullptr;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
GUI::getIDList() {
    return myWindow->getViewIDs();
}


int
GUI::getIDCount() {
    return (int)myWindow->getViewIDs().size();
}


double
GUI::getZoom(const std::string& viewID) {
    return getView(viewID)->getChanger().getZoom();
}


libsumo::TraCIPosition
GUI::getOffset(const std::string& viewID) {
    GUISUMOAbstractView* v = getView(viewID);
    libsumo::TraCIPosition pos;
    pos.x = v->getChanger().getXPos();
    pos.y = v->getChanger().getYPos();
    return pos;
}


std::string
GUI::getSchema(const std::string& viewID) {
    return getView(viewID)->getVisualisationSettings().name;
}


libsumo::TraCIPositionVector
GUI::getBoundary(const std::string& viewID) {
    const Boundary& b = getView(viewID)->getVisibleBoundary();
    TraCIPositionVector tb;
    TraCIPosition minV;
    TraCIPosition maxV;
    minV.x = b.xmin();
    maxV.x = b.xmax();
    minV.y = b.ymin();
    maxV.y = b.ymax();
    minV.z = b.zmin();
    maxV.z = b.zmax();
    tb.value.push_back(minV);
    tb.value.push_back(maxV);
    return tb;
}


void
GUI::setZoom(const std::string& viewID, double zoom) {
    GUISUMOAbstractView* const v = getView(viewID);
    const Position off(v->getChanger().getXPos(), v->getChanger().getYPos(), v->getChanger().zoom2ZPos(zoom));
    const Position p(off.x(), off.y(), 0);
    v->setViewportFromToRot(off, p, v->getChanger().getRotation());
}


void
GUI::setOffset(const std::string& viewID, double x, double y) {
    GUISUMOAbstractView* const v = getView(viewID);
    const Position off(x, y, v->getChanger().getZPos());
    const Position p(x, y, 0);
    v->setViewportFromToRot(off, p, v->getChanger().getRotation());
}


void
GUI::setSchema(const std::string& viewID, const std::string& schemeName) {
    getView(viewID)->setColorScheme(schemeName);
}


void
GUI::setBoundary(const std::string& viewID, double xmin, double ymin, double xmax, double ymax) {
    getView(viewID)->centerTo(Boundary(xmin, ymin, xmax, ymax));
}


void
GUI::screenshot(const std::string& viewID, const std::string& filename, const int width, const int height) {
    getView(viewID)->addSnapshot(SIMSTEP, filename, width, height);
}


void
GUI::trackVehicle(const std::string& viewID, const std::string& vehID) {
    GUISUMOAbstractView* const v = getView(viewID);
    if (vehID == "") {
        v->stopTrack();
    } else {
        GUIGlID glID = 0;
        SUMOVehicle* veh = MSNet::getInstance()->getVehicleControl().getVehicle(vehID);
        if (veh != nullptr) {
            glID = static_cast<GUIVehicle*>(veh)->getGlID();
        } else {
            MSTransportable* person = MSNet::getInstance()->getPersonControl().get(vehID);
            if (person != nullptr) {
                glID = static_cast<GUIPerson*>(person)->getGlID();
            } else {
                MSTransportable* container = MSNet::getInstance()->getContainerControl().get(vehID);
                if (container != nullptr) {
                    glID = static_cast<GUIContainer*>(container)->getGlID();
                } else {
                    throw TraCIException("Could not find vehicle or person '" + vehID + "'.");
                }
            }
        }
        if (v->getTrackedID() != glID) {
            v->startTrack(glID);
        }
    }
}


bool
GUI::hasView(const std::string& viewID) {
    return getView(viewID) != nullptr;
}


std::string
GUI::getTrackedVehicle(const std::string& viewID) {
    GUISUMOAbstractView* const v = getView(viewID);
    GUIGlObject* tracked = nullptr;
    const GUIGlID gid = v->getTrackedID();
    if (gid != GUIGlObject::INVALID_ID) {
        tracked = GUIGlObjectStorage::gIDStorage.getObjectBlocking(gid);
    }
    const std::string result = tracked == nullptr ? "" : tracked->getMicrosimID();
    if (gid != GUIGlObject::INVALID_ID) {
        GUIGlObjectStorage::gIDStorage.unblockObject(gid);
    }
    return result;
}


void
GUI::track(const std::string& objID, const std::string& viewID) {
    trackVehicle(viewID, objID);
}


bool
GUI::isSelected(const std::string& objID, const std::string& objType) {
    const std::string fullName = objType + ":" + objID;
    GUIGlObject* obj = GUIGlObjectStorage::gIDStorage.getObjectBlocking(fullName);
    if (obj == nullptr) {
        GUIGlObjectStorage::gIDStorage.unblockObject(obj->getGlID());
        throw TraCIException("The " + objType + " " + objID + " is not known.");
    }
    const bool result = gSelected.isSelected(obj);
    GUIGlObjectStorage::gIDStorage.unblockObject(obj->getGlID());
    return result;
}


void
GUI::toggleSelection(const std::string& objID, const std::string& objType) {
    const std::string fullName = objType + ":" + objID;
    GUIGlObject* obj = GUIGlObjectStorage::gIDStorage.getObjectBlocking(fullName);
    if (obj == nullptr) {
        GUIGlObjectStorage::gIDStorage.unblockObject(obj->getGlID());
        throw TraCIException("The " + objType + " " + objID + " is not known.");
    }
    gSelected.toggleSelection(obj->getGlID());
    GUIGlObjectStorage::gIDStorage.unblockObject(obj->getGlID());
}


std::string
GUI::getParameter(const std::string& /* viewID */, const std::string& /* name */) {
    return "";
}


void
GUI::setParameter(const std::string& /* viewID */, const std::string& /* name */, const std::string& /* value */) {
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(GUI)
LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(GUI, GUI)


bool
GUI::start(const std::vector<std::string>& cmd) {
    if (cmd[0].find("sumo-gui") == std::string::npos && std::getenv("LIBSUMO_GUI") == nullptr) {
        return false;
    }
#ifdef WIN32
    WRITE_WARNING("Libsumo on Windows does not work with GUI, falling back to plain libsumo.");
    return false;
#else
    try {
        if (!GUI::close("Libsumo started new instance.")) {
//            SystemFrame::close();
        }
        bool needStart = false;
        if (std::getenv("LIBSUMO_GUI") != nullptr) {
            needStart = true;
            for (const std::string& a : cmd) {
                if (a == "-S" || a == "--start") {
                    needStart = false;
                }
            }
        }
        int origArgc = (int)cmd.size();
        int argc = origArgc;
        if (needStart) {
            argc++;
        }
        char** argv = new char* [argc];
        int i;
        for (i = 0; i < origArgc; i++) {
            argv[i] = new char[cmd[i].size() + 1];
            std::strcpy(argv[i], cmd[i].c_str());
        }
        if (needStart) {
            argv[i++] = (char*)"-S";
        }
        // make the output aware of threading
        MsgHandler::setFactory(&MsgHandlerSynchronized::create);
        gSimulation = true;
        XMLSubSys::init();
        MSFrame::fillOptions();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions(true);
        OptionsCont::getOptions().processMetaOptions(false);
        // Open display
        myApp = new FXApp("SUMO GUI", "sumo-gui");
        myApp->init(argc, argv);
        int minor, major;
        if (!FXGLVisual::supported(myApp, major, minor)) {
            throw ProcessError("This system has no OpenGL support. Exiting.");
        }

        // build the main window
        myWindow = new GUIApplicationWindow(myApp, "*.sumo.cfg,*.sumocfg");
        gSchemeStorage.init(myApp);
        myWindow->dependentBuild(true);
        myApp->create();
        myWindow->getRunner()->enableLibsumo();
        // Load configuration given on command line
        if (argc > 1) {
            myWindow->loadOnStartup(true);
        }
    } catch (ProcessError& e) {
        throw TraCIException(e.what());
    }
    return true;
#endif
}


bool
GUI::load(const std::vector<std::string>& /* cmd */) {
    if (myWindow != nullptr) {
        WRITE_ERROR("libsumo.load is not implemented for the GUI.");
        return true;
    }
    return false;
}


bool
GUI::hasInstance() {
    return myWindow != nullptr;
}


bool
GUI::step(SUMOTime t) {
    if (myWindow != nullptr) {
        if (t == 0) {
            t = SIMSTEP + DELTA_T;
        }
        while (SIMSTEP < t) {
            myWindow->getRunner()->tryStep();
        }
        return true;
    }
    return false;
}


bool
GUI::close(const std::string& /*reason*/) {
    if (myWindow != nullptr) {
        myApp->stop();
        delete myWindow;
        myWindow = nullptr;
        SystemFrame::close();
        delete myApp;
        return true;
    }
    return false;
}


GUISUMOAbstractView*
GUI::getView(const std::string& id) {
    if (myWindow == nullptr) {
        return nullptr;
    }
    GUIGlChildWindow* const c = myWindow->getViewByID(id);
    if (c == nullptr) {
        return nullptr;
    }
    return c->getView();
}


}


/****************************************************************************/
