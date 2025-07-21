/*______________________________________________________________
                                               I N C L U D E S |
______________________________________________________________*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "node.h"
#include "net.h"
#include "hash.h"


/*______________________________________________________________
                                             F U N C T I O N S |
______________________________________________________________*/



/*_____________________________________________________________.
                                                         main */
int main(int argc, char *argv[]){

	cNet* N;
	int i;
	
	int error = 0;
	
	int matches = 0;
	int oldmatches = 0;
	int smartmatches = 0;
	int oldsmartmatches = 0;
	int iteration = 0;
	int nonuniform0 = 1;
	int nonuniform1 = 1;
	
	/*SEED RNG*/
	srand((unsigned) time(0) );
	
	if( 0== (N = new cNet) ) return -1;
	if (N->mode(IKAW_SCORE_ONLY)) N->displayMode();
	N->init(argc, argv);
	if (N->mode(IKAW_SCORE_ONLY)) N->displayMode();

	/*DO SOMETHING EXCITING!*/

	N->run();
	if (N->mode(IKAW_SCORE_ONLY)) N->displayMode();
	if(! N->mode(IKAW_SCORE_ONLY) )
		N->cumlNetDump();
	if( N->mode(IKAW_SCORE_ONLY))
		std::cout << N->fp << "\t" << N->score << std::endl;
	if( N->mode(IKAW_DDFN))
 		N->distDistDisplay();
	if( N->mode(IKAW_DEGD))
		N->degDistDisplay();

	delete N;

	return error;
}



//EOF
