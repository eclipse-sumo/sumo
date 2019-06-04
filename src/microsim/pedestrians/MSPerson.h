/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSPerson.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The class for modelling person-movements
/****************************************************************************/
#ifndef MSPerson_h
#define MSPerson_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <set>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <microsim/MSTransportable.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSEdge;
class MSLane;
class OutputDevice;
class SUMOVehicleParameter;
class MSStoppingPlace;
class SUMOVehicle;
class MSVehicleType;
class MSPModel;
class PedestrianState;
class DummyState;

typedef std::vector<const MSEdge*> ConstMSEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSPerson
  *
  * The class holds a simulated person together with her movement stages
  */
class MSPerson : public MSTransportable {
public:
    /**
     * A "real" stage performing the walking to an edge
     * The walking does not need any route as it is not simulated.
     * Only the duration is needed
     */
    class MSPersonStage_Walking : public MSTransportable::Stage {
    public:
        /// constructor
        MSPersonStage_Walking(const std::string& personID, const ConstMSEdgeVector& route, MSStoppingPlace* toStop, SUMOTime walkingTime,
                              double speed, double departPos, double arrivalPos, double departPosLat);

        /// destructor
        ~MSPersonStage_Walking();

        Stage* clone() const;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* person, SUMOTime now, Stage* previous);

        /// abort this stage (TraCI)
        void abort(MSTransportable*);

        /// sets the walking speed (ignored in other stages)
        void setSpeed(double speed);

        /// Returns the current edge
        const MSEdge* getEdge() const;
        const MSEdge* getFromEdge() const;
        double getEdgePos(SUMOTime now) const;

        ///
        Position getPosition(SUMOTime now) const;

        double getAngle(SUMOTime now) const;

        /// @brief get travel distance in this stage
        double getDistance() const {
            return walkDistance();
        }

        SUMOTime getWaitingTime(SUMOTime now) const;

        double getSpeed() const;

        /// @brief the edges of the current stage
        ConstMSEdgeVector getEdges() const;

        std::string getStageDescription() const {
            return "walking";
        }
        std::string getStageSummary() const;

        /** @brief Called on writing tripinfo output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const;

        /** @brief Called on writing vehroute output
         * @param[in] os The stream to write the information into
         * @param[in] withRouteLength whether route length shall be written
         * @exception IOError not yet implemented
         */
        virtual void routeOutput(OutputDevice& os, const bool withRouteLength) const;

        /** @brief Called for writing the events output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void beginEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSTransportable& p, SUMOTime t, OutputDevice& os) const;

        /// @brief move forward and return whether the person arrived
        bool moveToNextEdge(MSPerson* person, SUMOTime currentTime, MSEdge* nextInternal = nullptr);

        /// @brief place person on a previously passed edge
        void setRouteIndex(MSPerson* person, int routeOffset);

        /// @brief accessors to be used by MSPModel
        //@{
        double getMaxSpeed(const MSTransportable* const person) const;

        inline double getDepartPos() const {
            return myDepartPos;
        }

        inline double getDepartPosLat() const {
            return myDepartPosLat;
        }

        inline double getArrivalPos() const {
            return myArrivalPos;
        }

        inline const std::vector<const MSEdge*>::iterator getRouteStep() const {
            return myRouteStep;
        }

        inline const MSEdge* getRouteEdge() const {
            return *myRouteStep;
        }
        inline const MSEdge* getNextRouteEdge() const {
            return myRouteStep == myRoute.end() - 1 ? 0 : *(myRouteStep + 1);
        }
        inline const ConstMSEdgeVector& getRoute() const {
            return myRoute;
        }

        PedestrianState* getPedestrianState() const {
            return myPedestrianState;
        }
        //@}


    private:

        /// @brief compute total walking distance
        double walkDistance() const;

        /* @brief compute average speed if the total walking duration is given
         * @note Must be called when the previous stage changes myDepartPos from the default*/
        double computeAverageSpeed() const;


    private:
        /// the time the person is walking
        SUMOTime myWalkingTime;

        /// the time the person entered the edge
        SUMOTime myLastEdgeEntryTime;

        /// @brief The route of the person
        ConstMSEdgeVector myRoute;


        ConstMSEdgeVector::iterator myRouteStep;

        /// @brief The current internal edge this person is on or 0
        MSEdge* myCurrentInternalEdge;

        double myDepartPos;
        double myDepartPosLat;
        double mySpeed;

        /// @brief state that is to be manipulated by MSPModel
        PedestrianState* myPedestrianState;

        class arrival_finder {
        public:
            /// constructor
            explicit arrival_finder(SUMOTime time) : myTime(time) {}

            /// comparison operator
            bool operator()(double t) const {
                return myTime > t;
            }

        private:
            /// the searched arrival time
            SUMOTime myTime;
        };

    private:
        /// @brief Invalidated copy constructor.
        MSPersonStage_Walking(const MSPersonStage_Walking&);

        /// @brief Invalidated assignment operator.
        MSPersonStage_Walking& operator=(const MSPersonStage_Walking&);

    };

    /**
    * A "real" stage performing the travelling by a transport system
    * The given route will be chosen. The travel time is computed by the simulation
    */
    class MSPersonStage_Driving : public MSTransportable::Stage_Driving {
    public:
        /// constructor
        MSPersonStage_Driving(const MSEdge* destination, MSStoppingPlace* toStop,
                              const double arrivalPos, const std::vector<std::string>& lines,
                              const std::string& intendedVeh = "", SUMOTime intendedDepart = -1);

        /// destructor
        ~MSPersonStage_Driving();

        Stage* clone() const;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* person, SUMOTime now, Stage* previous);

        /// @brief returns the stage description as a string
        std::string getStageDescription() const;
        std::string getStageSummary() const;

        /** @brief Called on writing tripinfo output
        *
        * @param[in] os The stream to write the information into
        * @param[in] transportable The person to write information about
        * @exception IOError not yet implemented
        */
        virtual void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const;

        /** @brief Called on writing vehroute output
         *
         * @param[in] os The stream to write the information into
         * @param[in] withRouteLength whether route length shall be written
         * @exception IOError not yet implemented
         */
        virtual void routeOutput(OutputDevice& os, const bool withRouteLength) const;
    };

    /**
     * An intermediate stage performing the access from or to public transport as given
     * by the access elements of the public transport stop. The travel time is computed by the simulation
     */
    class MSPersonStage_Access : public MSTransportable::Stage {
    public:
        /// constructor
        MSPersonStage_Access(const MSEdge* destination, MSStoppingPlace* toStop,
                             const double arrivalPos, const double dist, const bool isExit);

        /// destructor
        ~MSPersonStage_Access();

        Stage* clone() const;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* person, SUMOTime now, Stage* previous);

        /// @brief returns the stage description as a string
        std::string getStageDescription() const;
        std::string getStageSummary() const;

        Position getPosition(SUMOTime now) const;

        double getAngle(SUMOTime now) const;

        /// @brief get travel distance in this stage
        double getDistance() const {
            return myDist;
        }

        /** @brief Called on writing tripinfo output
        *
        * @param[in] os The stream to write the information into
        * @param[in] transportable The person to write information about
        * @exception IOError not yet implemented
        */
        void tripInfoOutput(OutputDevice& os, const MSTransportable* const transportable) const;

        /// @brief Called on writing vehroute output. Currently does nothing.
        void routeOutput(OutputDevice&, const bool) const {};

        /// @brief Called on writing events output (begin of an action). Currently does nothing.
        void beginEventOutput(const MSTransportable&, SUMOTime, OutputDevice&) const {};

        /// @brief Called on writing events output (end of an action). Currently does nothing.
        void endEventOutput(const MSTransportable&, SUMOTime, OutputDevice&) const {};

    private:
        class ProceedCmd : public Command {
        public:
            ProceedCmd(MSTransportable* person, MSEdge* edge) : myPerson(person), myStopEdge(edge) {}
            ~ProceedCmd() {}
            SUMOTime execute(SUMOTime currentTime);
        private:
            MSTransportable* const myPerson;
            MSEdge* myStopEdge;
        private:
            /// @brief Invalidated assignment operator.
            ProceedCmd& operator=(const ProceedCmd&);
        };

    private:
        const double myDist;
        const bool myAmExit;
        SUMOTime myEstimatedArrival;
        PositionVector myPath;
    };

public:
    /// constructor
    MSPerson(const SUMOVehicleParameter* pars, MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan, const double speedFactor);

    /// destructor
    virtual ~MSPerson();

    /* @brief proceeds to the next step of the route,
     * @return Whether the persons plan continues  */
    bool proceed(MSNet* net, SUMOTime time);

    /// @brief return the list of internal edges if this person is walking and the pedestrian model allows it
    const std::string& getNextEdge() const;

    /// @brief returns the next edge ptr if this person is walking and the pedestrian model allows it
    const MSEdge* getNextEdgePtr() const;

    /** @brief Called on writing tripinfo output
    *
    * @param[in] os The stream to write the information into
    * @exception IOError not yet implemented
    */
    virtual void tripInfoOutput(OutputDevice& os) const;

    /** @brief Called on writing vehroute output
    *
    * @param[in] os The stream to write the information into
    * @exception IOError not yet implemented
    */
    virtual void routeOutput(OutputDevice& os, const bool withRouteLength) const;

    /// @brief whether this person is selected in the GUI
    virtual bool isSelected() const {
        return false;
    }

    inline double getSpeedFactor() const {
        return myChosenSpeedFactor;
    }

    /// @brief set new walk and replace the stages with relative indices in the interval [firstIndex, nextIndex[
    void reroute(ConstMSEdgeVector& newEdges, double departPos, int firstIndex, int nextIndex);


    /** @class Influencer
     * @brief Changes the wished person speed and position
     *
     * The class is used for passing velocities or positions obtained via TraCI to the person.
     */
    class Influencer {
    public:
        /// @brief Constructor
        Influencer();


        /// @brief Destructor
        ~Influencer();


        void setRemoteControlled(Position xyPos, MSLane* l, double pos, double posLat, double angle, int edgeOffset, const ConstMSEdgeVector& route, SUMOTime t);

        SUMOTime getLastAccessTimeStep() const {
            return myLastRemoteAccess;
        }

        void postProcessRemoteControl(MSPerson* p);

        bool isRemoteControlled() const;

        bool isRemoteAffected(SUMOTime t) const;

    private:
        Position myRemoteXYPos;
        MSLane* myRemoteLane;
        double myRemotePos;
        double myRemotePosLat;
        double myRemoteAngle;
        int myRemoteEdgeOffset;
        ConstMSEdgeVector myRemoteRoute;
        SUMOTime myLastRemoteAccess;
    };


    /** @brief Returns the velocity/lane influencer
     *
     * If no influencer was existing before, one is built, first
     * @return Reference to this vehicle's speed influencer
     */
    Influencer& getInfluencer();

    const Influencer* getInfluencer() const;

    bool hasInfluencer() const {
        return myInfluencer != 0;
    }

    /// @brief sets position outside the road network
    void setRemoteState(Position xyPos);

private:
    /// @brief An instance of a speed/position influencing instance; built in "getInfluencer"
    Influencer* myInfluencer;

    const double myChosenSpeedFactor;

    static DummyState myDummyState;

private:
    /// @brief Invalidated copy constructor.
    MSPerson(const MSPerson&);

    /// @brief Invalidated assignment operator.
    MSPerson& operator=(const MSPerson&);

};


#endif

/****************************************************************************/
