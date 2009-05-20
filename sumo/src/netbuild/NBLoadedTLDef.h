/****************************************************************************/
/// @file    NBLoadedTLDef.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// A loaded (complete) traffic light logic
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBLoadedTLDef_h
#define NBLoadedTLDef_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <set>
#include "NBTrafficLightDefinition.h"
#include "NBNode.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBLoadedTLDef
 * @brief A loaded (complete) traffic light logic
 */
class NBLoadedTLDef : public NBTrafficLightDefinition {
public:
    /** @class SignalGroup
     * @brief A single signal group, may control several connections
     */
    class SignalGroup : public Named {
    public:
        /** @brief Constructor
         * @param[in] id The id of the signal group
         */
        SignalGroup(const std::string &id) throw();

        /// @brief Destructor
        ~SignalGroup() throw();

        /** @brief Inserts a controlled connection
         * @param[in] c The connection to be controlled by this signal group
         */
        void addConnection(const NBConnection &c) throw();

        /** @brief Sets the begin of a phase
         * @param[in] time The time at which the phase starts
         * @param[in] color The color of this signal starting at the given time
         */
        void addPhaseBegin(SUMOTime time, TLColor color) throw();

        /** @brief Sets the times for redyellow and yellow
         * @param[in] tRedYellowe The duration of the redyellow phase
         * @param[in] tYellow The duration of the yellow phase
         */
        void setYellowTimes(SUMOTime tRedYellowe, SUMOTime tYellow) throw();

        /** @brief Returns the times at which the signal switches
         * @param[in] cycleDuration The duration of the complete cycle
         * @return The switch times of this signal
         */
        DoubleVector getTimes(SUMOTime cycleDuration) const throw();

        /** @brief Sorts the phases */
        void sortPhases() throw();

        /** @brief Returns the number of links (connection) controlled by this signal
         * @return The number of links controlled by this signal
         */
        unsigned int getLinkNo() const throw();

        /** @brief Returns whether vehicles on controlled links may drive at the given time
         * @param[in] time The regarded time
         * @return Whether vehicles may drive at this time
         */
        bool mayDrive(SUMOTime time) const throw();

        /** @brief Returns whether controlled links have yellow at the given time
         * @param[in] time The regarded time
         * @return Whether controlled links are yellow at this time
         */
        bool hasYellow(SUMOTime time) const throw();

        /** @brief Returns whether the given connection is controlled by this signal
         * @param[in] from The connection's start edge
         * @param[in] from The connection's end edge
         * @return Whether the connection is controlled by this signal
         */
        bool containsConnection(NBEdge *from, NBEdge *to) const throw();

        /** @brief Returns whether this signal controls the given edge
         * @param[in] from The incoming edge
         * @return Whether this edge is controlled by this signal
         */
        bool containsIncoming(NBEdge *from) const throw();

        /** @brief Replaces the given incoming edge by the others given
         * @param[in] which The edge to replace
         * @param[in] by The replacements
         */
        void remapIncoming(NBEdge *which, const EdgeVector &by) throw(ProcessError);

        /** @brief Returns whether this signal controls a connection where the given edge is the destination
         * @param[in] from The outgoing edge
         * @return Whether this edge's predecessing edge is controlled by this signal
         */
        bool containsOutgoing(NBEdge *to) const throw();

        /** @brief Replaces the given outgoing edge by the others given
         * @param[in] which The edge to replace
         * @param[in] by The replacements
         */
        void remapOutgoing(NBEdge *which, const EdgeVector &by) throw(ProcessError);
        
        /** @brief Returns the connection at the given index
         * @param[in] pos The position within this signal
         * @return The connection at the given index
         */
        const NBConnection &getConnection(unsigned int pos) const throw();

        /** @brief Sets the yellow time
         * @param[in] tyellow The yellow time to set
         */
        void patchTYellow(SUMOTime tyellow) throw();

        /** @brief Replaces a removed edge/lane
         * @param[in] removed The edge to replace
         * @param[in] removedLane The lane of this edge to replace
         * @param[in] by The edge to insert instead
         * @param[in] byLane This edge's lane to insert instead
         */
        void remap(NBEdge *removed, int removedLane, NBEdge *by, int byLane) throw();

        /** @class PhaseDef
         * @brief Definition of a single, loaded phase
         */
        class PhaseDef {
        public:
            /** @brief Constructor
             * @param[in] time The begin time of this phase
             * @param[in] color A signal's color from this time
             */
            PhaseDef(SUMOTime time, TLColor color) throw()
                    : myTime(time), myColor(color) { }

            /// @brief The begin time of this phase
            SUMOTime myTime;
            /// @brief A signal's color from this time
            TLColor myColor;
        };

        /** @class phase_by_time_sorter
         * @brief Sorts phases by their begin time
         */
        class phase_by_time_sorter {
        public:
            /// @brief Constructor
            explicit phase_by_time_sorter() { }

            /** @brief Sorts phases by their begin time
             * @param[in] p1 a phase definition
             * @param[in] p2 a phase definition
             */
            int operator()(const PhaseDef &p1, const PhaseDef &p2) {
                return p1.myTime<p2.myTime;
            }
        };

    private:
        /// @brief Connections controlled by this signal
        NBConnectionVector myConnections;
        /// @brief The phases of this signal
        std::vector<PhaseDef> myPhases;
        /// @brief The times of redyellow and yellow
        SUMOTime myTRedYellow, myTYellow;
    };



    /// @brief Definition of the container for signal groups
    typedef std::map<std::string, SignalGroup*> SignalGroupCont;


    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] junctions Junctions controlled by this tls
     */
    NBLoadedTLDef(const std::string &id,
                  const std::set<NBNode*> &junctions) throw();


    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] junction The junction controlled by this tls
     */
    NBLoadedTLDef(const std::string &id, NBNode *junction) throw();


    /** @brief Constructor
     * @param[in] id The id of the tls
     */
    NBLoadedTLDef(const std::string &id) throw();


    /// @brief Destructor
    ~NBLoadedTLDef() throw();


    /** @brief Returns the signal group which is responsible for the given connection
     * @param[in] from The connection's start edge
     * @param[in] to The connection's end edge
     * @return The signal group which controls the given connection
     */
    SignalGroup *findGroup(NBEdge *from, NBEdge *to) const throw();


    /** @brief Sets the duration of a cycle 
     * @param[in] cycleDur The duration of the cycle
     */
    void setCycleDuration(unsigned int cycleDur) throw();


    /** @brief Adds a signal group
     * @param[in] id The id of the signal group
     */
    void addSignalGroup(const std::string &id) throw();


    /** @brief Adds a connection to a signal group
     * @param[in] groupid The id of the signal group to add the connection to
     * @param[in] connection The connection to add
     */
    bool addToSignalGroup(const std::string &groupid,
                          const NBConnection &connection) throw();


    /** @brief Adds a list of connections to a signal group
     * @param[in] groupid The id of the signal group to add the connections to
     * @param[in] connections The connections to add
     */
    bool addToSignalGroup(const std::string &groupid,
                          const NBConnectionVector &connections) throw();


    /** @brief Sets the information about the begin of a phase
     * @param[in] groupid The id of the signal group to add the phase to
     * @param[in] time The time the phase starts at
     * @param[in] color The color of the signal during this phase
     */
    void addSignalGroupPhaseBegin(const std::string &groupid,
                                  SUMOTime time, TLColor color) throw();


    /** @brief Sets the times the light is yellow or red/yellow
     * @param[in] groupid The id of the signal group to add the phase to
     * @param[in] tRedYellow The duration of redyellow
     * @param[in] tYellow The duration of yellow
     */
    void setSignalYellowTimes(const std::string &groupid,
                              SUMOTime tRedYellow, SUMOTime tYellow) throw();


    /// @name Public methods from NBTrafficLightDefinition-interface
    /// @{

    /** @brief Informs edges about being controlled by a tls
     * @param[in] ec The container of edges
     * @see NBTrafficLightDefinition::setTLControllingInformation
     */
    void setTLControllingInformation(const NBEdgeCont &ec) const throw();


    /** @brief Replaces occurences of the removed edge in incoming/outgoing edges of all definitions
     * @param[in] removed The removed edge
     * @param[in] incoming The edges to use instead if an incoming edge was removed
     * @param[in] outgoing The edges to use instead if an outgoing edge was removed
     * @see NBTrafficLightDefinition::remapRemoved
     */
    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing) throw();
    /// @}


protected:
    /// @name Protected methods from NBTrafficLightDefinition-interface
    /// @{

    /** @brief Computes the traffic light logic finally in dependence to the type
     * @param[in] ec The edge container
     * @param[in] brakingTime Duration a vehicle needs for braking in front of the tls
     * @return The computed logic
     * @see NBTrafficLightDefinition::myCompute
     */
    NBTrafficLightLogic *myCompute(const NBEdgeCont &ec,
                                         unsigned int brakingTime) throw();


    /** @brief Collects the nodes participating in this traffic light
     * @see NBTrafficLightDefinition::collectNodes
     */
    void collectNodes() throw();


    /** @brief Collects the links participating in this traffic light
     * @exception ProcessError If a link could not be found
     * @see NBTrafficLightDefinition::collectLinks
     */
    void collectLinks() throw(ProcessError);


    /** @brief Returns the information whether a connection must brake, given a phase
     * @param[in] ec The edge control to retrieve edges from
     * @param[in] possProhibited The connection to investigate
     * @param[in] state The state
     * @param[in] strmpos The index of this connection within the masks
     * @return Whether the given connection must brake
     */
    bool mustBrake(const NBEdgeCont &ec,
                   const NBConnection &possProhibited,
                   const std::string &state,
                   unsigned int strmpos) const throw();


    /** @brief Replaces a removed edge/lane
     * @param[in] removed The edge to replace
     * @param[in] removedLane The lane of this edge to replace
     * @param[in] by The edge to insert instead
     * @param[in] byLane This edge's lane to insert instead
     * @see NBTrafficLightDefinition::replaceRemoved
     */
    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane) throw();
    /// @}

private:
    /** @brief Builds the phase for a given time
     * @param[in] ec The edge control to use
     * @param[in] time The time to build the phase for
     * @return The phase of this tls for the given time
     */
    std::string buildPhaseState(const NBEdgeCont &ec, unsigned int time) const throw();


private:
    /// @brief Controlled signal groups
    SignalGroupCont mySignalGroups;

    /// @brief The duration of a single cycle
    unsigned int myCycleDuration;


};


#endif

/****************************************************************************/

