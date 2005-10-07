//---------------------------------------------------------------------------//
//                        GUISupplementaryWeightsHandler.cpp -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu Apr 08 2004
//  copyright            : (C) 2005 by Christian Roessel
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : christian.roessel@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.7  2005/10/07 11:36:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/22 13:30:40  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/07/12 11:59:01  dkrajzew
// level 3 warnings removed; code style adapted
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

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
        WRITE_WARNING("GUISupplementaryWeightsHandler::myStartElement wrong ");
        WRITE_WARNING(

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
            absolutValueM      = TplConvert<char>::_2SUMOReal( attrValue.c_str());
            isAbsolutValueSetM = true;
        }
        else if ( attrName == string( "mult" ) ){
            multValueM      = TplConvert<char>::_2SUMOReal( attrValue.c_str() );
            isMultValueSetM = true;
        }
        else if ( attrName == string( "add" ) ){
            addValueM      = TplConvert<char>::_2SUMOReal( attrValue.c_str() );
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
        aw.abs = (SUMOReal) absolutValueM;
        isAbsolutValueSetM = false;
    }
    if ( isMultValueSetM ){
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.mult = (SUMOReal) multValueM;
        isMultValueSetM = false;
    }
    if ( isAddValueSetM ){
        aw.edgeID = edgeIdM;
        aw.timeBeg = intervalStartM;
        aw.timeEnd = intervalEndM;
        aw.add = (SUMOReal) addValueM;
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
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
