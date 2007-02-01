/****************************************************************************/
/// @file    NGNet.h
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id: $
///
// -------------------
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NGNetElements.h"


class NBNetBuilder;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
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


    /// list of nodes
    TNodeList NodeList;

    /// list of links
    TLinkList LinkList;

private:
    /// Connects both noes with two edges, one for each direction
    void connect(TNode *node1, TNode *node2);

private:
    /// last ID given to node or link
    int myLastID;

    NBNetBuilder &myNetBuilder;

};


#endif

/****************************************************************************/

