#ifndef lint
static const char SCCSID[]="@(#)PJ_labrd.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	Az, kRg, p0s, A, C, Ca, Cb, Cc, Cd; \
	int		rot;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(labrd, "Laborde") "\n\tCyl, Sph\n\tSpecial for Madagascar";
#define EPS	1.e-10
FORWARD(e_forward);
	double V1, V2, ps, sinps, cosps, sinps2, cosps2, I1, I2, I3, I4, I5, I6,
		x2, y2, t;

	V1 = P->A * log( tan(FORTPI + .5 * lp.phi) );
	t = P->e * sin(lp.phi);
	V2 = .5 * P->e * P->A * log ((1. + t)/(1. - t));
	ps = 2. * (atan(exp(V1 - V2 + P->C)) - FORTPI);
	I1 = ps - P->p0s;
	cosps = cos(ps);	cosps2 = cosps * cosps;
	sinps = sin(ps);	sinps2 = sinps * sinps;
	I4 = P->A * cosps;
	I2 = .5 * P->A * I4 * sinps;
	I3 = I2 * P->A * P->A * (5. * cosps2 - sinps2) / 12.;
	I6 = I4 * P->A * P->A;
	I5 = I6 * (cosps2 - sinps2) / 6.;
	I6 *= P->A * P->A *
		(5. * cosps2 * cosps2 + sinps2 * (sinps2 - 18. * cosps2)) / 120.;
	t = lp.lam * lp.lam;
	xy.x = P->kRg * lp.lam * (I4 + t * (I5 + t * I6));
	xy.y = P->kRg * (I1 + t * (I2 + t * I3));
	x2 = xy.x * xy.x;
	y2 = xy.y * xy.y;
	V1 = 3. * xy.x * y2 - xy.x * x2;
	V2 = xy.y * y2 - 3. * x2 * xy.y;
	xy.x += P->Ca * V1 + P->Cb * V2;
	xy.y += P->Ca * V2 - P->Cb * V1;
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid & spheroid */
	double x2, y2, V1, V2, V3, V4, t, t2, ps, pe, tpe, s,
		I7, I8, I9, I10, I11, d, Re;
	int i;

	x2 = xy.x * xy.x;
	y2 = xy.y * xy.y;
	V1 = 3. * xy.x * y2 - xy.x * x2;
	V2 = xy.y * y2 - 3. * x2 * xy.y;
	V3 = xy.x * (5. * y2 * y2 + x2 * (-10. * y2 + x2 ));
	V4 = xy.y * (5. * x2 * x2 + y2 * (-10. * x2 + y2 ));
	xy.x += - P->Ca * V1 - P->Cb * V2 + P->Cc * V3 + P->Cd * V4;
	xy.y +=   P->Cb * V1 - P->Ca * V2 - P->Cd * V3 + P->Cc * V4;
	ps = P->p0s + xy.y / P->kRg;
	pe = ps + P->phi0 - P->p0s;
	for ( i = 20; i; --i) {
		V1 = P->A * log(tan(FORTPI + .5 * pe));
		tpe = P->e * sin(pe);
		V2 = .5 * P->e * P->A * log((1. + tpe)/(1. - tpe));
		t = ps - 2. * (atan(exp(V1 - V2 + P->C)) - FORTPI);
		pe += t;
		if (fabs(t) < EPS)
			break;
	}
/*
	if (!i) {
	} else {
	}
*/
	t = P->e * sin(pe);
	t = 1. - t * t;
	Re = P->one_es / ( t * sqrt(t) );
	t = tan(ps);
	t2 = t * t;
	s = P->kRg * P->kRg;
	d = Re * P->k0 * P->kRg;
	I7 = t / (2. * d);
	I8 = t * (5. + 3. * t2) / (24. * d * s);
	d = cos(ps) * P->kRg * P->A;
	I9 = 1. / d;
	d *= s;
	I10 = (1. + 2. * t2) / (6. * d);
	I11 = (5. + t2 * (28. + 24. * t2)) / (120. * d * s);
	x2 = xy.x * xy.x;
	lp.phi = pe + x2 * (-I7 + I8 * x2);
	lp.lam = xy.x * (I9 + x2 * (-I10 + x2 * I11));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(labrd)
	double Az, sinp, R, N, t;

	P->rot	= pj_param(P->params, "bno_rot").i == 0;
	Az = pj_param(P->params, "razi").f;
	sinp = sin(P->phi0);
	t = 1. - P->es * sinp * sinp;
	N = 1. / sqrt(t);
	R = P->one_es * N / t;
	P->kRg = P->k0 * sqrt( N * R );
	P->p0s = atan( sqrt(R / N) * tan(P->phi0) );
	P->A = sinp / sin(P->p0s);
	t = P->e * sinp;
	P->C = .5 * P->e * P->A * log((1. + t)/(1. - t)) +
		- P->A * log( tan(FORTPI + .5 * P->phi0))
		+ log( tan(FORTPI + .5 * P->p0s));
	t = Az + Az;
	P->Ca = (1. - cos(t)) * ( P->Cb = 1. / (12. * P->kRg * P->kRg) );
	P->Cb *= sin(t);
	P->Cc = 3. * (P->Ca * P->Ca - P->Cb * P->Cb);
	P->Cd = 6. * P->Ca * P->Cb;
	P->inv = e_inverse;
	P->fwd = e_forward;
ENDENTRY(P)
