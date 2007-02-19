/****************************************************************************/
/// @file    FileErrorReporter.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that realises easier reports on errors occured while loading
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
#include <string>
#include <utils/common/MsgHandler.h>
#include "FileErrorReporter.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
                using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================

FileErrorReporter::Child::Child(FileErrorReporter &parent)
        : myParent(parent)
{}


FileErrorReporter::Child::~Child()
{}


void
FileErrorReporter::Child::addError(const std::string &error) const
{
    myParent.addError(error);
}


/* -------------------------------------------------------------------------
 * method definitions
 * ----------------------------------------------------------------------- */
FileErrorReporter::FileErrorReporter(const std::string &filetype,
                                     const std::string &file)
        : _file(file), _filetype(filetype)
{}


FileErrorReporter::~FileErrorReporter()
{}


void
FileErrorReporter::addError(const std::string &msg)
{
    MsgHandler::getErrorInstance()->inform("The " + _filetype + " '" + _file + "' is corrupt:\n " + msg);
}


void
FileErrorReporter::setFileName(const std::string &file)
{
    _file = file;
}


const string &
FileErrorReporter::getFileName() const
{
    return _file;
}



const std::string &
FileErrorReporter::getDataType() const
{
    return _filetype;
}



/****************************************************************************/

