/****************************************************************************/
/// @file    NGNetElements.h
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
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
#ifndef NGNetElements_h
#define NGNetElements_h
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

#include <list>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class TLink;
class TNode;
class NBNode;
class NBEdge;
class NBNetBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
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
    const std::string &GetID() const
    {
        return myID;
    }
    const Position2D &getPosition() const
    {
        return myPosition;
    }
    SUMOReal MaxNeighbours()
    {
        return myMaxNeighbours;
    }
    void SetMaxNeighbours(SUMOReal value)
    {
        myMaxNeighbours = value;
    }
    void SetX(SUMOReal x)
    {
        myPosition.set(x, myPosition.y());
    }
    void SetY(SUMOReal y)
    {
        myPosition.set(myPosition.x(), y);
    }
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
    SUMOReal myMaxNeighbours;

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
    const std::string &GetID() const
    {
        return myID;
    }
    TNode* StartNode()
    {
        return myStartNode;
    };
    TNode* EndNode()
    {
        return myEndNode;
    };

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


#endif

/****************************************************************************/

