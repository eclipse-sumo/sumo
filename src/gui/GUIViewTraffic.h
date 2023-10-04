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
/// @file    GUIViewTraffic.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Andreas Gaubatz
/// @date    Sept 2002
///
// A view on the simulation; this view is a microscopic one
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include "GUISUMOViewParent.h"
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUISUMOViewParent;
class GUIVehicle;
class GUIVideoEncoder;
class MSRoute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIViewTraffic
 * Microsocopic view at the simulation
 */
class GUIViewTraffic : public GUISUMOAbstractView {
public:
    /// @brief constructor
    GUIViewTraffic(FXComposite* p, GUIMainWindow& app,
                   GUISUMOViewParent* parent, GUINet& net, FXGLVisual* glVis,
                   FXGLCanvas* share);
    /// @brief destructor
    virtual ~GUIViewTraffic();

    /// @brief builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow*) override;

    /** @brief Starts vehicle tracking
     * @param[in] id The glID of the vehicle to track
     */
    void startTrack(int id) override;

    /** @brief Stops vehicle tracking
     */
    void stopTrack() override;

    /** @brief Returns the id of the tracked vehicle (-1 if none)
     * @return The glID of the vehicle to track
     */
    GUIGlID getTrackedID() const override;

    bool setColorScheme(const std::string& name) override;

    /// @brief recalibrate color scheme according to the current value range
    void buildColorRainbow(const GUIVisualizationSettings& s, GUIColorScheme& scheme, int active, GUIGlObjectType objectType,
                           bool hide = false, double hideThreshold = 0,
                           bool hide2 = false, double hideThreshold2 = 0) override;

    /// @brief return list of loaded edgeData attributes
    std::vector<std::string> getEdgeDataAttrs() const override;

    /// @brief return list of loaded edgeData ids (being computed in the current simulation)
    std::vector<std::string> getMeanDataIDs() const override;

    /// @brief return list of available attributes for the given meanData id
    std::vector<std::string> getMeanDataAttrs(const std::string& meanDataID) const override;

    /// @brief return list of available edge parameters
    std::vector<std::string> getEdgeLaneParamKeys(bool edgeKeys) const override;

    /// @brief return list of available vehicle parameters
    std::vector<std::string> getVehicleParamKeys(bool vTypeKeys) const override;

    /// @brief return list of available POI parameters
    std::vector<std::string> getPOIParamKeys() const override;

    /// @brief handle mouse click in gaming mode
    void onGamingClick(Position pos) override;
    void onGamingRightClick(Position pos) override;

    /// @brief get the current simulation time
    SUMOTime getCurrentTimeStep() const override;

    /// @brief interaction with the simulation
    long onCmdCloseLane(FXObject*, FXSelector, void*) override;
    long onCmdCloseEdge(FXObject*, FXSelector, void*) override;
    long onCmdAddRerouter(FXObject*, FXSelector, void*) override;

    /// @brief highlight edges according to reachability
    long onCmdShowReachability(FXObject*, FXSelector, void*) override;
    static long showLaneReachability(GUILane* lane, FXObject*, FXSelector);

    long onDoubleClicked(FXObject*, FXSelector, void*) override;

    /** @brief Adds a frame to a video snapshot which will be initialized if necessary
     */
    void saveFrame(const std::string& destFile, FXColor* buf) override;

    /** @brief Ends a video snapshot
     */
    void endSnapshot() override;

    /** @brief Checks whether it is time for a snapshot
     */
    void checkSnapshots() override;

    /// @brief retrieve breakpoints from the current runThread
    const std::vector<SUMOTime> retrieveBreakpoints() const override;

    /// @brief Draw (or not) the JuPedSim pedestrian network
    /// @param s The visualization settings
    void drawPedestrianNetwork(const GUIVisualizationSettings& s) const override;

    /// @brief Change the color of the JuPedSim pedestrian network
    /// @param s The visualization settings
    void changePedestrianNetworkColor(const GUIVisualizationSettings& s) const override;

protected:
    int doPaintGL(int mode, const Boundary& bound) override;

    GUILane* getLaneUnderCursor() override;

private:
    GUIGlID myTrackedID;

    /// @brief whether game mode was set to 'tls'
    bool myTLSGame;

#ifdef HAVE_FFMPEG
    GUIVideoEncoder* myCurrentVideo;
#endif

protected:
    GUIViewTraffic() { }

};
