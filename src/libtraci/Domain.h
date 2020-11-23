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


#define LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(CLASS, DOMAIN) \
void CLASS::subscribe(const std::string& objectID, const std::vector<int>& varIDs, double begin, double end, const libsumo::TraCIResults& params) { \
    libtraci::Connection::getActive().subscribeObjectVariable(libsumo::CMD_SUBSCRIBE_##DOMAIN##_VARIABLE, objectID, begin, end, varIDs, params); \
} \
\
void CLASS::unsubscribe(const std::string& objectID) { \
    subscribe(objectID, std::vector<int>()); \
} \
\
void CLASS::subscribeContext(const std::string& objectID, int domain, double dist, const std::vector<int>& varIDs, double begin, double end, const libsumo::TraCIResults& params) { \
    libtraci::Connection::getActive().subscribeObjectContext(libsumo::CMD_SUBSCRIBE_##DOMAIN##_CONTEXT, objectID, begin, end, domain, dist, varIDs, params); \
} \
\
void CLASS::unsubscribeContext(const std::string& objectID, int domain, double dist) { \
    subscribeContext(objectID, domain, dist, std::vector<int>()); \
} \
\
const libsumo::SubscriptionResults CLASS::getAllSubscriptionResults() { \
    return libtraci::Connection::getActive().getAllSubscriptionResults(libsumo::RESPONSE_SUBSCRIBE_##DOMAIN##_VARIABLE); \
} \
\
const libsumo::TraCIResults CLASS::getSubscriptionResults(const std::string& objectID) { \
    return libtraci::Connection::getActive().getAllSubscriptionResults(libsumo::RESPONSE_SUBSCRIBE_##DOMAIN##_VARIABLE)[objectID]; \
} \
\
const libsumo::ContextSubscriptionResults CLASS::getAllContextSubscriptionResults() { \
    return libtraci::Connection::getActive().getAllContextSubscriptionResults(libsumo::RESPONSE_SUBSCRIBE_##DOMAIN##_CONTEXT); \
} \
\
const libsumo::SubscriptionResults CLASS::getContextSubscriptionResults(const std::string& objectID) { \
    return libtraci::Connection::getActive().getAllContextSubscriptionResults(libsumo::RESPONSE_SUBSCRIBE_##DOMAIN##_VARIABLE)[objectID]; \
} \
\
void CLASS::subscribeParameterWithKey(const std::string& objectID, const std::string& key, double beginTime, double endTime) { \
    subscribe(objectID, std::vector<int>({libsumo::VAR_PARAMETER_WITH_KEY}), beginTime, endTime, libsumo::TraCIResults {{libsumo::VAR_PARAMETER_WITH_KEY, std::make_shared<libsumo::TraCIString>(key)}}); \
}


#define LIBTRACI_PARAMETER_IMPLEMENTATION(CLASS, DOMAIN) \
std::string \
CLASS::getParameter(const std::string& objectID, const std::string& param) { \
    tcpip::Storage content; \
    content.writeByte(libsumo::TYPE_STRING); \
    content.writeString(param); \
    return Dom::getString(libsumo::VAR_PARAMETER, objectID, &content); \
} \
\
void \
CLASS::setParameter(const std::string& objectID, const std::string& key, const std::string& value) { \
    tcpip::Storage content; \
    content.writeUnsignedByte(libsumo::TYPE_COMPOUND); \
    content.writeInt(2); \
    content.writeUnsignedByte(libsumo::TYPE_STRING); \
    content.writeString(key); \
    content.writeUnsignedByte(libsumo::TYPE_STRING); \
    content.writeString(value); \
    Connection::getActive().doCommand(libsumo::CMD_SET_##DOMAIN##_VARIABLE, libsumo::VAR_PARAMETER, objectID, &content); \
} \
\
const std::pair<std::string, std::string> \
CLASS::getParameterWithKey(const std::string& objectID, const std::string& key) { \
    return std::make_pair(key, getParameter(objectID, key)); \
}


// ===========================================================================
// class and type definitions
// ===========================================================================
namespace libtraci {
template<int GET, int SET>
class Domain {
public:
    static int getUnsignedByte(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getUnsignedByte(GET, var, id, add);
    }

    static int getByte(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getByte(GET, var, id, add);
    }

    static int getInt(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getInt(GET, var, id, add);
    }

    static double getDouble(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getDouble(GET, var, id, add);
    }

    static libsumo::TraCIPositionVector getPolygon(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getPolygon(GET, var, id, add);
    }

    static libsumo::TraCIPosition getPos(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getPos(GET, var, id, add);
    }

    static libsumo::TraCIPosition getPos3D(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getPos3D(GET, var, id, add);
    }

    static std::string getString(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getString(GET, var, id, add);
    }

    static std::vector<std::string> getStringVector(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getStringVector(GET, var, id, add);
    }

    static libsumo::TraCIColor getCol(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        return libtraci::Connection::getActive().getCol(GET, var, id, add);
    }

    static libsumo::TraCIStage getTraCIStage(int var, const std::string& id, tcpip::Storage* add = nullptr) {
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

    static void setCol(int var, const std::string& id, const libsumo::TraCIColor value) {
        libtraci::Connection::getActive().setCol(SET, var, id, value);
    }
};

}
