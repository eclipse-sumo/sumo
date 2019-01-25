/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDevice_String.cpp
/// @author  Michael Behrisch
/// @date    2009
/// @version $Id$
///
// An output device that encapsulates a stringstream
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <sstream>
#include <string>
#include "OutputDevice_String.h"


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_String::OutputDevice_String(const bool binary, const int defaultIndentation)
    : OutputDevice(binary, defaultIndentation) {
    setPrecision();
    myStream << std::setiosflags(std::ios::fixed);
}


OutputDevice_String::~OutputDevice_String() {
}


std::string
OutputDevice_String::getString() const {
    return myStream.str();
}


std::ostream&
OutputDevice_String::getOStream() {
    return myStream;
}


/****************************************************************************/
