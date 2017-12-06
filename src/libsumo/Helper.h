/****************************************************************************/
// Eclipse SUMO, Helper of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    Helper.h
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef Helper_h
#define Helper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Position;
class PositionVector;
class RGBColor;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Helper
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
    class Helper {
    public:
        /** @brief Connects to the specified SUMO server
        * @param[in] host The name of the host to connect to
        * @param[in] port The port to connect to
        * @exception tcpip::SocketException if the connection fails
        */
        //void connect(const std::string& host, int port);


        /// @brief ends the Helper and closes the connection
        void close();
        /// @}

        /// @brief load a Helper with the given arguments
        static void load(const std::vector<std::string>& args);

        /// @brief Advances by one step (or up to the given time)
        static void HelperStep(const SUMOTime time = 0);

        /// @brief {object->{variable->value}}
        typedef std::map<int, TraCIValue> TraCIValues;
        typedef std::map<std::string, TraCIValues> SubscribedValues;
        typedef std::map<std::string, SubscribedValues> SubscribedContextValues;

        //void subscribe(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars) const;
        //void subscribeContext(int domID, const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars) const;

        const SubscribedValues& getSubscriptionResults() const;
        const TraCIValues& getSubscriptionResults(const std::string& objID) const;

        const SubscribedContextValues& getContextSubscriptionResults() const;
        const SubscribedValues& getContextSubscriptionResults(const std::string& objID) const;

        /// @brief helper functions
        static TraCIPositionVector makeTraCIPositionVector(const PositionVector& positionVector);
        static TraCIPosition makeTraCIPosition(const Position& position);
        static Position makePosition(const TraCIPosition& position);

        static PositionVector makePositionVector(const TraCIPositionVector& vector);
        static TraCIColor makeTraCIColor(const RGBColor& color);
        static RGBColor makeRGBColor(const TraCIColor& color);

        static MSEdge* getEdge(const std::string& edgeID);
        static const MSLane* getLaneChecking(const std::string& edgeID, int laneIndex, double pos);
        static std::pair<MSLane*, double> convertCartesianToRoadMap(Position pos);

        static SUMOTime getCurrentTime();

        static SUMOTime getDeltaT();

        static TraCIBoundary getNetBoundary();

        static int getMinExpectedNumber();

        static TraCIStage findRoute(const std::string& from, const std::string& to, const std::string& typeID, const SUMOTime depart, const int routingMode);

        static std::vector<TraCIStage> findIntermodalRoute(const std::string& from, const std::string& to, const std::string& modes,
            const SUMOTime depart, const int routingMode, const double speed, const double walkFactor,
            const double departPos, const double arrivalPos, const double departPosLat,
            const std::string& pType, const std::string& vehType);

        static std::string getParameter(const std::string& objectID, const std::string& key);

    private:
        SubscribedValues mySubscribedValues;
        SubscribedContextValues mySubscribedContextValues;

        /// @brief invalidated standard constructor
        Helper();

        /// @brief invalidated copy constructor
        Helper(const Helper& src);

        /// @brief invalidated assignment operator
        Helper& operator=(const Helper& src);
    };
}


#endif

/****************************************************************************/
