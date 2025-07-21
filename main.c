/*______________________________________________________________
                                               I N C L U D E S |
______________________________________________________________*/

#include "node.h"
#include "dat.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


/*______________________________________________________________
                                                 D E F I N E S |
______________________________________________________________*/

#define IKAWNETSIZE 1024
#define IKAWNETDEPTH 8


/*______________________________________________________________
                                           P R O T O T Y P E S |
______________________________________________________________*/

int joinAndPropagate(sNode* one, sNode* two, int max);
int lookForMatches(sNode* n, sNet* paired, int verbose);
int choose(sNet* n, int verbose);
int sNetInit(sNet* n, int verbose);
int sNetOutputState(sNet* n, FILE* stream);
int sNetOutputStatState(sNet* n, FILE* stream);
int countMatches(sNet* n0, sNet* n1);

int main(int argc, char *argv[]){

	int i, error = 0;
	int matches = 0;
	int oldmatches = 0;
	int smartmatches = 0;
	int oldsmartmatches = 0;
	int iteration = 0;
//	float P;
	sNet* n0, * n1;
	sNode* nodeTemp;
	srand((unsigned) time(0) );
//	fprintf(stdout, "executing\n");

	if(! ( nodeTemp = calloc(IKAWNETSIZE, sizeof(sNode)) ) ) return -1;
//	fprintf(stdout, "made nodes\n");

	if(! (n0 = sNetCreate(IKAWNETSIZE, 8, 0.5)) ){
		free(nodeTemp);
		return -1;
	}
//	fprintf(stdout, "made net 0\n");

	if(! (n1 = sNetCreate(IKAWNETSIZE, 8, 0.5)) ){
		free(nodeTemp);
		sNetDestroy(n0);
		return -1;
	}
//	fprintf(stdout, "made net 1\n");

	sNetInit(n0, 0);
//	fprintf(stdout, "init 0 ");
	sNetInit(n1, 0);
//	fprintf(stdout, "init 1 ");
	for(i = 0; i < IKAWNETSIZE; ++i){
	}
	qsort(n1->nodes, IKAWNETSIZE, sizeof(sNode), _nodeCmp);
	for(i = 0; i < IKAWNETSIZE; ++i){
		n1->nodes[i].data[0] = n0->nodes[n1->nodes[i].index].data[0];
		n1->nodes[i].index = i;
	}
//	fprintf(stdout, "initialized\n");
	
	
	fprintf(stdout, "%8d\t%8d\n", iteration, matches);
	//MAIN LOOP
	for (i = 0; i < 16000000; ++i){

		choose(n0, 0);
		smartmatches += lookForMatches(n0->node0, n1, 1);
		choose(n1, 0);
		smartmatches += lookForMatches(n1->node0, n0, 1);
		matches = countMatches(n0, n1);
		if(matches != oldmatches){
			fprintf(stdout, "norm  %8d\t%8d\n", iteration, matches);
			oldmatches = matches;
		}
		if(smartmatches != oldsmartmatches){
			fprintf(stdout, "smrt  %8d\t%8d\n", iteration, smartmatches);
			oldsmartmatches = smartmatches;
		}
		++iteration;
	}
	fprintf(stdout, "final %d", countMatches(n0, n1));


//	sNetOutputStatState(n0, stdout);

	sNetDestroy(n0);
	sNetDestroy(n1);
	return error;

}




int joinAndPropagate(sNode* one, sNode* two, int max){
	int error = 0;
	int i;
	for(i = 0; i < max; ++i){
		if ( rand() > ( one->subdeg * RAND_MAX / (one->subdeg + two->subdeg) ) )
			propagate(two, i, one->data[i]);
		else
			propagate(one, i, two->data[i]);
	}
	error = join(one, two);
	return error;
}

int lookForMatches(sNode* n, sNet* paired, int verbose){
	sNodule* temp = n->top;
	int counter = 0;
	
	if(n->flag) return 0;
	n->flag = 1;

	while(temp){
		counter += lookForMatches(temp->edge, paired, 0);
		temp = temp->next;
	}

	if(n->data[0] == paired->nodes[n->index].data[0]){
		if(! n->match){
			if(verbose) fprintf(stdout, "found %8d, %8d", n->match, counter);
			++counter;
			n->match = 1;
			if(verbose) fprintf(stdout, "moved %8d, %8d", n->match, counter);
		}
	}
	else {
		if(n->match){
			if(verbose) fprintf(stdout, "lost  %8d, %8d", n->match, counter);
			--counter;
			n->match = 0;
			if(verbose) fprintf(stdout, "moved %8d, %8d", n->match, counter);
		}
	}

	n->flag = 0;
	if(verbose && 2){
		fprintf(stdout, "matching %8d\t", n->index);
		fprintf(stdout, "returning %8d\n", counter);
	}
	return counter;
}

int countMatches(sNet* n0, sNet* n1){
	int i = 0;
	int counter = 0;

	for (i = 0; i< n0->size; ++i){
		if(n0->nodes[i].data[0] == n1->nodes[i].data[0]) ++counter;
	}

	return counter;
}



int choose(sNet* n, int verbose){
	int index0;
	int index1;
	int error=0;
	int jointrue;

	index0 = (int)( (float)n->size * (float)rand() / (float)RAND_MAX ) % n->size;
	index1 = (int)( (float)n->size * (float)rand() / (float)RAND_MAX ) % n->size;
	jointrue = ( rand() > n->ip );

	n->node0 = &(n->nodes[index0]);
	n->node1 = &(n->nodes[index1]);

	if(index0 >= n->size) fprintf(stderr, "your algorithm sucks.");
	if(index1 >= n->size) fprintf(stderr, "your algorithm sucks.");

	if(jointrue){
		if(verbose) fprintf(stdout, "join 0x%8x to 0x%8x\n", index0, index1);
		error = joinAndPropagate(n->node0, n->node1, n->depth);
	}
	else{
		if(verbose) fprintf(stdout, "disjoin 0x%8x\n", index0);
		error = disjoin_all(n->node0);
	}


	if(error){
		fprintf(stderr, "Some kind of error has occurred.\n");
		fprintf(stderr, "choose, %s, index0 == %x, index1 == %x\n",
			jointrue?"join":"disjoin", index0, index1);
	}

	return error;
}

int sNetOutputState(sNet* n, FILE* stream){
	int i, j;
	sNodule* temp1;

	fprintf(stream, "________________________________________________________________\n");
	fprintf(stream, "                                                               |\n");
	fprintf(stream, "pointer\t%x\n", (int)n);
	fprintf(stream, "size\t%d\tdepth\t%d\n", n->size, n->depth);
	for (i=0; i<n->size; ++i){
		fprintf(stream, "\n%d {\n", i);
		for(j=0; j<n->depth; ++j){
			fprintf(stream, "%8x\t%8x\n", j, n->nodes[i].data[j] );
		}
		if(n->nodes[i].top) fprintf(stream, "-->\n");		
		temp1 = n->nodes[i].top;
		while(temp1){
			fprintf(stream, "\t%d\n", temp1->edge->index);
			temp1 = temp1->next;
		}
		fprintf(stream, "}\n\n", i);
	}
	fprintf(stream, "_______________________________________________________________|\n");
	return 0;
}


int sNetOutputStatState(sNet* n, FILE* stream){
	int i, j;
	sDat* out, * point;

	if(!(
		out = calloc(1, sizeof(sDat))
	)) return -1;

	for(j=0; j<n->depth; ++j){
		fprintf(stream, "Layer %d:\n", j);
		for(i=0; i<n->size; ++i){
			if(!(n->nodes[i].flag) ){// check for flag
				point = out;
				while(point){
					// if sDat point is clean, add node data to it
					if( 0 == point->data )
						point->data = n->nodes[i].data[j];
					// if sDat point matches node data, add subdeg to it and return
					if( n->nodes[i].data[j] == point->data ){
						point->count += n->nodes[i].subdeg;
						break;
					}
					else{
						// if there's no next point, make one
						if(!(point->next)){
							if(datExt(point)){
								// return error if this fails
								datKill(out);
								fprintf(stderr, "Insufficient memory:  sNetOutputStatState.\n");
								return -1;
							}
						}
						// head to the next pointer
						point = point->next;
					}
				}
				// flag the subnet
				flagsub(&(n->nodes[i]));
			}
		}
		for(i=0; i<n->size; ++i) unflagsub(&(n->nodes[i]));
		point = out;
		while(point){
			fprintf(stream, "0x%8x\t0x%8x\n", point->data, point->count);
			point = point->next;
		}
		fprintf(stream, "\n");
	}
	return 0;
}

int sNetInit(sNet* n, int verbose){
	int i, j;
	if(verbose) fprintf(stdout, "init: 0x%8x\n", n->nodes);
	for(i=0; i < n->size; ++i){
		if(verbose) fprintf(stdout, "0x%8x, 0x%8x\n", i, &(n->nodes[i]));
		n->nodes[i].subdeg = 1;
		n->nodes[i].index = i;
		for(j=0; j < n->depth; ++j){
// keep the high four bits empty for flagging and so on.
			n->nodes[i].data[j] = rand() & 0x0ffffffff;
			if(verbose) fprintf(stdout, "0x%8x, 0x%8x, 0x%8x\n", i, j, n->nodes[i].data[j]);
		}
	}
	return 0;
}


//EOF
