#ifndef UIntFunction2DoubleBinding_H
#define UIntFunction2DoubleBinding_H
/***************************************************************************
                          UIntFunction2DoubleBinding.h
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

#include <string>
#include <utils/convert/TplConvert.h>
#include "DoubleValueSource.h"

template< class T >
class UIntFunction2DoubleBinding : public DoubleValueSource
{
public:
    /// Type of the function to execute.
    typedef size_t ( T::* Operation )() const;

    UIntFunction2DoubleBinding( T* source, Operation operation ) :
        mySource( source ),
        myOperation( operation )
        {}

    /// Destructor.
    ~UIntFunction2DoubleBinding()
        {}

    double getValue() const
        {
            return (double) ( mySource->*myOperation )();
        }

    DoubleValueSource *copy() const {
        return new UIntFunction2DoubleBinding<T>(mySource, myOperation);
    }


protected:

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};

#endif // UIntFunction2DoubleBinding_H

// Local Variables:
// mode:C++
// End:
