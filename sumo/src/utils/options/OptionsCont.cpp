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
// used namespaces
// ===========================================================================
using namespace std;


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
        : myAddresses(), myValues(), myHaveInformedAboutDeprecatedDivider(false) {}


OptionsCont::~OptionsCont() throw() {
    clear();
}


void
OptionsCont::doRegister(const string &name, Option *v) throw(InvalidArgument) {
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
OptionsCont::doRegister(const string &name1, char abbr, Option *v) throw(InvalidArgument) {
    doRegister(name1, v);
    doRegister(convertChar(abbr), v);
}


void
OptionsCont::addSynonyme(const string &name1, const string &name2) throw(InvalidArgument) {
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
OptionsCont::exists(const string &name) const throw() {
    KnownContType::const_iterator i = myValues.find(name);
    return i!=myValues.end();
}


bool
OptionsCont::isSet(const string &name) const throw(InvalidArgument) {
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
OptionsCont::getSecure(const string &name) const throw(InvalidArgument) {
    KnownContType::const_iterator i = myValues.find(name);
    if (i==myValues.end()) {
        throw InvalidArgument("No option with the name '" + name + "' exists.");
    }
    return (*i).second;
}


string
OptionsCont::getString(const string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getString();
}


SUMOReal
OptionsCont::getFloat(const string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getFloat();
}


int
OptionsCont::getInt(const string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getInt();
}


bool
OptionsCont::getBool(const string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getBool();
}


const IntVector &
OptionsCont::getIntVector(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->getIntVector();
}


bool
OptionsCont::set(const string &name, const string &value) throw(InvalidArgument) {
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


bool
OptionsCont::set(const string &name, bool value) throw(InvalidArgument) {
    Option *o = getSecure(name);
    if (!o->isBool()) {
        throw InvalidArgument("The option '" + name + "' is not a boolean attribute and requires an argument.");
    }
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


vector<string>
OptionsCont::getSynonymes(const string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    vector<string> v(0);
    for (KnownContType::const_iterator i=myValues.begin(); i!=myValues.end(); i++) {
        if ((*i).second==o&&name!=(*i).first) {
            v.push_back((*i).first);
        }
    }
    return v;
}


ostream&
operator<<(ostream& os, const OptionsCont& oc) {
    vector<string> done;
    os << "Options set:" << endl;
    for (OptionsCont::KnownContType::const_iterator i=oc.myValues.begin();
            i!=oc.myValues.end(); i++) {
        vector<string>::iterator j = find(done.begin(), done.end(), (*i).first);
        if (j==done.end()) {
            vector<string> synonymes = oc.getSynonymes((*i).first);
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
                os << ": " << (*i).second->getValueString() << endl;
            } else {
                os << ": <INVALID>" << endl;
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
    vector<string> files = getStringVector(name);
    if (files.size()==0) {
        MsgHandler::getErrorInstance()->inform("The file list for '" + name + "' is empty.");
        ok = false;
    }
    for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
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
OptionsCont::reportDoubleSetting(const string &arg) const throw() {
    vector<string> synonymes = getSynonymes(arg);
    ostringstream s;
    s << "A value for the option '" + arg + "' was already set.\n Possible synonymes: ";
    for (vector<string>::iterator i=synonymes.begin(); i!=synonymes.end();) {
        s << (*i);
        i++;
        if (i!=synonymes.end()) {
            s << ", ";
        }
    }
    MsgHandler::getErrorInstance()->inform(s.str());
}


string
OptionsCont::convertChar(char abbr) const throw() {
    char buf[2];
    buf[0] = abbr;
    buf[1] = 0;
    string s(buf);
    return s;
}


bool
OptionsCont::isBool(const string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    return o->isBool();
}


void
OptionsCont::resetWritable() throw() {
    for (ItemAddressContType::iterator i=myAddresses.begin(); i!=myAddresses.end(); i++) {
        (*i)->myAmWritable = true;
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
    o->myDescription = description;
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
OptionsCont::addOptionSubTopic(const std::string &topic) throw() {
    mySubTopics.push_back(topic);
    mySubTopicEntries[topic] = vector<string>();
}


void
OptionsCont::splitLines(std::ostream &os, std::string what,
                        size_t offset, size_t nextOffset) throw() {
    while (what.length()>0) {
        if (what.length()>79-offset) {
            size_t splitPos = what.rfind(';', 79-offset);
            if (splitPos==string::npos) {
                splitPos = what.rfind(' ', 79-offset);
            } else {
                splitPos++;
            }
            if (splitPos!=string::npos) {
                os << what.substr(0, splitPos) << endl;
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
    os << endl;
}


bool
OptionsCont::processMetaOptions(bool missingOptions) throw(ProcessError) {
    if (missingOptions) {
        // no options are given
        cout << myFullName << endl;
        cout << " (c) DLR 2001-2009; http://sumo.sourceforge.net" << endl;
        cout << " Use --help to get the list of options." << endl;
        return true;
    }

    OptionsCont &oc = OptionsCont::getOptions();
    // check whether the help shall be printed
    if (oc.getBool("help")) {
        cout << myFullName << endl;
        cout << " (c) DLR 2001-2009; http://sumo.sourceforge.net" << endl;
        oc.printHelp(cout);
        return true;
    }
    // check whether the settings shall be printed
    if (oc.getBool("print-options")) {
        cout << oc;
    }
    // check whether something has to be done with options
    // whether the current options shall be saved
    if (oc.isSet("save-configuration")) {
        ofstream out(oc.getString("save-configuration").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save configuration to '" + oc.getString("save-configuration") + "'");
        } else {
            oc.writeConfiguration(out, true, false, false);
            if (oc.getBool("verbose")) {
                MsgHandler::getMessageInstance()->inform("Written configuration to '" + oc.getString("save-configuration") + "'");
            }
            return true;
        }
    }
    // whether the template shall be saved
    if (oc.isSet("save-template")) {
        ofstream out(oc.getString("save-template").c_str());
        if (!out.good()) {
            throw ProcessError("Could not save template to '" + oc.getString("save-template") + "'");
        } else {
            oc.writeConfiguration(out, false, true, oc.getBool("save-template.commented"));
            if (oc.getBool("verbose")) {
                MsgHandler::getMessageInstance()->inform("Written template to '" + oc.getString("save-template") + "'");
            }
            return true;
        }
    }
    return false;
}

void
OptionsCont::printHelp(std::ostream &os) throw() {
    vector<string>::const_iterator i, j;
    // print application description
    os << ' ' << endl;
    splitLines(os, myAppDescription , 0, 0);
    os << endl;
    // print usage BNF
    os << "Usage: " << myAppName << " [OPTION]*" << endl;
    os << ' ' << endl;
    // print usage examples
    if (myCallExamples.size()>1) {
        os << " Examples:" << endl;
    } else if (myCallExamples.size()!=0) {
        os << " Example:" << endl;
    }
    if (myCallExamples.size()!=0) {
        for (i=myCallExamples.begin(); i!=myCallExamples.end(); ++i) {
            os << "  " << myAppName << ' ' << (*i) << endl;
        }
    }
    os << ' ' << endl;
    // print additional text if any
    if (myAdditionalMessage.length()>0) {
        os << myAdditionalMessage << endl << ' ' << endl;
    }
    // print the options
    // check their sizes first
    //  we want to know how large the largest not-too-large-entry will be
    size_t tooLarge = 40;
    size_t maxSize = 0;
    for (i=mySubTopics.begin(); i!=mySubTopics.end(); ++i) {
        const vector<string> &entries = mySubTopicEntries[*i];
        for (j=entries.begin(); j!=entries.end(); ++j) {
            Option *o = getSecure(*j);
            // name, two leading spaces and "--"
            size_t csize = (*j).length() + 2 + 4;
            // abbreviation length ("-X, "->4chars) if any
            vector<string> synonymes = getSynonymes(*j);
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
        os << ' ' << *i << " Options:" << endl;
        const vector<string> &entries = mySubTopicEntries[*i];
        for (j=entries.begin(); j!=entries.end(); ++j) {
            // start length computation
            size_t csize = (*j).length() + 2;
            Option *o = getSecure(*j);
            os << "  ";
            // write abbreviation if given
            vector<string> synonymes = getSynonymes(*j);
            vector<string>::iterator a = find_if(synonymes.begin(), synonymes.end(), abbreviation_finder());
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
            string desc = o->getDescription();
            size_t offset = csize > tooLarge ? csize : maxSize;
            splitLines(os, desc, offset, maxSize);
        }
        os << endl;
    }
}


void
OptionsCont::writeConfiguration(std::ostream &os, bool filled,
                                bool complete, bool addComments) throw() {
    vector<string>::const_iterator i, j;
    os << "<configuration>" << endl << endl;
    for (i=mySubTopics.begin(); i!=mySubTopics.end(); ++i) {
        string subtopic = *i;
        if (subtopic=="Configuration") {
            continue;
        }
        for (size_t k=0; k<subtopic.length(); ++k) {
            if (subtopic[k]==' ') {
                subtopic[k] = '_';
            }
            if (subtopic[k]>='A'&&subtopic[k]<='Z') {
                subtopic[k] = subtopic[k] - 'A' + 'a';
            }
        }
        const vector<string> &entries = mySubTopicEntries[*i];
        bool hadOne = false;
        for (j=entries.begin(); j!=entries.end(); ++j) {
            Option *o = getSecure(*j);
            bool write = complete || (filled&&!o->isDefault());
            if (!write) {
                continue;
            }
            if (!hadOne) {
                os << "    <" << subtopic << ">" << endl;
            }
            // add the comment if wished
            if (addComments) {
                os << "        <!-- " << o->getDescription() << " -->" << endl;
            }
            // write the option and the value (if given)
            os << "        <" << *j << " value=\"";
            if (o->isSet()) {
                os << o->getValueString();
            }
            os << "\"/>" << endl;
            // append an endline if a comment was printed
            if (addComments) {
                os << endl;
            }
            hadOne = true;
        }
        if (hadOne) {
            os << "    </" << subtopic << ">" << endl << endl;
        }
    }
    os << "</configuration>" << endl;
}


void
OptionsCont::writeXMLHeader(std::ostream &os, const bool writeConfig) throw() {
    time_t rawtime;
    char buffer [80];

    os << "<?xml version=\"1.0\"?>\n\n";
    time(&rawtime);
    strftime(buffer, 80, "<!-- generated on %c by ", localtime(&rawtime));
    os << buffer << myFullName << "\n";
    if (writeConfig) {
        writeConfiguration(os, true, false, false);
    }
    os << "-->\n\n";
}


std::vector<std::string>
OptionsCont::getStringVector(const std::string &name) const throw(InvalidArgument) {
    Option *o = getSecure(name);
    string def = o->getString();
    if (def.find(';')!=string::npos&&!myHaveInformedAboutDeprecatedDivider) {
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
        vector<string> values = getStringVector(optionName);
        return find(values.begin(), values.end(), itemName)!=values.end();
    }
    return false;
}


/****************************************************************************/
