/*_____________________________________________________________.
                                                               |
                                                 N E T . C P P |
_(c)_2006_Andrew_Wyld_________________________________________*/

#ifndef _INC_IKAW_NET
#include "net.h"
#endif

/*_____________________________________________________________.
                                                cNet Functions*/

bool cNet::mode(int comp){
	if(0!=(comp&vMode)) return true;
	return false;
}

int cNet::reset(){
	int error=0;

	score=0;
	disruptClusterSize=0;
	disruptEdgesKilled=0;
	disruptRedundancy=0;
	spare=0;
	time=0;

	if(vertices!=0) delete [] vertices;
	vertices=0;
	//initialize vertex array
	if(0!=(error=vertexArray())){
		return error;
	}

	vertices->setMode(vMode);

	return 0;
}

void cNet::unflag(){
	for(int i=0; i<size; ++i)
		vertices[i].flag=0;
}

inline void cNet::zeroinit(){
	_1_N2=0;
	ccfthash=0;
	degdhash=0;
	ddfnhash=0;
	dumpInterval=0;
	dumpWait=0;
	edge0=0;
	edge1=0;
	f1_m_s=0;
	fm=0;
	fm_s=0;
	fp=0;
	fminp=0;
	fmaxp=0;
	fs=0;
	hash=0;
	hashIndex=0;
	ip=0;
	iminp=0;
	imaxp=0;
	m=0;
	m_s=0;
	numNod=0;
	outStream=stdout;
	printsize=IKAW_PRINTSIZE;
	psteps=0;
	runtime=0;
	s=0;
	score=0;
	disruptClusterSize=0;
	disruptEdgesKilled=0;
	disruptRedundancy=0;
	size=0;
	spare=0;
	time=0;
	verbose=0;
	vertex0=0;
	vertex1=0;
	vertices=0;
	vMode=0;
}

cNet::cNet(){
	zeroinit();
}

/*_____________________________________________________________.
                                                         init */
int cNet::init(int argc, char* argv[]){

	int temp=0, error=0;

	/*set up default cNet struct*/
	
	time = 0;
	size = IKAW_NETSIZE;
	printsize = IKAW_PRINTSIZE;
	fp = IKAW_NETP;
	ip = (int) ( fp * DRAND_MAX );
	m = IKAW_NETM;
	s = IKAW_NETS;
	verbose = 0;
	runtime = 4096;
	dumpInterval = 256;
	dumpWait = 0;
	outStream = stdout;


if( (0!=(error=parse(argc, argv))) ) return error;

	//set up helpful constants
	_1_N2 = 1/(double)size;
	_1_N2 *= _1_N2;
	m_s = m - s;
	fm = (double) m;
	fs = (double) s;
	fm_s = (double) m_s;
	f1_m_s = 1 / m_s;

	//initialize vertex array
	if(0!=(error=vertexArray())){
		return error;
	}

	vertices->setMode(vMode);

	if(0==(hash=new cDoubleHash)) return -1;
	if(0==(ddfnhash=new cHash<cDoubleHash*>)) return -1;
	if(0==(ccfthash=new cDoubleHash)) return -1;
	if(0==(pchash=new cHash<cDoubleHash*>)) return -1;

	return 0;
}

/*_____________________________________________________________.
                                                  vertexArray */
int cNet::vertexArray(){
	int error=0;

//	fprintf(stderr, "cVertexArray, calloc(%x, %x)\n", sizeof(cVertex), N->size);
	if (0==(vertices = new cVertex[size]) ){ //if it fails
		fprintf(stderr, "Out of memory initializing vertex array\n");
        return -1;
	}

	error=vertices[0].init(0, m, s, fp);
	std::cerr << "vertices[0].init(0, " << m << ", " << s << ", " << fp << ");\n";
	if(0!=error){
//	if (0!=(error=vertices[0].init(0, m, s, fp) )){
		fprintf(stderr, "do nothing\n");
		delete [] vertices;
		vertices=0;
		return error;
	}
	for(int i=1; i<size; ++i){
		if (0!=(error=vertices[i].init(i))){
			delete [] vertices;
			vertices=0;
			return error;
		}
	}

	return 0;
}

/*_____________________________________________________________.
                                                        ~cNet */
cNet::~cNet(){
	if(vertices!=0) delete [] vertices;
	if(hash!=0) delete hash;
	if(ddfnhash!=0) delete ddfnhash;
	if(ccfthash!=0) delete ccfthash;
	return;
}	

/*_____________________________________________________________.
                                                        parse */
int cNet::parse(int argc, char* argv[]){
	int error=0;
	int i = 1;
	while (argc - i) {

		/*single-element args.*/
		if ( !stricmp(argv[i], "-?") || !stricmp(argv[i], "-help") ){
			printf("-?\n-help\n\tprints this message\n");
			printf("-p <double>\n\tsets the probability of connection\n");
			printf("-m <int>\n\tsets the vector depth\n");
			printf("-s <int>\n\tsets the vector spread\n");
			printf("-size <int>\n\tsets the network size\n");
			printf("-depth\n\tsets the data array depth\n");
			printf("-verbose\n\tsets verbosity\n");
			printf("-runtime <int>\n\tsets run duration (cycles)\n");
			printf("-dumpInterval <int>\n\tsets result output interval\n");
			printf("-dumpWait\n\tsets result output wait time\n");
			++i;
			return -1;
		}

		/* check for ddfn */
		if ( !stricmp(argv[i], "-ddfn") ){
			vMode |= IKAW_DDFN;
			++i;
			continue;
		}

		/* check for ccft */
		if ( !stricmp(argv[i], "-ccft") ){
			vMode |= IKAW_CCFT;
			++i;
			continue;
		}

		/* check for degd */
		if ( !stricmp(argv[i], "-degd") ){
			vMode |= IKAW_DEGD;
			++i;
			continue;
		}

		/* check for multi */
		if ( !stricmp(argv[i], "-multi") ){
			vMode |= IKAW_ALLOW_MULTIGRAPHS;
			++i;
			continue;
		}

		/* check for edgelim */
		if ( !stricmp(argv[i], "-edgelim") ){
			vMode |= IKAW_LIMIT_EDGES;
			++i;
			continue;
		}

		/* check for edgelim */
		if ( !stricmp(argv[i], "-destrand") ){
			vMode |= IKAW_RANDOM_DESTRUCTION;
			++i;
			continue;
		}

		if ( !stricmp(argv[i], "-destlim") ){
			vMode |= IKAW_LIMIT_DESTRUCTION;
			++i;
			continue;
		}

		if ( !stricmp(argv[i], "-scoreonly") ){
			vMode |= IKAW_SCORE_ONLY;
			++i;
			continue;
		}

		if ( !stricmp(argv[i], "-bivar_ms") ){
			vMode |= IKAW_BIVAR_MS;
			++i;
			continue;
		}

		if ( !stricmp(argv[i], "-bivar_pc") ){
			vMode |= IKAW_BIVAR_PC;
			++i;
			continue;
		}

		if ( !stricmp(argv[i], "-bivar_ps") ){
			vMode |= IKAW_BIVAR_PS;
			++i;
			continue;
		}

		if ( !stricmp(argv[i], "-progression") ){
			vMode |= IKAW_PROGRESSION;
			++i;
			continue;
		}

		if (argc - i - 1) {
			/* there are at least two command-line args. */

			/* check for printsize */
			if ( !stricmp(argv[i], "-printsize") ){
				if( !(printsize = atof(argv[++i])) ){
					fprintf(stderr, "Specify a greater print output size (in points).\n");
					error = 1;
				}
				++i;
				continue;
			}

			/* check for p */
			if ( !stricmp(argv[i], "-p") ){
				if( !(fp = atof(argv[++i])) ){
					fprintf(stderr, "Specify a greater disconnection probability.\n");
					error = 1;
				}
				ip = (int) ( fp * DRAND_MAX );
				++i;
				continue;
			}
			
			/* check for minp */
			if ( !stricmp(argv[i], "-minp") ){
				if( !(fminp = atof(argv[++i])) ){
					fprintf(stderr, "Specify a greater minimum disconnection probability.\n");
					error = 1;
				}
				iminp = (int) ( fminp * DRAND_MAX );
				++i;
				continue;
			}

			/* check for maxp */
			if ( !stricmp(argv[i], "-maxp") ){
				if( !(fmaxp = atof(argv[++i])) ){
					fprintf(stderr, "Specify a greater maximum disconnection probability.\n");
					error = 1;
				}
				imaxp = (int) ( fmaxp * DRAND_MAX );
				++i;
				continue;
			}

			/* check for psteps */
			if ( !stricmp(argv[i], "-psteps") ){
				if( !(psteps = atoi(argv[++i])) ){
					fprintf(stderr, "Specify a nonzero number of steps for probability range.\n");
					error = 1;
				}
				++i;
				continue;
			}

			/* check for m */
			if ( !stricmp(argv[i], "-m") ){
				m = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for minm */
			if ( !stricmp(argv[i], "-minm") ){
				minm = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for maxm */
			if ( !stricmp(argv[i], "-maxm") ){
				maxm = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for s */
			if ( !stricmp(argv[i], "-s") ){
				s = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for mins */
			if ( !stricmp(argv[i], "-mins") ){
				mins = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for maxs */
			if ( !stricmp(argv[i], "-maxs") ){
				maxs = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for size */
			if ( !stricmp(argv[i], "-size") ){
				size = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for repeat */
			if ( !stricmp(argv[i], "-repeat") ){
				repeat = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for verbose */
			if ( !stricmp(argv[i], "-verbose") ){
				verbose = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for runtime */
			if ( !stricmp(argv[i], "-runtime") ){
				runtime = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for dumpInterval */
			if ( !stricmp(argv[i], "-dumpInterval") ){
				dumpInterval = atoi(argv[++i]);
				++i;
				continue;
			}

			/* check for dumpWait */
			if ( !stricmp(argv[i], "-dumpWait") ){
				dumpWait = atoi(argv[++i]);
				++i;
				continue;
			}

		}

		fprintf(stderr, "Unrecognized argument: %s\n", argv[i]);
		++i;

	}
	return error;
}

/*_____________________________________________________________.
                                                    cumlStore */
int cNet::cumlStore(){
	int i=0, error=0;
	int value=0;
	int total=0;
	cDoubleHash* temp = 0;
	++hashIndex;

	unflag();
	for(i=0; i<size; ++i){
		if(value = vertices[i].clusterSize(1) ){
			if(0!=(error=hash->addData(value, 1))){
				if(value>256) std::cerr << "big\n";
				return error;
			}
		}
	}
	unflag();

//	printf("\n");
	return 0;

}


/*_____________________________________________________________.
                                                  cumlNetDump */
/* */
int cNet::cumlNetDump(){
	int i = 0;
	double I = 0;
	int value = 0;
	cDoubleHash* temp = hash;
	temp->normalize();

	temp->display();
	
	temp=hash->logbin();

	temp->display();

	delete temp;

#ifdef __IKAW_EDGEDIST

//print overall edge/cluster distributions
	printf("\n\n");
	temp = hash;
	while(temp){
		printf("%d\t%d\n", temp->index, temp->edgedist[0]);
		temp = (cDoubleHash*)temp->next;
	}

//print distributions by edge strength
	printf("\n\n");
	temp = N->hash;
	while(temp){
		for (i=0; i<N->s + 1; ++i){
			I = (double)i/N->fs;
			printf("%d\t%f\t%d\n", temp->index, I, temp->edgedist[i]);
		}
		temp = (cDoubleHash*)temp->next;
	}

#endif

//	printf("\n");
	return 0;
}
/* */

/*_____________________________________________________________.
                                                      netDump */
int cNet::dump(int iteration){
	int i = 0;
	int value = 0, error=0;
	cDoubleHash* hash = 0;
	cDoubleHash* temp = 0;

	if(0==(hash=new cDoubleHash)) return -1;

//	printf("dumping net\nindex       \tvalue\n");
	for(i=0; i<size; ++i){
		if(value = vertices[i].clusterSize(1) ){
			if(0!=(error=hash->addData(i, 1))) return error;
		}
	}

	for(i=0; i<size; ++i)
		vertices[i].flagSet(0);

/*	temp = hash;
	while(temp){
		printf("%d\t%d\t%d\n", iteration, temp->index, temp->data);
		temp = temp->next;
	}
/* */
//	printf("\n");
	delete hash;
	hash=0;
	return 0;

}

/*_____________________________________________________________.
                                                       choose */
int cNet::choose(){

	int n=0;
	int index0=0;
	int index1=0;
	int error=0;
	int jointrue=0;
	int temp=0;
	cEdge* edge=0;
	cDoubleHash* dhash=0;
	cHash<cDoubleHash*>* ddhash=0;

//	fprintf(stderr,"choosing\n");

	unflag();

	index0 = basRnd(size);
	
	vertex0 = &(vertices[index0]);
	if (vertex0->doCommunicate() ){

		vertex1=vertex0->getNeighbour();
		if(verbose) fprintf(stderr, "%12f:   comm 0x%8x to 0x%8x\n", time, index0, vertex1->index);
		vertex0->mergeData(vertex1);
		//if communication results in completion, farm to smash function
		if (vertex0->disrupt()){

			if(mode(IKAW_BIVAR_PC)){
				currentHash->addData(vertex0->disruptClusterSize, 1.0);
				idxHash->addData(vertex0->disruptClusterSize, 1.0);
//				std::cerr << time << "\t";
			}
		
/*			if(! mode(IKAW_SCORE_ONLY) )
				fprintf(outStream,
					"%12.0f,\t%12d,\t%12d,\t%12d\n",
					time, vertex0->disruptClusterSize,
					vertex0->disruptEdgesKilled, vertex0->disruptRedundancy);*/
			vertex0->reset();
			score++;
		}
		if (vertex1->disrupt()){
			if(mode(IKAW_BIVAR_PC)){
				currentHash->addData(vertex1->disruptClusterSize, 1.0);
				idxHash->addData(vertex1->disruptClusterSize, 1.0);
//				std::cerr << time << "\t";
			}

/*			if(! mode(IKAW_SCORE_ONLY) )
				fprintf(outStream,
					"%12.0f,\t%12d,\t%12d,\t%12d\n",
					time, vertex1->disruptClusterSize,
					vertex1->disruptEdgesKilled, vertex1->disruptRedundancy);*/
			vertex1->reset();
			score++;
		}
	}
	else{
		//connect
		//choose second vertex for connection
		index1 = basRnd(size);
		vertex1 = &(vertices[index1]);
		// if no multigraphs, check vertices not already connected.
		if( mode(IKAW_ALLOW_MULTIGRAPHS) )
			if(vertex0->connected(vertex1)) return 0;
		
		error=vertex0->condJoin(vertex1);
	}
	++(time);
	return error;
}

/*_____________________________________________________________.
                                                   distDistFn */
int cNet::distDistFn(){

	int i=0, j=0, k=0, t=0, n=0, error=0;
	int label=1;
//	int** adjacency=0;
	cEdge* edge=0;
	cHash<cDoubleHash*>* supertemp=0;
	cDoubleHash* hash=0;
	cDoubleHash* temp=0;

	//if first call, allocate memory to superhash
	if(!ddfnhash){
		ddfnhash=new cHash<cDoubleHash*>;
	}
	
//do something with addData
	for(i=k; i<size; ++i){
		if(1==(n=vertices[i].clusterSize(0))) continue;
		vertices[i].pathCount(temp, ddfnhash->data);
	}
	return 0;

}


/*_____________________________________________________________.
                                                    degDistFn */
int cNet::degDistFn(){

	int i=0, n=0, error=0;
	cHash<cDoubleHash*>* supertemp=0;
	cDoubleHash* temp=0;

	//if first call, allocate memory to superhash
	if(!degdhash){
		degdhash=new cHash<cDoubleHash*>;
	}	

	for(i=0; i<size; ++i){
		if(1==(n=vertices[i].clusterSize(0))) continue;
		if(0==(temp=(degdhash->getData(n)))) return -1;
		temp->addData(0, 1);
		if(temp->addData(vertices[i].degree, 1)) return -1;
	}
	return 0;

}


/*_____________________________________________________________.
                                              distDistDisplay */
// logbins superhashed data for distance distrib fn
int cNet::distDistDisplay(){
	int i=0, j=0, k=0, t=0, n=0, error=0;
	int label=1;
	cEdge* edge=0;
	cHash<cDoubleHash*>* supertemp=0;
	cDoubleHash* hashtemp=0;
	cDoubleHash* hashnorm=0;
	cHash<cDoubleHash*>* displayloghash=0;
	cHash<cDoubleHash*>* temparr=0;
	cDoubleHash* hash=0;
	cDoubleHash* loghash=0;
	cDoubleHash* temp=0;

//	superSort(displayhash, 0);

//	displayloghash=ddfnhash->logbin();
	supertemp=ddfnhash;
	fprintf(outStream, "\nDISTANCE DISTRIBUTION FUNCTION\n");
	while(supertemp){
		fprintf(outStream, "\n");
		hashnorm = supertemp->data;
		if(0==hashnorm){
			supertemp = supertemp->next;
			continue;
		}
		hashtemp = hashnorm->next;
		while(hashtemp){
			fprintf(outStream, "%d\t%d\t%.12f\n", supertemp->index, hashtemp->index, hashtemp->data/hashnorm->data);
			hashtemp = hashtemp->next;
		}
		supertemp = supertemp->next;
	}
    
	return 0;
}


/*_____________________________________________________________.
                                               degDistDisplay */
// logbins superhashed data for distance distrib fn
int cNet::degDistDisplay(){
	int i=0, j=0, k=0, t=0, n=0, error=0;
	int label=1;
	cEdge* edge=0;
	cHash<cDoubleHash*>* supertemp=0;
	cDoubleHash* hashtemp=0;
	cDoubleHash* hashnorm=0;
	cHash<cDoubleHash*>* displayloghash=0;
	cHash<cDoubleHash*>* temparr=0;
	cDoubleHash* hash=0;
	cDoubleHash* loghash=0;
	cDoubleHash* temp=0;

//	superSort(displayhash, 0);

//	displayloghash=ddfnhash->logbin();
	supertemp=degdhash;
	fprintf(outStream, "\nDEGREE DISTRIBUTION\n");
	while(supertemp){
		fprintf(outStream, "\n");
		hashnorm = supertemp->data;
		if(0==hashnorm){
			supertemp = supertemp->next;
			continue;
		}
		hashtemp = hashnorm->next;
		while(hashtemp){
			fprintf(outStream, "%d\t%d\t%.12f\n", supertemp->index, hashtemp->index, hashtemp->data/hashnorm->data);
			hashtemp = hashtemp->next;
		}
		supertemp = supertemp->next;
	}
    
	return 0;
}

/*_____________________________________________________________.
                                                netClustCoeff */
// clustering coefficient for a whole network
int cNet::clustCoeff(){
	
	int lSize;
	double coeff;

	for (int i=0; i<size; ++i){
		lSize=vertices[i].clusterSize(0);
		coeff=vertices[i].clustCoeff();
		ccfthash->addData(lSize, coeff);
	}

	return 0;
}

int cNet::runas_bivar_ms(){
	int error=0;
	static int min_maxs_m;
	bivariate<doubledata>* output=0;
	doubledata ddtemp(0, runtime);
	double maxscore=0;

	//check all vars defined
	if(minm&&maxm&&mins&&maxs){

		//allocate bivariate output array
		if(!(output=new bivariate<doubledata>(maxm-minm+1, maxs-mins+1))) return -1;
	
		//set Y index values
		for(s=mins; s<=maxs; ++s) output->setYidx(s-mins, s);

		//set primary range
		for(m=minm; m<=maxm; ++m){
			min_maxs_m=(maxs>m)?m:maxs;

			//set X index values
			output->setXidx(m-minm, m);
			
			//set secondary range
			for(s=mins; ((s<=min_maxs_m)&&(s<m)); ++s){

				reset();
			
				std::cerr << "m " << m << "s " << s << "\n";
				//perform run
				for (int i=0; i<(dumpWait); ++i){
					if( error = choose() ) return error;

					}
				score=0;
				for (int i=0; i<(runtime); ++i){
					if( error = choose() ) return error;

					}
				std::cerr << score << std::endl;
				ddtemp=score;
				maxscore=(score>maxscore)?score:maxscore;
				//run complete
				output->populate(m-minm, s-mins, ddtemp);
				}
			//secondary range complete
			}
		//primary range complete
		output->setnormfactor((double)runtime);
		output->plot(printsize);
		return 0;
		}
	else{
		std::cerr << "Argument -bivar_ms used improperly.\n";
		std::cerr << "The following arguments must be specified for bivariate m/s output:\n";
		std::cerr << "-minm [nonzero integer];\n";
		std::cerr << "-maxm [nonzero integer];\n";
		std::cerr << "-mins [nonzero integer];\n";
		std::cerr << "-maxs [nonzero integer];\n";
		std::cerr << "Ranges of values of m and s are expected for this plot.\n";
		return -1;
	}
}


int cNet::runas_bivar_ps(){
	int error=0;
	int duration=(runtime-dumpWait)/dumpInterval;
	bivariate<doubledata>* outputmatrix = new bivariate<doubledata>(psteps+1, duration+1);
	static double fpgap= fmaxp-fminp;
	//check minimum start conditions
	if(iminp&&imaxp&&psteps&&(fpgap!=0.0)){

		outputmatrix->null();
		if(repeat==0) repeat=1;
		
		//cycle probabilities
		for(int j=0; j<=psteps; ++j){
			// pass this to some kind of static var
			fp = fminp + ( (fpgap*j) / psteps );
			outputmatrix->setXidx(j, fp);
			ip = (int) ( fp * DRAND_MAX );

			for(int rpt=0; rpt<repeat; rpt++){
				reset();
				vertices->setconsts(this);
				std::cerr << "Probability " << fp << std::endl;
				for (int i=0; i<(dumpWait); ++i){
					if( error = choose() ) return error;
					}
				score=0;
				for (int i=0; i<duration; ++i){
					for(int k=0; k<dumpInterval; ++k){
				
						if( error = choose() ) return error;
					}
					outputmatrix->add(j, i, (double)score);
					score=0;
				}
			}
		}
		outputmatrix->setnormfactor((double)dumpInterval*(double)repeat);
		outputmatrix->plot(printsize);


		//housekeeping
		delete outputmatrix;

	}
	else{
		std::cerr << "Argument -bivar_ps used improperly.\n";
		std::cerr << "The following arguments must be specified for bivariate p/ progressive output:\n";
		std::cerr << "-minp [0 < double < 1];\n";
		std::cerr << "-maxp [minp < double < 1];\n";
		std::cerr << "-psteps [nonzero integer];\n";
		std::cerr << "A range of values of p, with psteps intervals, is expected for this plot.\n";
		if(!iminp) std::cerr << "-minp not specified\n";
		if(!imaxp) std::cerr << "-maxp not specified\n";
		if(!psteps) std::cerr << "-psteps not specified\n";
		if(fpgap==0.0) std::cerr << "-minp == -maxp\n";
	}
	return 0;
	

}

int cNet::runas_bivar_pc(){
//TODO add try/catch memory stuff
	int error=0;
	int J;
	int end;
	static double fpgap= fmaxp-fminp;
	cDoubleHash** clusterHashes=new cDoubleHash*[psteps+2];
	cDoubleHash** logbinnedClusterHashes=new cDoubleHash*[psteps+2];
	bivariate<doubledata>* outputmatrix;
	double* scores = new double[psteps+2];
	doubledata dtemp;

	//check minimum start conditions
	if(iminp&&imaxp&&psteps&&(fpgap!=0.0)){

		clusterHashes[psteps+1]=idxHash=new cDoubleHash;
		//cycle probabilities
		for(int j=0; j<=psteps; ++j){
			clusterHashes[j]=currentHash=new cDoubleHash;
			// pass this to some kind of static var
			fp = fminp + ( (fpgap*j) / psteps );
			reset();
			vertices->setconsts(this);
			std::cerr << "Probability " << fp << std::endl;
			ip = (int) ( fp * DRAND_MAX );
			vMode^=IKAW_BIVAR_PC;
			for (int i=0; i<(dumpWait); ++i){
				if( error = choose() ) return error;
				}
			score=0;
			vMode^=IKAW_BIVAR_PC;
			for (int i=0; i<(runtime); ++i){
				if( error = choose() ) return error;
			}
			std::cerr << score << std::endl;
			logbinnedClusterHashes[j]=clusterHashes[j]->logbin();
			logbinnedClusterHashes[j]->linscale();
			scores[j]=score;
			score=0;
		}
		logbinnedClusterHashes[psteps+1]=clusterHashes[psteps+1]->logbin();
		logbinnedClusterHashes[psteps+1]->linscale();

		J=logbinnedClusterHashes[psteps+1]->getMaxIdx();

		J=15; //FILTHY HACK

		outputmatrix=new bivariate<doubledata>(psteps+1, J+1);

		for(int j=0; j<J; j++){
			outputmatrix->setYidx(j+1, twoth(j) );
		}

		for(int i=0; i<=psteps; ++i){
//			double xtemp=0;
			outputmatrix->setXidx(i, fminp + ( (fpgap*i) / psteps ) );
			outputmatrix->populate(i, 0, scores[i]);
//			std::cout << "$" << scores[i] << "$\t";
			for(int j=0; j<J; j++){
				dtemp=logbinnedClusterHashes[i]->getData(j, true);
				outputmatrix->populate(i, j+1, dtemp);
//				xtemp+=dtemp.getData();
			}
//			std::cout << "$" << xtemp << "$\n\n" ;
//			xtemp=0;
		}
		delete scores;
		outputmatrix->setnormfactor((double)runtime);
		outputmatrix->plot(printsize);


		//housekeeping
		delete outputmatrix;
		for(int j=0; j<=psteps; ++j){
			delete clusterHashes[j];
			delete logbinnedClusterHashes[j];
		}
		delete [] clusterHashes;
		delete [] logbinnedClusterHashes;

	}
	else{
		std::cerr << "Argument -bivar_pc used improperly.\n";
		std::cerr << "The following arguments must be specified for bivariate p/cluster size output:\n";
		std::cerr << "-minp [0 < double < 1];\n";
		std::cerr << "-maxp [minp < double < 1];\n";
		std::cerr << "-psteps [nonzero integer];\n";
		std::cerr << "A range of values of p, with psteps intervals, is expected for this plot.\n";
		if(!iminp) std::cerr << "-minp not specified\n";
		if(!imaxp) std::cerr << "-maxp not specified\n";
		if(!psteps) std::cerr << "-psteps not specified\n";
		if(fpgap==0.0) std::cerr << "-minp == -maxp\n";
	}
	return 0;
}

int cNet::runas_progression(){
	int error=0;
	int duration=(runtime-dumpWait)/dumpInterval;
	cDoubleHash** clusterSizes;
	bivariate<doubledata>* clusterSizeOutput;
	histogram<doubledata>* successes;

	int maxIdx=0, span=0, temp=0;
	int* output= new int[duration];

	successes = new histogram<doubledata>(duration);
	clusterSizes= new cDoubleHash*[duration];

	for (int i=0; i<(dumpWait); ++i){
		if( error = choose() ) return error;
	}

	for (int i=0; i<duration; ++i){
		for(int j=0; j<dumpInterval; ++j){
	
			if( error = choose() ) return error;
		}

		std::cerr << score << std::endl;
		
		// measure and store cluster size distrib
		cumlStore();
		hash->linscale();
		clusterSizes[i] = hash->logbin();
		clusterSizes[i]->linscale();
//		clusterSizes[i]->linscale();

		temp=clusterSizes[i]->getMaxIdx();
		maxIdx=(temp>maxIdx)?temp:maxIdx;

		std::cerr << hash->getMaxIdx() << "\t" << temp << std::endl;

		// reset cluster size hash
		delete hash;
		hash = new cDoubleHash;

		//store success score
		successes->populate(i, 1.0, score);

//		std::cout << score << std::endl;
		score=0;
	}

	span=twoaboveidx(maxIdx)+2;

	clusterSizeOutput= new bivariate<doubledata>(duration, span);
	for (int i=0; i<duration; ++i){
		for (int j=0; j<span; ++j){

			clusterSizeOutput->setYidx(j, twoth(j) );

			clusterSizeOutput->populate(i, j, clusterSizes[i]->getData(twoth(j)));

		}

	}

	std::cout << maxIdx << std::endl;
	delete clusterSizes;

	successes->setnormfactor(size);
	clusterSizeOutput->setnormfactor(size);
	clusterSizeOutput->plotpreamble(288);
	clusterSizeOutput->plotgraph(288);
	successes->plotgraph(288, 72);
	clusterSizeOutput->plotclose(288);

	delete clusterSizeOutput;
	delete successes;

	return 0;
}

int cNet::runas_normal(){
	int error=0;
	for (int i=0; i<(runtime); ++i){
		if( error = choose() ) return error;
		if( (i>dumpWait) && !(i % dumpInterval) ){
			cumlStore();
			fprintf(stderr, "loop %d\t", i);
			if( mode(IKAW_DDFN))
				distDistFn();
			if( mode(IKAW_DEGD))
				degDistFn();
			if( mode(IKAW_CCFT))
				clustCoeff();
		}
	}
	return 0;
}

int cNet::run(){
	if(mode(IKAW_BIVAR_MS)){
		return runas_bivar_ms();
	}
	else if(mode(IKAW_BIVAR_PC)){
		return runas_bivar_pc();
	}
	else if(mode(IKAW_BIVAR_PS)){
		return runas_bivar_ps();
	}
	else if(mode(IKAW_PROGRESSION)){
		return runas_progression();
	}
	else{
		return runas_normal();
	}
}