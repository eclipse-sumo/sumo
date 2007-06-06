/****************************************************************************/
/// @file    NGNet.h
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// The class storing the generated network
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
#ifndef NGNet_h
#define NGNet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NGNetElements.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TNGNet
 * The class storing the generated network
 *
 * An instance of this class stores both the edges and the nodes build during
 *  the generation of a network (using any type of generation algorithm).
 * These instances are later transformed into netbuild-structures using toNB()
 */
class TNGNet
{
public:
    /// Constructor
    TNGNet(NBNetBuilder &nb);

    /// Destructor
    ~TNGNet();

    /// Returns the node at the given position
    TNode* FindNode(int xID, int yID);

    /// returns next ID and increases myLastID
    std::string GetID();

    SUMOReal RadialToX(SUMOReal radius, SUMOReal phi);
    SUMOReal RadialToY(SUMOReal radius, SUMOReal phi);

    /// creates NGNetwork in chequerboard pattern
    void CreateChequerBoard(int NumX, int NumY, SUMOReal SpaceX, SUMOReal SpaceY);

    /** @brief creates NGNetwork in spiderweb pattern
        NumRadDiv >= 3
        NumCircles >= 1 */
    void CreateSpiderWeb(int NumRadDiv, int NumCircles, SUMOReal SpaceRad);

    /// Transfer to netbuild-structures
    void toNB() const;

    /// Adds a node to the network
    void add(TNode *node);

    /// Adds an edge to the network
    void add(TLink *edge);

    /// Returns the number of stored nodes
    size_t nodeNo() const;


private:
    /// Connects both noes with two edges, one for each direction
    void connect(TNode *node1, TNode *node2);

private:
    /// last ID given to node or link
    int myLastID;

    /// The builder used to build NB*-structures
    NBNetBuilder &myNetBuilder;

    /// list of nodes
    TNodeList myNodeList;

    /// list of links
    TLinkList myLinkList;

};


#endif

/****************************************************************************/

