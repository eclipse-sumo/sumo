/****************************************************************************/
/// @file    MSQueueExport.h
/// @author  Mario Krumnow
/// @date    2012-04-26
/// @version $Id$
///
// Export the queueing length in front of a junction (very experimental!)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#ifndef MSQueueExport_h
#define MSQueueExport_h


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
 * @class MSQueueExport
 * @brief Export the queueing length in front of a junction (very experimental!)
 *
 *  The class offers a static method, which writes the actual queueing length of
 *  the network into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSQueueExport {
public:
    /** @brief Export the queueing length in front of a junction (very experimental!)
     *
     * Opens the current time step, goes through the lanes check for a traffic jam in front of a junction
     *
     * @param[in] of The output device to use
     * @param[in] timestep The current time step
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void write(OutputDevice& of, SUMOTime timestep);


private:
    /// @brief Invalidated copy constructor.
    MSQueueExport(const MSQueueExport&);

    /// @brief Invalidated assignment operator.
    MSQueueExport& operator=(const MSQueueExport&);

    /// @brief Iterates through all the edges and extract the lanes
    static void writeEdge(OutputDevice& of);

    /// @brief Iterates through the lanes and check for available vehicle queues
    static void writeLane(OutputDevice& of, const MSLane& lane);

};



#endif

/****************************************************************************/

