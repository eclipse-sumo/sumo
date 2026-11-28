/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSOutputMissingValues.h
/// @author  Josh
/// @date    2026-11-26
///
/****************************************************************************/
#pragma once

#include <cmath>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

namespace MSOutputMissingValues {

template<class Attr, class Value>
OutputDevice& write(OutputDevice& output, const Attr& attr, const Value& value,
                    bool missing, bool tabularNull = false) {
    if (missing && output.isXML()) {
        const auto& mode = OptionsCont::getOptions().getString("output.xml-missing-values");
        if (mode == "omit") {
            return output;
        }
        if (mode == "empty") {
            return output.writeAttr(attr, "");
        }
    }
    return output.writeAttr(attr, value, tabularNull);
}

}
