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
// Revision 1.7  2005/10/07 11:39:05  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:01:53  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2003/06/05 11:44:51  dkrajzew
// class templates applied; documentation added
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
        SUMOReal x, SUMOReal y);
    static void close(NBEdgeCont &ec);
private:
    class SingleGeomPoint {
    public:
        SingleGeomPoint(int segment, SUMOReal x, SUMOReal y);
        ~SingleGeomPoint();
        Position2D getPosition() const;
        int getSegmentNo() const;
    private:
        int mySegment;
        SUMOReal myX, myY;
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

#endif

// Local Variables:
// mode:C++
// End:

