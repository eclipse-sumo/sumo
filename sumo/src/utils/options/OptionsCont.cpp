/****************************************************************************/
/// @file    OptionsCont.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A storage for options (typed value containers)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <exception>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <iterator>
#include "Option.h"
#include "OptionsCont.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <sstream>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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


OptionsCont::OptionsCont()
    : myAddresses(), myValues(), myDeprecatedSynonymes(), myHaveInformedAboutDeprecatedDivider(false) {
    myCopyrightNotices.push_back("Copyright (C) 2001-2013 DLR and contributors; http://sumo-sim.org");
}


OptionsCont::~OptionsCont() {
    clear();
}


void
OptionsCont::doRegister(const std::string& name, Option* v) {
    assert(v != 0);
    ItemAddressContType::iterator i = find(myAddresses.begin(), myAddresses.end(), v);
    if (i == myAddresses.end()) {
        myAddresses.push_back(v);
    }
    if (myValues.find(name) != myValues.end()) {
        throw ProcessError(name + " is an already used option name.");
    }
    myValues[name] = v;
}


void
OptionsCont::doRegister(const std::string& name1, char abbr, Option* v) {
    doRegister(name1, v);
    doRegister(convertChar(abbr), v);
}


void
OptionsCont::addSynonyme(const std::string& name1, const std::string& name2, bool isDeprecated) {
    KnownContType::iterator i1 = myValues.find(name1);
    KnownContType::iterator i2 = myValues.find(name2);
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


bool
OptionsCont::exists(const std::string& name) const {
    return myValues.count(name) > 0;
}


bool
OptionsCont::isSet(const std::string& name, bool failOnNonExistant) const {
    KnownContType::const_iterator i = myValues.find(name);
    if (i == myValues.end()) {
        if (failOnNonExistant) {
            throw ProcessError("Internal request for unknown option '" + name + "'!");
        } else {
            return false;
        }
    }
    return (*i).second->isSet();
}


void
OptionsCont::unSet(const std::string& name, bool failOnNonExistant) const {
    KnownContType::const_iterator i = myValues.find(name);
    if (i == myValues.end()) {
        if (failOnNonExistant) {
            throw ProcessError("Internal request for unknown option '" + name + "'!");
        } else {
            return;
        }
    }
    (*i).second->unSet();
}


bool
OptionsCont::isDefault(const std::string& name) const {
    KnownContType::const_iterator i = myValues.find(name);
    if (i == myValues.end()) {
        return false;
    }
    return (*i).second->isDefault();
}


Option*
OptionsCont::getSecure(const std::string& name) const {
    KnownContType::const_iterator k = myValues.find(name);
    if (k == myValues.end()) {
        throw ProcessError("No option with the name '" + name + "' exists.");
    }
    std::map<std::string, bool>::iterator s = myDeprecatedSynonymes.find(name);
    if (s != myDeprecatedSynonymes.end() && !s->second) {
        std::string defaultName;
        for (std::map<std::string, std::vector<std::string> >::const_iterator i = mySubTopicEntries.begin(); i != mySubTopicEntries.end(); ++i) {
            for (std::vector<std::string>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
                KnownContType::const_iterator l = myValues.find(*j);
                if (l != myValues.end() && l->second == k->second) {
                    defaultName = *j;
                    break;
                }
            }
            if (defaultName != "") {
                break;
            }
        }
        WRITE_WARNING("Please note that '" + name + "' is deprecated.\n Use '" + defaultName + "' instead.");
        s->second = true;
    }
    return k->second;
}


std::string
OptionsCont::getString(const std::string& name) const {
    Option* o = getSecure(name);
    return o->getString();
}


SUMOReal
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


bool
OptionsCont::set(const std::string& name, const std::string& value) {
    Option* o = getSecure(name);
    if (!o->isWriteable()) {
        reportDoubleSetting(name);
        return false;
    }
    try {
        if (!o->set(value)) {
            return false;
        }
    } catch (ProcessError& e) {
        WRITE_ERROR("While processing option '" + name + "':\n " + e.what());
        return false;
    }
    return true;
}


std::vector<std::string>
OptionsCont::getSynonymes(const std::string& name) const {
    Option* o = getSecure(name);
    std::vector<std::string> v(0);
    for (KnownContType::const_iterator i = myValues.begin(); i != myValues.end(); i++) {
        if ((*i).second == o && name != (*i).first) {
            v.push_back((*i).first);
        }
    }
    return v;
}


std::ostream&
operator<<(std::ostream& os, const OptionsCont& oc) {
    std::vector<std::string> done;
    os << "Options set:" << std::endl;
    for (OptionsCont::KnownContType::const_iterator i = oc.myValues.begin();
            i != oc.myValues.end(); i++) {
        std::vector<std::string>::iterator j = find(done.begin(), done.end(), (*i).first);
        if (j == done.end()) {
            std::vector<std::string> synonymes = oc.getSynonymes((*i).first);
            if (synonymes.size() != 0) {
                os << (*i).first << " (";
                for (j = synonymes.begin(); j != synonymes.end(); j++) {
                    if (j != synonymes.begin()) {
                        os << ", ";
                    }
                    os << (*j);
                }
                os << ")";
            } else {
                os << (*i).first;
            }
            if ((*i).second->isSet()) {
                os << ": " << (*i).second->getValueString() << std::endl;
            } else {
                os << ": <INVALID>" << std::endl;
            }
            done.push_back((*i).first);
            copy(synonymes.begin(), synonymes.end(), back_inserter(done));
        }
    }
    return os;
}


void
OptionsCont::relocateFiles(const std::string& configuration) const {
    for (ItemAddressContType::const_iterator i = myAddresses.begin(); i != myAddresses.end(); i++) {
        if ((*i)->isFileName() && (*i)->isSet()) {
            StringTokenizer st((*i)->getString(), ";, ", true);
            std::string conv;
            while (st.hasNext()) {
                if (conv.length() != 0) {
                    conv += ',';
                }
                std::string tmp = st.next();
                if (!FileHelpers::isAbsolute(tmp)) {
                    tmp = FileHelpers::getConfigurationRelative(configuration, tmp);
                }
                conv += tmp;
            }
            (*i)->set(conv);
        }
    }
}


bool
OptionsCont::isUsableFileList(const std::string& name) const {
    Option* o = getSecure(name);
    // check whether the option is set
    //  return false i not
    if (!o->isSet()) {
        return false;
    }
    // check whether the list of files is valid
    bool ok = true;
    std::vector<std::string> files = getStringVector(name);
    if (files.size() == 0) {
        WRITE_ERROR("The file list for '" + name + "' is empty.");
        ok = false;
    }
    for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
        if (!FileHelpers::exists(*fileIt)) {
            if (*fileIt != "") {
                WRITE_ERROR("File '" + *fileIt + "' does not exist.");
                ok = false;
            } else {
                WRITE_WARNING("Empty file name given; ignoring.");
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
    for (KnownContType::const_iterator i = myValues.begin(); i != myValues.end(); i++) {
        if (std::find(seenSynonymes.begin(), seenSynonymes.end(), (*i).first) != seenSynonymes.end()) {
            continue;
        }
        if ((*i).second->isSet() && !(*i).second->isDefault() && (*i).first.find(prefix) == 0) {
            WRITE_ERROR("Option '" + (*i).first + "' needs option '" + name + "'.");
            std::vector<std::string> synonymes = getSynonymes((*i).first);
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
    for (std::vector<std::string>::iterator i = synonymes.begin(); i != synonymes.end();) {
        s << (*i);
        i++;
        if (i != synonymes.end()) {
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
    for (ItemAddressContType::iterator i = myAddresses.begin(); i != myAddresses.end(); i++) {
        (*i)->resetWritable();
    }
}


bool
OptionsCont::isWriteable(const std::string& name) {
    Option* o = getSecure(name);
    return o->isWriteable();
}


void
OptionsCont::clear() {
    ItemAddressContType::iterator i;
    for (i = myAddresses.begin(); i != myAddresses.end(); i++) {
        delete(*i);
    }
    myAddresses.clear();
    myValues.clear();
    mySubTopics.clear();
    mySubTopicEntries.clear();
}


void
OptionsCont::addDescription(const std::string& name,
                            const std::string& subtopic,
                            const std::string& description) {
    Option* o = getSecure(name);
    assert(o != 0);
    assert(find(mySubTopics.begin(), mySubTopics.end(), subtopic) != mySubTopics.end());
    o->setDescription(description);
    mySubTopicEntries[subtopic].push_back(name);
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
                        size_t offset, size_t nextOffset) {
    while (what.length() > 0) {
        if (what.length() > 79 - offset) {
            size_t splitPos = what.rfind(';', 79 - offset);
            if (splitPos == std::string::npos) {
                splitPos = what.rfind(' ', 79 - offset);
            } else {
                splitPos++;
            }
            if (splitPos != std::string::npos) {
                os << what.substr(0, splitPos) << std::endl;
                what = what.substr(splitPos);
                for (size_t r = 0; r < nextOffset + 1; ++r) {
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
    if (missingOptions) {
        // no options are given
        std::cout << myFullName << std::endl;
        for (std::vector<std::string>::const_iterator it =
                    myCopyrightNotices.begin(); it != myCopyrightNotices.end(); ++it) {
            std::cout << " " << *it << std::endl;
        }
        std::cout << " License GPLv3+: GNU GPL Version 3 or later <http://gnu.org/licenses/gpl.html>\n";
        std::cout << " Use --help to get the list of options." << std::endl;
        return true;
    }

    // check whether the help shall be printed
    if (getBool("help")) {
        std::cout << myFullName << std::endl;
        for (std::vector<std::string>::const_iterator it =
                    myCopyrightNotices.begin(); it != myCopyrightNotices.end(); ++it) {
            std::cout << " " << *it << std::endl;
        }
        printHelp(std::cout);
        return true;
    }
    // check whether the help shall be printed
    if (getBool("version")) {
        std::cout << myFullName << std::endl;
        for (std::vector<std::string>::const_iterator it =
                    myCopyrightNotices.begin(); it != myCopyrightNotices.end(); ++it) {
            std::cout << " " << *it << std::endl;
        }
        std::cout << "\n" << myFullName << " is part of SUMO.\n";
        std::cout << "SUMO is free software: you can redistribute it and/or modify\n";
        std::cout << "it under the terms of the GNU General Public License as published by\n";
        std::cout << "the Free Software Foundation, either version 3 of the License, or\n";
        std::cout << "(at your option) any later version.\n\n";
        std::cout << "This program is distributed in the hope that it will be useful,\n";
        std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
        std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
        std::cout << "GNU General Public License for more details.\n\n";
        std::cout << "You should have received a copy of the GNU General Public License\n";
        std::cout << "along with this program. If not, see http://www.gnu.org/licenses/gpl.html" << std::endl;
        return true;
    }
    // check whether the settings shall be printed
    if (exists("print-options") && getBool("print-options")) {
        std::cout << (*this);
    }
    // check whether something has to be done with options
    // whether the current options shall be saved
    if (isSet("save-configuration", false)) { // sumo-gui does not register these
        if (getString("save-configuration") == "-" || getString("save-configuration") == "stdout") {
            writeConfiguration(std::cout, true, false, getBool("save-commented"));
            return true;
        }
        std::ofstream out(getString("save-configuration").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save configuration to '" + getString("save-configuration") + "'");
        } else {
            writeConfiguration(out, true, false, getBool("save-commented"));
            if (getBool("verbose")) {
                WRITE_MESSAGE("Written configuration to '" + getString("save-configuration") + "'");
            }
            return true;
        }
    }
    // whether the template shall be saved
    if (isSet("save-template", false)) { // sumo-gui does not register these
        if (getString("save-template") == "-" || getString("save-template") == "stdout") {
            writeConfiguration(std::cout, false, true, getBool("save-commented"));
            return true;
        }
        std::ofstream out(getString("save-template").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save template to '" + getString("save-template") + "'");
        } else {
            writeConfiguration(out, false, true, getBool("save-commented"));
            if (getBool("verbose")) {
                WRITE_MESSAGE("Written template to '" + getString("save-template") + "'");
            }
            return true;
        }
    }
    if (isSet("save-schema", false)) { // sumo-gui does not register these
        if (getString("save-schema") == "-" || getString("save-schema") == "stdout") {
            writeSchema(std::cout, getBool("save-commented"));
            return true;
        }
        std::ofstream out(getString("save-schema").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save schema to '" + getString("save-schema") + "'");
        } else {
            writeSchema(out, getBool("save-commented"));
            if (getBool("verbose")) {
                WRITE_MESSAGE("Written schema to '" + getString("save-schema") + "'");
            }
            return true;
        }
    }
    return false;
}

void
OptionsCont::printHelp(std::ostream& os) {
    std::vector<std::string>::const_iterator i, j;
    // print application description
    splitLines(os, myAppDescription, 0, 0);
    os << std::endl;
    // print usage BNF
    os << "Usage: " << myAppName << " [OPTION]*" << std::endl;
    // print additional text if any
    if (myAdditionalMessage.length() > 0) {
        os << myAdditionalMessage << std::endl << ' ' << std::endl;
    }
    // print the options
    // check their sizes first
    //  we want to know how large the largest not-too-large-entry will be
    size_t tooLarge = 40;
    size_t maxSize = 0;
    for (i = mySubTopics.begin(); i != mySubTopics.end(); ++i) {
        const std::vector<std::string>& entries = mySubTopicEntries[*i];
        for (j = entries.begin(); j != entries.end(); ++j) {
            Option* o = getSecure(*j);
            // name, two leading spaces and "--"
            size_t csize = (*j).length() + 2 + 4;
            // abbreviation length ("-X, "->4chars) if any
            std::vector<std::string> synonymes = getSynonymes(*j);
            if (find_if(synonymes.begin(), synonymes.end(), abbreviation_finder()) != synonymes.end()) {
                csize += 4;
            }
            // the type name
            if (!o->isBool()) {
                csize += 1 + o->getTypeName().length();
            }
            // divider
            csize += 2;
            if (csize < tooLarge && maxSize < csize) {
                maxSize = csize;
            }
        }
    }

    for (i = mySubTopics.begin(); i != mySubTopics.end(); ++i) {
        os << *i << " Options:" << std::endl;
        const std::vector<std::string>& entries = mySubTopicEntries[*i];
        for (j = entries.begin(); j != entries.end(); ++j) {
            // start length computation
            size_t csize = (*j).length() + 2;
            Option* o = getSecure(*j);
            os << "  ";
            // write abbreviation if given
            std::vector<std::string> synonymes = getSynonymes(*j);
            std::vector<std::string>::iterator a = find_if(synonymes.begin(), synonymes.end(), abbreviation_finder());
            if (a != synonymes.end()) {
                os << '-' << (*a) << ", ";
                csize += 4;
            }
            // write leading '-'/"--"
            os << "--";
            csize += 2;
            // write the name
            os << *j;
            // write the type if not a bool option
            if (!o->isBool()) {
                os << ' ' << o->getTypeName();
                csize += 1 + o->getTypeName().length();
            }
            csize += 2;
            // write the description formatting it
            os << "  ";
            for (size_t r = maxSize; r > csize; --r) {
                os << ' ';
            }
            size_t offset = csize > tooLarge ? csize : maxSize;
            splitLines(os, o->getDescription(), offset, maxSize);
        }
        os << std::endl;
    }
    os << std::endl;
    // print usage examples, calc size first
    if (myCallExamples.size() != 0) {
        os << "Examples:" << std::endl;
        for (std::vector<std::pair<std::string, std::string> >::const_iterator e = myCallExamples.begin(); e != myCallExamples.end(); ++e) {
            os << "  " << myAppName << ' ' << e->first << std::endl;
            os << "    " << e->second << std::endl;
        }
    }
    os << std::endl;
    os << "Report bugs at <http://sumo-sim.org/trac/>." << std::endl;
    os << "Get in contact via <sumo-user@lists.sourceforge.net>." << std::endl;
}


void
OptionsCont::writeConfiguration(std::ostream& os, bool filled,
                                bool complete, bool addComments) const {
    os << "<?xml version=\"1.0\"" << SUMOSAXAttributes::ENCODING << "?>\n\n";
    os << "<configuration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/" << myAppName << "Configuration.xsd\">" << std::endl << std::endl;
    for (std::vector<std::string>::const_iterator i = mySubTopics.begin(); i != mySubTopics.end(); ++i) {
        std::string subtopic = *i;
        if (subtopic == "Configuration" && !complete) {
            continue;
        }
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        std::transform(subtopic.begin(), subtopic.end(), subtopic.begin(), tolower);
        const std::vector<std::string>& entries = mySubTopicEntries.find(*i)->second;
        bool hadOne = false;
        for (std::vector<std::string>::const_iterator j = entries.begin(); j != entries.end(); ++j) {
            Option* o = getSecure(*j);
            bool write = complete || (filled && !o->isDefault());
            if (!write) {
                continue;
            }
            if (!hadOne) {
                os << "    <" << subtopic << ">" << std::endl;
            }
            // add the comment if wished
            if (addComments) {
                os << "        <!-- " << StringUtils::escapeXML(o->getDescription()) << " -->" << std::endl;
            }
            // write the option and the value (if given)
            os << "        <" << *j << " value=\"";
            if (o->isSet() && (filled || o->isDefault())) {
                os << o->getValueString();
            }
            if (complete) {
                std::vector<std::string> synonymes = getSynonymes(*j);
                if (!synonymes.empty()) {
                    os << "\" synonymes=\"";
                    for (std::vector<std::string>::const_iterator s = synonymes.begin(); s != synonymes.end(); ++s) {
                        if (s != synonymes.begin()) {
                            os << " ";
                        }
                        os << (*s);
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
OptionsCont::writeSchema(std::ostream& os, bool /* addComments */) {
    os << "<?xml version=\"1.0\"" << SUMOSAXAttributes::ENCODING << "?>\n\n";
    os << "<xsd:schema elementFormDefault=\"qualified\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n\n";
    os << "    <xsd:include schemaLocation=\"baseTypes.xsd\"/>\n";
    os << "    <xsd:element name=\"configuration\" type=\"configurationType\"/>\n\n";
    os << "    <xsd:complexType name=\"configurationType\">\n";
    os << "        <xsd:all>\n";
    for (std::vector<std::string>::const_iterator i = mySubTopics.begin(); i != mySubTopics.end(); ++i) {
        std::string subtopic = *i;
        if (subtopic == "Configuration") {
            continue;
        }
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        std::transform(subtopic.begin(), subtopic.end(), subtopic.begin(), tolower);
        os << "            <xsd:element name=\"" << subtopic << "\" type=\"" << subtopic << "Type\" minOccurs=\"0\"/>\n";
    }
    os << "        </xsd:all>\n";
    os << "    </xsd:complexType>\n\n";
    for (std::vector<std::string>::const_iterator i = mySubTopics.begin(); i != mySubTopics.end(); ++i) {
        std::string subtopic = *i;
        if (subtopic == "Configuration") {
            continue;
        }
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        std::transform(subtopic.begin(), subtopic.end(), subtopic.begin(), tolower);
        os << "    <xsd:complexType name=\"" << subtopic << "Type\">\n";
        os << "        <xsd:all>\n";
        const std::vector<std::string>& entries = mySubTopicEntries[*i];
        for (std::vector<std::string>::const_iterator j = entries.begin(); j != entries.end(); ++j) {
            Option* o = getSecure(*j);
            std::string type = o->getTypeName();
            std::transform(type.begin(), type.end(), type.begin(), tolower);
            if (type == "int[]") {
                type = "intArray";
            }
            os << "            <xsd:element name=\"" << *j << "\" type=\"" << type << "OptionType\" minOccurs=\"0\"/>\n";
        }
        os << "        </xsd:all>\n";
        os << "    </xsd:complexType>\n\n";
    }
    os << "</xsd:schema>\n";
}


void
OptionsCont::writeXMLHeader(std::ostream& os) {
    time_t rawtime;
    char buffer [80];

    os << "<?xml version=\"1.0\"" << SUMOSAXAttributes::ENCODING << "?>\n\n";
    time(&rawtime);
    strftime(buffer, 80, "<!-- generated on %c by ", localtime(&rawtime));
    os << buffer << myFullName << "\n";
    writeConfiguration(os, true, false, false);
    os << "-->\n\n";
}


std::vector<std::string>
OptionsCont::getStringVector(const std::string& name) const {
    Option* o = getSecure(name);
    std::string def = o->getString();
    if (def.find(';') != std::string::npos && !myHaveInformedAboutDeprecatedDivider) {
        WRITE_WARNING("Please note that using ';' as list separator is deprecated.\n From 1.0 onwards, only ',' will be accepted.");
        myHaveInformedAboutDeprecatedDivider = true;
    }
    StringTokenizer st(def, ";,", true);
    std::vector<std::string> ret = st.getVector();
    for (std::vector<std::string>::iterator i = ret.begin(); i != ret.end(); ++i) {
        (*i) = StringUtils::prune(*i);
    }
    return ret;
}


bool
OptionsCont::isInStringVector(const std::string& optionName,
                              const std::string& itemName) {
    if (isSet(optionName)) {
        std::vector<std::string> values = getStringVector(optionName);
        return find(values.begin(), values.end(), itemName) != values.end();
    }
    return false;
}


/****************************************************************************/
