#ifndef NGNetElements_h
#define NGNetElements_h
/***************************************************************************
                          NGNetElements.h
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
// Revision 1.1  2003/07/16 15:33:08  dkrajzew
// files needed to generate networks added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <list>


/* =========================================================================
 * some definitions
 * ======================================================================= */
#define PI 3.1415926535897932384626433832795


/* =========================================================================
 * class declarations
 * ======================================================================= */
class TLink;
class TNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
typedef std::list<TNode*> TNodeList;
typedef std::list<TLink*> TLinkList;

/**
 *
 */
class TNode
{
private:
	float myX;
	float myY;
	float myMaxNeighbours;
	int myID;
public:
	// constructors
	TNode();
	TNode(int ID);
	TNode(int ID, int xID, int yID);
	// destructor
	~TNode();
	int GetID() {return myID;}
	float x() {return myX;}
	float y() {return myY;}
	float MaxNeighbours() {return myMaxNeighbours;}
	void SetMaxNeighbours(float value) {myMaxNeighbours = value;}
	void SetX(float x) {myX = x;}
	void SetY(float y) {myY = y;}
	int xID;
	int yID;
	TLinkList LinkList; // list of connected links
	void RemoveLink(TLink *Link); // removes link from LinkList
};


/**
 *
 */
class TLink
{
private:
	int myID;
	TNode *myStartNode;
	TNode *myEndNode;
public:
	// constructors
	TLink();
	TLink(int ID);
	TLink(int ID, TNode *StartNode, TNode *EndNode);
	// destructor
	~TLink();
	int GetID() {return myID;}
	TNode* StartNode() {return myStartNode;};
	TNode* EndNode() {return myEndNode;};
};




/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NGNetElements.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
