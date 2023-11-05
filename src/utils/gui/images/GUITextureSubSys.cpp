/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GUITextureSubSys.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
///
// Helper for Gifs loading and usage
/****************************************************************************/


#include "GUITextureSubSys.h"

#include "textures/E3.cpp"
#include "textures/E3Selected.cpp"
#include "textures/LaneBike.cpp"
#include "textures/LaneBus.cpp"
#include "textures/LanePedestrian.cpp"
#include "textures/Lock.cpp"
#include "textures/NotMoving.cpp"
#include "textures/NotMovingSelected.cpp"
#include "textures/Rerouter.cpp"
#include "textures/RerouterSelected.cpp"
#include "textures/Rerouter_ClosingLaneReroute.cpp"
#include "textures/Rerouter_ClosingReroute.cpp"
#include "textures/Rerouter_DestProbReroute.cpp"
#include "textures/Rerouter_Interval.cpp"
#include "textures/Rerouter_ParkingAreaReroute.cpp"
#include "textures/Rerouter_RouteProbReroute.cpp"
#include "textures/RouteProbe.cpp"
#include "textures/RouteProbeSelected.cpp"
#include "textures/Stop.cpp"
#include "textures/StopContainer.cpp"
#include "textures/StopContainerSelected.cpp"
#include "textures/StopPerson.cpp"
#include "textures/StopPersonSelected.cpp"
#include "textures/StopSelected.cpp"
#include "textures/TLS.cpp"
#include "textures/TractionSubstation.cpp"
#include "textures/TractionSubstationSelected.cpp"
#include "textures/Vaporizer.cpp"
#include "textures/VaporizerSelected.cpp"
#include "textures/VariableSpeedSign.cpp"
#include "textures/VariableSpeedSignSelected.cpp"
#include "textures/VariableSpeedSign_Step.cpp"

#include "textures/POITextures/Nature.cpp"
#include "textures/POITextures/Hotel.cpp"

// ===========================================================================
// static member variable definitions
// ===========================================================================

GUITextureSubSys* GUITextureSubSys::myInstance = nullptr;

// ===========================================================================
// member definitions
// ===========================================================================

GUITextureSubSys::GUITextureSubSys(FXApp* app) :
    myApp(app) {
}


GUITextureSubSys::~GUITextureSubSys() {
}


void
GUITextureSubSys::initTextures(FXApp* a) {
    assert(myInstance == 0);
    myInstance = new GUITextureSubSys(a);
}


GUIGlID
GUITextureSubSys::getTexture(GUITexture textureID) {
    // If texture isn't loaded, load it
    if (myInstance->myTextures.count(textureID) == 0) {
        switch (textureID) {
            case GUITexture::E3:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, E3, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::E3_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, E3Selected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LOCK:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Lock, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::NOTMOVING:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, NotMoving, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::NOTMOVING_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, NotMovingSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Rerouter, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, RerouterSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_INTERVAL:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Rerouter_Interval, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_CLOSINGREROUTE:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Rerouter_ClosingReroute, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_CLOSINGLANEREROUTE:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Rerouter_ClosingLaneReroute, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_ROUTEPROBREROUTE:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Rerouter_RouteProbReroute, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_DESTPROBREROUTE:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Rerouter_DestProbReroute, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_PARKINGAREAREROUTE:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Rerouter_ParkingAreaReroute, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::ROUTEPROBE:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, RouteProbe, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::ROUTEPROBE_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, RouteProbeSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::TLS:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, TLS, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VAPORIZER:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Vaporizer, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VAPORIZER_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, VaporizerSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VARIABLESPEEDSIGN:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, VariableSpeedSign, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VARIABLESPEEDSIGN_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, VariableSpeedSignSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VARIABLESPEEDSIGN_STEP:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, VariableSpeedSign_Step, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LANE_BIKE:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, LaneBike, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LANE_BUS:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, LaneBus, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LANE_PEDESTRIAN:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, LanePedestrian, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOP:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, Stop, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOP_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, StopSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPPERSON:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, StopPerson, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPPERSON_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, StopPersonSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPCONTAINER:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, StopContainer, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPCONTAINER_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, StopContainerSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::TRACTIONSUBSTATION:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, TractionSubstation, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::TRACTIONSUBSTATION_SELECTED:
                myInstance->myTextures[textureID] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, TractionSubstationSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            default:
                throw ProcessError(TL("Undefined texture"));
        }
    }
    // Return GLID associated to the texture
    return myInstance->myTextures.at(textureID);
}


GUIGlID
GUITextureSubSys::getPOITexture(POIIcon icon) {
    // first check if texture exist
    if (myInstance->myPOITextures.count(icon) == 0) {
        switch (icon) {
            case POIIcon::NATURE:
                myInstance->myPOITextures[icon] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, POITexture_Nature, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case POIIcon::HOTEL:
                myInstance->myPOITextures[icon] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, POITexture_Hotel, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            default:
                throw ProcessError("Undefined POI texture");
        }
    }
    return myInstance->myPOITextures.at(icon);
}


void
GUITextureSubSys::resetTextures() {
    // Reset all textures
    GUITexturesHelper::clearTextures();
    for (auto& texture : myInstance->myTextures) {
        texture.second = 0;
    }
    myInstance->myTextures.clear();
    for (auto& POITexture : myInstance->myPOITextures) {
        POITexture.second = 0;
    }
    myInstance->myPOITextures.clear();
}


void
GUITextureSubSys::close() {
    delete myInstance;
    myInstance = nullptr;
}

/****************************************************************************/
