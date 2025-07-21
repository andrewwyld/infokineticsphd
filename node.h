//#define __IKAW_DIRECTED

#ifndef __IKAWNODE
#define __IKAWNODE

/*_____________________________________________________________.
                                                      sNodule */
//sNodule encodes an edge for an sNode.
typedef struct _sNodule {
	//doubly-linked list pointers
	struct _sNodule * prev;
	struct _sNodule * next;

	//edge encoded
	struct _sNode * edge;

	//sNode owning sNodule
	struct _sNode * owner;
} sNodule;

/*_____________________________________________________________.
                                                        sNode */
typedef struct _sNode {
	//flag.  mostly for checking loops when:
		//propagating
		//destroying networks.
	int flag;

	//match flag.  used to check if a node had a match
		//but doesn't anymore.
	int match;

	//node degree.  can be inferred from sNodule.
	int degree;

	//net degree.
	int subdeg;

	//net index
	int index;

	//data array
	int * data;

	//top nodule in pile
	sNodule * top;

	//bottom nodule in pile
	sNodule * bottom;
} sNode;

/*_____________________________________________________________.
                                                         sNet */
typedef struct _sNet {
	//number of nodes
	int size;

	//data array depth for each node 
	int depth;

	//probability as float
	float fp;

	//probability as int (based on rand() values)
	int ip;

	//array of nodes
	sNode * nodes;

	//node pointers for general use
	sNode * node0;
	sNode * node1;

} sNet;


#ifdef __IKAW_DIRECTED
#define join(one, two) join_directed(one, two)
#define disjoin(n) disjoin_directed(n)
#else
#define join(one, two) join_undirected(one, two)
#define disjoin(n) disjoin_undirected(n)
#endif

int excise(sNodule* n);

int extend(sNode* n);

int join_directed(sNode* one, sNode* two);
int join_undirected(sNode* one, sNode* two);

int disjoin_directed(sNode* n);
int disjoin_undirected(sNode* n);
int disjoin_all(sNode* n);

int nodeCmp(const sNode* n0, const sNode* n1, int level);
int _nodeCmp(const void* n0, const void* n1);

int flagsub(sNode* n);
int unflagsub(sNode* n);

int propagdeg(sNode* n, int subdeg);
int propagate(sNode* n, int level, int value);

sNode* sNodeArray(int size, int depth);
sNet* sNetCreate(int size, int depth, float p);
int sNetDestroy(sNet* n);

#endif


