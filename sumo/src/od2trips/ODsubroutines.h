/**********declares some c-style subroutines************************/

	extern int ODread (string, vector<OD_IN>&, int*, int* );
	extern int ODwrite (string , vector<OD_OUT>& , int);
    extern int Get_rand (unsigned short , int, int*, int*, bool);
	extern void IndexSort (int*, int*, CMPFUN, unsigned int);
