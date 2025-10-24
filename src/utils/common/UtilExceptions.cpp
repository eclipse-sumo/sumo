/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    UtilExceptions.cpp
/// @author  Daniel Krajzewicz
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @author  Felix Brack
/// @date    Mon, 17 Dec 2001
///
// Exceptions for used by some utility classes
/****************************************************************************/
#include <config.h>

// stacktrace is not supported in MAC
#if defined(HAVE_BOOST) && !defined(__APPLE__)
#ifdef _MSC_VER
// needed to avoid problem in boost win winsocket
#pragma warning(push, 0)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <boost/stacktrace.hpp>
#pragma warning(pop)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <boost/stacktrace.hpp>
#pragma GCC diagnostic pop
#endif
#endif

#include "UtilExceptions.h"

// ===========================================================================
// class definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

ProcessError::ProcessError() :
    std::runtime_error(TL("Process Error")) {
    // process trace
    processTrace();
}


ProcessError::ProcessError(const std::string& msg) :
    std::runtime_error(msg) {
    // process trace
    processTrace();
}


const std::string&
ProcessError::getTrace() const {
    return myTrace;
}


void
ProcessError::processTrace() {
// only process if we have boost and we're not in apple
#if defined(HAVE_BOOST) && !defined(__APPLE__)
    // declare stacktrace
    boost::stacktrace::stacktrace st;
    // convert trace using ostringstream
    std::ostringstream oss;
    oss << st;
    myTrace = oss.str();
#endif
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

InvalidArgument::InvalidArgument(const std::string& message) :
    ProcessError(message) {
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

EmptyData::EmptyData() :
    ProcessError(TL("Empty Data")) {
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

FormatException::FormatException(const std::string& msg) :
    ProcessError(msg) {
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

NumberFormatException::NumberFormatException(const std::string& data) :
    FormatException(TLF("Invalid Number Format %", data)) {
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

TimeFormatException::TimeFormatException(const std::string& data) :
    FormatException(TLF("Invalid Time Format %", data)) {
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

BoolFormatException::BoolFormatException(const std::string& data) :
    FormatException(TLF("Invalid Bool Format %", data)) {
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

OutOfBoundsException::OutOfBoundsException(const std::string& msg) :
    ProcessError(msg) {
}


// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

UnknownElement::UnknownElement() :
    ProcessError(TL("Unknown Element")) {
}


UnknownElement::UnknownElement(const std::string& msg) :
    ProcessError(msg) {
}

// ---------------------------------------------------------------------------
// ProcessError - methods
// ---------------------------------------------------------------------------

IOError::IOError(const std::string& message) :
    ProcessError(message) {
}
