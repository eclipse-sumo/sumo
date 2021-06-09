/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
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
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <libsumo/TraCIDefs.h>

#define DEFAULT_VIEW "View #0"


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class GUISUMOAbstractView;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class POI
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class GUI {
public:
    static double getZoom(const std::string& viewID = DEFAULT_VIEW);
    static libsumo::TraCIPosition getOffset(const std::string& viewID = DEFAULT_VIEW);
    static std::string getSchema(const std::string& viewID = DEFAULT_VIEW);
    static libsumo::TraCIPositionVector getBoundary(const std::string& viewID = DEFAULT_VIEW);
    static void setZoom(const std::string& viewID, double zoom);
    static void setOffset(const std::string& viewID, double x, double y);
    static void setSchema(const std::string& viewID, const std::string& schemeName);
    static void setBoundary(const std::string& viewID, double xmin, double ymin, double xmax, double ymax);
    static void screenshot(const std::string& viewID, const std::string& filename, const int width = -1, const int height = -1);
    static void trackVehicle(const std::string& viewID, const std::string& vehID);
    static bool hasView(const std::string& viewID = DEFAULT_VIEW);
    static std::string getTrackedVehicle(const std::string& viewID = DEFAULT_VIEW);
    static void track(const std::string& objID, const std::string& viewID = DEFAULT_VIEW);
    static bool isSelected(const std::string& objID, const std::string& objType = "vehicle");
    static void toggleSelection(const std::string& objID, const std::string& objType = "vehicle");

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

#ifndef LIBTRACI
    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static GUISUMOAbstractView* getView(const std::string& id);
#endif

    /// @brief invalidated standard constructor
    GUI() = delete;
};


}
