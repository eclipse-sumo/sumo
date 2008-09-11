/****************************************************************************/
/// @file    NBLoadedTLDef.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	»missingDescription«
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
 */
class NBLoadedTLDef : public NBTrafficLightDefinition
{
public:
class SignalGroup : public Named
    {
    public:
        SignalGroup(const std::string &id) throw();
        ~SignalGroup() throw();
        void addConnection(const NBConnection &c);
        void addPhaseBegin(SUMOTime time, TLColor color);
        void setYellowTimes(SUMOTime tRedYellowe, SUMOTime tYellow);
        DoubleVector getTimes(SUMOTime cycleDuration) const;
        void sortPhases();
        size_t getLinkNo() const;
        bool mayDrive(SUMOTime time) const;
        bool hasYellow(SUMOTime time) const;
//        bool mustBrake(SUMOReal time) const;
        bool containsConnection(NBEdge *from, NBEdge *to) const;
        bool containsIncoming(NBEdge *from) const;
        void remapIncoming(NBEdge *which, const EdgeVector &by);
        bool containsOutgoing(NBEdge *to) const;
        void remapOutgoing(NBEdge *which, const EdgeVector &by);
        const NBConnection &getConnection(size_t pos) const;
        void patchTYellow(SUMOTime tyellow);
//        void patchFalseGreenPhases(SUMOReal duration);
        void remap(NBEdge *removed, int removedLane,
                   NBEdge *by, int byLane);

        friend class phase_by_time_sorter;

    private:
        class PhaseDef
        {
        public:
            PhaseDef(SUMOTime time, TLColor color)
                    : myTime(time), myColor(color) { }
            PhaseDef(const PhaseDef &p)
                    : myTime(p.myTime), myColor(p.myColor) { }
            SUMOTime myTime;
            TLColor myColor;
        };

        class phase_by_time_sorter
        {
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
                : public Named
    {
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
    SignalGroup *findGroup(NBEdge *from, NBEdge *to) const;

    /** @brief Sets the duration of a cycle
        valid only for loaded traffic lights */
    void setCycleDuration(size_t cycleDur);

    /// Adds a signal group
    void addSignalGroup(const std::string &id);

    /** @brief Adds a signal to a signal group
        The signal is described by the connection it is placed at */
    bool addToSignalGroup(const std::string &groupid,
                          const NBConnection &connection);

    /** @brief Adds a list of signals to a signal group
        Each signal is described by the connection it is placed at */
    bool addToSignalGroup(const std::string &groupid,
                          const NBConnectionVector &connections);

    /** @brief Sets the information about the begin of a phase
        Valid for loaded traffic lights only */
    void addSignalGroupPhaseBegin(const std::string &groupid,
                                  SUMOTime time, TLColor color);

    /// Sets the times the light is yellow or red/yellow
    void setSignalYellowTimes(const std::string &groupid,
                              SUMOTime tRedYellowe, SUMOTime tYellow);

    void setTLControllingInformation(const NBEdgeCont &ec) const;

public:
    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing);

protected:
    /// Computes the traffic light logic
    NBTrafficLightLogicVector *myCompute(const NBEdgeCont &ec,
                                         size_t breakingTime, std::string type);

    /// Collects the nodes participating in this traffic light
    void collectNodes();

    void collectLinks();

    bool mustBrake(const NBEdgeCont &ec,
                   const NBConnection &possProhibited,
                   const std::bitset<64> &green, const std::bitset<64> &yellow,
                   size_t strmpos) const;

    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane);

private:
    struct Masks {
        std::bitset<64> driveMask;
        std::bitset<64> brakeMask;
        std::bitset<64> yellowMask;
    };

private:

    Masks buildPhaseMasks(const NBEdgeCont &ec, size_t time) const;

private:
    SignalGroupCont mySignalGroups;

    size_t myCycleDuration;

};


#endif

/****************************************************************************/

