/****************************************************************************/
/// @file    GUIGifSubSys.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2016
/// @version $Id: GUIGifSubSys.cpp 20975 2016-06-15 13:02:40Z palcraft $
///
// Helper for Gifs loading and usage
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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

#include <fx.h>
#include <cassert>
#include "GUIGifSubSys.h"

#include "GNELogo_Lock.cpp"
#include "GNELogo_Empty.cpp"
#include "GNELogo_TLS.cpp"
#include "GNELogo_E1.cpp"
#include "GNELogo_E2.cpp"
#include "GNELogo_E3.cpp"
#include "GNELogo_E3Selected.cpp"
#include "GNELogo_Entry.cpp"
#include "GNELogo_Exit.cpp"
#include "GNELogo_Rerouter.cpp"
#include "GNELogo_RerouterSelected.cpp"
#include "GNELogo_RouteProbe.cpp"
#include "GNELogo_RouteProbeSelected.cpp"
#include "GNELogo_Vaporizer.cpp"
#include "GNELogo_VaporizerSelected.cpp"
#include "GNELogo_VariableSpeedSignal.cpp"
#include "GNELogo_VariableSpeedSignalSelected.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variable definitions
// ===========================================================================

GUIGifSubSys* GUIGifSubSys::myInstance = 0;

// ===========================================================================
// member definitions
// ===========================================================================

GUIGifSubSys::GUIGifSubSys(FXApp* a) :
    myApp(a) {
    // Fill map of textures
    myTextures[GNELOGO_E1] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_E2] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_E3] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_E3SELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_EMPTY] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_EMPTY_SELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_ENTRY] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_EXIT] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_LOCK] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_LOCKSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_REROUTER] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_REROUTERSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_ROUTEPROBE] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_ROUTEPROBESELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_TLS] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_VAPORIZER] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_VAPORIZERSELECTED] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_VARIABLESPEEDSIGNAL] = std::pair<bool, GUIGlID>(false, 0);
    myTextures[GNELOGO_VARIABLESPEEDSIGNALSELECTED] = std::pair<bool, GUIGlID>(false, 0);

}


GUIGifSubSys::~GUIGifSubSys() {
}


void
GUIGifSubSys::init(FXApp* a) {
    assert(myInstance == 0);
    myInstance = new GUIGifSubSys(a);
}


GUIGlID
GUIGifSubSys::getGif(GUIGif which) {
    // Obtain GLID and boolean associated to this texture
    std::map<GUIGif, std::pair<bool, GUIGlID> >::iterator i = myInstance->myTextures.find(which);

    // If texture isn't loaded
    if(i->second.first == false) {
        switch (i->first) {
            case GNELOGO_E1 : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_E1, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_E2 : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_E2, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_E3 : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_E3, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_E3SELECTED : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_E3Selected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_EMPTY : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_Empty, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            //case GNELOGO_EMPTYSELECTED : 
                //i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_EmptySelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                //break;
            case GNELOGO_ENTRY : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_Entry, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_EXIT : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_Exit, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_LOCK : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_Lock, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            //case GNELOGO_LOCKSELECTED : 
                //i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_LockSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                //break;
            case GNELOGO_REROUTER : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_Rerouter, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_REROUTERSELECTED : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_RerouterSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_ROUTEPROBE : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_RouteProbe, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_ROUTEPROBESELECTED : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_RouteProbeSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_TLS : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_TLS, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_VAPORIZER : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_Vaporizer, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_VAPORIZERSELECTED : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_VaporizerSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_VARIABLESPEEDSIGNAL : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_VariableSpeedSignal, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
            case GNELOGO_VARIABLESPEEDSIGNALSELECTED : 
                i->second.second = GUITexturesHelper::add(new FXGIFImage(myInstance->myApp, GNELogo_VariableSpeedSignalSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP));
                break;
        }

    }
    // Return GLID associated to the texture
    return i->second.second;
}


void 
GUIGifSubSys::reset() {
    // Reset all textures
    for(std::map<GUIGif, std::pair<bool, GUIGlID> >::iterator i = myInstance->myTextures.begin(); i != myInstance->myTextures.end(); i++)
        i->second.first = false;
}


void
GUIGifSubSys::close() {
    delete myInstance;
    myInstance = 0;
}


/****************************************************************************/

