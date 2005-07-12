
#ifndef ValueSource_h
#define ValueSource_h
//---------------------------------------------------------------------------//
//                        ValueSource.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
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
//

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<typename _T>
class ValueSource {
public:
    ValueSource() { }
    virtual ~ValueSource() { }
    virtual _T getValue() const = 0;
    virtual ValueSource *copy() const = 0;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
