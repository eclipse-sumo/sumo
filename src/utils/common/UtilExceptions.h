/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    UtilExceptions.h
/// @author  Daniel Krajzewicz
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @author  Felix Brack
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Exceptions for used by some utility classes
/****************************************************************************/
#ifndef UtilExceptions_h
#define UtilExceptions_h


// ===========================================================================
// included modules
// ===========================================================================

#include <string>
#include <stdexcept>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * ProcessError
 * The base class for all exceptions in SUMO. The reason itself can either be
 * reported before throwing the exception or in the message parameter.
 */
class ProcessError : public std::runtime_error {
public:
    /// @brief constructor
    ProcessError()
        : std::runtime_error("Process Error") {}

    /// @brief constructor
    ProcessError(const std::string& msg)
        : std::runtime_error(msg) {}
};


/**
 * InvalidArgument
 * Thrown when an argument was not proper in the current context.
 * A message will be supplied.
 */
class InvalidArgument : public ProcessError {
public:
    /// @brief constructor
    InvalidArgument(const std::string& message)
        : ProcessError(message) {}
};


/**
 * EmptyData
 * Thrown when data required by a method is missing
 */
class EmptyData : public ProcessError {
public:
    /// @brief constructor
    EmptyData()
        : ProcessError("Empty Data") {}
};


/**
 * FormatException
 * Thrown when a string that shall be converted into
 * something else contained the wrong characters
 */
class FormatException : public ProcessError {
public:
    /// @brief constructor
    FormatException(const std::string& msg)
        : ProcessError(msg) {}
};


/**
 * NumberFormatException
 * Thrown when the string that shall be converted into a
 * numerical representation has any other characters then
 * digits and a dot
 */
class NumberFormatException : public FormatException {
public:
    /// @brief constructor
    NumberFormatException(const std::string& data)
        : FormatException("Invalid Number Format '" + data + "'") {}
};


/**
 * BoolFormatException
 * Thrown when the string that shall be converted into a
 * boolean does not match
 */
class BoolFormatException : public FormatException {
public:
    /// @brief constructor
    BoolFormatException(const std::string& data)
        : FormatException("Invalid Bool Format '" + data + "'") {}
};


/**
 * OutOfBoundsException
 * Thrown when an array element out of the array's
 * bounderies is accessed
 */
class OutOfBoundsException : public ProcessError {
public:
    /// @brief constructor
    OutOfBoundsException()
        : ProcessError("Out Of Bounds") {}
};


/**
 * UnknownElement
 * Thrown when a named element is tried to be accessed
 * which is not known to the container
 */
class UnknownElement : public ProcessError {
public:
    /// @brief constructor
    UnknownElement()
        : ProcessError("Unknown Element") {}

    /// @brief constructor
    UnknownElement(const std::string& msg)
        : ProcessError(msg) {}
};


class IOError : public ProcessError {
public:
    /// @brief constructor
    IOError(const std::string& message)
        : ProcessError(message) {}
};

/// define SOFT_ASSERT raise an assertion in debug mode everywhere except on the windows test server
#ifdef MSVC_TEST_SERVER
#ifdef _DEBUG
#define SOFT_ASSERT(expr) if (!(expr)) {throw ProcessError("should not happen");}
#else
#define SOFT_ASSERT(expr)
#endif
#else
#define SOFT_ASSERT(expr) assert(expr);
#endif

#endif

/****************************************************************************/
