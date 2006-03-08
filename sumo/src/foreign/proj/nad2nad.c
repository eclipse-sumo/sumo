/* <<<< North American Datum Transfer Program >>>> */
#ifndef lint
static const char SCCSID[]="@(#)nad2nad.c	4.5	94/02/15	GIE	REL";
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define PJ_LIST_H <nad_list.h>
#include <projects.h>
#include "emess.h"

#define MAX_LINE 200
#define MAX_PARGS 100
#define PJ_INVERS(P) (P->inv ? 1 : 0)
	static int
echoin = 0,	/* echo input data to output line */
tag = '#';	/* beginning of line tag character */
	static char
*oform = (char *)0,	/* output format for x-y or decimal degrees */
*oterr = "*\t*",	/* output line for unprojectable input */
*inargs = 0,
*outargs = 0,
*czone = 0,
*usage =
"%s\nusage: %s [ -eEfihortwW [args] ] [ files ]\n";
	struct CTABLE
*ctab = 0,
*htab = 0;
static struct TAG_LIST {
	char *tag;
	short sw;
} ops_list[] = {
	"utm=",	0,
	"spcs=", 1,
	"feet", 2,
	"27", 3,
	"83", 4,
	"hp", 5,
	"bin", 6,
	"rev", 7,
	0, 0,
};
static struct IO_CON {
	short rev;	/* reverse lon/lat or x/y */
	short bin;	/* io binary */
	short ll;	/* io lat-lon */
	short t83;	/* data in 83 datum */
	short zone;	/* <100 utm zone, ==0 geog,  else state plane zone */
	short nprojc; /* number of entries in projc */
	char *hp;	/* high precision name */
	char *projc[10];	/* params for pj_init */
	PJ *cnv;
} input = {
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,
}, output = {
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,
};
	static void
set_zone(int in, struct IO_CON *io) {
	char tmp[20];

	if (io->hp) {
		io->t83 = 1;
		if (!(htab = nad_init(io->hp)))
			emess(1,"hp datum file: %s, failed: %s", io->hp,
				pj_strerrno(pj_errno));
	}
	if (io->zone > 0) {
		if (io->zone <= 60) { /* UTM zone */
			io->nprojc = 2; /* no other options allowed */
			io->projc[0] = "proj=utm";
			sprintf(tmp, "zone=%d", io->zone);
			io->projc[1] = io->t83 ? "ellps=GRS80" : "ellps=clrk66";
		} else /* SPCS zone */
			sprintf(tmp, "init=nad%s:%d", io->t83 ? "83" : "27", io->zone);
		io->projc[io->nprojc++] = tmp;
		io->projc[io->nprojc++] = "no_defs";
		if (!(io->cnv = pj_init(io->nprojc, io->projc)))
			emess(1,pj_strerrno(pj_errno));
		io->ll = 0;
	}
}
	static void
setup() {
	/* check and set zone operations */
	if (input.hp && output.hp)
		emess(1,"both input and output cannot be high precision");
	set_zone(1, &input);
	set_zone(0, &output);
	if (input.cnv && !output.cnv)
		output.ll = 1;
	if (output.cnv && !input.cnv)
		input.ll = 1;
	if (!input.cnv && !output.cnv)
		output.ll = input.ll = 1;
	if (czone) {
		if (!input.hp && !output.hp && input.t83 == output.t83)
			emess(1,"identical datums");
		if (!(ctab = nad_init(czone)))
			emess(1,"datum file: %s, failed: %s", czone, pj_strerrno(pj_errno));
	} else if (input.t83 != output.t83)
		emess(1,"conversion region (-r) not specified");
}
	static void
set_ops(char *s, struct IO_CON *io) {
	char *intag;
	struct TAG_LIST *p;

	for ( ; intag = strtok(s, " ,\t"); s = 0) {
		for (p = ops_list; p->tag; ++p) {
			if (!strncmp(intag, p->tag, strlen(p->tag)))
				break;
		}
		if (!p->tag)
			emess(1,"invalid selection");
		switch (p->sw) {
		case 0:
		case 1:
			s = strchr(intag, '=') + 1;
			io->zone = atoi(s);
			break;
		case 2:
			if (io->zone <= 60)
				emess(1,"spcs zone must be selected");
			io->projc[io->nprojc++] = "units=us-ft";
			break;
		case 3: io->t83 = 0; break;
		case 4: io->t83 = 1; break;
		case 5:
			if (!(intag = strchr(intag, '=')) || *++intag == '\0')
				emess(1,"hp missing name");
			strcpy(io->hp = (char*)malloc(strlen(intag)+1), intag);
			break;
		case 6: io->bin = 1; break;
		case 7: io->rev = 1; break;
		}
	}
}
	static void
process(FILE *fid) {
	char line[MAX_LINE], *s, t, pline[100];
	projUV val;
	double tmp;

	for (;;) {
		if (input.bin)
			fread(&val, sizeof(projUV), 1, fid);
		else if (s = fgets(line, MAX_LINE, fid)) {
			if (*s == tag) {
				fputs(line, stdout);
				continue;
			} else if (input.ll) {
				val.u = dmstor(s, &s);
				val.v = dmstor(s, &s);
			} else {
				val.u = strtod(s, &s);
				val.v = strtod(s, &s);
			}
		}
		if (feof(fid))
			break;
		if (input.rev) {
			tmp = val.u;
			val.u = val.v;
			val.v = tmp;
		}
		/* data in, manupulate */
		if (input.cnv)
			val = pj_inv(val, input.cnv);
		if (input.hp)
			val = nad_cvt(val, 1, htab);
		/* nad conversion */
		if (ctab)
			val = nad_cvt(val, input.t83 ? 1 : 0, ctab);
		if (output.hp)
			val = nad_cvt(val, 0, htab);
		if (output.cnv)
			val = pj_fwd(val, output.cnv);
		/* output data */
		if (output.rev) {
			tmp = val.u;
			val.u = val.v;
			val.v = tmp;
		}
		if (output.bin)
			(void)fwrite(&val, sizeof(projUV), 1, stdout);
		else {
			if (echoin) {
				t = *s;
				*s = '\0';
				(void)fputs(line, stdout);
				(void)putchar('\t');
				*s = t;
			}
			if (val.u == HUGE_VAL)
				(void)fputs(oterr, stdout);
			else if (output.ll)
				if (oform) {
					(void)printf(oform, val.u * RAD_TO_DEG);
					(void)putchar('\t');
					(void)printf(oform, val.v * RAD_TO_DEG);
				} else if (output.rev) {
					(void)fputs(rtodms(pline, val.u, 'N', 'S'), stdout);
					(void)putchar('\t');
					(void)fputs(rtodms(pline, val.v, 'E', 'W'), stdout);
				} else {
					(void)fputs(rtodms(pline, val.u, 'E', 'W'), stdout);
					(void)putchar('\t');
					(void)fputs(rtodms(pline, val.v, 'N', 'S'), stdout);
				}
			else {
				(void)printf(oform ? oform : "%.2f", val.u);
				(void)putchar('\t');
				(void)printf(oform ? oform : "%.2f", val.v);
			}
			if (input.bin)
				putchar('\n');
			else
				(void)fputs(s, stdout);
		}
	}
}
	void
main(int argc, char **argv) {
	char *arg, **eargv = argv, work[MAX_PARGS];
	FILE *fid;
	int eargc = 0, c;

	if (emess_dat.Prog_name = strrchr(*argv,DIR_CHAR))
		++emess_dat.Prog_name;
	else emess_dat.Prog_name = *argv;
	if (argc <= 1 ) {
		(void)fprintf(stderr, usage, pj_release, emess_dat.Prog_name);
		exit (0);
	}
		/* process run line arguments */
	while (--argc > 0) { /* collect run line arguments */
		if(**++argv == '-') for(arg = *argv;;) {
			switch(*++arg) {
			case '\0': /* position of "stdin" */
				if (arg[-1] == '-') eargv[eargc++] = "-";
				break;
			case 'i': /* input control */
			case 'o': /* output control */
				if (--argc <= 0) goto noargument;
				strncpy(work, *++argv, MAX_PARGS);
				set_ops(work, *arg == 'i' ? &input : &output);
				continue;
			case 'r': /* nad27/83 conversion zone */
				if (--argc <= 0) goto noargument;
				czone = *++argv;
				continue;
			case 'E': /* echo ascii input to ascii output */
				echoin = 1;
				continue;
			case 't': /* set col. one char */
				if (arg[1]) tag = *++arg;
				else emess(1,"missing -t col. 1 tag");
				continue;
			case 'W': /* specify seconds precision */
			case 'w': /* -W for constant field width */
				if ((c = arg[1]) != 0 && isdigit(c)) {
					set_rtodms(c - '0', *arg == 'W');
					++arg;
				} else
				    emess(1,"-W argument missing or non-digit");
				continue;
			case 'f': /* alternate output format degrees or xy */
				if (--argc <= 0) goto noargument;
				oform = *++argv;
				continue;
			case 'e': /* error line alternative */
				if (--argc <= 0)
noargument:			   emess(1,"missing argument for -%c",*arg);
				oterr = *++argv;
				continue;
			default:
				emess(1, "invalid option: -%c",*arg);
				break;
			}
			break;
		} else /* assumed to be input file name(s) */
			eargv[eargc++] = *argv;
	}
	if (eargc == 0) /* if no specific files force sysin */
		eargv[eargc++] = "-";
	/* done with parameter and control input */
	setup();
	/* process input file list */
	for ( ; eargc-- ; ++eargv) {
		if (**eargv == '-') {
			fid = stdin;
			emess_dat.File_name = "<stdin>";
		} else {
			if ((fid = fopen(*eargv, "r")) == NULL) {
				emess(-2, *eargv, "input file");
				continue;
			}
			emess_dat.File_name = *eargv;
		}
		emess_dat.File_line = 0;
		/* process file */
		process(fid);
		(void)fclose(fid);
		emess_dat.File_name = 0;
	}
	exit(0); /* normal completion */
}
