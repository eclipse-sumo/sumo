#ifndef lint
static const char SCCSID[]="@(#)PJ_laea.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	sinb1; \
	double	cosb1; \
	double	xmf; \
	double	ymf; \
	double	mmf; \
	double	qp; \
	double	dd; \
	double	rq; \
	double	*apa; \
	int		mode;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(laea, "Lambert Azimuthal Equal Area") "\n\tAzi, Sph&Ell";
#define sinph0	P->sinb1
#define cosph0	P->cosb1
#define EPS10	1.e-10
#define NITER	20
#define CONV	1.e-10
#define N_POLE	0
#define S_POLE	1
#define EQUIT	2
#define OBLIQ	3
FORWARD(e_forward); /* ellipsoid */
	double coslam, sinlam, sinphi, q, sinb=0.0, cosb=0.0, b=0.0;

	coslam = cos(lp.lam);
	sinlam = sin(lp.lam);
	sinphi = sin(lp.phi);
	q = pj_qsfn(sinphi, P->e, P->one_es);
	if (P->mode == OBLIQ || P->mode == EQUIT) {
		sinb = q / P->qp;
		cosb = sqrt(1. - sinb * sinb);
	}
	switch (P->mode) {
	case OBLIQ:
		b = 1. + P->sinb1 * sinb + P->cosb1 * cosb * coslam;
		break;
	case EQUIT:
		b = 1. + cosb * coslam;
		break;
	case N_POLE:
		b = HALFPI + lp.phi;
		q = P->qp - q;
		break;
	case S_POLE:
		b = lp.phi - HALFPI;
		q = P->qp + q;
		break;
	}
	if (fabs(b) < EPS10) F_ERROR;
	switch (P->mode) {
	case OBLIQ:
		xy.y = P->ymf * ( b = sqrt(2. / b) )
		   * (P->cosb1 * sinb - P->sinb1 * cosb * coslam);
		goto eqcon;
		break;
	case EQUIT:
		xy.y = (b = sqrt(2. / (1. + cosb * coslam))) * sinb * P->ymf;
eqcon:
		xy.x = P->xmf * b * cosb * sinlam;
		break;
	case N_POLE:
	case S_POLE:
		if (q >= 0.) {
			xy.x = (b = sqrt(q)) * sinlam;
			xy.y = coslam * (P->mode == S_POLE ? b : -b);
		} else
			xy.x = xy.y = 0.;
		break;
	}
	return (xy);
}
FORWARD(s_forward); /* spheroid */
	double  coslam, cosphi, sinphi;

	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	coslam = cos(lp.lam);
	switch (P->mode) {
	case EQUIT:
		xy.y = 1. + cosphi * coslam;
		goto oblcon;
	case OBLIQ:
		xy.y = 1. + sinph0 * sinphi + cosph0 * cosphi * coslam;
oblcon:
		if (xy.y <= EPS10) F_ERROR;
		xy.x = (xy.y = sqrt(2. / xy.y)) * cosphi * sin(lp.lam);
		xy.y *= P->mode == EQUIT ? sinphi :
		   cosph0 * sinphi - sinph0 * cosphi * coslam;
		break;
	case N_POLE:
		coslam = -coslam;
	case S_POLE:
		if (fabs(lp.phi + P->phi0) < EPS10) F_ERROR;
		xy.y = FORTPI - lp.phi * .5;
		xy.y = 2. * (P->mode == S_POLE ? cos(xy.y) : sin(xy.y));
		xy.x = xy.y * sin(lp.lam);
		xy.y *= coslam;
		break;
	}
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	double cCe, sCe, q, rho, ab=0.0;

	switch (P->mode) {
	case EQUIT:
	case OBLIQ:
		if ((rho = hypot(xy.x /= P->dd, xy.y *=  P->dd)) < EPS10) {
			lp.lam = 0.;
			lp.phi = P->phi0;
			return (lp);
		}
		cCe = cos(sCe = 2. * asin(.5 * rho / P->rq));
		xy.x *= (sCe = sin(sCe));
		if (P->mode == OBLIQ) {
			q = P->qp * (ab = cCe * P->sinb1 + xy.y * sCe * P->cosb1 / rho);
			xy.y = rho * P->cosb1 * cCe - xy.y * P->sinb1 * sCe;
		} else {
			q = P->qp * (ab = xy.y * sCe / rho);
			xy.y = rho * cCe;
		}
		break;
	case N_POLE:
		xy.y = -xy.y;
	case S_POLE:
		if (!(q = (xy.x * xy.x + xy.y * xy.y)) ) {
			lp.lam = 0.;
			lp.phi = P->phi0;
			return (lp);
		}
		/*
		q = P->qp - q;
		*/
		ab = 1. - q / P->qp;
		if (P->mode == S_POLE)
			ab = - ab;
		break;
	}
	lp.lam = atan2(xy.x, xy.y);
	lp.phi = pj_authlat(asin(ab), P->apa);
	return (lp);
}
INVERSE(s_inverse); /* spheroid */
	double  cosz=0.0, rh, sinz=0.0;

	rh = hypot(xy.x, xy.y);
	if ((lp.phi = rh * .5 ) > 1.) I_ERROR;
	lp.phi = 2. * asin(lp.phi);
	if (P->mode == OBLIQ || P->mode == EQUIT) {
		sinz = sin(lp.phi);
		cosz = cos(lp.phi);
	}
	switch (P->mode) {
	case EQUIT:
		lp.phi = fabs(rh) <= EPS10 ? 0. : asin(xy.y * sinz / rh);
		xy.x *= sinz;
		xy.y = cosz * rh;
		break;
	case OBLIQ:
		lp.phi = fabs(rh) <= EPS10 ? P->phi0 :
		   asin(cosz * sinph0 + xy.y * sinz * cosph0 / rh);
		xy.x *= sinz * cosph0;
		xy.y = (cosz - sin(lp.phi) * sinph0) * rh;
		break;
	case N_POLE:
		xy.y = -xy.y;
		lp.phi = HALFPI - lp.phi;
		break;
	case S_POLE:
		lp.phi -= HALFPI;
		break;
	}
	lp.lam = (xy.y == 0. && (P->mode == EQUIT || P->mode == OBLIQ)) ?
		0. : atan2(xy.x, xy.y);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(laea)
	double t;

	if (fabs((t = fabs(P->phi0)) - HALFPI) < EPS10)
		P->mode = P->phi0 < 0. ? S_POLE : N_POLE;
	else if (fabs(t) < EPS10)
		P->mode = EQUIT;
	else
		P->mode = OBLIQ;
	if (P->es) {
		double sinphi;

		P->e = sqrt(P->es);
		P->qp = pj_qsfn(1., P->e, P->one_es);
		P->mmf = .5 / (1. - P->es);
		P->apa = pj_authset(P->es);
		switch (P->mode) {
		case N_POLE:
		case S_POLE:
			P->dd = 1.;
			break;
		case EQUIT:
			P->dd = 1. / (P->rq = sqrt(.5 * P->qp));
			P->xmf = 1.;
			P->ymf = .5 * P->qp;
			break;
		case OBLIQ:
			P->rq = sqrt(.5 * P->qp);
			sinphi = sin(P->phi0);
			P->sinb1 = pj_qsfn(sinphi, P->e, P->one_es) / P->qp;
			P->cosb1 = sqrt(1. - P->sinb1 * P->sinb1);
			P->dd = cos(P->phi0) / (sqrt(1. - P->es * sinphi * sinphi) *
			   P->rq * P->cosb1);
			P->ymf = (P->xmf = P->rq) / P->dd;
			P->xmf *= P->dd;
			break;
		}
		P->inv = e_inverse;
		P->fwd = e_forward;
	} else {
		if (P->mode == OBLIQ) {
			sinph0 = sin(P->phi0);
			cosph0 = cos(P->phi0);
		}
		P->inv = s_inverse;
		P->fwd = s_forward;
	}
ENDENTRY(P)
