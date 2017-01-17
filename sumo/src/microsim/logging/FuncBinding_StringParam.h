/****************************************************************************/
/// @file    FuncBinding_StringParam.h
/// @author  Daniel Krajzewicz
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
#ifndef FuncBinding_StringParam_h
#define FuncBinding_StringParam_h


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
 * @class FuncBinding_StringParam
 */
template< class T, typename R  >
class FuncBinding_StringParam : public ValueSource<R> {
public:
    /// Type of the function to execute.
    typedef R(T::* Operation)(const std::string&) const;

    FuncBinding_StringParam(T* source, Operation operation,
                            const std::string& param)
        :
        mySource(source),
        myOperation(operation),
        myParam(param) {}

    /// Destructor.
    ~FuncBinding_StringParam() {}

    R getValue() const {
        return (mySource->*myOperation)(myParam);
    }

    ValueSource<R>* copy() const {
        return new FuncBinding_StringParam<T, R>(
                   mySource, myOperation, myParam);
    }

    ValueSource<SUMOReal>* makeSUMORealReturningCopy() const {
        throw 1;
    }


protected:

private:
    /// The object the action is directed to.
    T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    std::string myParam;

};


#endif

/****************************************************************************/

