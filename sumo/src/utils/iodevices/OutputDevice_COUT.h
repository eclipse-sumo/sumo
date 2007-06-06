/****************************************************************************/
/// @file    OutputDevice_COUT.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates cout
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
class OutputDevice_COUT : public OutputDevice
{
public:
    /// Constructor
    OutputDevice_COUT();

    /// Destructor
    ~OutputDevice_COUT();

    /// returns the information whether one can write into the device
    bool ok();

    /// Closes the device
    void close();

    /** @brief returns the information whether a stream is available
        If not, one has to use the "writeString" API */
    bool supportsStreams() const;

    /// Returns the assiciated ostream
    std::ostream &getOStream();

    //{
    /// Writes the given string
    OutputDevice &writeString(const std::string &str);

    void closeInfo();
    //}

};


#endif

/****************************************************************************/

