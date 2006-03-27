#ifndef DFRORouteCont_h
#define DFRORouteCont_h
/***************************************************************************
                          DFDetFlowLoader.h
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

// $Log$
// Revision 1.6  2006/03/27 07:32:15  dkrajzew
// some further work...
//
// Revision 1.5  2006/03/17 09:04:26  dkrajzew
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

#include <vector>
#include <map>
#include "DFRORouteDesc.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class DFRORouteCont
 * @brief A container for routes
 */
class DFRORouteCont {
public:
    DFRORouteCont();
//	DFRORouteCont(const DFRORouteCont &s);
    ~DFRORouteCont();
    void addRouteDesc(DFRORouteDesc *desc);
    bool removeRouteDesc(DFRORouteDesc *desc);
    bool readFrom(const std::string &file);
    bool save(std::vector<std::string> &saved,
		const std::string &prependix, std::ostream &os/*const std::string &file*/);
    bool computed() const;
	const std::vector<DFRORouteDesc*> &get() const;
    void sortByDistance();
    void setDets2Follow(const std::map<ROEdge*, std::vector<ROEdge*> > &d2f);
    const std::map<ROEdge*, std::vector<ROEdge*> > &getDets2Follow() const;


protected:
    class by_distance_sorter {
    public:
        /// constructor
        explicit by_distance_sorter() { }

        int operator() (DFRORouteDesc *p1, DFRORouteDesc *p2) {
            return p1->distance2Last<p2->distance2Last;
        }
    };

    std::vector<DFRORouteDesc*> myRoutes;
    std::map<ROEdge*, std::vector<ROEdge*> > myDets2Follow;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
