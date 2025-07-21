#ifndef _INC_IKAW_NODE
#define _INC_IKAW_NODE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "hash.h"
#include "dlist.h"
#include "ikawdefs.h"

class cEdge;
typedef dlist<cEdge> cEdgeList;
class cVertex;
typedef dlist<cVertex> cVertexList;
class cCluster;

/*_____________________________________________________________.
                                                        cEdge */
//cEdge encodes an edge for a cVertex.
class cEdge {
	//doubly-linked list pointers

	friend class cVertex;

public:

	cEdge();
	cEdge(cEdgeList* list, cVertex* source, cVertex* destin);
	cEdge(cEdge *nPrev, cVertex* source, cVertex* destin);
	cEdge(cVertex* source, cVertex* destin);
	cEdge(cEdge *nPrev, cEdge *nReverse);
	~cEdge();

	void setDepth(int dDepth);
	int getDepth();

	void match(cEdge * reverse);
	cEdge * cEdge::findMatch(cVertex * comp);

	void append(cEdge * appendix);

	cEdge * getPrev(){return prev;}
	cEdge * getNext(){return next;}

	int getStrength(){return strength;}
	int getWeakness(){return weakness;}

	cVertex* getVertex(int i);

//edge
	cVertex * vDestin;
//owner
	cVertex * vSource;
//corresponding return edge for undirected graphs
	cEdge * vReverse;

protected:
	cEdge * prev;
	cEdge * next;
//	cEdge * left;
//	cEdge * right;
	int strength;
	int weakness;

	static double edgeCount;
	static int depth;

};


/*_____________________________________________________________.
                                                      cVertex */
class cVertex {

	friend class cEdge;
	friend class cEdgeList;
	friend class cNet;

public:
	cVertex(int depth, int distrib);
	cVertex(int nIndex);
	cVertex();
	~cVertex();

	void setconsts(cNet* N);
	int init();
	int init(int i);
	int init(int i, int nM, int nS, double nP);

	int rndFn();
	void rndFnInit();
	void setDepth(int dDepth);
	int getDepth();
	int comp(cVertex * destin);
	int comp();

	int join(cVertex * destin);
	int condJoin(cVertex* destin);
	void disjoin(cVertex * destin);
	int disjoin();
	int disjoin(double strength);
	int disjoin(int depth);
	int disjoin(int depth, double strength);
	void flagSet(int fFlag);
	int flagSub(int value);
	int propagate(int level, int value);
	int set();
	int set(int i);
	int reset();
	double clustCoeff();
	bool doCommunicate();
	int countFresh(cDoubleHash* hash, cDoubleHash* copyhash, int depth);
	int pathCount(cDoubleHash* hash, cDoubleHash* copyhash);
	cVertex* getNeighbour(int i);
	cVertex* getNeighbour();


	int disrupt();
	void mergeData(cVertex* that);


	int clusterSize();
	int clusterSize(int nFlag);
//mode marker
	void setMode(int comp);
	bool mode(int comp);
	int getDegree(){return degree;}
//net index
	int index;

//disrupt function data
	static int disruptClusterSize;
	static int disruptEdgesKilled;
	static int disruptRedundancy;
//functions
	bool connected(cVertex* two);
//vertex degree.  can be inferred from cEdge.
	int degree;


private:

	int allocate();

	void halfjoin(cVertex * destin);
//flag.  mostly for checking loops when:
	//propagating
	//destroying networks.
	int flag;

//match flag.  used to check if a vertex had a match
	//but doesn't anymore.
	int match;

//mode
	static int vMode;

//data / purpose arrays
	int * data;
	int * purpose;
	int * skill;

//edge list
	cEdge * edges;
	cEdge * bottom;
	cEdge * temp;

//probability
	static int ip;
	static double fp;

//vector depth
	static int m;
	static double fm;

//vector distribution
	static int s;
	static double fs;

//difference (for speed)
	static int m_s;
	static double fm_s;
	static double f1_m;
	static double f1_m_s;

//static index marker
	static int sIndex;

};

class cCluster{

public:

//	inline const cCluster& operator += (const cCluster& source){
//		mList+=source.mList;
//	}

private:

	cVertexList mList;

};



int vertexCmp(const cVertex* n0, const cVertex* n1, int level);
int _vertexCmp(const void* n0, const void* n1);

int basRnd(int bound);

double clustCoeff(cVertex * n);

#endif


