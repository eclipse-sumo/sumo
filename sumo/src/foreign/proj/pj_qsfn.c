/* determine small q */
#ifndef lint
static const char SCCSID[]="@(#)pj_qsfn.c	4.3	93/06/12	GIE	REL";
#endif
#include <math.h>
#include <projects.h>

# define EPSILON 1.0e-7
	double
pj_qsfn(double sinphi, double e, double one_es) {
	double con;

	if (e >= EPSILON) {
		con = e * sinphi;
		return (one_es * (sinphi / (1. - con * con) -
		   (.5 / e) * log ((1. - con) / (1. + con))));
	} else
		return (sinphi + sinphi);
}
