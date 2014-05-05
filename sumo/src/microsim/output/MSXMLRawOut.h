/****************************************************************************/
/// @file    MSXMLRawOut.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Bjoern Hendriks
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Realises dumping the complete network state
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
#ifndef MSXMLRawOut_h
#define MSXMLRawOut_h


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
class MSBaseVehicle;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSXMLRawOut
 * @brief Realises dumping the complete network state
 *
 * The class offers a static method, which writes the complete dump of
 *  the given network into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSXMLRawOut {
public:
    /** @brief Writes the complete network state of the given edges into the given device
     *
     * Opens the current time step, goes through the edges and writes each using
     *  writeEdge.
     *
     * @param[in] of The output device to use
     * @param[in] ec The EdgeControl which holds the edges to write
     * @param[in] timestep The current time step
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void write(OutputDevice& of, const MSEdgeControl& ec,
                      SUMOTime timestep);


    /** @brief Writes the dump of the given vehicle into the given device
     *
     * @param[in] of The output device to use
     * @param[in] veh The vehicle to dump
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeVehicle(OutputDevice& of, const MSBaseVehicle& veh);


private:
    /** @brief Writes the dump of the given edge into the given device
     *
     * If the edge is not empty or also empty edges shall be dumped, the edge
     *  description is opened and writeLane is called for each lane.
     *
     * @param[in] of The output device to use
     * @param[in] edge The edge to dump
     * @todo MSGlobals::gOmitEmptyEdgesOnDump should not be used; rather the according option read in write
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeEdge(OutputDevice& of, const MSEdge& edge, SUMOTime timestep);


    /** @brief Writes the dump of the given lane into the given device
     *
     * Opens the lane description and goes through all vehicles, calling writeVehicle
     *  for each.
     *
     * @param[in] of The output device to use
     * @param[in] lane The lane to dump
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeLane(OutputDevice& of, const MSLane& lane);


private:
    /// @brief Invalidated copy constructor.
    MSXMLRawOut(const MSXMLRawOut&);

    /// @brief Invalidated assignment operator.
    MSXMLRawOut& operator=(const MSXMLRawOut&);


};


#endif

/****************************************************************************/

