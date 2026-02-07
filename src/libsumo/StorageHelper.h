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

    static inline std::vector<double> readTypedDoubleList(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_DOUBLELIST && error != "") {
            throw TraCIException(error);
        }
        return ret.readDoubleList();
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

    static inline const libsumo::TraCIColor readTypedColor(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_COLOR && error != "") {
            throw TraCIException(error);
        }
        libsumo::TraCIColor into;
        into.r = static_cast<unsigned char>(ret.readUnsignedByte());
        into.g = static_cast<unsigned char>(ret.readUnsignedByte());
        into.b = static_cast<unsigned char>(ret.readUnsignedByte());
        into.a = static_cast<unsigned char>(ret.readUnsignedByte());
        return into;
    }

    static inline const libsumo::TraCIPosition readTypedPosition2D(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::POSITION_2D && error != "") {
            throw TraCIException(error);
        }
        libsumo::TraCIPosition p;
        p.x = ret.readDouble();
        p.y = ret.readDouble();
        return p;
    }

    static inline const libsumo::TraCIPositionVector readTypedPolygon(tcpip::Storage& ret, const std::string& error = "") {
        if (ret.readUnsignedByte() != libsumo::TYPE_POLYGON && error != "") {
            throw TraCIException(error);
        }
        libsumo::TraCIPositionVector poly;
        readPolygon(ret, poly);
        return poly;
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

    static inline void readPolygon(tcpip::Storage& ret, libsumo::TraCIPositionVector& poly) {
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
        const int n = readTypedInt(inputStorage, error);
        for (int i = 0; i < n; ++i) {
            libsumo::TraCIVehicleData vd;
            vd.id = readTypedString(inputStorage, error);
            vd.length = readTypedDouble(inputStorage, error);
            vd.entryTime = readTypedDouble(inputStorage, error);
            vd.leaveTime = readTypedDouble(inputStorage, error);
            vd.typeID = readTypedString(inputStorage, error);
            result.emplace_back(vd);
        }
    }

    static inline void readReservation(tcpip::Storage& inputStorage, libsumo::TraCIReservation& result, const std::string& error = "") {
        readCompound(inputStorage, 10, error);
        result.id = readTypedString(inputStorage, error);
        result.persons = readTypedStringList(inputStorage, error);
        result.group = readTypedString(inputStorage, error);
        result.fromEdge = readTypedString(inputStorage, error);
        result.toEdge = readTypedString(inputStorage, error);
        result.departPos = readTypedDouble(inputStorage, error);
        result.arrivalPos = readTypedDouble(inputStorage, error);
        result.depart = readTypedDouble(inputStorage, error);
        result.reservationTime = readTypedDouble(inputStorage, error);
        result.state = readTypedInt(inputStorage, error);
    }

    static inline void readLogic(tcpip::Storage& inputStorage, libsumo::TraCILogic& logic, const std::string& error = "") {
        readCompound(inputStorage, 5, error);
        logic.programID = readTypedString(inputStorage);
        logic.type = readTypedInt(inputStorage);
        logic.currentPhaseIndex = readTypedInt(inputStorage);
        int numPhases = readCompound(inputStorage);
        while (numPhases-- > 0) {
            readCompound(inputStorage, 6);
            libsumo::TraCIPhase* phase = new libsumo::TraCIPhase();
            phase->duration = readTypedDouble(inputStorage);
            phase->state = readTypedString(inputStorage);
            phase->minDur = readTypedDouble(inputStorage);
            phase->maxDur = readTypedDouble(inputStorage);
            int numNext = readCompound(inputStorage);
            while (numNext-- > 0) {
                phase->next.push_back(readTypedInt(inputStorage));
            }
            phase->name = readTypedString(inputStorage);
            logic.phases.emplace_back(phase);
        }
        int numParams = readCompound(inputStorage);
        while (numParams-- > 0) {
            const std::vector<std::string> key_value = readTypedStringList(inputStorage);
            logic.subParameter[key_value[0]] = key_value[1];
        }
    }

    static inline void readConstraintVector(tcpip::Storage& inputStorage, std::vector<libsumo::TraCISignalConstraint>& result, const std::string& error = "") {
        const int n = readTypedInt(inputStorage, error);
        for (int i = 0; i < n; ++i) {
            libsumo::TraCISignalConstraint c;
            c.signalId = readTypedString(inputStorage);
            c.tripId = readTypedString(inputStorage);
            c.foeId = readTypedString(inputStorage);
            c.foeSignal = readTypedString(inputStorage);
            c.limit = readTypedInt(inputStorage);
            c.type = readTypedInt(inputStorage);
            c.mustWait = readTypedByte(inputStorage) != 0;
            c.active = readTypedByte(inputStorage) != 0;
            const std::vector<std::string> paramItems = readTypedStringList(inputStorage);
            for (int j = 0; j < (int)paramItems.size(); j += 2) {
                c.param[paramItems[j]] = paramItems[j + 1];
            }
            result.emplace_back(c);
        }
    }

    static inline void readLinkVectorVector(tcpip::Storage& inputStorage, std::vector< std::vector<libsumo::TraCILink> >& result, const std::string& error = "") {
        const int n = readTypedInt(inputStorage, error);
        for (int i = 0; i < n; ++i) {
            std::vector<libsumo::TraCILink> controlledLinks;
            int numLinks = readTypedInt(inputStorage);
            while (numLinks-- > 0) {
                std::vector<std::string> link = readTypedStringList(inputStorage);
                controlledLinks.emplace_back(link[0], link[2], link[1]);
            }
            result.emplace_back(controlledLinks);
        }
    }


    static inline void readBestLanesVector(tcpip::Storage& inputStorage, std::vector<libsumo::TraCIBestLanesData>& result, const std::string& error = "") {
        const int n = readTypedInt(inputStorage, error);
        for (int i = 0; i < n; ++i) {
            libsumo::TraCIBestLanesData info;
            info.laneID = readTypedString(inputStorage);
            info.length = readTypedDouble(inputStorage);
            info.occupation = readTypedDouble(inputStorage);
            info.bestLaneOffset = readTypedUnsignedByte(inputStorage);
            info.allowsContinuation = readBool(inputStorage);
            info.continuationLanes = readTypedStringList(inputStorage, error);
            result.emplace_back(info);
        }
    }

    static inline void readCollisionVector(tcpip::Storage& inputStorage, std::vector<libsumo::TraCICollision>& result, const std::string& error = "") {
        int numCollisions = readTypedInt(inputStorage, error);
        while (numCollisions-- > 0) {
            libsumo::TraCICollision c;
            c.collider = readTypedString(inputStorage);
            c.victim = readTypedString(inputStorage);
            c.colliderType = readTypedString(inputStorage);
            c.victimType = readTypedString(inputStorage);
            c.colliderSpeed = readTypedDouble(inputStorage);
            c.victimSpeed = readTypedDouble(inputStorage);
            c.type = readTypedString(inputStorage);
            c.lane = readTypedString(inputStorage);
            c.pos = readTypedDouble(inputStorage);
            result.emplace_back(c);
        }
    }

    static inline void readJunctionFoeVector(tcpip::Storage& inputStorage, std::vector<libsumo::TraCIJunctionFoe>& result, const std::string& error = "") {
        const int n = readTypedInt(inputStorage, error);
        for (int i = 0; i < n; ++i) {
            libsumo::TraCIJunctionFoe info;
            info.foeId = readTypedString(inputStorage);
            info.egoDist = readTypedDouble(inputStorage);
            info.foeDist = readTypedDouble(inputStorage);
            info.egoExitDist = readTypedDouble(inputStorage);
            info.foeExitDist = readTypedDouble(inputStorage);
            info.egoLane = readTypedString(inputStorage);
            info.foeLane = readTypedString(inputStorage);
            info.egoResponse = readBool(inputStorage);
            info.foeResponse = readBool(inputStorage);
            result.emplace_back(info);
        }
    }

    static inline void readStopVector(tcpip::Storage& inputStorage, std::vector<libsumo::TraCINextStopData>& result, const std::string& error = "") {
        const int n = readTypedInt(inputStorage, error);
        for (int i = 0; i < n; ++i) {
            libsumo::TraCINextStopData s;
            s.lane = readTypedString(inputStorage);
            s.endPos = readTypedDouble(inputStorage);
            s.stoppingPlaceID = readTypedString(inputStorage);
            s.stopFlags = readTypedInt(inputStorage);
            s.duration = readTypedDouble(inputStorage);
            s.until = readTypedDouble(inputStorage);
            s.startPos = readTypedDouble(inputStorage);
            s.intendedArrival = readTypedDouble(inputStorage);
            s.arrival = readTypedDouble(inputStorage);
            s.depart = readTypedDouble(inputStorage);
            s.split = readTypedString(inputStorage);
            s.join = readTypedString(inputStorage);
            s.actType = readTypedString(inputStorage);
            s.tripId = readTypedString(inputStorage);
            s.line = readTypedString(inputStorage);
            s.speed = readTypedDouble(inputStorage);
            result.emplace_back(s);
        }
    }

    static inline void readTLSDataVector(tcpip::Storage& inputStorage, std::vector<libsumo::TraCINextTLSData>& result, const std::string& error = "") {
        const int n = readTypedInt(inputStorage, error);
        for (int i = 0; i < n; ++i) {
            libsumo::TraCINextTLSData tls;
            tls.id = readTypedString(inputStorage);
            tls.tlIndex = readTypedInt(inputStorage);
            tls.dist = readTypedDouble(inputStorage);
            tls.state = (char)readTypedByte(inputStorage);
            result.emplace_back(tls);
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
