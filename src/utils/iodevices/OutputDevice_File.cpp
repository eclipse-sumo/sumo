/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    OutputDevice_File.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2004
/// @version $Id$
///
// An output device that encapsulates an ofstream
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <cstring>
#include <cerrno>
#include <utils/common/UtilExceptions.h>
#include "OutputDevice_File.h"


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_File::OutputDevice_File(const std::string& fullName, const bool binary)
    : OutputDevice(binary), myFileStream(0) {
#ifdef WIN32
    if (fullName == "/dev/null") {
        myFileStream = new std::ofstream("NUL");
    } else
#endif
        myFileStream = new std::ofstream(fullName.c_str(), binary ? std::ios::binary : std::ios_base::out);
    if (!myFileStream->good()) {
        delete myFileStream;
        throw IOError("Could not build output file '" + fullName + "' (" + std::strerror(errno) + ").");
    }
}


OutputDevice_File::~OutputDevice_File() {
    myFileStream->close();
    delete myFileStream;
}


std::ostream&
OutputDevice_File::getOStream() {
    return *myFileStream;
}


/****************************************************************************/

