/****************************************************************************/
/// @file    CastingFunctionBinding.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
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
#ifndef CastingFunctionBinding_h
#define CastingFunctionBinding_h
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
template< class _T, typename _R, typename _O  >
class CastingFunctionBinding : public ValueSource<_R>
{
public:
    /// Type of the function to execute.
    typedef _O(_T::* Operation)() const;

    CastingFunctionBinding(_T* source, Operation operation) :
            mySource(source),
            myOperation(operation)
    {}

    /// Destructor.
    ~CastingFunctionBinding()
    {}

    _R getValue() const
    {
        return (_R)(mySource->*myOperation)();
    }

    ValueSource<_R> *copy() const
    {
        return new CastingFunctionBinding<_T, _R, _O>(mySource, myOperation);
    }

protected:

private:
    /// The object the action is directed to.
    _T* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};


#endif

/****************************************************************************/

