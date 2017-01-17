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
#ifndef FuncBinding_IntParam_h
#define FuncBinding_IntParam_h



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

    SUMOReal getValue() const {
        return (mySource->*myOperation)(myParam);
    }

    ValueSource<R>* copy() const {
        return new FuncBinding_IntParam<T, R>(
                   mySource, myOperation, myParam);
    }

    ValueSource<SUMOReal>* makeSUMORealReturningCopy() const {
        return new FuncBinding_IntParam<T, SUMOReal>(mySource, myOperation, myParam);
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

