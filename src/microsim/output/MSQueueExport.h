/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSQueueExport.h
/// @author  Mario Krumnow
/// @date    2012-04-26
///
// Export the queueing length in front of a junction (very experimental!)
/****************************************************************************/
#pragma once
#include <config.h>

#include <functional>
#include <map>
#include <utility>
#include <vector>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;
class MESegment;


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

    /** @brief Writes the last (possibly incomplete) aggregation interval and clears the collected samples
     *
     * @param[in] of The output device to use
     * @param[in] timestep The final time step
     */
    static void finish(OutputDevice& of, SUMOTime timestep);


private:
    /// @brief Invalidated copy constructor.
    MSQueueExport(const MSQueueExport&);

    /// @brief Invalidated assignment operator.
    MSQueueExport& operator=(const MSQueueExport&);

    /// @brief Iterates through the edges and their lanes (micro) or segment queues (meso).
    /// If of is nullptr, queue length samples are collected for aggregation instead of being written.
    /// ensureOpen lazily opens the enclosing timestep element before the first record is written.
    static void writeEdge(OutputDevice* of, double threshold, const std::function<void()>& ensureOpen);

    /// @brief Checks a single lane for a vehicle queue (micro)
    static void writeLane(OutputDevice* of, const MSLane& lane, double threshold, const std::function<void()>& ensureOpen);

    /// @brief Checks a single segment queue (meso)
    static void writeMesoQueue(OutputDevice* of, const MSEdge& edge, const MESegment& segment, int qIdx, double segmentOffset, double threshold, const std::function<void()>& ensureOpen);

    /// @brief Writes aggregated per-edge queue length statistics for the given interval
    static void writeInterval(OutputDevice& of, SUMOTime begin, SUMOTime end);

    /// @brief Returns the given percentile (with linear interpolation) of a sorted sample vector
    static double percentile(const std::vector<double>& sorted, double p);

    /// @brief The begin of the current aggregation interval
    static SUMOTime myIntervalStart;

    /// @brief Queue length samples (in vehicles, in meters) per edge, collected during the current aggregation interval
    static std::map<const MSEdge*, std::vector<std::pair<double, double> > > myEdgeSamples;

};
