///
/// @file    GUISupplementaryWeightsHandler.cpp
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Thu Apr 08 2004 15:31 CEST
/// @version $Id$
///
/// @brief
///
///

/* Copyright (C) 2004 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#include "GUISupplementaryWeightsHandler.h"

#include <string>
#include <utility>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/router/FloatValueTimeLine.h>
#include <utils/convert/TplConvert.h>
#include "GUIGlobals.h"
#include "GUIAddWeightsStorage.h"

using namespace std;

GUISupplementaryWeightsHandler::GUISupplementaryWeightsHandler(
        const std::string& filename )
    : SUMOSAXHandler( "sumo-supplementary-netweights", filename )
    , hasStartedSupplementaryWeightsM( false )
    , hasStartedIntervalM( false )
    , hasStartedWeightM( false )
    , isEdgeIdSetM( false )
    , isAbsolutValueSetM( false )
    , isMultValueSetM( false )
    , isAddValueSetM( false )
    , intervalStartM( 0 )
    , intervalEndM( 0 )
    , edgeIdM( "" )
    , absolutValueM( 0 )
    , multValueM( 0 )
    , addValueM( 0 )
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
GUISupplementaryWeightsHandler::myStartElement( int
                                               , const std::string& name
                                               , const Attributes& attrs )
{
    if ( name == string( "supplementary-weights" ) ) {
        startParseSupplementaryWeights( attrs );
    }
    else if ( name == string( "interval" ) ) {
        startParseInterval( attrs );
    }
    else if ( name == string( "weight" ) ) {
        startParseWeight( attrs );
    }
/*    else {
        MsgHandler::getErrorInstance()->inform(
            "GUISupplementaryWeightsHandler::myStartElement wrong ");
        MsgHandler::getWarningInstance()->inform(

             << "attribute " << name << endl;
        assert( false );
    }*/
}

void
GUISupplementaryWeightsHandler::myEndElement( int, const std::string& name )
{
    if ( name == string( "interval" ) ) {
        stopParseInterval();
    }
    else if ( name == string( "weight" ) ) {
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
    const Attributes& attrs )
{
    assert( attrs.getLength() == 0 );
    assert( ! hasStartedSupplementaryWeightsM );
    assert( ! hasStartedIntervalM );
    assert( ! hasStartedWeightM );
    hasStartedSupplementaryWeightsM = true;
}

void
GUISupplementaryWeightsHandler::startParseInterval( const Attributes& attrs )
{
    assert( attrs.getLength() == 2 );
    assert( hasStartedSupplementaryWeightsM );
    assert( ! hasStartedIntervalM );
    assert( ! hasStartedWeightM );
    hasStartedIntervalM = true;

    try {
        intervalStartM = getLong( attrs, SUMO_ATTR_BEGIN );
        intervalEndM   = getLong( attrs, SUMO_ATTR_END );
    }
    catch (...) {
        MsgHandler::getErrorInstance()->inform(
            "Problems with timestep value.");
    }
}

void
GUISupplementaryWeightsHandler::startParseWeight( const Attributes& attrs )
{
    assert( attrs.getLength() >= 2 );
    assert( hasStartedSupplementaryWeightsM );
    assert( hasStartedIntervalM );
    assert( ! hasStartedWeightM );
    hasStartedWeightM = true;

    // Check attributes and assign them to members
    for ( unsigned index = 0; index < attrs.getLength(); ++index ) {
        const string attrName(
            TplConvert<XMLCh>::_2str( attrs.getLocalName( index ) ) );
        const string attrValue(
            TplConvert<XMLCh>::_2str( attrs.getValue( index ) ) );

        if ( attrName == string( "edge-id" ) ){
            edgeIdM      = attrValue;
            isEdgeIdSetM = true;
        }
        else if ( attrName == string( "absolut" ) ){
            absolutValueM      = TplConvert<char>::_2float( attrValue.c_str());
            isAbsolutValueSetM = true;
        }
        else if ( attrName == string( "mult" ) ){
            multValueM      = TplConvert<char>::_2float( attrValue.c_str() );
            isMultValueSetM = true;
        }
        else if ( attrName == string( "add" ) ){
            addValueM      = TplConvert<char>::_2float( attrValue.c_str() );
            isAddValueSetM = true;
        }
/*        else {
            err << "GUISupplementaryWeightsHandler::startParseWeight wrong "
                 << "attribute name \"" << attrName << "\"." << endl;
            assert( false );
        }*/
    }

    assert( isEdgeIdSetM );
    GUIAddWeight aw;
    if ( isAbsolutValueSetM ){
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.abs = (float) absolutValueM;
        isAbsolutValueSetM = false;
    }
    if ( isMultValueSetM ){
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.mult = (float) multValueM;
        isMultValueSetM = false;
    }
    if ( isAddValueSetM ){
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.add = (float) addValueM;
        isAddValueSetM = false;
    }
    gAddWeightsStorage.push_back(aw);
}




void
GUISupplementaryWeightsHandler::stopParseInterval( void )
{
    assert( hasStartedSupplementaryWeightsM );
    assert( hasStartedIntervalM );
    assert( ! hasStartedWeightM );
    hasStartedIntervalM = false;
}

void
GUISupplementaryWeightsHandler::stopParseWeight( void )
{
    assert( hasStartedSupplementaryWeightsM );
    assert( hasStartedIntervalM );
    assert( hasStartedWeightM );
    hasStartedWeightM   = false;
    isEdgeIdSetM       = false;
    isAbsolutValueSetM = false;
    isMultValueSetM    = false;
    isAddValueSetM     = false;
}
