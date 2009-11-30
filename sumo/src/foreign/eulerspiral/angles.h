/************************************************************************
 *                                                                      *
 *       Copyright 2004, Brown University, Providence, RI               *
 *                                                                      *
 *  Permission to use and modify this software and its documentation    *
 *  for any purpose other than its incorporation into a commercial      *
 *  product is hereby granted without fee. Recipient agrees not to      *
 *  re-distribute this software or any modifications of this            *
 *  software without the permission of Brown University. Brown          *
 *  University makes no representations or warrantees about the         *
 *  suitability of this software for any purpose.  It is provided       *
 *  "as is" without express or implied warranty. Brown University       *
 *  requests notification of any modifications to this software or      *
 *  its documentation. Notice should be sent to:                        *
 *                                                                      *
 *  To:                                                                 *
 *        Software Librarian                                            *
 *        Laboratory for Engineering Man/Machine Systems,               *
 *        Division of Engineering, Box D,                               *
 *        Brown University                                              *
 *        Providence, RI 02912                                          *
 *        Software_Librarian@lems.brown.edu                             *
 *                                                                      *
 *  We will acknowledge all electronic notifications.                   *
 *                                                                      *
 ************************************************************************/

#ifndef _ANGLES_H
#define _ANGLES_H

#include <cmath>

//##########################################################
// THE ANGLE DEFINITIONS
//##########################################################
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

inline double angle0To2Pi (double angle)
{
#if 0  
  while (angle >= M_PI*2)
    angle -= M_PI*2;
  while (angle < 0)
    angle += M_PI*2;
  return angle;
#else
  if (angle>2*M_PI)
      return  fmod (angle,M_PI*2);
   else if (angle < 0)
      return (2*M_PI+ fmod (angle,M_PI*2));
   else return angle;
#endif
}

inline double CCW (double reference, double angle1)
{
   double fangle1 = angle0To2Pi(angle1);
   double fref = angle0To2Pi(reference);

   if (fref > fangle1){
    return angle0To2Pi(2*M_PI - (fref - fangle1));
  }
   else
      return angle0To2Pi(fangle1 - fref); 
}

#endif
