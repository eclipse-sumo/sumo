#ifndef NBTrafficLightDefinition_h
#define NBTrafficLightDefinition_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightDefinition.h -
//  The definition of a traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.2  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.1  2003/06/05 11:43:20  dkrajzew
// definition class for traffic lights added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <utils/common/Named.h>
#include <utils/common/DoubleVector.h>
#include "NBCont.h"
#include "NBConnection.h"
#include "NBConnectionDefs.h"
#include "NBLinkPossibilityMatrix.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;
class OptionsCont;
class NBTrafficLightLogicVector;
class NBTrafficLightPhases;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBTrafficLightDefinition
    : public Named {

public:
    /** possible types of removeing a link from regardation during the
        building of the traffic light logic */
    enum LinkRemovalType {
        /// all links will be regarded
        LRT_NO_REMOVAL,
        /** all left-movers which are together with other direction on the same
            lane will be removed */
        LRT_REMOVE_WHEN_NOT_OWN,
        /// remove all left-movers
        LRT_REMOVE_ALL_LEFT
    };

    enum TLColor {
        TLCOLOR_UNKNOWN,
        TLCOLOR_RED,
        TLCOLOR_YELLOW,
        TLCOLOR_REDYELLOW,
        TLCOLOR_GREEN,
        TLCOLOR_BLINK,
        TLCOLOR_BLUE // :-)
    };


    class SignalGroup
        : public Named {
    public:
        SignalGroup(const std::string &id);
        ~SignalGroup();
        void addConnection(const NBConnection &c);
        void addPhaseBegin(double time, TLColor color);
        void setYellowTimes(double tRedYellowe, double tYellow);
        DoubleVector getTimes() const;
        void sortPhases();
        size_t getLinkNo() const;
        bool mayDrive(double time) const;
        bool mustBrake(double time) const;
        bool containsConnection(NBEdge *from, NBEdge *to) const;
        bool containsIncoming(NBEdge *from) const;
        void remapIncoming(NBEdge *which, const EdgeVector &by);
        bool containsOutgoing(NBEdge *to) const;
        void remapOutgoing(NBEdge *which, const EdgeVector &by);
        const NBConnection &getConnection(size_t pos) const;

        friend class phase_by_time_sorter;

    private:
        class PhaseDef {
        public:
            PhaseDef(double time, TLColor color)
                : myTime(time), myColor(color) { }
            PhaseDef(const PhaseDef &p)
                : myTime(p.myTime), myColor(p.myColor) { }
            double myTime;
            TLColor myColor;
        };

        class phase_by_time_sorter {
        public:
            /// constructor
            explicit phase_by_time_sorter() { }

            int operator() (const PhaseDef &p1, const PhaseDef &p2) {
                return p1.myTime<p2.myTime;
            }
        };

        NBConnectionVector myConnections;
        typedef std::vector<PhaseDef> GroupsPhases;
        GroupsPhases myPhases;
        double myTRedYellow, myTYellow;
    };

    class Phase
        : public Named {
    public:
        Phase(const std::string &id, size_t begin, size_t end);
        ~Phase();
/*        void addSignalGroupColor(const std::string &signalgroup,
            TLColor color);*/
    private:
        std::string mySignalGroup;
        int myBegin, myEnd;
        typedef std::map<std::string, TLColor> SignalGroupColorMap;
        SignalGroupColorMap _groupColors;
    };

    /// Definition of the container for signal groups
    typedef std::map<std::string, SignalGroup*> SignalGroupCont;
    NBTrafficLightDefinition(const std::string &id, const std::vector<NBNode*> &junctions);
    NBTrafficLightDefinition(const std::string &id, NBNode *junction);
    NBTrafficLightDefinition(const std::string &id);
    ~NBTrafficLightDefinition();
    NBTrafficLightLogicVector *compute(OptionsCont &oc);
    void setCycleDuration(size_t cycleDur);
    void addSignalGroup(const std::string &id);
    bool addToSignalGroup(const std::string &groupid,
        const NBConnection &connection);
    bool addToSignalGroup(const std::string &groupid,
        const NBConnectionVector &connections);
    void addSignalGroupPhaseBegin(const std::string &groupid,
        double time, TLColor color);
    void setSignalYellowTimes(const std::string &groupid,
        double tRedYellowe, double tYellow);
    void addNode(NBNode *node);
    SignalGroup *findGroup(NBEdge *from, NBEdge *to) const;

    /** @brief builds the traffic light logics
        Returns the number of possible logics for this tl */
/*    int buildTrafficLight(const std::string &key,
        const SignalGroupCont &defs, size_t cycleTime,
        size_t breakingTime, bool buildAll) const;
*/

	bool mustBrake(NBEdge *from, NBEdge *to) const;

    /** returns the information whether the connections from1->to1 and
        from2->to2 are foes */
    bool forbidden(NBEdge *from1, NBEdge *to1,
        NBEdge *from2, NBEdge *to2) const;

    void remapRemoved(NBEdge *removed,
        const EdgeVector &incoming, const EdgeVector &outgoing);

private:
    NBTrafficLightLogicVector *buildLoadedTrafficLights();

    NBTrafficLightLogicVector *buildOwnTrafficLights(
        size_t breakingTime, bool buildAll) const;


    /** compute the traffic light logics for the current node and the
        given settings */
    NBTrafficLightLogicVector *computeTrafficLightLogics(
        const std::string &key,
        bool joinLaneLinks, bool removeTurnArounds, LinkRemovalType removal,
        bool appendSmallestOnly, bool skipLarger,
        size_t breakingTime) const;

    /** compute the pases for the current node and the given settings */
    NBTrafficLightPhases * computePhases(bool joinLaneLinks,
        bool removeTurnArounds, LinkRemovalType removal,
        bool appendSmallestOnly, bool skipLarger) const;

    /** build the matrix of links that may be used simultaneously */
    NBLinkPossibilityMatrix *getPossibilityMatrix(bool joinLanes,
        bool removeTurnArounds, LinkRemovalType removalType) const;

/*    NBTrafficLightLogic *buildTrafficLightsLogic(const std::string &key,
        size_t noLinks, const PhaseIndexVector &phaseList,
        NBLinkCliqueContainer &cliquen,
        const NBRequestEdgeLinkIterator &cei1) const;*/

    std::pair<std::bitset<64>, std::bitset<64> >
        buildPhaseMasks(size_t time) const;

    size_t computeBrakingTime(double minDecel) const;


    void collectEdges();

    void collectLinks();

    std::pair<size_t, size_t> getSizes() const;

    /// returns the information whether the given link is a left-mover
    bool isLeftMover(NBEdge *from, NBEdge *to) const;


    friend class NBRequestEdgeLinkIterator;

private:
    SignalGroupCont mySignalGroups;

    size_t myCycleDuration;

    /// Definition of the container type for participating nodes
    typedef std::vector<NBNode*> NodeCont;

    /// The container with participating nodes
    NodeCont _nodes;

    ///
    EdgeVector _incoming;

    EdgeVector _within;

//    EdgeVector _outgoing;

    NBConnectionVector _links;

};




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBTrafficLightDefinition.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

