#ifndef lint
static const char SCCSID[]="@(#)geod_for.c	4.6	95/09/23	GIE	REL";
#endif
# include "projects.h"
# include "geodesic.h"
# define MERI_TOL 1e-9
	static double
th1,costh1,sinth1,sina12,cosa12,M,N,c1,c2,D,P,s1;
	static int
merid, signS;
	void
geod_pre(void) {
	al12 = adjlon(al12); /* reduce to  +- 0-PI */
	signS = fabs(al12) > HALFPI ? 1 : 0;
	th1 = ellipse ? atan(onef * tan(phi1)) : phi1;
	costh1 = cos(th1);
	sinth1 = sin(th1);
	if ((merid = fabs(sina12 = sin(al12)) < MERI_TOL)) {
		sina12 = 0.;
		cosa12 = fabs(al12) < HALFPI ? 1. : -1.;
		M = 0.;
	} else {
		cosa12 = cos(al12);
		M = costh1 * sina12;
	}
	N = costh1 * cosa12;
	if (ellipse) {
		if (merid) {
			c1 = 0.;
			c2 = f4;
			D = 1. - c2;
			D *= D;
			P = c2 / D;
		} else {
			c1 = geod_f * M;
			c2 = f4 * (1. - M * M);
			D = (1. - c2)*(1. - c2 - c1 * M);
			P = (1. + .5 * c1 * M) * c2 / D;
		}
	}
	if (merid) s1 = HALFPI - th1;
	else {
		s1 = (fabs(M) >= 1.) ? 0. : acos(M);
		s1 =  sinth1 / sin(s1);
		s1 = (fabs(s1) >= 1.) ? 0. : acos(s1);
	}
}
	void
geod_for(void) {
	double d,sind,u,V,X,ds,cosds,sinds,ss,de;

	if (ellipse) {
		d = geod_S / (D * geod_a);
		if (signS) d = -d;
		u = 2. * (s1 - d);
		V = cos(u + d);
		X = c2 * c2 * (sind = sin(d)) * cos(d) * (2. * V * V - 1.);
		ds = d + X - 2. * P * V * (1. - 2. * P * cos(u)) * sind;
		ss = s1 + s1 - ds;
	} else {
		ds = geod_S / geod_a;
		if (signS) ds = - ds;
	}
	cosds = cos(ds);
	sinds = sin(ds);
	if (signS) sinds = - sinds;
	al21 = N * cosds - sinth1 * sinds;
	if (merid) {
		phi2 = atan( tan(HALFPI + s1 - ds) / onef);
		if (al21 > 0.) {
			al21 = PI;
			if (signS)
				de = PI;
			else {
				phi2 = - phi2;
				de = 0.;
			}
		} else {
			al21 = 0.;
			if (signS) {
				phi2 = - phi2;
				de = 0;
			} else
				de = PI;
		}
	} else {
		al21 = atan(M / al21);
		if (al21 > 0)
			al21 += PI;
		if (al12 < 0.)
			al21 -= PI;
		al21 = adjlon(al21);
		phi2 = atan(-(sinth1 * cosds + N * sinds) * sin(al21) /
			(ellipse ? onef * M : M));
		de = atan2(sinds * sina12 ,
			(costh1 * cosds - sinth1 * sinds * cosa12));
		if (ellipse)
			if (signS)
				de += c1 * ((1. - c2) * ds +
					c2 * sinds * cos(ss));
			else
				de -= c1 * ((1. - c2) * ds -
					c2 * sinds * cos(ss));
	}
	lam2 = adjlon( lam1 + de );
}
