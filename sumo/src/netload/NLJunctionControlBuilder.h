#ifndef NLJunctionControlBuilder_h
#define NLJunctionControlBuilder_h
/***************************************************************************
                          NLJunctionControlBuilder.h
              Container for MSJunctionControl-structures during
              their building
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.11  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/05/04 08:41:33  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2004/12/16 12:23:37  dkrajzew
// first steps towards a better parametrisation of traffic lights
//
// Revision 1.8  2004/08/02 12:47:30  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.7  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.6  2003/11/18 14:23:57  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.5  2003/07/07 08:35:10  dkrajzew
// changes due to loading of geometry applied from the gui-version
//  (no major drawbacks in loading speed)
//
// Revision 1.4  2003/03/20 16:35:44  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:06:33  dkrajzew
// new import format applied; new detectors applied
//
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:48  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload;
//  new format definition parseable in one step
//
// Revision 1.4  2002/06/11 14:39:25  dkrajzew
// windows eol removed
//
// Revision 1.3  2002/06/11 13:44:34  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new
//  netconverting methods debugged
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:44  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:09  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <microsim/MSJunction.h>
#include <microsim/MSRightOfWayJunction.h>
#include <microsim/MSJunctionControl.h>
#include <utils/geom/Position2D.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/traffic_lights/MSActuatedTrafficLightLogic.h>
#include <microsim/MSBitSetLogic.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEventControl;
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NLJunctionControlBuilder
 * NLJunctionControlBuilder is a factory for MSJunction-instances while
 * their building until they are transfered into a MSJunctionControl-instance
 * at last.
 * To avoid memory fraction, the list of the MSJunction-instances is
 * preallocated to the counted number of MSJunctions int he XML-file.
 * The result is a MSJunctionControl-instance holding the parsed MSJunction-
 * -instances.
 */
class NLJunctionControlBuilder {
private:
    /// Definition of a lane vector
    typedef std::vector<MSLane*> LaneVector;
    /// Definition of a string vector
    typedef std::vector<std::string> StringVector;
    /// Definitions of a connection map
    typedef std::map<std::string, StringVector> SSVMap;

public:
    /// standard constructor
    NLJunctionControlBuilder(OptionsCont &oc);

    /// Destructor
    virtual ~NLJunctionControlBuilder();

    /// preallocates space for the found number of junctions
    void prepare(unsigned int no);

    /// begins the processing of the named junction
    void openJunction(const std::string &id, const std::string &key,
        const std::string &type, double x, double y);

    /// Adds an incoming lane to the previously chosen junction
    void addIncomingLane(MSLane *lane);

    /// Adds an internal lane to the previously chosen junction
    void addInternalLane(MSLane *lane);

    /** @brief Closes (ends) the processing of the current junction;
        This method may throw a XMLIdAlreadyUsedException when a junction
        with the same id as the current was already added */
    void closeJunction();

    /** Builds the MSJunctionControl which holds all of the simulations
        junctions */
    MSJunctionControl *build() const;

    /** Returns the current inlane - container */
    const LaneVector &getIncomingLanes() const;

    /// clears the inlanes-container
    void initIncomingLanes();

    /// initialises a junction logic
    void initJunctionLogic();

    /// adds a logic item
    void addLogicItem(int request, const std::string &response,
        const std::string &foes);

    /// begins the reading of a traffic lights logic
    void initTrafficLightLogic(const std::string &type,
        size_t absDuration, int requestSize, int tlLogicNo,
        int detectorOffset);

    /// adds a phase to the traffic lights logic currently build
    void addPhase(size_t duration, const std::bitset<64> &phase,
        const std::bitset<64> &prios, const std::bitset<64> &yellow,
        int min, int max);

    /// Sets the size of the request
    void setRequestSize(int size);

    /// Sets the lane number the parsed logic will be responsible for
    void setLaneNumber(int size);

    /// Set the key of the logic
    void setKey(const std::string &key);

    /// Set the offset with which the logic shall start
    void setOffset(int val);

    /// Set the number of the logic
    void setTLLogicNo(int val);

    /// Returns a previously build logic
    MSTrafficLightLogic * const getTLLogic(const std::string &id) const;

    /// Returns the complete tls-logic control
    MSTLLogicControl *buildTLLogics() const;

    /// ends the building of a traffic lights logic
    virtual void closeTrafficLightLogic();

    /// ends the building of a junction logic (row-logic)
    void closeJunctionLogic();

    /// closes the building of the junction control
    void closeJunctions(NLDetectorBuilder &db, const SSVMap &continuations);


protected:
    /** @brief adds an information about the initialisation of a tls
        The initialisation is done during the closing of junctions */
    virtual void addJunctionInitInfo(MSExtendedTrafficLightLogic *key,
        const LaneVector &lv, double det_offset);

    /// Adds a build tls-logic
    virtual void addTLLogic(MSTrafficLightLogic *logic);

    /** builds a junction that does not use a logic */
    virtual MSJunction *buildNoLogicJunction();

    /** builds a junction with a logic */
    virtual MSJunction *buildLogicJunction();

    /** builds the junction logic catching occuring errors */
    MSJunctionLogic *getJunctionLogicSecure();

    /** builds the junction's list of lanes catching occuring errors */
    MSRightOfWayJunction::LaneCont getInLaneContSecure();

    /// Compute the initial step of a tls-logic from the given offset
    SUMOTime computeInitTLSStep() const;

    /// Compute the time offset the tls shall for the first time
    SUMOTime computeInitTLSEventOffset() const;


protected:
    /// The offset within the junction
    SUMOTime          m_Offset;

    /// the number of the current traffic light logic
    int             _tlLogicNo;

    /// The current logic type
    std::string     m_LogicType;

    /// the right-of-way-logic of the currently chosen bitset-logic
    MSBitsetLogic::Logic                    *m_pActiveLogic;

    /// the description about which in-junction lanes disallow other passing the junction
    MSBitsetLogic::Foes                     *m_pActiveFoes;

    /// the current phase definitions for a simple traffic light
    MSSimpleTrafficLightLogic::Phases   m_ActivePhases;

    /// the size of the request
    int             _requestSize;

    /// the number of lanes
    int             _laneNo;

    /// counter for the inserted items
    int             _requestItems;


    /// the list of the simulations junctions
    MSJunctionControl::JunctionCont          *m_pJunctions;

    /// the list of the incoming lanes of the currently chosen junction
    LaneVector                    m_pActiveIncomingLanes;

    /// the list of the internal lanes of the currently chosen junction
    LaneVector                    m_pActiveInternalLanes;

    /// the id of the currently chosen junction
    std::string                 m_CurrentId;

    /// the key of the currently chosen junction
    std::string                 m_Key;

    /** the key of the traffic loght logic when the current junction
        has traffic lights */
    std::string                 m_TLKey;

    /// the type of the currently chosen junction
    int                         m_Type;

    /// the position of the junction
    Position2D                  myPosition;

    /** the time delay to the next traffic light switch
        (when the current junction has traffic lights) */
    size_t                      m_Delay;

    /** the junction's traffic lights' first phase index
        (when the current junction has traffic lights) */
    size_t                      m_InitStep;

    /// The absolute duration of a tls-control loop
    size_t myAbsDuration;

    /// A map containing the build logics
    std::map<std::string, MSTrafficLightLogic*> myLogics;

    /// A definition of junction initialisation
    typedef std::pair<LaneVector, double> TLInitInfo;

    /// Definition of a map which contains information which junctions shall be initialised using which values
    typedef std::map<MSExtendedTrafficLightLogic*, TLInitInfo> TLLogicInitInfoMap;

    /// The map which contains information which junctions shall be initialised using which values
    TLLogicInitInfoMap myJunctions2PostLoadInit;

    /// Default detector offset
    double m_DetectorOffset;

    /// Default detector positions
    double myStdDetectorPositions;

    /// Default detector lengths (agentbased)
    double myStdDetectorLengths;

    /// Default learning horizon (agentbased)
    int myStdLearnHorizon;

    /// Default decision horizon (agentbased)
    int myStdDecisionHorizon;

    /// Default difference minimum (agentbased)
    double myStdDeltaLimit;

    /// The loaded default cycle time
    int myStdTCycle;

    // Default maximum gap  (actuated)
    double myStdActuatedMaxGap;

    // Default passing time (actuated)
    double myStdActuatedPassingTime;

    // Default maximum gap actuated)
    double myStdActuatedDetectorGap;

protected:
    /// numerical representation for a junction with no purpose
    static const int TYPE_NOJUNCTION;

    /** numerical representation for a junction where vehicles cominng
        from the right side may drive as first */
    static const int TYPE_RIGHT_BEFORE_LEFT;

    /** numerical representation of a junction where a street has a
        higher priority */
    static const int TYPE_PRIORITY_JUNCTION;

    /** a dead end (all roads end here) */
    static const int TYPE_DEAD_END;

private:
    /** invalid copy operator */
    NLJunctionControlBuilder(const NLJunctionControlBuilder &s);

    /** invalid assignment operator */
    NLJunctionControlBuilder &operator=(const NLJunctionControlBuilder &s);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
