/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <vector>
#include <limits>
#include <map>
#include <string>
#include <stdexcept>
#include <sstream>
#include <memory>
#include <foreign/tcpip/storage.h>
#include <libtraci/Connection.h>
#include <libsumo/StorageHelper.h>


#define LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(CLASS, DOMAIN) \
const int CLASS::DOMAIN_ID(libsumo::CMD_GET_##DOMAIN##_VARIABLE); \
void CLASS::subscribe(const std::string& objectID, const std::vector<int>& varIDs, double begin, double end, const libsumo::TraCIResults& params) { \
    libtraci::Connection::getActive().subscribe(libsumo::CMD_SUBSCRIBE_##DOMAIN##_VARIABLE, objectID, begin, end, -1, -1, varIDs, params); \
} \
\
void CLASS::unsubscribe(const std::string& objectID) { \
    subscribe(objectID, std::vector<int>()); \
} \
\
void CLASS::subscribeContext(const std::string& objectID, int domain, double dist, const std::vector<int>& varIDs, double begin, double end, const libsumo::TraCIResults& params) { \
    libtraci::Connection::getActive().subscribe(libsumo::CMD_SUBSCRIBE_##DOMAIN##_CONTEXT, objectID, begin, end, domain, dist, varIDs, params); \
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
    return libtraci::Connection::getActive().getAllContextSubscriptionResults(libsumo::RESPONSE_SUBSCRIBE_##DOMAIN##_CONTEXT)[objectID]; \
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
    static inline tcpip::Storage& get(int var, const std::string& id, tcpip::Storage* add = nullptr, int expectedType = libsumo::TYPE_COMPOUND) {
        return libtraci::Connection::getActive().doCommand(GET, var, id, add, expectedType);
    }

    static int getUnsignedByte(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        return get(var, id, add, libsumo::TYPE_UBYTE).readUnsignedByte();
    }

    static int getByte(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        return get(var, id, add, libsumo::TYPE_BYTE).readByte();
    }

    static int getInt(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        return get(var, id, add, libsumo::TYPE_INTEGER).readInt();
    }

    static double getDouble(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        return get(var, id, add, libsumo::TYPE_DOUBLE).readDouble();
    }

    static libsumo::TraCIPositionVector getPolygon(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        tcpip::Storage& result = get(var, id, add, libsumo::TYPE_POLYGON);
        libsumo::TraCIPositionVector ret;
        int size = result.readUnsignedByte();
        if (size == 0) {
            size = result.readInt();
        }
        for (int i = 0; i < size; ++i) {
            libsumo::TraCIPosition p;
            p.x = result.readDouble();
            p.y = result.readDouble();
            p.z = 0.;
            ret.value.push_back(p);
        }
        return ret;
    }

    static libsumo::TraCIPosition getPos(int var, const std::string& id, tcpip::Storage* add = nullptr, const bool isGeo = false) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        tcpip::Storage& result = get(var, id, add, isGeo ? libsumo::POSITION_LON_LAT : libsumo::POSITION_2D);
        libsumo::TraCIPosition p;
        p.x = result.readDouble();
        p.y = result.readDouble();
        return p;
    }

    static libsumo::TraCIPosition getPos3D(int var, const std::string& id, tcpip::Storage* add = nullptr, const bool isGeo = false) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        tcpip::Storage& result = get(var, id, add, isGeo ? libsumo::POSITION_LON_LAT_ALT : libsumo::POSITION_3D);
        libsumo::TraCIPosition p;
        p.x = result.readDouble();
        p.y = result.readDouble();
        p.z = result.readDouble();
        return p;
    }

    static std::string getString(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        return get(var, id, add, libsumo::TYPE_STRING).readString();
    }

    static std::vector<std::string> getStringVector(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        return get(var, id, add, libsumo::TYPE_STRINGLIST).readStringList();
    }

    static std::vector<double> getDoubleVector(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        return get(var, id, add, libsumo::TYPE_DOUBLELIST).readDoubleList();
    }

    static libsumo::TraCIColor getCol(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        tcpip::Storage& result = get(var, id, add, libsumo::TYPE_COLOR);
        libsumo::TraCIColor c;
        c.r = (unsigned char)result.readUnsignedByte();
        c.g = (unsigned char)result.readUnsignedByte();
        c.b = (unsigned char)result.readUnsignedByte();
        c.a = (unsigned char)result.readUnsignedByte();
        return c;
    }

    static libsumo::TraCIStage getTraCIStage(int var, const std::string& id, tcpip::Storage* add = nullptr) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        tcpip::Storage& result = get(var, id, add);
        libsumo::TraCIStage s;
        result.readInt(); // components
        s.type = StoHelp::readTypedInt(result);
        s.vType = StoHelp::readTypedString(result);
        s.line = StoHelp::readTypedString(result);
        s.destStop = StoHelp::readTypedString(result);
        s.edges = StoHelp::readTypedStringList(result);
        s.travelTime = StoHelp::readTypedDouble(result);
        s.cost = StoHelp::readTypedDouble(result);
        s.length = StoHelp::readTypedDouble(result);
        s.intended = StoHelp::readTypedString(result);
        s.depart = StoHelp::readTypedDouble(result);
        s.departPos = StoHelp::readTypedDouble(result);
        s.arrivalPos = StoHelp::readTypedDouble(result);
        s.description = StoHelp::readTypedString(result);
        return s;
    }

    static void set(int var, const std::string& id, tcpip::Storage* add) {
        std::unique_lock<std::mutex> lock{ libtraci::Connection::getActive().getMutex() };
        libtraci::Connection::getActive().doCommand(SET, var, id, add);
    }

    static void setInt(int var, const std::string& id, int value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_INTEGER);
        content.writeInt(value);
        set(var, id, &content);
    }

    static void setDouble(int var, const std::string& id, double value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(value);
        set(var, id, &content);
    }

    static void setString(int var, const std::string& id, const std::string& value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(value);
        set(var, id, &content);
    }

    static void setStringVector(int var, const std::string& id, const std::vector<std::string>& value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
        content.writeStringList(value);
        set(var, id, &content);
    }

    static void setCol(int var, const std::string& id, const libsumo::TraCIColor value) {
        tcpip::Storage content;
        content.writeUnsignedByte(libsumo::TYPE_COLOR);
        content.writeUnsignedByte(value.r);
        content.writeUnsignedByte(value.g);
        content.writeUnsignedByte(value.b);
        content.writeUnsignedByte(value.a);
        set(var, id, &content);
    }

};

}
