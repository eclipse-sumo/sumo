/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2009-2024 German Aerospace Center (DLR) and others.
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
/// @file    OutputDevice_String.cpp
/// @author  Michael Behrisch
/// @date    2009
///
// An output device that encapsulates a stringstream
/****************************************************************************/
#include <config.h>

#include <sstream>
#include <string>
#include "OutputDevice_String.h"


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_String::OutputDevice_String(const int defaultIndentation)
    : OutputDevice(defaultIndentation) {
    // auto stream = new std::ostringstream();
    // (*stream) << std::setiosflags(std::ios::fixed);
    myStreamDevice = new OStreamDevice(new std::ostringstream());
    myStreamDevice->setOSFlags(std::ios::fixed);
    myStreamDevice->setPrecision(2);
}


OutputDevice_String::~OutputDevice_String() {
}


std::string
OutputDevice_String::getString() const {
    return myStreamDevice->str();
}

/****************************************************************************/
