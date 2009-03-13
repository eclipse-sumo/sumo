/****************************************************************************/
/// @file    MsgRetrievingFunction.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 24 Oct 2003
/// @version $Id$
///
// Encapsulates an object's method for using it as a MsgRetriever
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <sstream>
#include <utils/iodevices/OutputDevice.h>
#include "MsgHandler.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MsgRetrievingFunction
 * @brief Encapsulates an object's method for using it as a MsgRetriever
 *
 * You may find an example for this class' usage in GUIRunThread.
 */
template< class T >
class MsgRetrievingFunction : public OutputDevice {
public:
    /// @brief Type of the function to execute.
    typedef void(T::* Operation)(const MsgHandler::MsgType, const std::string &);


    /** @brief Constructor
     *
     * @param[in] object The object to call the method of
     * @param[in] operation The method to call
     */
    MsgRetrievingFunction(T* object, Operation operation, MsgHandler::MsgType type) :
            myObject(object),
            myOperation(operation),
            myMsgType(type) {}


    /// @brief Destructor.
    ~MsgRetrievingFunction() throw() {}


protected:
    /// @name Methods that override/implement OutputDevice-methods
    /// @{

    /** @brief Returns the associated ostream
     *
     * The stream is an ostringstream, actually, into which the message
     *  is written. It is sent when postWriteHook is called.
     *
     * @return The used stream
     * @see postWriteHook
     */
    std::ostream &getOStream() throw() {
        return myMessage;
    }


    /** @brief Sends the data which was written to the string stream via the retrieving function.
     */
    virtual void postWriteHook() throw() {
        (myObject->*myOperation)(myMsgType, myMessage.str());
        myMessage.str("");
    }
    /// @}

private:
    /// @brief The object the action is directed to.
    T* myObject;

    /// @brief The object's operation to perform.
    Operation myOperation;

    /// @brief The type of message to retrieve.
    MsgHandler::MsgType myMsgType;

    /// @brief message buffer
    std::ostringstream myMessage;
};


#endif

/****************************************************************************/

