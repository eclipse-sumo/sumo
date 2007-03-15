/****************************************************************************/
/// @file    MSCell.cpp
/// @author  Danilot Teta Boyom
/// @date    Jul 2003
/// @version $Id$
///
// A class represents the cells
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

#include <vector>
#include <algorithm>
#include <fstream>
#include <utils/geom/GeomHelper.h>
#include "MSEdge.h"
#include "MSCell.h"
#include "MSCORN.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSCell::MSCell(SUMOReal xcellsize, SUMOReal ycellsize)
        : _xcellsize(xcellsize), _ycellsize(ycellsize)
{
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


void
MSCell::addEdge(MSEdge *edge)
{
    _edgesCont.push_back(edge);
}


void
MSCell::setCellNeighbors(std::vector<MSCell*> ret)
{
    _neighbors = ret;
}


void
MSCell::setEdgesNeighbors(void)
{
    // all Edges in this Cell
    for (std::vector<MSEdge*>::iterator i=_edgesCont.begin(); i!=_edgesCont.end(); i++) {
        // all others Cells
        int anzahl = -1; // sich selbst rausnehmen
        std::string neighbor;
        for (std::vector<MSCell*>::iterator j=_neighbors.begin(); j!=_neighbors.end(); j++) {
            // all Edges in others Cells
            for (std::vector<MSEdge*>::iterator k=((*j)->_edgesCont).begin(); k!=((*j)->_edgesCont).end(); k++) {
                (*i)->addNeighborEdge(*k);
                anzahl++;
                neighbor=neighbor+" "+(*k)->getID();
            }
        }
    }
}



/****************************************************************************/

