#ifndef DoubleFunctionBinding_H
#define DoubleFunctionBinding_H
/***************************************************************************
                          DoubleFunctionBinding.h
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
class DoubleFunctionBinding : public DoubleValueSource
{
public:
    /// Type of the function to execute.
    typedef double ( T::* Operation )() const;

    DoubleFunctionBinding( T* source, Operation operation ) :
        mySource( source ),
        myOperation( operation )
        {}

    /// Destructor.
    ~DoubleFunctionBinding()
        {}

    double getValue() const
        {
            return ( mySource->*myOperation )();
        }

    DoubleValueSource *copy() const {
        return new DoubleFunctionBinding<T>(mySource, myOperation);
    }

protected:

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};

#endif // DoubleFunctionBinding_H

// Local Variables:
// mode:C++
// End:
