/****************************************************************************/
/// @file    GUIVisualizationSettings.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Stores the information about how to visualize structures
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIVisualizationSettings_h
#define GUIVisualizationSettings_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>
#include "GUIColorer.h"


// ===========================================================================
// class declarations
// ===========================================================================
class BaseSchemeInfoSource;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================

// cannot declare this as inner class because it needs to be used in forward
// declaration (@todo fix inclusion order by removing references to guisim!)
struct GUIVisualizationTextSettings {
    GUIVisualizationTextSettings(bool _show, float _size, RGBColor _color) :
        show(_show), size(_size), color(_color) {}

    bool show;
    float size;
    RGBColor color;

    bool operator==(const GUIVisualizationTextSettings& other) {
        return show == other.show &&
               size == other.size &&
               color == other.color;
    }
    bool operator!=(const GUIVisualizationTextSettings& other) {
        return !((*this) == other);
    }

    std::string print(const std::string& name) const {
        return name + "_show=\"" + toString(show) + "\" " +
               name + "_size=\"" + toString(size) + "\" " +
               name + "_color=\"" + toString(color) + "\" ";
    }
};

/**
 * @class GUIVisualizationSettings
 * @brief Stores the information about how to visualize structures
 */
class GUIVisualizationSettings {
public:

    /// @brief constructor
    GUIVisualizationSettings();

    /// @brief The name of this setting
    std::string name;

    /// @brief Information whether antialiase shall be enabled
    bool antialiase;
    /// @brief Information whether dithering shall be enabled
    bool dither;

    /// @name Background visualization settings
    //@{

    /// @brief The background color to use
    RGBColor backgroundColor;
    /// @brief Information whether a grid shall be shown
    bool showGrid;
    /// @brief Information about the grid spacings
    SUMOReal gridXSize, gridYSize;
    //@}


    /// @name lane visualization settings
    //@{

#ifdef HAVE_INTERNAL
    /// @brief The mesoscopic edge colorer
    GUIColorer edgeColorer;

    /// @brief this should be set at the same time as MSGlobals::gUseMesoSim
    static bool UseMesoSim;
#endif
    /// @brief The lane colorer
    GUIColorer laneColorer;
    /// @brief Information whether lane borders shall be drawn
    bool laneShowBorders;
    /// @brief Information whether link textures (arrows) shall be drawn
    bool showLinkDecals;
    /// @brief Information whether rails shall be drawn
    bool showRails;
    // Setting bundles for optional drawing names with size and color
    GUIVisualizationTextSettings edgeName, internalEdgeName, cwaEdgeName, streetName;

    bool hideConnectors;
    /// @brief The lane exaggeration (upscale thickness)
    float laneWidthExaggeration;
    //@}


    /// @name vehicle visualization settings
    //@{

    /// @brief The vehicle colorer
    GUIColorer vehicleColorer;
    /// @brief The quality of vehicle drawing
    int vehicleQuality;
    /// @brief The minimum size of vehicles to let them be drawn
    float minVehicleSize;
    /// @brief The vehicle exaggeration (upscale)
    float vehicleExaggeration;
    /// @brief Information whether vehicle blinkers shall be drawn
    bool showBlinker;
    /// @brief Information whether the lane change preference shall be drawn
    bool drawLaneChangePreference;
    /// @brief Information whether the minimum gap shall be drawn
    bool drawMinGap;
    // Setting bundles for optional drawing vehicle names
    GUIVisualizationTextSettings vehicleName;
    //@}


    /// @name person visualization settings
    //@{

    /// @brief The person colorer
    GUIColorer personColorer;
    /// @brief The quality of person drawing
    int personQuality;
    /// @brief The minimum size of persons to let them be drawn
    float minPersonSize;
    /// @brief The person exaggeration (upscale)
    float personExaggeration;
    // Setting bundles for optional drawing person names
    GUIVisualizationTextSettings personName;
    //@}


    /// @name junction visualization settings
    //@{

    /// @brief The junction colorer
    GUIColorer junctionColorer;
    /// @brief Information whether a link's tls index shall be drawn
    bool drawLinkTLIndex;
    /// @brief Information whether a link's junction index shall be drawn
    bool drawLinkJunctionIndex;
    // Setting bundles for optional drawing junction names
    GUIVisualizationTextSettings junctionName, internalJunctionName;
    /// @brief Information whether lane-to-lane arrows shall be drawn
    bool showLane2Lane;
    /// @brief whether the shape of the junction should be drawn
    bool drawJunctionShape;
    //@}


    /// @name Additional structures visualization settings
    //@{

    /// @brief The additional structures visualization scheme
    // @todo decouple addExageration for POIs, Polygons, Triggers etc
    int addMode;
    /// @brief The minimum size of additional structures to let them be drawn
    float minAddSize;
    /// @brief The additional structures exaggeration (upscale)
    float addExaggeration;
    // Setting bundles for optional drawing additional names
    GUIVisualizationTextSettings addName;
    //@}


    /// @name shapes visualization settings
    //@{

    /// @brief The minimum size of shapes to let them be drawn
    float minPOISize;
    /// @brief The additional shapes (upscale)
    float poiExaggeration;
    // Setting bundles for optional drawing poi names
    GUIVisualizationTextSettings poiName;

    /// @brief The minimum size of shapes to let them be drawn
    float minPolySize;
    /// @brief The additional shapes (upscale)
    float polyExaggeration;
    // Setting bundles for optional drawing poi names
    GUIVisualizationTextSettings polyName;
    //@}

    /// @brief Information whether the size legend shall be drawn
    bool showSizeLegend;

    /// @brief information about a lane's width (temporary, used for a single view)
    SUMOReal scale;

    /// @brief whether the application is in gaming mode or not
    bool gaming;

    /// @brief the current NETEDIT mode (temporary)
    int editMode;

    /// @brief the current selection scaling in NETEDIT (temporary)
    SUMOReal selectionScale;

    /** @brief Writes the settings into an output device
     * @param[in] dev The device to write the settings into
     */
    void save(OutputDevice& dev) const;

    /** @brief Returns the number of the active lane (edge) coloring schme
     * @return number of the active scheme
     */
    size_t getLaneEdgeMode() const;

    /** @brief Returns the current lane (edge) coloring schme
     * @return current scheme
     */
    GUIColorScheme& getLaneEdgeScheme();

    /** @brief Assignment operator */
    bool operator==(const GUIVisualizationSettings& vs2);
};


#endif

/****************************************************************************/

