/*_____________________________________________________________.
                                                               |
                                               N O D E . C P P |
_(c)_2006_Andrew_Wyld_________________________________________*/

#include "node.h"
#include "net.h"

/*_____________________________________________________________.
                                               static members */

double cEdge::edgeCount;
int cEdge::depth;

int cVertex::disruptClusterSize;
int cVertex::disruptEdgesKilled;
int cVertex::disruptRedundancy;
int cVertex::ip;
double cVertex::fp;
int cVertex::m;
double cVertex::fm;
int cVertex::s;
double cVertex::fs;
int cVertex::m_s;
double cVertex::fm_s;
double cVertex::f1_m;
double cVertex::f1_m_s;
int cVertex::sIndex;
int cVertex::vMode;


/*_____________________________________________________________.
                                              cEdge Functions */

//default constructor
cEdge::cEdge(){
	++edgeCount;
	vDestin=0;
	vReverse=0;
	vSource=0;
	next=0;
	prev=0;
	strength=0;
	weakness=0;
}

//overloaded constructor
cEdge::cEdge(cEdge * nPrev, cVertex* source, cVertex* destin){
	prev = nPrev;
	if(0!=prev){
		next = prev->next;
		prev->next = this;
		if(0!=next) next->prev = this;
	}
	else{
		next=0;
	}
	vDestin=destin;
	vSource=source;
	strength=source->comp(destin);
	weakness=depth-strength;
	vReverse=0;
	++edgeCount;
}

//overloaded constructor
cEdge::cEdge(cEdge* nPrev, cEdge * nReverse){
	prev=nPrev;
	if(0!=prev){
		next = prev->next;
		prev->next=this;
		if(0!=next) next->prev = this;
	}
	else{
		next=0;
	}
	vReverse=nReverse;
	nReverse->vReverse=this;
	vSource=nReverse->vDestin;
	vDestin=nReverse->vSource;
	strength = nReverse->strength;
	weakness = nReverse->weakness;
	++edgeCount;
}


//overloaded constructor
cEdge::cEdge(cVertex *source, cVertex *destin){
	vSource=source;
	vDestin=destin;
	strength=source->comp(destin);
	weakness=depth-strength;
	vReverse=0;
	next=0;
	prev=0;
	++edgeCount;
}

//destructor
cEdge::~cEdge(){
	if(this==vSource->edges){
		vSource->edges=next;
	}
	if(this==vSource->bottom){
		vSource->bottom=prev;
	}
	if(this==vSource->temp){
		vSource->temp=next;
	}
	if(0!=prev){
		prev->next=next;
	}
	if(0!=next){
		next->prev=prev;
	}
#ifndef IKAW_DIRECTED
	if(0!=vReverse){
		vReverse->vReverse=0;
		delete vReverse;
	}
#endif
	if(0!=vSource){
		vSource->degree--;
	}
	else{
		std::cout << "edge without source being deleted\n";
	}
/*	if(0!=right){
		right->left=left;
	}
	if(0!=left){
		left->right=right;
	}
*/	--edgeCount;
}

void cEdge::setDepth(int dDepth){
	depth=dDepth;
	return;
}

int cEdge::getDepth(){
	return depth;
}

//function for pairing edges
void cEdge::match(cEdge * reverse){
	vReverse=reverse;
	reverse->vReverse=this;
	return;
}

void cEdge::append(cEdge * appendix){
	cEdge * temp=this;
	while(0!=(temp->next)) temp=temp->next;
	temp->next = appendix;
	appendix->prev = temp;
	return;
}

cEdge * cEdge::findMatch(cVertex * comp){
	cEdge* temp=this;
	while(0!=temp){
		if((temp->vDestin)==comp) return temp;
		temp=temp->next;
	}
	return 0;
}

cVertex* cEdge::getVertex(int i){
	cEdge* temp=this;
	for(int j=0;j<i;++j){
		temp=temp->next;
		if(0==temp) return 0;
	}
	return temp->vDestin;
}

void cVertex::mergeData(cVertex* that){
	for(int i=0; i<m; ++i){
		if(that==0) std::cout << "ERROR vertex being merged with null\t" << this->index << std::endl;
		this->data[i] |= that->data[i];
		that->data[i]  = this->data[i];
	}
	return;
}


/*_____________________________________________________________.
                                            cVertex Functions */
cVertex::cVertex(int depth, int distrib){
	m=depth;
	fm=(double)m;
	s=distrib;
	fs=(double)s;

	m_s=m-s;
	fm_s=(double)m_s;
	if(0!=m) f1_m=1.0/fm;
	if(0!=m_s) f1_m_s=1.0/fm_s;

	bottom=0;
	data=0;
	degree=0;
	edges=0;
	flag=0;
	match=0;
	index=0;
	sIndex=1;
	match=0;
	purpose=0;
	skill=0;
}

cVertex::cVertex(int nIndex){
	bottom=0;
	data=0;
	degree=0;
	edges=0;
	flag=0;
	match=0;
	index=nIndex;
	sIndex=index;
	match=0;
	purpose=0;
	skill=0;
}

cVertex::cVertex(){
	bottom=0;
	data=0;
	degree=0;
	edges=0;
	flag=0;
	match=0;
	index=sIndex++;
	match=0;
	purpose=0;
	skill=0;
}

cVertex::~cVertex(){
//	bool temp = (edges?true:false);
//	std::cerr << index;
	if(0!=data) delete [] data;
//	std::cerr << " d";
	if(0!=purpose) delete [] purpose;
//	std::cerr << "p";
	if(0!=skill) delete [] skill;
//	std::cerr << "s ";
	while(edges!=0){
//		std::cerr << ".";
		delete edges;
	}
//	if (temp) std::cerr << std::endl;
}

int cVertex::allocate(){
//	std::cerr << "al:" << index << "\t";
	if(0==(data=new int[m])) return -1;
	if(0==(purpose=new int[m])){
		delete [] data;
		data=0;
		return -1;
	}
	if(0==(skill=new int[m])){
		delete [] data;
		data=0;
		delete [] purpose;
		purpose=0;
		return -1;
	}
	memset((void*)data, 0, m*sizeof(int));
	memset((void*)purpose, 0, m*sizeof(int));
	memset((void*)skill, 0, m*sizeof(int));
	return 0;
}

void cVertex::setconsts(cNet* N){
	m=N->m;
	fm=(double)m;
	s=N->s;
	fs=(double)s;
	fp=N->fp;
	ip = (int) ( fp * DRAND_MAX );

	m_s=m-s;
	fm_s=(double)m_s;
	f1_m_s=1.0/m_s;
}

int cVertex::init(){
	int error=0;
	if(0==data)	error=this->allocate();
    if(0!=error) return error;
	this->set();
	return 0;
	index=++sIndex;
}

int cVertex::init(int i){
	int error=0;
	if(0==data)	error=this->allocate();
    if(0!=error) return error;
	index=i;
	this->set();
	return 0;
	index=++sIndex;
}

int cVertex::init(int i, int nM, int nS, double nP){
	m=nM;
	fm=(double)m;
	s=nS;
	fs=(double)s;
	fp=nP;
	ip = (int) ( fp * DRAND_MAX );

	m_s=m-s;
	fm_s=(double)m_s;
	f1_m_s=1.0/m_s;

	sIndex=i;
	index=i;

	int error=0;
	if(0==data)	error=this->allocate();
    if(0!=error) return error;
	this->set();
	return 0;
}

void cVertex::rndFnInit(){
	srand((unsigned) time(0) );
	return;
}

int cVertex::rndFn(){
	double temp=(double)rand()/DRAND_MAX;
	return (int)(temp*fm);
}

//NEED AN INITIALIZER

void cVertex::setDepth(int dDepth){
	cEdge * temp=0;
	m=dDepth;
	fm=(double)m;
	m_s=m-s;
	fm_s=(double)m_s;
	if(0.0!=fm_s) f1_m_s=1.0/fm_s;
	temp->setDepth(m);
    return;
}

int cVertex::getDepth(){
	return m;
}

int cVertex::comp(cVertex * destin){
	int ttl=0;
	for(int i=0; i<m; ++i)
		ttl+=destin->purpose[i]*purpose[i];
	return ttl;
}

void cVertex::flagSet(int fFlag){
	flag=fFlag;
	return;
}

int cVertex::flagSub(int value){
	cEdge * temp=edges;
	int ttl=1;
	if(value!=flag){
		flag=value;
		while(temp){
			ttl += temp->vDestin->flagSub(value);
			temp = temp->getNext();
		}
	}
	return ttl;
}


//join vertices
int cVertex::join(cVertex * destin){
	if(destin==this) return 0;
	if(!mode(IKAW_ALLOW_MULTIGRAPHS)) if(connected(destin)) return 0;
	cEdge * temp0 = new cEdge(this->bottom, this, destin);
	if(0==temp0) return -1;
#ifndef IKAW_DIRECTED
	cEdge * temp1 = new cEdge(destin->bottom, temp0);
	if(0==temp1){
		delete temp0;
		return -1;
	}
	destin->bottom=temp1;
	if(destin->edges==0) destin->edges=temp1;
#endif
	bottom=temp0;
	if(edges==0) edges=temp0;
	++degree;
	++destin->degree;
	return 0;
}

int cVertex::condJoin(cVertex * destin){
	double str=this->comp(destin);
	if(rndFn()<str) return join(destin);
	return 0;
}

void cVertex::disjoin(cVertex * destin){
	cEdge * temp0 = edges;
	while(temp0){
		if(temp0->vDestin==destin){
			delete temp0;
			return;
		}
		temp0=temp0->getNext();
	}
	return;
}

int cVertex::disjoin(){
	cEdge* temp0 = edges;
	cEdge* temp1;
	int count=0;

	while(0!=temp0){
		++count;
		temp1 = temp0->getNext();
		delete temp0;
		temp0 = temp1;
	}
	degree=0;
	return count;
}

/*int cVertex::disjoin(){
	int count=0;
	while(edges){
		count++;
		delete edges;
	}
	return count;
}*/

int cVertex::disjoin(double strength){
	bool total=true;
	cEdge* temp0 = edges;
	cEdge* temp1;
	int count=0;

	while(0!=temp0){
		temp1 = temp0->getNext();
		if(temp0->getStrength()<strength){
			++count;
			delete temp0;
		}
		else total=false;
		temp0 = temp1;
	}
	if(total) edges=0;
	return count;
}

//WILL NEED FIXOLOGY
int cVertex::disjoin(int depth){
	cEdge* temp0 = edges;
	cEdge* temp1 = edges;
	int count=0;

	if(0!=flag) return 0;

	if(depth==0){
		flag = 1;
		while(temp0){
			temp0->vDestin->disjoin(0);
			temp0 = temp0->getNext();
			count += temp1->vDestin->disjoin();
			temp1 = temp0;
			degree=0;
		}
		return count;
	}

	if(depth==1){
		count = this->disjoin();
		return count;
	}

	if(depth>1){
		flag = 1;
		while(temp0){
			count += temp0->vDestin->disjoin(depth-1);
			temp0 = temp0->getNext();
			temp1->vDestin->disjoin();
			temp1 = temp0;
			degree=0;
		}
		return count;
	}

	return 0;
}

int cVertex::disjoin(int depth, double strength){
	cEdge* temp0 = edges;
	int count=0;
	temp=edges;

	if(0!=flag) return 0;

	if(depth==0){
		flag = 1;
		while(temp){
			temp0 = temp;
			count += temp->vDestin->disjoin(0, strength);
			if(temp0==temp) temp=temp->getNext();
		}
		count += disjoin(strength);
		return count;
	}

	if(depth==1){
		count = disjoin(strength);
		return count;
	}

	if(depth>1){
		flag = 1;
		while(temp){
			temp0 = temp;
			count += temp->vDestin->disjoin(0, strength);
			if(temp0==temp) temp=temp->getNext();
		}
		count += disjoin(strength);
		return count;
	}

	return 0;
}


/*_____________________________________________________________.
                                                    propagate */
int cVertex::propagate(int level, int value){
	cEdge * temp = edges;

	//ensure if this node is revisited, nothing happens
	if(0!=flag) return 0;
	flag = 1;

	//assign value to data's levelth int
	data[level] = value;
	
	// propagate to all linked nodes
	while(temp){
		temp->vDestin->propagate(level, value);
		temp = temp->getNext();
	}

	//reset flag
	flag = 0;
	return 0;
}


/*_____________________________________________________________.
                                                  fDotProduct */
double fDotProduct(int m, double one[], double two[]){
	int i;
	double ret = 0;
	for (i = 0; i < m; ++i)
		ret += one[i]*two[i];
	return ret;
}

/*_____________________________________________________________.
                                                  iDotProduct */
int iDotProduct(int m, int one[], int two[]){
	int i;
	int ret = 0;
	for (i = 0; i < m; ++i)
		ret += one[i]*two[i];
	return ret;
}

/*_____________________________________________________________.
                                                          set */
int cVertex::set(){
	int j, temp;
	for(j=0; j < s; ++j){
		do {
			temp = basRnd(m);
		} while(purpose[temp] != 0 );
//		n->vector[temp] = N->unit;
		purpose[temp] = 1;
//		std::cerr << temp << "\t";
	}
	temp = basRnd(m);
	data[temp] = 1;
//	std::cerr << temp << std::endl;

	return 0;
}


/*_____________________________________________________________.
                                                       reset */
int cVertex::reset(){
	int i;
	for(i=0; i < m; ++i){
//		n->vector[i] = 0.0;
		purpose[i] = 0;
		data[i] = 0;
	}
	return this->set();
}

/*_____________________________________________________________.
                                                    connected */
//returns true if two edges are connected
bool cVertex::connected(cVertex* two){
	cEdge* temp=edges;

	while(temp){
		if(temp->vDestin==two){
			return true;
		}
		temp=temp->getNext();
	}
	return false;

}

cVertex* cVertex::getNeighbour(int i){
	if(0==edges) return 0;
	return edges->getVertex(i);
}

cVertex* cVertex::getNeighbour(){
	if(0==edges) return 0;
	int i=basRnd(degree);
	return edges->getVertex(i);
}


/*_____________________________________________________________.
                                                   clustCoeff */
//outputs the cluster coefficient for n
double cVertex::clustCoeff(){

	cEdge * temp0 = edges;
	cEdge * temp1;
	double neighbours = 0;
	double edges = 0;

	while(temp0){
		temp1=temp0->getNext();
		while(temp1){
			if(temp0->vDestin->connected(temp1->vDestin)) edges += 1;
			neighbours += 1;
			temp1=temp1->getNext();
		}
		temp0=temp0->getNext();
	}

	return edges/neighbours;

}

int cVertex::clusterSize(){
	int temp = 1;
	cEdge* nod = edges;
	if(0!=flag) return 0;
	flag=1;
	while(nod){
		temp += nod->vDestin->clusterSize();
		nod = nod->getNext();
	}
	return temp;
}

int cVertex::clusterSize(int nFlag){
	int temp = 1;
	cEdge* nod = edges;
	if(0!=flag) return 0;
	flag=1;
	while(nod){
		temp += nod->vDestin->clusterSize();
		nod = nod->getNext();
	}
	flagSub(nFlag);
	return temp;
}

/*_____________________________________________________________.
                                                      disrupt */
// shake things up a bit
int cVertex::disrupt(){
	int moose = 0;
	double goose = 0;
	int bruce = 0;

	if(-1 == (moose = comp())) return 0;

	disruptClusterSize = clusterSize(0);

	if( (0 == moose) || mode(IKAW_LIMIT_DESTRUCTION) ){
		disruptEdgesKilled = degree;
		disjoin();
		return 1;
	}

	if( mode(IKAW_RANDOM_DESTRUCTION) ){
		goose = (double)basRnd(s) * f1_m;
	}
	else {
		goose = (
			(double) moose * fm * f1_m_s
			);
	}

	disruptRedundancy = moose;
	disruptEdgesKilled  = disjoin(0, goose);
	
	disjoin();
	reset();

	return 1;
}

int cVertex::countFresh(cDoubleHash* hash, cDoubleHash* copyhash, int depth){
	cEdge* temp=edges;
	int count=0;
	int total=0;
	
//current distance:  depth
	if(depth){
//previously visited at greater distance 
		if(flag>depth){
			if(0!=hash->addData(flag, -1)) return -1;
			if(0!=hash->addData(depth, 1)) return -1;
			if(0!=copyhash->addData(flag, -1)) return -1;
			if(0!=copyhash->addData(depth, 1)) return -1;
			flag=depth;
		}
//not previously visited
		if(flag==0){
			if(0!=hash->addData(0, 1)) return -1;
			if(0!=hash->addData(depth, 1)) return -1;
			if(0!=copyhash->addData(0, 1)) return -1;
			if(0!=copyhash->addData(depth, 1)) return -1;
			flag=depth;
		}
		else
//previously visited at smaller distance
			return 0;
    }

	while(temp){
//visit subsequent vertices
		temp->vDestin->countFresh(hash, copyhash, depth+1);
		temp=temp->getNext();
	}
	return 0;

}

/*_____________________________________________________________.
                                                    pathCount */
// counts distance distribution for a given vertex using a
// breadth-first search
int cVertex::pathCount(cDoubleHash* hash, cDoubleHash* copyhash){

	int i=0, j=0, k=0, t=0;
	int count=0;
	int depth=1;
	int error=0;
	
	flagSub(0);

	flag=-1;

	countFresh(hash, copyhash, 0);

//	std::cout << std::endl;
//	flagSub(3);
	flagSub(0);
	return 0;
}

void cVertex::setMode(int comp){
	vMode=comp;
	return;
}

bool cVertex::mode(int comp){
	if(0!=(comp&vMode)) return true;
	return false;
}

/*_____________________________________________________________.
                                                       ratRnd */
//returns true p times out of q.
int ratRnd( int p, int q){
/*
	if (p*2>q) return ratRnd(q-p, q);
	if (rand()<p*(RAND_MAX/q)) return 1;
	return 0;
/*	*/
	return (
		rand() <
		(int) ( (double) p * DRAND_MAX / (double) q )
	);
/*	*/
}

/*_____________________________________________________________.
                                                       fltRnd */
//returns true p times out of q.
bool fltRnd( double p, double q){
/*
	if (p*2>q) return ratRnd(q-p, q);
	if (rand()<p*(RAND_MAX/q)) return 1;
	return 0;
/*	*/
	return (
		( p * DRAND_MAX / q ) > (double)rand()
	);
/*	*/
}

/*_____________________________________________________________.
                                                       basRnd */
//returns a random integer under the bound.
int basRnd(int bound){
/*
	return ( rand()/(RAND_MAX/bound) ) % bound;
/*	*/
	return (
		(int) ( (double) rand() * (double) bound * D1_RAND_MAX ) % bound
		);
/*	*/
}

/*_____________________________________________________________.
                                              communicateTrue */
//permits communication
bool cVertex::doCommunicate(){
//	linear version
	if(degree==0) return false;
	return !fltRnd((1.0 - fp), (double) (degree + 1) * fp );
//	square version
//	return ratRnd(1, (degree+1)*(degree+1));
}

// returns number of redundant entries in complete data vector
int cVertex::comp(){
	int ret = 0;
	int tmp = 0;
	for(int i=0; i<m; ++i){
		if(0 > ( tmp = data[i] - purpose[i] ) ) return -1;
		ret += tmp;
	}
	return ret;
}

/* EOF */