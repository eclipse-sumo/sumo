/****************************************************************************/
/// @file    MsgRetrievingFunction.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 24 Oct 2003
/// @version $Id$
///
// Encapsulates an object's method in order to use it as a MsgRetriever
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
#ifndef MsgRetrievingFunction_h
#define MsgRetrievingFunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "MsgRetriever.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MsgRetrievingFunction
 * A class that encapsulates an object with a given method to be usable as
 *  a retriever of messages.
 */
template< class T >
class MsgRetrievingFunction : public MsgRetriever
{
public:
    /// Type of the function to execute.
    typedef void(T::* Operation)(const std::string &);

    /// Constructor
    MsgRetrievingFunction(T* object, Operation operation) :
            myObject(object),
            myOperation(operation) {}

    /// Destructor.
    ~MsgRetrievingFunction() {}

    /// Called to inform the object about a new message
    void inform(const std::string &msg) {
        (myObject->*myOperation)(msg);
    }

private:
    /// The object the action is directed to.
    T* myObject;

    /// The object's operation to perform.
    Operation myOperation;

};


#endif

/****************************************************************************/

