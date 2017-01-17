/****************************************************************************/
/// @file    NWWriter_OpenDrive.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the openDRIVE format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NWWriter_OpenDrive_h
#define NWWriter_OpenDrive_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StringBijection.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class NBEdge;
class OptionsCont;
class PositionVector;
class OutputDevice;
class OutputDevice_String;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NWWriter_OpenDrive
 * @brief Exporter writing networks using the openDRIVE format
 *
 */
class NWWriter_OpenDrive {
public:
    /** @brief Writes the network into a openDRIVE-file
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);

protected:
    /// @brief write geometry as sequence of lines (sumo style)
    static SUMOReal writeGeomLines(const PositionVector& shape, OutputDevice& device, OutputDevice& elevationDevice, SUMOReal offset = 0);

    /* @brief write geometry as sequence of lines and bezier curves
     *
     * @param[in] straightThresh angular changes below threshold are considered to be straight and no curve will be fitted between the segments
     * @param[out] length Return the total length of the reference line
     */
    static bool writeGeomSmooth(const PositionVector& shape, SUMOReal speed, OutputDevice& device, OutputDevice& elevationDevice, SUMOReal straightThresh, SUMOReal& length);

    /// @brief write geometry as a single bezier curve (paramPoly3)
    static SUMOReal writeGeomPP3(OutputDevice& device,
                                 OutputDevice& elevationDevice,
                                 PositionVector init,
                                 SUMOReal length,
                                 SUMOReal offset = 0);

    static void writeElevationProfile(const PositionVector& shape, OutputDevice& device, const OutputDevice_String& elevationDevice);

    static void writeEmptyCenterLane(OutputDevice& device, const std::string& mark, SUMOReal markWidth);
    static int getID(const std::string& origID, StringBijection<int>& map, int& lastID);

    static std::string getLaneType(SVCPermissions permissions);

    /// @brief get the left border of the given lane (the leftmost one by default)
    static PositionVector getLeftLaneBorder(const NBEdge* edge, int laneIndex = -1);

    /// @brief check if the lane geometries are compatible with OpenDRIVE assumptions (colinear stop line)
    static void checkLaneGeometries(const NBEdge* e);
};


#endif

/****************************************************************************/

