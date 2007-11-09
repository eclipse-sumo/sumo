/****************************************************************************/
/// @file    FunctionBinding.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	»missingDescription«
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
#ifndef FunctionBinding_h
#define FunctionBinding_h



// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FunctionBinding
 */
template< class T, typename R  >
class FunctionBinding : public ValueSource<R>
{
public:
    /// Type of the function to execute.
    typedef R(T::* Operation)() const;

    FunctionBinding(T* source, Operation operation) :
            mySource(source),
            myOperation(operation) {}

    /// Destructor.
    ~FunctionBinding() {}

    R getValue() const {
        return (mySource->*myOperation)();
    }

    ValueSource<R> *copy() const {
        return new FunctionBinding<T, R>(mySource, myOperation);
    }

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};


#endif

/****************************************************************************/

