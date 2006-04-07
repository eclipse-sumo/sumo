/***************************************************************************
                          DFRORouteCont.cpp
    A container for routes
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.8  2006/04/07 05:29:39  dkrajzew
// removed some warnings
//
// Revision 1.7  2006/03/27 07:32:15  dkrajzew
// some further work...
//
// Revision 1.6  2006/03/17 09:04:26  dkrajzew
// class-documentation added/patched
//
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

#include <fstream>
#include <cassert>
#include "DFRORouteDesc.h"
#include "DFRORouteCont.h"
#include <router/ROEdge.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
DFRORouteCont::DFRORouteCont()
{
}

/*
DFRORouteCont::DFRORouteCont(const DFRORouteCont &s)
{
    {
    	for(std::vector<DFRORouteDesc*>::const_iterator j=s.myRoutes.begin(); j!=s.myRoutes.end(); ++j) {
	    	DFRORouteDesc *desc = new DFRORouteDesc(*(*j));
		    myRoutes.push_back(desc);
	    }
    }
    {
    	for(std::map<ROEdge*, std::vector<ROEdge*> >::const_iterator j=s.myDets2Follow.begin(); j!=s.myDets2Follow.end(); ++j) {
		    myDets2Follow[(*j).first] = (*j).second;
	    }
    }
}
*/

DFRORouteCont::~DFRORouteCont()
{
    {
    	for(std::vector<DFRORouteDesc*>::const_iterator j=myRoutes.begin(); j!=myRoutes.end(); ++j) {
	    	delete (*j);
	    }
    }
}


void
DFRORouteCont::addRouteDesc(DFRORouteDesc *desc)
{
	/*
    ROEdge *start = desc.edges2Pass[0];
    if(myRoutes.find(start)==myRoutes.end()) {
        myRoutes[start] = std::vector<DFRORouteDesc>();
    }
	*/
    myRoutes.push_back(desc);
}


bool
DFRORouteCont::removeRouteDesc(DFRORouteDesc *desc)
{
    std::vector<DFRORouteDesc*>::const_iterator j =
        find(myRoutes.begin(), myRoutes.end(), desc);
    if(j==myRoutes.end()) {
        return false;
    }
    return true;
}


bool
DFRORouteCont::readFrom(const std::string &file)
{
    throw 1;
}


bool
DFRORouteCont::save(std::vector<std::string> &saved,
					const std::string &prependix, std::ostream &os/*const std::string &file*/)
{
	/*
    ofstream strm(file.c_str());
    if(!strm.good()) {
        return false;
    }
    //strm << "<routes>" << endl;
//    for(std::map<ROEdge*, std::vector<DFRORouteDesc> >::iterator i=myRoutes.begin(); i!=myRoutes.end(); ++i) {
//        const std::vector<DFRORouteDesc> &routes = (*i).second;
*/
	bool haveSavedOnAtLeast = false;
        for(std::vector<DFRORouteDesc*>::const_iterator j=myRoutes.begin(); j!=myRoutes.end(); ++j) {
            const DFRORouteDesc *desc = (*j);
			if(find(saved.begin(), saved.end(), (*j)->routename)!=saved.end()) {
				continue;
			}
			saved.push_back((*j)->routename);
            assert(desc->edges2Pass.size()>=1);
            os << "   <route id=\"" << prependix << (*j)->routename << "\" multi_ref=\"x\">";
            for(std::vector<ROEdge*>::const_iterator k=desc->edges2Pass.begin(); k!=desc->edges2Pass.end(); k++) {
                if(k!=desc->edges2Pass.begin()) {
                    os << ' ';
                }
                os << (*k)->getID();
            }
            os << "</route>" << endl;
			haveSavedOnAtLeast = true;
        }
/*        strm << endl;
  //  }
    //strm << "</routes>" << endl;
    return true;*/
	return haveSavedOnAtLeast;
}


bool
DFRORouteCont::computed() const
{
    return myRoutes.size()!=0;
}


const std::vector<DFRORouteDesc*> &
DFRORouteCont::get() const
{
	return myRoutes;
}


void
DFRORouteCont::sortByDistance()
{
    sort(myRoutes.begin(), myRoutes.end(), by_distance_sorter());
}


void
DFRORouteCont::setDets2Follow(const std::map<ROEdge*, std::vector<ROEdge*> > &d2f)
{
    myDets2Follow = d2f;
}


const std::map<ROEdge*, std::vector<ROEdge*> > &
DFRORouteCont::getDets2Follow() const
{
    return myDets2Follow;
}


void
DFRORouteCont::removeIllegal(const std::vector<std::vector<ROEdge*> > &illegals)
{
    for(std::vector<DFRORouteDesc*>::iterator i=myRoutes.begin(); i!=myRoutes.end(); ) {
        DFRORouteDesc *desc = *i;
        bool remove = false;
        for(std::vector<std::vector<ROEdge*> >::const_iterator j=illegals.begin(); !remove&&j!=illegals.end(); ++j) {
            int noFound = 0;
            for(std::vector<ROEdge*>::const_iterator k=(*j).begin(); !remove&&k!=(*j).end(); ++k) {
                if(find(desc->edges2Pass.begin(), desc->edges2Pass.end(), *k)!=desc->edges2Pass.end()) {
                    noFound++;
                    if(noFound>1) {
                        remove = true;
                    }
                }
            }
        }
        if(remove) {
            i = myRoutes.erase(i);
        } else {
            ++i;
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

