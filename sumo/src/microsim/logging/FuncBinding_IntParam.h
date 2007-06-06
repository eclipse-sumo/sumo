/****************************************************************************/
/// @file    FuncBinding_IntParam.h
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

#include <utils/helpers/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template< class _T, typename _R  >
class FuncBinding_IntParam : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _R(_T::* Operation)(int) const;

    FuncBinding_IntParam(_T* source, Operation operation,
                         size_t param)
            :
            mySource(source),
            myOperation(operation),
            myParam(param)
    {}

    /// Destructor.
    ~FuncBinding_IntParam()
    {}

    SUMOReal getValue() const
    {
        return (mySource->*myOperation)(myParam);
    }

    ValueSource<_R> *copy() const
    {
        return new FuncBinding_IntParam<_T, _R>(
                   mySource, myOperation, myParam);
    }


protected:

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    int myParam;

};


#endif

/****************************************************************************/

