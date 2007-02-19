/****************************************************************************/
/// @file    NIArtemisTempSegments.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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
#ifndef NIArtemisTempSegments_h
#define NIArtemisTempSegments_h
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
#include <string>
#include <utils/geom/Position2D.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIArtemisTempSegments
{
public:
    static void add(const std::string &linkname, int segment,
                        SUMOReal x, SUMOReal y);
    static void close(NBEdgeCont &ec);
private:
    class SingleGeomPoint
    {
    public:
        SingleGeomPoint(int segment, SUMOReal x, SUMOReal y);
        ~SingleGeomPoint();
        Position2D getPosition() const;
        int getSegmentNo() const;
    private:
        int mySegment;
        SUMOReal myX, myY;
    };


    class info_sorter
    {
    public:
        /// constructor
        explicit info_sorter()
        { }

        int operator()(SingleGeomPoint *p1, SingleGeomPoint *p2)
        {
            return p1->getSegmentNo() < p2->getSegmentNo();
        }
    };


    typedef std::vector<SingleGeomPoint*> PosInfos;
    typedef std::map<std::string, PosInfos> Link2PosInfos;

    static Link2PosInfos myLinkPosInfos;
};


#endif

/****************************************************************************/

