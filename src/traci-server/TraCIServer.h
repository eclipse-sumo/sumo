/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServer.h
/// @author  Axel Wegener
/// @author  Friedemann Wesner
/// @author  Christoph Sommer
/// @author  Tino Morenz
/// @author  Daniel Krajzewicz
/// @author  Thimor Bohn
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    2007/10/24
/// @version $Id$
///
// TraCI server used to control sumo by a remote TraCI client
/****************************************************************************/
#ifndef TRACISERVER_H
#define TRACISERVER_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <set>

#define BUILD_TCPIP
#include <foreign/tcpip/socket.h>
#include <foreign/tcpip/storage.h>
#include <utils/common/NamedRTree.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <libsumo/TraCIDefs.h>
#include "TraCIConstants.h"
#include "TraCIServerAPI_Lane.h"


// ===========================================================================
// class definitions
// ===========================================================================
/** @class TraCIServer
 * @brief TraCI server used to control sumo by a remote TraCI client
 */
class TraCIServer : public MSNet::VehicleStateListener {
public:
    /// @brief Definition of a method to be called for serving an associated commandID
    typedef bool(*CmdExecutor)(TraCIServer& server, tcpip::Storage& inputStorage, tcpip::Storage& outputStorage);


    SUMOTime getTargetTime() const {
        return myTargetTime;
    }
    bool isEmbedded() const {
        return myAmEmbedded;
    }
    static TraCIServer* getInstance() {
        return myInstance;
    }

    /// @name Initialisation and Shutdown
    /// @{

    /** @brief Initialises the server
     * @param[in] execs The (additional) command executors to use
     */
    static void openSocket(const std::map<int, CmdExecutor>& execs);


    /// @brief request termination of connection
    static void close();


    /** @brief check whether close was requested
     * @return Whether the connection was closed
     */
    static bool wasClosed();
    /// @}


    /// @brief process all commands until the next SUMO simulation step.
    ///        It is guaranteed that t->getTargetTime() >= myStep after call
    ///        (except the case that a load or close command is received)s
    void processCommandsUntilSimStep(SUMOTime step);

    /// @brief clean up subscriptions
    void cleanup();


#ifdef HAVE_PYTHON
    /// @brief process the command
    static std::string execute(std::string cmd);

    /// @brief run the given script
    static void runEmbedded(std::string pyFile);
#endif

    void vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to);

    /// @name Writing Status Messages
    /// @{

    /** @brief Writes a status command to the given storage
     * @param[in] commandId The id of the command to respond to
     * @param[in] status The status to send
     * @param[in] description The status description (error message, for example)
     * @param[in, filled] outputStorage The storage to write the status into
     */
    void writeStatusCmd(int commandId, int status, const std::string& description, tcpip::Storage& outputStorage);


    /** @brief Writes a status command to myOutputStorage
     * @param[in] commandId The id of the command to respond to
     * @param[in] status The status to send
     * @param[in] description The status description (error message, for example)
     */
    void writeStatusCmd(int commandId, int status, const std::string& description);


    /** @brief Writes a status command to the given storage with status = RTYPE_ERR
     * @param[in] commandId The id of the command to respond to
     * @param[in] description The status description (error message, for example)
     * @param[in, filled] outputStorage The storage to write the status into
     */
    bool writeErrorStatusCmd(int commandId, const std::string& description, tcpip::Storage& outputStorage);
    /// @}



    const std::map<MSNet::VehicleState, std::vector<std::string> >& getVehicleStateChanges() const {
        if (myAmEmbedded) {
            return myVehicleStateChanges;
        } else if (myCurrentSocket == mySockets.end()) {
            // Requested in context of a subscription update
            return myVehicleStateChanges;
        } else {
            // Requested in the context of a custom query by active client
            return myCurrentSocket->second->vehicleStateChanges;
        }
    }

    void writeResponseWithLength(tcpip::Storage& outputStorage, tcpip::Storage& tempMsg);

    void collectObjectsInRange(int domain, const PositionVector& shape, double range, std::set<std::string>& into);


    /// @name Helpers for reading and checking values
    /// @{

    /** @brief Reads the value type and an int, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether an integer value was given (by data type)
     */
    bool readTypeCheckingInt(tcpip::Storage& inputStorage, int& into);


    /** @brief Reads the value type and a double, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether a double value was given (by data type)
     */
    bool readTypeCheckingDouble(tcpip::Storage& inputStorage, double& into);


    /** @brief Reads the value type and a string, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether a string value was given (by data type)
     */
    bool readTypeCheckingString(tcpip::Storage& inputStorage, std::string& into);


    /** @brief Reads the value type and a string list, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether a double value was given (by data type)
     */
    bool readTypeCheckingStringList(tcpip::Storage& inputStorage, std::vector<std::string>& into);


    /** @brief Reads the value type and a color, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether a color was given (by data type)
     */
    bool readTypeCheckingColor(tcpip::Storage& inputStorage, libsumo::TraCIColor& into);


    /** @brief Reads the value type and a 2D position, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether a 2D position was given (by data type)
     */
    bool readTypeCheckingPosition2D(tcpip::Storage& inputStorage, libsumo::TraCIPosition& into);


    /** @brief Reads the value type and a 2D bounding box, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether a 2D bounding box was given (by data type)
     */
    bool readTypeCheckingBoundary(tcpip::Storage& inputStorage, Boundary& into);


    /** @brief Reads the value type and a byte, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether a byte was given (by data type)
     */
    bool readTypeCheckingByte(tcpip::Storage& inputStorage, int& into);


    /** @brief Reads the value type and an unsigned byte, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether an unsigned byte was given (by data type)
     */
    bool readTypeCheckingUnsignedByte(tcpip::Storage& inputStorage, int& into);


    /** @brief Reads the value type and a polygon, verifying the type
     *
     * @param[in, changed] inputStorage The storage to read from
     * @param[out] into Holder of the read value
     * @return Whether an unsigned byte was given (by data type)
     */
    bool readTypeCheckingPolygon(tcpip::Storage& inputStorage, PositionVector& into);
    /// @}


    /// @brief Sets myTargetTime on server and sockets to the given value
    /// @note  Used in MSStateHandler to update the server's time after loading a state
    void setTargetTime(SUMOTime targetTime);

    std::vector<std::string>& getLoadArgs() {
        return myLoadArgs;
    }


private:
    /** @brief Constructor
     * @param[in] port The port to listen to (to open)
     */
    TraCIServer(const SUMOTime begin, const int port, const int numClients);


    /// @brief Destructor
    virtual ~TraCIServer();



    struct SocketInfo {
    public:
        /// @brief constructor
        SocketInfo(tcpip::Socket* socket, SUMOTime t)
            : targetTime(t), socket(socket) {}
        /// @brief destructor
        ~SocketInfo() {
            delete socket;
        }
        /// @brief Target time: next point of action for the client
        SUMOTime targetTime;
        /// @brief Socket object for this client
        tcpip::Socket* socket;
        /// @brief container for vehicle state changes since last step taken by this client
        std::map<MSNet::VehicleState, std::vector<std::string> > vehicleStateChanges;
    private:
        SocketInfo(const SocketInfo&);
    };

    /// @name Server-internal command handling
    /// @{

    /** @brief Returns the TraCI-version
     * @return Always true
     */
    bool commandGetVersion();


    /** @brief Handles subscriptions to send after a simstep2 command
     */
    void postProcessSimulationStep();
    /// @}


    /// @brief Reads the next command ID from the input storage
    /// @return the command ID
    /// @param[out] the version with reference parameters provides information on the command start position and length used in dispatchCommand for checking purposes
    int readCommandID(int& commandStart, int& commandLength);

    /// @brief Handles command, writes response to myOutputStorage
    int dispatchCommand();

    /// @brief Called once after connection of all clients for executing SET_ORDER (and possibly prior GET_VERSION) commands,
    ///        that should be executed before simulation starts (in processCommandsUntilNextSimStep()).
    void checkClientOrdering();

    /// @brief checks for and processes reordering requests (relevant for multiple clients)
    void processReorderingRequests();

    /// @brief get the minimal next target time among all clients
    SUMOTime nextTargetTime() const;

    /// @brief send out subscription results (actually just the content of myOutputStorage) to clients which will act in this step (i.e. with client target time <= myTargetTime)
    void sendOutputToAll() const;

    /// @brief sends an empty response to a simstep command to the current client. (This applies to a situation where the TraCI step frequency is higher than the SUMO step frequency)
    void sendSingleSimStepResponse();

    /// @brief removes myCurrentSocket from mySockets and returns an iterator pointing to the next member according to the ordering
    std::map<int, SocketInfo*>::iterator removeCurrentSocket();


private:
    /// @brief Singleton instance of the server
    static TraCIServer* myInstance;

    /// @brief Whether the connection was set to be to close
    static bool myDoCloseConnection;

    /// @brief The server socket
    tcpip::Socket* myServerSocket;

    /// @brief The socket connections to the clients
    /// the first component (index) determines the client's order (lowest index's commands are processed first), @see CMD_SETORDER
    std::map<int, SocketInfo*> mySockets;

    /// @brief This stores the setOrder(int) requests of the clients.
    std::map<int, SocketInfo*> mySocketReorderRequests;

    /// @brief The currently active client socket
    std::map<int, SocketInfo*>::iterator myCurrentSocket;

    /// @brief The time step to reach until processing the next commands
    SUMOTime myTargetTime;

    /// @brief The storage to read from
    tcpip::Storage myInputStorage;

    /// @brief The storage to writeto
    tcpip::Storage myOutputStorage;

    /// @brief The last timestep's subscription results
    tcpip::Storage mySubscriptionCache;

    /// @brief Whether the server runs in embedded mode
    const bool myAmEmbedded;

    /// @brief Map of commandIds -> their executors; applicable if the executor applies to the method footprint
    std::map<int, CmdExecutor> myExecutors;

    /// @brief Map of variable ids to the size of the parameter in bytes
    std::map<int, int> myParameterSizes;

    std::vector<std::string> myLoadArgs;

    /** @class Subscription
     * @brief Representation of a subscription
     */
    class Subscription {
    public:
        /** @brief Constructor
         * @param[in] commandIdArg The command id of the subscription
         * @param[in] idArg The id of the object that is subscribed
         * @param[in] variablesArg The subscribed variables
         * @param[in] beginTimeArg The begin time of the subscription
         * @param[in] endTimeArg The end time of the subscription
         * @param[in] contextVarsArg Whether the subscription is a context subscription (variable subscription otherwise)
         * @param[in] contextDomainArg The domain ID of the context
         * @param[in] rangeArg The range of the context
         */
        Subscription(int commandIdArg, const std::string& idArg,
                     const std::vector<int>& variablesArg, const std::vector<std::vector<unsigned char> >& paramsArg,
                     SUMOTime beginTimeArg, SUMOTime endTimeArg, bool contextVarsArg, int contextDomainArg, double rangeArg)
            : commandId(commandIdArg), id(idArg), variables(variablesArg), parameters(paramsArg), beginTime(beginTimeArg), endTime(endTimeArg),
              contextVars(contextVarsArg), contextDomain(contextDomainArg), range(rangeArg) {}

        /// @brief commandIdArg The command id of the subscription
        int commandId;
        /// @brief The id of the object that is subscribed
        std::string id;
        /// @brief The subscribed variables
        std::vector<int> variables;
        /// @brief The parameters for the subscribed variables
        std::vector<std::vector<unsigned char> > parameters;
        /// @brief The begin time of the subscription
        SUMOTime beginTime;
        /// @brief The end time of the subscription
        SUMOTime endTime;
        /// @brief Whether the subscription is a context subscription (variable subscription otherwise)
        bool contextVars;
        /// @brief The domain ID of the context
        int contextDomain;
        /// @brief The range of the context
        double range;

    };

    /// @brief The list of known, still valid subscriptions
    std::vector<Subscription> mySubscriptions;

    /// @brief Changes in the states of simulated vehicles
    /// @note
    /// Server cache myVehicleStateChanges is used for managing last steps subscription updates
    /// and for client information in case that myAmEmbedded==true, which implies a single client.
    /// For the potential multiclient case (myAmEmbedded==false), each socket in mySockets is associated
    /// with a proper vehicleStateChanges container mySockets[...].second->vehicleStateChanges
    /// Performance could be improved if for a single client, myVehicleStateChanges is used only.
    std::map<MSNet::VehicleState, std::vector<std::string> > myVehicleStateChanges;

private:
    bool addObjectVariableSubscription(const int commandId, const bool hasContext);
    void initialiseSubscription(const Subscription& s);
    void removeSubscription(int commandId, const std::string& identity, int domain);
    bool processSingleSubscription(const TraCIServer::Subscription& s, tcpip::Storage& writeInto,
                                   std::string& errors);


    bool findObjectShape(int domain, const std::string& id, PositionVector& shape);


private:
    /// @brief Invalidated assignment operator
    TraCIServer& operator=(const TraCIServer& s);

};


#endif
