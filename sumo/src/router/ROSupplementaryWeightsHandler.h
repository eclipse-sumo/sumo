#ifndef ROSUPPLEMENTARYWEIGHTSHANDLER_H
#define ROSUPPLEMENTARYWEIGHTSHANDLER_H

///
/// @file    ROSupplementaryWeightsHandler.h
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

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <set>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class RONet;
class ROEdge;
class FloatValueTimeLine;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class ROSupplementaryWeightsHandler : public SUMOSAXHandler
{
public:
    /// Constructor
    ///
    /// @param optionCont Container holding command-line options.
    /// @param net The RONet to which supplementary weights should be added.
    /// @param filename XML-file that contains the supplementary weights.
    ///
    ROSupplementaryWeightsHandler( OptionsCont& optionCont
                                   , RONet& net
                                   , const std::string& filename );

    /// Destructor
    ~ROSupplementaryWeightsHandler();

protected:
    /// Processing of start-tags. Calls one of the startParse* methods.
    ///
    /// @param element Not used.
    /// @param name Name ot the tag.
    /// @param attrs Attributes of the tag.
    ///
    void myStartElement( int element
                         , const std::string& name
                         , const Attributes& attrs );

    /// Processing of end tags. Calls one of the stopParse* methods.
    ///
    /// @param element Not used.
    /// @param name Name of the tag.
    ///
    void myEndElement( int element
                       , const std::string& name );

    /// Processing of characters. We do nothing here.
    ///
    void myCharacters(int, const std::string&, const std::string& )
        {}

    /// Call on appearance of the opening tag
    /// "supplementary-weights". Sets some state variables.
    ///
    /// @param attrs The tags attributes. Here:
    /// "supplementary-weights" need not have attributes.
    ///
    void startParseSupplementaryWeights( const Attributes& attrs );

    /// Call on appearance of the opening tag "interval". Sets some
    /// state variables and extracts the attributes "begin" and "end".
    ///
    /// @param attrs The tags attributes. Here: "interval" requires
    /// the attributes "begin" and "end".
    ///
    void startParseInterval( const Attributes& attrs );

    /// Call on appearance of the opening tag "weight". Sets some
    /// state variables and extracts the attributes "edge-id",
    /// "absolut", "add" and "mult".
    ///
    /// @param attrs The tags attributes. Here: "weight" requires the
    /// attribute "edge-id" and at least one of "absolut", "add" and
    /// "mult". Any combination of "absolut", "add" and "mult" may be
    /// provided.
    ///
    void startParseWeight(  const Attributes& attrs );

    /// Call on appearance of the closing tag
    /// "supplementary-weights". Passes the created
    /// supplementary-weights to the RONet's ROEdges.
    ///
    void stopParseSupplementaryWeights( void );

    /// Call on appearance of the closing tag "interval". Sets some
    /// state variables.
    ///
    void stopParseInterval( void );

    /// Call on appearance of the closing tag "weight". Sets some
    /// state variables.
    ///
    void stopParseWeight( void );

    /// Association between edge-ids and weights-containers.
    typedef std::map< std::string, FloatValueTimeLine* > WeightsMap;

    /// Inserts either a new TimeRange-value-pair to an existing
    /// weights-container (=FloatValueTimeLine) in the passed
    /// WeightsMap or creates a new one and inserts it into the
    /// WeightsMap. The member edgeIdM is used as the WeightsMap's
    /// key. The members intervalStartM and intervalEndM define the
    /// TimeRange.
    ///
    /// @param aMap One of the three WeightsMap-members.
    /// @param aValue The value to insert.
    ///
    void insertValuedTimeRangeIntoMap( WeightsMap& aMap
                                       , SUMOReal aValue );

    /// Retrieve the weights-container (=FloatValueTimeLine) from the
    /// passed WeightsMap. Use aEdgeId as key. If there is no
    /// weights-container, an empty one will be created.
    ///
    /// @param aMap The WeightsMap that sould be searched.
    /// @param aEdgeId The search key.
    ///
    /// @return The found or newly created weights-container.
    ///
    FloatValueTimeLine* getFloatValueTimeLine( WeightsMap& aMap
                                               , std::string aEdgeId );

private:
    /// The "global" command-line options
    OptionsCont& optionsM;

    /// The net we are working on.
    RONet& netM;


    bool hasStartedSupplementaryWeightsM; ///< State variable for tag
                                          ///"supplementary-weights"
    bool hasStartedIntervalM;   ///< State variable for tag "interval".
    bool hasStartedWeightM;     ///< State variable for tag "weight".
    bool isEdgeIdSetM;          ///< Flag indicating the occurence of
                                ///the attribute "edge-id" within tag
                                ///"weights".
    bool isAbsolutValueSetM;    ///< Flag indicating the occurence of
                                ///the attribute "absolut" within tag
                                ///"weights".
    bool isMultValueSetM;       ///< Flag indicating the occurence of
                                ///the attribute "mult" within tag
                                ///"weights".
    bool isAddValueSetM;        ///< Flag indicating the occurence of
                                ///the attribute "add" within tag
                                ///"weights".

    SUMOTime intervalStartM; ///< Value of the attribute "begin"
                                  ///within tag "interval".
    SUMOTime intervalEndM; ///< Value of the attribute "end"
                                ///within tag "interval".

    std::string edgeIdM;        ///< Value of the attribute "edge-id"
                                ///within tag "weights".
    SUMOReal absolutValueM;       ///< Value of the attribute "absolut"
                                ///within tag "weights".
    SUMOReal multValueM;          ///< Value of the attribute "mult"
                                ///within tag "weights".
    SUMOReal addValueM;           ///< Value of the attribute "add"
                                ///within tag "weights".

    /// Iterator to WeightsMap.
    typedef WeightsMap::iterator WeightsMapIt;

    WeightsMap absolutMapM;     ///< Container holding "absolut"
                                ///weights-container
                                ///(=FloatValueTimeLine) by "edge-id".
    WeightsMap multMapM;        ///< Container holding "mult"
                                ///weights-container
                                ///(=FloatValueTimeLine) by "edge-id".
    WeightsMap addMapM;         ///< Container holding "add"
                                ///weights-container
                                ///(=FloatValueTimeLine) by "edge-id".

    /// Edge-Id-strings set.
    typedef std::set< std::string > EdgeSet;
    /// Iterator to EdgeSet.
    typedef EdgeSet::iterator EdgeSetIt;

    EdgeSet weightedEdgesM;     ///< Container holding "edge-id"s to
                                ///all processed edges.

    /// copy ctor
    ROSupplementaryWeightsHandler( const ROSupplementaryWeightsHandler& );

    /// assignment operator
    ROSupplementaryWeightsHandler& operator=(
        const ROSupplementaryWeightsHandler& );

};

/*
Example supplementary-weights-file:
-----------------------------------
<?xml version="1.0" standalone="yes"?>
<supplementary-weights>
  <interval begin="60" end="119">
    <weight edge-id="1fi" absolut="42" mult="2.5" add="-21"/>
    <weight edge-id="2o" mult="13"/>
  </interval>
  <interval begin="120" end="179">
    <weight edge-id="1fi" absolut="48" mult="2.5"/>
    <weight edge-id="2o" add="7"/>
  </interval>
</supplementary-weights>
*/

// Local Variables:
// mode:C++
// End:

#endif // ROSUPPLEMENTARYWEIGHTSHANDLER_H
