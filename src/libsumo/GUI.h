/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUI.h
/// @author  Michael Behrisch
/// @date    07.04.2021
///
// C++ libsumo / TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <libsumo/TraCIDefs.h>

#define DEFAULT_VIEW "View #0"


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
namespace FX {
class FXApp;
}
class GUISUMOAbstractView;
class GUIApplicationWindow;
typedef long long int SUMOTime;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUI
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class GUI {
public:
#if defined(HAVE_LIBSUMOGUI) || defined(HAVE_FOX) || defined(LIBTRACI)
    static double getZoom(const std::string& viewID = DEFAULT_VIEW);
    static double getAngle(const std::string& viewID = DEFAULT_VIEW);
    static libsumo::TraCIPosition getOffset(const std::string& viewID = DEFAULT_VIEW);
    static std::string getSchema(const std::string& viewID = DEFAULT_VIEW);
    static libsumo::TraCIPositionVector getBoundary(const std::string& viewID = DEFAULT_VIEW);
    static bool hasView(const std::string& viewID = DEFAULT_VIEW);
    static std::string getTrackedVehicle(const std::string& viewID = DEFAULT_VIEW);
    static bool isSelected(const std::string& objID, const std::string& objType = "vehicle");

    static void setZoom(const std::string& viewID, double zoom);
    static void setAngle(const std::string& viewID, double angle);
    static void setOffset(const std::string& viewID, double x, double y);
    static void setSchema(const std::string& viewID, const std::string& schemeName);
    static void setBoundary(const std::string& viewID, double xmin, double ymin, double xmax, double ymax);
    static void trackVehicle(const std::string& viewID, const std::string& vehID);
    static void screenshot(const std::string& viewID, const std::string& filename, const int width = -1, const int height = -1);
    static void track(const std::string& objID, const std::string& viewID = DEFAULT_VIEW);
    static void toggleSelection(const std::string& objID, const std::string& objType = "vehicle");
    static void addView(const std::string& viewID, const std::string& schemeName = "", bool in3D = false);
    static void removeView(const std::string& viewID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

#ifndef LIBTRACI
#ifndef SWIG
    static bool start(const std::vector<std::string>& cmd);

    static bool load(const std::vector<std::string>& cmd);

    static bool hasInstance();

    static bool step(SUMOTime t);

    static bool close(const std::string& reason);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
    static GUISUMOAbstractView* getView(const std::string& id);

    static GUIApplicationWindow* myWindow;

    static FX::FXApp* myApp;

#endif
#endif
#endif

    /// @brief invalidated standard constructor
    GUI() = delete;
};


}
