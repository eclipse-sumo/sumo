
#ifndef FuncBinding_UIntParam_H
#define FuncBinding_UIntParam_H
/***************************************************************************
                          FuncBinding_UIntParam.h
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


template< class _T, typename _R  >
class FuncBinding_UIntParam : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _R ( _T::* Operation )(size_t) const;

    FuncBinding_UIntParam( _T* source, Operation operation,
                size_t param )
                :
        mySource( source ),
        myOperation( operation ),
        myParam(param)
        {}

    /// Destructor.
    ~FuncBinding_UIntParam()
        {}

    double getValue() const
        {
            return ( mySource->*myOperation )(myParam);
        }

    ValueSource<_R> *copy() const {
        return new FuncBinding_UIntParam<_T, _R>(
            mySource, myOperation, myParam);
    }


protected:

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    size_t myParam;

};

#endif // FuncBinding_UIntParam_H

// Local Variables:
// mode:C++
// End:
