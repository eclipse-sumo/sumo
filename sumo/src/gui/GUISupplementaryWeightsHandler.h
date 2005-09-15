#ifndef GUISupplementaryWeightsHandler_H
#define GUISupplementaryWeightsHandler_H
//---------------------------------------------------------------------------//
//                        GUISupplementaryWeightsHandler.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu Apr 08 2004 15:31
//  copyright            : (C) 2004 by Christian Roessel
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
// $Log$
// Revision 1.4  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
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
class RONet;
class ROEdge;
class FloatValueTimeLine;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUISupplementaryWeightsHandler : public SUMOSAXHandler
{
public:
    /// Constructor
    ///
    /// @param filename XML-file that contains the supplementary weights.
    ///
    GUISupplementaryWeightsHandler(const std::string& filename );

    /// Destructor
    ~GUISupplementaryWeightsHandler();

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

private:

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
    double absolutValueM;       ///< Value of the attribute "absolut"
                                ///within tag "weights".
    double multValueM;          ///< Value of the attribute "mult"
                                ///within tag "weights".
    double addValueM;           ///< Value of the attribute "add"
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
    GUISupplementaryWeightsHandler( const GUISupplementaryWeightsHandler& );

    /// assignment operator
    GUISupplementaryWeightsHandler& operator=(
        const GUISupplementaryWeightsHandler& );

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

#endif // GUISupplementaryWeightsHandler_H
