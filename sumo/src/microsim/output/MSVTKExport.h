/****************************************************************************/
/// @file    MSVTKExport.h
/// @author  Mario Krumnow
/// @date    2012-04-26
/// @version $Id$
///
// Produce a VTK output to use with Tools like ParaView
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSVTKExport_h
#define MSVTKExport_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVTKExport
 * @brief Produce a VTK output to use with Tools like ParaView
 *
 *  The class offers a static method, which writes VTK Files for each timestep
 *  of the simulation, where at least one vehicle is present.
 *
 * @todo
 */
class MSVTKExport {
public:
    /** @brief Produce a VTK output to use with Tools like ParaView
     *
     * @param[in] of The output device to use
     * @param[in] ec The EdgeControl which holds the edges to write
     * @param[in] timestep The current time step
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void write(OutputDevice& of, SUMOTime timestep);

private:
    /// @brief Invalidated copy constructor.
    MSVTKExport(const MSVTKExport&);

    /// @brief Invalidated assignment operator.
    MSVTKExport& operator=(const MSVTKExport&);

    /// @brief Deletes the whitespaces at the end of a String
    static std::string trim(std::string istring);

    /// @brief Checks if there is a whitespace
    static bool ctype_space(const char c);

    /// @brief Get a comma separated String from a Vector
    static std::string List2String(std::vector<double> input);

    /// @brief Get a Vector with the speed values of each vehicle in the actual timestep
    static std::vector<double> getSpeed();

    /// @brief Get a Vector of the Positions (x,y,z) of each vehicle in the actual timestep
    static std::vector<double> getPositions();

    /// @brief Get a String with the indexes of all vehicles (needed in the VTk File)
    static std::string getOffset(int nr);

};


#endif

/****************************************************************************/
