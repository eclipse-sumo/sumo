#ifndef ValueRetriever_h
#define ValueRetriever_h
//---------------------------------------------------------------------------//
//                        ValueRetriever.h -
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
// $Log$
// Revision 1.1  2005/09/15 12:20:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/09/09 12:56:09  dksumo
// helpers added
//
// Revision 1.2  2005/05/30 08:15:45  dksumo
// comments added
//

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<typename _T>
class ValueRetriever {
public:
    ValueRetriever() { }
    virtual ~ValueRetriever() { }
    virtual void addValue(_T value) = 0;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
