//---------------------------------------------------------------------------//
//                        NamedObjectCont.cpp -
//  A map over named objects
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
// Revision 1.6  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.5  2004/01/12 15:40:54  dkrajzew
// debugging
//
// Revision 1.4  2004/01/12 15:13:00  dkrajzew
// allowed the extraction of a vector containing the stored items
//
// Revision 1.3  2003/02/10 17:42:36  roessel
// Added necessary keyword typename.
//
// Revision 1.2  2003/02/07 10:47:17  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cassert>
#include "NamedObjectCont.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
template<class T>
NamedObjectCont<T>::NamedObjectCont<T>()
{
}


template< class T >
NamedObjectCont< T >::~NamedObjectCont<T>()
{
    for( typename IDMap::iterator i=myMap.begin();
         i!=myMap.end();
         i++) {
        delete (*i).second;
    }
}


template<class T>
bool NamedObjectCont<T>::add(const std::string &id, T item)
{
    if(myMap.find(id)!=myMap.end()) {
        return false;
    }
    myMap.insert(std::make_pair(id, item));
    myVector.push_back(item);
    return true;
}


template<class T>
T
NamedObjectCont<T>::get(const std::string &id) const
{
    typename std::map<std::string, T>::const_iterator i = myMap.find(id);
    if(i==myMap.end()) {
        return 0;
    }
    return (*i).second;
}


template<class T>
void
NamedObjectCont<T>::clear()
{
    for(typename IDMap::iterator i=myMap.begin(); i!=myMap.end(); i++) {
        delete (*i).second;
    }
    while(myMap.size()>0) {
        myMap.erase(myMap.begin());
    }
    myVector.clear();
}


template<class T>
size_t
NamedObjectCont<T>::size() const
{
    assert(myMap.size()==myVector.size());
    return myMap.size();
}


template<class T>
void
NamedObjectCont<T>::erase(const std::string &id)
{
    typename IDMap::iterator i=myMap.find(id);
    if(i==myMap.end()) {
        throw 1; // !!! should not happen
    }
    T o = (*i).second;
    myMap.erase(i);
    // and from the vector
    typename ObjectVector::iterator i2 =
        find(myVector.begin(), myVector.end(), o);
    if(i2==myVector.end()) {
        throw 1; // !!! should not happen
    }
    myVector.erase(i2);
    delete o;
}


template<class T>
const std::vector<T> &
NamedObjectCont<T>::getVector() const
{
    return myVector;
}


template<class T>
const std::map<std::string, T> &
NamedObjectCont<T>::getMyMap() const
{
    return myMap;
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NamedObjectCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


