/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    StorageHelper.h
/// @author  Michael Behrisch
/// @date    2020-12-17
///
// Functions for reading, writing and converting TraCIResults to tcpip::Storage
/****************************************************************************/
#pragma once
#include <config.h>
#include <foreign/tcpip/storage.h>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class definitions
// ===========================================================================
namespace libsumo {

class StorageHelper {
public:
    static std::shared_ptr<tcpip::Storage> toStorage(const TraCIResult& v) {
        std::shared_ptr<tcpip::Storage> result = std::make_shared<tcpip::Storage>();
        result->writeUnsignedByte(v.getType());
        switch (v.getType()) {
            case TYPE_STRING:
                result->writeString(v.getString());
                break;
            case TYPE_DOUBLE:
                result->writeDouble(((const TraCIDouble&)v).value);
                break;
            default:
                // Error!
                break;
        }
        return result;
    }

};


}