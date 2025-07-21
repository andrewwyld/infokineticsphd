#ifndef __IKAWDAT
#define __IKAWDAT

/*_____________________________________________________________.
                                                         sDat */
typedef struct s_dat {
	int data;
	int count;
	struct s_dat * next;
} sDat;

sDat * datEnd (sDat * n);
int datKill(sDat * n);
int datExt(sDat* n);

#endif