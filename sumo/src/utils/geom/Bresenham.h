#ifndef Bresenham_h
#define Bresenham_h
/***************************************************************************
                          Bresenham.h
              A class to realise a uniform n:m - relationship using the
              bresenham - algorithm
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.4  2005/09/23 06:07:01  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2004/11/23 10:34:46  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:50:20  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:01:08  dkrajzew
// initial commit for utility classes for geometry handling
//
// Revision 1.3  2002/06/11 15:58:24  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/07 14:58:46  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
//
/* =========================================================================
 * class definitions
 * ======================================================================= */
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
        virtual void execute(SUMOReal val1, SUMOReal val2) = 0;
    };

public:
    /** compute the bresenham - interpolation between both values
        the higher number is increased by one for each step while the smaller
        is increased by smaller/higher.
        In each step, the callback is executed. */
    static void compute(BresenhamCallBack *callBack, SUMOReal val1, SUMOReal val2);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
