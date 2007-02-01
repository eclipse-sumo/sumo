/****************************************************************************/
/// @file    UtilExceptions.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id: $
///
// Exceptions for used by some utility classes
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

#include <exception>
#include "UtilExceptions.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * InvalidArgument - static variables
 * ----------------------------------------------------------------------- */
std::string UtilException::myDefaultMessage = "<no description available>";


/* -------------------------------------------------------------------------
 * InvalidArgument - methods
 * ----------------------------------------------------------------------- */
InvalidArgument::InvalidArgument(const string &msg)
        : _msg(msg)
{}


InvalidArgument::~InvalidArgument() throw()
{}


const string &InvalidArgument::msg() const
{
    return _msg;
}



/* -------------------------------------------------------------------------
 * ProcessError - methods
 * ----------------------------------------------------------------------- */
ProcessError::ProcessError()
{}


ProcessError::~ProcessError() throw()
{}



/* -------------------------------------------------------------------------
 * UnsupportedFeature - methods
 * ----------------------------------------------------------------------- */
UnsupportedFeature::UnsupportedFeature(const string &message)
        : _message(message)
{}


UnsupportedFeature::~UnsupportedFeature() throw()
{}


const string &
UnsupportedFeature::msg() const
{
    return _message;
}


/* -------------------------------------------------------------------------
 * FileBuildError - methods
 * ----------------------------------------------------------------------- */
FileBuildError::FileBuildError(const string &message)
        : myMessage(message)
{}


FileBuildError::~FileBuildError() throw()
{}


const string &
FileBuildError::msg() const
{
    return myMessage;
}


// -------------------------------------------------------------------------
// NetworkError - methods
// -------------------------------------------------------------------------
NetworkError::NetworkError(const std::string &message) : myMessage(message)
{}

NetworkError::~NetworkError() throw()
{}

const std::string &
NetworkError::msg() const
{
    return myMessage;
}



/****************************************************************************/

