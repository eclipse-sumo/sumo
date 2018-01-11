/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <stdexcept>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * ProcessError
 * A general exception type that may be thrown when the process is no
 * longer able to proceed due to any reason. The reason itself is mostly
 * reported before throwing the exception
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
 * Thrown when an argument was not proper in the current context
 * A message will be supplied
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
class EmptyData : public std::runtime_error {
public:
    /// @brief constructor
    EmptyData()
        : std::runtime_error("Empty Data") {}
};


/**
 * FormatException
 * Thrown when a string that shall be converted into
 * something else contained the wrong characters
 */
class FormatException : public std::runtime_error {
public:
    /// @brief constructor
    FormatException(const std::string& msg)
        : std::runtime_error(msg) {}
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
    NumberFormatException()
        : FormatException("Number Format") {}
};


/**
 * BoolFormatException
 * Thrown when the string that shall be converted into a
 * boolean does not match
 */
class BoolFormatException : public FormatException {
public:
    /// @brief constructor
    BoolFormatException()
        : FormatException("Bool Format") {}
};


/**
 * OutOfBoundsException
 * Thrown when an array element out of the array's
 * bounderies is accessed
 */
class OutOfBoundsException : public std::runtime_error {
public:
    /// @brief constructor
    OutOfBoundsException()
        : std::runtime_error("Out Of Bounds") {}
};


/**
 * UnknownElement
 * Thrown when a named element is tried to be accesed
 * which is not known to the container
 */
class UnknownElement : public std::runtime_error {
public:
    /// @brief constructor
    UnknownElement()
        : std::runtime_error("Unknown Element") {}

    /// @brief constructor
    UnknownElement(const std::string& msg)
        : std::runtime_error(msg) {}
};


class IOError : public ProcessError {
public:
    /// @brief constructor
    IOError(const std::string& message)
        : ProcessError(message) {}
};


#endif

/****************************************************************************/
