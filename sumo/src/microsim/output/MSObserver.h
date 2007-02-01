/****************************************************************************/
/// @file    MSObserver.h
/// @author  Christian Roessel
/// @date    Fri Nov 21 12:44:14 2003
/// @version $Id: $
///
// * @author  Christian Roessel <christian.roessel@dlr.de>
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSObserver_h
#define MSObserver_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/helpers/msvc6_TypeTraits.h>
#endif
#ifndef WIN32
#include <utils/helpers/gcc_TypeTraits.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
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

    virtual void update(ParameterType aObserved) = 0;

protected:

    MSObserverPassesObserved(void)
    {}

    virtual ~MSObserverPassesObserved(void)
    {}

private:

    MSObserverPassesObserved(const MSObserverPassesObserved&);
    MSObserverPassesObserved& operator=(const MSObserverPassesObserved&);

};



template<
class ObservedType
, class ObservedQuantity
>
class MSObserver
{
public:
    // Return bool (instead of void) to please MSVC++
    virtual bool update(void) = 0;

protected:

    MSObserver(void)
    {};

    virtual ~MSObserver(void)
    {}

private:

    MSObserver(const MSObserver&);
    MSObserver& operator=(const MSObserver&);

};


#endif

/****************************************************************************/

