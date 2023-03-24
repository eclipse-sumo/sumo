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

    static int readTypedInt(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_INTEGER && error != "") {
            throw TraCIException(error);
        }
        return ret.readInt();
    }

    static int readTypedByte(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_BYTE && error != "") {
            throw TraCIException(error);
        }
        return ret.readByte();
    }

    static double readTypedDouble(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_DOUBLE && error != "") {
            throw TraCIException(error);
        }
        return ret.readDouble();
    }

    static std::string readTypedString(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_STRING && error != "") {
            throw TraCIException(error);
        }
        return ret.readString();
    }

    static std::vector<std::string> readTypedStringList(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_STRINGLIST && error != "") {
            throw TraCIException(error);
        }
        return ret.readStringList();
    }

    static int readCompound(tcpip::Storage& ret, int expectedSize = -1, const std::string& error = "") {
        const int type = ret.readUnsignedByte();
        const int size = ret.readInt();
        if (error != "") {
            if (type != libsumo::TYPE_COMPOUND || (expectedSize != -1 && size != expectedSize)) {
                throw TraCIException(error);
            }
        }
        return size;
    }

    static void readStage(tcpip::Storage& inputStorage, libsumo::TraCIStage& stage, const std::string& error = "") {
        stage.type = readTypedInt(inputStorage, error);
        stage.vType = readTypedString(inputStorage, error);
        stage.line = readTypedString(inputStorage, error);
        stage.destStop = readTypedString(inputStorage, error);
        stage.edges = readTypedStringList(inputStorage, error);
        stage.travelTime = readTypedDouble(inputStorage, error);
        stage.cost = readTypedDouble(inputStorage, error);
        stage.length = readTypedDouble(inputStorage, error);
        stage.intended = readTypedString(inputStorage, error);
        stage.depart = readTypedDouble(inputStorage, error);
        stage.departPos = readTypedDouble(inputStorage, error);
        stage.arrivalPos = readTypedDouble(inputStorage, error);
        stage.description = readTypedString(inputStorage, error);
    }


    static void writeTypedByte(tcpip::Storage& content, int value) {
        content.writeUnsignedByte(libsumo::TYPE_BYTE);
        content.writeByte(value);
    }

    static void writeTypedInt(tcpip::Storage& content, int value) {
        content.writeUnsignedByte(libsumo::TYPE_INTEGER);
        content.writeInt(value);
    }

    static void writeTypedDouble(tcpip::Storage& content, double value) {
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(value);
    }

    static void writeTypedString(tcpip::Storage& content, const std::string& value) {
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(value);
    }

    static void writeTypedStringList(tcpip::Storage& content, const std::vector<std::string>& value) {
        content.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
        content.writeStringList(value);
    }

    static void writeCompound(tcpip::Storage& content, int size) {
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(size);
    }

    static void writePolygon(tcpip::Storage& content, const libsumo::TraCIPositionVector& shape) {
        content.writeUnsignedByte(libsumo::TYPE_POLYGON);
        if (shape.value.size() <= 255) {
            content.writeUnsignedByte((int)shape.value.size());
        } else {
            content.writeUnsignedByte(0);
            content.writeInt((int)shape.value.size());
        }
        for (const libsumo::TraCIPosition& pos : shape.value) {
            content.writeDouble(pos.x);
            content.writeDouble(pos.y);
        }
    }

    static void writeStage(tcpip::Storage& outputStorage, const libsumo::TraCIStage& stage) {
        writeCompound(outputStorage, 13);
        outputStorage.writeUnsignedByte(libsumo::TYPE_INTEGER);
        outputStorage.writeInt(stage.type);
        writeTypedString(outputStorage, stage.vType);
        writeTypedString(outputStorage, stage.line);
        writeTypedString(outputStorage, stage.destStop);
        writeTypedStringList(outputStorage, stage.edges);
        writeTypedDouble(outputStorage, stage.travelTime);
        writeTypedDouble(outputStorage, stage.cost);
        writeTypedDouble(outputStorage, stage.length);
        writeTypedString(outputStorage, stage.intended);
        writeTypedDouble(outputStorage, stage.depart);
        writeTypedDouble(outputStorage, stage.departPos);
        writeTypedDouble(outputStorage, stage.arrivalPos);
        writeTypedString(outputStorage, stage.description);
    }

};


}

typedef libsumo::StorageHelper StoHelp;
