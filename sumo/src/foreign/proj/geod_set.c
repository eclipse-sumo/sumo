#ifndef lint
static const char SCCSID[]="@(#)geod_set.c	4.8	95/09/23	GIE	REL";
#endif

#define _IN_GEOD_SET

#include <string.h>
#include "projects.h"
#include "geodesic.h"
#include "emess.h"
	void
geod_set(int argc, char **argv) {
	paralist *start = 0, *curr;
	double es;
	char *name;
	int i;

    /* put arguments into internal linked list */
	if (argc <= 0)
		emess(1, "no arguments in initialization list");
	for (i = 0; i < argc; ++i)
		if (i)
			curr = curr->next = pj_mkparam(argv[i]);
		else
			start = curr = pj_mkparam(argv[i]);
	/* set elliptical parameters */
	if (pj_ell_set(start, &geod_a, &es)) emess(1,"ellipse setup failure");
	/* set units */
	if (name = pj_param(start, "sunits").s) {
		char *s;
                struct PJ_UNITS *unit_list = pj_get_units_ref();
		for (i = 0; (s = unit_list[i].id) && strcmp(name, s) ; ++i) ;
		if (!s)
			emess(1,"%s unknown unit conversion id", name);
		fr_meter = 1. / (to_meter = atof(unit_list[i].to_meter));
	} else
		to_meter = fr_meter = 1.;
	if (ellipse = es != 0.) {
		onef = sqrt(1. - es);
		geod_f = 1 - onef;
		f2 = geod_f/2;
		f4 = geod_f/4;
		f64 = geod_f*geod_f/64;
	} else {
		onef = 1.;
		geod_f = f2 = f4 = f64 = 0.;
	}
	/* check if line or arc mode */
	if (pj_param(start, "tlat_1").i) {
		double del_S;
#undef f
		phi1 = pj_param(start, "rlat_1").f;
		lam1 = pj_param(start, "rlon_1").f;
		if (pj_param(start, "tlat_2").i) {
			phi2 = pj_param(start, "rlat_2").f;
			lam2 = pj_param(start, "rlon_2").f;
			geod_inv();
			geod_pre();
		} else if (geod_S = pj_param(start, "dS").f) {
			al12 = pj_param(start, "rA").f;
			geod_pre();
			geod_for();
		} else emess(1,"incomplete geodesic/arc info");
		if ((n_alpha = pj_param(start, "in_A").i) > 0) {
			if (!(del_alpha = pj_param(start, "rdel_A").f))
				emess(1,"del azimuth == 0");
		} else if (del_S = fabs(pj_param(start, "ddel_S").f)) {
			n_S = geod_S / del_S + .5;
		} else if ((n_S = pj_param(start, "in_S").i) <= 0)
			emess(1,"no interval divisor selected");
	}
	/* free up linked list */
	for ( ; start; start = curr) {
		curr = start->next;
		pj_dalloc(start);
	}
}
