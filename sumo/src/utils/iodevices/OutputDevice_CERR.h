/****************************************************************************/
/// @file    OutputDevice_CERR.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cerr
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OutputDevice_CERR_h
#define OutputDevice_CERR_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "OutputDevice.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice_CERR
 * @brief An output device that encapsulates cerr
 */
class OutputDevice_CERR : public OutputDevice {
public:
    /** @brief Returns the single cerr instance
     *
     * Creates and returns the cerr device.
     */
    static OutputDevice* getDevice();


protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     * @return cerr
     */
    std::ostream& getOStream();


    /** @brief Called after every write access.
     *
     * Calls flush on stderr.
     */
    virtual void postWriteHook();
    /// @}


private:
    /** @brief Constructor
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_CERR();


    /// @brief Destructor
    ~OutputDevice_CERR();


private:
    /// @brief my singular instance
    static OutputDevice* myInstance;


};


#endif

/****************************************************************************/

