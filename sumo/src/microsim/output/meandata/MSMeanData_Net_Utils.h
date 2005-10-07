#ifndef MSMeanData_Net_Utils_h
#define MSMeanData_Net_Utils_h
//---------------------------------------------------------------------------//
//                        MSMeanData_Net_Utils.cpp -
//  Utilities for building the lanes' mean data output
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Mon, 10.05.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2005/10/07 11:37:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 11:08:51  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 12:14:39  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
// Revision 1.4  2005/05/04 08:13:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added, new mean data functionality
//
// Revision 1.3  2005/02/17 10:33:38  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.2  2004/11/23 10:14:27  dkrajzew
// all detectors moved to microscim/output; new detectors usage applied
//
// Revision 1.1  2004/08/02 12:05:34  dkrajzew
// moved meandata to an own folder
//
// Revision 1.1  2004/07/02 09:01:44  dkrajzew
// microsim output refactoring (moved to a subfolder)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include "MSMeanData_Net_Cont.h"
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdgeControl;
class MSDetector2File;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSMeanData_Net_Utils
 * Some helping functions for usage of mean data outputs
 */
class MSMeanData_Net_Utils {
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


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:


