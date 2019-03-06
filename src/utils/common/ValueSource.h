/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ValueSource.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
#ifndef ValueSource_h
#define ValueSource_h


// ===========================================================================
// included modules
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template<typename T>
class ValueSource {
public:
    ValueSource() { }
    virtual ~ValueSource() { }
    virtual T getValue() const = 0;
    virtual ValueSource* copy() const = 0;
    virtual ValueSource<double>* makedoubleReturningCopy() const = 0;

};


#endif

/****************************************************************************/

