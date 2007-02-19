/****************************************************************************/
/// @file    FuncBinding_StringParam.h
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
#ifndef FuncBinding_StringParam_h
#define FuncBinding_StringParam_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
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
class FuncBinding_StringParam : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _R(_T::* Operation)(const std::string &) const;

    FuncBinding_StringParam(_T* source, Operation operation,
                            const std::string &param)
            :
            mySource(source),
            myOperation(operation),
            myParam(param)
    {}

    /// Destructor.
    ~FuncBinding_StringParam()
    {}

    _R getValue() const
    {
        return (mySource->*myOperation)(myParam);
    }

    ValueSource<_R> *copy() const
    {
        return new FuncBinding_StringParam<_T, _R>(
                   mySource, myOperation, myParam);
    }


protected:

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    std::string myParam;

};


#endif

/****************************************************************************/

