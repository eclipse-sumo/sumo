/****************************************************************************/
/// @file    AGActivityTripWriter.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Class for writing Trip objects in a SUMO-route file.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AGACTIVITYTRIPWRITER_H
#define AGACTIVITYTRIPWRITER_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class AGTrip;


// ===========================================================================
// class definitions
// ===========================================================================
class AGActivityTripWriter {
public:
    AGActivityTripWriter(OutputDevice& file);

    void addTrip(const AGTrip& trip);

private:
    OutputDevice& myTripOutput;

private:
    /// @brief Invalidated copy constructor.
    AGActivityTripWriter(const AGActivityTripWriter&);

    /// @brief Invalidated assignment operator.
    AGActivityTripWriter& operator=(const AGActivityTripWriter&);

};


#endif

/****************************************************************************/
