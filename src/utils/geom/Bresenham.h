/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Bresenham.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A class to realise a uniform n:m - relationship using the
/****************************************************************************/
#ifndef Bresenham_h
#define Bresenham_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * The class' only static method "execute" obtains a callback object and
 * performs the computation of the n:m - relationship
 */
class Bresenham {
public:
    /**
     * BresenhamCallBack
     * This class is the base interface-describing class for a callback class
     * for the bresenham-function.
     * Derived classes must implement the execute-method which is called
     * on every bresenham-step
     */
    class BresenhamCallBack {
    public:
        /** constuctor */
        BresenhamCallBack() { }

        /** destructor */
        virtual ~BresenhamCallBack() { }

        /** called when a bresenham step has been computed */
        virtual void execute(const int val1, const int val2) = 0;
    };

public:
    /** compute the bresenham - interpolation between both values
        the higher number is increased by one for each step while the smaller
        is increased by smaller/higher.
        In each step, the callback is executed. */
    static void compute(BresenhamCallBack* callBack, const int val1, const int val2);
};


#endif

/****************************************************************************/

