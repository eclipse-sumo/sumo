/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    CastingFunctionBinding_Param.h
/// @author  Daniel Krajzewicz
/// @author  Christian Roessel
/// @author  Sascha Krieg
/// @date    Fri, 29.04.2005
///
//  Function type template
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CastingFunctionBinding_Param
 */
template< class T, typename R, typename O, typename P >
class CastingFunctionBinding_Param : public ValueSource<R> {
public:
    /// Type of the function to execute.
    typedef O(T::* Operation)(P) const;

    CastingFunctionBinding_Param(T* source, Operation operation, P param, const R scale = 1.0) :
        mySource(source),
        myOperation(operation),
        myScale(scale),
        myParam(param)
    {}

    /// Destructor.
    ~CastingFunctionBinding_Param() {}

    R getValue() const {
        return myScale * (R)(mySource->*myOperation)(myParam);
    }

    ValueSource<R>* copy() const {
        return new CastingFunctionBinding_Param<T, R, O, P>(mySource, myOperation, myParam, myScale);
    }

    ValueSource<double>* makedoubleReturningCopy() const {
        return new CastingFunctionBinding_Param<T, double, O, P>(mySource, myOperation, myParam, myScale);
    }

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    /// The scale to apply.
    const R myScale;

    /// The parameter to apply to the operation.
    const P myParam;

private:
    /// @brief invalidated assignment operator
    CastingFunctionBinding_Param<T, R, O, P>& operator=(const CastingFunctionBinding_Param<T, R, O, P>&);

};
