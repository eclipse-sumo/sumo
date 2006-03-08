#ifndef lint
static const char SCCSID[]="@(#)PJ_omerc.c	4.2	95/01/01	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	alpha, lamc, lam1, phi1, lam2, phi2, Gamma, al, bl, el, \
		singam, cosgam, sinrot, cosrot, u_0; \
	int		ellips, rot;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(omerc, "Oblique Mercator")
	"\n\tCyl, Sph&Ell\n\t no_rot rot_conv no_uoff and\n\t"
"alpha= lonc= or\n\t lon_1= lat_1= lon_2= lat_2=";
#define TOL	1.e-7
#define EPS	1.e-10
#define TSFN0(x)	tan(.5 * (HALFPI - (x)))
FORWARD(e_forward); /* ellipsoid & spheroid */
	double  con, q, s, ul, us, vl, vs;

	vl = sin(P->bl * lp.lam);
	if (fabs(fabs(lp.phi) - HALFPI) <= EPS) {
		ul = lp.phi < 0. ? -P->singam : P->singam;
		us = P->al * lp.phi / P->bl;
	} else {
		q = P->el / (P->ellips ? pow(pj_tsfn(lp.phi, sin(lp.phi), P->e), P->bl)
			: TSFN0(lp.phi));
		s = .5 * (q - 1. / q);
		ul = 2. * (s * P->singam - vl * P->cosgam) / (q + 1. / q);
		con = cos(P->bl * lp.lam);
		if (fabs(con) >= TOL) {
			us = P->al * atan((s * P->cosgam + vl * P->singam) / con) / P->bl;
			if (con < 0.)
				us += PI * P->al / P->bl;
		} else
			us = P->al * P->bl * lp.lam;
	}
	if (fabs(fabs(ul) - 1.) <= EPS) F_ERROR;
	vs = .5 * P->al * log((1. - ul) / (1. + ul)) / P->bl;
	us -= P->u_0;
	if (! P->rot) {
		xy.x = us;
		xy.y = vs;
	} else {
		xy.x = vs * P->cosrot + us * P->sinrot;
		xy.y = us * P->cosrot - vs * P->sinrot;
	}
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid & spheroid */
	double  q, s, ul, us, vl, vs;

	if (! P->rot) {
		us = xy.x;
		vs = xy.y;
	} else {
		vs = xy.x * P->cosrot - xy.y * P->sinrot;
		us = xy.y * P->cosrot + xy.x * P->sinrot;
	}
	us += P->u_0;
	q = exp(- P->bl * vs / P->al);
	s = .5 * (q - 1. / q);
	vl = sin(P->bl * us / P->al);
	ul = 2. * (vl * P->cosgam + s * P->singam) / (q + 1. / q);
	if (fabs(fabs(ul) - 1.) < EPS) {
		lp.lam = 0.;
		lp.phi = ul < 0. ? -HALFPI : HALFPI;
	} else {
		lp.phi = P->el / sqrt((1. + ul) / (1. - ul));
		if (P->ellips) {
			if ((lp.phi = pj_phi2(pow(lp.phi, 1. / P->bl), P->e)) == HUGE_VAL)
				I_ERROR;
		} else
			lp.phi = HALFPI - 2. * atan(lp.phi);
		lp.lam = - atan2((s * P->cosgam -
			vl * P->singam), cos(P->bl * us / P->al)) / P->bl;
	}
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(omerc)
	double con, com, cosph0, d, f, h, l, sinph0, p, j;
	int azi;

	P->rot	= pj_param(P->params, "bno_rot").i == 0;
	if( (azi	= pj_param(P->params, "talpha").i) != 0.0) {
		P->lamc	= pj_param(P->params, "rlonc").f;
		P->alpha	= pj_param(P->params, "ralpha").f;
		if ( fabs(P->alpha) <= TOL ||
			fabs(fabs(P->phi0) - HALFPI) <= TOL ||
			fabs(fabs(P->alpha) - HALFPI) <= TOL)
			E_ERROR(-32);
	} else {
		P->lam1	= pj_param(P->params, "rlon_1").f;
		P->phi1	= pj_param(P->params, "rlat_1").f;
		P->lam2	= pj_param(P->params, "rlon_2").f;
		P->phi2	= pj_param(P->params, "rlat_2").f;
		if (fabs(P->phi1 - P->phi2) <= TOL ||
			(con = fabs(P->phi1)) <= TOL ||
			fabs(con - HALFPI) <= TOL ||
			fabs(fabs(P->phi0) - HALFPI) <= TOL ||
			fabs(fabs(P->phi2) - HALFPI) <= TOL) E_ERROR(-33);
	}
	com = (P->ellips = P->es > 0.) ? sqrt(P->one_es) : 1.;
	if (fabs(P->phi0) > EPS) {
		sinph0 = sin(P->phi0);
		cosph0 = cos(P->phi0);
		if (P->ellips) {
			con = 1. - P->es * sinph0 * sinph0;
			P->bl = cosph0 * cosph0;
			P->bl = sqrt(1. + P->es * P->bl * P->bl / P->one_es);
			P->al = P->bl * P->k0 * com / con;
			d = P->bl * com / (cosph0 * sqrt(con));
		} else {
			P->bl = 1.;
			P->al = P->k0;
			d = 1. / cosph0;
		}
		if ((f = d * d - 1.) <= 0.)
			f = 0.;
		else {
			f = sqrt(f);
			if (P->phi0 < 0.)
				f = -f;
		}
		P->el = f += d;
		if (P->ellips)	P->el *= pow(pj_tsfn(P->phi0, sinph0, P->e), P->bl);
		else		P->el *= TSFN0(P->phi0);
	} else {
		P->bl = 1. / com;
		P->al = P->k0;
		P->el = d = f = 1.;
	}
	if (azi) {
		P->Gamma = asin(sin(P->alpha) / d);
		P->lam0 = P->lamc - asin((.5 * (f - 1. / f)) *
		   tan(P->Gamma)) / P->bl;
	} else {
		if (P->ellips) {
			h = pow(pj_tsfn(P->phi1, sin(P->phi1), P->e), P->bl);
			l = pow(pj_tsfn(P->phi2, sin(P->phi2), P->e), P->bl);
		} else {
			h = TSFN0(P->phi1);
			l = TSFN0(P->phi2);
		}
		f = P->el / h;
		p = (l - h) / (l + h);
		j = P->el * P->el;
		j = (j - l * h) / (j + l * h);
		if ((con = P->lam1 - P->lam2) < -PI)
			P->lam2 -= TWOPI;
		else if (con > PI)
			P->lam2 += TWOPI;
		P->lam0 = adjlon(.5 * (P->lam1 + P->lam2) - atan(
		   j * tan(.5 * P->bl * (P->lam1 - P->lam2)) / p) / P->bl);
		P->Gamma = atan(2. * sin(P->bl * adjlon(P->lam1 - P->lam0)) /
		   (f - 1. / f));
		P->alpha = asin(d * sin(P->Gamma));
	}
	P->singam = sin(P->Gamma);
	P->cosgam = cos(P->Gamma);
	f = pj_param(P->params, "brot_conv").i ? P->Gamma : P->alpha;
	P->sinrot = sin(f);
	P->cosrot = cos(f);
	P->u_0 = pj_param(P->params, "bno_uoff").i ? 0. :
		fabs(P->al * atan(sqrt(d * d - 1.) / P->cosrot) / P->bl);
	if (P->phi0 < 0.)
		P->u_0 = - P->u_0;
	P->inv = e_inverse;
	P->fwd = e_forward;
ENDENTRY(P)
