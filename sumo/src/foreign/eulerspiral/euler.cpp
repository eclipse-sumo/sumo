/*
#include "vcl_iostream.h"
#include <vcl_fstream.h>
#include "vcl_complex.h"
#include "vcl_cmath.h"
*/

#include "points.h"
#include "BiArc.h"
#include "euler.h" 
#include <utils/common/StdDefs.h>
#include <complex>
#include <iostream>

#define eGamma    1e-8

//global lookup table: This is only instantiated when required and cleaned upon program exit
static EulerSpiralLookupTable *globalEulerSpiralLookupTable;

//ensures single instantiation of the global variable
EulerSpiralLookupTable* EulerSpiralLookupTable::get_globalEulerSpiralLookupTable()
{
  if (! globalEulerSpiralLookupTable)
    globalEulerSpiralLookupTable = new EulerSpiralLookupTable();

  return globalEulerSpiralLookupTable;
}

EulerSpiralLookupTable::EulerSpiralLookupTable()
{
/*
  int i,j;

  //read in the tables from data files if available
  vcl_ifstream fp_k0_in("ES_k0.dat", vcl_ios::in | vcl_ios::binary);
  vcl_ifstream fp_gamma_in("ES_gamma.dat", vcl_ios::in | vcl_ios::binary);
  vcl_ifstream fp_L_in("ES_L.dat", vcl_ios::in | vcl_ios::binary);

  //This number is the first entry in the file
  fp_k0_in.read ((char*)&NN, sizeof (NN));
  fp_gamma_in.read ((char*)&NN, sizeof (NN));
  fp_L_in.read ((char*)&NN, sizeof (NN));

  //COUT << "Number of theta samples " << NN <<endl;
  
  //initialize the tables to this size
  ES_k0 = new double*[NN];
  for (i=0; i<NN; i++)
    ES_k0[i] = new double [NN];

  ES_k1 = new double*[NN];
  for (i=0; i<NN; i++)
    ES_k1[i] = new double [NN];

  ES_gamma = new double*[NN];
  for (i=0; i<NN; i++)
    ES_gamma[i] = new double [NN];

  ES_L = new double*[NN];
  for (i=0; i<NN; i++)
    ES_L[i] = new double [NN];

  //compute the dtt
  _dt = 2*M_PI/NN;

  //create and fill in the theta array
  _theta = new double [NN];
  double t1 = -M_PI;
  for (i=0; i<NN; i++, t1+=_dt)
    _theta[i] = t1;

  //now read the files to fill in the lookup tables
  for (i=0; i<NN; i++){
    for (j=0; j<NN; j++){
      
      double k0, k1, gamma, L;

      fp_k0_in.read ((char*)&k0, sizeof (k0));
      fp_gamma_in.read ((char*)&gamma, sizeof (gamma));
      fp_L_in.read ((char*)&L, sizeof (L));
      k1 = k0 + gamma*L;

      //store these in the array
      ES_k0[i][j] = k0;
      ES_k1[i][j] = k1;
      ES_gamma[i][j] = gamma;
      ES_L[i][j] = L;
    }
  }

  fp_k0_in.close();
  fp_gamma_in.close();
  fp_L_in.close();
  */
}

EulerSpiralLookupTable::~EulerSpiralLookupTable()
{
  int i;
/*
  //delete the arrays
  if (ES_k0) {
    for (i=0; i<=NN; i++)
      delete []ES_k0[i];
    delete []ES_k0;
  }
  ES_k0 = NULL;
  if (ES_k1) {
    for (i=0; i<=NN; i++)
      delete []ES_k1[i];
    delete []ES_k1;
  }
  ES_k1 = NULL;
  if (ES_gamma) {
    for (i=0; i<=NN; i++)
      delete []ES_gamma[i];
    delete []ES_gamma;
  }
  ES_gamma = NULL;
  if (ES_L) {
    for (i=0; i<=NN; i++)
      delete []ES_L[i];
    delete []ES_L;
  }
  ES_L = NULL;

  delete []_theta;
  */
}

//delta theta values for the table (tells you about the accuracy of the lookup)
double EulerSpiralLookupTable::dt()
{
  return _dt;
}

double EulerSpiralLookupTable::theta(int N)
{
  return _theta[N];
}

double EulerSpiralLookupTable::k0(double start_angle, double end_angle)
{
  //assume it is already 0-2Pi
  double sangle, eangle;

  if (start_angle>M_PI) sangle = start_angle-2*M_PI;
  else                  sangle = start_angle;

  if (end_angle>M_PI) eangle = end_angle-2*M_PI;
  else                eangle = end_angle;
  
  //output bilinear interpolated data from the tables 
  int ilow, ihigh, jlow, jhigh;

  ilow = (int)floor((sangle+M_PI)/_dt);
  ihigh = (int)ceil((sangle+M_PI)/_dt);

  jlow = (int)floor((eangle+M_PI)/_dt);
  jhigh = (int)ceil((eangle+M_PI)/_dt);

  double slow = _theta[ilow];
  double shigh = _theta[ihigh];
  double elow = _theta[jlow];
  double ehigh = _theta[jhigh];

  double a = (sangle - slow)/_dt;
  double b = (eangle - elow)/_dt;

  double k0 = (1-a)*(1-b)*ES_k0[ilow][jlow] + a*(1-b)*ES_k0[ihigh][jlow] +
              (1-a)*b*ES_k0[ilow][jhigh] + a*b*ES_k0[ihigh][jhigh];

  return k0;
}

double EulerSpiralLookupTable::k1(double start_angle, double end_angle)
{
  //output bilinear interpolated data from the tables

  //assume it is already 0-2Pi
  double sangle, eangle;

  if (start_angle>M_PI) sangle = start_angle-2*M_PI;
  else                  sangle = start_angle;

  if (end_angle>M_PI) eangle = end_angle-2*M_PI;
  else                eangle = end_angle;
  
  //output bilinear interpolated data from the tables 
  int ilow, ihigh, jlow, jhigh;

  ilow = (int)floor((sangle+M_PI)/_dt);
  ihigh = (int)ceil((sangle+M_PI)/_dt);

  jlow = (int)floor((eangle+M_PI)/_dt);
  jhigh = (int)ceil((eangle+M_PI)/_dt);

  double slow = _theta[ilow];
  double shigh = _theta[ihigh];
  double elow = _theta[jlow];
  double ehigh = _theta[jhigh];

  double a = (sangle - slow)/_dt;
  double b = (eangle - elow)/_dt;

  double k1 = (1-a)*(1-b)*ES_k1[ilow][jlow] + a*(1-b)*ES_k1[ihigh][jlow] +
              (1-a)*b*ES_k1[ilow][jhigh] + a*b*ES_k1[ihigh][jhigh];

  return k1;
}

double EulerSpiralLookupTable::gamma(double start_angle, double end_angle)
{
  //output bilinear interpolated data from the tables
  
  //assume it is already 0-2Pi
  double sangle, eangle;

  if (start_angle>M_PI) sangle = start_angle-2*M_PI;
  else                  sangle = start_angle;

  if (end_angle>M_PI) eangle = end_angle-2*M_PI;
  else                eangle = end_angle;
  
  //output bilinear interpolated data from the tables 
  int ilow, ihigh, jlow, jhigh;

  ilow = (int)floor((sangle+M_PI)/_dt);
  ihigh = (int)ceil((sangle+M_PI)/_dt);

  jlow = (int)floor((eangle+M_PI)/_dt);
  jhigh = (int)ceil((eangle+M_PI)/_dt);

  double slow = _theta[ilow];
  double shigh = _theta[ihigh];
  double elow = _theta[jlow];
  double ehigh = _theta[jhigh];

  double a = (sangle - slow)/_dt;
  double b = (eangle - elow)/_dt;

  double gamma = (1-a)*(1-b)*ES_gamma[ilow][jlow] + a*(1-b)*ES_gamma[ihigh][jlow] +
              (1-a)*b*ES_gamma[ilow][jhigh] + a*b*ES_gamma[ihigh][jhigh];

  return gamma;
}

double EulerSpiralLookupTable::L(double start_angle, double end_angle)
{
  //output bilinear interpolated data from the tables
  
  //assume it is already 0-2Pi
  double sangle, eangle;

  if (start_angle>M_PI) sangle = start_angle-2*M_PI;
  else                  sangle = start_angle;

  if (end_angle>M_PI) eangle = end_angle-2*M_PI;
  else                eangle = end_angle;
  
  //output bilinear interpolated data from the tables 
  int ilow, ihigh, jlow, jhigh;

  ilow = (int)floor((sangle+M_PI)/_dt);
  ihigh = (int)ceil((sangle+M_PI)/_dt);

  jlow = (int)floor((eangle+M_PI)/_dt);
  jhigh = (int)ceil((eangle+M_PI)/_dt);

  double slow = _theta[ilow];
  double shigh = _theta[ihigh];
  double elow = _theta[jlow];
  double ehigh = _theta[jhigh];

  double a = (sangle - slow)/_dt;
  double b = (eangle - elow)/_dt;

  double L = (1-a)*(1-b)*ES_L[ilow][jlow] + a*(1-b)*ES_L[ihigh][jlow] +
              (1-a)*b*ES_L[ilow][jhigh] + a*b*ES_L[ihigh][jhigh];

  return L;
}


// Computes the Euler spiral for the given params
//if the global lookup table is available, it looks up the ES params first and then optimizes them
//this should dramatically cut down in the time to optimize
void EulerSpiral::compute_es_params ()
{
  //compute scaling distance
  double d = euc_distance(params.start_pt, params.end_pt);
  params.psi = angle0To2Pi(atan2(params.end_pt.y()-params.start_pt.y(),params.end_pt.x()-params.start_pt.x()));

  //degeneracy check
  if (d<eError)
    return; 

  //first compute a biarc estimate
  _bi_arc_estimate.set_start_params(params.start_pt, params.start_angle);
  _bi_arc_estimate.set_end_params(params.end_pt, params.end_angle);
  _bi_arc_estimate.compute_biarc_params();

  //get the total turning angle::This is an important parameter because
  //it defines the one solution out of many possible solutions
  params.turningAngle = _bi_arc_estimate.params.K1*_bi_arc_estimate.params.L1 +
                 _bi_arc_estimate.params.K2*_bi_arc_estimate.params.L2;

  //From here on, normlize the parameters and use these to perform the optimization

  double k0_init_est = _bi_arc_estimate.params.K1*d;
  double L_init_est = _bi_arc_estimate.params.L()/d;
  double dstep = 0.1;

  //Alternately, we can get the initial values from the lookup table and perform 
  //the optimization from there
  //double k0_init_est = globalEulerSpiralLookupTable->get_globalEulerSpiralLookupTable()->k0(CCW(params.psi, params.start_angle), CCW(params.psi, params.end_angle));
  //double L_init_est = globalEulerSpiralLookupTable->get_globalEulerSpiralLookupTable()->L(CCW(params.psi, params.start_angle), CCW(params.psi, params.end_angle));
  //double dstep = globalEulerSpiralLookupTable->get_globalEulerSpiralLookupTable()->dt()/4;

  //then perform a simple gradient descent to find the real solution
  double error = compute_error(k0_init_est, L_init_est);
  double prev_error = error;
  
  double k0 = k0_init_est;
  double L = L_init_est;

  double e1, e2, e3, e4;

  for (int i=0;i<MAX_NUM_ITERATIONS;i++)
  {
     if (error<eError)
        break;
    
     e1 = compute_error(k0 + dstep, L);
     e2 = compute_error(k0 - dstep, L);
     e3 = compute_error(k0, L + dstep);
     if (L>dstep)  e4 = compute_error(k0, L - dstep);
    
     error = MIN2(MIN2(e1,e2),MIN2(e3,e4));
    
     if (error>prev_error)
     {
       dstep = dstep/2;
       continue;
     }
    
     if    (error==e1)  k0 = k0 + dstep;
     else if (error==e2) k0 = k0 - dstep;
     else if (error==e3) L = L + dstep;
     else if (error==e4) L = L - dstep;
      
     prev_error = error;
  }

  //store the parameters
  params.K0 = k0/d;
  params.L = L*d;
  params.gamma = 2*(params.turningAngle - k0*L)/(L*L)/(d*d);
  params.K2 = (k0 + params.gamma*L)/d;
  params.error = error;
}

//compute the extrinsic points
void EulerSpiral::computeSpiral(std::vector<Point2D<double> > &spiral, double ds, int NPts)
{
  if (ds==0 && NPts==0){
    //use default values
    NPts = 100;
    ds = params.L/NPts;
  }

  spiral.clear();
  spiral.push_back(params.start_pt);

  double s=ds;
  if (NPts == 0) 
    NPts = (int) (params.L / ds);
  for (int i=1; i<NPts; i++,s+=ds){
    Point2D<double> cur_pt = compute_end_pt(s);
    spiral.push_back(cur_pt);
  }
  spiral.push_back(params.end_pt);
}

//////////////////////////////////////////////
// Supporting functions
//////////////////////////////////////////////

Point2D<double> EulerSpiral::compute_es_point(EulerSpiralParams& es_params, double arclength, bool bNormalized)
{
  params = es_params;
  return compute_end_pt(params.K0, params.gamma, arclength, bNormalized);
}

Point2D<double> EulerSpiral::compute_end_pt(double arclength, bool bNormalized)
{
  return compute_end_pt(params.K0, params.gamma, arclength, bNormalized);
}

Point2D<double> EulerSpiral::compute_end_pt(double k0, double gamma, double L, bool bNormalized)
{
  Point2D<double> start_pt;
  Point2D<double> end_pt;

  double theta;

  if (bNormalized){
    start_pt = Point2D<double>(0,0);
    theta = CCW(params.psi, params.start_angle);
  }
  else {
    start_pt = params.start_pt;
    theta = params.start_angle;
  }

  if (L==0)
    return start_pt;

  if (fabs(gamma)<eGamma) 
  { 
    if (fabs(k0)<eK)
    {
      //straight line
      end_pt.setX(start_pt.getX()+L*cos(theta));
      end_pt.setY(start_pt.getY()+L*sin(theta));
    }
    else 
    { 
      //circle
      double const_term = 1.0/k0;
      end_pt.setX(start_pt.getX()+const_term*(sin(k0*L+theta)-sin(theta)));
      end_pt.setY(start_pt.getY()-const_term*(cos(k0*L+theta)-cos(theta))); 
    }
    return end_pt;
  }

  double const1 = sqrt(M_PI*fabs(gamma));
  double const2 = sqrt(M_PI/fabs(gamma));

  Point2D<double> fresnel1 = get_fresnel_integral((k0+gamma*L)/const1);
  Point2D<double> fresnel2 = get_fresnel_integral(k0/const1);

  double C = (fresnel1.getX() - fresnel2.getX());
  if(gamma<0) {
      C *= -1.;
  }
  double S = fresnel1.getY() - fresnel2.getY();

  double cos_term = cos(theta-((k0*k0)/(2.0*gamma)));
  double sin_term = sin(theta-((k0*k0)/(2.0*gamma))); 
  
  end_pt.setX(start_pt.getX() + const2*(C*cos_term - S*sin_term));
  end_pt.setY(start_pt.getY() + const2*(C*sin_term + S*cos_term));

  return end_pt;
}

inline double   EulerSpiral::compute_error(double k0, double L)
{
  //assumes normalized parameters

  //compute the endpoint of the Euler spiral with the given intrinsic parameters
  double gamma = 2*(params.turningAngle - k0*L)/(L*L);
  Point2D<double> cur_end_pt = compute_end_pt(k0, gamma, L, true);

  //the error is the distance between the current end point and the desired end point
  return euc_distance(Point2D<double>(1,0), cur_end_pt);
}

// Fresnel Integral code from Numerical recipes in C
// EPS   is the relative error; 
// MAXIT is the maximum number of iterations allowed; 
// FPMIN is a number near the smallest representable floating-point number;
// XMIN  is the dividing line between using the series and continued fraction.

#define EPS    6.0e-8
#define MAXIT  100
#define FPMIN  1.0e-30
#define XMIN   1.5

//Computes the Fresnel integrals S(x) and C(x) for all real x
Point2D<double> EulerSpiral::get_fresnel_integral(double x)
{
  bool odd;
  int k,n;
  double a,ax,fact,pix2,sign,sum,sumc,sums,term,test;

  std::complex<double> b,cc,d,h,del,cs;
  Point2D<double> result;

  ax=fabs(x);
  if (ax < sqrt(FPMIN)) 
  { 
    //Special case: avoid failure of convergence test because of undeflow.
    result.setY(0.0);
    result.setX(ax);
  }
  else {
    if (ax <= XMIN)
    {
      // Evaluate both series simultaneously.
      sum=sums=0.0;
      sumc=ax;
      sign=1.0;
      fact=(M_PI/2.0)*ax*ax;
      odd=true;
      term=ax;
      n=3;

      for (k=1;k<=MAXIT;k++)
      { 
        term *= fact/k;
        sum  += sign*term/n;
        test=fabs(sum)*EPS;
        if (odd) 
        {
          sign = -sign;
          sums=sum;
          sum=sumc;
        }
        else {
          sumc=sum;
          sum=sums;
        }

        if (term < test) break;
        odd=!odd;
        n +=2;
      }

      if (k > MAXIT) 
        std::cout << "series failed in fresnel" << std::endl;

      result.setY(sums); 
      result.setX(sumc);
    }
    else {
      // Evaluate continued fraction by modified Lentz's method
      pix2=M_PI*ax*ax;
      b   = std::complex<double>(1.0,-pix2);
      cc  = std::complex<double>(1.0/FPMIN,0.0);
      d=h = std::complex<double>(1.0,0.0)/b;
      n = -1;

      for (k=2;k<=MAXIT;k++) 
      {
        n +=2;
        a = -n*(n+1);
        b= b+std::complex<double>(4.0,0.0);
        d=(std::complex<double>(1.0,0.0)/((a*d)+b));

        //Denominators cannot be zero
        cc=(b+(std::complex<double>(a,0.0)/cc));

        del=(cc*d);
        h=h*del;
        if ((fabs(del.real()-1.0)+fabs(del.imag())) < EPS)
          break;
      }
      if (k > MAXIT) 
        std::cout << "cf failed in frenel" << std::endl;

      h=std::complex<double>(ax,-ax)*h;
      cs=std::complex<double>(0.5,0.5)*(std::complex<double>(1.0,0.0) - std::complex<double>(cos(0.5*pix2),sin(0.5*pix2))*h );

      result.setX(cs.real());
      result.setY(cs.imag());
    }
  }

  if (x<0){ //use antisymmetry
    result = -1*result;  
  }

  return result;
}

// write Euler Spiral parameters to file
/*
void EulerSpiral::write_es_info_to_file(vcl_ofstream & fp){
/*
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
/

  fp << "start_pt: = ( " << (params.start_pt).getX() << 
    " , " << (params.start_pt).getY()<< " )" << vcl_endl;
  fp << "start_angle = " << params.start_angle << vcl_endl;
  fp << "end_pt = ( " << (params.end_pt).getX() << 
    " , " << (params.end_pt).getY()<< " )" << vcl_endl;
  fp << "end_angle = " << params.end_angle << vcl_endl;
  fp << "K0 = " << params.K0 << vcl_endl;
  fp << "K2 = " << params.K2 << vcl_endl;
  fp << "gamma = " << params.gamma << vcl_endl;
  fp << "L = " << params.L << vcl_endl;
  fp << "turningAngle = " << params.turningAngle << vcl_endl;
  fp << "error = " << params.error << vcl_endl;
  fp << "psi = " << params.psi << vcl_endl;
}
*/

