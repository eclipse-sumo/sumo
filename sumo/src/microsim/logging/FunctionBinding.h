/****************************************************************************/
/// @file    FunctionBinding.h
/// @author  Daniel Krajzewicz
/// @author  Christian Roessel
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	»missingDescription«
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

    FunctionBinding(T* const source, Operation operation, const R scale = 1) :
        mySource(source),
        myOperation(operation),
        myScale(scale) {}

    /// Destructor.
    ~FunctionBinding() {}

    R getValue() const {
        return myScale * (mySource->*myOperation)();
    }

    ValueSource<R>* copy() const {
        return new FunctionBinding<T, R>(mySource, myOperation, myScale);
    }

    ValueSource<SUMOReal>* makeSUMORealReturningCopy() const {
        return new CastingFunctionBinding<T, SUMOReal, R>(mySource, myOperation, myScale);
    }

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    /// The scale to apply.
    const R myScale;

private:
    /// @brief invalidated assignment operator
    FunctionBinding<T, R>& operator=(const FunctionBinding<T, R>&);

};


#endif

/****************************************************************************/

