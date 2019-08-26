/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Person.h
/// @author  Leonhard Luecken
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef Person_h
#define Person_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/VehicleType.h>
#include <libsumo/TraCIConstants.h>
#include <microsim/MSTransportable.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSPerson;
class PositionVector;
namespace libsumo {
class VariableWrapper;
}


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Person
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class Person {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static double getSpeed(const std::string& personID);
    static TraCIPosition getPosition(const std::string& personID, const bool includeZ = false);
    static TraCIPosition getPosition3D(const std::string& personID);
    static std::string getRoadID(const std::string& personID);
    static std::string getTypeID(const std::string& personID);
    static double getWaitingTime(const std::string& personID);
    static std::string getNextEdge(const std::string& personID);
    static std::string getVehicle(const std::string& personID);
    static int getRemainingStages(const std::string& personID);
    static TraCIStage getStage(const std::string& personID, int nextStageIndex = 0);
    static std::vector<std::string> getEdges(const std::string& personID, int nextStageIndex = 0);
    static std::string getParameter(const std::string& routeID, const std::string& param);
    static double getAngle(const std::string& personID);
    static double getSlope(const std::string& personID);
    static double getLanePosition(const std::string& personID);

    LIBSUMO_VEHICLE_TYPE_GETTER

    static void add(const std::string& personID, const std::string& edgeID, double pos, double depart = DEPARTFLAG_NOW, const std::string typeID = "DEFAULT_PEDTYPE");
    static void appendStage(const TraCIStage& stage, const std::string& personID);
    static void replaceStage(const std::string& personID, const int stageIndex, const TraCIStage& stage);
    static void appendWaitingStage(const std::string& personID, double duration, const std::string& description = "waiting", const std::string& stopID = "");
    static void appendWalkingStage(const std::string& personID, const std::vector<std::string>& edgeIDs, double arrivalPos, double duration = -1, double speed = -1, const std::string& stopID = "");
    static void appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID = "");
    static void removeStage(const std::string& personID, int nextStageIndex);
    static void rerouteTraveltime(const std::string& personID);
    static void moveTo(const std::string& personID, const std::string& edgeID, double position);
    static void moveToXY(const std::string& personID, const std::string& edgeID, const double x, const double y, double angle = INVALID_DOUBLE_VALUE, const int keepRoute = 1);
    static void setParameter(const std::string& personID, const std::string& key, const std::string& value);
    static void setSpeed(const std::string& personID, double speed);
    static void setType(const std::string& personID, const std::string& typeID);

    LIBSUMO_VEHICLE_TYPE_SETTER

    LIBSUMO_SUBSCRIPTION_API

    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the poi to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper);

private:
    static MSPerson* getPerson(const std::string& id);
    static MSTransportable::Stage* convertTraCIStage(const TraCIStage& stage, const std::string personID);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;

private:
    /// @brief invalidated standard constructor
    Person() = delete;

};


}


#endif

/****************************************************************************/
