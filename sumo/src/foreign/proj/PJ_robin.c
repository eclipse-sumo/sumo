#ifndef lint
static const char SCCSID[]="@(#)PJ_robin.c	4.1 94/02/15     GIE     REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(robin, "Robinson") "\n\tPCyl., Sph.";
#define V(C,z) (C.c0 + z * (C.c1 + z * (C.c2 + z * C.c3)))
#define DV(C,z) (C.c1 + z * (C.c2 + C.c2 + z * 3. * C.c3))
/* note: following terms based upon 5 deg. intervals in degrees. */
static struct COEFS {
	float c0, c1, c2, c3;
} X[] = {
1,	-5.67239e-12,	-7.15511e-05,	3.11028e-06,
0.9986,	-0.000482241,	-2.4897e-05,	-1.33094e-06,
0.9954,	-0.000831031,	-4.4861e-05,	-9.86588e-07,
0.99,	-0.00135363,	-5.96598e-05,	3.67749e-06,
0.9822,	-0.00167442,	-4.4975e-06,	-5.72394e-06,
0.973,	-0.00214869,	-9.03565e-05,	1.88767e-08,
0.96,	-0.00305084,	-9.00732e-05,	1.64869e-06,
0.9427,	-0.00382792,	-6.53428e-05,	-2.61493e-06,
0.9216,	-0.00467747,	-0.000104566,	4.8122e-06,
0.8962,	-0.00536222,	-3.23834e-05,	-5.43445e-06,
0.8679,	-0.00609364,	-0.0001139,	3.32521e-06,
0.835,	-0.00698325,	-6.40219e-05,	9.34582e-07,
0.7986,	-0.00755337,	-5.00038e-05,	9.35532e-07,
0.7597,	-0.00798325,	-3.59716e-05,	-2.27604e-06,
0.7186,	-0.00851366,	-7.0112e-05,	-8.63072e-06,
0.6732,	-0.00986209,	-0.000199572,	1.91978e-05,
0.6213,	-0.010418,	8.83948e-05,	6.24031e-06,
0.5722,	-0.00906601,	0.000181999,	6.24033e-06,
0.5322, 0.,0.,0.  },
Y[] = {
0,	0.0124,	3.72529e-10,	1.15484e-09,
0.062,	0.0124001,	1.76951e-08,	-5.92321e-09,
0.124,	0.0123998,	-7.09668e-08,	2.25753e-08,
0.186,	0.0124008,	2.66917e-07,	-8.44523e-08,
0.248,	0.0123971,	-9.99682e-07,	3.15569e-07,
0.31,	0.0124108,	3.73349e-06,	-1.1779e-06,
0.372,	0.0123598,	-1.3935e-05,	4.39588e-06,
0.434,	0.0125501,	5.20034e-05,	-1.00051e-05,
0.4968,	0.0123198,	-9.80735e-05,	9.22397e-06,
0.5571,	0.0120308,	4.02857e-05,	-5.2901e-06,
0.6176,	0.0120369,	-3.90662e-05,	7.36117e-07,
0.6769,	0.0117015,	-2.80246e-05,	-8.54283e-07,
0.7346,	0.0113572,	-4.08389e-05,	-5.18524e-07,
0.7903,	0.0109099,	-4.86169e-05,	-1.0718e-06,
0.8435,	0.0103433,	-6.46934e-05,	5.36384e-09,
0.8936,	0.00969679,	-6.46129e-05,	-8.54894e-06,
0.9394,	0.00840949,	-0.000192847,	-4.21023e-06,
0.9761,	0.00616525,	-0.000256001,	-4.21021e-06,
1., 0.,0.,0 };
#define FXC	0.8487
#define FYC	1.3523
#define C1	11.45915590261646417544
#define RC1	0.08726646259971647884
#define NODES	18
#define ONEEPS	1.000001
#define EPS	1e-8
FORWARD(s_forward); /* spheroid */
	int i;
	double dphi;

	i = floor((dphi = fabs(lp.phi)) * C1);
	if (i >= NODES) i = NODES - 1;
	dphi = RAD_TO_DEG * (dphi - RC1 * i);
	xy.x = V(X[i], dphi) * FXC * lp.lam;
	xy.y = V(Y[i], dphi) * FYC;
	if (lp.phi < 0.) xy.y = -xy.y;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	int i;
	double t, t1;
	struct COEFS T;

	lp.lam = xy.x / FXC;
	lp.phi = fabs(xy.y / FYC);
	if (lp.phi >= 1.) { /* simple pathologic cases */
		if (lp.phi > ONEEPS) I_ERROR
		else {
			lp.phi = xy.y < 0. ? -HALFPI : HALFPI;
			lp.lam /= X[NODES].c0;
		}
	} else { /* general problem */
		/* in Y space, reduce to table interval */
		for (i = floor(lp.phi * NODES);;) {
			if (Y[i].c0 > lp.phi) --i;
			else if (Y[i+1].c0 <= lp.phi) ++i;
			else break;
		}
		T = Y[i];
		/* first guess, linear interp */
		t = 5. * (lp.phi - T.c0)/(Y[i+1].c0 - T.c0);
		/* make into root */
		T.c0 -= lp.phi;
		for (;;) { /* Newton-Raphson reduction */
			t -= t1 = V(T,t) / DV(T,t);
			if (fabs(t1) < EPS)
				break;
		}
		lp.phi = (5 * i + t) * DEG_TO_RAD;
		if (xy.y < 0.) lp.phi = -lp.phi;
		lp.lam /= V(X[i], t);
	}
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(robin) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
