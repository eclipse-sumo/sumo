#ifndef UIntParametrisedDblFuncBinding_H
#define UIntParametrisedDblFuncBinding_H
/***************************************************************************
                          UIntParametrisedDblFuncBinding.h
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

#include "DoubleValueSource.h"

template< class T  >
class UIntParametrisedDblFuncBinding : public DoubleValueSource
{
public:
    /// Type of the function to execute.
    typedef double ( T::* Operation )(size_t) const;

    UIntParametrisedDblFuncBinding( T* source, Operation operation,
                size_t param )
                :
        mySource( source ),
        myOperation( operation ),
        myParam(param)
        {}

    /// Destructor.
    ~UIntParametrisedDblFuncBinding()
        {}

    double getValue() const
        {
            return ( mySource->*myOperation )(myParam);
        }

    DoubleValueSource *copy() const {
        return new UIntParametrisedDblFuncBinding<T>(
            mySource, myOperation, myParam);
    }


protected:

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    size_t myParam;

};

#endif // UIntParametrisedDblFuncBinding_H

// Local Variables:
// mode:C++
// End:
