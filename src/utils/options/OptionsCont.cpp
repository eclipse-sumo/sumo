/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    OptionsCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Mon, 17 Dec 2001
///
// A storage for options (typed value containers)
/****************************************************************************/
#include <config.h>

#include <map>
#include <string>
#include <exception>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cerrno>
#include <iterator>
#include <sstream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include "Option.h"
#include "OptionsIO.h"
#include "OptionsCont.h"


// ===========================================================================
// static member definitions
// ===========================================================================

OptionsCont OptionsCont::myOptions;

// ===========================================================================
// method definitions
// ===========================================================================

OptionsCont&
OptionsCont::getOptions() {
    return myOptions;
}


OptionsCont::OptionsCont() {
    myCopyrightNotices.push_back(TL("Copyright (C) 2001-2023 German Aerospace Center (DLR) and others; https://sumo.dlr.de"));
}


OptionsCont::~OptionsCont() {
    clear();
}


void
OptionsCont::doRegister(const std::string& name, Option* o) {
    // first check that option isn't null
    if (o == nullptr) {
        throw ProcessError("Option cannot be null");
    }
    // now check that there isn't another addresse (or synonym) related with the option
    if (myValues.find(name) != myValues.end()) {
        throw ProcessError(name + " is an already used option name.");
    }
    // check if previously was inserted in addresses (to avoid synonyms in addresses)
    bool isSynonym = false;
    for (const auto& addresse : myAddresses) {
        if (addresse.second == o) {
            isSynonym = true;
        }
    }
    if (!isSynonym) {
        myAddresses.push_back(std::make_pair(name, o));
    }
    // insert in values
    myValues[name] = o;
}


void
OptionsCont::doRegister(const std::string& name1, char abbr, Option* o) {
    doRegister(name1, o);
    doRegister(convertChar(abbr), o);
}


void
OptionsCont::addSynonyme(const std::string& name1, const std::string& name2, bool isDeprecated) {
    auto i1 = myValues.find(name1);
    auto i2 = myValues.find(name2);
    if (i1 == myValues.end() && i2 == myValues.end()) {
        throw ProcessError("Neither the option '" + name1 + "' nor the option '" + name2 + "' is known yet");
    }
    if (i1 != myValues.end() && i2 != myValues.end()) {
        if ((*i1).second == (*i2).second) {
            return;
        }
        throw ProcessError("Both options '" + name1 + "' and '" + name2 + "' do exist and differ.");
    }
    if (i1 == myValues.end() && i2 != myValues.end()) {
        doRegister(name1, (*i2).second);
        if (isDeprecated) {
            myDeprecatedSynonymes[name1] = false;
        }
    }
    if (i1 != myValues.end() && i2 == myValues.end()) {
        doRegister(name2, (*i1).second);
        if (isDeprecated) {
            myDeprecatedSynonymes[name2] = false;
        }
    }
}


void
OptionsCont::addXMLDefault(const std::string& name, const std::string& xmlRoot) {
    myXMLDefaults[xmlRoot] = name;
}


bool
OptionsCont::exists(const std::string& name) const {
    return myValues.count(name) > 0;
}


bool
OptionsCont::isSet(const std::string& name, bool failOnNonExistant) const {
    auto i = myValues.find(name);
    if (i == myValues.end()) {
        if (failOnNonExistant) {
            throw ProcessError(TLF("Internal request for unknown option '%'!", name));
        } else {
            return false;
        }
    }
    return (*i).second->isSet();
}


bool
OptionsCont::isDefault(const std::string& name) const {
    auto i = myValues.find(name);
    if (i == myValues.end()) {
        return false;
    }
    return (*i).second->isDefault();
}


Option*
OptionsCont::getSecure(const std::string& name) const {
    const auto& valuesFinder = myValues.find(name);
    if (valuesFinder == myValues.end()) {
        throw ProcessError(TLF("No option with the name '%' exists.", name));
    }
    const auto& synonymFinder = myDeprecatedSynonymes.find(name);
    if ((synonymFinder != myDeprecatedSynonymes.end()) && !synonymFinder->second) {
        std::string defaultName;
        for (const auto& subtopicEntry : mySubTopicEntries) {
            for (const auto& value : subtopicEntry.second) {
                const auto l = myValues.find(value);
                if ((l != myValues.end()) && (l->second == valuesFinder->second)) {
                    defaultName = value;
                    break;
                }
            }
            if (defaultName != "") {
                break;
            }
        }
        WRITE_WARNINGF(TL("Please note that '%' is deprecated.\n Use '%' instead."), name, defaultName);
        synonymFinder->second = true;
    }
    return valuesFinder->second;
}


std::string
OptionsCont::getValueString(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getValueString();
}


std::string
OptionsCont::getString(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getString();
}


double
OptionsCont::getFloat(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getFloat();
}


int
OptionsCont::getInt(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getInt();
}


bool
OptionsCont::getBool(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getBool();
}


const IntVector&
OptionsCont::getIntVector(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getIntVector();
}

const StringVector&
OptionsCont::getStringVector(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getStringVector();
}


bool
OptionsCont::set(const std::string& name, const std::string& value, const bool append) {
    Option* o = getSecure(name);
    if (!o->isWriteable()) {
        reportDoubleSetting(name);
        return false;
    }
    try {
        // Substitute environment variables defined by ${NAME} with their value
        if (!o->set(StringUtils::substituteEnvironment(value, &OptionsIO::getLoadTime()), value, append)) {
            return false;
        }
    } catch (ProcessError& e) {
        WRITE_ERROR("While processing option '" + name + "':\n " + e.what());
        return false;
    }
    return true;
}


bool
OptionsCont::setDefault(const std::string& name, const std::string& value) {
    Option* const o = getSecure(name);
    if (o->isWriteable() && set(name, value)) {
        o->resetDefault();
        return true;
    }
    return false;
}


bool
OptionsCont::setByRootElement(const std::string& root, const std::string& value) {
    if (myXMLDefaults.count(root) > 0) {
        return set(myXMLDefaults[root], value);
    }
    if (myXMLDefaults.count("") > 0) {
        return set(myXMLDefaults[""], value);
    }
    return false;
}


std::vector<std::string>
OptionsCont::getSynonymes(const std::string& name) const {
    Option* o = getSecure(name);
    std::vector<std::string> synonymes;
    for (const auto& value : myValues) {
        if ((value.second == o) && (name != value.first)) {
            synonymes.push_back(value.first);
        }
    }
    return synonymes;
}


const std::string&
OptionsCont::getDescription(const std::string& name) const {
    return getSecure(name)->getDescription();
}


const std::string&
OptionsCont::getSubTopic(const std::string& name) const {
    return getSecure(name)->getSubTopic();
}


std::ostream&
operator<<(std::ostream& os, const OptionsCont& oc) {
    std::vector<std::string> done;
    os << "Options set:" << std::endl;
    for (const auto& value : oc.myValues) {
        const auto& finder = std::find(done.begin(), done.end(), value.first);
        if (finder == done.end()) {
            std::vector<std::string> synonymes = oc.getSynonymes(value.first);
            if (synonymes.size() != 0) {
                os << value.first << " (";
                for (auto synonym = synonymes.begin(); synonym != synonymes.end(); synonym++) {
                    if (synonym != synonymes.begin()) {
                        os << ", ";
                    }
                    os << (*synonym);
                }
                os << ")";
            } else {
                os << value.first;
            }
            if (value.second->isSet()) {
                os << ": " << value.second->getValueString() << std::endl;
            } else {
                os << ": <INVALID>" << std::endl;
            }
            done.push_back(value.first);
            copy(synonymes.begin(), synonymes.end(), back_inserter(done));
        }
    }
    return os;
}


void
OptionsCont::relocateFiles(const std::string& configuration) const {
    for (const auto& addresse : myAddresses) {
        if (addresse.second->isFileName() && addresse.second->isSet()) {
            StringVector fileList = StringVector(addresse.second->getStringVector());
            for (auto& file : fileList) {
                file = FileHelpers::checkForRelativity(file, configuration);
                try {
                    file = StringUtils::urlDecode(file);
                } catch (NumberFormatException& e) {
                    WRITE_WARNING(toString(e.what()) + " when trying to decode filename '" + file + "'.");
                }
            }
            StringVector rawList = StringTokenizer(addresse.second->getValueString(), ",").getVector();
            for (auto& file : rawList) {
                file = FileHelpers::checkForRelativity(file, configuration);
            }
            const std::string conv = joinToString(fileList, ',');
            if (conv != joinToString(addresse.second->getStringVector(), ',')) {
                const bool hadDefault = addresse.second->isDefault();
                addresse.second->set(conv, joinToString(rawList, ','), false);
                if (hadDefault) {
                    addresse.second->resetDefault();
                }
            }
        }
    }
}


bool
OptionsCont::isUsableFileList(const std::string& name) const {
    Option* const o = getSecure(name);
    if (!o->isSet()) {
        return false;
    }
    // check whether the list of files is valid
    bool ok = true;
    std::vector<std::string> files = getStringVector(name);
    if (files.size() == 0) {
        WRITE_ERRORF(TL("The file list for '%' is empty."), name);
        ok = false;
    }
    for (const auto& file : files) {
        if (!FileHelpers::isReadable(file)) {
            if (file != "") {
                WRITE_ERRORF(TL("File '%' is not accessible (%)."), file, std::strerror(errno));
                ok = false;
            } else {
                WRITE_WARNING(TL("Empty file name given; ignoring."));
            }
        }
    }
    return ok;
}


bool
OptionsCont::checkDependingSuboptions(const std::string& name, const std::string& prefix) const {
    Option* o = getSecure(name);
    if (o->isSet()) {
        return true;
    }
    bool ok = true;
    std::vector<std::string> seenSynonymes;
    for (const auto& value : myValues) {
        if (std::find(seenSynonymes.begin(), seenSynonymes.end(), value.first) != seenSynonymes.end()) {
            continue;
        }
        if (value.second->isSet() && !value.second->isDefault() && value.first.find(prefix) == 0) {
            WRITE_ERRORF(TL("Option '%' needs option '%'."), value.first, name);
            std::vector<std::string> synonymes = getSynonymes(value.first);
            std::copy(synonymes.begin(), synonymes.end(), std::back_inserter(seenSynonymes));
            ok = false;
        }
    }
    return ok;
}


void
OptionsCont::reportDoubleSetting(const std::string& arg) const {
    std::vector<std::string> synonymes = getSynonymes(arg);
    std::ostringstream s;
    s << "A value for the option '" + arg + "' was already set.\n Possible synonymes: ";
    auto synonym = synonymes.begin();
    while (synonym != synonymes.end()) {
        s << (*synonym);
        synonym++;
        if (synonym != synonymes.end()) {
            s << ", ";
        }
    }
    WRITE_ERROR(s.str());
}


std::string
OptionsCont::convertChar(char abbr) const {
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    std::string s(buf);
    return s;
}


bool
OptionsCont::isBool(const std::string& name) const {
    Option* o = getSecure(name);
    return o->isBool();
}


void
OptionsCont::resetWritable() {
    for (const auto& addresse : myAddresses) {
        addresse.second->resetWritable();
    }
}


void
OptionsCont::resetDefault() {
    for (const auto& addresse : myAddresses) {
        addresse.second->resetDefault();
    }
}


void
OptionsCont::resetDefault(const std::string& name) {
    getSecure(name)->resetDefault();
}


bool
OptionsCont::isWriteable(const std::string& name) {
    Option* o = getSecure(name);
    return o->isWriteable();
}


void
OptionsCont::clear() {
    // delete only adresse (because synonyms placed in values aim to the same Option)
    for (const auto& addresse : myAddresses) {
        delete addresse.second;
    }
    myAddresses.clear();
    myValues.clear();
    mySubTopics.clear();
    mySubTopicEntries.clear();
}


void
OptionsCont::addDescription(const std::string& name, const std::string& subtopic,
                            const std::string& description) {
    Option* o = getSecure(name);
    if (o == nullptr) {
        throw ProcessError("Option doesn't exist");
    }
    if (find(mySubTopics.begin(), mySubTopics.end(), subtopic) == mySubTopics.end()) {
        throw ProcessError("SubTopic '" + subtopic + "' doesn't exist");
    }
    o->setDescription(description);
    o->setSubtopic(subtopic);
    mySubTopicEntries[subtopic].push_back(name);
}


void
OptionsCont::setFurtherAttributes(const std::string& name, const std::string& subtopic, bool required, bool positional, const std::string& listSep) {
    Option* o = getSecure(name);
    if (o == nullptr) {
        throw ProcessError("Option doesn't exist");
    }
    if (find(mySubTopics.begin(), mySubTopics.end(), subtopic) == mySubTopics.end()) {
        throw ProcessError("SubTopic '" + subtopic + "' doesn't exist");
    }
    if (required) {
        o->setRequired();
    }
    if (positional) {
        o->setPositional();
    }
    o->setListSeparator(listSep);
}


void
OptionsCont::setApplicationName(const std::string& appName,
                                const std::string& fullName) {
    myAppName = appName;
    myFullName = fullName;
}


void
OptionsCont::setApplicationDescription(const std::string& appDesc) {
    myAppDescription = appDesc;
}


void
OptionsCont::addCallExample(const std::string& example, const std::string& desc) {
    myCallExamples.push_back(std::make_pair(example, desc));
}


void
OptionsCont::setAdditionalHelpMessage(const std::string& add) {
    myAdditionalMessage = add;
}


void
OptionsCont::addCopyrightNotice(const std::string& copyrightLine) {
    myCopyrightNotices.push_back(copyrightLine);
}


void
OptionsCont::clearCopyrightNotices() {
    myCopyrightNotices.clear();
}


void
OptionsCont::addOptionSubTopic(const std::string& topic) {
    mySubTopics.push_back(topic);
    mySubTopicEntries[topic] = std::vector<std::string>();
}


void
OptionsCont::splitLines(std::ostream& os, std::string what,
                        int offset, int nextOffset) {
    while (what.length() > 0) {
        if ((int)what.length() > 79 - offset) {
            std::string::size_type splitPos = what.rfind(';', 79 - offset);
            if (splitPos == std::string::npos) {
                splitPos = what.rfind(' ', 79 - offset);
            } else {
                splitPos++;
            }
            if (splitPos != std::string::npos) {
                os << what.substr(0, splitPos) << std::endl;
                what = what.substr(splitPos + 1);
                for (int r = 0; r < (nextOffset + 1); ++r) {
                    os << ' ';
                }
            } else {
                os << what;
                what = "";
            }
            offset = nextOffset;
        } else {
            os << what;
            what = "";
        }
    }
    os << std::endl;
}


bool
OptionsCont::processMetaOptions(bool missingOptions) {
    MsgHandler::setupI18n(getString("language"));
    if (missingOptions) {
        // no options are given
        std::cout << myFullName << std::endl;
        std::cout << TL(" Build features: ") << HAVE_ENABLED << std::endl;
        for (const auto& copyrightNotice : myCopyrightNotices) {
            std::cout << " " << copyrightNotice.data() << std::endl;
        }
        std::cout << TL(" License EPL-2.0: Eclipse Public License Version 2 <https://eclipse.org/legal/epl-v20.html>") << std::endl;
        std::cout << TL(" Use --help to get the list of options.") << std::endl;
        return true;
    }

    myWriteLicense = getBool("write-license");
    // check whether the help shall be printed
    if (getBool("help")) {
        std::cout << myFullName << std::endl;
        for (const auto& copyrightNotice : myCopyrightNotices) {
            std::cout << " " << copyrightNotice.data() << std::endl;
        }
        printHelp(std::cout);
        return true;
    }
    // check whether the help shall be printed
    if (getBool("version")) {
        std::cout << myFullName << std::endl;
        std::cout << TL(" Build features: ") << HAVE_ENABLED << std::endl;
        for (const auto& copyrightNotice : myCopyrightNotices) {
            std::cout << " " << copyrightNotice.data() << std::endl;
        }
        std::cout << "\n" << myFullName << " is part of SUMO.\n";
        std::cout << "This program and the accompanying materials\n";
        std::cout << "are made available under the terms of the Eclipse Public License v2.0\n";
        std::cout << "which accompanies this distribution, and is available at\n";
        std::cout << "http://www.eclipse.org/legal/epl-v20.html\n";
        std::cout << "This program may also be made available under the following Secondary\n";
        std::cout << "Licenses when the conditions for such availability set forth in the Eclipse\n";
        std::cout << "Public License 2.0 are satisfied: GNU General Public License, version 2\n";
        std::cout << "or later which is available at\n";
        std::cout << "https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html\n";
        std::cout << "SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later" << std::endl;
        return true;
    }
    // check whether the settings shall be printed
    if (getBool("print-options")) {
        std::cout << (*this);
    }
    // check whether something has to be done with options
    // whether the current options shall be saved
    if (isSet("save-configuration")) {
        const std::string& configPath = getString("save-configuration");
        if (configPath == "-" || configPath == "stdout") {
            writeConfiguration(std::cout, true, false, getBool("save-commented"));
            return true;
        }
        std::ofstream out(StringUtils::transcodeToLocal(configPath).c_str());
        if (!out.good()) {
            throw ProcessError(TLF("Could not save configuration to '%'", configPath));
        } else {
            writeConfiguration(out, true, false, getBool("save-commented"), configPath);
            if (getBool("verbose")) {
                WRITE_MESSAGEF(TL("Written configuration to '%'"), configPath);
            }
            return true;
        }
    }
    // whether the template shall be saved
    if (isSet("save-template")) {
        if (getString("save-template") == "-" || getString("save-template") == "stdout") {
            writeConfiguration(std::cout, false, true, getBool("save-commented"));
            return true;
        }
        std::ofstream out(StringUtils::transcodeToLocal(getString("save-template")).c_str());
        if (!out.good()) {
            throw ProcessError(TLF("Could not save template to '%'", getString("save-template")));
        } else {
            writeConfiguration(out, false, true, getBool("save-commented"));
            if (getBool("verbose")) {
                WRITE_MESSAGEF(TL("Written template to '%'"), getString("save-template"));
            }
            return true;
        }
    }
    if (isSet("save-schema")) {
        if (getString("save-schema") == "-" || getString("save-schema") == "stdout") {
            writeSchema(std::cout);
            return true;
        }
        std::ofstream out(StringUtils::transcodeToLocal(getString("save-schema")).c_str());
        if (!out.good()) {
            throw ProcessError(TLF("Could not save schema to '%'", getString("save-schema")));
        } else {
            writeSchema(out);
            if (getBool("verbose")) {
                WRITE_MESSAGEF(TL("Written schema to '%'"), getString("save-schema"));
            }
            return true;
        }
    }
    return false;
}


const std::vector<std::string>&
OptionsCont::getSubTopics() const {
    return mySubTopics;
}


std::vector<std::string>
OptionsCont::getSubTopicsEntries(const std::string& subtopic) const {
    if (mySubTopicEntries.count(subtopic) > 0) {
        return mySubTopicEntries.find(subtopic)->second;
    } else {
        return std::vector<std::string>();
    }
}


std::string
OptionsCont::getTypeName(const std::string name) {
    return getSecure(name)->getTypeName();
}


const std::string&
OptionsCont::getFullName() const {
    return myFullName;
}


bool
OptionsCont::isEmpty() const {
    return myAddresses.size() == 0;
}


std::vector<std::pair<std::string, Option*> >::const_iterator
OptionsCont::begin() const {
    return myAddresses.cbegin();
}


std::vector<std::pair<std::string, Option*> >::const_iterator
OptionsCont::end() const {
    return myAddresses.cend();
}


void
OptionsCont::printHelp(std::ostream& os) {
    // print application description
    splitLines(os, TL(myAppDescription.c_str()), 0, 0);
    os << std::endl;

    // check option sizes first
    //  we want to know how large the largest not-too-large-entry will be
    int tooLarge = 40;
    int maxSize = 0;
    for (const auto& subTopic : mySubTopics) {
        for (const auto& entry : mySubTopicEntries[subTopic]) {
            Option* o = getSecure(entry);
            // name, two leading spaces and "--"
            int csize = (int)entry.length() + 2 + 4;
            // abbreviation length ("-X, "->4chars) if any
            const auto synonymes = getSynonymes(entry);
            for (const auto& synonym : synonymes) {
                if (synonym.length() == 1 && myDeprecatedSynonymes.count(synonym) == 0) {
                    csize += 4;
                    break;
                }
            }
            // the type name
            if (!o->isBool()) {
                csize += 1 + (int)o->getTypeName().length();
            }
            // divider
            csize += 2;
            if (csize < tooLarge && maxSize < csize) {
                maxSize = csize;
            }
        }
    }

    const std::string helpTopic = StringUtils::to_lower_case(getSecure("help")->getValueString());
    if (helpTopic != "") {
        bool foundTopic = false;
        for (const auto& topic : mySubTopics) {
            if (StringUtils::to_lower_case(topic).find(helpTopic) != std::string::npos) {
                foundTopic = true;
                printHelpOnTopic(topic, tooLarge, maxSize, os);
            }
        }
        if (!foundTopic) {
            // print topic list
            os << "Help Topics:"  << std::endl;
            for (std::string t : mySubTopics) {
                os << "    " << t << std::endl;
            }
        }
        return;
    }
    // print usage BNF
    os << "Usage: " << myAppName << " [OPTION]*" << std::endl;
    // print additional text if any
    if (myAdditionalMessage.length() > 0) {
        os << myAdditionalMessage << std::endl << ' ' << std::endl;
    }
    // print the options
    for (const auto& subTopic : mySubTopics) {
        printHelpOnTopic(subTopic, tooLarge, maxSize, os);
    }
    os << std::endl;
    // print usage examples, calc size first
    if (myCallExamples.size() != 0) {
        os << "Examples:" << std::endl;
        for (const auto& callExample : myCallExamples) {
            os << "  " << myAppName << ' ' << callExample.first << std::endl;
            os << "    " << callExample.second << std::endl;
        }
    }
    os << std::endl;
    os << "Report bugs at <https://github.com/eclipse/sumo/issues>." << std::endl;
    os << "Get in contact via <sumo@dlr.de>." << std::endl;
}


void
OptionsCont::printHelpOnTopic(const std::string& topic, int tooLarge, int maxSize, std::ostream& os) {
    os << topic << " Options:" << std::endl;
    for (const auto& entry : mySubTopicEntries[topic]) {
        // start length computation
        int csize = (int)entry.length() + 2;
        Option* o = getSecure(entry);
        os << "  ";
        // write abbreviation if given
        const auto synonymes = getSynonymes(entry);
        for (const auto& synonym : synonymes) {
            if (synonym.length() == 1 && myDeprecatedSynonymes.count(synonym) == 0) {
                os << '-' << synonym << ", ";
                csize += 4;
                break;
            }
        }
        // write leading '-'/"--"
        os << "--";
        csize += 2;
        // write the name
        os << entry;
        // write the type if not a bool option
        if (!o->isBool()) {
            os << ' ' << o->getTypeName();
            csize += 1 + (int)o->getTypeName().length();
        }
        csize += 2;
        // write the description formatting it
        os << "  ";
        for (int r = maxSize; r > csize; --r) {
            os << ' ';
        }
        int offset = csize > tooLarge ? csize : maxSize;
        splitLines(os, o->getDescription(), offset, maxSize);
    }
    os << std::endl;
}


void
OptionsCont::writeConfiguration(std::ostream& os, const bool filled,
                                const bool complete, const bool addComments, const std::string& relativeTo,
                                const bool forceRelative, const bool inComment) const {
    if (!inComment) {
        writeXMLHeader(os, false);
    }
    os << "<configuration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.dlr.de/xsd/";
    if (myAppName == "sumo-gui") {
        os << "sumo";
    } else if (myAppName == "netedit") {
        os << "netconvert";
    } else {
        os << myAppName;
    }
    os << "Configuration.xsd\">" << std::endl << std::endl;
    for (std::string subtopic : mySubTopics) {
        if (subtopic == "Configuration" && !complete) {
            continue;
        }
        const std::vector<std::string>& entries = mySubTopicEntries.find(subtopic)->second;
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        subtopic = StringUtils::to_lower_case(subtopic);
        bool hadOne = false;
        for (const std::string& name : entries) {
            Option* o = getSecure(name);
            bool write = complete || (filled && !o->isDefault());
            if (!write) {
                continue;
            }
            if (name == "registry-viewport" && !complete) {
                continue;
            }
            if (!hadOne) {
                os << "    <" << subtopic << ">" << std::endl;
            }
            // add the comment if wished
            if (addComments) {
                os << "        <!-- " << StringUtils::escapeXML(o->getDescription(), inComment) << " -->" << std::endl;
            }
            // write the option and the value (if given)
            os << "        <" << name << " value=\"";
            if (o->isSet() && (filled || o->isDefault())) {
                if (o->isFileName() && relativeTo != "") {
                    StringVector fileList = StringTokenizer(o->getValueString(), ",").getVector();
                    for (auto& file : fileList) {
                        file = FileHelpers::fixRelative(StringUtils::urlEncode(file, " ;%"), relativeTo,
                                                        forceRelative || getBool("save-configuration.relative"));
                    }
                    os << StringUtils::escapeXML(joinToString(fileList, ','), inComment);
                } else {
                    os << StringUtils::escapeXML(o->getValueString(), inComment);
                }
            }
            if (complete) {
                std::vector<std::string> synonymes = getSynonymes(name);
                if (!synonymes.empty()) {
                    os << "\" synonymes=\"";
                    for (auto synonym = synonymes.begin(); synonym != synonymes.end(); synonym++) {
                        if (synonym != synonymes.begin()) {
                            os << " ";
                        }
                        os << (*synonym);
                    }
                }
                os << "\" type=\"" << o->getTypeName();
                if (!addComments) {
                    os << "\" help=\"" << StringUtils::escapeXML(o->getDescription());
                }
            }
            os << "\"/>" << std::endl;
            // append an endline if a comment was printed
            if (addComments) {
                os << std::endl;
            }
            hadOne = true;
        }
        if (hadOne) {
            os << "    </" << subtopic << ">" << std::endl << std::endl;
        }
    }
    os << "</configuration>" << std::endl;
}


void
OptionsCont::writeSchema(std::ostream& os) {
    writeXMLHeader(os, false);
    os << "<xsd:schema elementFormDefault=\"qualified\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n\n";
    os << "    <xsd:include schemaLocation=\"baseTypes.xsd\"/>\n";
    os << "    <xsd:element name=\"configuration\" type=\"configurationType\"/>\n\n";
    os << "    <xsd:complexType name=\"configurationType\">\n";
    os << "        <xsd:all>\n";
    for (std::string subtopic : mySubTopics) {
        if (subtopic == "Configuration") {
            continue;
        }
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        subtopic = StringUtils::to_lower_case(subtopic);
        os << "            <xsd:element name=\"" << subtopic << "\" type=\"" << subtopic << "TopicType\" minOccurs=\"0\"/>\n";
    }
    os << "        </xsd:all>\n";
    os << "    </xsd:complexType>\n\n";
    for (std::string subtopic : mySubTopics) {
        if (subtopic == "Configuration") {
            continue;
        }
        const std::vector<std::string>& entries = mySubTopicEntries.find(subtopic)->second;
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        subtopic = StringUtils::to_lower_case(subtopic);
        os << "    <xsd:complexType name=\"" << subtopic << "TopicType\">\n";
        os << "        <xsd:all>\n";
        for (const auto& entry : entries) {
            Option* o = getSecure(entry);
            std::string type = o->getTypeName();
            type = StringUtils::to_lower_case(type);
            if (type == "int[]") {
                type = "intArray";
            }
            if (type == "str[]") {
                type = "strArray";
            }
            os << "            <xsd:element name=\"" << entry << "\" type=\"" << type << "OptionType\" minOccurs=\"0\"/>\n";
        }
        os << "        </xsd:all>\n";
        os << "    </xsd:complexType>\n\n";
    }
    os << "</xsd:schema>\n";
}


void
OptionsCont::writeXMLHeader(std::ostream& os, const bool includeConfig) const {
    time_t rawtime;
    char buffer [80];

    os << "<?xml version=\"1.0\"" << SUMOSAXAttributes::ENCODING << "?>\n\n";
    time(&rawtime);
    strftime(buffer, 80, "<!-- generated on %F %T by ", localtime(&rawtime));
    os << buffer << myFullName << "\n";
    if (myWriteLicense) {
        os << "This data file and the accompanying materials\n"
           "are made available under the terms of the Eclipse Public License v2.0\n"
           "which accompanies this distribution, and is available at\n"
           "http://www.eclipse.org/legal/epl-v20.html\n"
           "This file may also be made available under the following Secondary\n"
           "Licenses when the conditions for such availability set forth in the Eclipse\n"
           "Public License 2.0 are satisfied: GNU General Public License, version 2\n"
           "or later which is available at\n"
           "https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html\n"
           "SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later\n";
    }
    if (includeConfig) {
        writeConfiguration(os, true, false, false, "", false, true);
    }
    os << "-->\n\n";
}


bool
OptionsCont::isInStringVector(const std::string& optionName,
                              const std::string& itemName) const {
    if (isSet(optionName)) {
        std::vector<std::string> values = getStringVector(optionName);
        return std::find(values.begin(), values.end(), itemName) != values.end();
    }
    return false;
}

/****************************************************************************/
