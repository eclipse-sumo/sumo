/****************************************************************************/
/// @file    FuncBinding_StringParam.h
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

#include <utils/helpers/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FuncBinding_StringParam
 */
template< class myT, typename myR  >
class FuncBinding_StringParam : public ValueSource<myR>
{
public:
    /// Type of the function to execute.
    typedef myR(myT::* Operation)(const std::string &) const;

    FuncBinding_StringParam(myT* source, Operation operation,
                            const std::string &param)
            :
            mySource(source),
            myOperation(operation),
            myParam(param)
    {}

    /// Destructor.
    ~FuncBinding_StringParam()
    {}

    myR getValue() const
    {
        return (mySource->*myOperation)(myParam);
    }

    ValueSource<myR> *copy() const
    {
        return new FuncBinding_StringParam<myT, myR>(
                   mySource, myOperation, myParam);
    }


protected:

private:
    /// The object the action is directed to.
    myT* mySource;

    /// The object's operation to perform.
    Operation myOperation;

    std::string myParam;

};


#endif

/****************************************************************************/

