/****************************************************************************/
/// @file    GUIVisualizationSettings.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Stores the information about how to visualize structures
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <guisim/GUIEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class BaseSchemeInfoSource;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIVisualizationSettings
 * @brief Stores the information about how to visualize structures
 */
class GUIVisualizationSettings {
public:
    /// @brief constructor
    GUIVisualizationSettings() throw();

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
    /// @brief Information whether background decals (textures) shall be used
    bool showBackgroundDecals;
    /// @brief Information whether a grid shall be shown
    bool showGrid;
    /// @brief Information about the grid spacings
    SUMOReal gridXSize, gridYSize;
    //@}


    /// @name lane visualization settings
    //@{

#ifdef HAVE_MESOSIM
    /// @brief The lane or edge colorer
    GUIEdge::Colorer edgeColorer;
#endif
    /// @brief The lane visualization (color assignment) scheme
    int laneEdgeMode;
    /// @brief The map if used colors (scheme->used colors)
    std::map<int, std::vector<RGBColor> > laneColorings;
    /// @brief Information whether lane borders shall be drawn
    bool laneShowBorders;
    /// @brief Information whether link textures (arrows) shall be drawn
    bool showLinkDecals;
    /// @brief Information whether rails shall be drawn
    bool showRails;
    /// @brief Information whether the edge's name shall be drawn
    bool drawEdgeName;
    /// @brief The size of the edge name
    float edgeNameSize;
    /// @brief The color of edge names
    RGBColor edgeNameColor;
    /// @brief Information whether the names of internal edges shall be drawn
    bool drawInternalEdgeName;
    /// @brief The size of the internal edge name
    float internalEdgeNameSize;
    /// @brief The color of internal edge names
    RGBColor internalEdgeNameColor;
    /// @brief Whether macroscopic connectors shall be hidden
    bool hideConnectors;
    //@}


    /// @name vehicle visualization settings
    //@{

    /// @brief The vehicle visualization (color assignment) scheme
    int vehicleMode;
    /// @brief The quality of vehicle drawing
    int vehicleQuality;
    /// @brief The minimum size of vehicles to let them be drawn
    float minVehicleSize;
    /// @brief The vehicle exaggeration (upscale)
    float vehicleExaggeration;
    /// @brief The map if used colors (scheme->used colors)
    std::map<int, std::vector<RGBColor> > vehicleColorings;
    /// @brief Information whether vehicle blinkers shall be drawn
    bool showBlinker;
    /// @brief Information whether the c2c radius shall be drawn
    bool drawcC2CRadius;
    /// @brief Information whether the lane change preference shall be drawn
    bool drawLaneChangePreference;
    /// @brief Information whether the vehicle's name shall be drawn
    bool drawVehicleName;
    /// @brief The size of the vehicle name
    float vehicleNameSize;
    /// @brief The color of vehicle names
    RGBColor vehicleNameColor;
    //@}


    /// @name junction visualization settings
    //@{

    /// @brief The junction visualization (color assignment) scheme
    int junctionMode;
    /// @brief Information whether a link's tls index shall be drawn
    bool drawLinkTLIndex;
    /// @brief Information whether a link's junction index shall be drawn
    bool drawLinkJunctionIndex;
    /// @brief Information whether the junction's name shall be drawn
    bool drawJunctionName;
    /// @brief The size of the junction name
    float junctionNameSize;
    /// @brief The color of junction names
    RGBColor junctionNameColor;
    //@}


    /// @brief Information whether lane-to-lane arrows shall be drawn
    bool showLane2Lane;


    /// @name Additional structures visualization settings
    //@{

    /// @brief The additional structures visualization scheme
    int addMode;
    /// @brief The minimum size of additional structures to let them be drawn
    float minAddSize;
    /// @brief The additional structures exaggeration (upscale)
    float addExaggeration;
    /// @brief Information whether the additional's name shall be drawn
    bool drawAddName;
    /// @brief The size of the additionals' name
    float addNameSize;
    // The color of additionals' names
    //RGBColor addNameColor;
    //@}


    /// @name shapes visualization settings
    //@{

    /// @brief The minimum size of shapes to let them be drawn
    float minPOISize;
    /// @brief The additional shapes (upscale)
    float poiExaggeration;
    /// @brief Information whether the poi's name shall be drawn
    bool drawPOIName;
    /// @brief The size of the poi name
    float poiNameSize;
    /// @brief The color of poi names
    RGBColor poiNameColor;
    //@}

    /// @brief Information whether the size legend shall be drawn
    bool showSizeLegend;

    /// @brief A temporary (used for a single view) information about a lane's width
    SUMOReal scale;
    /// @brief A temporary (used for a single view) information about a lane's width
    bool needsGlID;


    /** @brief Writes the settings into an output device
     * @param[in] dev The device to write the settings into
     */
    void save(OutputDevice &dev) const throw(IOError);


    /** @brief Assignment operator */
    bool operator==(const GUIVisualizationSettings &vs2);

private:
    void initColorMap(const BaseSchemeInfoSource &sm,
                      std::map<int, std::vector<RGBColor> > &colMap);

};


#endif

/****************************************************************************/

