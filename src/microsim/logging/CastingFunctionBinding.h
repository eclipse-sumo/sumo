/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#ifndef CastingFunctionBinding_h
#define CastingFunctionBinding_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

    ValueSource<double>* makedoubleReturningCopy() const {
        return new CastingFunctionBinding<T, double, O>(mySource, myOperation, myScale);
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

