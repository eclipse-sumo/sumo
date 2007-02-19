/****************************************************************************/
/// @file    LogFile.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
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
#ifndef LogFile_h
#define LogFile_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <string>
#include "MsgRetriever.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LogFile
 * This implementation of a MsgRetriever retrieves messages from an
 * application's MsgHandler and writes them into a file. No distinction is
 * being made between messages, warnings and errors beside appending the
 * texts "Warning: " or "Error:" to the front of the message (done in
 * MsgHandler).
 * A log-file is build within the SystemFrame if a filename was supplied
 * using the "log-file"-option. It is also deleted within the SystemFrame.
 */
class LogFile : public MsgRetriever
{
public:
    /// Constructor
    LogFile(const std::string &name);

    /// Destructor
    ~LogFile();

    /// Retrieves the message to log
    void inform(const std::string &msg);

    /// Returns the information whether the is still writable
    bool good();

private:
    /// The used file
    std::ofstream myFile;

};


#endif

/****************************************************************************/

