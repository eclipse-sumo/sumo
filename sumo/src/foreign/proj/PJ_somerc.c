#ifndef lint
static const char SCCSID[]="@(#)PJ_somerc.c	4.1	95/08/09	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	K, c, hlf_e, kR, cosp0, sinp0;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(somerc, "Swiss. Obl. Mercator") "\n\tCyl, Ell\n\tFor CH1903";
#define EPS	1.e-10
#define NITER 6
FORWARD(e_forward);
	double phip, lamp, phipp, lampp, sp, cp;

	sp = P->e * sin(lp.phi);
	phip = 2.* atan( exp( P->c * (
		log(tan(FORTPI + 0.5 * lp.phi)) - P->hlf_e * log((1. + sp)/(1. - sp)))
		+ P->K)) - HALFPI;
	lamp = P->c * lp.lam;
	cp = cos(phip);
	phipp = aasin(P->cosp0 * sin(phip) - P->sinp0 * cp * cos(lamp));
	lampp = aasin(cp * sin(lamp) / cos(phipp));
	xy.x = P->kR * lampp;
	xy.y = P->kR * log(tan(FORTPI + 0.5 * phipp));
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid & spheroid */
	double phip, lamp, phipp, lampp, cp, esp, con, delp;
	int i;

	phipp = 2. * (atan(exp(xy.y / P->kR)) - FORTPI);
	lampp = xy.x / P->kR;
	cp = cos(phipp);
	phip = aasin(P->cosp0 * sin(phipp) + P->sinp0 * cp * cos(lampp));
	lamp = aasin(cp * sin(lampp) / cos(phip));
	con = (P->K - log(tan(FORTPI + 0.5 * phip)))/P->c;
	for (i = NITER; i ; --i) {
		esp = P->e * sin(phip);
		delp = (con + log(tan(FORTPI + 0.5 * phip)) - P->hlf_e *
			log((1. + esp)/(1. - esp)) ) *
			(1. - esp * esp) * cos(phip) * P->rone_es;
		phip -= delp;
		if (fabs(delp) < EPS)
			break;
	}
	if (i) {
		lp.phi = phip;
		lp.lam = lamp / P->c;
	} else
		I_ERROR
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(somerc)
	double cp, phip0, sp;

	P->hlf_e = 0.5 * P->e;
	cp = cos(P->phi0);
	cp *= cp;
	P->c = sqrt(1 + P->es * cp * cp * P->rone_es);
	sp = sin(P->phi0);
	P->cosp0 = cos( phip0 = aasin(P->sinp0 = sp / P->c) );
	sp *= P->e;
	P->K = log(tan(FORTPI + 0.5 * phip0)) - P->c * (
		log(tan(FORTPI + 0.5 * P->phi0)) - P->hlf_e *
		log((1. + sp) / (1. - sp)));
	P->kR = P->k0 * sqrt(P->one_es) / (1. - sp * sp);
	P->inv = e_inverse;
	P->fwd = e_forward;
ENDENTRY(P)
