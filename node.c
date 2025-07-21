/*_____________________________________________________________.
                                                               |
                                                   N O D E . C |
_(c)_2004_Andrew_Wyld_________________________________________*/

#include "node.h"
#include <stdlib.h>
#include <stdio.h>

/*_____________________________________________________________.
                                                      findEnd */
//seeks last nodule connected to this nodule
sNodule * findEnd (sNodule * n){
	sNodule * temp = n;
	while(temp->next) temp = temp->next;
	return temp;
}

/*_____________________________________________________________.
                                                    findStart */
//seeks first nodule connected to this nodule
sNodule * findStart (sNodule * n){
	sNodule * temp = n;
	while(temp->prev) temp = temp->prev;
	return temp;
}

/*_____________________________________________________________.
                                                       excise */
//kills this nodule, does housekeeping
int excise(sNodule* n){
	if(n->edge){//reduce degree counter for edge removal
		--(n->owner->degree);
	}

	if(n->prev) { //n was not top node
		if(n->next) { //n was not last node
			n->prev->next = n->next;
			n->next->prev = n->prev;
		}
		else { //n was last node
			n->prev->next = 0;
			n->owner->bottom = n->prev;
		}
	}
	else { //n was top node
		if(n->next) { //n was not last node
			n->owner->top = n->next;
			n->next->prev = 0;
		}
		else { //n was only node!
			n->owner->top = 0;
			n->owner->bottom = 0;
		}
	}
	free(n);
	return 0;
}


/*_____________________________________________________________.
                                                       extend */
//adds a nodule to the bottom of the pile
int extend(sNode* n){
	sNodule ** temp;

	if(!n->bottom)//check for any nodules at all
		temp = &(n->top);//no nodules?  make the first one
	else
		temp = &(n->bottom->next);//nodules?  make a new last one

	if(! ( (*temp) = (sNodule*)calloc(sizeof(sNodule), 1) )	){
		fprintf(stderr, "Out of memory while creating nodule.\n");
		return -1;
	}

	if(!n->bottom){//set bottom to top if top is only nodule
		n->bottom = n->top;
	}
	else{//double-link last nodule
		n->bottom->next->prev = n->bottom;
		n->bottom = n->bottom->next;
	}

	n->bottom->owner = n;

	return 0;
}


//CALLED AS join(one, two)
/*_____________________________________________________________.
                                                join_directed */
//joins a node to another (one-way).
int join_directed(sNode* one, sNode* two){
	if(extend(two)) return -1;
    two->bottom->edge = one;
	return 0;
}

/*_____________________________________________________________.
                                              join_undirected */
//joins a node to another (two-way).
int join_undirected(sNode* one, sNode* two){
	int fartwiener;
	if(join_directed(one, two)) return -1;
	if(join_directed(two, one)) return -1;
	fartwiener = one->subdeg + two->subdeg;
	propagdeg(one, fartwiener);
	return 0;
}


//CALLED AS disjoin(n)
/*_____________________________________________________________.
                                             disjoin_directed */
int disjoin_directed(sNode* n){
	sNodule* temp1 = n->top;
	sNodule* temp2;

	while(temp1){
		temp2 = temp1->next;
		excise(temp1);
		temp1 = temp2;
	}

	return 0;
}

/*_____________________________________________________________.
                                           disjoin_undirected */
int disjoin_undirected(sNode* n){
	sNodule* temp1 = n->top;
	sNodule* temp2, * temp3, * temp4;
	int subdegtmp = n->subdeg;

	while(temp1){
		temp2 = temp1->next;
		temp3 = temp1->edge->top;	//find top nodule of target node
		while(temp3){				//cycle through target node's nodules
			temp4 = temp3->next;
			if(temp3->edge == n){	//if target node's current nodule points here
				excise(temp3);		//kill it!
				subdegtmp--;
			}
			temp3 = temp4;//move on.
		}
		excise(temp1);//kill current nodule
		temp1 = temp2;//and move on
	}

	return 0;
}

/*_____________________________________________________________.
                                                  disjoin_all */
int disjoin_all(sNode* n){
	sNodule* temp1 = n->top;
	sNodule* temp2;

	//ensure if this node is revisited, nothing happens
	if(n->flag) return 0;
	n->flag = 1;

	while(temp1){
		temp2 = temp1->next;
		disjoin_all(temp1->edge);
		excise(temp1);
		temp1 = temp2;
	}

	//reset flag
	n->flag = 0;
	n->subdeg = 1;
	return 0;
}

/*_____________________________________________________________.
                                                      nodeCmp */
int nodeCmp(const sNode* n0, const sNode* n1, int level){
	int result = 0;
	if(n0->data[level] > n1->data[level]) result = 1;
	if(n0->data[level] < n1->data[level]) result = -1;
	return result;
}

/*_____________________________________________________________.
                                                     _nodeCmp */
int _nodeCmp(const void* n0, const void* n1){
	return nodeCmp((sNode*) n0, (sNode*) n1, 0);
}

/*_____________________________________________________________.
                                                      flagsub */
int flagsub(sNode* n){
	sNodule * temp;

	//ensure if this node is revisited, nothing happens
	if(n->flag) return 0;
	n->flag = 1;

	// propagate to all linked nodes
	temp = n->top;
	while(temp){
		flagsub(temp->edge);
		temp = temp->next;
	}

	return 0;
}


/*_____________________________________________________________.
                                                    unflagsub */
int unflagsub(sNode* n){
	sNodule * temp;

	//ensure if this node is revisited, nothing happens
	if(!(n->flag)) return 0;
	n->flag = 0;

	// propagate to all linked nodes
	temp = n->top;
	while(temp){
		unflagsub(temp->edge);
		temp = temp->next;
	}

	return 0;
}


/*_____________________________________________________________.
                                                    propagdeg */
int propagdeg(sNode* n, int subdeg){
	sNodule * temp;

	//ensure if this node is revisited, nothing happens
	if(n->flag) return 0;
	n->flag = 1;

	//assign subdeg
	n->subdeg = subdeg;
	
	// propagate to all linked nodes
	temp = n->top;
	while(temp){
		propagdeg(temp->edge, subdeg);
		temp = temp->next;
	}

	//reset flag
	n->flag = 0;
	return 0;
}


/*_____________________________________________________________.
                                                    propagate */
int propagate(sNode* n, int level, int value){
	sNodule * temp;

	//ensure if this node is revisited, nothing happens
	if(n->flag) return 0;
	n->flag = 1;

	//assign value to data's levelth int
	n->data[level] = value;
	
	// propagate to all linked nodes
	temp = n->top;
	while(temp){
		propagate(temp->edge, level, value);
		temp = temp->next;
	}

	//reset flag
	n->flag = 0;
	return 0;
}


/*_____________________________________________________________.
                                                   sNodeArray */
sNode* sNodeArray(int size, int depth){
	sNode* temp;
	int i, j;

	if (!( //attempt to allocate data array
		temp = calloc(size, sizeof(sNode))
	)){ //if it fails
		fprintf(stderr, "Out of memory initializing node array\n");
		fprintf(stderr, "sNodeArray, calloc(%x, %x)\n", sizeof(sNode), size);
        return 0;
	}

	for(i=0; i<size; ++i){
		if (!(
			temp[i].data = calloc(depth, sizeof(int))
		)){
			fprintf(stderr, "Out of memory initializing data array\n");
			fprintf(stderr, "sNodeArray, calloc(%x, %x), %x", sizeof(sNode), depth, i);
			for(j=0; j<i; ++j)
				fprintf(stderr, "freeing node array %d\n", j);
				free(temp[i].data);
			free(temp);
			return 0;
		}
		temp[i].subdeg = 1;
		temp[i].index = i;
	}
	return temp;
}

/*_____________________________________________________________.
                                                   sNetCreate */
sNet* sNetCreate(int size, int depth, float p){
	sNet* temp;

	//allocate sNet object
	if(! (
		temp = (sNet*)calloc(sizeof(sNet), 1)
	) ){ //failure condition:
		fprintf(stderr, "Out of memory initializing sNet.\n");
		fprintf(stderr, "sNetCreate, calloc(%x, %x)\n", sizeof(sNet), 1);
        return 0;
	}

	if(!( temp->nodes = sNodeArray(size, depth) )){
		free(temp);
		return 0;
	}
	temp->size = size;
	temp->depth = depth;
	temp->fp = p;
	temp->ip = (int) (p * (float)RAND_MAX);
	return temp;

}	

/*_____________________________________________________________.
                                                  sNetDestroy */
int sNetDestroy(sNet* n){

	int i;
	for ( i=0; i<n->size; ++i ){
		if(n->nodes[i].data) free(n->nodes[i].data);
	}
	free(n->nodes);
	free(n);
	return 0;
}	













