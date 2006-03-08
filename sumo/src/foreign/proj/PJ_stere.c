#ifndef lint
static const char SCCSID[]="@(#)PJ_stere.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double phits; \
	double sinX1; \
	double cosX1; \
	double akm1; \
	int	mode;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(stere, "Stereographic") "\n\tAzi, Sph&Ell\n\tlat_ts=";
PROJ_HEAD(ups, "Universal Polar Stereographic") "\n\tAzi, Sph&Ell\n\tsouth";
#define sinph0	P->sinX1
#define cosph0	P->cosX1
#define EPS10	1.e-10
#define TOL	1.e-8
#define NITER	8
#define CONV	1.e-10
#define S_POLE	0
#define N_POLE	1
#define OBLIQ	2
#define EQUIT	3
	static double
ssfn_(double phit, double sinphi, double eccen) {
	sinphi *= eccen;
	return (tan (.5 * (HALFPI + phit)) *
	   pow((1. - sinphi) / (1. + sinphi), .5 * eccen));
}
FORWARD(e_forward); /* ellipsoid */
	double coslam, sinlam, sinX=0.0, cosX=0.0, X, A, sinphi;

	coslam = cos(lp.lam);
	sinlam = sin(lp.lam);
	sinphi = sin(lp.phi);
	if (P->mode == OBLIQ || P->mode == EQUIT) {
		sinX = sin(X = 2. * atan(ssfn_(lp.phi, sinphi, P->e)) - HALFPI);
		cosX = cos(X);
	}
	switch (P->mode) {
	case OBLIQ:
		A = P->akm1 / (P->cosX1 * (1. + P->sinX1 * sinX +
		   P->cosX1 * cosX * coslam));
		xy.y = A * (P->cosX1 * sinX - P->sinX1 * cosX * coslam);
		goto xmul;
	case EQUIT:
		A = 2. * P->akm1 / (1. + cosX * coslam);
		xy.y = A * sinX;
xmul:
		xy.x = A * cosX;
		break;
	case S_POLE:
		lp.phi = -lp.phi;
		coslam = - coslam;
		sinphi = -sinphi;
	case N_POLE:
		xy.x = P->akm1 * pj_tsfn(lp.phi, sinphi, P->e);
		xy.y = - xy.x * coslam;
		break;
	}
	xy.x = xy.x * sinlam;
	return (xy);
}
FORWARD(s_forward); /* spheroid */
	double  sinphi, cosphi, coslam, sinlam;

	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	coslam = cos(lp.lam);
	sinlam = sin(lp.lam);
	switch (P->mode) {
	case EQUIT:
		xy.y = 1. + cosphi * coslam;
		goto oblcon;
	case OBLIQ:
		xy.y = 1. + sinph0 * sinphi + cosph0 * cosphi * coslam;
oblcon:
		if (xy.y <= EPS10) F_ERROR;
		xy.x = (xy.y = P->akm1 / xy.y) * cosphi * sinlam;
		xy.y *= (P->mode == EQUIT) ? sinphi :
		   cosph0 * sinphi - sinph0 * cosphi * coslam;
		break;
	case N_POLE:
		coslam = - coslam;
		lp.phi = - lp.phi;
	case S_POLE:
		if (fabs(lp.phi - HALFPI) < TOL) F_ERROR;
		xy.x = sinlam * ( xy.y = P->akm1 * tan(FORTPI + .5 * lp.phi) );
		xy.y *= coslam;
		break;
	}
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	double cosphi, sinphi, tp=0.0, phi_l=0.0, rho, halfe=0.0, halfpi=0.0;
	int i;

	rho = hypot(xy.x, xy.y);
	switch (P->mode) {
	case OBLIQ:
	case EQUIT:
		cosphi = cos( tp = 2. * atan2(rho * P->cosX1 , P->akm1) );
		sinphi = sin(tp);
                if( rho == 0.0 )
		    phi_l = asin(cosphi * P->sinX1);
                else
		    phi_l = asin(cosphi * P->sinX1 + (xy.y * sinphi * P->cosX1 / rho));

		tp = tan(.5 * (HALFPI + phi_l));
		xy.x *= sinphi;
		xy.y = rho * P->cosX1 * cosphi - xy.y * P->sinX1* sinphi;
		halfpi = HALFPI;
		halfe = .5 * P->e;
		break;
	case N_POLE:
		xy.y = -xy.y;
	case S_POLE:
		phi_l = HALFPI - 2. * atan(tp = - rho / P->akm1);
		halfpi = -HALFPI;
		halfe = -.5 * P->e;
		break;
	}
	for (i = NITER; i--; phi_l = lp.phi) {
		sinphi = P->e * sin(phi_l);
		lp.phi = 2. * atan(tp * pow((1.+sinphi)/(1.-sinphi),
		   halfe)) - halfpi;
		if (fabs(phi_l - lp.phi) < CONV) {
			if (P->mode == S_POLE)
				lp.phi = -lp.phi;
			lp.lam = (xy.x == 0. && xy.y == 0.) ? 0. : atan2(xy.x, xy.y);
			return (lp);
		}
	}
	I_ERROR;
}
INVERSE(s_inverse); /* spheroid */
	double  c, rh, sinc, cosc;

	sinc = sin(c = 2. * atan((rh = hypot(xy.x, xy.y)) / P->akm1));
	cosc = cos(c);
	lp.lam = 0.;
	switch (P->mode) {
	case EQUIT:
		if (fabs(rh) <= EPS10)
			lp.phi = 0.;
		else
			lp.phi = asin(xy.y * sinc / rh);
		if (cosc != 0. || xy.x != 0.)
			lp.lam = atan2(xy.x * sinc, cosc * rh);
		break;
	case OBLIQ:
		if (fabs(rh) <= EPS10)
			lp.phi = P->phi0;
		else
			lp.phi = asin(cosc * sinph0 + xy.y * sinc * cosph0 / rh);
		if ((c = cosc - sinph0 * sin(lp.phi)) != 0. || xy.x != 0.)
			lp.lam = atan2(xy.x * sinc * cosph0, c * rh);
		break;
	case N_POLE:
		xy.y = -xy.y;
	case S_POLE:
		if (fabs(rh) <= EPS10)
			lp.phi = P->phi0;
		else
			lp.phi = asin(P->mode == S_POLE ? - cosc : cosc);
		lp.lam = (xy.x == 0. && xy.y == 0.) ? 0. : atan2(xy.x, xy.y);
		break;
	}
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) { /* general initialization */
	double t;

	if (fabs((t = fabs(P->phi0)) - HALFPI) < EPS10)
		P->mode = P->phi0 < 0. ? S_POLE : N_POLE;
	else
		P->mode = t > EPS10 ? OBLIQ : EQUIT;
	P->phits = fabs(P->phits);
	if (P->es) {
		double X;

		switch (P->mode) {
		case N_POLE:
		case S_POLE:
			if (fabs(P->phits - HALFPI) < EPS10)
				P->akm1 = 2. * P->k0 /
				   sqrt(pow(1+P->e,1+P->e)*pow(1-P->e,1-P->e));
			else {
				P->akm1 = cos(P->phits) /
				   pj_tsfn(P->phits, t = sin(P->phits), P->e);
				t *= P->e;
				P->akm1 /= sqrt(1. - t * t);
			}
			break;
		case EQUIT:
			P->akm1 = 2. * P->k0;
			break;
		case OBLIQ:
			t = sin(P->phi0);
			X = 2. * atan(ssfn_(P->phi0, t, P->e)) - HALFPI;
			t *= P->e;
			P->akm1 = 2. * P->k0 * cos(P->phi0) / sqrt(1. - t * t);
			P->sinX1 = sin(X);
			P->cosX1 = cos(X);
			break;
		}
		P->inv = e_inverse;
		P->fwd = e_forward;
	} else {
		switch (P->mode) {
		case OBLIQ:
			sinph0 = sin(P->phi0);
			cosph0 = cos(P->phi0);
		case EQUIT:
			P->akm1 = 2. * P->k0;
			break;
		case S_POLE:
		case N_POLE:
			P->akm1 = fabs(P->phits - HALFPI) >= EPS10 ?
			   cos(P->phits) / tan(FORTPI - .5 * P->phits) :
			   2. * P->k0 ;
			break;
		}
		P->inv = s_inverse;
		P->fwd = s_forward;
	}
	return P;
}
ENTRY0(stere)
	P->phits = pj_param(P->params, "tlat_ts").i ?
		P->phits = pj_param(P->params, "rlat_ts").f : HALFPI;
ENDENTRY(setup(P))
ENTRY0(ups)
	/* International Ellipsoid */
	P->phi0 = pj_param(P->params, "bsouth").i ? - HALFPI: HALFPI;
	if (!P->es) E_ERROR(-34);
	P->k0 = .994;
	P->x0 = 2000000.;
	P->y0 = 2000000.;
	P->phits = HALFPI;
	P->lam0 = 0.;
ENDENTRY(setup(P))
