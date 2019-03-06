/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MsgRetrievingFunction.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 24 Oct 2003
/// @version $Id$
///
// Encapsulates an object's method for using it as a message retriever
/****************************************************************************/
#ifndef MsgRetrievingFunction_h
#define MsgRetrievingFunction_h


// ===========================================================================
// included modules
// ===========================================================================

#include <string>
#include <sstream>
#include <utils/iodevices/OutputDevice.h>
#include "MsgHandler.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MsgRetrievingFunction
 * @brief Encapsulates an object's method for using it as a message retriever
 *
 * You may find an example for this class' usage in GUIRunThread.
 */
template< class T >
class MsgRetrievingFunction : public OutputDevice {
public:
    /// @brief Type of the function to execute.
    typedef void(T::* Operation)(const MsgHandler::MsgType, const std::string&);


    /** @brief Constructor
     * @param[in] object The object to call the method of
     * @param[in] operation The method to call
     * @param[in] type The type of the message
     */
    MsgRetrievingFunction(T* object, Operation operation, MsgHandler::MsgType type) :
        myObject(object),
        myOperation(operation),
        myMsgType(type) {}


    /// @brief Destructor
    ~MsgRetrievingFunction() {}


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
    std::ostream& getOStream() {
        return myMessage;
    }


    /** @brief Sends the data which was written to the string stream via the retrieving function.
     */
    virtual void postWriteHook() {
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

