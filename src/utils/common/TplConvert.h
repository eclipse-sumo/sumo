/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TplConvert.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sun, 09 Jun 2002
/// @version $Id$
///
// Some conversion methods (from strings to other)
/****************************************************************************/
#ifndef TplConvert_h
#define TplConvert_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <sstream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/TranscodingException.hpp>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * TplConvert
 * Some methods that convert 0-terminated XMLCh-arrays into std::strings
 */
class TplConvert {
public:
    /// @name Conversion of basic types and data arrays to String
    //@{
    /**@brief converts a 0-terminated char-type array into std::string
     * @throw an EmptyData - exception if the given pointer is 0
     */
    static inline std::string _2str(const XMLCh* const data) {
        return _2str(data, (int)XERCES_CPP_NAMESPACE::XMLString::stringLen(data));
    }

    /**@brief converts a char-type array into std::string considering the given length
     * @throw an EmptyData - exception if the given pointer is 0
     */
    static inline std::string _2str(const XMLCh* const data, int length) {
        if (data == 0) {
            throw EmptyData();
        }
        if (length == 0) {
            return "";
        }
#if _XERCES_VERSION < 30100
        char* t = XERCES_CPP_NAMESPACE::XMLString::transcode(data);
        std::string result(t);
        XERCES_CPP_NAMESPACE::XMLString::release(&t);
        return result;
#else
        try {
            XERCES_CPP_NAMESPACE::TranscodeToStr utf8(data, "UTF-8");
            return reinterpret_cast<const char*>(utf8.str());
        } catch (XERCES_CPP_NAMESPACE::TranscodingException&) {
            return "?";
        }
#endif
    }
    //@}
};


#endif

/****************************************************************************/
