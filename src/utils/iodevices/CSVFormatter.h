/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
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
/// @file    CSVFormatter.h
/// @author  Michael Behrisch
/// @date    2025-06-12
///
// Output formatter for CSV output
/****************************************************************************/
#pragma once
#include <config.h>

#ifdef HAVE_FMT
#include <fmt/ostream.h>
#endif

#include "OutputFormatter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CSVFormatter
 * @brief Output formatter for CSV output
 */
class CSVFormatter : public OutputFormatter {
public:
    /// @brief Constructor
    CSVFormatter(const char separator = ';');

    /// @brief Destructor
    virtual ~CSVFormatter() { }

    /** @brief This is not an xml format so it does nothing
     */
    bool writeXMLHeader(std::ostream& into, const std::string& rootElement,
                        const std::map<SumoXMLAttr, std::string>& attrs,
                        bool includeConfig = true);

    /** @brief Keeps track of an open XML tag by adding a new element to the stack
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] xmlElement Name of element to open (unused)
     * @return The OutputDevice for further processing
     */
    void openTag(std::ostream& into, const std::string& xmlElement);

    /** @brief Keeps track of an open XML tag by adding a new element to the stack
     *
     * @param[in] into The output stream to use (unused)
     * @param[in] xmlElement Name of element to open (unused)
     */
    void openTag(std::ostream& into, const SumoXMLTag& xmlElement);

    /** @brief Closes the most recently opened tag
     *
     * @param[in] into The output stream to use
     * @return Whether a further element existed in the stack and could be closed
     * @todo it is not verified that the topmost element was closed
     */
    bool closeTag(std::ostream& into, const std::string& comment = "");


    /** @brief writes a preformatted tag to the device but ensures that any
     * pending tags are closed
     * @param[in] into The output stream to use
     * @param[in] val The preformatted data
     */
    void writePreformattedTag(std::ostream& /* into */, const std::string& /* val */) {}

    /** @brief writes arbitrary padding
     */
    void writePadding(std::ostream& /* into */, const std::string& /* val */) {}

    inline void checkHeader(const SumoXMLAttr attr) {
        if (myMaxDepth == 0) {
            if (myHeader != "") {
                myHeader += mySeparator;
            }
            myHeader += toString(attr);
        }
    }

    /** @brief writes an arbitrary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    void writeAttr(std::ostream& into, const std::string& attr, const T& val) {
        if (myMaxDepth == 0) {
            if (myHeader != "") {
                myHeader += mySeparator;
            }
            myHeader += attr;
        }
        myXMLStack.back() << toString(val, into.precision()) << mySeparator;
    }

    /** @brief writes a named attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    void writeAttr(std::ostream& into, const SumoXMLAttr attr, const T& val) {
        checkHeader(attr);
        myXMLStack.back() << toString(val, into.precision()) << mySeparator;
    }

    bool wroteHeader() const {
        return myMaxDepth != 0;
    }

    void setExpectedAttrs(SumoXMLAttrMask expectedAttrs) {
        myExpectedAttrs = expectedAttrs;
    }

private:
    /// @brief the CSV header
    std::string myHeader;

    /// @brief The stack of begun xml elements
    std::vector<std::ostringstream> myXMLStack;

    /// @brief The value separator
    const char mySeparator;

    /// @brief the maximum depth of the XML hierarchy
    int myMaxDepth;

    SumoXMLAttrMask myExpectedAttrs;
    SumoXMLAttrMask mySeenAttrs;
};


// ===========================================================================
// specialized template implementations (for speedup)
// ===========================================================================
template <>
inline void CSVFormatter::writeAttr(std::ostream& into, const SumoXMLAttr attr, const double& val) {
    checkHeader(attr);
#ifdef HAVE_FMT
    fmt::print(myXMLStack.back(), "{:.{}f}{}", val, into.precision(), mySeparator);
#else
    myXMLStack.back() << toString(val, into.precision()) << mySeparator;
#endif
}


template <>
inline void CSVFormatter::writeAttr(std::ostream& /* into */, const SumoXMLAttr attr, const std::string& val) {
    checkHeader(attr);
    myXMLStack.back() << val << mySeparator;
}
