//---------------------------------------------------------------------------//
//                        TrackerValueDesc.cpp -
//  Storage for a tracked value
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
// Revision 1.2  2003/06/05 11:38:47  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <utils/gfx/RGBColor.h>
#include <gui/GUIGlObject.h>
#include <guisim/GUINet.h>
#include "TrackerValueDesc.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
TrackerValueDesc::TrackerValueDesc(const std::string &name,
                                   const RGBColor &col,
                                   GUIGlObject *o, size_t itemPos)
    : myObject(o), myItemPos(itemPos),
    myActiveCol(col), myInactiveCol(col),
    myAmActive(true),
    myMin(0), myMax(0)
{
}


TrackerValueDesc::~TrackerValueDesc()
{
}


void
TrackerValueDesc::simStep()
{
	if(!myObject->active()) {
		return;
	}
    double val = myObject->getTableParameter(myItemPos);
    if(myValues.size()==0) {
        myMin = val;
        myMax = val;
    } else {
        myMin = val < myMin ? val : myMin;
        myMax = val > myMax ? val : myMax;
    }
    myValues.push_back(val);
}


float
TrackerValueDesc::getRange() const
{
    getMin();
    getMax();
    return myMax - myMin;
}


float
TrackerValueDesc::getMin() const
{
    return myMin;
}


float
TrackerValueDesc::getMax() const
{
    return myMax;
}


float
TrackerValueDesc::getYCenter() const
{
    getMin();
    getMax();
    return (myMin + myMax) / 2.0;
}


const RGBColor &
TrackerValueDesc::getColor() const
{
    return myActiveCol;
}


const std::vector<float> &
TrackerValueDesc::getValues() const
{
    return myValues;
}


const std::string &
TrackerValueDesc::getName() const
{
    return myName;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "TrackerValueDesc.icc"
//#endif

// Local Variables:
// mode:C++
// End:


