#ifndef NIArtemisTempSegments_h
#define NIArtemisTempSegments_h
//---------------------------------------------------------------------------//
//                        NIArtemisTempSegments.h -  ccc
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
// $Log$
// Revision 1.3  2003/06/05 11:44:51  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <vector>
#include <map>
#include <string>
#include <utils/geom/Position2D.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIArtemisTempSegments {
public:
    static void add(const std::string &linkname, int segment,
        double x, double y);
    static void close();
private:
    class SingleGeomPoint {
    public:
        SingleGeomPoint(int segment, double x, double y);
        ~SingleGeomPoint();
        Position2D getPosition() const;
        int getSegmentNo() const;
    private:
        int mySegment;
        double myX, myY;
    };


    class info_sorter {
    public:
        /// constructor
        explicit info_sorter() { }

        int operator() (SingleGeomPoint *p1, SingleGeomPoint *p2) {
            return p1->getSegmentNo() < p2->getSegmentNo();
        }
    };


    typedef std::vector<SingleGeomPoint*> PosInfos;
    typedef std::map<std::string, PosInfos> Link2PosInfos;

    static Link2PosInfos myLinkPosInfos;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIArtemisTempSegments.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

