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

template<class _I>
NamedObjectCont<_I>::NamedObjectCont()
{
}

template<class _I>
NamedObjectCont<_I>::~NamedObjectCont()
{
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i).second;
    }
}


template<class _I>
bool NamedObjectCont<_I>::add(const std::string &id, _I item) {
    if(_cont.find(id)!=_cont.end()) {
        return false;
    }
    _cont.insert(myCont::value_type(id, item));
    return true;
}


template<class _I>
_I
NamedObjectCont<_I>::get(const std::string &id) const
{
    myCont::const_iterator i = _cont.find(id);
    if(i==_cont.end()) {
        return 0;
    }
    return (*i).second;
}


template<class _I>
void
NamedObjectCont<_I>::clear()
{
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i).second;
    }
    while(_cont.size()>0) {
        _cont.erase(_cont.begin());
    }
}


template<class _I>
size_t
NamedObjectCont<_I>::size() const
{
    return _cont.size();
}


template<class _I>
void
NamedObjectCont<_I>::erase(const std::string &id)
{
    myCont::iterator i=_cont.find(id);
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


