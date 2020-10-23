/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2020 German Aerospace Center (DLR) and others.
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
/// @file    Domain.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
// we do not include config.h here, since we should be independent of a special sumo build
#include <vector>
#include <limits>
#include <map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <foreign/tcpip/storage.h>
#include <libtraci/Connection.h>


// ===========================================================================
// class and type definitions
// ===========================================================================
namespace libtraci {
template<int GET, int SET, int SUBSCRIBE, int CONTEXT>
class Domain {
public:
    static void subscribe(const std::string& objectID, const std::vector<int>& varIDs = std::vector<int>({-1}),
                          double begin = libsumo::INVALID_DOUBLE_VALUE, double end = libsumo::INVALID_DOUBLE_VALUE) {
        libtraci::Connection::getActive().subscribeObjectVariable(SUBSCRIBE, objectID, begin, end, varIDs);
    }

    static void unsubscribe(const std::string& objectID) {
        subscribe(objectID, std::vector<int>());
    }

    static void subscribeContext(const std::string& objectID, int domain, double dist, const std::vector<int>& varIDs = std::vector<int>({-1}),
                                 double begin = libsumo::INVALID_DOUBLE_VALUE, double end = libsumo::INVALID_DOUBLE_VALUE) {
        libtraci::Connection::getActive().subscribeObjectContext(CONTEXT, objectID, begin, end, domain, dist, varIDs);
    }

    static void unsubscribeContext(const std::string& objectID, int domain, double dist) {
        subscribeContext(objectID, domain, dist, std::vector<int>());
    }

    static const libsumo::SubscriptionResults getAllSubscriptionResults() {
        return libtraci::Connection::getActive().getAllSubscriptionResults(SUBSCRIBE);
    }

    static const libsumo::TraCIResults getSubscriptionResults(const std::string& objectID) {
        return libtraci::Connection::getActive().getAllSubscriptionResults(SUBSCRIBE)[objectID];
    }

    static const libsumo::ContextSubscriptionResults getAllContextSubscriptionResults() {
        return libtraci::Connection::getActive().getAllContextSubscriptionResults(CONTEXT);
    }

    static const libsumo::SubscriptionResults getContextSubscriptionResults(const std::string& objectID) {
        return libtraci::Connection::getActive().getAllContextSubscriptionResults(SUBSCRIBE)[objectID];
    }

    static int getUnsignedByte(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getUnsignedByte(GET, var, id, add);
    }

    static int getByte(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getByte(GET, var, id, add);
    }

    static int getInt(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getInt(GET, var, id, add);
    }

    static double getDouble(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getDouble(GET, var, id, add);
    }

    static libsumo::TraCIPositionVector getPolygon(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getPolygon(GET, var, id, add);
    }

    static libsumo::TraCIPosition getPos(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getPos(GET, var, id, add);
    }

    static libsumo::TraCIPosition getPos3D(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getPos3D(GET, var, id, add);
    }

    static std::string getString(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getString(GET, var, id, add);
    }

    static std::vector<std::string> getStringVector(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getStringVector(GET, var, id, add);
    }

    static libsumo::TraCIColor getCol(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getCol(GET, var, id, add);
    }

    static libsumo::TraCIStage getTraCIStage(int var, const std::string& id, tcpip::Storage* add=nullptr) {
        return libtraci::Connection::getActive().getTraCIStage(GET, var, id, add);
    }

    static void setInt(int var, const std::string& id, int value) {
        libtraci::Connection::getActive().setInt(SET, var, id, value);
    }

    static void setDouble(int var, const std::string& id, double value) {
        libtraci::Connection::getActive().setDouble(SET, var, id, value);
    }

    static void setString(int var, const std::string& id, const std::string& value) {
        libtraci::Connection::getActive().setString(SET, var, id, value);
    }

    static void setStringVector(int var, const std::string& id, const std::vector<std::string>& value) {
        libtraci::Connection::getActive().setStringVector(SET, var, id, value);
    }
};

}
