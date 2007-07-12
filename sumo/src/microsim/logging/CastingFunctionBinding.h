/****************************************************************************/
/// @file    CastingFunctionBinding.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//	»missingDescription«
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
 * @class CastingFunctionBinding
 */
template< class myT, typename myR, typename myO  >
class CastingFunctionBinding : public ValueSource<myR>
{
public:
    /// Type of the function to execute.
    typedef myO(myT::* Operation)() const;

    CastingFunctionBinding(myT* source, Operation operation) :
            mySource(source),
            myOperation(operation)
    {}

    /// Destructor.
    ~CastingFunctionBinding()
    {}

    myR getValue() const
    {
        return (myR)(mySource->*myOperation)();
    }

    ValueSource<myR> *copy() const
    {
        return new CastingFunctionBinding<myT, myR, myO>(mySource, myOperation);
    }

protected:

private:
    /// The object the action is directed to.
    myT* mySource;

    /// The object's operation to perform.
    Operation myOperation;
};


#endif

/****************************************************************************/

