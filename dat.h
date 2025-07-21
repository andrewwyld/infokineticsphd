#ifndef _INC_IKAWDAT
#define _INC_IKAWDAT

/*_____________________________________________________________.
                                                         sDat */
class cDat {
public:
	cDat(cDat* n);
	~cDat();
	cDat* end();
	int data;
	int count;
	cDat * next;
};

#endif