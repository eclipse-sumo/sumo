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

#include <string>
#include <map>
#include <bitset>
#include <utils/common/DoubleVector.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIArtemisTempEdgeLanes {
public:
    static void add(const std::string &link, int lane, int section,
        double start, double end, const std::string &mvmt);
    static void close();
private:

    class LinkLaneDesc {
    public:
        LinkLaneDesc(int lane, int section,
            double start, double end, const std::string &mvmt);
        ~LinkLaneDesc();
        double getStart() const;
        double getEnd() const;
        int getLane() const;
        void patchPosition(double length);
    private:
        int myLane;
        int mySection;
        double myStart;
        double myEnd;
    };

    static size_t getBeginIndex(double start, const DoubleVector &poses);
    static size_t getEndIndex(double start, const DoubleVector &poses);
    static size_t count(const std::bitset<64> &lanes);



    typedef std::vector<LinkLaneDesc*> LaneDescVector;
    typedef std::map<std::string, LaneDescVector> Link2LaneDesc;
    static Link2LaneDesc myLinkLaneDescs;

    typedef std::map<std::string, DoubleVector> Link2Positions;
    static Link2Positions myLinkLanePositions;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NIArtemisTempEdgeLanes.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

