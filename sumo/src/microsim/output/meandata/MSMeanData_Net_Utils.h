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
 * included modules
 * ======================================================================= */
#include <vector>
#include "MSMeanData_Net_Cont.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdgeControl;


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
    static MSMeanData_Net_Cont buildList(MSEdgeControl &ec,
        std::vector<size_t> dumpMeanDataIntervalls,
        std::string baseNameDumpFiles);

    /// Checks whether an output shall be generated and does it if so
    static void checkOutput(MSMeanData_Net_Cont &cont,
        size_t passedSteps, size_t start, size_t step, MSEdgeControl &edges);

    /// Builds a list with unique aggregation times
    static std::vector<size_t> buildUniqueList(
        std::vector<size_t> dumpMeanDataIntervalls);

};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:


