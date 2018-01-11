/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUITextureSubSys.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
/// @version $Id$
///
// Helper for Gifs loading and usage
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <cassert>
#include "GUITextureSubSys.h"

#include "GNETexture_Lock.cpp"
#include "GNETexture_LockSelected.cpp"
#include "GNETexture_Empty.cpp"
#include "GNETexture_EmptySelected.cpp"
#include "GNETexture_TLS.cpp"
#include "GNETexture_E3.cpp"
#include "GNETexture_E3Selected.cpp"
#include "GNETexture_Rerouter.cpp"
#include "GNETexture_RerouterSelected.cpp"
#include "GNETexture_RouteProbe.cpp"
#include "GNETexture_RouteProbeSelected.cpp"
#include "GNETexture_Vaporizer.cpp"
#include "GNETexture_VaporizerSelected.cpp"
#include "GNETexture_VariableSpeedSign.cpp"
#include "GNETexture_VariableSpeedSignSelected.cpp"
#include "GNETexture_NotMoving.cpp"
#include "GNETexture_NotMovingSelected.cpp"
#include "GNETexture_LaneBus.cpp"
#include "GNETexture_LanePedestrian.cpp"
#include "GNETexture_LaneBike.cpp"



// ===========================================================================
// static member variable definitions
// ===========================================================================

GUITextureSubSys* GUITextureSubSys::myInstance = 0;

// ===========================================================================
// member definitions
// ===========================================================================

GUITextureSubSys::GUITextureSubSys(FXApp* a) :
    myApp(a) {
    // Fill map of textures
    myTextures[GNETEXTURE_E3] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_E3SELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_EMPTY] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_EMPTYSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_LOCK] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_LOCKSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_NOTMOVING] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_NOTMOVINGSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_REROUTER] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_REROUTERSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_ROUTEPROBE] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_ROUTEPROBESELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_TLS] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_VAPORIZER] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_VAPORIZERSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_VARIABLESPEEDSIGN] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_VARIABLESPEEDSIGNSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_LANEBIKE] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_LANEBUS] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNETEXTURE_LANEPEDESTRIAN] = std::pair<bool, GUIGlID>(false, 0);
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
    // Obtain GLID and boolean associated to this texture
    std::map<GUITexture, std::pair<bool, GUIGlID> >::iterator i = myInstance->myTextures.find(which);
    // If texture isn't loaded, load it
    if (i->second.first == false) {
        switch (i->first) {
            case GNETEXTURE_E3 :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_E3, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_E3SELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_E3Selected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_EMPTY :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Empty, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_EMPTYSELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_EmptySelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_LOCK :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Lock, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_LOCKSELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LockSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_NOTMOVING :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_NotMoving, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_NOTMOVINGSELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_NotMovingSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_REROUTER :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Rerouter, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_REROUTERSELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_RerouterSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_ROUTEPROBE :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_RouteProbe, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_ROUTEPROBESELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_RouteProbeSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_TLS :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_TLS, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_VAPORIZER :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_Vaporizer, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_VAPORIZERSELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_VaporizerSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_VARIABLESPEEDSIGN :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_VariableSpeedSign, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_VARIABLESPEEDSIGNSELECTED :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_VariableSpeedSignSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_LANEBIKE :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LaneBike, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_LANEBUS :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LaneBus, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNETEXTURE_LANEPEDESTRIAN :
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNETexture_LanePedestrian, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            default:
                throw ProcessError("Undefined texture");
        }
        // Set loaded flag to true
        i->second.first = true;
    }
    // Return GLID associated to the texture
    return i->second.second;
}


void
GUITextureSubSys::resetTextures() {
    // Reset all textures
    GUITexturesHelper::clearTextures();
    for (std::map<GUITexture, std::pair<bool, GUIGlID> >::iterator i = myInstance->myTextures.begin(); i != myInstance->myTextures.end(); i++) {
        i->second.first = false;
    }
}


void
GUITextureSubSys::close() {
    delete myInstance;
    myInstance = 0;
}


/****************************************************************************/

