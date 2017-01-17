/****************************************************************************/
/// @file    CastingFunctionBinding.h
/// @author  Daniel Krajzewicz
/// @author  Christian Roessel
/// @author  Sascha Krieg
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//  Function type template
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef CastingFunctionBinding_h
#define CastingFunctionBinding_h


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
 * @class CastingFunctionBinding
 */
template< class T, typename R, typename O  >
class CastingFunctionBinding : public ValueSource<R> {
public:
    /// Type of the function to execute.
    typedef O(T::* Operation)() const;

    CastingFunctionBinding(T* source, Operation operation, const R scale = 1) :
        mySource(source),
        myOperation(operation),
        myScale(scale) {}

    /// Destructor.
    ~CastingFunctionBinding() {}

    R getValue() const {
        return myScale * (R)(mySource->*myOperation)();
    }

    ValueSource<R>* copy() const {
        return new CastingFunctionBinding<T, R, O>(mySource, myOperation, myScale);
    }

    ValueSource<SUMOReal>* makeSUMORealReturningCopy() const {
        return new CastingFunctionBinding<T, SUMOReal, O>(mySource, myOperation, myScale);
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
    CastingFunctionBinding<T, R, O>& operator=(const CastingFunctionBinding<T, R, O>&);

};


#endif

/****************************************************************************/

