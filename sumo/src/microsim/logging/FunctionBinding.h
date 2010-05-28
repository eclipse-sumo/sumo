/****************************************************************************/
/// @file    FunctionBinding.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	»missingDescription«
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include "CastingFunctionBinding.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FunctionBinding
 */
template< class T, typename R  >
class FunctionBinding : public ValueSource<R> {
public:
    /// Type of the function to execute.
    typedef R(T::* Operation)() const;

    FunctionBinding(T* const source, Operation operation) :
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

    ValueSource<SUMOReal> *makeSUMORealReturningCopy() const {
        return new CastingFunctionBinding<T, SUMOReal, R>(mySource, myOperation);
    }

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};


#endif

/****************************************************************************/

