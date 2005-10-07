
#ifndef FunctionBinding_H
#define FunctionBinding_H
//---------------------------------------------------------------------------//
//                        FunctionBinding.h -
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
// Revision 1.6  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 11:07:54  dkrajzew
// LARGE CODE RECHECK
//
//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/helpers/ValueSource.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template< class _T, typename _R  >
class FunctionBinding : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _R ( _T::* Operation )() const;

    FunctionBinding( _T* source, Operation operation ) :
        mySource( source ),
        myOperation( operation )
        {}

    /// Destructor.
    ~FunctionBinding()
        {}

    _R getValue() const
        {
            return ( mySource->*myOperation )();
        }

    ValueSource<_R> *copy() const {
        return new FunctionBinding<_T, _R>(mySource, myOperation);
    }

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};

#endif // FunctionBinding_H

// Local Variables:
// mode:C++
// End:
