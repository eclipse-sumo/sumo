//---------------------------------------------------------------------------//
//                        GUIVehicle.cpp -
//  A MSVehicle extended by some values for usage within the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cmath>
#include <vector>
#include <string>
#include <microsim/MSVehicle.h>
#include "GUIVehicle.h"

using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
float GUIVehicle::_laneChangeColor1[3];
float GUIVehicle::_laneChangeColor2[3];


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIVehicle::GUIVehicle( std::string id, MSRoute* route,
                       MSNet::Time departTime,
                       const MSVehicleType* type,
                       size_t noMeanData,
                       float *defColor)
    : MSVehicle(id, route, departTime, type, noMeanData),
    GUIGlObject(string("vehicle:")+id)
{
    // copy the defined color
    if(defColor!=0) {
        _definedColor[0] = defColor[0];
        _definedColor[1] = defColor[1];
        _definedColor[2] = defColor[2];
    }
    // compute both random colors
    //  color1
    long prod = 1;
    for(size_t i=0; i<id.length(); i++) {
        prod *= (int) id.at(i);
        if(prod>(1<<24)) {
            prod /= 128;
        }
    }
    _randomColor1[0] = (double) (256-(prod & 255)) / (double) 255;
    _randomColor1[1] = (double) (256-((prod>>8) & 255)) / (double) 255;
    _randomColor1[2] = (double) (256-((prod>>16) & 255)) / (double) 255;
    // color2
    _randomColor2[0] = (double)rand() / (double)(RAND_MAX+1);
    _randomColor2[1] = (double)rand() / (double)(RAND_MAX+1);
    _randomColor2[2] = (double)rand() / (double)(RAND_MAX+1);
    // lane change color (static!!!)
    _laneChangeColor1[0] = 1;
    _laneChangeColor1[1] = 1;
    _laneChangeColor1[2] = 1;
    _laneChangeColor2[0] = float(0.7);
    _laneChangeColor2[1] = float(0.7);
    _laneChangeColor2[2] = float(0.7);
}


GUIVehicle::~GUIVehicle()
{
}


std::vector<std::string>
GUIVehicle::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSVehicle::myDict.size());
    for(MSVehicle::DictType::iterator i=MSVehicle::myDict.begin();
        i!=MSVehicle::myDict.end(); i++) {
        ret.push_back((*i).first);
    }
    return ret;
}


const float *
GUIVehicle::getDefinedColor() const
{
    return _definedColor;
}


const float *
GUIVehicle::getRandomColor1() const
{
    return _randomColor1;
}


const float *
GUIVehicle::getRandomColor2() const
{
    return _randomColor2;
}


int
GUIVehicle::getPassedColor() const
{
    int passed = 255 - myLastLaneChangeOffset;
    if(passed<128) {
        passed = 128;
    }
    return passed;
}


const float *
GUIVehicle::getLaneChangeColor2() const
{
    if(myLastLaneChangeOffset==0) {
        return _laneChangeColor1;
    } else {
        return _laneChangeColor2;
    }
}


long
GUIVehicle::getWaitingTime() const
{
    return myWaitingTime;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIVehicle.icc"
//#endif

// Local Variables:
// mode:C++
// End:


