/****************************************************************************/
/// @file    ODMatrix.cpp
/// @author  Daniel Krajzewicz
/// @date    05 Apr. 2006
/// @version $Id$
///
// An internal representation of the loaded matrix
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

#include "ODmatrix.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <iostream>
#include <algorithm>
#include <list>
#include <utils/common/RandHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ODMatrix::ODMatrix()
        : myNoLoaded(0), myNoWritten(0)
{}


ODMatrix::~ODMatrix()
{
    for (CellVector::iterator i=myContainer.begin(); i!=myContainer.end(); ++i) {
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


SUMOReal
ODMatrix::computeEmissions(const ODDistrictCont &dc, ODCell *cell,
                           size_t &vehName, std::vector<ODVehicle> &into,
                           bool uniform, const std::string &prefix)
{
    int vehicles2emit = (int) cell->vehicleNumber;
    // compute whether the fraction forces an additional vehicle emission
    SUMOReal mrand = randSUMO();
    SUMOReal mprob = (SUMOReal)cell->vehicleNumber-(SUMOReal) vehicles2emit;
    if (mrand<mprob) {
        vehicles2emit++;
    }

    SUMOReal offset = (SUMOReal)(cell->end - cell->begin) / (SUMOReal) vehicles2emit / (SUMOReal) 2.;
    for (int i=0; i<vehicles2emit; i++) {
        ODVehicle veh;
        veh.id = prefix + toString(vehName++);

        if (uniform) {
            veh.depart = (int)(offset + cell->begin + ((SUMOReal)(cell->end - cell->begin) * (SUMOReal) i / (SUMOReal) vehicles2emit));
        } else {
            veh.depart = (int)(cell->begin + randSUMO() * (double)(cell->end - cell->begin));
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
    return cell->vehicleNumber - vehicles2emit;
}


void
ODMatrix::write(SUMOTime begin, SUMOTime end,
                std::ofstream &strm, const ODDistrictCont &dc,
                bool uniform,
                const std::string &prefix)
{
    std::map<std::pair<std::string, std::string>, SUMOReal> fractionLeft;
    size_t vehName = 0;
    sort(myContainer.begin(), myContainer.end(), cell_by_begin_sorter());
    // recheck begin time
    ODCell *first = *myContainer.begin();
    begin = MAX2(begin, first->begin);
    CellVector::iterator next = myContainer.begin();
    std::vector<ODVehicle> vehicles;
    // go through the time steps
    for (SUMOTime t=begin; t!=end; t++) {
        MsgHandler::getMessageInstance()->progressMsg("Parsing time " + toString(t));
        // recheck whether a new cell got valid
        bool changed = false;
        while (next!=myContainer.end()&&(*next)->begin<=t&&(*next)->end>t) {
            std::pair<std::string, std::string> odID = make_pair((*next)->origin, (*next)->destination);
            // check whether the current cell must be extended by the last fraction
            if (fractionLeft.find(odID)!=fractionLeft.end()) {
                (*next)->vehicleNumber += fractionLeft[odID];
                fractionLeft[odID] = 0;
            }
            // get the new emissions (into tmp)
            std::vector<ODVehicle> tmp;
            SUMOReal fraction = computeEmissions(dc, *next, vehName, tmp, uniform, prefix);
            // copy new emissions if any
            if (tmp.size()!=0) {
                copy(tmp.begin(), tmp.end(), back_inserter(vehicles));
                changed = true;
            }
            // save the fraction
            if (fraction!=0) {
                if (fractionLeft.find(odID)==fractionLeft.end()) {
                    fractionLeft[odID] = fraction;
                } else {
                    fractionLeft[odID] += fraction;
                }
            }
            //
            ++next;
        }
        if (changed) {
            sort(vehicles.begin(), vehicles.end(), descending_departure_comperator());
        }
        std::vector<ODVehicle>::reverse_iterator i = vehicles.rbegin();
        for (; i!=vehicles.rend()&&(*i).depart==t; ++i) {
            myNoWritten++;
            strm << "   <tripdef id=\"" << (*i).id << "\" depart=\"" << t << "\" ";
            strm << "from=\"" << (*i).from << "\" ";
            strm << "to=\"" << (*i).to << "\" ";
            if ((*i).type.length()!=0) {
                strm << "type=\"" << (*i).type << "\" ";
            }
            if (!OptionsSubSys::getOptions().getBool("no-color")) {
                strm << "color=\"" << (*i).color << "\" ";
            }
            strm << "/>"<< endl;
        }
        while (vehicles.size()!=0&&(*vehicles.rbegin()).depart==t) {
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
    for (size_t i=0; i<ps.getAreaNo(); i++) {
        ODCell *ncell = new ODCell();
        ncell->begin = (SUMOTime) ps.getAreaBegin(i);
        ncell->end = (SUMOTime) ps.getAreaEnd(i);
        ncell->origin = cell->origin;
        ncell->destination = cell->destination;
        ncell->vehicleType = cell->vehicleType;
        ncell->vehicleNumber = cell->vehicleNumber * ps.getAreaPerc(i);
        newCells.push_back(ncell);
    }
}


void
ODMatrix::applyCurve(const Distribution_Points &ps)
{
    CellVector oldCells = myContainer;
    myContainer.clear();
    for (CellVector::iterator i=oldCells.begin(); i!=oldCells.end(); ++i) {
        CellVector newCells;
        applyCurve(ps, *i, newCells);
        copy(newCells.begin(), newCells.end(), back_inserter(myContainer));
        delete *i;
    }
}



/****************************************************************************/

