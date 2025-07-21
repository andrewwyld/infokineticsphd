/*_____________________________________________________________.
                                                               |
                                                 D A T . C P P |
_(c)_2006_Andrew_Wyld_________________________________________*/

#include "dat.h"
#include <stdlib.h>
#include <stdio.h>

/*_____________________________________________________________.
                                                       datEnd */
//seeks last dat item in list
cDat * cDat::end (){
	cDat * temp = this;
	while(temp->next) temp = temp->next;
	return temp;
}

/*_____________________________________________________________.
                                                       datExc */
//kills this dat, does minor housekeeping
cDat::~cDat(){
	if(0!=next) delete next;
}


/*_____________________________________________________________.
                                                       datExt */
//adds an item to the bottom of the pile
cDat::cDat(cDat* n){
	(n->end())->next=this;
}