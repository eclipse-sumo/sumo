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
#include <map>
#include "NamedObjectCont.h"

template<class T>
NamedObjectCont<T>::NamedObjectCont<T>()
{
}

template< class T >
NamedObjectCont< T >::~NamedObjectCont<T>()
{
    for( typename myCont::iterator i=_cont.begin();
         i!=_cont.end();
         i++) {
        delete (*i).second;
    }
}


template<class T>
bool NamedObjectCont<T>::add(const std::string &id, T item) {
    if(_cont.find(id)!=_cont.end()) {
        return false;
    }
    _cont.insert(std::make_pair(id, item));
    return true;
}


template<class T>
T
NamedObjectCont<T>::get(const std::string &id) const
{
    typename std::map<std::string, T>::const_iterator i = _cont.find(id);
    if(i==_cont.end()) {
        return 0;
    }
    return (*i).second;
}


template<class T>
void
NamedObjectCont<T>::clear()
{
    for(typename myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i).second;
    }
    while(_cont.size()>0) {
        _cont.erase(_cont.begin());
    }
}


template<class T>
size_t
NamedObjectCont<T>::size() const
{
    return _cont.size();
}


template<class T>
void
NamedObjectCont<T>::erase(const std::string &id)
{
    typename myCont::iterator i=_cont.find(id);
    if(i==_cont.end()) {
        throw 1; // !!! should not happen
    }
    delete (*i).second;
    _cont.erase(i);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NamedObjectCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:


