/****************************************************************************/
/// @file    MSTriggeredReader.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The basic class for classes that read triggers
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
#ifndef MSTriggeredReader_h
#define MSTriggeredReader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSNet.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTriggeredReader
 * Superclass for structures that read from a file where the times the next
 * reading is performed are triggered by events,
 */
class MSTriggeredReader {
public:
    /// Destructor
    virtual ~MSTriggeredReader();

    void init();
    bool isInitialised() const;
    SUMOTime wrappedExecute(SUMOTime current);

protected:
    /// Constructor (for an abstract class)
    MSTriggeredReader(MSNet& net);

    /** @brief Processes the next event
        Returns true if the next element shall be read (a new timestemp is then available also)
        Returns false if the action could not be accomplished; this method is then called in the next step again */
    virtual bool processNextEntryReaderTriggered() = 0;

    /// Reads from the file
    virtual bool readNextTriggered() = 0;

    virtual void myInit() = 0;

    virtual void inputEndReached() = 0;


protected:
    /// The offset to the next event
    SUMOTime myOffset;
    bool myWasInitialised;

private:
    /// @brief Invalidated copy constructor.
    MSTriggeredReader(const MSTriggeredReader&);

    /// @brief Invalidated assignment operator.
    MSTriggeredReader& operator=(const MSTriggeredReader&);


};


#endif

/****************************************************************************/

