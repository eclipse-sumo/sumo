#ifndef MsgRetrievingFunction_h
#define MsgRetrievingFunction_h
/***************************************************************************
                          MsgRetrievingFunction.h  -
    Encapsulates an object's method in order to use it as a MsgRetriever
                             -------------------
    project              : SUMO
    begin                : Mon, 24 Oct 2003
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.2  2004/11/23 10:27:45  dkrajzew
// debugging
//
// Revision 1.1  2003/11/26 09:51:10  dkrajzew
// changes to allow more than a single function of an object to retrieve messages
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "MsgRetriever.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MsgRetrievingFunction
 * A class that encapsulates an object with a given method to be usable as
 *  a retriever of messages.
 */
template< class _T >
class MsgRetrievingFunction : public MsgRetriever
{
public:
    /// Type of the function to execute.
    typedef void ( _T::* Operation )(const std::string &);

    /// Constructor
    MsgRetrievingFunction( _T* object, Operation operation ) :
        myObject( object ),
        myOperation( operation )
        {}

    /// Destructor.
    ~MsgRetrievingFunction()
        {}

    /// Called to inform the object about a new message
    void inform(const std::string &msg)
        {
            ( myObject->*myOperation )(msg);
        }

private:
    /// The object the action is directed to.
    _T* myObject;

    /// The object's operation to perform.
    Operation myOperation;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

