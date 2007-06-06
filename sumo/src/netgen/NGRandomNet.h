/****************************************************************************/
/// @file    NGRandomNet.h
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// Additional structures for building random nets
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
#ifndef NGRandomNet_h
#define NGRandomNet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NGNet.h"
#include <map>


// ===========================================================================
// class definitions
// ===========================================================================
// ------------------- TNeighbourList ----------------------------------------
typedef std::map<int, SUMOReal> TNeighbourList;

/**
 * @class TNeighbourDistribution
 */
class TNeighbourDistribution
{
public:
    TNeighbourList Neighbours;

    /**
     * @brief adds a NeighbourItem to list - if NumNeighbours is already existing,
     *
     * the old ratio is overwritten */
    void Add(int NumNeighbours, SUMOReal ratio);

    /// get random number of neighbours
    int Num();

};


/**
 * @class TNGRandomNet
 */
class TNGRandomNet
{
public:
    // constructor,
    TNGRandomNet(TNGNet *Net);
    // properties,
    SUMOReal GetMinLinkAngle()
    {
        return myMinLinkAngle;
    }
    void SetMinLinkAngle(SUMOReal value)
    {
        myMinLinkAngle = value;
    }
    SUMOReal GetMinDistance()
    {
        return myMinDistance;
    }
    void SetMinDistance(SUMOReal value)
    {
        myMinDistance = value;
    }
    SUMOReal GetMaxDistance()
    {
        return myMaxDistance;
    }
    void SetMaxDistance(SUMOReal value)
    {
        myMaxDistance = value;
    }
    SUMOReal GetConnectivity()
    {
        return myConnectivity;
    }
    void SetConnectivity(SUMOReal value)
    {
        myConnectivity = value;
    }
    int GetNumTries()
    {
        return myNumTries;
    }
    void SetNumTries(int num)
    {
        myNumTries = num;
    }
    // sets the distrubtion of number of neighbours
    TNeighbourDistribution NeighbourDistribution;
    //
    void CreateNet(int NumNodes);
private:
    // lists of all nodes and links
    TNGNet *myNet;
    // lists of outer nodes and links
    TNodeList OuterNodes;
    TLinkList OuterLinks;
    void RemoveOuterNode(TNode *Node);
    void RemoveOuterLink(TLink *Link);
    // list of possible new connections
    TNodeList ConNodes;
    // restrictions
    SUMOReal myMinLinkAngle;  // minimum angle allowed between two links
    SUMOReal myMinDistance;   // minimum distance allowed between two nodes
    SUMOReal myMaxDistance;   // maxmium distance allowed between two nodes
    SUMOReal myConnectivity;  // probability of connecting to a existing node if possible
    //
    int myNumTries; // number of tries to create a new node
    int myNumNodes; // number of nodes to be created
    // checks whether a possible connection between these two nodes
    // complies with the restrictions above
    bool CheckAngles(TNode* Node);
    bool CanConnect(TNode* BaseNode, TNode* NewNode);
    bool CreateNewNode(TNode *BaseNode); // creates new random node and returns true, if creation was successfull
    // finds possible connections between Node and OuterNodes
    // complying with restrictions
    void FindPossibleOuterNodes(TNode *Node);

};


#endif

/****************************************************************************/

