//---------------------------------------------------------------------------//
//                        MSCell.h -
//  A class dividing the network in rectangular cells
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jul 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2006/11/16 12:30:54  dkrajzew
// warnings removed
//
// Revision 1.3  2006/11/16 07:02:17  dkrajzew
// warnings removed
//
// Revision 1.2  2006/11/02 11:44:50  dkrajzew
// added Danilo Teta-Boyom's changes to car2car-communication
//
// Revision 1.1  2006/09/18 09:58:02  dkrajzew
// removed deprecated c2c functions, added new made by Danilot Boyom
//
// Revision 1.7  2005/10/06 13:39:50  dksumo
// using of a configuration file rechecked
//
// Revision 1.6  2005/09/20 06:13:04  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.5  2005/09/09 12:55:51  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.4  2005/08/01 13:48:01  dksumo
// getting rid of singleton dictionaries
//
// Revision 1.3  2005/02/17 09:27:36  dksumo
// code beautifying;
// some warnings removed;
// compileable under linux
//
// Revision 1.2  2004/10/29 06:01:55  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.7  2004/07/02 08:41:40  dkrajzew
// detector drawer are now also responsible for other additional items
//
// Revision 1.6  2003/12/09 11:27:15  dkrajzew
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <algorithm>
#include <fstream>
#include <utils/geom/GeomHelper.h>
#include "MSEdge.h"
#include "MSCell.h"
#include "MSCORN.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * MSCell - methods
 * ----------------------------------------------------------------------- */

MSCell::MSCell(SUMOReal xcellsize, SUMOReal ycellsize)
    : _xcellsize(xcellsize), _ycellsize(ycellsize)
{
//	_edgesCont = 0;
//	_neighbors = 0;

}


MSCell::~MSCell()
{
	_edgesCont.clear();
	_neighbors.clear();

}

void
MSCell::setIndex(size_t index)
{
	myIndex = index;
}

 /// Adds an edge into the Container
void
MSCell::addEdge(MSEdge *edge)
{
	_edgesCont.push_back(edge);
}

/// Removes an edge from the Container
void
MSCell::removeEdge(MSEdge *)
{
}

void
MSCell::setCellNeighbors(std::vector<MSCell*> ret)
{
	_neighbors = ret;
}


std::vector<MSCell*> &
MSCell::getCellNeighbors(void)
{
	return _neighbors;
}


std::vector<MSEdge*> &
MSCell::getEdges(void)
{
	return _edgesCont;
}

void
MSCell::setEdgesNeighbors(void)
{
	// all Edges in this Cell
	for(std::vector<MSEdge*>::iterator i=_edgesCont.begin(); i!=_edgesCont.end(); i++) {
		// all others Cells
		int anzahl = -1; // sich selbst rausnehmen
		std::string neighbor;
		for(std::vector<MSCell*>::iterator j=_neighbors.begin(); j!=_neighbors.end(); j++) {
			 // all Edges in others Cells
			for(std::vector<MSEdge*>::iterator k=((*j)->_edgesCont).begin(); k!=((*j)->_edgesCont).end(); k++) {
				(*i)->addNeighborEdge((*k)->getID(),*k);
				anzahl++;
				neighbor=neighbor+" "+(*k)->getID();
			}
		}
		MSCORN::saveEdgeNearInfoData((*i)->getID(), neighbor, anzahl);
	}
}

size_t
MSCell::getIndex(void){
	return myIndex;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




