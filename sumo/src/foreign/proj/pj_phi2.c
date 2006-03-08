/* determine latitude angle phi-2 */
#ifndef lint
static const char SCCSID[]="@(#)pj_phi2.c	4.3	93/06/12	GIE	REL";
#endif
#include <projects.h>

#define HALFPI		1.5707963267948966
#define TOL 1.0e-10
#define N_ITER 15

	double
pj_phi2(double ts, double e) {
	double eccnth, Phi, con, dphi;
	int i;

	eccnth = .5 * e;
	Phi = HALFPI - 2. * atan (ts);
	i = N_ITER;
	do {
		con = e * sin (Phi);
		dphi = HALFPI - 2. * atan (ts * pow((1. - con) /
		   (1. + con), eccnth)) - Phi;
		Phi += dphi;
	} while ( fabs(dphi) > TOL && --i);
	if (i <= 0)
		pj_errno = -18;
	return Phi;
}
