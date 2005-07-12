
#ifndef CastingFunctionBinding_H
#define CastingFunctionBinding_H
//---------------------------------------------------------------------------//
//                        CastingFunctionBinding.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <helpers/ValueSource.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
template< class _T, typename _R, typename _O  >
class CastingFunctionBinding : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _O ( _T::* Operation )() const;

    CastingFunctionBinding( _T* source, Operation operation ) :
        mySource( source ),
        myOperation( operation )
        {}

    /// Destructor.
    ~CastingFunctionBinding()
        {}

    _R getValue() const
        {
            return (_R) ( mySource->*myOperation )();
        }

    ValueSource<_R> *copy() const {
        return new CastingFunctionBinding<_T, _R, _O>(mySource, myOperation);
    }

protected:

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};

#endif // CastingFunctionBinding_H

// Local Variables:
// mode:C++
// End:
