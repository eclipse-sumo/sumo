/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_FCDReplay.h
/// @author  Michael Behrisch
/// @date    01.03.2024
///
// A device which replays recorded floating car data
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/Command.h>
#include <utils/common/MapMatcher.h>
#include <utils/xml/SUMOSAXHandler.h>
#include "MSVehicleDevice.h"


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOSAXReader;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_FCDReplay
 * @brief A device which replays a vehicle trajectory from an fcd file
 *
 * @see MSDevice
 */
class MSDevice_FCDReplay : public MSVehicleDevice {
public:
    /** @brief Inserts MSDevice_FCDReplay-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a FCDReplay-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into);

    /** @brief Static intialization
     */
    static void init();
    static SUMOTime parseNext(SUMOTime t);

public:
    /// @brief Destructor.
    ~MSDevice_FCDReplay();

    void move(SUMOTime currentTime);

    /// @brief return the name for this type of device
    const std::string deviceName() const override {
        return "fcd-replay";
    }

    struct TrajectoryEntry {
        SUMOTime time;
        Position pos;
        std::string edgeOrLane;
        double lanePos;
        double speed;
        double angle;
    };

    typedef std::vector<TrajectoryEntry> Trajectory;

    void setTrajectory(Trajectory* const t) {
        myTrajectory = t;
        myTrajectoryIndex = 1;
    }

private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_FCDReplay(SUMOVehicle& holder, const std::string& id);

    class MoveVehicles : public Command {
    public:
        SUMOTime execute(SUMOTime currentTime) override;
    private:
        /// @brief Invalidated assignment operator.
        MoveVehicles& operator=(const MoveVehicles&) = delete;
    };

    class FCDHandler : public SUMOSAXHandler, public MapMatcher<MSEdge, MSLane, MSJunction> {
    public:
        FCDHandler(const std::string& file);
        SUMOTime getTime() const {
            return myTime;
        }
        void updateTrafficObjects(const SUMOTime intervalStart);

    protected:
        /// @name inherited from GenericSAXHandler
        //@{

        /** @brief Called on the opening of a tag
         *
         * @param[in] element ID of the currently opened element
         * @param[in] attrs Attributes within the currently opened element
         * @exception ProcessError If something fails
         * @see GenericSAXHandler::myStartElement
         */
        void myStartElement(int element, const SUMOSAXAttributes& attrs) override;
        //@}

        void initLaneTree(NamedRTree* tree) override;

        MSEdge* retrieveEdge(const std::string& id) override;

    private:
        struct StageStart {
            std::string vehicle;
            int trajectoryOffset;
            int routeOffset;
        };

        MSTransportable::MSTransportablePlan* makePlan(const SUMOVehicleParameter& params, const ConstMSEdgeVector& route,
                const std::vector<StageStart>& stages, const Trajectory& t);
        ConstMSEdgeVector checkRoute(const ConstMSEdgeVector& edges, const SUMOVehicle* const vehicle);

        SUMOTime myTime = 0;
        std::map<std::string, Trajectory> myTrajectories;
        std::map<std::string, std::tuple<SUMOTime, std::string, bool, ConstMSEdgeVector, std::vector<StageStart> > > myRoutes;
        std::map<const Position, std::string> myPositions;
    };

private:
    static FCDHandler* myHandler;
    static SUMOSAXReader* myParser;
    Trajectory* myTrajectory = nullptr;
    int myTrajectoryIndex = 0;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_FCDReplay(const MSDevice_FCDReplay&) = delete;

    /// @brief Invalidated assignment operator.
    MSDevice_FCDReplay& operator=(const MSDevice_FCDReplay&) = delete;

};
