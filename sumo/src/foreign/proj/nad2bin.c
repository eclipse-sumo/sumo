/* Convert bivariate ASCII NAD27 to NAD83 tables to binary structure */
#ifndef lint
static const char SCCSID[]="@(#)nad2bin.c	4.2 93/08/25 GIE REL";
#endif
#include <stdio.h>
#include <stdlib.h>
#define PJ_LIB__
#include <projects.h>
#define U_SEC_TO_RAD 4.848136811095359935899141023e-12
	static char
*usage = "<ASCII_dist_table local_bin_table";
	void
main(int argc, char **argv) {
	struct CTABLE ct;
	FLP *p, t;
	size_t tsize;
	int i, j, ichk;
	long lam, laml, phi, phil;
	FILE *bin;

	if (argc != 2) {
		fprintf(stderr,"usage: %s %s\n", argv[0], usage);
		exit(1);
	}
	fgets(ct.id, MAX_TAB_ID, stdin);
	scanf("%d %d %*d %lf %lf %lf %lf", &ct.lim.lam, &ct.lim.phi,
		&ct.ll.lam, &ct.del.lam, &ct.ll.phi, &ct.del.phi);
	if (!(ct.cvs = (FLP *)malloc(tsize = ct.lim.lam * ct.lim.phi *
		sizeof(FLP)))) {
		perror("mem. alloc");
		exit(1);
	}
	ct.ll.lam *= DEG_TO_RAD;
	ct.ll.phi *= DEG_TO_RAD;
	ct.del.lam *= DEG_TO_RAD;
	ct.del.phi *= DEG_TO_RAD;
	/* load table */
	for (p = ct.cvs, i = 0; i < ct.lim.phi; ++i) {
		scanf("%d:%ld %ld", &ichk, &laml, &phil);
		if (ichk != i) {
			fprintf(stderr,"format check on row\n");
			exit(1);
		}
		t.lam = laml * U_SEC_TO_RAD;
		t.phi = phil * U_SEC_TO_RAD;
		*p++ = t;
		for (j = 1; j < ct.lim.lam; ++j) {
			scanf("%ld %ld", &lam, &phi);
			t.lam = (laml += lam) * U_SEC_TO_RAD;
			t.phi = (phil += phi) * U_SEC_TO_RAD;
			*p++ = t;
		}
	}
	if (feof(stdin)) {
		fprintf(stderr, "premature EOF\n");
		exit(1);
	}
	if (!(bin = freopen(argv[1], "wb", stdout))) {
		perror(argv[1]);
		exit(2);
	}
	if (fwrite(&ct, sizeof(ct), 1, stdout) != 1 ||
		fwrite(ct.cvs, tsize, 1, stdout) != 1) {
		fprintf(stderr, "output failure\n");
		exit(2);
	}
	exit(0); /* normal completion */
}
