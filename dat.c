/*_____________________________________________________________.
                                                               |
                                                     D A T . C |
_(c)_2004_Andrew_Wyld_________________________________________*/

#include "dat.h"
#include <stdlib.h>
#include <stdio.h>

/*_____________________________________________________________.
                                                       datEnd */
//seeks last dat item in list
sDat * datEnd (sDat * n){
	sDat * temp = n;
	while(temp->next) temp = temp->next;
	return temp;
}

/*_____________________________________________________________.
                                                       datExc */
//kills this dat, does minor housekeeping
int datKill(sDat * n){
	sDat * temp0, * temp1;
	temp0 = n;

	while(temp0){
		temp1 = temp0->next;
		free(temp0);
		temp0 = temp1;
	}
	return 0;
}


/*_____________________________________________________________.
                                                       datExt */
//adds a nodule to the bottom of the pile
int datExt(sDat* n){
	sDat ** ppTemp;

	ppTemp = & n->next;
	while(*ppTemp)
		ppTemp = &((*ppTemp)->next);
	if(!( (*ppTemp) = calloc(1, sizeof(sDat)) )){
		fprintf(stderr, "datExt failed\n");
		return -1;
	}
	else
		return 0;
}