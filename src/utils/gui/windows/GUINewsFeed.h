/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
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
/// @file    GUINewsFeed.h
/// @author  Michael Behrisch
/// @date    2026-11-20
///
// Fetches the SUMO release news RSS feed in the background (sumo-gui, netedit)
/****************************************************************************/
#pragma once
#include <config.h>

#include <atomic>
#include <ctime>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <utils/foxtools/fxheader.h>
#include <utils/foxtools/MFXThreadEvent.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUINewsFeed
 * @brief Periodically fetches an RSS feed with news about upcoming releases
 *
 * The download runs in a separate thread, the GUI thread is notified via an
 * MFXThreadEvent. The time of the last successful access is stored in the FOX
 * registry (section "NEWS", key "lastaccess", seconds since the epoch).
 * The automatic check can be disabled by setting "lastaccess" to a time far in the future.
 * Explicit requests via showLatest() always work and do not modify "lastaccess".
 */
class GUINewsFeed : public FXObject {
    FXDECLARE(GUINewsFeed)

public:
    /// @brief a single entry of the feed
    struct Item {
        std::string title;
        std::string link;
        std::string description;
        /// @brief publication time (seconds since epoch), 0 if unknown
        time_t published = 0;
    };

    /// @brief message ids
    enum {
        ID_FETCHED = 1,
        ID_SHOW,
        ID_LAST
    };

    /// @brief default feed location
    static const std::string DEFAULT_URL;

    /// @brief minimum time between two accesses (seconds)
    static const time_t MIN_INTERVAL;

    /** @brief constructor
     * @param[in] app the application whose registry is used
     * @param[in] owner the window used as parent of the news dialog
     * @param[in] statusBar the status bar receiving the news button (if null the dialog is shown directly)
     * @param[in] url the feed address
     */
    GUINewsFeed(FXApp* app, FXWindow* owner, FXStatusBar* statusBar, const std::string& url = DEFAULT_URL);

    /// @brief destructor, aborts a running download
    ~GUINewsFeed();

    /** @brief starts a download if enabled, curl is available and the last access is long enough ago
     * @return whether a download was started
     */
    bool startCheck(const bool force = false);

    /// @brief shows the latest items, downloading them first if necessary
    void showLatest();

    /// @brief time of the last successful access (0 if never)
    time_t getLastAccess() const;

    /// @brief extracts the items of an RSS document
    static std::vector<Item> parseRSS(const std::string& xml);

    /// @brief called in the GUI thread when the download has finished
    long onFetched(FXObject*, FXSelector, void*);

    /// @brief called when the news button was clicked, shows the latest items
    long onCmdShow(FXObject*, FXSelector, void*);

protected:
    FOX_CONSTRUCTOR(GUINewsFeed)

private:
    /// @brief the download, executed in myThread
    void fetch();

    /// @brief the application (registry access)
    FXApp* myApp = nullptr;

    /// @brief parent of the news dialog
    FXWindow* myOwner = nullptr;

    /// @brief the status bar holding the news button
    FXStatusBar* myStatusBar = nullptr;

    /// @brief the button in the status bar (null while there is nothing new)
    FXButton* myButton = nullptr;

    /// @brief the text of the latest items (empty until a download succeeded)
    std::string myNewsText;

    /// @brief whether the dialog should open as soon as the running download finishes
    bool myShowAfterFetch = false;

    /// @brief the feed address
    std::string myURL;

    /// @brief the worker thread
    std::thread myThread;

    /// @brief tells the worker thread to give up
    std::atomic<bool> myAbort;

    /// @brief wakes the GUI thread
    FXEX::MFXThreadEvent myThreadEvent;

    /// @brief protects myDocument and myOK
    std::mutex myMutex;

    /// @brief the downloaded document
    std::string myDocument;

    /// @brief whether the download succeeded
    bool myOK = false;

    /// @brief the last access time before this download started
    time_t myPreviousAccess = 0;

    /// @brief invalidated copy constructor
    GUINewsFeed(const GUINewsFeed&) = delete;

    /// @brief invalidated assignment operator
    GUINewsFeed& operator=(const GUINewsFeed&) = delete;
};
