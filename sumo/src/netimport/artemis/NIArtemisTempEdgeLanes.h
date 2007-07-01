/****************************************************************************/
/// @file    NIArtemisTempEdgeLanes.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A temporary storage for artims edge definitions
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
#ifndef NIArtemisTempEdgeLanes_h
#define NIArtemisTempEdgeLanes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <bitset>
#include <utils/common/DoubleVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNodeCont;
class NBEdgeCont;
class NBDistrictCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIArtemisTempEdgeLanes
{
public:
    static void add(const std::string &link, int lane, int section,
                        SUMOReal start, SUMOReal end);
    static void close(NBDistrictCont &dc,
                      NBEdgeCont &ec, NBNodeCont &nc);
private:

    class LinkLaneDesc
    {
    public:
        LinkLaneDesc(int lane, int section,
                     SUMOReal start, SUMOReal end);
        ~LinkLaneDesc();
        SUMOReal getStart() const;
        SUMOReal getEnd() const;
        int getLane() const;
        void patchPosition(SUMOReal length);
    private:
        int myLane;
        int mySection;
        SUMOReal myStart;
        SUMOReal myEnd;
    };

    static size_t getBeginIndex(SUMOReal start, const DoubleVector &poses);
    static size_t getEndIndex(SUMOReal start, const DoubleVector &poses);
    static size_t count(const std::bitset<64> &lanes);



    typedef std::vector<LinkLaneDesc*> LaneDescVector;
    typedef std::map<std::string, LaneDescVector> Link2LaneDesc;
    static Link2LaneDesc myLinkLaneDescs;

    typedef std::map<std::string, DoubleVector> Link2Positions;
    static Link2Positions myLinkLanePositions;

};


#endif

/****************************************************************************/

