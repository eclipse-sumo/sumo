/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    StopEdge.h
/// @author  Michael Behrisch
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The StopEdge is a special intermodal edge representing bus and train stops
/****************************************************************************/
#ifndef StopEdge_h
#define StopEdge_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "IntermodalEdge.h"
#include "FareToken.h"

// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the stop edge type representing bus and train stops
template<class E, class L, class N, class V>
class StopEdge : public IntermodalEdge<E, L, N, V> {
public:
  
  ///@brief Constructor without fare information
    StopEdge(const std::string id,
             int numericalID,
             const E* edge) :
        IntermodalEdge<E, L, N, V>(id, numericalID, edge, "!stop"),
        myFareZone(0),
        myFareToken(FareToken::None),
        myStartToken(FareToken::None)
        { }
        
  ///@brief Constructor with fare information
    StopEdge(const std::string id,
             int numericalID,
             const E* edge,
             int fareZone,
             FareToken fareToken,
             FareToken startToken) :
        IntermodalEdge<E, L, N, V>(id, numericalID, edge, "!stop"),
        myFareZone(fareZone),
        myFareToken(fareToken),
        myStartToken(startToken){
    }
        
        
    bool includeInRoute(bool /* allEdges */) const {
        return true;
    }
    
    
private:
  
  
  /// ~~~Fields containing fare information~~~ ///
  
  /** the fare zone this stop is a part pf **/
  int const myFareZone;
  
  /** the faretoken that can be collected at this station **/
  FareToken const myFareToken;
  
  /** the faretoken that is used when a trip is started at this station **/
  FareToken const myStartToken;

public:
  
  virtual int getFareZone() const {
    return myFareZone;
    }
    
  virtual FareToken  getFareToken() const {
    return myFareToken;
  }
  
  virtual  FareToken   getStartToken() const {
    return myStartToken;
  }
  
};


#endif

/****************************************************************************/
