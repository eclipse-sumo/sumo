#ifndef NGRandomNet_h
#define NGRandomNet_h
/***************************************************************************
                          NGRandomNet.h
                             -------------------
    project              : SUMO
    begin                : Mar, 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
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
// Revision 1.5  2005/04/27 11:48:51  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.4  2004/11/23 10:22:03  dkrajzew
// debugging
//
// Revision 1.3  2003/07/21 11:05:31  dkrajzew
// patched some bugs found in first real-life execution
//
// Revision 1.2  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.1  2003/07/16 15:33:08  dkrajzew
// files needed to generate networks added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NGNet.h"
#include <map>


/* =========================================================================
 * class definitions
 * ======================================================================= */
// ------------------- TNeighbourList ----------------------------------------
typedef std::map<int, float> TNeighbourList;

// ------------------- TNeighbourDistribution --------------------------------
class TNeighbourDistribution
{
public:
	TNeighbourList Neighbours;
	// adds a NeighbourItem to list - if NumNeighbours is already existing,
	// the old ratio is overwritten
	void Add(int NumNeighbours, float ratio);
	// get random number of neighbours
	int Num();
};

// ------------------- TNGRandomNet- -------------------------------------------
class TNGRandomNet
{
public:
	// constructor,
	TNGRandomNet(TNGNet *Net);
	// properties,
	float GetMinLinkAngle() {return myMinLinkAngle;}
	void SetMinLinkAngle(float value) {myMinLinkAngle = value;}
	float GetMinDistance() {return myMinDistance;}
	void SetMinDistance(float value) {myMinDistance = value;}
	float GetMaxDistance() {return myMaxDistance;}
	void SetMaxDistance(float value) {myMaxDistance = value;}
	float GetConnectivity() {return myConnectivity;}
	void SetConnectivity(float value) {myConnectivity = value;}
	int GetNumTries() {return myNumTries;}
	void SetNumTries(int num) {myNumTries = num;}
	// sets the distrubtion of number of neighbours
	TNeighbourDistribution NeighbourDistribution;
	//
	void CreateNet(int NumNodes);
private:
	// lists of all nodes and links
	TNodeList *Nodes;
	TLinkList *Links;
	TNGNet *myNet;
	// lists of outer nodes and links
	TNodeList OuterNodes;
	TLinkList OuterLinks;
	void RemoveOuterNode(TNode *Node);
	void RemoveOuterLink(TLink *Link);
	// list of possible new connections
	TNodeList ConNodes;
	// restrictions
	float myMinLinkAngle;  // minimum angle allowed between two links
	float myMinDistance;   // minimum distance allowed between two nodes
	float myMaxDistance;   // maxmium distance allowed between two nodes
	float myConnectivity;  // probability of connecting to a existing node if possible
	//
	int myNumTries; // number of tries to create a new node
	int myNumNodes; // number of nodes to be created
	// checks whether a possible connection between these two nodes
	// complies with the restrictions above
	bool CheckAngles(TNode* Node);
	bool CanConnect(TNode* BaseNode, TNode* NewNode);
	// get random values
	double GetAngle(); // returns random angle [0; 2PI]
	double GetDistance(); // returns random distance [myMinDistance; myMaxDistance]
	bool UseOuterNode(); // uses myConnectivity; returns true, if an outer node shall be used
	bool CreateNewNode(TNode *BaseNode); // creates new random node and returns true, if creation was successfull
	// finds possible connections between Node and OuterNodes
	// complying with restrictions
	void FindPossibleOuterNodes(TNode *Node);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
