/****************************************************************************/
/// @file    LogFile.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
/// @version $Id$
///
// Encapsulated output for saving application messages
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
#ifndef LogFile_h
#define LogFile_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "MsgRetriever.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LogFile
 * @brief Encapsulated output for saving application messages
 *
 * This implementation of a MsgRetriever retrieves messages from an
 * application's MsgHandler and writes them into a file. No distinction is
 * being made between messages, warnings and errors beside appending the
 * texts "Warning: " or "Error:" to the front of the message (done in
 * MsgHandler).
 *
 * A log-file is built within MsgHandler::initOutputOptions if a filename
 *  was supplied using the "log-file"-option. It is also deleted within the
 *  MsgHandler.
 */
class LogFile : public MsgRetriever
{
public:
    /** @brief Constructor
     *
     * Opens the internal output device using OutputDevice::getDevice(name) and
     *  the given name.
     *
     * @param[in] name Name of the device to open
     */
    LogFile(const std::string &name);


    /** @brief Destructor
     *
     * The device is not closed herein as it may be used by different outputs.
     *  It is closed within the device handler.
     */
    ~LogFile();


    /** @brief Retrieves the message to log
     *
     * Writes the given message to the output device.
     *
     * @param[in] msg The msg to write to the device
     */
    void inform(const std::string &msg);


private:
    /// @brief The used file
    OutputDevice &myOutput;

private:
    /// @brief invalidated copy constructor
    LogFile(const LogFile&);

    /// @brief invalidated assignment operator
    LogFile& operator=(const LogFile&);

};


#endif

/****************************************************************************/

