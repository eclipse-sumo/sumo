#ifndef NBCapacity2Lanes_h
#define NBCapacity2Lanes_h
/***************************************************************************
                          NBCapacity2Lanes.h
			  A helper class for the computation of the number of lanes an
                edge has in dependence to this edge's capacity
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.1  2002/07/25 08:41:44  dkrajzew
// Visum7.5 and Cell import added
//
//
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBCapacity2Lanes
 * A helper class for the computation of the number of lanes an edge has
 * in dependence to this edge's capacity
 */
class NBCapacity2Lanes {
private:
    /// the norming factor
    float _divider;
public:
    /// constructor
    NBCapacity2Lanes(float divider) : _divider(divider) { }
    /// destructor
    ~NBCapacity2Lanes() { }
    /// returns the number of lanes computed from the given capacity
    int get(float capacity) {
        capacity /= _divider;
        if(capacity>(int) capacity) {
            capacity += 1;
        }
        // just assure that the number of lanes is not zero
        if(capacity==0) {
            capacity = 1;
        }
        return (int) capacity;
    }
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBCapacity2Lanes.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
