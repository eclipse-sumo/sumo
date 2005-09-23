#ifndef NIArtemisTempEdgeLanes_h
#define NIArtemisTempEdgeLanes_h
//---------------------------------------------------------------------------//
//                        NIArtemisTempEdgeLanes.h -
//  A temporary storage for artims edge definitions
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <bitset>
#include <utils/common/DoubleVector.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNodeCont;
class NBEdgeCont;
class NBDistrictCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIArtemisTempEdgeLanes {
public:
    static void add(const std::string &link, int lane, int section,
        SUMOReal start, SUMOReal end, const std::string &mvmt);
    static void close(NBDistrictCont &dc,
        NBEdgeCont &ec, NBNodeCont &nc);
private:

    class LinkLaneDesc {
    public:
        LinkLaneDesc(int lane, int section,
            SUMOReal start, SUMOReal end, const std::string &mvmt);
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

protected:
    NBNodeCont &myNodeCont;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

