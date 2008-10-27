/****************************************************************************/
/// @file    GUIVisualizationSettings.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: GUIVisualizationSettings.h 6047 2008-09-11 13:08:48Z dkrajzew $
///
// Stores the information about how to visualize structures
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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


// ===========================================================================
// class definitions
// ===========================================================================
    /**
     * @struct GUIVisualizationSettings
     * @brief Stores the information about how to visualize structures
     */
    struct GUIVisualizationSettings {
        /// The name of this setting
        std::string name;

        /// Information whether antialiase shall be enabled
        bool antialiase;
        /// Information whether dithering shall be enabled
        bool dither;

        /// @name background visualization settings
        //@{

        /// The background color to use
        RGBColor backgroundColor;
        /// Information whether background decals (textures) shall be used
        bool showBackgroundDecals;
        /// information whether a grid shall be shown
        bool showGrid;
        /// Information about the grid spacings
        SUMOReal gridXSize, gridYSize;
        //@}


        /// @name lane visualization settings
        //@{

        /// The lane visualization scheme
        int laneEdgeMode;
        /// The map if used colors (scheme->used colors)
        std::map<int, std::vector<RGBColor> > laneColorings;
#ifdef HAVE_MESOSIM
        /// The map if used colors (scheme->used colors)
        std::map<int, std::vector<RGBColor> > edgeColorings;
#endif
        /// Information whether lane borders shall be drawn
        bool laneShowBorders;
        /// Information whether link textures (arrows) shall be drawn
        bool showLinkDecals;
        int laneEdgeExaggMode; // !!! unused
        SUMOReal minExagg; // !!! unused
        SUMOReal maxExagg; // !!! unused
        /// Information whether rails shall be drawn
        bool showRails;
        /// Information whether the edge's name shall be drawn
        bool drawEdgeName;
        /// The size of the edge name
        float edgeNameSize;
        /// The color of edge names
        RGBColor edgeNameColor;
        //@}


        /// @name vehicle visualization settings
        //@{

        /// The vehicle visualization scheme
        int vehicleMode;
        /// The minimum size of vehicles to let them be drawn
        float minVehicleSize;
        /// The vehicle exaggeration (upscale)
        float vehicleExaggeration;
        /// The map if used colors (scheme->used colors)
        std::map<int, std::vector<RGBColor> > vehicleColorings;
        /// Information whether vehicle blinkers shall be drawn
        bool showBlinker;
        /// Information whether the c2c radius shall be drawn
        bool drawcC2CRadius;
        /// Information whether the lane change preference shall be drawn
        bool drawLaneChangePreference;
        /// Information whether the vehicle's name shall be drawn
        bool drawVehicleName;
        /// The size of the vehicle name
        float vehicleNameSize;
        /// The color of vehicle names
        RGBColor vehicleNameColor;
        //@}


        /// @name junction visualization settings
        //@{

        /// The junction visualization scheme
        int junctionMode;
        /// Information whether a link's tls index shall be drawn
        bool drawLinkTLIndex;
        /// Information whether a link's junction index shall be drawn
        bool drawLinkJunctionIndex;
        /// Information whether the junction's name shall be drawn
        bool drawJunctionName;
        /// The size of the junction name
        float junctionNameSize;
        /// The color of junction names
        RGBColor junctionNameColor;
        //@}


        /// Information whether lane-to-lane arrows shall be drawn
        bool showLane2Lane;


        /// @name additional structures visualization settings
        //@{

        /// The additional structures visualization scheme
        int addMode;
        /// The minimum size of additional structures to let them be drawn
        float minAddSize;
        /// The additional structures exaggeration (upscale)
        float addExaggeration;
        /// Information whether the additional's name shall be drawn
        bool drawAddName;
        /// The size of the additionals' name
        float addNameSize;
        // The color of additionals' names
        //RGBColor addNameColor;
        //@}


        /// @name shapes visualization settings
        //@{

        /// The minimum size of shapes to let them be drawn
        float minPOISize;
        /// The additional shapes (upscale)
        float poiExaggeration;
        /// Information whether the poi's name shall be drawn
        bool drawPOIName;
        /// The size of the poi name
        float poiNameSize;
        /// The color of poi names
        RGBColor poiNameColor;
        //@}

        /// Information whether the size legend shall be drawn
        bool showSizeLegend;

        bool operator==(const GUIVisualizationSettings &vs2);


        SUMOReal scale;
        bool needsGlID;
        mutable bool showNextLaneBorders;

    };


#endif

/****************************************************************************/

