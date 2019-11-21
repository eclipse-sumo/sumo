/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NITypeLoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Perfoms network import
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOSAXReader.h>
#include <utils/xml/XMLSubSys.h>
#include "NITypeLoader.h"


void
NITypeLoader::load(SUMOSAXHandler* handler, const std::vector<std::string>& files,
                      const std::string& type, const bool stringParse) {
    // build parser
    std::string exceptMsg = "";
    // start the parsing
    try {
        for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
            if (stringParse) {
                handler->setFileName("built in type map");
                SUMOSAXReader* reader = XMLSubSys::getSAXReader(*handler);
                reader->parseString(*file);
                delete reader;
                continue;
            }
            if (!FileHelpers::isReadable(*file)) {
                WRITE_ERROR("Could not open " + type + "-file '" + *file + "'.");
                exceptMsg = "Process Error";
                continue;
            }
            PROGRESS_BEGIN_MESSAGE("Parsing " + type + " from '" + *file + "'");
            XMLSubSys::runParser(*handler, *file);
            PROGRESS_DONE_MESSAGE();
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
        exceptMsg = StringUtils::transcode(toCatch.getMessage())
                    + "\n  The " + type + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (const ProcessError& toCatch) {
        exceptMsg =
            std::string(toCatch.what()) + "\n  The " + type + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (...) {
        exceptMsg = "The " + type + " could not be loaded from '" + handler->getFileName() + "'.";
    }
    delete handler;
    if (exceptMsg != "") {
        throw ProcessError(exceptMsg);
    }
}

/****************************************************************************/
