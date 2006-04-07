//---------------------------------------------------------------------------//
//                        ODMatrix.cpp -
//  An internal representation of the loaded matrix
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 05 Apr. 2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.1  2006/04/07 05:25:15  dkrajzew
// complete od2trips rework
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "ODMatrix.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <iostream>
#include <algorithm>
#include <list>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ODMatrix::ODMatrix()
    : myNoLoaded(0), myNoWritten(0)
{
}


ODMatrix::~ODMatrix()
{
    for(CellVector::iterator i=myContainer.begin(); i!=myContainer.end(); ++i) {
        delete *i;
    }
    myContainer.clear();
}


void
ODMatrix::add(ODCell *cell)
{
    myNoLoaded += cell->vehicleNumber;
    myContainer.push_back(cell);
}


void
ODMatrix::computeEmissions(const ODDistrictCont &dc, ODCell *cell,
                           size_t &vehName, std::vector<ODVehicle> &into,
                           bool uniform)
{
    SUMOReal offset = (SUMOReal) (cell->end - cell->begin) / (SUMOReal) cell->vehicleNumber / (SUMOReal) 2.;
    for(size_t i=0; i<cell->vehicleNumber; i++) {
        ODVehicle veh;
        veh.id = toString(vehName++);

        if(uniform) {
            veh.depart = (int) (offset + cell->begin + ((SUMOReal) (cell->end - cell->begin) * (SUMOReal) i / (SUMOReal) cell->vehicleNumber));
        } else {
            veh.depart = (int) (cell->begin + (SUMOReal) ((double) rand() / (double) RAND_MAX) * (double) (cell->end - cell->begin));
        }

        veh.from = dc.getRandomSourceFromDistrict(cell->origin);
        veh.to = dc.getRandomSinkFromDistrict(cell->destination);
        veh.type = cell->vehicleType;
        SUMOReal red = dc.getDistrictColor(cell->origin);
        SUMOReal blue = dc.getDistrictColor(cell->destination);
        SUMOReal green = (red + blue) / 2.0f;
        veh.color = RGBColor(red, green, blue);
        into.push_back(veh);
    }
}


void
ODMatrix::write(SUMOTime begin, SUMOTime end,
                std::ofstream &strm, const ODDistrictCont &dc,
                bool uniform)
{
    size_t vehName = 0;
    sort(myContainer.begin(), myContainer.end(), cell_by_begin_sorter());
    // recheck begin time
    ODCell *first = *myContainer.begin();
    begin = MAX2(begin, first->begin);
    CellVector::iterator next = myContainer.begin();
    int absN = 0;
    std::vector<ODVehicle> vehicles;
    // go through the time steps
    for(SUMOTime t=begin; t!=end; t++) {
        MsgHandler::getMessageInstance()->inform("Parsing time " + toString(t));
        // recheck whether a new cell got valid
        bool changed = false;
        while(next!=myContainer.end()&&(*next)->begin<=t&&(*next)->end>t) {
            std::vector<ODVehicle> tmp;
            computeEmissions(dc, *next, vehName, tmp, uniform);
            if(tmp.size()!=0) {
                copy(tmp.begin(), tmp.end(), back_inserter(vehicles));
                changed = true;
            }
            ++next;
        }
        if(changed) {
            sort(vehicles.begin(), vehicles.end(), descending_departure_comperator());
        }
        std::vector<ODVehicle>::reverse_iterator i = vehicles.rbegin();
        for(; i!=vehicles.rend()&&(*i).depart==t; ++i) {
            myNoWritten++;
    		strm << "   <tripdef id=\"" << (*i).id << "\" depart=\"" << t << "\" ";
	    	strm << "from=\"" << (*i).from << "\" ";
		    strm << "to=\"" << (*i).to << "\" ";
            strm << "type=\"" << (*i).type << "\"";
            if(!OptionsSubSys::getOptions().getBool("no-color")) {
                strm << " color=\"" << (*i).color << "\"";
            }
            strm << "/>"<< endl;
        }
        while(vehicles.size()!=0&&(*vehicles.rbegin()).depart==t) {
            vehicles.pop_back();
        }
    }
}


SUMOReal
ODMatrix::getNoLoaded() const
{
    return myNoLoaded;
}


SUMOReal
ODMatrix::getNoWritten() const
{
    return myNoWritten;
}


void
ODMatrix::applyCurve(const Distribution_Points &ps, ODCell *cell, CellVector &newCells)
{
    for(size_t i=0; i<ps.getAreaNo(); i++) {
        ODCell *cell = new ODCell();
        cell->begin = (SUMOTime) ps.getAreaBegin(i);
        cell->end = (SUMOTime) ps.getAreaEnd(i);
        cell->origin = cell->origin;
        cell->destination = cell->destination;
        cell->vehicleType = cell->vehicleType;
        cell->vehicleNumber = cell->vehicleNumber * ps.getAreaPerc(i);
        newCells.push_back(cell);
    }
}


void
ODMatrix::applyCurve(const Distribution_Points &ps)
{
    CellVector oldCells = myContainer;
    myContainer.clear();
    for(CellVector::iterator i=oldCells.begin(); i!=oldCells.end(); ++i) {
        CellVector newCells;
        applyCurve(ps, *i, newCells);
        copy(newCells.begin(), newCells.end(), back_inserter(myContainer));
        delete *i;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
