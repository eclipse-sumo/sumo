/****************************************************************************/
/// @file    MsgHandler.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Retrieves messages about the process and gives them further to output
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2003-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MsgHandler_h
#define MsgHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <iostream>


// ===========================================================================
// class declarations
// ===========================================================================
class AbstractMutex;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * MsgHandler
 */
class MsgHandler {
public:
    /**
     * @enum MsgType
     * An enumeration to differ between different types of messages
     * (errors, warning and information)
     */
    enum MsgType {
        /// The message is only something to show
        MT_MESSAGE,
        /// The message is a warning
        MT_WARNING,
        /// The message is an error
        MT_ERROR
    };

    /// Returns the instance to add normal messages to
    static MsgHandler* getMessageInstance();

    /// Returns the instance to add warnings to
    static MsgHandler* getWarningInstance();

    /// Returns the instance to add errors to
    static MsgHandler* getErrorInstance();

    static void initOutputOptions();

    /// Removes pending handler
    static void cleanupOnEnd();

    /// adds a new error to the list
    void inform(std::string msg, bool addType = true);

    /** @brief Begins a process information
     *
     * When a longer action is started, this method should be used to inform the user about it.
     * There will be no newline printed, but the message handler will be informed that
     *  a process message has been begun. If an error occurs, a newline will be printed.
     * After the action has been performed, use endProcessMsg to inform the user about it.
     */
    void beginProcessMsg(std::string msg, bool addType = true);

    /// Ends a process information
    void endProcessMsg(std::string msg);

    /// Clears information whether an error occured previously
    void clear();

    /// Adds a further retriever to the instance responsible for a certain msg type
    void addRetriever(OutputDevice* retriever);

    /// Removes the retriever from the handler
    void removeRetriever(OutputDevice* retriever);

    /// Returns whether the given output device retrieves messages from the handler
    bool isRetriever(OutputDevice* retriever) const;

    /// Returns the information whether any messages were added
    bool wasInformed() const;

    /** @brief Sets the lock to use
        The lock will not be deleted */
    static void assignLock(AbstractMutex* lock);

    /** @brief Generic output operator
     * @return The MsgHandler for further processing
     */
    template <class T>
    MsgHandler& operator<<(const T& t) {
        // inform all other receivers
        for (RetrieverVector::iterator i = myRetrievers.begin(); i != myRetrievers.end(); i++) {
            (*(*i)) << t;
        }
        return *this;
    }

protected:
    /// Builds the string which includes the mml-message type
    inline std::string build(const std::string& msg, bool addType) {
        if (addType) {
            switch (myType) {
                case MT_MESSAGE:
                    break;
                case MT_WARNING:
                    return "Warning: " + msg;
                    break;
                case MT_ERROR:
                    return "Error: " + msg;
                    break;
                default:
                    break;
            }
        }
        return msg;
    }


private:
    /// standard constructor
    MsgHandler(MsgType type);

    /// destructor
    ~MsgHandler();

private:
    /// The instance to handle errors
    static MsgHandler* myErrorInstance;

    /// The instance to handle warnings
    static MsgHandler* myWarningInstance;

    /// The instance to handle normal messages
    static MsgHandler* myMessageInstance;

    /// Information whether a process information is printed to cout
    static bool myAmProcessingProcess;

    /** @brief The lock if any has to be used
        The lock will not be deleted */
    static AbstractMutex* myLock;

private:
    /// The type of the instance
    MsgType myType;

    /// information wehther an error occured at all
    bool myWasInformed;

    /// Definition of the list of retrievers to inform
    typedef std::vector<OutputDevice*> RetrieverVector;

    /// The list of retrievers that shall be informed about new messages or errors
    RetrieverVector myRetrievers;

private:
    /** invalid copy constructor */
    MsgHandler(const MsgHandler& s);

    /** invalid assignment operator */
    MsgHandler& operator=(const MsgHandler& s);

};


// ===========================================================================
// global definitions
// ===========================================================================
#define WRITE_WARNING(msg) MsgHandler::getWarningInstance()->inform(msg);
#define WRITE_MESSAGE(msg) MsgHandler::getMessageInstance()->inform(msg);
#define PROGRESS_BEGIN_MESSAGE(msg) MsgHandler::getMessageInstance()->beginProcessMsg((msg) + std::string("..."));
#define PROGRESS_DONE_MESSAGE() MsgHandler::getMessageInstance()->endProcessMsg("done.");
#define PROGRESS_FAILED_MESSAGE() MsgHandler::getMessageInstance()->endProcessMsg("failed.");
#define WRITE_ERROR(msg)   MsgHandler::getErrorInstance()->inform(msg);

#endif

/****************************************************************************/

