/****************************************************************************/
/// @file    OptionsCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A storage for options (typed value containers)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
OptionsCont &
OptionsCont::getOptions() throw() {
    return myOptions;
}


OptionsCont::OptionsCont() throw()
        : myAddresses(), myValues(), myHaveInformedAboutDeprecatedDivider(false) {
    myCopyrightNotices.push_back("(c) DLR 2001-2010; http://sumo.sourceforge.net");
}


OptionsCont::~OptionsCont() throw() {
    clear();
}


void
OptionsCont::doRegister(const std::string &name, Option *v) throw(InvalidArgument) {
    assert(v!=0);
    ItemAddressContType::iterator i = find(myAddresses.begin(), myAddresses.end(), v);
    if (i==myAddresses.end()) {
        myAddresses.push_back(v);
    }
    if (myValues.find(name)!=myValues.end()) {
        throw InvalidArgument(name + " is an already used option name.");
    }
    myValues[name] = v;
}


void
OptionsCont::doRegister(const std::string &name1, char abbr, Option *v) throw(InvalidArgument) {
    doRegister(name1, v);
    doRegister(convertChar(abbr), v);
}


void
OptionsCont::addSynonyme(const std::string &name1, const std::string &name2) throw(InvalidArgument) {
    KnownContType::iterator i1 = myValues.find(name1);
    KnownContType::iterator i2 = myValues.find(name2);
    if (i1==myValues.end()&&i2==myValues.end()) {
        throw InvalidArgument("Neither the option '" + name1 + "' nor the option '" + name2 + "' is known yet");
    }
    if (i1!=myValues.end()&&i2!=myValues.end()) {
        if ((*i1).second==(*i2).second) {
            return;
        }
        throw InvalidArgument("Both options '" + name1 + "' and '" + name2 + "' do exist and differ.");
    }
    if (i1==myValues.end()&&i2!=myValues.end()) {
        doRegister(name1, (*i2).second);
    }
    if (i1!=myValues.end()&&i2==myValues.end()) {
        doRegister(name2, (*i1).second);
    }
}


bool
OptionsCont::exists(const std::string &name) const throw() {
    KnownContType::const_iterator i = myValues.find(name);
    return i!=myValues.end();
}


bool
OptionsCont::isSet(const std::string &name) const throw(InvalidArgument) {
    KnownContType::const_iterator i = myValues.find(name);
    if (i==myValues.end()) {
        return false;
    }
    return (*i).second->isSet();
}


bool
OptionsCont::isDefault(const std::string &name) const throw(InvalidArgument) {
    KnownContType::const_iterator i = myValues.find(name);
    if (i==myValues.end()) {
        return false;
    }
    return (*i).second->isDefault();
}


Option *
OptionsCont::getSecure(const std::string &name) const throw(InvalidArgument) {
    KnownContType::const_iterator i = myValues.find(name);
    if (i==myValues.end()) {
        throw InvalidArgument("No option with the name '" + name + "' exists.");
    }
    return (*i).second;
}


std::string
OptionsCont::getString(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getString();
}


SUMOReal
OptionsCont::getFloat(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getFloat();
}


int
OptionsCont::getInt(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getInt();
}


bool
OptionsCont::getBool(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getBool();
}


const IntVector &
OptionsCont::getIntVector(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getIntVector();
}


bool
OptionsCont::set(const std::string &name, const std::string &value) throw(InvalidArgument) {
    Option *o = getSecure(name);
    if (!o->isWriteable()) {
        reportDoubleSetting(name);
        return false;
    }
    try {
        if (!o->set(value)) {
            return false;
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform("While processing option '" + name + "':\n " + e.what());
        return false;
    }
    return true;
}


std::vector<std::string>
OptionsCont::getSynonymes(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    std::vector<std::string> v(0);
    for (KnownContType::const_iterator i=myValues.begin(); i!=myValues.end(); i++) {
        if ((*i).second==o&&name!=(*i).first) {
            v.push_back((*i).first);
        }
    }
    return v;
}


std::ostream&
operator<<(std::ostream& os, const OptionsCont& oc) {
    std::vector<std::string> done;
    os << "Options set:" << std::endl;
    for (OptionsCont::KnownContType::const_iterator i=oc.myValues.begin();
            i!=oc.myValues.end(); i++) {
        std::vector<std::string>::iterator j = find(done.begin(), done.end(), (*i).first);
        if (j==done.end()) {
            std::vector<std::string> synonymes = oc.getSynonymes((*i).first);
            if (synonymes.size()!=0) {
                os << (*i).first << " (";
                for (j=synonymes.begin(); j!=synonymes.end(); j++) {
                    if (j!=synonymes.begin()) {
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
OptionsCont::relocateFiles(const std::string &configuration) const throw() {
    for (ItemAddressContType::const_iterator i=myAddresses.begin(); i!=myAddresses.end(); i++) {
        if ((*i)->isFileName() && (*i)->isSet()) {
            StringTokenizer st((*i)->getString(), ";, ", true);
            std::string conv;
            while (st.hasNext()) {
                if (conv.length()!=0) {
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
OptionsCont::isUsableFileList(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    // check whether the option is set
    //  return false i not
    if (!o->isSet()) {
        return false;
    }
    // check whether the list of files is valid
    bool ok = true;
    std::vector<std::string> files = getStringVector(name);
    if (files.size()==0) {
        MsgHandler::getErrorInstance()->inform("The file list for '" + name + "' is empty.");
        ok = false;
    }
    for (std::vector<std::string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
        if (!FileHelpers::exists(*fileIt)) {
            if (*fileIt!="") {
                MsgHandler::getErrorInstance()->inform("File '" + *fileIt + "' does not exist.");
                ok = false;
            } else {
                MsgHandler::getWarningInstance()->inform("Empty file name given; ignoring.");
            }
        }
    }
    return ok;
}


bool
OptionsCont::checkDependingSuboptions(const std::string &name, const std::string &prefix) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    if (o->isSet()) {
        return true;
    }
    bool ok = true;
    for (KnownContType::const_iterator i=myValues.begin(); i!=myValues.end(); i++) {
        if ((*i).second->isSet() && !(*i).second->isDefault() && (*i).first.find(prefix) == 0) {
            MsgHandler::getErrorInstance()->inform("Option '" + (*i).first + "' needs option '" + name + "'.");
            ok = false;
        }
    }
    return ok;
}


void
OptionsCont::reportDoubleSetting(const std::string &arg) const throw() {
    std::vector<std::string> synonymes = getSynonymes(arg);
    std::ostringstream s;
    s << "A value for the option '" + arg + "' was already set.\n Possible synonymes: ";
    for (std::vector<std::string>::iterator i=synonymes.begin(); i!=synonymes.end();) {
        s << (*i);
        i++;
        if (i!=synonymes.end()) {
            s << ", ";
        }
    }
    MsgHandler::getErrorInstance()->inform(s.str());
}


std::string
OptionsCont::convertChar(char abbr) const throw() {
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    std::string s(buf);
    return s;
}


bool
OptionsCont::isBool(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->isBool();
}


void
OptionsCont::resetWritable() throw() {
    for (ItemAddressContType::iterator i=myAddresses.begin(); i!=myAddresses.end(); i++) {
        (*i)->resetWritable();
    }
}


bool
OptionsCont::isWriteable(const std::string &name) throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->isWriteable();
}


void
OptionsCont::clear() throw() {
    ItemAddressContType::iterator i;
    for (i=myAddresses.begin(); i!=myAddresses.end(); i++) {
        delete(*i);
    }
    myAddresses.clear();
    myValues.clear();
    mySubTopics.clear();
    mySubTopicEntries.clear();
}


void
OptionsCont::addDescription(const std::string &name,
                            const std::string &subtopic,
                            const std::string &description) throw(InvalidArgument) {
    Option *o = getSecure(name);
    assert(o!=0);
    assert(o->myDescription=="");
    assert(find(mySubTopics.begin(), mySubTopics.end(), subtopic)!=mySubTopics.end());
    o->setDescription(description);
    mySubTopicEntries[subtopic].push_back(name);
}


void
OptionsCont::setApplicationName(const std::string &appName,
                                const std::string &fullName) throw() {
    myAppName = appName;
    myFullName = fullName;
}


void
OptionsCont::setApplicationDescription(const std::string &appDesc) throw() {
    myAppDescription = appDesc;
}


void
OptionsCont::addCallExample(const std::string &example) throw() {
    myCallExamples.push_back(example);
}


void
OptionsCont::setAdditionalHelpMessage(const std::string &add) throw() {
    myAdditionalMessage = add;
}


void
OptionsCont::addCopyrightNotice(const std::string &copyrightLine) throw() {
    myCopyrightNotices.push_back(copyrightLine);
}


void
OptionsCont::clearCopyrightNotices() throw() {
    myCopyrightNotices.clear();
}


void
OptionsCont::addOptionSubTopic(const std::string &topic) throw() {
    mySubTopics.push_back(topic);
    mySubTopicEntries[topic] = std::vector<std::string>();
}


void
OptionsCont::splitLines(std::ostream &os, std::string what,
                        size_t offset, size_t nextOffset) throw() {
    while (what.length()>0) {
        if (what.length()>79-offset) {
            size_t splitPos = what.rfind(';', 79-offset);
            if (splitPos==std::string::npos) {
                splitPos = what.rfind(' ', 79-offset);
            } else {
                splitPos++;
            }
            if (splitPos!=std::string::npos) {
                os << what.substr(0, splitPos) << std::endl;
                what = what.substr(splitPos);
                for (size_t r=0; r<nextOffset+1; ++r) {
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
OptionsCont::processMetaOptions(bool missingOptions) throw(ProcessError) {
    if (missingOptions) {
        // no options are given
        std::cout << myFullName << std::endl;
        for (std::vector<std::string>::const_iterator it =
                    myCopyrightNotices.begin(); it != myCopyrightNotices.end(); ++it) {
            std::cout << " " << *it << std::endl;
        }
        std::cout << " Use --help to get the list of options." << std::endl;
        return true;
    }

    OptionsCont &oc = OptionsCont::getOptions();
    // check whether the help shall be printed
    if (oc.getBool("help")) {
        std::cout << myFullName << std::endl;
        for (std::vector<std::string>::const_iterator it =
                    myCopyrightNotices.begin(); it != myCopyrightNotices.end(); ++it) {
            std::cout << " " << *it << std::endl;
        }
        oc.printHelp(std::cout);
        return true;
    }
    // check whether the help shall be printed
    if (oc.getBool("version")) {
        std::cout << myFullName << std::endl;
        for (std::vector<std::string>::const_iterator it =
                    myCopyrightNotices.begin(); it != myCopyrightNotices.end(); ++it) {
            std::cout << " " << *it << std::endl;
        }
        return true;
    }
    // check whether the settings shall be printed
    if (oc.exists("print-options") && oc.getBool("print-options")) {
        std::cout << oc;
    }
    // check whether something has to be done with options
    // whether the current options shall be saved
    if (oc.isSet("save-configuration")) {
        std::ofstream out(oc.getString("save-configuration").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save configuration to '" + oc.getString("save-configuration") + "'");
        } else {
            oc.writeConfiguration(out, true, false, oc.getBool("save-commented"));
            if (oc.getBool("verbose")) {
                MsgHandler::getMessageInstance()->inform("Written configuration to '" + oc.getString("save-configuration") + "'");
            }
            return true;
        }
    }
    // whether the template shall be saved
    if (oc.isSet("save-template")) {
        std::ofstream out(oc.getString("save-template").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save template to '" + oc.getString("save-template") + "'");
        } else {
            oc.writeConfiguration(out, false, true, oc.getBool("save-commented"));
            if (oc.getBool("verbose")) {
                MsgHandler::getMessageInstance()->inform("Written template to '" + oc.getString("save-template") + "'");
            }
            return true;
        }
    }
    if (oc.isSet("save-schema")) {
        std::ofstream out(oc.getString("save-schema").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save schema to '" + oc.getString("save-schema") + "'");
        } else {
            oc.writeSchema(out, oc.getBool("save-commented"));
            if (oc.getBool("verbose")) {
                MsgHandler::getMessageInstance()->inform("Written schema to '" + oc.getString("save-schema") + "'");
            }
            return true;
        }
    }
    return false;
}

void
OptionsCont::printHelp(std::ostream &os) throw() {
    std::vector<std::string>::const_iterator i, j;
    // print application description
    os << ' ' << std::endl;
    splitLines(os, myAppDescription , 0, 0);
    os << std::endl;
    // print usage BNF
    os << "Usage: " << myAppName << " [OPTION]*" << std::endl;
    os << ' ' << std::endl;
    // print usage examples
    if (myCallExamples.size()>1) {
        os << " Examples:" << std::endl;
    } else if (myCallExamples.size()!=0) {
        os << " Example:" << std::endl;
    }
    if (myCallExamples.size()!=0) {
        for (i=myCallExamples.begin(); i!=myCallExamples.end(); ++i) {
            os << "  " << myAppName << ' ' << (*i) << std::endl;
        }
    }
    os << ' ' << std::endl;
    // print additional text if any
    if (myAdditionalMessage.length()>0) {
        os << myAdditionalMessage << std::endl << ' ' << std::endl;
    }
    // print the options
    // check their sizes first
    //  we want to know how large the largest not-too-large-entry will be
    size_t tooLarge = 40;
    size_t maxSize = 0;
    for (i=mySubTopics.begin(); i!=mySubTopics.end(); ++i) {
        const std::vector<std::string> &entries = mySubTopicEntries[*i];
        for (j=entries.begin(); j!=entries.end(); ++j) {
            Option *o = getSecure(*j);
            // name, two leading spaces and "--"
            size_t csize = (*j).length() + 2 + 4;
            // abbreviation length ("-X, "->4chars) if any
            std::vector<std::string> synonymes = getSynonymes(*j);
            if (find_if(synonymes.begin(), synonymes.end(), abbreviation_finder())!=synonymes.end()) {
                csize += 4;
            }
            // the type name
            if (!o->isBool()) {
                csize += 1 + o->getTypeName().length();
            }
            // divider
            csize += 2;
            if (csize<tooLarge&&maxSize<csize) {
                maxSize = csize;
            }
        }
    }

    for (i=mySubTopics.begin(); i!=mySubTopics.end(); ++i) {
        os << ' ' << *i << " Options:" << std::endl;
        const std::vector<std::string> &entries = mySubTopicEntries[*i];
        for (j=entries.begin(); j!=entries.end(); ++j) {
            // start length computation
            size_t csize = (*j).length() + 2;
            Option *o = getSecure(*j);
            os << "  ";
            // write abbreviation if given
            std::vector<std::string> synonymes = getSynonymes(*j);
            std::vector<std::string>::iterator a = find_if(synonymes.begin(), synonymes.end(), abbreviation_finder());
            if (a!=synonymes.end()) {
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
            size_t r;
            for (r=maxSize; r>csize; --r) {
                os << ' ';
            }
            std::string desc = o->getDescription();
            size_t offset = csize > tooLarge ? csize : maxSize;
            splitLines(os, desc, offset, maxSize);
        }
        os << std::endl;
    }
}


void
OptionsCont::writeConfiguration(std::ostream &os, bool filled,
                                bool complete, bool addComments) throw() {
    std::vector<std::string>::const_iterator i, j;
    // to the best of our knowledge files are written in latin-1 on windows and linux
    os << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n\n";
    os << "<configuration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.sf.net/xsd/" << myAppName << "Configuration.xsd\">" << std::endl << std::endl;
    for (i=mySubTopics.begin(); i!=mySubTopics.end(); ++i) {
        std::string subtopic = *i;
        if (subtopic=="Configuration") {
            continue;
        }
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        std::transform(subtopic.begin(), subtopic.end(), subtopic.begin(), tolower);
        const std::vector<std::string> &entries = mySubTopicEntries[*i];
        bool hadOne = false;
        for (j=entries.begin(); j!=entries.end(); ++j) {
            Option *o = getSecure(*j);
            bool write = complete || (filled&&!o->isDefault());
            if (!write) {
                continue;
            }
            if (!hadOne) {
                os << "    <" << subtopic << ">" << std::endl;
            }
            // add the comment if wished
            if (addComments) {
                os << "        <!-- " << o->getDescription() << " -->" << std::endl;
            }
            // write the option and the value (if given)
            os << "        <" << *j << " value=\"";
            if (o->isSet()) {
                os << o->getValueString();
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
OptionsCont::writeSchema(std::ostream &os, bool addComments) throw() {
    // to the best of our knowledge files are written in latin-1 on windows and linux
    os << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n\n";
    os << "<xsd:schema elementFormDefault=\"qualified\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n\n";
    os << "    <xsd:element name=\"configuration\" type=\"configurationType\"/>\n\n";
    os << "    <xsd:complexType name=\"configurationType\">\n";
    os << "        <xsd:sequence>\n";
    for (std::vector<std::string>::const_iterator i=mySubTopics.begin(); i!=mySubTopics.end(); ++i) {
        std::string subtopic = *i;
        if (subtopic=="Configuration") {
            continue;
        }
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        std::transform(subtopic.begin(), subtopic.end(), subtopic.begin(), tolower);
        os << "            <xsd:element name=\"" << subtopic << "\" type=\"" << subtopic << "Type\" minOccurs=\"0\" maxOccurs=\"1\"/>\n";
    }
    os << "        </xsd:sequence>\n";
    os << "    </xsd:complexType>\n\n";
    for (std::vector<std::string>::const_iterator i=mySubTopics.begin(); i!=mySubTopics.end(); ++i) {
        std::string subtopic = *i;
        if (subtopic=="Configuration") {
            continue;
        }
        std::replace(subtopic.begin(), subtopic.end(), ' ', '_');
        std::transform(subtopic.begin(), subtopic.end(), subtopic.begin(), tolower);
        os << "    <xsd:complexType name=\"" << subtopic << "Type\">\n";
        os << "        <xsd:sequence>\n";
        const std::vector<std::string> &entries = mySubTopicEntries[*i];
        for (std::vector<std::string>::const_iterator j=entries.begin(); j!=entries.end(); ++j) {
            os << "            <xsd:element name=\"" << *j << "\" type=\"" << *j << "Type\" minOccurs=\"0\" maxOccurs=\"1\"/>\n";
        }
        os << "        </xsd:sequence>\n";
        os << "    </xsd:complexType>\n\n";
        for (std::vector<std::string>::const_iterator j=entries.begin(); j!=entries.end(); ++j) {
            Option *o = getSecure(*j);
            std::string type = o->getTypeName();
            std::transform(type.begin(), type.end(), type.begin(), tolower);
            if (type == "bool") {
                type = "boolean";
            } else {
                if  (type != "int" && type != "float") {
                    type = "string";
                }
            }
            os << "    <xsd:complexType name=\"" << *j << "Type\">\n";
            if (addComments) {
                os << "        <!-- " << o->getDescription() << " -->\n";
            }
            os << "        <xsd:attribute name=\"value\" type=\"xsd:" << type << "\" use=\"required\"/>\n";
            os << "    </xsd:complexType>\n\n";
        }
    }
    os << "</xsd:schema>\n";
}


void
OptionsCont::writeXMLHeader(std::ostream &os, const std::string xmlParams) throw() {
    time_t rawtime;
    char buffer [80];

    os << "<?xml version=\"1.0\"" << xmlParams << "?>\n\n";
    time(&rawtime);
    strftime(buffer, 80, "<!-- generated on %c by ", localtime(&rawtime));
    os << buffer << myFullName << "\n";
    writeConfiguration(os, true, false, false);
    os << "-->\n\n";
}


std::vector<std::string>
OptionsCont::getStringVector(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    std::string def = o->getString();
    if (def.find(';')!=std::string::npos&&!myHaveInformedAboutDeprecatedDivider) {
        MsgHandler::getWarningInstance()->inform("Please note that using ';' as list separator is deprecated.\n From 1.0 onwards, only ',' will be accepted.");
        myHaveInformedAboutDeprecatedDivider = true;
    }
    StringTokenizer st(def, ";,", true);
    std::vector<std::string> ret = st.getVector();
    for (std::vector<std::string>::iterator i=ret.begin(); i!=ret.end(); ++i) {
        (*i) = StringUtils::prune(*i);
    }
    return ret;
}


bool
OptionsCont::isInStringVector(const std::string &optionName,
                              const std::string &itemName) throw(InvalidArgument) {
    if (isSet(optionName)) {
        std::vector<std::string> values = getStringVector(optionName);
        return find(values.begin(), values.end(), itemName)!=values.end();
    }
    return false;
}


/****************************************************************************/
