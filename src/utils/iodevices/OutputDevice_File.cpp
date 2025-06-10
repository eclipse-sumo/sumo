/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2004-2025 German Aerospace Center (DLR) and others.
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
/// @file    OutputDevice_File.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2004
///
// An output device that encapsulates an ofstream
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <cstring>
#include <cerrno>
#ifdef HAVE_ZLIB
#include <foreign/zstr/zstr.hpp>
#endif
#include <utils/common/StringUtils.h>
#include <utils/common/UtilExceptions.h>
#include "OutputDevice_File.h"


// ===========================================================================
// method definitions
// ===========================================================================
OutputDevice_File::OutputDevice_File(const std::string& fullName, const bool compressed)
    : OutputDevice(0, fullName) {
    std::unique_ptr<std::ostream> fileStream;
    
    if (fullName == "/dev/null") {
        myAmNull = true;
#ifdef WIN32
        fileStream = std::make_unique<std::ofstream>("NUL");
        if (!fileStream->good()) {
            throw IOError(TLF("Could not redirect to NUL device (%).", std::string(std::strerror(errno))));
        }
        myStreamDevice = std::make_unique<StreamDevice>(std::move(fileStream));
        return;
#endif
    }
    const std::string& localName = StringUtils::transcodeToLocal(fullName);
#ifdef HAVE_ZLIB
    if (compressed) {
        try {
            fileStream = std::make_unique<zstr::ofstream>(localName.c_str(), std::ios_base::out);
        } catch (strict_fstream::Exception& e) {
            throw IOError("Could not build output file '" + fullName + "' (" + e.what() + ").");
        } catch (zstr::Exception& e) {
            throw IOError("Could not build output file '" + fullName + "' (" + e.what() + ").");
        }
    } else {
        fileStream = std::make_unique<std::ofstream>(localName.c_str(), std::ios_base::out);
    }
#else
    UNUSED_PARAMETER(compressed);
    fileStream = std::make_unique<std::ofstream>(localName.c_str(), std::ios_base::out);
#endif
    if (!fileStream->good()) {
        throw IOError("Could not build output file '" + fullName + "' (" + std::strerror(errno) + ").");
    }
    
    // Create StreamDevice with the file stream
    myStreamDevice = std::make_unique<StreamDevice>(std::move(fileStream));
}


StreamDevice&
OutputDevice_File::getStreamDevice() {
    return *myStreamDevice;
}


/****************************************************************************/