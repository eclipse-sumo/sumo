/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2021 German Aerospace Center (DLR) and others.
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
/// @file    Person.h
/// @author  Leonhard Luecken
/// @date    15.09.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/VehicleType.h>
#ifndef LIBTRACI
#include <microsim/transportables/MSTransportable.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class MSPerson;
struct Reservation;
class PositionVector;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Person
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class Person {
public:
    static double getSpeed(const std::string& personID);
    static libsumo::TraCIPosition getPosition(const std::string& personID, const bool includeZ = false);
    static libsumo::TraCIPosition getPosition3D(const std::string& personID);
    static std::string getRoadID(const std::string& personID);
    static std::string getLaneID(const std::string& personID);
    static std::string getTypeID(const std::string& personID);
    static double getWaitingTime(const std::string& personID);
    static std::string getNextEdge(const std::string& personID);
    static std::string getVehicle(const std::string& personID);
    static int getRemainingStages(const std::string& personID);
    static libsumo::TraCIStage getStage(const std::string& personID, int nextStageIndex = 0);
    static std::vector<std::string> getEdges(const std::string& personID, int nextStageIndex = 0);
    static double getAngle(const std::string& personID);
    static double getSlope(const std::string& personID);
    static double getLanePosition(const std::string& personID);

    static std::vector<libsumo::TraCIReservation> getTaxiReservations(int stateFilter = 0);
    static std::string splitTaxiReservation(std::string reservationID, const std::vector<std::string>& personIDs);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_VEHICLE_TYPE_GETTER

    static void add(const std::string& personID, const std::string& edgeID, double pos, double depart = libsumo::DEPARTFLAG_NOW, const std::string typeID = "DEFAULT_PEDTYPE");
    static void appendStage(const std::string& personID, const libsumo::TraCIStage& stage);
    static void replaceStage(const std::string& personID, const int stageIndex, const libsumo::TraCIStage& stage);
    static void appendWaitingStage(const std::string& personID, double duration, const std::string& description = "waiting", const std::string& stopID = "");
    static void appendWalkingStage(const std::string& personID, const std::vector<std::string>& edges, double arrivalPos, double duration = -1, double speed = -1, const std::string& stopID = "");
    static void appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID = "");
    static void removeStage(const std::string& personID, int nextStageIndex);
    static void rerouteTraveltime(const std::string& personID);
    static void moveTo(const std::string& personID, const std::string& edgeID, double position);
    static void moveToXY(const std::string& personID, const std::string& edgeID, const double x, const double y, double angle = libsumo::INVALID_DOUBLE_VALUE, const int keepRoute = 1);
    static void setSpeed(const std::string& personID, double speed);
    static void setType(const std::string& personID, const std::string& typeID);

    LIBSUMO_VEHICLE_TYPE_SETTER

    LIBSUMO_SUBSCRIPTION_API

#ifdef LIBTRACI
    static void writeStage(const libsumo::TraCIStage& stage, tcpip::Storage& content);
#else
    /** @brief Saves the shape of the requested object in the given container
     *  @param id The id of the poi to retrieve
     *  @param shape The container to fill
     */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static MSPerson* getPerson(const std::string& id);
    static MSStage* convertTraCIStage(const TraCIStage& stage, const std::string personID);
    static bool filterReservation(int stateFilter, const Reservation* res, std::vector<libsumo::TraCIReservation>& reservations);

    /// @brief clase for CW Sorter
    class reservation_by_id_sorter {
    public:
        /// @brief constructor
        reservation_by_id_sorter() {};

        /// @brief comparing operation for sort
        int operator()(const TraCIReservation& r1, const TraCIReservation& r2) const;
    };

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif

private:
    /// @brief invalidated standard constructor
    Person() = delete;

};


}
