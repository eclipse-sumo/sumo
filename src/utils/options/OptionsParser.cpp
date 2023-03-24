/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    OptionsParser.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
///
// Parses the command line arguments
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <cstring>
#include "Option.h"
#include "OptionsCont.h"
#include "OptionsParser.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>


// ===========================================================================
// method definitions
// ===========================================================================
bool
OptionsParser::parse(const std::vector<std::string>& args, const bool ignoreAppenders) {
    bool ok = true;
    const int argc = (int)args.size();
    for (int i = 1; i < argc;) {
        try {
            int add;
            // try to set the current option
            if (i < argc - 1) {
                add = check(args[i], &args[i + 1], ok, ignoreAppenders);
            } else {
                add = check(args[i], nullptr, ok, ignoreAppenders);
            }
            i += add;
        } catch (ProcessError& e) {
            WRITE_ERROR("On processing option '" + args[i] + "':\n " + e.what());
            i++;
            ok = false;
        }
    }
    return ok;
}


int
OptionsParser::check(const std::string& arg1, const std::string* const arg2, bool& ok, const bool ignoreAppenders) {
    // the first argument should be an option
    // (only the second may be a free string)
    if (!checkParameter(arg1)) {
        ok = false;
        return 1;
    }

    OptionsCont& oc = OptionsCont::getOptions();
    const bool append = arg1[0] == '+';
    // process not abbreviated switches
    if (append || arg1[1] == '-') {
        const std::string tmp(arg1.substr(append ? 1 : 2));
        const std::string::size_type idx1 = tmp.find('=');
        if (append && ignoreAppenders) {
            return idx1 == std::string::npos ? 2 : 1;
        }
        // check whether a parameter was submitted
        if (idx1 != std::string::npos) {
            ok &= oc.set(tmp.substr(0, idx1), tmp.substr(idx1 + 1), append);
        } else {
            if (arg2 == nullptr || (oc.isBool(tmp) && (*arg2)[0] == '-')) {
                ok &= oc.set(tmp, "true");
            } else {
                ok &= oc.set(tmp, *arg2, append);
                return 2;
            }
        }
        return 1;
    }
    // go through the abbreviated switches
    const int len = (int)arg1.size();
    for (int i = 1; i < len; i++) {
        // set boolean switches
        const std::string abbr = arg1.substr(i, 1);
        if (oc.isBool(abbr)) {
            if (arg2 == nullptr || (*arg2)[0] == '-' || i != len - 1) {
                ok &= oc.set(abbr, "true");
            } else {
                ok &= oc.set(abbr, *arg2);
                return 2;
            }
            // set non-boolean switches
        } else {
            // check whether the parameter comes directly after the switch
            //  and process if so
            if (arg2 == nullptr || i != len - 1) {
                ok &= processNonBooleanSingleSwitch(oc, arg1.substr(i), append);
                return 1;
                // process parameter following after a space
            } else {
                ok &= oc.set(abbr, *arg2, append);
                // option name and attribute were in two arguments
                return 2;
            }
        }
    }
    // all switches within the current argument were boolean switches
    return 1;
}


bool
OptionsParser::processNonBooleanSingleSwitch(OptionsCont& oc, const std::string& arg, const bool append) {
    if (arg[1] == '=') {
        if (arg.size() < 3) {
            WRITE_ERRORF(TL("Missing value for parameter '%'."), arg.substr(0, 1));
            return false;
        } else {
            return oc.set(arg.substr(0, 1), arg.substr(2), append);
        }
    } else {
        if (arg.size() < 2) {
            WRITE_ERRORF(TL("Missing value for parameter '%'."), arg);
            return false;
        } else {
            return oc.set(arg.substr(0, 1), arg.substr(1), append);
        }
    }
}


bool
OptionsParser::checkParameter(const std::string& arg1) {
    if (arg1[0] != '-' && arg1[0] != '+') {
        WRITE_ERRORF(TL("The parameter '%' is not allowed in this context.\n Switch or parameter name expected."), arg1);
        return false;
    }
    if ((arg1[0] == '-' && arg1[1] == '+') || (arg1[0] == '+' && arg1[1] == '-')) {
        WRITE_ERRORF(TL("Mixed parameter syntax in '%'."), arg1);
        return false;
    }
    return true;
}


/****************************************************************************/
