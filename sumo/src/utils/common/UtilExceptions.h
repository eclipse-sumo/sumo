/****************************************************************************/
/// @file    UtilExceptions.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
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
#ifndef UtilExceptions_h
#define UtilExceptions_h
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
#include <stdexcept>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * InvalidArgument
 * Thrown when an argument was not proper in the current context
 * A message will be supplied
 */
class InvalidArgument : public runtime_error
{
public:
    /** constructor */
    InvalidArgument(const string &message)
            : runtime_error(message)
    {}

};


/**
 * ProcessError
 * A general exception type that may be thrown when the process is no
 * longer able to proceed due to any reason. The reason itself is mostly
 * reported before throwing the exception
 */
class ProcessError : runtime_error
{
public:
    /** constructor */
    ProcessError()
            : runtime_error("Process Error")
    {}

};


/**
 * EmptyData
 * Thrown when data required by a method is missing
 */
class EmptyData : public runtime_error
{
public:
    /** constructor */
    EmptyData()
            : runtime_error("Empty Data")
    {}

};


/**
 * NumberFormatException
 * Thrown when the string that shall be converted into a
 * numerical representation has any other chracters then
 * digits and a dot
 */
class NumberFormatException : runtime_error
{
public:
    /** constructor */
    NumberFormatException()
            : runtime_error("Number Format")
    {}

};


/**
 * OutOfBoundsException
 * Thrown when an array element out of the array's
 * bounderies is accessed
 */
class OutOfBoundsException : runtime_error
{
public:
    /** constructor */
    OutOfBoundsException()
            : runtime_error("Out Of Bounds")
    {}

};


/**
 * UnknownElement
 * Thrown when a named element is tried to be accesed
 * which is not known to the container
 */
class UnknownElement : runtime_error
{
public:
    /** constructor */
    UnknownElement()
            : runtime_error("Unknown Element")
    {}

};


class FileBuildError : public runtime_error
{
public:
    /** constructor */
    FileBuildError(const string &message)
            : runtime_error(message)
    {}

};


class NetworkError : public runtime_error
{
public:
    // constructor
    NetworkError(const string &message)
            : runtime_error(message)
    {}

};


#endif

/****************************************************************************/
