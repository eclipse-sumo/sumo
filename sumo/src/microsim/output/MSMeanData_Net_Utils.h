/****************************************************************************/
/// @file    MSMeanData_Net_Utils.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Utilities for building mean data output
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
#ifndef MSMeanData_Net_Utils_h
#define MSMeanData_Net_Utils_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdgeControl;
class MSDetectorControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMeanData_Net_Utils
 * @brief Utilities for building mean data output
 */
class MSMeanData_Net_Utils
{
public:
    /** @brief Checks the given parameter for validity and builds the network's mean data container
     *
     * At first, some constraints are checked, whether the number of dump begin
     *  steps and end steps are the same or whether each begin is before the
     *  according end. If not, a ProcessError is thrown.
     *
     * Otherwise, both the edge-based and the lane-based mean data container are built
     *  using buildList and returned.
     *
     * @param[in] det2file The MSDetectorControl to add the mean data as output to
     * @param[in] ec The edge control containing the edges to be dumped
     * @param[in] dumpMeanDataIntervals Intervals to use for edge-dumps [s]
     * @param[in] baseNameDumpFiles The base name of the edge-dumps
     * @param[in] laneDumpMeanDataIntervals Intervals to use for lane-dumps [s]
     * @param[in] baseNameLaneDumpFiles The base name of the lane-dumps
     * @param[in] dumpBegins Begin times of dumps
     * @param[in] dumpEnds End times of dumps
     * @param[in] withEmptyEdges Information whether empty edges shall be written
     * @param[in] withEmptyLanes Information whether empty lanes shall be written
     * @return The built list of network-wide mean data containers
     * @exception ProcessError If dump begins / ends are not valid
     * @see buildList
     */
    static std::vector<MSMeanData_Net*> buildList(MSDetectorControl &det2file, MSEdgeControl &ec,
            std::vector<int> dumpMeanDataIntervals, std::string baseNameDumpFiles,
            std::vector<int> laneDumpMeanDataIntervals, std::string baseNameLaneDumpFiles,
            const std::vector<int> &dumpBegins, const std::vector<int> &dumpEnds,
            bool withEmptyEdges, bool withEmptyLanes) throw(ProcessError);


protected:
    /** @brief Builds the list of mean data outputs (files) as described by the parameter
     *
     * At first, it is asserted that the intervals are unque using buildUniqueList.
     *
     * @param[in] det2file The MSDetectorControl to add the mean data as output to
     * @param[in] ec The edge control containing the edges to be dumped
     * @param[in] dumpMeanDataIntervals Intervals to use [s]
     * @param[in] baseNameDumpFiles The base name to use
     * @param[in] dumpBegins Begin times of dumps
     * @param[in] dumpEnds End times of dumps
     * @param[in] useLanes Information whether lane-based or edge-based container shall be built
     * @param[in] withEmptyEdges Information whether empty edges shall be written
     * @param[in] withEmptyLanes Information whether empty lanes shall be written
     * @return The built list of network-wide mean data containers
     * @see buildUniqueList
     */
    static std::vector<MSMeanData_Net*> buildList(MSDetectorControl &det2file,
            MSEdgeControl &ec,
            std::vector<int> dumpMeanDataIntervals,
            std::string baseNameDumpFiles,
            const std::vector<int> &dumpBegins,
            const std::vector<int> &dumpEnds,
            bool useLanes, bool withEmptyEdges, bool withEmptyLanes) throw();


    /** @brief Builds a list with unique aggregation times
     *
     * @param[in] dumpMeanDataIntervals
     * @return A list where each interval appears only once
     */
    static std::vector<int> buildUniqueList(std::vector<int> dumpMeanDataIntervals) throw();


};


#endif

/****************************************************************************/

