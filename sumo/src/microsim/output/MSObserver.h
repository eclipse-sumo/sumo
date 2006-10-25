#ifndef MSOBSERVER_H
#define MSOBSERVER_H


/**
 * @file    MSObserver.h
 * @author  Christian Roessel <christian.roessel@dlr.de>
 * @date    Started Fri Nov 21 12:44:14 2003
 * @version
 *
 * @brief
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// See Gamma et al. "Design Patterns" for information about the
// Observer pattern.

// A concrete observer must inherit from MSObserver and implement an
// update method. This method will be called if the state of the
// observed object (derived from MSSubject) changes.
//
// A concrete subject defines the Observer type by it's base MSSubject
// (see MSSubject.h).

// $Log$
// Revision 1.6  2006/10/25 12:22:38  dkrajzew
// updated
//
// Revision 1.5  2005/10/06 13:39:19  dksumo
// using of a configuration file rechecked
//
// Revision 1.4  2005/09/09 12:51:23  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.3  2005/06/14 11:21:06  dksumo
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef WIN32
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/helpers/msvc6_TypeTraits.h>
#endif
#ifndef WIN32
#include <utils/helpers/gcc_TypeTraits.h>
#endif


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template<
    class ObservedType
    , class ObservedQuantity
    >
class MSObserverPassesObserved
{
public:
    typedef ObservedType Observed;
    typedef typename Loki::TypeTraits< Observed >::ParameterType ParameterType;

    virtual void update( ParameterType aObserved ) = 0;

protected:

    MSObserverPassesObserved( void ){}

    virtual ~MSObserverPassesObserved( void ){}

private:

    MSObserverPassesObserved( const MSObserverPassesObserved& );
    MSObserverPassesObserved& operator=( const MSObserverPassesObserved& );

};



template<
    class ObservedType
    , class ObservedQuantity
    >
class MSObserver
{
public:
    // Return bool (instead of void) to please MSVC++
    virtual bool update( void ) = 0;

protected:

    MSObserver( void ){};

    virtual ~MSObserver( void ){}

private:

    MSObserver( const MSObserver& );
    MSObserver& operator=( const MSObserver& );

};


// Local Variables:
// mode:C++
// End:

#endif // MSOBSERVER_H
