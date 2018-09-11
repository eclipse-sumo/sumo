/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    EffortCalculator.h
/// @author  Michael Behrisch
/// @date    2018-08-21
/// @version $Id$
///
// The EffortCalculator is an interface for additional edge effort calculators
/****************************************************************************/
#ifndef EffortCalculator_h
#define EffortCalculator_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the effort calculator interface
template<class E>
class EffortCalculator {
  
public:
  
    /** Pass the set of all edges in the routing query to the effortCalculator **/
    virtual void init(const std::vector<E*>& edges) = 0;
    
    /** Return the effort of a given edge **/
    virtual double getEffort(E const * edge) const = 0;
    
    /** Update the effort of the edge **/
    virtual void update(E const *  edge, E const *  prev) = 0;
    
    /** Set the effort of the first edge in the query to zero **/
    virtual void setInitialState(E const *edge)  = 0;
    
    /** basic output facility to inform about effort at this edge **/
    virtual void output(E const *edge) const = 0 ;
    
};


#endif

/****************************************************************************/
