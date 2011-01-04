/****************************************************************************/
/// @file    OutputDevice_COUT.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cout
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OutputDevice_COUT_h
#define OutputDevice_COUT_h


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
 * @class OutputDevice_COUT
 * @brief An output device that encapsulates cout
 */
class OutputDevice_COUT : public OutputDevice {
public:
    /** @brief Constructor
     * @exception IOError Should not be thrown by this implementation
     */
    OutputDevice_COUT() throw(IOError);


    /// @brief Destructor
    ~OutputDevice_COUT() throw();


protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     * @return cout
     */
    std::ostream &getOStream() throw();
    /// @}

};


#endif

/****************************************************************************/

