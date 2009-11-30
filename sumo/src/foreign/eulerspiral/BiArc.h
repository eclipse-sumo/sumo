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

#ifndef BI_ARC_H
#define BI_ARC_H

#include "points.h"
#include "angles.h"

#define eA 0.0001   //Epsilon for angles
#define eK 0.0001   //Epsilon for curvature
#define K_LARGE 100000 //large curvature value

class BiArcParams
{
public:
  int flag;  //0:single arc, 1:biarc

  Point2D<double> start_pt;
  Point2D<double> end_pt;

  double start_angle;
  double end_angle;

  double K1;
  double K2;

  double L1;
  double L2;

  double E;  //energy

  double R1;
  double R2;
  
  int dir1;
  int dir2;

  Point2D<double> mid_pt;
  Point2D<double> center1;
  Point2D<double> center2;

  BiArcParams()
  {
    flag         = 0;

    start_angle  = 0;
    end_angle    = 0;

    K1           = 0;
    K2           = 0;

    L1           = 0;
    L2           = 0;

    dir1         = 0;
    dir2         = 0;

    R1           = 0;
    R2           = 0;

    E            = 0;

  };

  ~BiArcParams(){};

  BiArcParams( const BiArcParams &rhs)
  {
    start_pt    = rhs.start_pt;
    end_pt      = rhs.end_pt;

    start_angle = rhs.start_angle;
    end_angle   = rhs.end_angle;

    K1          = rhs.K1;
    K2          = rhs.K2;

    L1         = rhs.L1;
    L2         = rhs.L2;

    mid_pt    = rhs.mid_pt;
    center1     = rhs.center1;
    center2     = rhs.center2;

    dir1        = rhs.dir1;
    dir2        = rhs.dir2;

    R1          = rhs.R1;
    R2          = rhs.R2;

    flag        = rhs.flag;
    E           = rhs.E;
  
  };

  BiArcParams& operator=(const BiArcParams &rhs)
  {
    if (this!=&rhs){
      start_pt    = rhs.start_pt;
      end_pt      = rhs.end_pt;

      start_angle = rhs.start_angle;
      end_angle   = rhs.end_angle;

      K1          = rhs.K1;
      K2          = rhs.K2;

      L1          = rhs.L1;
      L2          = rhs.L2;

      mid_pt    = rhs.mid_pt;
      center1     = rhs.center1;
      center2     = rhs.center2;

      dir1        = rhs.dir1;
      dir2        = rhs.dir2;

      R1          = rhs.R1;
      R2          = rhs.R2;

      flag        = rhs.flag;
      E           = rhs.E;
    }
    return *this;
  }

  //total arclength
  double L(){return (L1+L2);}

  void scale (double factor)
  {
    K1 /=factor;
    L1 *=factor;

    K2 /=factor;
    L2 *=factor;
  }
};

class BiArc
{
public:
  BiArcParams params;

  BiArc(){}
  BiArc(Point2D<double> start_pt, double start_angle, Point2D<double> end_pt, double end_angle)
  {
    params.start_pt = start_pt;
    params.start_angle = angle0To2Pi(start_angle);

      params.end_pt = end_pt;
    params.end_angle = angle0To2Pi(end_angle);

    //since we have all the parameters, we might as well compute it
    compute_biarc_params();
  }
  
  ~BiArc(){}

  void    compute_biarc_params ();
  void    compute_other_stuff  ();
  double  compute_join_theta   (double k1, double k2);
  double  compute_arclength    (double theta0, double theta2, double k);

  void set_start_params(Point2D<double> start_pt, double start_angle)
  {
    params.start_pt = start_pt;
    params.start_angle = angle0To2Pi(start_angle);
  }

  void set_end_params(Point2D<double> end_pt, double end_angle)
  {
    params.end_pt = end_pt;
    params.end_angle = angle0To2Pi(end_angle);
  }
};

#endif
