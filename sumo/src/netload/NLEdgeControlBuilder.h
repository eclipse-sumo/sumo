#ifndef NLEdgeControlBuilder_h
#define NLEdgeControlBuilder_h
/***************************************************************************
                          NLEdgeControlBuilder.h
			  Holds the edges while they are build
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
// Revision 1.1  2002/10/16 15:36:48  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.4  2002/06/11 14:39:24  dkrajzew
// windows eol removed
//
// Revision 1.3  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:41  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:05  traffic
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
#include <microsim/MSEdge.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdgeControl;
class MSLane;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLEdgeControlBuilder
 * This class is the container for MSEdge-instances while they are build.
 * As instances of the MSEdge-class contain references to other instances of
 * this class which may not yet be known at their generation, they are
 * prebuild first and initialised with their correct values in a second step.
 *
 * While building an intialisation of the MSEdge, the value are stored in a
 * preallocated list to avoid memory farction. For the same reason, the list
 * of edges, later splitted into two lists, one containing single-lane-edges
 * and one containing multi-lane-edges, is preallocated to the size that was
 * previously computed by counting the edges in the first parser step.
 * As a result, the build MSEdgeControlBuilder is returned.
 * ToDo/Remarks:
 * The MSEdgeControlBuilder posesses an id. This is not set by the XML-file
 * and no interface for setting it is implemented yet.
 */
class NLEdgeControlBuilder {
public:
    /// definition of the used storage for edges
    typedef std::vector<MSEdge*> EdgeCont;
protected:
    /** storage for edges; to allow the splitting of edges after their number
        is known, they are hold inside this vector and laterly moved into two
        vectors, one for single-lane-edges and one for multi-lane-edges
        respectively */
    EdgeCont                  *m_pEdges;
    /// pointer to the currently chosen edge
    MSEdge                    *m_pActiveEdge;
    /// pointer to a temporary lane storage
    MSEdge::LaneCont          *m_pLaneStorage;
    /// list of the lanes that belong to the current edge
    MSEdge::LaneCont          *m_pLanes;
    /// pointer to the following edge the structure is currently working on
    MSEdge                    *m_pCurrentDestination;
    /// connection to following edges from the current edge
    MSEdge::AllowedLanesCont  *m_pAllowedLanes;
    /// pointer to the depart lane
    MSLane                    *m_pDepartLane;
    /// number of single-lane-edges
    unsigned int              m_iNoSingle;
    /// number of multi-lane-edges
    unsigned int              m_iNoMulti;
public:
    /** standard constructor; the parameter is a hint for the maximal number
        of lanes inside an edge */
    NLEdgeControlBuilder(unsigned int storageSize=10);
    /// standard destructor
    virtual ~NLEdgeControlBuilder();
    /** prepares the builder for the building of the specified number of
        edges (preallocates ressources) */
    void prepare(unsigned int no);
    /** adds an edge with the given id to the list of edges; this method
        throws an XMLIdAlreadyUsedException when the id was already used for
        another edge */
    virtual void addEdge(const std::string &id);
    /// chooses the previously added edge as the current edge
    void chooseEdge(const std::string &id);
    /** returns the pointer to the edge the builder is currently working on
        (needed?!!!) */
    MSEdge *getActiveReference();
    /** adds a lane to the current edge; this method throws an
        XMLDepartLaneDuplicationException when the lane is marked to be the
        depart lane and another so marked lane was added before */
    void addLane(MSLane *lane, bool isDepartLane);
    /// closes (ends) the addition of lanes to the current edge
    void closeLanes();
    /** begins the specification of lanes that may be used to reach the given
        edge from the current edge */
    void openAllowedEdge(MSEdge *edge);
    /** adds a lane that may be used to reach the edge previously specified by
        "openAllowedEdge"; this method throws an XMLInvalidChildException when
        the lane is not belonging to the current edge */
    void addAllowed(MSLane *lane);
    /// closes the specification of lanes that may be used to reach an edge
    void closeAllowedEdge();
    /** closes the building of an edge; the edge is completely described by
        now and may not be opened again what is not tested!!! */
    void closeEdge();

    /// builds the MSEdgeControl-class which holds all edges
    MSEdgeControl *build();
private:
    /** invalid copy constructor */
    NLEdgeControlBuilder(const NLEdgeControlBuilder &s);
    /** invalid assignment operator */
    NLEdgeControlBuilder &operator=(const NLEdgeControlBuilder &s);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLEdgeControlBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:





