/* determine latitude from authalic latitude */
#ifndef lint
static const char SCCSID[]="@(#)pj_auth.c	4.3	93/06/12	GIE	REL";
#endif
#include <projects.h>
# define P00 .33333333333333333333
# define P01 .17222222222222222222
# define P02 .10257936507936507936
# define P10 .06388888888888888888
# define P11 .06640211640211640211
# define P20 .01641501294219154443
#define APA_SIZE 3
	double *
pj_authset(double es) {
	double t, *APA;

	if (APA = (double *)pj_malloc(APA_SIZE * sizeof(double))) {
		APA[0] = es * P00;
		t = es * es;
		APA[0] += t * P01;
		APA[1] = t * P10;
		t *= es;
		APA[0] += t * P02;
		APA[1] += t * P11;
		APA[2] = t * P20;
	}
	return APA;
}
	double
pj_authlat(double beta, double *APA) {
	double t = beta+beta;
	return(beta + APA[0] * sin(t) + APA[1] * sin(t+t) + APA[2] * sin(t+t+t));
}
