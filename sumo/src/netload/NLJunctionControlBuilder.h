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
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:48  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.4  2002/06/11 14:39:25  dkrajzew
// windows eol removed
//
// Revision 1.3  2002/06/11 13:44:34  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
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
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <microsim/MSJunction.h>
#include <microsim/MSRightOfWayJunction.h>
#include <microsim/MSJunctionControl.h>
#include "NLContainer.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEventControl;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/**
 * NLJunctionControlBuilder is the container for MSJunction-instances while
 * their building until they are transfered into a MSJunctionControl-instance
 * at last.
 * To avoid memory fraction, the list of the MSJunction-instances is
 * preallocated to the counted number of MSJunctions int he XML-file.
 * The result is a MSJunctionControl-instance holding the parsed MSJunction-
 * -instances
 */
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NLJunctionControlBuilder {
public:
    /// standard constructor
    NLJunctionControlBuilder(NLContainer *container);

    /// Destructor
    ~NLJunctionControlBuilder();

    /// preallocates space for the found number of junctions
    void prepare(unsigned int no);

    /// begins the processing of the named junction
    void openJunction(const std::string &id, const std::string &key,
        const std::string &type, double x, double y);

    /// Adds an incoming lane to the previously chosen junction
    void addInLane(MSLane *lane);

    /** @brief Closes (ends) the processing of the current junction;
        This method may throw a XMLIdAlreadyUsedException when a junction
        with the same id as the current was already added */
    void closeJunction();

    /** Builds the MSJunctionControl which holds all of the simulations
        junctions */
    MSJunctionControl *build();

private:
    /** builds a junction that does not use a logic */
    MSJunction *buildNoLogicJunction();

    /** builds a junction with a logic */
    MSJunction *buildLogicJunction();

    /** builds a traffic light junction */
    MSJunction *buildTrafficLightJunction();

    /** builds the junction logic catching occuring errors */
    MSJunctionLogic *getJunctionLogicSecure();

    /** builds the junction's list of lanes catching occuring errors */
    MSRightOfWayJunction::InLaneCont getInLaneContSecure();

private:
    /// Definition of a lane vector
    typedef std::vector<MSLane*> LaneCont;

    /// the list of the simulations junctions
    MSJunctionControl::JunctionCont          *m_pJunctions;

    /// the list of the incoming lanes of the currently chosen junction
    LaneCont                    m_pActiveInLanes;

    /// the id of the currently chosen junction
    std::string                 m_CurrentId;

    /// the key of the currently chosen junction
    std::string                 m_Key;

    /** the key of the traffic loght logic when the current junction
        has traffic lights */
    std::string                 m_TLKey;

    /// the type of the currently chosen junction
    int                         m_Type;

    /// the container used
    NLContainer                 *m_Container;

    /// the position of the junction
    double                      m_X, m_Y;

    /** the time delay to the next traffic light switch
        (when the current junction has traffic lights) */
    size_t                      m_Delay;

    /** the junction's traffic lights' first phase index
        (when the current junction has traffic lights) */
    size_t                      m_InitStep;

private:
    /// numerical representation for a junction with no purpose
    static const int TYPE_NOJUNCTION;

    /// numerical representation for a traffic light-steered junction
    static const int TYPE_TRAFFIC_LIGHT;

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
//#ifndef DISABLE_INLINE
//#include "NLJunctionControlBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
