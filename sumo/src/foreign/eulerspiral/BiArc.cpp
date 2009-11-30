#include "BiArc.h"
#include <cassert>

void BiArc::compute_biarc_params(void)
{
  double k1, k2, k3, k4;
  double L1, L2, L3, L4;

  double t0 = params.start_angle;
  double t2 = params.end_angle;
  double tjoin;

  double L = euc_distance(params.start_pt, params.end_pt);

  //degenerate case
  if (L<eA){
    params.K1 = HUGE;
    params.K2 = HUGE;
    params.E = HUGE;
    return;
  }

  double psi = atan2(params.end_pt.y()-params.start_pt.y(),params.end_pt.x()-params.start_pt.x());
  if (psi<0) psi+=2*M_PI; //psi = [0,2*pi)

  params.flag = 0;

  L1 = -10;
  L2 = -10;

  // due to the 0-2*pi discontinuity even for perfect arcs,
  // (psi-(t2+t0)/2)~{-pi,0, pi} for cocircular solutions
  // if (abs(mod(psi -(t2+t0)/2, pi))<eA) % this condition is not correct

  if (fabs(psi-(t2+t0)/2)<eA || fabs(psi-(t2+t0)/2+M_PI)<eA || fabs(psi-(t2+t0)/2-M_PI)<eA){
    if (fabs(fmod(psi-t0,2*M_PI))<eA){    // straight line (still need to check mod 2*pi)
      k1 = 0;
      k2 = 0;
      L1 = L;
      L2 = 0;
    } 
    else {    // single arc
      k1 = -4*sin((3*t0+t2)/4 - psi)*cos((t2-t0)/4)/L;
      k2 = 0;
      L1 = compute_arclength(t0,t2,k1);
      L2 = 0;
    }
    //record the solutions the parameters
    params.L1 = L1;
    params.L2 = L2;
    params.K1 = k1;
    params.K2 = k2;
    params.E = 0;
    compute_other_stuff();
    return;
  } 
  else {
    params.flag = 1;   //truly a biarc

    k1 = -4*sin((3*t0+t2)/4 - psi)*cos((t2-t0)/4)/L;
    k2 = 4*sin((t0+3*t2)/4 - psi)*cos((t2-t0)/4)/L;

    if (fabs(k1)<eK){
      k1 = 0;
      L1 = L*sin((t2+t0)/2-psi)/sin((t2-t0)/2);
      L2 = compute_arclength(t0,t2,k2);
    }
    else {
      if (fabs(k2)<eK){
        k2 = 0;
        L2 = L*sin((t2+t0)/2-psi)/sin((t0-t2)/2);
        L1 = compute_arclength(t0,t2,k1);
      }
      else {
        // tjoin will be incorrect if k1~0 or k2~0
        tjoin = compute_join_theta(k1,k2);
        L1 = compute_arclength(t0,tjoin,k1);
        L2 = compute_arclength(tjoin,t2,k2);
      }
    }

    // the other possible biarc
    L3 = -10;
    L4 = -10;

    k3 = 4*cos((3*t0+t2)/4 - psi)*sin((t2-t0)/4)/L;
    k4 = 4*cos((t0+3*t2)/4 - psi)*sin((t2-t0)/4)/L;

    // since this solution picks the biarc with the bigger turn
    // the curvature solutions can still be close to zero
    if ( (fabs(k3)>eK || fabs(k4)>eK) && fabs(k4-k3)>eK){
      if (fabs(k3)<eK){
        k3 = 0;
        L3 = L*sin((t2+t0)/2-psi)/sin((t2-t0)/2);
        L4 = compute_arclength(t0,t2,k4);
      }
      else {
        if (fabs(k4)<eK){
          k4 = 0;
          L4 = L*sin((t2+t0)/2-psi)/sin((t0-t2)/2);
          L3 = compute_arclength(t0,t2,k3);
        }
        else {
          tjoin = compute_join_theta(k3,k4);
          L3 = compute_arclength(t0,tjoin,k3);
          L4 = compute_arclength(tjoin,t2,k4);
        }
      }
    }

    // choose the smaller one
    // but due to the epsilon settings (eA and eK) we could still get an incorrect solution
    // this could be caught by looking at the signs of Ls.

    if ((L1>0 && L2>0) && ((L3<0 || L4<0) || (L1+L2)<(L3+L4))){
      //k1 and k2 are the correct solutions
      params.L1 = L1;
      params.L2 = L2;
      params.K1 = k1;
      params.K2 = k2;
      params.E = (k2-k1)*(k2-k1); 
    }
    else { 
      if (L3>0 && L4>0){
        params.L1 = L3;
        params.L2 = L4;
        params.K1 = k3;
        params.K2 = k4;
        params.E = (k3-k4)*(k3-k4);
      }
      else {
        //this should never happen
        assert(false);
      }
    }
  }
  compute_other_stuff();
}

void BiArc::compute_other_stuff(void)
{
  if (params.K1 != 0){
    params.R1 = fabs(1/params.K1);
    params.center1.setX(params.start_pt.x() - cos(params.start_angle-M_PI/2)/params.K1);
    params.center1.setY(params.start_pt.y() - sin(params.start_angle-M_PI/2)/params.K1);
    double dt = params.L1*params.K1;
    params.mid_pt.setX(params.center1.x() - cos(params.start_angle+M_PI/2+dt)/params.K1);
    params.mid_pt.setY(params.center1.y() - sin(params.start_angle+M_PI/2+dt)/params.K1);
  }
  else {
    params.mid_pt.setX(params.start_pt.x() + cos(params.start_angle)*params.L1);
    params.mid_pt.setY(params.start_pt.y() + sin(params.start_angle)*params.L1);
    params.R1 = HUGE;
  }

  if (params.K2 != 0){
    params.R2 = fabs(1/params.K2);
    params.center2.setX(params.end_pt.x() - cos(params.end_angle-M_PI/2)/params.K2);
    params.center2.setY(params.end_pt.y() - sin(params.end_angle-M_PI/2)/params.K2);
  }
  else {
    params.R2 = HUGE;
  }

  params.dir1 = params.K1<0 ? -1. : 1.;//sign(params.K1); //CCW=+1
  params.dir2 = params.K2<0 ? -1. : 1.;//sign(params.K2);
}

/* ---------------- BiArc support Functions --------------------- */

double BiArc::compute_join_theta(double k1, double k2)
{
  // compute the theta at which the two arcs meet
  double x0=params.start_pt.x();
  double y0=params.start_pt.y();
  double x2=params.end_pt.x();
  double y2=params.end_pt.y();
  double t0=params.start_angle;
  double t2=params.end_angle;

  double sin_join_theta = (k1*k2*(x2-x0)+k2*sin(t0)-k1*sin(t2))/(k2-k1);
  double cos_join_theta = (-k1*k2*(y2-y0)+k2*cos(t0)-k1*cos(t2))/(k2-k1);

  double join_theta = atan2(sin_join_theta, cos_join_theta);
  if (join_theta<0) join_theta += 2*M_PI;

  return join_theta;
}

double BiArc::compute_arclength(double t0, double t1, double k)
{
  double num = (t1-t0);

  if (k<0 && (t1-t0)>0)
     num = num-2*M_PI;
  else if (k>0 && (t1-t0)<0)
     num = num+2*M_PI;

  return num/k;
}
