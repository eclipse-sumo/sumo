/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIOSGView.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Mirko Barthauer
/// @date    19.01.2012
///
// An OSG-based 3D view on the simulation
/****************************************************************************/
#include <config.h>

#ifdef HAVE_OSG

#include <cmath>
#include <fxkeys.h>
#include <iostream>
#include <limits>
#include <utility>
#include <foreign/rtree/SUMORTree.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUISUMOViewParent.h>
#include <gui/GUIViewTraffic.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUIJunctionWrapper.h>
#include <guisim/GUILane.h>
#include <guisim/GUINet.h>
#include <guisim/GUIPerson.h>
#include <guisim/GUIVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/transportables/MSTransportableControl.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/RGBColor.h>
#include <utils/common/StringUtils.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_EditViewport.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include "GUIOSGBuilder.h"
#include "GUIOSGPerspectiveChanger.h"
#include "GUIOSGView.h"


FXDEFMAP(GUIOSGView) GUIOSGView_Map[] = {
    //________Message_Type_________        ___ID___                        ________Message_Handler________
    FXMAPFUNC(SEL_CHORE,                MID_CHORE,			GUIOSGView::OnIdle),
};
FXIMPLEMENT(GUIOSGView, GUISUMOAbstractView, GUIOSGView_Map, ARRAYNUMBER(GUIOSGView_Map))


std::ostream&
operator<<(std::ostream& os, const osg::Vec3d& v) {
    return os << v.x() << "," << v.y() << "," << v.z();
}

// ===========================================================================
// GUIOSGView::Command_TLSChange member method definitions
// ===========================================================================

GUIOSGView::Command_TLSChange::Command_TLSChange(const MSLink* const link, osg::Switch* switchNode)
    : myLink(link), mySwitch(switchNode), myLastState(LINKSTATE_TL_OFF_NOSIGNAL) {
    execute();
}


GUIOSGView::Command_TLSChange::~Command_TLSChange() {}


void
GUIOSGView::Command_TLSChange::execute() {
    switch (myLink->getState()) {
        case LINKSTATE_TL_GREEN_MAJOR:
        case LINKSTATE_TL_GREEN_MINOR:
            mySwitch->setSingleChildOn(0);
            break;
        case LINKSTATE_TL_YELLOW_MAJOR:
        case LINKSTATE_TL_YELLOW_MINOR:
            mySwitch->setSingleChildOn(1);
            break;
        case LINKSTATE_TL_RED:
        case LINKSTATE_STOP:
            mySwitch->setSingleChildOn(2);
            break;
        case LINKSTATE_TL_REDYELLOW:
            mySwitch->setSingleChildOn(3);
            break;
        case LINKSTATE_TL_OFF_BLINKING:
        case LINKSTATE_TL_OFF_NOSIGNAL:
            mySwitch->setSingleChildOn(3);
            break;
        default:
            mySwitch->setAllChildrenOff();
    }
    myLastState = myLink->getState();
}

// ===========================================================================
// GUIOSGView member method definitions
// ===========================================================================

GUIOSGView::GUIOSGView(
    FXComposite* p,
    GUIMainWindow& app,
    GUISUMOViewParent* parent,
    GUINet& net, FXGLVisual* glVis,
    FXGLCanvas* share) :
    GUISUMOAbstractView(p, app, parent, net.getVisualisationSpeedUp(), glVis, share),
    myTracked(0), myCameraManipulator(new GUIOSGManipulator(this)), myLastUpdate(-1),
    myOSGNormalizedCursorX(0.), myOSGNormalizedCursorY(0.) {
    if (myChanger != nullptr) {
        delete (myChanger);
    }
    int w = getWidth();
    int h = getHeight();
    myAdapter = new FXOSGAdapter(this, new FXCursor(parent->getApp(), CURSOR_CROSS));
    myViewer = new osgViewer::Viewer();
    myChanger = new GUIOSGPerspectiveChanger(*this, *myGrid);
    const char* sumoPath = getenv("SUMO_HOME");
    if (sumoPath != 0) {
        std::string newPath = std::string(sumoPath) + "/data/3D";
        if (FileHelpers::isReadable(newPath)) {
            osgDB::FilePathList path = osgDB::Registry::instance()->getDataFilePathList();
            path.push_back(newPath);
            osgDB::Registry::instance()->setDataFilePathList(path);
        }
    }

    myGreenLight = osgDB::readNodeFile("tlg.obj");
    myYellowLight = osgDB::readNodeFile("tly.obj");
    myRedLight = osgDB::readNodeFile("tlr.obj");
    myRedYellowLight = osgDB::readNodeFile("tlu.obj");
    myPoleBase = osgDB::readNodeFile("poleBase.obj");
    if (myGreenLight == 0 || myYellowLight == 0 || myRedLight == 0 || myRedYellowLight == 0 || myPoleBase == 0) {
        WRITE_ERROR(TL("Could not load traffic light files."));
    }
    // calculate camera frustum to scale the ground plane all across
    double left, right, bottom, top, zNear, zFar;
    myViewer->getCamera()->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar);
    myRoot = GUIOSGBuilder::buildOSGScene(myGreenLight, myYellowLight, myRedLight, myRedYellowLight, myPoleBase);
    myPlane = new osg::MatrixTransform();
    myPlane->setCullCallback(new ExcludeFromNearFarComputationCallback());
    myPlane->addChild(GUIOSGBuilder::buildPlane((float)(zFar - zNear)));
    myPlane->addUpdateCallback(new PlaneMoverCallback(myViewer->getCamera()));
    myRoot->addChild(myPlane);
    // add the stats handler
    osgViewer::StatsHandler* statsHandler = new osgViewer::StatsHandler();
    statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_I);
    myViewer->addEventHandler(statsHandler);
    myViewer->setSceneData(myRoot);
    myViewer->setCameraManipulator(myCameraManipulator);

    myViewer->setKeyEventSetsDone(0);
    myViewer->getCamera()->setGraphicsContext(myAdapter);
    myViewer->getCamera()->setViewport(0, 0, w, h);
    myViewer->getCamera()->setNearFarRatio(0.005); // does not work together with setUpDepthPartitionForCamera
    myViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
    myViewer->addEventHandler(new PickHandler(this));
    osg::Vec3d lookFrom, lookAt, up;
    myCameraManipulator->getHomePosition(lookFrom, lookAt, up);
    lookFrom = lookAt + osg::Z_AXIS;
    up = osg::Y_AXIS;
    myCameraManipulator->setHomePosition(lookFrom, lookAt, up);
    myViewer->home();
    recenterView();
    myViewer->home();
    getApp()->addChore(this, MID_CHORE);
    myTextNode = new osg::Geode();
    myText = new osgText::Text;
    myText->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
    myText->setShaderTechnique(osgText::NO_TEXT_SHADER);
    osgText::Font* font = osgText::readFontFile("arial.ttf");
    if (font != nullptr) {
        myText->setFont(font);
    }
    myText->setCharacterSize(16.f);
    myTextNode->addDrawable(myText);
    myText->setAlignment(osgText::TextBase::AlignmentType::LEFT_TOP);
    myText->setDrawMode(osgText::TextBase::DrawModeMask::FILLEDBOUNDINGBOX | osgText::TextBase::DrawModeMask::TEXT);
    myText->setBoundingBoxColor(osg::Vec4(0.0f, 0.0f, 0.2f, 0.5f));
    myText->setBoundingBoxMargin(2.0f);

    myHUD = new osg::Camera;
    myHUD->setProjectionMatrixAsOrtho2D(0, 800, 0, 800); // default size will be overwritten
    myHUD->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    myHUD->setViewMatrix(osg::Matrix::identity());
    myHUD->setClearMask(GL_DEPTH_BUFFER_BIT);
    myHUD->setRenderOrder(osg::Camera::POST_RENDER);
    myHUD->setAllowEventFocus(false);
    myHUD->setGraphicsContext(myAdapter);
    myHUD->addChild(myTextNode);
    myHUD->setViewport(0, 0, w, h);
    myViewer->addSlave(myHUD, false);
    myCameraManipulator->updateHUDText();

    // adjust the main light
    adoptViewSettings();

    osgUtil::Optimizer optimizer;
    optimizer.optimize(myRoot);
}


GUIOSGView::~GUIOSGView() {
    getApp()->removeChore(this, MID_CHORE);
    myViewer->setDone(true);
    myViewer = 0;
    myRoot = 0;
    myAdapter = 0;
    myCameraManipulator = 0;
    myHUD = 0;
    myText = 0;
    myTextNode = 0;
    myGreenLight = 0;
    myYellowLight = 0;
    myRedLight = 0;
    myRedYellowLight = 0;
    myPoleBase = 0;
}


void
GUIOSGView::adoptViewSettings() {
    // lighting
    osg::Light* globalLight = myViewer->getLight();
    globalLight->setAmbient(toOSGColorVector(myVisualizationSettings->ambient3DLight));
    globalLight->setDiffuse(toOSGColorVector(myVisualizationSettings->diffuse3DLight));
    myViewer->getCamera()->setClearColor(toOSGColorVector(myVisualizationSettings->skyColor));

    // ground color
    osg::Geode* planeGeode = dynamic_cast<osg::Geode*>(myPlane->getChild(0));
    osg::Geometry* planeGeom = dynamic_cast<osg::Geometry*>(planeGeode->getChild(0));
    osg::Vec4ubArray* colors = dynamic_cast<osg::Vec4ubArray*>(planeGeom->getColorArray());
    (*colors)[0].set(myVisualizationSettings->backgroundColor.red(),
                     myVisualizationSettings->backgroundColor.green(),
                     myVisualizationSettings->backgroundColor.blue(),
                     myVisualizationSettings->backgroundColor.alpha());
    planeGeom->setColorArray(colors);

    // show/hide OSG nodes
    unsigned int cullMask = 0xFFFFFFFF;
    cullMask ^= (-int(myVisualizationSettings->show3DTLSDomes) ^ cullMask) & (1UL << NODESET_TLSDOMES);
    cullMask ^= (-int(myVisualizationSettings->show3DTLSLinkMarkers) ^ cullMask) & (1UL << NODESET_TLSLINKMARKERS);
    cullMask ^= (-int(myVisualizationSettings->generate3DTLSModels) ^ cullMask) & (1UL << NODESET_TLSMODELS);
    myViewer->getCamera()->setCullMask(cullMask);
    unsigned int hudCullMask = (myVisualizationSettings->show3DHeadUpDisplay) ? 0xFFFFFFFF : 0;
    myHUD->setCullMask(hudCullMask);
}


Position
GUIOSGView::getPositionInformation() const {
    Position pos;
    getPositionAtCursor(myOSGNormalizedCursorX, myOSGNormalizedCursorY, pos);
    return pos;
}


void
GUIOSGView::recalculateBoundaries() {
}


bool
GUIOSGView::is3DView() const {
    return true;
}


void
GUIOSGView::buildViewToolBars(GUIGlChildWindow* v) {
    // build coloring tools
    {
        const std::vector<std::string>& names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i) {
            v->getColoringSchemesCombo()->appendItem(i->c_str());
            if ((*i) == myVisualizationSettings->name) {
                v->getColoringSchemesCombo()->setCurrentItem(v->getColoringSchemesCombo()->getNumItems() - 1);
            }
        }
        v->getColoringSchemesCombo()->setNumVisible(5);
    }
    // for junctions
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Junction\tLocate a junction within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), v, MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,
                 GUIDesignButtonPopup);
    // for edges
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Street\tLocate a street within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), v, MID_HOTKEY_SHIFT_E_LOCATEEDGE,
                 GUIDesignButtonPopup);
    // for vehicles
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Vehicle\tLocate a vehicle within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), v, MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,
                 GUIDesignButtonPopup);
    // for persons
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Person\tLocate a person within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), v, MID_HOTKEY_SHIFT_P_LOCATEPERSON,
                 GUIDesignButtonPopup);
    // for containers
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Container\tLocate a container within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATECONTAINER), v, MID_HOTKEY_SHIFT_C_LOCATECONTAINER,
                 GUIDesignButtonPopup);
    // for tls
    new FXButton(v->getLocatorPopup(),
                 "\tLocate TLS\tLocate a tls within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), v, MID_HOTKEY_SHIFT_T_LOCATETLS,
                 GUIDesignButtonPopup);
    // for additional stuff
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Additional\tLocate an additional structure within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), v, MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,
                 GUIDesignButtonPopup);
    // for pois
    new FXButton(v->getLocatorPopup(),
                 "\tLocate POI\tLocate a POI within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), v, MID_HOTKEY_SHIFT_O_LOCATEPOI,
                 GUIDesignButtonPopup);
    // for polygons
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Polygon\tLocate a Polygon within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), v, MID_HOTKEY_SHIFT_L_LOCATEPOLY,
                 GUIDesignButtonPopup);
}


void
GUIOSGView::resize(int w, int h) {
    GUISUMOAbstractView::resize(w, h);
    updateHUDPosition(w, h);
}


void
GUIOSGView::position(int x, int y, int w, int h) {
    GUISUMOAbstractView::position(x, y, w, h);
    updateHUDPosition(w, h);
}


void
GUIOSGView::updateHUDPosition(int w, int h) {
    // keep the HUD text in the left top corner
#ifdef DEBUG
    std::cout << "GUIOSGView::updateHUDPosition() begin" << std::endl;
#endif
    myHUD->setProjectionMatrixAsOrtho2D(0, w, 0, h);
    myText->setPosition(osg::Vec3d(0., static_cast<double>(height), 0.));
#ifdef DEBUG
    std::cout << "GUIOSGView::updateHUDPosition() end" << std::endl;
#endif
}


void
GUIOSGView::updateHUDText(const std::string text) {
#ifdef DEBUG
    std::cout << "GUIOSGView::updateHUDText(" << text << ") begin" << std::endl;
#endif
    myText->setText(text, osgText::String::ENCODING_UTF8);
#ifdef DEBUG
    std::cout << "GUIOSGView::updateHUDText(" << text << ") end" << std::endl;
#endif
}


void
GUIOSGView::recenterView() {
    stopTrack();
    Position center = myGrid->getCenter();
    double radius = std::max(myGrid->xmax() - myGrid->xmin(), myGrid->ymax() - myGrid->ymin());
    myChanger->centerTo(center, radius);
}


bool
GUIOSGView::setColorScheme(const std::string& name) {
    if (!gSchemeStorage.contains(name)) {
        return false;
    }
    if (myGUIDialogViewSettings != 0) {
        if (myGUIDialogViewSettings->getCurrentScheme() != name) {
            myGUIDialogViewSettings->setCurrentScheme(name);
        }
    }
    myVisualizationSettings = &gSchemeStorage.get(name.c_str());
    myVisualizationSettings->gaming = myApp->isGaming();
    adoptViewSettings();
    update();
    return true;
}


long
GUIOSGView::onPaint(FXObject*, FXSelector, void*) {
    if (!isEnabled()) {
        return 1;
    }
    myDecalsLockMutex.lock();
    for (GUISUMOAbstractView::Decal& d : myDecals) {
        if (!d.initialised) {
            if (d.filename.length() == 6 && d.filename.substr(0, 5) == "light") {
                GUIOSGBuilder::buildLight(d, *myRoot);
            } else if (d.filename.length() > 3 && d.filename.substr(0, 3) == "tl:") {
                const int linkStringIdx = (int)d.filename.find(':', 3);
                GUINet* net = (GUINet*) MSNet::getInstance();
                try {
                    const std::string tlLogic = d.filename.substr(3, linkStringIdx - 3);
                    MSTLLogicControl::TLSLogicVariants& vars = net->getTLSControl().get(tlLogic);
                    const int linkIdx = StringUtils::toInt(d.filename.substr(linkStringIdx + 1));
                    if (linkIdx < 0 || linkIdx >= static_cast<int>(vars.getActive()->getLinks().size())) {
                        throw NumberFormatException("");
                    }
                    const MSLink* const link = vars.getActive()->getLinksAt(linkIdx)[0];
                    osg::Group* tlNode = GUIOSGBuilder::getTrafficLight(d, vars, link, myGreenLight, myYellowLight, myRedLight, myRedYellowLight, myPoleBase, true, 0.5);
                    tlNode->setName("tlLogic:" + tlLogic);
                    myRoot->addChild(tlNode);
                } catch (NumberFormatException&) {
                    WRITE_ERRORF(TL("Invalid link index in '%'."), d.filename);
                } catch (InvalidArgument&) {
                    WRITE_ERRORF(TL("Unknown traffic light in '%'."), d.filename);
                }
            } else {
                GUIOSGBuilder::buildDecal(d, *myRoot);
            }
            d.initialised = true;
        }
    }
    myDecalsLockMutex.unlock();

    // reset active flag
    for (auto& item : myVehicles) {
        item.second.active = false;
    }

    GUINet* net = static_cast<GUINet*>(MSNet::getInstance());
    // build edges
    for (const MSEdge* e : net->getEdgeControl().getEdges()) {
        for (const MSLane* l : e->getLanes()) {
            const MSLane::VehCont& vehicles = l->getVehiclesSecure();
            for (MSVehicle* msVeh : vehicles) {
                GUIVehicle* veh = static_cast<GUIVehicle*>(msVeh);
                if (!(veh->isOnRoad() || veh->isParking() || veh->wasRemoteControlled())) {
                    continue;
                }
                auto itVeh = myVehicles.find(veh);
                if (itVeh == myVehicles.end()) {
                    myVehicles[veh] = GUIOSGBuilder::buildMovable(veh->getVehicleType());
                    myRoot->addChild(myVehicles[veh].pos);
                    myVehicles[veh].pos->setName("vehicle:" + veh->getID());
                    veh->setNode(myVehicles[veh].pos);
                } else {
                    itVeh->second.active = true;
                }
                osg::PositionAttitudeTransform* n = myVehicles[veh].pos;
                n->setPosition(osg::Vec3d(veh->getPosition().x(), veh->getPosition().y(), veh->getPosition().z()));
                const double dir = veh->getAngle() + M_PI / 2.;
                const double slope = -veh->getSlope();
                n->setAttitude(osg::Quat(osg::DegreesToRadians(slope), osg::Vec3(1, 0, 0),
                                         0, osg::Vec3(0, 1, 0),
                                         dir, osg::Vec3(0, 0, 1)));
                /*
                osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
                // path->setLoopMode( osg::AnimationPath::NO_LOOPING );
                osg::AnimationPath::ControlPoint pointA(n->getPosition(), n->getAttitude());
                osg::AnimationPath::ControlPoint pointB(osg::Vec3(veh->getPosition().x(), veh->getPosition().y(), veh->getPosition().z()),
                                                        osg::Quat(dir, osg::Vec3(0, 0, 1)) *
                                                        osg::Quat(osg::DegreesToRadians(slope), osg::Vec3(0, 1, 0)));
                path->insert(0.0f, pointA);
                path->insert(0.5f, pointB);
                n->setUpdateCallback(new osg::AnimationPathCallback(path));
                */
                RGBColor col;

                if (!GUIBaseVehicle::setFunctionalColor(myVisualizationSettings->vehicleColorer.getActive(), veh, col)) {
                    col = myVisualizationSettings->vehicleColorer.getScheme().getColor(veh->getColorValue(*myVisualizationSettings, myVisualizationSettings->vehicleColorer.getActive()));
                }
                myVehicles[veh].mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4d(col.red() / 255., col.green() / 255., col.blue() / 255., col.alpha() / 255.));
                myVehicles[veh].lights->setValue(0, veh->signalSet(MSVehicle::VEH_SIGNAL_BLINKER_RIGHT | MSVehicle::VEH_SIGNAL_BLINKER_EMERGENCY));
                myVehicles[veh].lights->setValue(1, veh->signalSet(MSVehicle::VEH_SIGNAL_BLINKER_LEFT | MSVehicle::VEH_SIGNAL_BLINKER_EMERGENCY));
                myVehicles[veh].lights->setValue(2, veh->signalSet(MSVehicle::VEH_SIGNAL_BRAKELIGHT));
            }
            l->releaseVehicles();
        }
    }
    // remove inactive
    for (auto veh = myVehicles.begin(); veh != myVehicles.end();) {
        if (!veh->second.active) {
            removeVeh((veh++)->first);
        } else {
            ++veh;
        }
    }

    const SUMOTime now = MSNet::getInstance()->getCurrentTimeStep();
    if (now != myLastUpdate || (myGUIDialogViewSettings != 0 && myGUIDialogViewSettings->shown())) {
        GUINet::getGUIInstance()->updateColor(*myVisualizationSettings);
    }
    if (now != myLastUpdate && myTracked != 0) {
        osg::Vec3d lookFrom, lookAt, up;
        lookAt[0] = myTracked->getPosition().x();
        lookAt[1] = myTracked->getPosition().y();
        lookAt[2] = myTracked->getPosition().z();
        const double angle = myTracked->getAngle();
        lookFrom[0] = lookAt[0] + 50. * cos(angle);
        lookFrom[1] = lookAt[1] + 50. * sin(angle);
        lookFrom[2] = lookAt[2] + 10.;
        osg::Matrix m;
        m.makeLookAt(lookFrom, lookAt, osg::Z_AXIS);
        myViewer->getCameraManipulator()->setByInverseMatrix(m);
    }

    // reset active flag
    for (auto& item : myPersons) {
        item.second.active = false;
    }

    for (const MSEdge* e : net->getEdgeControl().getEdges()) {
        const GUIEdge* ge = static_cast<const GUIEdge*>(e);
        const std::set<MSTransportable*, ComparatorNumericalIdLess>& persons = ge->getPersonsSecure();
        for (auto person : persons) {
            if (person->hasArrived() || !person->hasDeparted()) {
                //std::cout << SIMTIME << " person " << person->getID() << " is loaded but arrived\n";
                continue;
            }
            auto itPers = myPersons.find(person);
            if (itPers == myPersons.end()) {
                myPersons[person] = GUIOSGBuilder::buildMovable(person->getVehicleType());
                myRoot->addChild(myPersons[person].pos);
            } else {
                itPers->second.active = true;
            }
            osg::PositionAttitudeTransform* n = myPersons[person].pos;
            const Position pos = person->getPosition();
            n->setPosition(osg::Vec3d(pos.x(), pos.y(), pos.z()));
            const double dir = person->getAngle() + M_PI / 2.;
            n->setAttitude(osg::Quat(dir, osg::Vec3d(0, 0, 1)));

            RGBColor col;
            GUIPerson* actualPerson = dynamic_cast<GUIPerson*>(person);
            if (!GUIPerson::setFunctionalColor(myVisualizationSettings->personColorer.getActive(), actualPerson, col)) {
                col = myVisualizationSettings->personColorer.getScheme().getColor(actualPerson->getColorValue(*myVisualizationSettings, myVisualizationSettings->vehicleColorer.getActive()));
            }
            myPersons[person].mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4d(col.red() / 255., col.green() / 255., col.blue() / 255., col.alpha() / 255.));
        }
        ge->releasePersons();
    }

    // remove inactive
    for (auto person = myPersons.begin(); person != myPersons.end();) {
        if (!person->second.active) {
            removeTransportable((person++)->first);
        } else {
            ++person;
        }
    }

    if (myAdapter->makeCurrent()) {
        myViewer->frame();
        makeNonCurrent();
    }
    myLastUpdate = now;
    return 1;
}


void
GUIOSGView::removeVeh(MSVehicle* veh) {
    if (myTracked == veh) {
        stopTrack();
    }
    std::map<MSVehicle*, OSGMovable>::iterator i = myVehicles.find(veh);
    if (i != myVehicles.end()) {
        myRoot->removeChild(i->second.pos);
        myVehicles.erase(i);
    }
}


void
GUIOSGView::removeTransportable(MSTransportable* t) {
    std::map<MSTransportable*, OSGMovable>::iterator i = myPersons.find(t);
    if (i != myPersons.end()) {
        myRoot->removeChild(i->second.pos);
        myPersons.erase(i);
    }
}


void GUIOSGView::updateViewportValues() {
    osg::Vec3d lookFrom, lookAt, up;
    myViewer->getCameraManipulator()->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    myGUIDialogEditViewport->setValues(Position(lookFrom[0], lookFrom[1], lookFrom[2]),
                                       Position(lookAt[0], lookAt[1], lookAt[2]), calculateRotation(lookFrom, lookAt, up));
}


void
GUIOSGView::showViewportEditor() {
    getViewportEditor(); // make sure it exists;
    osg::Vec3d lookFrom, lookAt, up;
    myViewer->getCameraManipulator()->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    Position from(lookFrom[0], lookFrom[1], lookFrom[2]), at(lookAt[0], lookAt[1], lookAt[2]);
    myGUIDialogEditViewport->setOldValues(from, at, calculateRotation(lookFrom, lookAt, up));
    myGUIDialogEditViewport->setZoomValue(100);
    myGUIDialogEditViewport->show();
}


void
GUIOSGView::setViewportFromToRot(const Position& lookFrom, const Position& lookAt, double rotation) {
    osg::Vec3d lookFromOSG, lookAtOSG, up;
    lookFromOSG[0] = lookFrom.x();
    lookFromOSG[1] = lookFrom.y();
    lookFromOSG[2] = lookFrom.z();
    lookAtOSG[0] = lookAt.x();
    lookAtOSG[1] = lookAt.y();
    lookAtOSG[2] = lookAt.z();

    osg::Vec3d viewAxis, viewUp, orthogonal, normal;
    viewAxis = lookFromOSG - lookAtOSG;
    viewAxis.normalize();
    viewUp = (viewAxis[0] + viewAxis[1] == 0.) ? osg::Vec3d(0., 1., 0.) : osg::Vec3d(0., 0., 1.); // check for parallel vectors
    orthogonal = viewUp ^ viewAxis;
    orthogonal.normalize();
    normal = viewAxis ^ orthogonal;

    rotation = std::fmod(rotation, 360.);
    if (rotation < 0) {
        rotation += 360.;
    }
    myChanger->setRotation(rotation);
    double angle = DEG2RAD(rotation);
    up = normal * cos(angle) - orthogonal * sin(angle);
    up.normalize();

    double zoom = (myGUIDialogEditViewport != nullptr) ? myGUIDialogEditViewport->getZoomValue() : 100.;
    lookFromOSG = lookFromOSG + viewAxis * (100. - zoom);
    lookAtOSG = lookFromOSG - viewAxis;
    myViewer->getCameraManipulator()->setHomePosition(lookFromOSG, lookAtOSG, up);
    myViewer->home();
}


void
GUIOSGView::copyViewportTo(GUISUMOAbstractView* view) {
    osg::Vec3d lookFrom, lookAt, up;
    myViewer->getCameraManipulator()->getHomePosition(lookFrom, lookAt, up);
    view->setViewportFromToRot(Position(lookFrom[0], lookFrom[1], lookFrom[2]),
                               Position(lookAt[0], lookAt[1], lookAt[2]), 0);
}


void
GUIOSGView::startTrack(int id) {
    if (myTracked == 0 || (int)myTracked->getGlID() != id) {
        myTracked = 0;
        MSVehicleControl::constVehIt it = MSNet::getInstance()->getVehicleControl().loadedVehBegin();
        for (; it != MSNet::getInstance()->getVehicleControl().loadedVehEnd(); it++) {
            GUIVehicle* veh = (GUIVehicle*)(*it).second;
            if ((int)veh->getGlID() == id) {
                if (!veh->isOnRoad() || myVehicles.find(veh) == myVehicles.end()) {
                    return;
                }
                myTracked = veh;
                break;
            }
        }
        if (myTracked != 0) {
            osg::Vec3d lookFrom, lookAt, up;
            lookAt[0] = myTracked->getPosition().x();
            lookAt[1] = myTracked->getPosition().y();
            lookAt[2] = myTracked->getPosition().z();
            lookFrom[0] = lookAt[0] + 50.;
            lookFrom[1] = lookAt[1] + 50.;
            lookFrom[2] = lookAt[2] + 10.;
            osg::Matrix m;
            m.makeLookAt(lookFrom, lookAt, osg::Z_AXIS);
            myViewer->getCameraManipulator()->setByInverseMatrix(m);
        }
    }
}


void
GUIOSGView::stopTrack() {
    myTracked = 0;
}


GUIGlID
GUIOSGView::getTrackedID() const {
    return myTracked == 0 ? GUIGlObject::INVALID_ID : myTracked->getGlID();
}


void
GUIOSGView::onGamingClick(Position pos) {
    MSTLLogicControl& tlsControl = MSNet::getInstance()->getTLSControl();
    const MSTrafficLightLogic* minTll = nullptr;
    double minDist = std::numeric_limits<double>::infinity();
    for (const MSTrafficLightLogic* const tll : tlsControl.getAllLogics()) {
        if (tlsControl.isActive(tll)) {
            // get the links
            const MSTrafficLightLogic::LaneVector& lanes = tll->getLanesAt(0);
            if (lanes.size() > 0) {
                const Position& endPos = lanes[0]->getShape().back();
                if (endPos.distanceTo(pos) < minDist) {
                    minDist = endPos.distanceTo(pos);
                    minTll = tll;
                }
            }
        }
    }
    if (minTll != 0) {
        const MSTLLogicControl::TLSLogicVariants& vars = tlsControl.get(minTll->getID());
        const std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
        if (logics.size() > 1) {
            MSSimpleTrafficLightLogic* l = (MSSimpleTrafficLightLogic*) logics[0];
            for (int i = 0; i < (int)logics.size() - 1; i++) {
                if (minTll->getProgramID() == logics[i]->getProgramID()) {
                    l = (MSSimpleTrafficLightLogic*) logics[i + 1];
                    tlsControl.switchTo(minTll->getID(), l->getProgramID());
                }
            }
            if (l == logics[0]) {
                tlsControl.switchTo(minTll->getID(), l->getProgramID());
            }
            l->changeStepAndDuration(tlsControl, MSNet::getInstance()->getCurrentTimeStep(), 0, l->getPhase(0).duration);
            update();
        }
    }
}


SUMOTime
GUIOSGView::getCurrentTimeStep() const {
    return MSNet::getInstance()->getCurrentTimeStep();
}


long GUIOSGView::onConfigure(FXObject* sender, FXSelector sel, void* ptr) {
    // update the window dimensions, in case the window has been resized.
    const int w = getWidth();
    const int h = getHeight();
    if (w > 0 && h > 0) {
        myAdapter->getEventQueue()->windowResize(0, 0, w, h);
        myAdapter->resized(0, 0, w, h);
        updateHUDPosition(w, h);
    }
    return FXGLCanvas::onConfigure(sender, sel, ptr);
}


long GUIOSGView::onKeyPress(FXObject* sender, FXSelector sel, void* ptr) {
    int key = ((FXEvent*)ptr)->code;
    myAdapter->getEventQueue()->keyPress(key);
    // leave key handling for some cases to OSG
    if (key == FX::KEY_f || key == FX::KEY_Left || key == FX::KEY_Right || key == FX::KEY_Up || key == FX::KEY_Down) {
        return 1;
    }
    return FXGLCanvas::onKeyPress(sender, sel, ptr);
}


long GUIOSGView::onKeyRelease(FXObject* sender, FXSelector sel, void* ptr) {
    int key = ((FXEvent*)ptr)->code;
    myAdapter->getEventQueue()->keyRelease(key);
    // leave key handling for some cases to OSG
    if (key == FX::KEY_f || key == FX::KEY_Left || key == FX::KEY_Right || key == FX::KEY_Up || key == FX::KEY_Down) {
        return 1;
    }
    return FXGLCanvas::onKeyRelease(sender, sel, ptr);
}


long GUIOSGView::onLeftBtnPress(FXObject* sender, FXSelector sel, void* ptr) {
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);

    FXEvent* event = (FXEvent*)ptr;
    myAdapter->getEventQueue()->mouseButtonPress((float)event->click_x, (float)event->click_y, 1);
    if (myApp->isGaming()) {
        onGamingClick(getPositionInformation());
    }

    return FXGLCanvas::onLeftBtnPress(sender, sel, ptr);
}


long GUIOSGView::onLeftBtnRelease(FXObject* sender, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    myAdapter->getEventQueue()->mouseButtonRelease((float)event->click_x, (float)event->click_y, 1);
    myChanger->onLeftBtnRelease(ptr);
    return FXGLCanvas::onLeftBtnRelease(sender, sel, ptr);
}


long GUIOSGView::onMiddleBtnPress(FXObject* sender, FXSelector sel, void* ptr) {
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);

    FXEvent* event = (FXEvent*)ptr;
    myAdapter->getEventQueue()->mouseButtonPress((float)event->click_x, (float)event->click_y, 2);

    return FXGLCanvas::onMiddleBtnPress(sender, sel, ptr);
}


long GUIOSGView::onMiddleBtnRelease(FXObject* sender, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    myAdapter->getEventQueue()->mouseButtonRelease((float)event->click_x, (float)event->click_y, 2);
    myChanger->onMiddleBtnRelease(ptr);
    return FXGLCanvas::onMiddleBtnRelease(sender, sel, ptr);
}


long GUIOSGView::onRightBtnPress(FXObject* sender, FXSelector sel, void* ptr) {
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);

    FXEvent* event = (FXEvent*)ptr;
    myAdapter->getEventQueue()->mouseButtonPress((float)event->click_x, (float)event->click_y, 3);

    return FXGLCanvas::onRightBtnPress(sender, sel, ptr);
}


long GUIOSGView::onRightBtnRelease(FXObject* sender, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    myAdapter->getEventQueue()->mouseButtonRelease((float)event->click_x, (float)event->click_y, 3);
    myChanger->onRightBtnRelease(ptr);
    return FXGLCanvas::onRightBtnRelease(sender, sel, ptr);
}


long
GUIOSGView::onMouseMove(FXObject* sender, FXSelector sel, void* ptr) {
    // if popup exist but isn't shown, destroy it first
    if (myPopup && (myPopup->shown() == false)) {
        destroyPopup();
    }

    FXEvent* event = (FXEvent*)ptr;
    osgGA::GUIEventAdapter* ea = myAdapter->getEventQueue()->mouseMotion((float)event->win_x, (float)event->win_y);
    setWindowCursorPosition(ea->getXnormalized(), ea->getYnormalized());
    if (myGUIDialogEditViewport != nullptr && myGUIDialogEditViewport->shown()) {
        updateViewportValues();
    }
    updatePositionInformation();
    return FXGLCanvas::onMotion(sender, sel, ptr);
}


long
GUIOSGView::OnIdle(FXObject* /* sender */, FXSelector /* sel */, void*) {
    forceRefresh();
    update();
    getApp()->addChore(this, MID_CHORE);
    return 1;
}


long
GUIOSGView::onCmdCloseLane(FXObject*, FXSelector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        lane->closeTraffic();
        GUIGlObjectStorage::gIDStorage.unblockObject(lane->getGlID());
        GUINet::getGUIInstance()->updateColor(*myVisualizationSettings);
        update();
    }
    return 1;
}


long
GUIOSGView::onCmdCloseEdge(FXObject*, FXSelector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        dynamic_cast<GUIEdge*>(&lane->getEdge())->closeTraffic(lane);
        GUIGlObjectStorage::gIDStorage.unblockObject(lane->getGlID());
        GUINet::getGUIInstance()->updateColor(*myVisualizationSettings);
        update();
    }
    return 1;
}


long
GUIOSGView::onCmdAddRerouter(FXObject*, FXSelector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        dynamic_cast<GUIEdge*>(&lane->getEdge())->addRerouter();
        GUIGlObjectStorage::gIDStorage.unblockObject(lane->getGlID());
        update();
    }
    return 1;
}


long
GUIOSGView::onCmdShowReachability(FXObject* menu, FXSelector selector, void*) {
    GUILane* lane = getLaneUnderCursor();
    if (lane != nullptr) {
        // reset
        GUIViewTraffic::showLaneReachability(lane, menu, selector);
        // switch to 'color by selection' unless coloring 'by reachability'
        if (myVisualizationSettings->laneColorer.getActive() != 36) {
            myVisualizationSettings->laneColorer.setActive(1);
            GUINet::getGUIInstance()->updateColor(*myVisualizationSettings);
        }
        update();
    }
    return 1;
}


long
GUIOSGView::onVisualizationChange(FXObject*, FXSelector, void*) {
    adoptViewSettings();
    return 1;
}


void
GUIOSGView::setWindowCursorPosition(float x, float y) {
    myOSGNormalizedCursorX = x;
    myOSGNormalizedCursorY = y;
}


double
GUIOSGView::calculateRotation(const osg::Vec3d& lookFrom, const osg::Vec3d& lookAt, const osg::Vec3d& up) {
    osg::Vec3d viewAxis, viewUp, orthogonal, normal;
    viewAxis = lookFrom - lookAt;
    viewAxis.normalize();
    viewUp = (abs(viewAxis[0]) + abs(viewAxis[1]) == 0.) ? osg::Y_AXIS : osg::Z_AXIS; // check for parallel vectors
    orthogonal = viewUp ^ viewAxis;
    orthogonal.normalize();
    normal = viewAxis ^ orthogonal;
    double angle = atan2((normal ^ up).length() / (normal.length() * up.length()), (normal * up) / (normal.length() * up.length()));
    if (angle < 0) {
        angle += M_PI;
    }
    return RAD2DEG(angle);
}


void
GUIOSGView::updatePositionInformation() const {
    Position pos;
    if (getPositionAtCursor(myOSGNormalizedCursorX, myOSGNormalizedCursorY, pos)) {
        myApp->getCartesianLabel()->setText(("x:" + toString(pos.x()) + ", y:" + toString(pos.y())).c_str());
        // set geo position
        GeoConvHelper::getFinal().cartesian2geo(pos);
        if (GeoConvHelper::getFinal().usingGeoProjection()) {
            myApp->getGeoLabel()->setText(("lat:" + toString(pos.y(), gPrecisionGeo) + ", lon:" + toString(pos.x(), gPrecisionGeo)).c_str());
        } else {
            myApp->getGeoLabel()->setText(("x:" + toString(pos.x()) + ", y:" + toString(pos.y()) + TL(" (No projection defined)")).c_str());
        }
    } else {
        // set placeholder
        myApp->getCartesianLabel()->setText(TL("N/A"));
        myApp->getGeoLabel()->setText(TL("N/A"));
    }
}


bool
GUIOSGView::getPositionAtCursor(float xNorm, float yNorm, Position& pos) const {
    // only reasonable if view axis points to the ground (not parallel to the ground or in the sky)
    osg::Vec3d lookFrom, lookAt, up, viewAxis;
    myViewer->getCameraManipulator()->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    if ((lookAt - lookFrom).z() >= 0.) {
        // looking to the sky makes position at ground pointless
        return false;
    }
    // solve linear equation of ray crossing the ground plane
    osg::Matrixd iVP = osg::Matrixd::inverse(myViewer->getCamera()->getViewMatrix() * myViewer->getCamera()->getProjectionMatrix());
    osg::Vec3 nearPoint = osg::Vec3(xNorm, yNorm, 0.0f) * iVP;
    osg::Vec3 farPoint = osg::Vec3(xNorm, yNorm, 1.0f) * iVP;
    osg::Vec3 ray = farPoint - nearPoint;
    osg::Vec3 groundPos = nearPoint - ray * nearPoint.z() / ray.z();
    pos.setx(groundPos.x());
    pos.sety(groundPos.y());
    pos.setz(0.);
    return true;
}


std::vector<GUIGlObject*>
GUIOSGView::getGUIGlObjectsUnderCursor() {
    std::vector<GUIGlObject*> result;
    osgUtil::LineSegmentIntersector::Intersections intersections;
    if (myViewer->computeIntersections(myViewer->getCamera(), osgUtil::Intersector::CoordinateFrame::PROJECTION, myOSGNormalizedCursorX, myOSGNormalizedCursorY, intersections)) {
        for (auto intersection : intersections) {
            if (!intersection.nodePath.empty()) {
                // the object is identified by the ID stored in OSG
                for (osg::Node* currentNode : intersection.nodePath) {
                    if (currentNode->getName().length() > 0 && currentNode->getName().find(":") != std::string::npos) {
                        const std::string objID = currentNode->getName();
                        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(objID);
                        // check that GUIGlObject exist
                        if (o == nullptr) {
                            continue;
                        }
                        // check that GUIGlObject isn't the network
                        if (o->getGlID() == 0) {
                            continue;
                        }
                        result.push_back(o);
                        // unblock object
                        GUIGlObjectStorage::gIDStorage.unblockObject(o->getGlID());
                    }
                }
            }
        }
    }
    return result;
}


GUILane*
GUIOSGView::getLaneUnderCursor() {
    std::vector<GUIGlObject*> objects = getGUIGlObjectsUnderCursor();
    if (objects.size() > 0) {
        return dynamic_cast<GUILane*>(objects[0]);
    }
    return nullptr;
}


void
GUIOSGView::zoom2Pos(Position& camera, Position& lookAt, double zoom) {
    osg::Vec3d lookFromOSG, lookAtOSG, viewAxis, up;
    myViewer->getCameraManipulator()->getInverseMatrix().getLookAt(lookFromOSG, lookAtOSG, up);
    lookFromOSG[0] = camera.x();
    lookFromOSG[1] = camera.y();
    lookFromOSG[2] = camera.z();
    lookAtOSG[0] = lookAt.x();
    lookAtOSG[1] = lookAt.y();
    lookAtOSG[2] = lookAt.z();
    viewAxis = lookAtOSG - lookFromOSG;
    viewAxis.normalize();

    // compute new camera and lookAt pos
    osg::Vec3d cameraUpdate = lookFromOSG + viewAxis * (zoom - 100.);
    osg::Vec3d lookAtUpdate = cameraUpdate + viewAxis;

    myViewer->getCameraManipulator()->setHomePosition(cameraUpdate, lookAtUpdate, up);
    myViewer->home();
}


osg::Vec4d
GUIOSGView::toOSGColorVector(RGBColor c, bool useAlpha) {
    return osg::Vec4d(c.red() / 255., c.green() / 255., c.blue() / 255., (useAlpha) ? c.alpha() / 255. : 1.);
}


GUIOSGView::FXOSGAdapter::FXOSGAdapter(GUISUMOAbstractView* parent, FXCursor* cursor)
    : myParent(parent), myOldCursor(cursor) {
    _traits = new GraphicsContext::Traits();
    _traits->x = 0;
    _traits->y = 0;
    _traits->width = parent->getWidth();
    _traits->height = parent->getHeight();
    _traits->windowDecoration = false;
    _traits->doubleBuffer = true;
    _traits->sharedContext = 0;
    if (valid()) {
        setState(new osg::State());
        getState()->setGraphicsContext(this);
        if (_traits.valid() && _traits->sharedContext != 0) {
            getState()->setContextID(_traits->sharedContext->getState()->getContextID());
            incrementContextIDUsageCount(getState()->getContextID());
        } else {
            getState()->setContextID(createNewContextID());
        }
    }
}


GUIOSGView::FXOSGAdapter::~FXOSGAdapter() {
    delete myOldCursor;
}


void
GUIOSGView::FXOSGAdapter::grabFocus() {
    // focus this window
    myParent->setFocus();
}


void
GUIOSGView::FXOSGAdapter::useCursor(bool cursorOn) {
    if (cursorOn) {
        myParent->setDefaultCursor(myOldCursor);
    } else {
        myParent->setDefaultCursor(NULL);
    }
}


bool
GUIOSGView::FXOSGAdapter::makeCurrentImplementation() {
    myParent->makeCurrent();
    return true;
}


bool
GUIOSGView::FXOSGAdapter::releaseContext() {
    myParent->makeNonCurrent();
    return true;
}


void
GUIOSGView::FXOSGAdapter::swapBuffersImplementation() {
    myParent->swapBuffers();
}


bool
GUIOSGView::PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */) {
    if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG) {
        myDrag = true;
    } else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
        if (!myDrag) {
            if (myParent->makeCurrent()) {
                std::vector<GUIGlObject*> objects = myParent->getGUIGlObjectsUnderCursor();
                if (objects.size() > 0) {
                    myParent->openObjectDialog(objects);
                }
                myParent->makeNonCurrent();
            }
        }
        myDrag = false;
    }
    return false;
}


#endif

/****************************************************************************/
