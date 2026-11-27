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
#include <regex>
#include <sstream>
#ifdef HAVE_CURL
#include <curl/curl.h>
#endif

#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/Translation.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GUINewsFeed.h"


// ===========================================================================
// constants
// ===========================================================================
const std::string GUINewsFeed::DEFAULT_URL = "https://bsky.app/profile/eclipse-sumo.bsky.social/rss";
const time_t GUINewsFeed::MIN_INTERVAL = 30 * 24 * 60 * 60;

#define NEWS_REG_SECTION "NEWS"
#define NEWS_MAX_SHOWN 5
#define NEWS_DISABLED 4102444800LL // year 2100
#define NEWS_WRAP_COLUMNS 80
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

/// @brief inserts line breaks at spaces so that lines have at most the given number of bytes (if possible)
std::string
wrapLine(const std::string& line, const size_t width) {
    std::string result;
    size_t lineStart = 0;
    size_t lastSpace = std::string::npos;
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ' ') {
            lastSpace = i;
        }
        if (i - lineStart >= width && lastSpace != std::string::npos && lastSpace > lineStart) {
            result += line.substr(lineStart, lastSpace - lineStart) + "\n";
            lineStart = lastSpace + 1;
            lastSpace = std::string::npos;
        }
    }
    return result + line.substr(lineStart);
}

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
        result = StringUtils::replace(result, "&#xA;", "\n");
        result = StringUtils::replace(result, "&#34;", "\"");
        result = StringUtils::replace(result, "&#39;", "'");
        result = StringUtils::replace(result, "&amp;", "&");
    }
    pos = end;
    return true;
}

/// @brief parses an RFC 822 date like "Thu, 19 Nov 2026 10:00:00 +0000" or "19 Nov 2026 10:00 +0000" (the time zone is treated as UTC)
time_t
parseDate(const std::string& text) {
    for (const char* format : {"%a, %d %b %Y %H:%M:%S", "%d %b %Y %H:%M"}) {
        std::tm tm = {};
        std::istringstream in(text);
        in >> std::get_time(&tm, format);
        if (!in.fail()) {
#ifdef _MSC_VER
            return _mkgmtime(&tm);
#else
            return timegm(&tm);
#endif
        }
    }
    return 0;
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
    if (!myItems.empty()) {
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
    myItems.clear();
    bool hasNew = false;
    for (const Item& item : parseRSS(document)) {
        if ((int)myItems.size() < NEWS_MAX_SHOWN) {
            myItems.push_back(item);
            hasNew |= item.published > myPreviousAccess;
        }
    }
    if (requested) {
        if (myItems.empty()) {
            FXMessageBox::information(myOwner, MBOX_OK, TL("SUMO news"), "%s", TL("There is currently no news."));
        } else {
            onCmdShow(nullptr, 0, nullptr);
        }
    } else if (hasNew) {
        if (myStatusBar == nullptr) {
            onCmdShow(nullptr, 0, nullptr);
        } else if (myButton == nullptr) {
            myButton = GUIDesigns::buildFXButton(myStatusBar, "", TL("Show the latest SUMO news"), TL("Show the latest SUMO news"),
                                                 GUIIconSubSys::getIcon(GUIIcon::NEWS), this, ID_SHOW, BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_RIGHT);
            if (myStatusBar->id() != 0) {
                myButton->create();
            }
            myStatusBar->recalc();
        }
    }
    return 1;
}


namespace {
/// @brief adds a row of labels to parent, words that look like URLs (with or without scheme) become clickable links
void
addRichLine(FXComposite* parent, const std::string& line) {
    static const std::regex urlRegex("^(https?://\\S+|([A-Za-z0-9-]+\\.)+[A-Za-z]{2,}(/\\S*)?)$");
    const char* const leadChars = "(\"'";
    const char* const trailChars = ".,;:!?)\"'";
    FXHorizontalFrame* row = new FXHorizontalFrame(parent, LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    std::string text;
    bool first = true;
    const auto flush = [&]() {
        if (!text.empty()) {
            new FXLabel(row, text.c_str(), nullptr, LABEL_NORMAL | JUSTIFY_LEFT, 0, 0, 0, 0, 0, 0, 0, 0);
            text.clear();
        }
    };
    std::istringstream words(line);
    std::string word;
    while (words >> word) {
        const size_t coreStart = word.find_first_not_of(leadChars);
        const size_t coreEnd = word.find_last_not_of(trailChars);
        if (coreStart != std::string::npos && coreEnd != std::string::npos && coreStart <= coreEnd
                && std::regex_match(word.substr(coreStart, coreEnd - coreStart + 1), urlRegex)) {
            const std::string core = word.substr(coreStart, coreEnd - coreStart + 1);
            text += (first ? "" : " ") + word.substr(0, coreStart);
            flush();
            MFXLinkLabel* link = new MFXLinkLabel(row, core.c_str(), nullptr, LABEL_NORMAL | JUSTIFY_LEFT, 0, 0, 0, 0, 0, 0, 0, 0);
            link->setTipText((core.compare(0, 4, "http") == 0 ? core : "https://" + core).c_str());
            text = word.substr(coreEnd + 1);
        } else {
            text += (first ? "" : " ") + word;
        }
        first = false;
    }
    flush();
}
}


long
GUINewsFeed::onCmdShow(FXObject*, FXSelector, void*) {
    FXDialogBox* dialog = new FXDialogBox(myOwner, TL("SUMO news"), GUIDesignDialogBox);
    dialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI));
    FXScrollWindow* scroll = new FXScrollWindow(dialog, GUIDesignScrollWindowFixed(640, 420));
    FXVerticalFrame* content = new FXVerticalFrame(scroll, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    bool first = true;
    for (const Item& item : myItems) {
        if (!first) {
            new FXHorizontalSeparator(content, GUIDesignHorizontalSeparator);
        }
        first = false;
        // the date links to the post, if there is no date the link itself is shown
        std::string dateText = item.link;
        if (item.published > 0) {
            char date[32];
            const std::tm* tm = std::gmtime(&item.published);
            if (tm != nullptr && std::strftime(date, sizeof(date), "%Y-%m-%d", tm) > 0) {
                dateText = date;
            }
        }
        if (!item.link.empty()) {
            MFXLinkLabel* link = new MFXLinkLabel(content, dateText.c_str(), nullptr, LABEL_NORMAL | JUSTIFY_LEFT, 0, 0, 0, 0, 0, 0, 0, 0);
            link->setTipText(item.link.c_str());
        }
        // the title (if any) is followed by the description
        std::vector<std::string> lines;
        for (const std::string& part : {item.title, item.description}) {
            std::istringstream in(part);
            std::string line;
            while (std::getline(in, line)) {
                lines.push_back(StringUtils::trim(line));
            }
        }
        for (const std::string& line : lines) {
            if (!line.empty()) {
                std::istringstream wrapped(wrapLine(line, NEWS_WRAP_COLUMNS));
                std::string part;
                while (std::getline(wrapped, part)) {
                    addRichLine(content, part);
                }
            }
        }
    }
    // automatic checking is disabled by a last access far in the future
    const bool wasEnabled = getLastAccess() < NEWS_DISABLED;
    FXCheckButton* autoCheck = new FXCheckButton(dialog, TL("Check for news automatically"), nullptr, 0, CHECKBUTTON_NORMAL | LAYOUT_LEFT);
    autoCheck->setCheck(wasEnabled);
    FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(dialog, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
    GUIDesigns::buildFXButton(buttonFrame, TL("OK"), "", "", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), dialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonDialog);
    new FXHorizontalFrame(buttonFrame, GUIDesignAuxiliarHorizontalFrame);
    dialog->create();
    dialog->execute(PLACEMENT_OWNER);
    if (autoCheck->getCheck() != wasEnabled) {
        myApp->reg().writeStringEntry(NEWS_REG_SECTION, "lastaccess", autoCheck->getCheck() ? "0" : toString((long long)NEWS_DISABLED).c_str());
    }
    delete dialog;
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
