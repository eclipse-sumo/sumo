/***************************************************************************
                          NBDistrictCont.cpp
			  A container for all of the nets districts
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/03/03 14:58:59  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:43  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include "NBDistrict.h"
#include "NBDistrictCont.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static members
 * ======================================================================= */
NBDistrictCont::DistrictCont NBDistrictCont::_districts;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
NBDistrictCont::insert(NBDistrict *district)
{
    DistrictCont::iterator i = _districts.find(district->getID());
    if(i!=_districts.end()) return false;
    _districts.insert(DistrictCont::value_type(district->getID(), district));
    return true;
}


NBDistrict *
NBDistrictCont::retrieve(const string &id)
{
    DistrictCont::iterator i = _districts.find(id);
    if(i==_districts.end()) return 0;
    return (*i).second;
}


void
NBDistrictCont::writeXML(ostream &into)
{
    for(DistrictCont::iterator i=_districts.begin(); i!=_districts.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << endl;
}


int NBDistrictCont::size() {
    return _districts.size();
}


void
NBDistrictCont::clear() {
    for(DistrictCont::iterator i=_districts.begin(); i!=_districts.end(); i++)
        delete((*i).second);
    _districts.clear();
}


void
NBDistrictCont::report(bool verbose)
{
    if(verbose) {
        cout << "   " << size() << " districts loaded." << endl;
    }
}


bool
NBDistrictCont::addSource(const std::string &dist, NBEdge *source,
                          double weight)
{
    NBDistrict *o = retrieve(dist);
    if(o==0) {
        return false;
    }
    return o->addSource(source, weight);
}


bool
NBDistrictCont::addSink(const std::string &dist, NBEdge *destination,
                        double weight)
{
    NBDistrict *o = retrieve(dist);
    if(o==0) {
        return false;
    }
    return o->addSink(destination, weight);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBDistrictCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:

