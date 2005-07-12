#ifndef NamedObjectContSingleton_h
#define NamedObjectContSingleton_h
//---------------------------------------------------------------------------//
//                        NamedObjectContSingleton.h -
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
// Revision 1.2  2005/07/12 12:43:49  dkrajzew
// code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NamedObjectCont.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<class T>
class NamedObjectContSingleton {
public:
    static NamedObjectCont<T> &getInstance() { return myObjects; }

private:
    static NamedObjectCont<T> myObjects;

};

template<class T> NamedObjectCont<T> NamedObjectContSingleton<T>::myObjects;

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
