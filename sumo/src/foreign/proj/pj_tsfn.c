/* determine small t */
#ifndef lint
static const char SCCSID[]="@(#)pj_tsfn.c	4.3	93/06/12	GIE	REL";
#endif
#include <math.h>
#include <projects.h>
#define HALFPI		1.5707963267948966
	double
pj_tsfn(double phi, double sinphi, double e) {
	sinphi *= e;
	return (tan (.5 * (HALFPI - phi)) /
	   pow((1. - sinphi) / (1. + sinphi), .5 * e));
}
