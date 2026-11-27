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
/// @file    GUINewsFeed.cpp
/// @author  Michael Behrisch
/// @date    2026-11-20
///
// Fetches the SUMO release news RSS feed in the background (sumo-gui, netedit)
/****************************************************************************/
#include <config.h>

#include <iomanip>
#include <sstream>
#ifdef HAVE_CURL
#include <curl/curl.h>
#endif

#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/Translation.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GUINewsFeed.h"


// ===========================================================================
// constants
// ===========================================================================
const std::string GUINewsFeed::DEFAULT_URL = "https://sumo.dlr.de/releases/news.xml";
const time_t GUINewsFeed::MIN_INTERVAL = 30 * 24 * 60 * 60;

#define NEWS_REG_SECTION "NEWS"
#define NEWS_MAX_SHOWN 5
#define NEWS_MAX_BYTES (1024 * 1024)


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUINewsFeed) GUINewsFeedMap[] = {
    FXMAPFUNC(FXEX::SEL_THREAD, GUINewsFeed::ID_FETCHED, GUINewsFeed::onFetched),
    FXMAPFUNC(SEL_COMMAND, GUINewsFeed::ID_SHOW, GUINewsFeed::onCmdShow),
};

FXIMPLEMENT(GUINewsFeed, FXObject, GUINewsFeedMap, ARRAYNUMBER(GUINewsFeedMap))


// ===========================================================================
// helpers
// ===========================================================================
namespace {
#ifdef HAVE_CURL
size_t
collect(char* data, size_t size, size_t nmemb, void* userdata) {
    std::string* result = static_cast<std::string*>(userdata);
    const size_t bytes = size * nmemb;
    if (result->size() + bytes > NEWS_MAX_BYTES) {
        return 0; // abort the transfer
    }
    result->append(data, bytes);
    return bytes;
}

int
checkAbort(void* userdata, curl_off_t, curl_off_t, curl_off_t, curl_off_t) {
    return static_cast<std::atomic<bool>*>(userdata)->load() ? 1 : 0;
}
#endif

/// @brief returns the text of the first <tag> element in xml (starting at pos), unwrapping CDATA
bool
getElement(const std::string& xml, const std::string& tag, size_t& pos, std::string& result) {
    const std::string open = "<" + tag;
    size_t start = pos;
    while ((start = xml.find(open, start)) != std::string::npos) {
        const size_t after = start + open.size();
        if (after < xml.size() && (xml[after] == '>' || xml[after] == ' ')) {
            break;
        }
        start = after;
    }
    if (start == std::string::npos) {
        return false;
    }
    const size_t contentStart = xml.find('>', start);
    const size_t end = xml.find("</" + tag + ">", contentStart);
    if (contentStart == std::string::npos || end == std::string::npos) {
        return false;
    }
    result = StringUtils::trim(xml.substr(contentStart + 1, end - contentStart - 1));
    if (result.compare(0, 9, "<![CDATA[") == 0 && result.size() >= 12) {
        result = result.substr(9, result.size() - 12);
    } else {
        result = StringUtils::replace(result, "&lt;", "<");
        result = StringUtils::replace(result, "&gt;", ">");
        result = StringUtils::replace(result, "&quot;", "\"");
        result = StringUtils::replace(result, "&apos;", "'");
        result = StringUtils::replace(result, "&amp;", "&");
    }
    pos = end;
    return true;
}

/// @brief parses an RFC 822 date like "Thu, 19 Nov 2026 10:00:00 +0000" (the time zone is treated as UTC)
time_t
parseDate(const std::string& text) {
    std::tm tm = {};
    std::istringstream in(text);
    in >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S");
    if (in.fail()) {
        return 0;
    }
#ifdef _MSC_VER
    return _mkgmtime(&tm);
#else
    return timegm(&tm);
#endif
}
}


// ===========================================================================
// member method definitions
// ===========================================================================
GUINewsFeed::GUINewsFeed(FXApp* app, FXWindow* owner, FXStatusBar* statusBar, const std::string& url) :
    myApp(app),
    myOwner(owner),
    myStatusBar(statusBar),
    myURL(url),
    myAbort(false),
    myThreadEvent(this, ID_FETCHED) {
}


GUINewsFeed::~GUINewsFeed() {
    myAbort = true;
    if (myThread.joinable()) {
        myThread.join();
    }
}


time_t
GUINewsFeed::getLastAccess() const {
    const std::string stored = myApp->reg().readStringEntry(NEWS_REG_SECTION, "lastaccess", "0");
    try {
        return (time_t)StringUtils::toLong(stored);
    } catch (...) {
        return 0;
    }
}


void
GUINewsFeed::showLatest() {
    if (!myNewsText.empty()) {
        onCmdShow(nullptr, 0, nullptr);
    } else if (myThread.joinable()) {
        myShowAfterFetch = true;
    } else if (!startCheck(true)) {
        FXMessageBox::information(myOwner, MBOX_OK, TL("SUMO news"), "%s", TL("The news feed is not available."));
    }
}


bool
GUINewsFeed::startCheck(const bool force) {
#ifdef HAVE_CURL
    if (myThread.joinable()) {
        return false;
    }
    myPreviousAccess = getLastAccess();
    const time_t now = time(nullptr);
    // a last access in the future (or less than MIN_INTERVAL ago) suppresses the automatic check
    if (!force && myPreviousAccess > 0 && now - myPreviousAccess < MIN_INTERVAL) {
        return false;
    }
    myShowAfterFetch = force;
    myThread = std::thread(&GUINewsFeed::fetch, this);
    return true;
#else
    return false;
#endif
}


void
GUINewsFeed::fetch() {
#ifdef HAVE_CURL
    std::string document;
    bool ok = false;
    CURL* curl = curl_easy_init();
    if (curl != nullptr) {
        curl_easy_setopt(curl, CURLOPT_URL, myURL.c_str());
        curl_easy_setopt(curl, CURLOPT_PROTOCOLS_STR, "http,https");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "sumo-gui-news");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, collect);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &document);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, checkAbort);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &myAbort);
        ok = curl_easy_perform(curl) == CURLE_OK;
        curl_easy_cleanup(curl);
    }
    if (myAbort) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(myMutex);
        myDocument = document;
        myOK = ok;
    }
    myThreadEvent.signal();
#endif
}


long
GUINewsFeed::onFetched(FXObject*, FXSelector, void*) {
    std::string document;
    bool ok;
    {
        std::lock_guard<std::mutex> lock(myMutex);
        document = myDocument;
        ok = myOK;
    }
    if (myThread.joinable()) {
        myThread.join();
    }
    const bool requested = myShowAfterFetch;
    myShowAfterFetch = false;
    if (!ok) {
        // offline or feed unavailable, try again next time
        if (requested) {
            FXMessageBox::information(myOwner, MBOX_OK, TL("SUMO news"), "%s", TL("The news feed is currently not available."));
        }
        return 1;
    }
    if (!requested) {
        // an explicit request must not reset a (possibly manually set) last access time
        myApp->reg().writeStringEntry(NEWS_REG_SECTION, "lastaccess", toString((long long)time(nullptr)).c_str());
    }
    std::string latest;
    bool hasNew = false;
    int count = 0;
    for (const Item& item : parseRSS(document)) {
        if (count++ < NEWS_MAX_SHOWN) {
            latest += item.title + "\n" + item.link + "\n\n";
            hasNew |= item.published > myPreviousAccess;
        }
    }
    myNewsText = latest;
    if (requested) {
        if (latest.empty()) {
            FXMessageBox::information(myOwner, MBOX_OK, TL("SUMO news"), "%s", TL("There is currently no news."));
        } else {
            onCmdShow(nullptr, 0, nullptr);
        }
    } else if (hasNew) {
        if (myStatusBar == nullptr) {
            onCmdShow(nullptr, 0, nullptr);
        } else if (myButton == nullptr) {
            myButton = GUIDesigns::buildFXButton(myStatusBar, TL("News"), TL("Show the latest SUMO news"), TL("Show the latest SUMO news"),
                                                 nullptr, this, ID_SHOW, BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_RIGHT);
            if (myStatusBar->id() != 0) {
                myButton->create();
            }
            myStatusBar->recalc();
        }
    }
    return 1;
}


long
GUINewsFeed::onCmdShow(FXObject*, FXSelector, void*) {
    FXMessageBox::information(myOwner, MBOX_OK, TL("SUMO news"), "%s", myNewsText.c_str());
    if (myButton != nullptr) {
        delete myButton;
        myButton = nullptr;
        myStatusBar->recalc();
    }
    return 1;
}


std::vector<GUINewsFeed::Item>
GUINewsFeed::parseRSS(const std::string& xml) {
    std::vector<Item> result;
    size_t pos = 0;
    std::string block;
    while (getElement(xml, "item", pos, block)) {
        Item item;
        size_t p = 0;
        getElement(block, "title", p, item.title);
        p = 0;
        getElement(block, "link", p, item.link);
        p = 0;
        getElement(block, "description", p, item.description);
        p = 0;
        std::string date;
        if (getElement(block, "pubDate", p, date)) {
            item.published = parseDate(date);
        }
        result.push_back(item);
    }
    return result;
}


/****************************************************************************/
