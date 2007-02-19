/****************************************************************************/
/// @file    DFRORouteCont.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// missing_desc
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
#ifndef DFRORouteCont_h
#define DFRORouteCont_h
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
#include <map>
#include "DFRORouteDesc.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DFRORouteCont
 * @brief A container for routes
 */
class DFRORouteCont
{
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
    void removeIllegal(const std::vector<std::vector<ROEdge*> > &illegals);


protected:
    class by_distance_sorter
    {
    public:
        /// constructor
        explicit by_distance_sorter()
        { }

        int operator()(DFRORouteDesc *p1, DFRORouteDesc *p2)
        {
            return p1->distance2Last<p2->distance2Last;
        }
    };

    class route_by_id_finder
    {
    public:
        /** constructor */
        explicit route_by_id_finder(const DFRORouteDesc &desc) : myDesc(desc)
        { }

        /** the comparing function */
        bool operator()(DFRORouteDesc *desc)
        {
            return myDesc.routename==desc->routename;
        }

    private:
        /// The time to search for
        const DFRORouteDesc &myDesc;

    };


    std::vector<DFRORouteDesc*> myRoutes;
    std::map<ROEdge*, std::vector<ROEdge*> > myDets2Follow;

};


#endif

/****************************************************************************/

