
#ifndef CastingFunctionBinding_H
#define CastingFunctionBinding_H
/***************************************************************************
                          CastingFunctionBinding.h
                             -------------------
    begin                :
    copyright            : (C) 2001 by
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <helpers/ValueSource.h>

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
