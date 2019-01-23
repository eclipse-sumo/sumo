/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    FuncBinding_IntParam.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// �Function type template
/****************************************************************************/
#ifndef FuncBinding_IntParam_h
#define FuncBinding_IntParam_h



// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FuncBinding_IntParam
 */
template< class T, typename R  >
class FuncBinding_IntParam : public ValueSource<R> {
public:
    /// Type of the function to execute.
    typedef R(T::* Operation)(int) const;

    FuncBinding_IntParam(T* source, Operation operation,
                         int param)
        :
        mySource(source),
        myOperation(operation),
        myParam(param) {}

    /// Destructor.
    ~FuncBinding_IntParam() {}

    double getValue() const {
        return (mySource->*myOperation)(myParam);
    }

    ValueSource<R>* copy() const {
        return new FuncBinding_IntParam<T, R>(
                   mySource, myOperation, myParam);
    }

    ValueSource<double>* makedoubleReturningCopy() const {
        return new FuncBinding_IntParam<T, double>(mySource, myOperation, myParam);
    }

protected:

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    int myParam;

};


#endif

/****************************************************************************/

