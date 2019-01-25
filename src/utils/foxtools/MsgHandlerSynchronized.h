/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2003-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MsgHandlerSynchronized.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Retrieves messages about the process and gives them further to output
/****************************************************************************/
#ifndef MsgHandlerSynchronized_h
#define MsgHandlerSynchronized_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#ifdef HAVE_FOX
#include <fx.h>
#endif
#include <string>
#include <vector>
#include <iostream>
#include <utils/common/MsgHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * MsgHandlerSynchronized
 */
class MsgHandlerSynchronized : public MsgHandler {
public:
    static MsgHandler* create(MsgType type) {
        return new MsgHandlerSynchronized(type);
    }

    /// @brief adds a new error to the list
    void inform(std::string msg, bool addType = true);

    /** @brief Begins a process information
     *
     * When a longer action is started, this method should be used to inform the user about it.
     * There will be no newline printed, but the message handler will be informed that
     *  a process message has been begun. If an error occurs, a newline will be printed.
     * After the action has been performed, use endProcessMsg to inform the user about it.
     */
    void beginProcessMsg(std::string msg, bool addType = true);

    /// @brief Ends a process information
    void endProcessMsg(std::string msg);

    /// @brief Clears information whether an error occurred previously
    void clear();

    /// @brief Adds a further retriever to the instance responsible for a certain msg type
    void addRetriever(OutputDevice* retriever);

    /// @brief Removes the retriever from the handler
    void removeRetriever(OutputDevice* retriever);

private:
    /// @brief standard constructor
    MsgHandlerSynchronized(MsgType type);

    /// @brief destructor
    ~MsgHandlerSynchronized();

    /// @brief The lock for synchronizing all outputs using handlers of this class
    static FXMutex myLock;

private:
    /// @brief invalid copy constructor
    MsgHandlerSynchronized(const MsgHandlerSynchronized& s) = delete;

    /// @brief invalid assignment operator
    MsgHandlerSynchronized& operator=(const MsgHandlerSynchronized& s) = delete;
};

#endif

/****************************************************************************/
