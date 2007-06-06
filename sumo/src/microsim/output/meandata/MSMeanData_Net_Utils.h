/****************************************************************************/
/// @file    MSMeanData_Net_Utils.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// missing_desc
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
#include "MSMeanData_Net_Cont.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdgeControl;
class MSDetector2File;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMeanData_Net_Utils
 * Some helping functions for usage of mean data outputs
 */
class MSMeanData_Net_Utils
{
public:
    /// Builds the list of mean data outputs (files) as described by the parameter
    static MSMeanData_Net_Cont buildList(MSDetector2File &det2file,
                                         MSEdgeControl &ec,
                                         std::vector<SUMOTime> dumpMeanDataIntervalls,
                                         std::string baseNameDumpFiles,
                                         std::vector<SUMOTime> laneDumpMeanDataIntervalls,
                                         std::string baseNameLaneDumpFiles,
                                         const std::vector<int> &dumpBegins,
                                         const std::vector<int> &dumpEnds);

protected:
    /// Builds the list of mean data outputs (files) as described by the parameter
    static MSMeanData_Net_Cont buildList(MSDetector2File &det2file,
                                         MSEdgeControl &ec,
                                         std::vector<SUMOTime> dumpMeanDataIntervalls,
                                         std::string baseNameDumpFiles,
                                         const std::vector<int> &dumpBegins,
                                         const std::vector<int> &dumpEnds,
                                         bool useLanes);

    /// Builds a list with unique aggregation times
    static std::vector<SUMOTime> buildUniqueList(
        std::vector<SUMOTime> dumpMeanDataIntervalls);

};


#endif

/****************************************************************************/

