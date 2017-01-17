/****************************************************************************/
/// @file    Bresenham.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A class to realise a uniform n:m - relationship using the
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef Bresenham_h
#define Bresenham_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


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

