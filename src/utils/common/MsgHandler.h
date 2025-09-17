/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2003-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MsgHandler.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Mirko Barthauer
/// @date    Tue, 17 Jun 2003
///
// Retrieves messages about the process and gives them further to output
/****************************************************************************/
#pragma once
#include <config.h>
#include <string>
#include <vector>
#include <map>
#include <utils/common/StringUtils.h>
#include <utils/common/Translation.h>
#include <utils/iodevices/OutputDevice.h>


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
    enum class MsgType {
        /// The message is only something to show
        MT_MESSAGE,
        /// The message is a warning
        MT_WARNING,
        /// The message is an error
        MT_ERROR,
        /// The message is debug output
        MT_DEBUG,
        /// The message is GL debug output
        MT_GLDEBUG
    };

private:
    typedef MsgHandler* (*Factory)(MsgType);

public:
    /// @brief Sets the factory function to use for new MsgHandlers
    static void setFactory(Factory func) {
        // clean old instances
        cleanupOnEnd();
        myFactory = func;
    }

    /// @brief Returns the instance to add normal messages to
    static MsgHandler* getMessageInstance();

    /// @brief Returns the instance to add warnings to
    static MsgHandler* getWarningInstance();

    /// @brief Returns the instance to add errors to
    static MsgHandler* getErrorInstance();

    /// @brief enable/disable debug messages
    static void enableDebugMessages(bool enable);

    /// @brief enable/disable gl-debug messages
    static void enableDebugGLMessages(bool enable);

    /// @brief check whether to enable/disable debug messages
    static inline bool writeDebugMessages() {
        return myWriteDebugMessages;
    }

    /// @brief check whether to enable/disable gl-debug messages
    static inline bool writeDebugGLMessages() {
        return myWriteDebugGLMessages;
    }

    /// @brief reformats a long string to contain newline after a certain line length in px (depending on the current font)
    static std::string insertLineBreaks(std::string msg, int lineWidth);

    /// @brief ensure that that given output device is no longer used as retriever by any instance
    static void removeRetrieverFromAllInstances(OutputDevice* out);

    ///@brief set up gettext stuff
    static void setupI18n(const std::string& locale = "");

    ///@brief init output options
    static void initOutputOptions();

    /// @brief Removes pending handler
    static void cleanupOnEnd();

    /// @brief adds a new error to the list
    virtual void inform(std::string msg, bool addType = true);

    /// @brief adds a new formatted message
    // variadic function
    template<typename T, typename... Targs>
    void informf(const std::string& format, T value, Targs... Fargs) {
        if (!aggregationThresholdReached(format)) {
            inform(StringUtils::format(format, value, Fargs...), true);
        }
    }

    /** @brief Begins a process information
     *
     * When a longer action is started, this method should be used to inform the user about it.
     * There will be no newline printed, but the message handler will be informed that
     *  a process message has been begun. If an error occurs, a newline will be printed.
     * After the action has been performed, use endProcessMsg to inform the user about it.
     */
    virtual void beginProcessMsg(std::string msg, bool addType = true);

    /// @brief Ends a process information with predefined messages
    virtual void endProcessMsg2(bool success, long duration = -1);

    /// @brief Ends a process information
    virtual void endProcessMsg(std::string msg);

    /// @brief Clears information whether an error occurred previously and print aggregated message summary
    virtual void clear(bool resetInformed = true);

    /// @brief Adds a further retriever to the instance responsible for a certain msg type
    virtual void addRetriever(OutputDevice* retriever);

    /// @brief Removes the retriever from the handler
    virtual void removeRetriever(OutputDevice* retriever);

    /// @brief Returns whether the given output device retrieves messages from the handler
    bool isRetriever(OutputDevice* retriever) const;

    /// @brief Returns the information whether any messages were added
    bool wasInformed() const;

    /** @brief Generic output operator
     * @return The MsgHandler for further processing
     */
    template <class T>
    MsgHandler& operator<<(const T& t) {
        // inform all other receivers
        for (OutputDevice* o : myRetrievers) {
            (*o) << t;
        }
        return *this;
    }

    void setAggregationThreshold(const int thresh) {
        myAggregationThreshold = thresh;
    }

    int getAggregationThreshold() const {
        return myAggregationThreshold;
    }

protected:

    std::string buildProcessIdPrefix() const;

    /// @brief Builds the string which includes the mml-message type
    inline std::string build(const std::string& msg, bool addType) {
        std::string prefix;
        if (myWriteTimestamps) {
            prefix += "[" + StringUtils::isoTimeString() + "] ";
        }
        if (myWriteProcessId) {
            prefix += buildProcessIdPrefix();
        }
        if (addType) {
            switch (myType) {
                case MsgType::MT_MESSAGE:
                    break;
                case MsgType::MT_WARNING:
                    prefix += myWarningPrefix;
                    break;
                case MsgType::MT_ERROR:
                    prefix += myErrorPrefix;
                    break;
                case MsgType::MT_DEBUG:
                    prefix += "Debug: ";
                    break;
                case MsgType::MT_GLDEBUG:
                    prefix += "GLDebug: ";
                    break;
                default:
                    break;
            }
        }
        return prefix + msg;
    }

    virtual bool aggregationThresholdReached(const std::string& format) {
        return myAggregationThreshold >= 0 && myAggregationCount[format]++ >= myAggregationThreshold;
    }

    /// @brief standard constructor
    MsgHandler(MsgType type);

    /// @brief destructor
    virtual ~MsgHandler();

private:
    /// @brief The function to call for new MsgHandlers, nullptr means use default constructor
    static Factory myFactory;

    /// @brief The instance to handle errors
    static MsgHandler* myErrorInstance;

    /// @brief The instance to handle warnings
    static MsgHandler* myWarningInstance;

    /// @brief The instance to handle normal messages
    static MsgHandler* myMessageInstance;

    /// @brief Information whether a process information is printed to cout
    static bool myAmProcessingProcess;

private:
    /// @brief The type of the instance
    MsgType myType;

    /// @brief information whether an output occurred at all
    bool myWasInformed;

    /// @brief do not output more messages of the same type if the count exceeds this threshold
    int myAggregationThreshold;

    /// @brief count for messages of the same type
    std::map<const std::string, int> myAggregationCount;

    /// @brief The list of retrievers that shall be informed about new messages or errors
    std::vector<OutputDevice*> myRetrievers;

    /// @brief storage for initial messages
    std::vector<std::string> myInitialMessages;

    /** @brief Flag to enable or disable debug output
     *
     * This value is used to show more internal information through warning messages about certain operations
     */
    static bool myWriteDebugMessages;

    /// @brief Flag to enable or disable GL specific debug output
    static bool myWriteDebugGLMessages;

    /// @brief Whether to prefix every message with a time stamp
    static bool myWriteTimestamps;

    /// @brief Whether to prefix every message with the process id
    static bool myWriteProcessId;

    /// @brief The possibly translated error prefix (mainly for speedup)
    static std::string myErrorPrefix;

    /// @brief The possibly translated warning prefix (mainly for speedup)
    static std::string myWarningPrefix;

private:
    /// @brief invalid copy constructor
    MsgHandler(const MsgHandler& s) = delete;

    /// @brief invalid assignment operator
    MsgHandler& operator=(const MsgHandler& s) = delete;
};


// ===========================================================================
// global definitions
// ===========================================================================
#define WRITE_WARNING(msg) MsgHandler::getWarningInstance()->inform(msg);
#define WRITE_WARNINGF(...) MsgHandler::getWarningInstance()->informf(__VA_ARGS__);
#define WRITE_MESSAGE(msg) MsgHandler::getMessageInstance()->inform(msg);
#define WRITE_MESSAGEF(...) MsgHandler::getMessageInstance()->informf(__VA_ARGS__);
#define PROGRESS_BEGIN_MESSAGE(msg) MsgHandler::getMessageInstance()->beginProcessMsg((msg) + std::string(" ..."));
#define PROGRESS_DONE_MESSAGE() MsgHandler::getMessageInstance()->endProcessMsg2(true);
#define PROGRESS_BEGIN_TIME_MESSAGE(msg) SysUtils::getCurrentMillis(); MsgHandler::getMessageInstance()->beginProcessMsg((msg) + std::string(" ..."));
#define PROGRESS_TIME_MESSAGE(before) MsgHandler::getMessageInstance()->endProcessMsg2(true, SysUtils::getCurrentMillis() - before);
#define PROGRESS_FAILED_MESSAGE() MsgHandler::getMessageInstance()->endProcessMsg2(false);
#define WRITE_ERROR(msg) MsgHandler::getErrorInstance()->inform(msg);
#define WRITE_ERRORF(...) MsgHandler::getErrorInstance()->informf(__VA_ARGS__);
#ifdef HAVE_INTL
// basic translation
#define TL(string) gettext(string)
// complex translation ("This % an %", "is", "example")
#define TLF(string, ...) StringUtils::format(gettext(string), __VA_ARGS__)
#else
// basic translation
#define TL(string) (string)
// complex translation ("This % an %", "is", "example")
#define TLF(string, ...) StringUtils::format(string, __VA_ARGS__)
#endif
