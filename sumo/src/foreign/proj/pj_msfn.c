/* determine constant small m */
#ifndef lint
static const char SCCSID[]="@(#)pj_msfn.c	4.3	93/06/12	GIE	REL";
#endif
#include <math.h>
#include <projects.h>
	double
pj_msfn(double sinphi, double cosphi, double es) {
	return (cosphi / sqrt (1. - es * sinphi * sinphi));
}
