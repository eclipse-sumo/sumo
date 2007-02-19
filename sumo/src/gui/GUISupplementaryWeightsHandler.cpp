/****************************************************************************/
/// @file    GUISupplementaryWeightsHandler.cpp
/// @author  Christian Roessel
/// @date    Thu Apr 08 2004
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================

#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUISupplementaryWeightsHandler.h"

#include <string>
#include <utility>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/router/FloatValueTimeLine.h>
#include <utils/common/TplConvert.h>
#include "GUIGlobals.h"
#include "GUIAddWeightsStorage.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

GUISupplementaryWeightsHandler::GUISupplementaryWeightsHandler(
    const std::string& filename)
        : SUMOSAXHandler("sumo-supplementary-netweights", filename)
        , hasStartedSupplementaryWeightsM(false)
        , hasStartedIntervalM(false)
        , hasStartedWeightM(false)
        , isEdgeIdSetM(false)
        , isAbsolutValueSetM(false)
        , isMultValueSetM(false)
        , isAddValueSetM(false)
        , intervalStartM(0)
        , intervalEndM(0)
        , edgeIdM("")
        , absolutValueM(0)
        , multValueM(0)
        , addValueM(0)
        , absolutMapM()
        , multMapM()
        , addMapM()
        , weightedEdgesM()
{}


GUISupplementaryWeightsHandler::~GUISupplementaryWeightsHandler()
{
    absolutMapM.clear();
    multMapM.clear();
    addMapM.clear();
    weightedEdgesM.clear();
}

void
GUISupplementaryWeightsHandler::myStartElement(int
        , const std::string& name
        , const Attributes& attrs)
{
    if (name == "supplementary-weights") {
        startParseSupplementaryWeights(attrs);
    } else if (name == "interval") {
        startParseInterval(attrs);
    } else if (name == "weight") {
        startParseWeight(attrs);
    }
    /*    else {
            WRITE_WARNING("GUISupplementaryWeightsHandler::myStartElement wrong ");
            WRITE_WARNING(

                 << "attribute " << name << endl;
            assert( false );
        }*/
}

void
GUISupplementaryWeightsHandler::myEndElement(int, const std::string& name)
{
    if (name == "interval") {
        stopParseInterval();
    } else if (name == "weight") {
        stopParseWeight();
    }
    /*    else {
            err << "GUISupplementaryWeightsHandler::myEndElement wrong "
                 << "attribute \"" << name << "\"." << endl;
            assert( false );
        }*/
}

void
GUISupplementaryWeightsHandler::startParseSupplementaryWeights(
    const Attributes& attrs)
{
    assert(attrs.getLength() == 0);
    assert(! hasStartedSupplementaryWeightsM);
    assert(! hasStartedIntervalM);
    assert(! hasStartedWeightM);
    hasStartedSupplementaryWeightsM = true;
}

void
GUISupplementaryWeightsHandler::startParseInterval(const Attributes& attrs)
{
    assert(attrs.getLength() == 2);
    assert(hasStartedSupplementaryWeightsM);
    assert(! hasStartedIntervalM);
    assert(! hasStartedWeightM);
    hasStartedIntervalM = true;

    try {
        intervalStartM = getLong(attrs, SUMO_ATTR_BEGIN);
        intervalEndM   = getLong(attrs, SUMO_ATTR_END);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Problems with timestep value.");
    }
}

void
GUISupplementaryWeightsHandler::startParseWeight(const Attributes& attrs)
{
    assert(attrs.getLength() >= 2);
    assert(hasStartedSupplementaryWeightsM);
    assert(hasStartedIntervalM);
    assert(! hasStartedWeightM);
    hasStartedWeightM = true;

    // Check attributes and assign them to members
    for (unsigned index = 0; index < attrs.getLength(); ++index) {
        const string attrName(
            TplConvert<XMLCh>::_2str(attrs.getLocalName(index)));
        const string attrValue(
            TplConvert<XMLCh>::_2str(attrs.getValue(index)));

        if (attrName == "edge-id") {
            edgeIdM      = attrValue;
            isEdgeIdSetM = true;
        } else if (attrName == "absolut") {
            absolutValueM      = TplConvert<char>::_2SUMOReal(attrValue.c_str());
            isAbsolutValueSetM = true;
        } else if (attrName == "mult") {
            multValueM      = TplConvert<char>::_2SUMOReal(attrValue.c_str());
            isMultValueSetM = true;
        } else if (attrName == "add") {
            addValueM      = TplConvert<char>::_2SUMOReal(attrValue.c_str());
            isAddValueSetM = true;
        }
        /*        else {
                    err << "GUISupplementaryWeightsHandler::startParseWeight wrong "
                         << "attribute name \"" << attrName << "\"." << endl;
                    assert( false );
                }*/
    }

    assert(isEdgeIdSetM);
    GUIAddWeight aw;
    if (isAbsolutValueSetM) {
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.abs = (SUMOReal) absolutValueM;
        isAbsolutValueSetM = false;
    }
    if (isMultValueSetM) {
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.mult = (SUMOReal) multValueM;
        isMultValueSetM = false;
    }
    if (isAddValueSetM) {
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.add = (SUMOReal) addValueM;
        isAddValueSetM = false;
    }
    gAddWeightsStorage.push_back(aw);
}




void
GUISupplementaryWeightsHandler::stopParseInterval(void)
{
    assert(hasStartedSupplementaryWeightsM);
    assert(hasStartedIntervalM);
    assert(! hasStartedWeightM);
    hasStartedIntervalM = false;
}

void
GUISupplementaryWeightsHandler::stopParseWeight(void)
{
    assert(hasStartedSupplementaryWeightsM);
    assert(hasStartedIntervalM);
    assert(hasStartedWeightM);
    hasStartedWeightM   = false;
    isEdgeIdSetM       = false;
    isAbsolutValueSetM = false;
    isMultValueSetM    = false;
    isAddValueSetM     = false;
}



/****************************************************************************/

