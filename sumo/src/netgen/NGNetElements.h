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
// Revision 1.2  2003/07/21 11:05:31  dkrajzew
// patched some bugs found in first real-life execution
//
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
class NBNode;
class NBEdge;


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
public:
	// constructors
	TNode();
    TNode(const std::string &id);
	TNode(const std::string &id, int xID, int yID);
    TNode(const std::string &id, int xID, int yID, bool amCenter);
	// destructor
	~TNode();
    const std::string &GetID() const {return myID;}
	float x() {return myX;}
	float y() {return myY;}
	float MaxNeighbours() {return myMaxNeighbours;}
	void SetMaxNeighbours(float value) {myMaxNeighbours = value;}
	void SetX(float x) {myX = x;}
	void SetY(float y) {myY = y;}
    /// removes link from LinkList
	void RemoveLink(TLink *Link);

    /// Build the according netbuilder-node
    NBNode *buildNBNode() const;

    /// adds a link to the list of used
    void addLink(TLink *link);

    /// removes a link from the list of used
    void removeLink(TLink *link);

    bool connected(TNode *node) const;

	int xID;
	int yID;

    /// list of connected links
	TLinkList LinkList;

private:

    /// The x-position of the node
	float myX;

    /// The y-position of the node
	float myY;

    /// The maximum number of neighbours
	float myMaxNeighbours;

    /// The id of the node
    std::string myID;

    /// Information whether this is the center of a cpider-net
    bool myAmCenter;

};


/**
 *
 */
class TLink
{
public:
	// constructors
	TLink();
	TLink(const std::string &id);
	TLink(const std::string &id, TNode *StartNode, TNode *EndNode);
	// destructor
	~TLink();
    const std::string &GetID() const {return myID;}
	TNode* StartNode() {return myStartNode;};
	TNode* EndNode() {return myEndNode;};

    /// Build the according netbuilder-edge
    NBEdge *buildNBEdge() const;

private:
    /// The id of the edge
    std::string myID;

    /// The node the edge starts at
	TNode *myStartNode;

    /// The node the edge ends at
	TNode *myEndNode;

};




/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NGNetElements.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
