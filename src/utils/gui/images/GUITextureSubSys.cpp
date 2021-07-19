/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <cassert>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>

#include "GUITextureSubSys.h"

#include "GNETexture_StopContainer.cpp"
#include "GNETexture_StopContainerSelected.cpp"
#include "GNETexture_E3.cpp"
#include "GNETexture_E3Selected.cpp"
#include "GNETexture_Empty.cpp"
#include "GNETexture_EmptySelected.cpp"
#include "GNETexture_LaneBike.cpp"
#include "GNETexture_LaneBus.cpp"
#include "GNETexture_LanePedestrian.cpp"
#include "GNETexture_Lock.cpp"
#include "GNETexture_LockSelected.cpp"
#include "GNETexture_NotMoving.cpp"
#include "GNETexture_NotMovingSelected.cpp"
#include "GNETexture_StopPerson.cpp"
#include "GNETexture_StopPersonSelected.cpp"
#include "GNETexture_Rerouter.cpp"
#include "GNETexture_RerouterSelected.cpp"
#include "GNETexture_RouteProbe.cpp"
#include "GNETexture_RouteProbeSelected.cpp"
#include "GNETexture_Stop.cpp"
#include "GNETexture_StopSelected.cpp"
#include "GNETexture_TLS.cpp"
#include "GNETexture_Vaporizer.cpp"
#include "GNETexture_VaporizerSelected.cpp"
#include "GNETexture_VariableSpeedSign.cpp"
#include "GNETexture_VariableSpeedSignSelected.cpp"


// ===========================================================================
// static member variable definitions
// ===========================================================================

GUITextureSubSys* GUITextureSubSys::myInstance = nullptr;

// ===========================================================================
// member definitions
// ===========================================================================

GUITextureSubSys::GUITextureSubSys(FXApp* a) :
    myApp(a) {
    // Fill map of textures
    myTextures[GUITexture::E3] = 0;
    myTextures[GUITexture::E3_SELECTED] = 0;
    myTextures[GUITexture::EMPTY] = 0;
    myTextures[GUITexture::EMPTY_SELECTED] = 0;
    myTextures[GUITexture::LOCK] = 0;
    myTextures[GUITexture::LOCK_SELECTED] = 0;
    myTextures[GUITexture::NOTMOVING] = 0;
    myTextures[GUITexture::NOTMOVING_SELECTED] = 0;
    myTextures[GUITexture::REROUTER] = 0;
    myTextures[GUITexture::REROUTER_SELECTED] = 0;
    myTextures[GUITexture::ROUTEPROBE] = 0;
    myTextures[GUITexture::ROUTEPROBE_SELECTED] = 0;
    myTextures[GUITexture::TLS] = 0;
    myTextures[GUITexture::VAPORIZER] = 0;
    myTextures[GUITexture::VAPORIZER_SELECTED] = 0;
    myTextures[GUITexture::VARIABLESPEEDSIGN] = 0;
    myTextures[GUITexture::VARIABLESPEEDSIGN_SELECTED] = 0;
    myTextures[GUITexture::LANE_BIKE] = 0;
    myTextures[GUITexture::LANE_BUS] = 0;
    myTextures[GUITexture::LANE_PEDESTRIAN] = 0;
    myTextures[GUITexture::STOP] = 0;
    myTextures[GUITexture::STOP_SELECTED] = 0;
    myTextures[GUITexture::STOPPERSON] = 0;
    myTextures[GUITexture::STOPPERSON_SELECTED] = 0;
}


GUITextureSubSys::~GUITextureSubSys() {
}


void
GUITextureSubSys::initTextures(FXApp* a) {
    assert(myInstance == 0);
    myInstance = new GUITextureSubSys(a);
}


GUIGlID
GUITextureSubSys::getTexture(GUITexture which) {
    // If texture isn't loaded, load it
    if (myInstance->myTextures.at(which) == 0) {
        switch (which) {
            case GUITexture::E3 :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_E3, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::E3_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_E3Selected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::EMPTY :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Empty, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::EMPTY_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_EmptySelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LOCK :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Lock, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LOCK_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LockSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::NOTMOVING :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_NotMoving, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::NOTMOVING_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_NotMovingSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Rerouter, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::REROUTER_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_RerouterSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::ROUTEPROBE :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_RouteProbe, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::ROUTEPROBE_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_RouteProbeSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::TLS :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_TLS, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VAPORIZER :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Vaporizer, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VAPORIZER_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_VaporizerSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VARIABLESPEEDSIGN :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_VariableSpeedSign, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::VARIABLESPEEDSIGN_SELECTED :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_VariableSpeedSignSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LANE_BIKE :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LaneBike, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LANE_BUS :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LaneBus, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::LANE_PEDESTRIAN :
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LanePedestrian, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOP:
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Stop, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOP_SELECTED:
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_StopSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPPERSON:
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_StopPerson, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPPERSON_SELECTED:
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_StopPersonSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPCONTAINER:
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_StopContainer, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GUITexture::STOPCONTAINER_SELECTED:
                myInstance->myTextures[which] = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_StopContainerSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            default:
                throw ProcessError("Undefined texture");
        }
    }
    // Return GLID associated to the texture
    return myInstance->myTextures.at(which);
}


void
GUITextureSubSys::resetTextures() {
    // Reset all textures
    GUITexturesHelper::clearTextures();
    for (auto& i : myInstance->myTextures) {
        i.second = 0;
    }
}


void
GUITextureSubSys::close() {
    delete myInstance;
    myInstance = nullptr;
}


/****************************************************************************/
