/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
#include <utils/common/ToString.h>


// ===========================================================================
// class definitions
// ===========================================================================
namespace libsumo {

class StorageHelper {
public:
    static inline std::shared_ptr<tcpip::Storage> toStorage(const TraCIResult& v) {
        std::shared_ptr<tcpip::Storage> result = std::make_shared<tcpip::Storage>();
        if (v.getType() == POSITION_ROADMAP || v.getType() == POSITION_2D || v.getType() == POSITION_3D) {
            writeCompound(*result, 2);
        }
        if (v.getType() != -1) {
            result->writeUnsignedByte(v.getType());
        }
        switch (v.getType()) {
            case TYPE_STRING:
                result->writeString(v.getString());
                break;
            case TYPE_DOUBLE:
                result->writeDouble(((const TraCIDouble&)v).value);
                break;
            case TYPE_INTEGER:
                result->writeInt(((const TraCIInt&)v).value);
                break;
            case TYPE_BYTE:
                result->writeByte(((const TraCIInt&)v).value);
                break;
            case TYPE_UBYTE:
                result->writeUnsignedByte(((const TraCIInt&)v).value);
                break;
            case POSITION_ROADMAP:
                result->writeString(((const TraCIRoadPosition&)v).edgeID);
                result->writeDouble(((const TraCIRoadPosition&)v).pos);
                result->writeUnsignedByte(((const TraCIRoadPosition&)v).laneIndex);
                break;
            case POSITION_2D:
                result->writeDouble(((const TraCIPosition&)v).x);
                result->writeDouble(((const TraCIPosition&)v).y);
                break;
            case POSITION_3D:
                result->writeDouble(((const TraCIPosition&)v).x);
                result->writeDouble(((const TraCIPosition&)v).y);
                result->writeDouble(((const TraCIPosition&)v).z);
                break;
            case -1: {
                // a hack for transfering multiple values
                const auto& pl = ((const TraCIStringDoublePairList&)v).value;
                const bool tisb = pl.size() == 2 && pl[0].first != "";
                writeCompound(*result, pl.size() == 2 && !tisb ? 2 : (int)pl.size() + 1);
                if (pl.size() == 1) {
                    writeTypedDouble(*result, pl.front().second);
                    writeTypedString(*result, pl.front().first);
                } else if (pl.size() == 2) {
                    if (tisb) {
                        writeTypedInt(*result, (int)(pl.front().second + 0.5));
                        writeTypedString(*result, pl.front().first);
                        writeTypedByte(*result, (int)(pl.back().second + 0.5));
                    } else {
                        writeTypedDouble(*result, pl.front().second);
                        writeTypedDouble(*result, pl.back().second);
                    }
                } else if (pl.size() == 3) {
                    writeTypedDouble(*result, pl[0].second);
                    writeTypedDouble(*result, pl[1].second);
                    writeTypedDouble(*result, pl[2].second);
                    writeTypedString(*result, pl[2].first);
                } else if (pl.size() == 4) {
                    writeTypedDouble(*result, pl[0].second);
                    writeTypedDouble(*result, pl[1].second);
                    writeTypedDouble(*result, pl[2].second);
                    writeTypedDouble(*result, pl[3].second);
                    writeTypedString(*result, pl[3].first);
                }
                break;
            }
            default:
                throw TraCIException("Unknown type " + toHex(v.getType()));
        }
        if (v.getType() == POSITION_ROADMAP || v.getType() == POSITION_2D || v.getType() == POSITION_3D) {
            result->writeUnsignedByte(REQUEST_DRIVINGDIST);
        }
        return result;
    }

    static inline int readTypedInt(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_INTEGER && error != "") {
            throw TraCIException(error);
        }
        return ret.readInt();
    }

    static inline int readTypedByte(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_BYTE && error != "") {
            throw TraCIException(error);
        }
        return ret.readByte();
    }

    static inline int readTypedUnsignedByte(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_UBYTE && error != "") {
            throw TraCIException(error);
        }
        return ret.readUnsignedByte();
    }

    static inline double readTypedDouble(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_DOUBLE && error != "") {
            throw TraCIException(error);
        }
        return ret.readDouble();
    }

    static inline std::string readTypedString(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_STRING && error != "") {
            throw TraCIException(error);
        }
        return ret.readString();
    }

    static inline std::vector<std::string> readTypedStringList(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_STRINGLIST && error != "") {
            throw TraCIException(error);
        }
        return ret.readStringList();
    }

    static inline int readCompound(tcpip::Storage& ret, int expectedSize = -1, const std::string& error = "") {
        const int type = ret.readUnsignedByte();
        const int size = ret.readInt();
        if (error != "") {
            if (type != libsumo::TYPE_COMPOUND || (expectedSize != -1 && size != expectedSize)) {
                throw TraCIException(error);
            }
        }
        return size;
    }

    static inline void readPolygon(tcpip::Storage& ret, libsumo::TraCIPositionVector& poly, const std::string& error = "") {
        int size = ret.readUnsignedByte();
        if (size == 0) {
            size = ret.readInt();
        }
        for (int i = 0; i < size; ++i) {
            libsumo::TraCIPosition p;
            p.x = ret.readDouble();
            p.y = ret.readDouble();
            p.z = 0.;
            poly.value.emplace_back(p);
        }
    }

    static inline bool readBool(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_UBYTE && error != "") {
            throw TraCIException(error);
        }
        return ret.readUnsignedByte() != 0;
    }

    static inline void readStage(tcpip::Storage& inputStorage, libsumo::TraCIStage& stage, const std::string& error = "") {
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

    static inline void readConnection(tcpip::Storage& inputStorage, libsumo::TraCIConnection& connection, const std::string& error = "") {
        connection.approachedLane = readTypedString(inputStorage, error);
        connection.approachedInternal = readTypedString(inputStorage, error);
        connection.hasPrio = readBool(inputStorage, error);
        connection.isOpen = readBool(inputStorage, error);
        connection.hasFoe = readBool(inputStorage, error);
        connection.state = readTypedString(inputStorage, error);
        connection.direction = readTypedString(inputStorage, error);
        connection.length = readTypedDouble(inputStorage, error);
    }

    static inline void readVehicleDataVector(tcpip::Storage& inputStorage, std::vector<libsumo::TraCIVehicleData>& result, const std::string& error = "") {
        const int n = readTypedInt(inputStorage);
        for (int i = 0; i < n; ++i) {
            libsumo::TraCIVehicleData vd;
            vd.id = readTypedString(inputStorage);
            vd.length = readTypedDouble(inputStorage);
            vd.entryTime = readTypedDouble(inputStorage);
            vd.leaveTime = readTypedDouble(inputStorage);
            vd.typeID = readTypedString(inputStorage);
            result.push_back(vd);
        }
    }


    static inline void writeTypedByte(tcpip::Storage& content, int value) {
        content.writeUnsignedByte(libsumo::TYPE_BYTE);
        content.writeByte(value);
    }

    static inline void writeTypedUnsignedByte(tcpip::Storage& content, int value) {
        content.writeUnsignedByte(libsumo::TYPE_UBYTE);
        content.writeUnsignedByte(value);
    }

    static inline void writeTypedInt(tcpip::Storage& content, int value) {
        content.writeUnsignedByte(libsumo::TYPE_INTEGER);
        content.writeInt(value);
    }

    static inline void writeTypedDouble(tcpip::Storage& content, double value) {
        content.writeUnsignedByte(libsumo::TYPE_DOUBLE);
        content.writeDouble(value);
    }

    static inline void writeTypedString(tcpip::Storage& content, const std::string& value) {
        content.writeUnsignedByte(libsumo::TYPE_STRING);
        content.writeString(value);
    }

    static inline void writeTypedStringList(tcpip::Storage& content, const std::vector<std::string>& value) {
        content.writeUnsignedByte(libsumo::TYPE_STRINGLIST);
        content.writeStringList(value);
    }

    static inline void writeCompound(tcpip::Storage& content, int size) {
        content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
        content.writeInt(size);
    }

    static inline void writePolygon(tcpip::Storage& content, const libsumo::TraCIPositionVector& shape) {
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

    static inline void writeStage(tcpip::Storage& content, const libsumo::TraCIStage& stage) {
        writeCompound(content, 13);
        content.writeUnsignedByte(libsumo::TYPE_INTEGER);
        content.writeInt(stage.type);
        writeTypedString(content, stage.vType);
        writeTypedString(content, stage.line);
        writeTypedString(content, stage.destStop);
        writeTypedStringList(content, stage.edges);
        writeTypedDouble(content, stage.travelTime);
        writeTypedDouble(content, stage.cost);
        writeTypedDouble(content, stage.length);
        writeTypedString(content, stage.intended);
        writeTypedDouble(content, stage.depart);
        writeTypedDouble(content, stage.departPos);
        writeTypedDouble(content, stage.arrivalPos);
        writeTypedString(content, stage.description);
    }

    static inline void writeConstraint(tcpip::Storage& content, const libsumo::TraCISignalConstraint& c) {
        writeTypedString(content, c.signalId);
        writeTypedString(content, c.tripId);
        writeTypedString(content, c.foeId);
        writeTypedString(content, c.foeSignal);
        writeTypedInt(content, c.limit);
        writeTypedInt(content, c.type);
        writeTypedByte(content, c.mustWait);
        writeTypedByte(content, c.active);
        std::vector<std::string> paramItems;
        for (const auto& item : c.param) {
            paramItems.push_back(item.first);
            paramItems.push_back(item.second);
        }
        writeTypedStringList(content, paramItems);
    }

};


}

typedef libsumo::StorageHelper StoHelp;
