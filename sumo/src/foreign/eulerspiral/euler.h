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

#ifndef EULER_H
#define EULER_H

// Rewritten by Amir: Feb 1, 2004

#include "angles.h"
#include "BiArc.h"
#include <vector>

//some defines for Euler-spiral optimization
#define MAX_NUM_ITERATIONS    50000
#define eError            1e-5

class EulerSpiralParams;
class EulerSpiralLookupTable;
class EulerSpiral;

class EulerSpiralParams 
{
public:
  Point2D<double> start_pt;
  Point2D<double> end_pt;

  double start_angle;
  double end_angle;
  double K0;
  double K2;
  double gamma;
  double L;
  double turningAngle;
  double error;
  double psi;

  EulerSpiralParams()
  {
    start_angle   = 0;
    end_angle     = 0;
    K0            = 0;
    K2            = 0;
    gamma        = 0;
    L             = 0;
    turningAngle  = 0;
    error         = 0;
    psi           = 0;
  }

  ~EulerSpiralParams(){};

  EulerSpiralParams(const EulerSpiralParams &rhs)
  {
    start_pt      = rhs.start_pt;
    end_pt        = rhs.end_pt;
    start_angle   = rhs.start_angle;
    end_angle     = rhs.end_angle;
    K0            = rhs.K0;
    K2            = rhs.K2;
    gamma        = rhs.gamma;
    L             = rhs.L;
    turningAngle  = rhs.turningAngle;
    error         = rhs.error;
    psi           = rhs.psi;
  }

  EulerSpiralParams& operator=(const EulerSpiralParams &rhs)
  {
    if (this!=&rhs)
    {
      start_pt      = rhs.start_pt;
      end_pt        = rhs.end_pt;
      start_angle   = rhs.start_angle;
      end_angle     = rhs.end_angle;
      K0            = rhs.K0;
      K2            = rhs.K2;
      gamma        = rhs.gamma;
      L             = rhs.L;
      turningAngle  = rhs.turningAngle;
      error         = rhs.error;
      psi           = rhs.psi;
    }
    return *this;
  }
};

class EulerSpiralLookupTable
{
private:
  // following line commented out to make clang compiler happy, MB 14.06.2014 
  //int NN; //size of the lookup tables (NNxNN) or number of data points between -pi and pi
  double* _theta; //lookup the theta value corresponding to the index NN long
  double _dt; //delta theta 

  //lookup tables read in from files
  double** ES_k0;
  double** ES_k1;
  double** ES_gamma;
  double** ES_L;
  
public:
  EulerSpiralLookupTable();
  ~EulerSpiralLookupTable();

  static EulerSpiralLookupTable* get_globalEulerSpiralLookupTable();

  double k0(double start_angle, double end_angle);
  double k1(double start_angle, double end_angle);
  double gamma(double start_angle, double end_angle);
  double L(double start_angle, double end_angle);

  double dt(); //delta theta values for the table (tells you about the accuracy of the lookup)
  double theta(int N); //lookup the theta value indexed by N
};

class EulerSpiral
{
private:
  BiArc          _bi_arc_estimate;

public:
  EulerSpiralParams params;
  std::vector <Point2D<double> > pts;

  EulerSpiral(){};

  //constructor Type 1
  EulerSpiral(Point2D<double> start_pt, double start_angle, Point2D<double> end_pt, double end_angle)
  {
    params.start_pt = start_pt;
    params.start_angle = angle0To2Pi(start_angle);

    params.end_pt = end_pt;
    params.end_angle = angle0To2Pi(end_angle);

    //since we have all the parameters, we might as well compute it
    compute_es_params();
  }

  //Constructor type 2
  EulerSpiral(Point2D<double> start_pt, double start_angle, double k0, double gamma, double L)
  {
    params.start_pt = start_pt;
    params.start_angle = angle0To2Pi(start_angle);
    params.end_pt = compute_end_pt(k0, gamma, L);
    params.end_angle = start_angle + 0.5*gamma*L*L + k0*L;

    //since we have all the parameters, we might as well compute it
    compute_es_params();
  }

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

  void set_params(Point2D<double> start_pt, double start_angle, Point2D<double> end_pt, double end_angle)
  {
    params.start_pt = start_pt;
    params.start_angle = angle0To2Pi(start_angle);

    params.end_pt = end_pt;
    params.end_angle = angle0To2Pi(end_angle);
  }

  void compute_es_params ();

  //compute the extrinsic points
  void computeSpiral(std::vector<Point2D<double> > &spiral, double ds=0, int NPts=0);

  // Supporting functions
  Point2D<double> get_fresnel_integral(double value);
  Point2D<double> compute_end_pt(double arclength, bool bNormalized=false);
  Point2D<double> compute_end_pt(double k0, double gamma, double L, bool bNormalized=false);
  Point2D<double> compute_es_point(EulerSpiralParams& es_params, double arclength, bool bNormalized=false);
  inline double   compute_error(double k0, double L);
  
  // function to output data
//  void write_es_info_to_file(vcl_ofstream & fp);
};

#endif

