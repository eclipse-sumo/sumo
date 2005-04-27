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
// Revision 1.6  2005/04/27 11:48:51  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.5  2004/11/23 10:22:03  dkrajzew
// debugging
//
// Revision 1.4  2004/08/02 12:41:40  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.3  2004/03/19 13:03:52  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/07/21 11:05:31  dkrajzew
// patched some bugs found in first real-life execution
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
#include <list>
#include <utils/geom/Position2D.h>


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
class NBNetBuilder;


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
    const Position2D &getPosition() const { return myPosition; }
    double MaxNeighbours() {return myMaxNeighbours;}
    void SetMaxNeighbours(float value) {myMaxNeighbours = value;}
    void SetX(double x) { myPosition.set(x, myPosition.y()); }
    void SetY(double y) { myPosition.set(myPosition.x(), y); }
    /// removes link from LinkList
    void RemoveLink(TLink *Link);

    /// Build the according netbuilder-node
    NBNode *buildNBNode(NBNetBuilder &nb) const;

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
    /// The id of the node
    std::string myID;

    /// The position of the node
    Position2D myPosition;

    /// The maximum number of neighbours
    double myMaxNeighbours;

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
    NBEdge *buildNBEdge(NBNetBuilder &nb) const;

private:
    /// The id of the edge
    std::string myID;

    /// The node the edge starts at
    TNode *myStartNode;

    /// The node the edge ends at
    TNode *myEndNode;

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
