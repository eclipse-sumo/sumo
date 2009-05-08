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

        bool containsConnection(NBEdge *from, NBEdge *to) const throw();
        bool containsIncoming(NBEdge *from) const throw();
        void remapIncoming(NBEdge *which, const EdgeVector &by) throw(ProcessError);
        bool containsOutgoing(NBEdge *to) const throw();
        void remapOutgoing(NBEdge *which, const EdgeVector &by) throw(ProcessError);
        const NBConnection &getConnection(unsigned int pos) const throw();
        void patchTYellow(SUMOTime tyellow) throw();
        void remap(NBEdge *removed, int removedLane,
                   NBEdge *by, int byLane) throw();

        friend class phase_by_time_sorter;

    private:
        class PhaseDef {
        public:
            PhaseDef(SUMOTime time, TLColor color) throw()
                    : myTime(time), myColor(color) { }
            PhaseDef(const PhaseDef &p) throw()
                    : myTime(p.myTime), myColor(p.myColor) { }
            SUMOTime myTime;
            TLColor myColor;
        };

        class phase_by_time_sorter {
        public:
            /// constructor
            explicit phase_by_time_sorter() { }

            int operator()(const PhaseDef &p1, const PhaseDef &p2) {
                return p1.myTime<p2.myTime;
            }
        };

        NBConnectionVector myConnections;
        typedef std::vector<PhaseDef> GroupsPhases;
        GroupsPhases myPhases;
        SUMOTime myTRedYellow, myTYellow;
    };

    class Phase
                : public Named {
    public:
        Phase(const std::string &id, SUMOTime begin, SUMOTime end) throw();
        ~Phase() throw();
    private:
        std::string mySignalGroup;
        SUMOTime myBegin, myEnd;
        typedef std::map<std::string, TLColor> SignalGroupColorMap;
        SignalGroupColorMap myGroupColors;
    };

    /// Definition of the container for signal groups
    typedef std::map<std::string, SignalGroup*> SignalGroupCont;

    /// Constructor
    NBLoadedTLDef(const std::string &id,
                  const std::set<NBNode*> &junctions) throw();

    /// Constructor
    NBLoadedTLDef(const std::string &id, NBNode *junction) throw();

    /// Constructor
    NBLoadedTLDef(const std::string &id) throw();

    /// Destructor
    ~NBLoadedTLDef() throw();

    /// Returns the signal group which is responsible for the given connection
    SignalGroup *findGroup(NBEdge *from, NBEdge *to) const throw();

    /** @brief Sets the duration of a cycle
        valid only for loaded traffic lights */
    void setCycleDuration(unsigned int cycleDur) throw();

    /// Adds a signal group
    void addSignalGroup(const std::string &id) throw();

    /** @brief Adds a signal to a signal group
        The signal is described by the connection it is placed at */
    bool addToSignalGroup(const std::string &groupid,
                          const NBConnection &connection) throw();

    /** @brief Adds a list of signals to a signal group
        Each signal is described by the connection it is placed at */
    bool addToSignalGroup(const std::string &groupid,
                          const NBConnectionVector &connections) throw();

    /** @brief Sets the information about the begin of a phase
        Valid for loaded traffic lights only */
    void addSignalGroupPhaseBegin(const std::string &groupid,
                                  SUMOTime time, TLColor color) throw();

    /// Sets the times the light is yellow or red/yellow
    void setSignalYellowTimes(const std::string &groupid,
                              SUMOTime tRedYellowe, SUMOTime tYellow) throw();

    void setTLControllingInformation(const NBEdgeCont &ec) const throw();

public:
    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing) throw();

protected:
    /// Computes the traffic light logic
    NBTrafficLightLogicVector *myCompute(const NBEdgeCont &ec,
                                         unsigned int breakingTime) throw();

    /// Collects the nodes participating in this traffic light
    void collectNodes() throw();

    void collectLinks() throw(ProcessError);

    bool mustBrake(const NBEdgeCont &ec,
                   const NBConnection &possProhibited,
                   const std::bitset<64> &green, const std::bitset<64> &yellow,
                   unsigned int strmpos) const throw();

    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane) throw();

private:
    struct Masks {
        std::bitset<64> driveMask;
        std::bitset<64> brakeMask;
        std::bitset<64> yellowMask;
    };

private:

    Masks buildPhaseMasks(const NBEdgeCont &ec, unsigned int time) const throw();

private:
    SignalGroupCont mySignalGroups;

    size_t myCycleDuration;

};


#endif

/****************************************************************************/

