///
/// @file    ROSupplementaryWeightsHandler.cpp
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

#include "ROSupplementaryWeightsHandler.h"

#include <string>
#include <utility>
#include <utils/options/OptionsCont.h>
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
#include "ROEdge.h"
#include "RONet.h"

using namespace std;

ROSupplementaryWeightsHandler::ROSupplementaryWeightsHandler(
    OptionsCont& optionCont
    , RONet& net
    , const std::string& filename ) :
    SUMOSAXHandler( "sumo-supplementary-netweights", filename )
    , optionsM( optionCont )
    , netM( net )
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


ROSupplementaryWeightsHandler::~ROSupplementaryWeightsHandler()
{
    absolutMapM.clear();
    multMapM.clear();
    addMapM.clear();
    weightedEdgesM.clear();
}

void
ROSupplementaryWeightsHandler::myStartElement( int 
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
    else {
        cerr << "ROSupplementaryWeightsHandler::myStartElement wrong "
             << "attribute " << name << endl;
        assert( false );
    }
}

void
ROSupplementaryWeightsHandler::myEndElement( int, const std::string& name )
{
    if ( name == string( "supplementary-weights" ) ) {
        stopParseSupplementaryWeights();
    }
    else if ( name == string( "interval" ) ) {
        stopParseInterval();
    }
    else if ( name == string( "weight" ) ) {
        stopParseWeight();
    }
    else {
        cerr << "ROSupplementaryWeightsHandler::myEndElement wrong "
             << "attribute \"" << name << "\"." << endl;
        assert( false );
    }
}

void
ROSupplementaryWeightsHandler::startParseSupplementaryWeights(
    const Attributes& attrs )
{
    assert( attrs.getLength() == 0 );
    assert( ! hasStartedSupplementaryWeightsM );
    assert( ! hasStartedIntervalM );
    assert( ! hasStartedWeightM );
    hasStartedSupplementaryWeightsM = true;
}

void
ROSupplementaryWeightsHandler::startParseInterval( const Attributes& attrs )
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
ROSupplementaryWeightsHandler::startParseWeight( const Attributes& attrs )
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
        else {
            cerr << "ROSupplementaryWeightsHandler::startParseWeight wrong "
                 << "attribute name \"" << attrName << "\"." << endl;
            assert( false );
        }
    }

    assert( isEdgeIdSetM );
    if ( isAbsolutValueSetM ){
        insertValuedTimeRangeIntoMap( absolutMapM, absolutValueM );
    }
    if ( isMultValueSetM ){
        insertValuedTimeRangeIntoMap( multMapM, multValueM );
    }
    if ( isAddValueSetM ){
        insertValuedTimeRangeIntoMap( addMapM, addValueM );
    }
    weightedEdgesM.insert( edgeIdM );
}

void
ROSupplementaryWeightsHandler::insertValuedTimeRangeIntoMap(
    WeightsMap& aMap
    , double aValue )
{
    WeightsMapIt iter = aMap.find( edgeIdM );
    if ( iter == aMap.end() ){
        FloatValueTimeLine* valueTimeLine = new FloatValueTimeLine();
        iter = aMap.insert( make_pair( edgeIdM, valueTimeLine ) ).first;
    }
    iter->second->addValue( intervalStartM,
                            intervalEndM,
                            aValue );
}


void
ROSupplementaryWeightsHandler::stopParseSupplementaryWeights( void )
{
    assert( hasStartedSupplementaryWeightsM );
    assert( ! hasStartedIntervalM );
    assert( ! hasStartedWeightM );
    // Do not allow several "supplementary-weights" tags, therefore don't
    // reset hasStartedSupplementaryWeights.

    // Pass timeValueLines to edges
    for ( EdgeSetIt edgeIt = weightedEdgesM.begin();
          edgeIt != weightedEdgesM.end(); ++edgeIt ) {
        
        string edgeId = *edgeIt;
        FloatValueTimeLine* absolut =
            getFloatValueTimeLine( absolutMapM, edgeId );
        FloatValueTimeLine* mult = getFloatValueTimeLine( multMapM, edgeId );
        FloatValueTimeLine* add = getFloatValueTimeLine( addMapM, edgeId );
        
        netM.getEdge( edgeId )->setSupplementaryWeights( absolut, add, mult );
    }
}

FloatValueTimeLine*
ROSupplementaryWeightsHandler::getFloatValueTimeLine( WeightsMap& aMap,
                                                      string aEdgeId )
{
    WeightsMapIt it = aMap.find( aEdgeId );
    FloatValueTimeLine* retVal = 0;
    if ( it == aMap.end() ) {
        retVal = new FloatValueTimeLine();
    }
    else {
        retVal = it->second;
    }
    return retVal;
}


void
ROSupplementaryWeightsHandler::stopParseInterval( void )
{
    assert( hasStartedSupplementaryWeightsM );
    assert( hasStartedIntervalM );
    assert( ! hasStartedWeightM );
    hasStartedIntervalM = false;
}

void
ROSupplementaryWeightsHandler::stopParseWeight( void )
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
