#ifndef GUILaneStateBounderiesStorage_h
#define GUILaneStateBounderiesStorage_h
//---------------------------------------------------------------------------//
//                        GUILaneStateBounderiesStorage.h -
//  A class that stores the min and max-bounderies of lane state values
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 20th Oct 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2003/12/04 13:36:09  dkrajzew
// work on setting of aggregated value bounderies
//
// Revision 1.2  2003/11/24 10:16:08  dkrajzew
// documentation added; division by zero-error patched
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <microsim/MSE2Collector.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUILaneStateBounderiesStorage {
public:
    /// Constructor
    GUILaneStateBounderiesStorage();

    /// Destructor
    ~GUILaneStateBounderiesStorage();

    /** @brief Adds a value to
        The bounderies will be adapted to the current values */
    void addValue(E2::DetType type, double val);

    /// Normalises the given value to the current step's min max
    double timeStepNorm(double val, E2::DetType type);

    /// Initialises the bounderies for a step
    void initStep();

private:
    /// Definition of a mapping of a detector type to values
    typedef std::map<E2::DetType, double> ValueMap;

    /// Storage for detector-dependent whole-simulation maximum bounderies
    ValueMap myTimeGlobalMaxes;

    /// Storage for detector-dependent whole-simulation minimum bounderies
    ValueMap myTimeGlobalMins;

    /// Storage for detector-dependent whole-simulation value ranges
    ValueMap myTimeGlobalRanges;

    /// Storage for detector-dependent time-step maximum bounderies
    ValueMap myTimeStepMaxes;

    /// Storage for detector-dependent time-step minimum bounderies
    ValueMap myTimeStepMins;

    /// Storage for detector-dependent time-step minimum value ranges
    ValueMap myTimeStepRanges;

    /// Information whether at least one value was added to the whole-simulation bounderies
    bool myAmGlobalInitialised;

    /// Information whether at least one value was added to the time-step bounderies
    bool myAmStepInitialised;

};

#endif

