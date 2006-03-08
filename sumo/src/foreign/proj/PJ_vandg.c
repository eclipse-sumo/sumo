#ifndef lint
static const char SCCSID[]="@(#)PJ_vandg.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(vandg, "van der Grinten (I)") "\n\tMisc Sph";
# define TOL		1.e-10
# define THIRD		.33333333333333333333
# define TWO_THRD	.66666666666666666666
# define C2_27		.07407407407407407407
# define PI4_3		4.18879020478639098458
# define PISQ		9.86960440108935861869
# define TPISQ		19.73920880217871723738
# define HPISQ		4.93480220054467930934
FORWARD(s_forward); /* spheroid */
	double  al, al2, g, g2, p2;

	p2 = fabs(lp.phi / HALFPI);
	if ((p2 - TOL) > 1.) F_ERROR;
	if (p2 > 1.)
		p2 = 1.;
	if (fabs(lp.phi) <= TOL) {
		xy.x = lp.lam;
		xy.y = 0.;
	} else if (fabs(lp.lam) <= TOL || fabs(p2 - 1.) < TOL) {
		xy.x = 0.;
		xy.y = PI * tan(.5 * asin(p2));
		if (lp.phi < 0.) xy.y = -xy.y;
	} else {
		al = .5 * fabs(PI / lp.lam - lp.lam / PI);
		al2 = al * al;
		g = sqrt(1. - p2 * p2);
		g = g / (p2 + g - 1.);
		g2 = g * g;
		p2 = g * (2. / p2 - 1.);
		p2 = p2 * p2;
		xy.x = g - p2; g = p2 + al2;
		xy.x = PI * (al * xy.x + sqrt(al2 * xy.x * xy.x - g * (g2 - p2))) / g;
		if (lp.lam < 0.) xy.x = -xy.x;
		xy.y = fabs(xy.x / PI);
		xy.y = 1. - xy.y * (xy.y + 2. * al);
		if (xy.y < -TOL) F_ERROR;
		if (xy.y < 0.)	xy.y = 0.;
		else		xy.y = sqrt(xy.y) * (lp.phi < 0. ? -PI : PI);
	}
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double t, c0, c1, c2, c3, al, r2, r, m, d, ay, x2, y2;

	x2 = xy.x * xy.x;
	if ((ay = fabs(xy.y)) < TOL) {
		lp.phi = 0.;
		t = x2 * x2 + TPISQ * (x2 + HPISQ);
		lp.lam = fabs(xy.x) <= TOL ? 0. :
		   .5 * (x2 - PISQ + sqrt(t)) / xy.x;
		return (lp);
	}
	y2 = xy.y * xy.y;
	r = x2 + y2;	r2 = r * r;
	c1 = - PI * ay * (r + PISQ);
	c3 = r2 + TWOPI * (ay * r + PI * (y2 + PI * (ay + HALFPI)));
	c2 = c1 + PISQ * (r - 3. *  y2);
	c0 = PI * ay;
	c2 /= c3;
	al = c1 / c3 - THIRD * c2 * c2;
	m = 2. * sqrt(-THIRD * al);
	d = C2_27 * c2 * c2 * c2 + (c0 * c0 - THIRD * c2 * c1) / c3;
	if (((t = fabs(d = 3. * d / (al * m))) - TOL) <= 1.) {
		d = t > 1. ? (d > 0. ? 0. : PI) : acos(d);
		lp.phi = PI * (m * cos(d * THIRD + PI4_3) - THIRD * c2);
		if (xy.y < 0.) lp.phi = -lp.phi;
		t = r2 + TPISQ * (x2 - y2 + HPISQ);
		lp.lam = fabs(xy.x) <= TOL ? 0. :
		   .5 * (r - PISQ + (t <= 0. ? 0. : sqrt(t))) / xy.x;
	} else
		I_ERROR;
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(vandg) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
