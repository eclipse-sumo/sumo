/****************************************************************************/
/// @file    FunctionBinding.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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

#include <utils/helpers/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template< class _T, typename _R  >
class FunctionBinding : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _R(_T::* Operation)() const;

    FunctionBinding(_T* source, Operation operation) :
            mySource(source),
            myOperation(operation)
    {}

    /// Destructor.
    ~FunctionBinding()
    {}

    _R getValue() const
    {
        return (mySource->*myOperation)();
    }

    ValueSource<_R> *copy() const
    {
        return new FunctionBinding<_T, _R>(mySource, myOperation);
    }

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};


#endif

/****************************************************************************/

